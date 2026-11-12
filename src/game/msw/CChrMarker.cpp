// The markers shown over the characters: msw::CChrMarker (the marker over
// every character, with the "1P" to "4P" label of a human player), the arrow
// model msw::CArrowSignalBase and the one-on-one marker
// msw::CChr1on1MarkerBase. The game modules derive their own from all three.
//
// One file in the original (0x800EFFDC..0x800F29D8): the three classes share
// one .sdata2 pool (0x805C2748..), the marker resource file, the model
// colour material names and the team colour table.
#include "game/msw/CChrMarker.hpp"
#include "game/msw/CArrowSignalBase.hpp"
#include "game/msw/CChr1on1MarkerBase.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

namespace {

    enum {
        // The marker models' resource file in the load data (CLoadData::GetRes).
        RES_MARKER = 0x23,
        LANGUAGE_NUM = 6,
        MARKER_TYPE_NUM = 10,
        PLAYER_NUM = 4,
        TEAM_COLOR_NUM = 4
    };

    // The states of both markers (names guessed).
    enum {
        STATE_SLEEP,     // hidden
        STATE_APPEAR,    // coming up
        STATE_MAIN,      // shown, pulsing
        STATE_DISAPPEAR, // going away
        STATE_NUM
    };

    // The colour materials of the marker models.
    const char *s_pColorMat0 = "marker_color00";
    const char *s_pColorMat1 = "marker_color01";

    // The marker model of each marker type (CChrMarker::m_nType), per
    // language of the game (the star marker carries text).
    const char *s_apMarkerMdl[LANGUAGE_NUM][MARKER_TYPE_NUM] = {
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "JpJa_mc_star_marker00", "JpJa_mc_star_marker00" },
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "UsEn_mc_star_marker00", "UsEn_mc_star_marker00" },
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "EuFr_mc_star_marker00", "EuFr_mc_star_marker00" },
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "EuGe_mc_star_marker00", "EuGe_mc_star_marker00" },
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "EuIt_mc_star_marker00", "EuIt_mc_star_marker00" },
        { "mc_marker00", "mc_marker01", "mc_marker00", "mc_marker01", "mh_marker01_01", "md_nocatch_marker00",
          "mh_tackle_marker00", "mh_tackle_marker01", "EuSp_mc_star_marker00", "EuSp_mc_star_marker00" },
    };

    // The "1P" to "4P" label models, per language.
    const char *s_apPlayerMdl[LANGUAGE_NUM][PLAYER_NUM] = {
        { "JpJa_mc_marker_1p", "JpJa_mc_marker_2p", "JpJa_mc_marker_3p", "JpJa_mc_marker_4p" },
        { "UsEn_mc_marker_1p", "UsEn_mc_marker_2p", "UsEn_mc_marker_3p", "UsEn_mc_marker_4p" },
        { "EuFr_mc_marker_1p", "EuFr_mc_marker_2p", "EuFr_mc_marker_3p", "EuFr_mc_marker_4p" },
        { "EuGe_mc_marker_1p", "EuGe_mc_marker_2p", "EuGe_mc_marker_3p", "EuGe_mc_marker_4p" },
        { "EuIt_mc_marker_1p", "EuIt_mc_marker_2p", "EuIt_mc_marker_3p", "EuIt_mc_marker_4p" },
        { "EuSp_mc_marker_1p", "EuSp_mc_marker_2p", "EuSp_mc_marker_3p", "EuSp_mc_marker_4p" },
    };

    const char *s_pRsrcName = "Marker.brres";
    // The arrow model of CArrowSignalBase.
    const char *s_pArrowMdl = "mh_target_goal01_01";

    // CChr1on1MarkerBase's model and colour materials (the original keeps
    // its own constant copies of the material names).
    const char *const s_p1on1Mdl = "mv_target_space01_01";
    const char *const s_p1on1ColorMat0 = "marker_color00";
    const char *const s_p1on1ColorMat1 = "marker_color01";

    // The marker colour of each team (blue, red, green, yellow), built by
    // the file's static initialiser.
    bcs::BCS_Color s_aTeamColor[TEAM_COLOR_NUM] = {
        bcs::BCS_Color(0x0088FFB7),
        bcs::BCS_Color(0xCC0000B7),
        bcs::BCS_Color(0x22BB00B7),
        bcs::BCS_Color(0xFFB400B7),
    };

    // (names guessed) How fast the shown markers pulse, in radians a frame;
    // the second value is not read by anything left in the file. Both are
    // set by the static initialiser.
    const float s_fWaveSpeed = bcs::DEG_TO_RAD(10.0f);
    const float s_fWaveSpeed2 = bcs::DEG_TO_RAD(20.0f);

    const float HALF_PI = 1.5707964f;
    const float TWO_PI = 6.2831855f;

    // (guessed helpers, as in BasketF's game/basket/CArrowSignalBasket.cpp)
    inline float Clamp01(float x)
    {
        return x > 1.0f ? 1.0f : (x < 0.0f ? 0.0f : x);
    }

    inline float NormalizeRad(float rad)
    {
        while (rad > TWO_PI) {
            rad -= TWO_PI;
        }
        while (rad < 0.0f) {
            rad += TWO_PI;
        }
        return rad;
    }
}

namespace msw {

    // ---------------------------------------------------------------------
    // CChrMarker

    nw4r::g3d::ResFile CChrMarker::GetRsrcPack()
    {
        nw4r::g3d::ResFile archive = CLoadData::GetRes(RES_MARKER);
        return nw4r::g3d::ResFile::FromAligned(archive.GetExternalData(s_pRsrcName));
    }

    bcs::BCS_Color CChrMarker::GetTeamColor(long nTeam)
    {
        return s_aTeamColor[nTeam];
    }

    CChrMarker::CChrMarker(bcs::CTask *pParent, long nChr, long nType, long nArg)
        : bcs::CTask(pParent, 0x19),
          m_nFlag(0),
          m_pMdl(0),
          m_pPlayerMdl(0),
          m_nChr(nChr),
          m_nType(nType),
          m_Unk90(nArg),
          m_fDispRate(0.0f),
          m_fWave(HALF_PI)
    {
        InitStep();
        Initialize();
    }

    CChrMarker::~CChrMarker()
    {
        Finalize();
    }

    void CChrMarker::InitStep()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_SLEEP, &CChrMarker::CtorStateSleep_, &CChrMarker::LoopStateSleep_,
                 &CChrMarker::DtorStateSleep_);
        SetState(STATE_APPEAR, &CChrMarker::CtorStateAppear_, &CChrMarker::LoopStateAppear_,
                 &CChrMarker::DtorStateAppear_);
        SetState(STATE_MAIN, &CChrMarker::CtorStateMain_, &CChrMarker::LoopStateMain_,
                 &CChrMarker::DtorStateMain_);
        SetState(STATE_DISAPPEAR, &CChrMarker::CtorStateDisappear_, &CChrMarker::LoopStateDisappear_,
                 &CChrMarker::DtorStateDisappear_);
    }

    void CChrMarker::Initialize()
    {
        nw4r::g3d::ResFile file = GetRsrcPack();
        CChrCore *pChr = CChrMan::GetChrCore(m_nChr);

        if (m_Unk90 < 0) {
            if (CGame::g_GameWork.GetGameType() != CGame::GAME_TYPE_PARTY) {
                m_Unk90 = CGame::GetTeamColorIdx(pChr->GetTeamNo());
            } else {
                m_Unk90 = pChr->GetCtrlPlayerNo();
            }
        }

        long nLang = 0;
        long nPlayer = pChr->GetCtrlPlayerNo();
        switch (bcs::CMainApp::GetLanguage()) {
            default:
                nLang = 1;
                break;
            case 0:
                break;
            case 2:
                nLang = 3;
                break;
            case 3:
                nLang = 2;
                break;
            case 4:
                nLang = 5;
                break;
            case 5:
                nLang = 4;
                break;
        }

        nw4r::g3d::ResMdl mdl = file.GetResMdl(s_apMarkerMdl[nLang][m_nType]);
        m_pMdl = new bcs::CMdlObj();
        m_pMdl->Create(mdl, 0, 0);
        m_pMdl->AttachToObject(pChr->GetMdl(), pChr->GetVNode(3), bcs::CScnObj::ATTACHMODE_3);
        m_pMdl->m_vTrans.y = 0.0f;
        m_pMdl->MatSetZModeAll(1, GX_ALWAYS, 0);
        if (m_nType == 0 || m_nType == 2 || m_nType == 8) {
            m_pMdl->SetDrawPriority(5, 5);
        } else {
            m_pMdl->SetDrawPriority(4, 4);
        }
        m_pMdl->SetMatTevColor(s_pColorMat0, GX_TEVREG0, GetColor_(m_Unk90));
        if (m_nType >= 8 && m_nType <= 9) {
            m_pMdl->SetMatTevColorA(s_pColorMat1, GX_TEVREG0, 0xFF);
        }
        bcs::CRenderMan::PushBack(m_pMdl, 5);

        // The player label over a human player's character.
        if (m_nType == 0 || m_nType == 2) {
            if (m_pMdl != 0 && (unsigned long)nPlayer <= 3) {
                mdl = file.GetResMdl(s_apPlayerMdl[nLang][nPlayer]);
                m_pPlayerMdl = new (0x100) bcs::CMdlObj();
                m_pPlayerMdl->Create(mdl, 0, 0);
                m_pPlayerMdl->AttachToObject(m_pMdl, 0, bcs::CScnObj::ATTACHMODE_1);
                m_pPlayerMdl->MatSetZModeAll(1, GX_ALWAYS, 0);
                m_pPlayerMdl->SetDrawPriority(6, 6);
                m_pPlayerMdl->SetMatTevColor(s_pColorMat0, GX_TEVREG0, GetColor_(nPlayer));
                bcs::CRenderMan::PushBack(m_pPlayerMdl, 5);
            }
        }

        if ((CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_BASKET ||
             CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_HOCKEY) &&
            CGame::g_GameWork.m_nGameType == CGame::GAME_TYPE_NORMAL) {
            if ((m_nType >= 2 && m_nType <= 3) || m_nType == 0) {
                m_nFlag |= 1;
            }
        }

        ChangeState(STATE_SLEEP);
    }

    void CChrMarker::EnableViewCheck(int bEnable)
    {
        if (bEnable) {
            m_nFlag |= 2;
            return;
        }
        if (m_nFlag & 2) {
            if (m_pMdl != 0) {
                m_pMdl->SetViewFlag(0);
            }
            if (m_pPlayerMdl != 0) {
                m_pPlayerMdl->SetViewFlag(0);
            }
            m_nFlag &= ~2;
        }
    }

    bcs::BCS_Color CChrMarker::GetColor_(long nIdx)
    {
        return s_aTeamColor[nIdx];
    }

    void CChrMarker::Finalize()
    {
        if (m_pPlayerMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pPlayerMdl);
        }
        m_pPlayerMdl = 0;
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
    }

    void CChrMarker::SetFogIdx(long nIdx)
    {
        if (m_pMdl != 0) {
            m_pMdl->SetFogIdx(nIdx);
        }
        if (m_pPlayerMdl != 0) {
            m_pPlayerMdl->SetFogIdx(nIdx);
        }
    }

    void CChrMarker::Main()
    {
        if (m_pMdl != 0 && !(CGame::g_uPauseBit & 4)) {
            ExecState();
            MainFunc();
            ArrowSignalCheck();

            // On a split screen each model is shown in one half only (view
            // flags 5 and 10; meaning of the values not known).
            if ((m_nFlag & 2) && (m_pMdl != 0 || m_pPlayerMdl != 0)) {
                unsigned short nView = 0;
                if (bcs::CWorldEnv::GetViewSplitNum() > 1) {
                    if (CGame::g_GameWork.m_nUnk00 != 0) {
                        nView = 5;
                        if (m_nChr & 1) {
                            nView = 10;
                        }
                    } else {
                        nView = 10;
                        if (m_nChr & 1) {
                            nView = 5;
                        }
                    }
                }
                if (m_pMdl != 0) {
                    m_pMdl->SetViewFlag(nView);
                }
                if (m_pPlayerMdl != 0) {
                    m_pPlayerMdl->SetViewFlag(nView);
                }
            }

            if (m_pMdl != 0) {
                int bHide = 0;
                if (!CChrMan::IsChrMarkerDisp()) {
                    bHide = 1;
                }
                if (m_nChr >= 0) {
                    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
                    if (pChr != 0 && pChr->IsEnable() && (pChr->GetStatusFlag() & 0x800000)) {
                        bHide = 1;
                    }
                }
                if (bHide) {
                    m_pMdl->SetAlphaRate(0);
                }
            }

            if (m_pPlayerMdl != 0) {
                if (m_pMdl != 0) {
                    m_pPlayerMdl->SetAlphaRate(m_pMdl->GetAlphaRate());
                } else {
                    m_pPlayerMdl->SetAlphaRate(0);
                }
            }
        }
    }

    void CChrMarker::ArrowSignalCheck() {}

    // Up for a character in play: always for marker types 2 and 3, else
    // with bit 8 of the character's status flags set (type 0) or clear
    // (the other types).
    int CChrMarker::AppearCheck()
    {
        if (m_nChr < 0) {
            return 0;
        }
        CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
        if (pChr != 0) {
            if (m_nType >= 2 && m_nType <= 3) {
                return 1;
            }
            if (m_nType == 0) {
                if (pChr->GetStatusFlag() & 0x100) {
                    return 1;
                }
            } else if (!(pChr->GetStatusFlag() & 0x100)) {
                return 1;
            }
        }
        return 0;
    }

    int CChrMarker::DisappearCheck()
    {
        if (m_nChr < 0) {
            return 1;
        }
        CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
        if (pChr != 0 && !(m_nType >= 2 && m_nType <= 3)) {
            if (m_nType == 0) {
                if (!(pChr->GetStatusFlag() & 0x100)) {
                    return 1;
                }
            } else if (pChr->GetStatusFlag() & 0x100) {
                return 1;
            }
        }
        return 0;
    }

    void CChrMarker::CtorStateSleep()
    {
        if (m_pMdl != 0) {
            m_pMdl->SetAlphaRate(0);
        }
        m_fDispRate = 0.0f;
        m_fWave = HALF_PI;
    }

    void CChrMarker::LoopStateSleep()
    {
        if (AppearCheck()) {
            ChangeState(STATE_APPEAR);
        }
    }

    void CChrMarker::DtorStateSleep() {}

    void CChrMarker::CtorStateAppear() {}

    // Comes up over 12 frames, growing and fading in.
    void CChrMarker::LoopStateAppear()
    {
        m_fDispRate += (1.0f / 12.0f) * GetFrameRateF();
        m_fDispRate = Clamp01(m_fDispRate);
        float fRate = nw4r::math::SinRad(HALF_PI * m_fDispRate);
        m_pMdl->SetAlphaRateF(fRate);
        float fScale = fRate * GetDispScale();
        m_pMdl->m_vScale.Set(fScale, 1.0f, fScale);
        if (m_pPlayerMdl != 0) {
            m_pPlayerMdl->m_vScale.Set(fScale, 1.0f, fScale);
        }
        if (m_fDispRate >= 1.0f) {
            ChangeState(STATE_MAIN);
        }
    }

    void CChrMarker::DtorStateAppear() {}

    void CChrMarker::CtorStateMain() {}

    // Shown: the marker pulses between 0.8 and 1.0 of its size.
    void CChrMarker::LoopStateMain()
    {
        if (DisappearCheck()) {
            ChangeState(STATE_DISAPPEAR);
        }
        m_fWave += GetFrameRateF() * s_fWaveSpeed;
        m_fWave = NormalizeRad(m_fWave);
        m_pMdl->SetAlphaRateF(1.0f);
        float fWave = 0.1f * nw4r::math::SinRad(m_fWave) + 0.9f;
        float fScale = fWave * GetDispScale();
        m_pMdl->m_vScale.Set(fScale, 1.0f, fScale);
        if (m_pPlayerMdl != 0) {
            m_pPlayerMdl->m_vScale.Set(1.0f, 1.0f, 1.0f);
        }
    }

    void CChrMarker::DtorStateMain() {}

    void CChrMarker::CtorStateDisappear() {}

    // Goes away over 12 frames, the reverse of LoopStateAppear.
    void CChrMarker::LoopStateDisappear()
    {
        m_fDispRate -= (1.0f / 12.0f) * GetFrameRateF();
        m_fDispRate = Clamp01(m_fDispRate);
        float fRate = nw4r::math::SinRad(HALF_PI * m_fDispRate);
        m_pMdl->SetAlphaRateF(fRate);
        float fScale = fRate * GetDispScale();
        m_pMdl->m_vScale.Set(fScale, 1.0f, fScale);
        if (m_pPlayerMdl != 0) {
            m_pPlayerMdl->m_vScale.Set(fScale, 1.0f, fScale);
        }
        if (m_fDispRate <= 0.0f) {
            ChangeState(STATE_SLEEP);
        }
    }

    void CChrMarker::DtorStateDisappear() {}

    float CChrMarker::GetDispScale() const
    {
        return 1.0f;
    }

    // ---------------------------------------------------------------------
    // CArrowSignalBase

    CArrowSignalBase::CArrowSignalBase(bcs::CTask *pParent, long nChrIdx, long nTeam)
        : bcs::CTask(pParent, -1),
          m_pMdl(0),
          m_nChrIdx(nChrIdx),
          m_nTeam(nTeam),
          m_nState(0),
          m_nCount(0),
          m_Unk48(0.0f),
          m_Unk4C(1.0f),
          m_Unk50(0.0f),
          m_Unk54(1.0f),
          m_Unk58(0.0f),
          m_Unk5C(0.0f)
    {
    }

    CArrowSignalBase::~CArrowSignalBase()
    {
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
    }

    void CArrowSignalBase::CreateMdl_(float fScale, float fRotY)
    {
        nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();
        // The original looks the character up and does not use him.
        CChrCore *pChr = CChrMan::GetChrCore(m_nChrIdx);
        (void)pChr;
        nw4r::g3d::ResMdl mdl = file.GetResMdl(s_pArrowMdl);
        m_pMdl = new (0x100) bcs::CMdlObj();
        m_pMdl->Create(mdl, 0, 0);
        m_pMdl->OnFlag(0x4000);
        m_pMdl->m_vTrans.y = 0.0f;
        m_pMdl->SetRotY(fRotY);
        m_pMdl->m_vScale.Set(fScale, 1.0f, fScale);
        m_pMdl->MatSetZModeAll(1, GX_ALWAYS, 0);
        m_pMdl->SetDrawPriority(4, 4);
        m_pMdl->SetMatTevColor(s_pColorMat0, GX_TEVREG0, CChrMarker::GetTeamColor(CGame::GetTeamColorIdx(m_nTeam)));
        bcs::CRenderMan::PushBack(m_pMdl, 5);
    }

    void CArrowSignalBase::Main() {}

    // ---------------------------------------------------------------------
    // CChr1on1MarkerBase

    CChr1on1MarkerBase::CChr1on1MarkerBase(bcs::CTask *pParent, long nChr)
        : bcs::CUnivTaskEx(pParent, -1),
          m_fPosX(0.0f),
          m_fPosY(0.0f),
          m_fPosZ(0.0f),
          m_fRotY(0.0f),
          m_pMdl(0),
          m_nChr(nChr),
          m_fDispRate(0.0f),
          m_fWave(HALF_PI)
    {
        InitState();
    }

    CChr1on1MarkerBase::~CChr1on1MarkerBase()
    {
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
    }

    // Defined after the destructor and inlined into the constructor: the
    // original emits CStateTemplate's constructor and CFsm::ChangeState
    // after the destructor and keeps a second, unread copy of the state
    // table's member pointers (the out-of-line copy, stripped by the link).
    void CChr1on1MarkerBase::InitState()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_SLEEP, &CChr1on1MarkerBase::CtorStateSleep, &CChr1on1MarkerBase::LoopStateSleep,
                 &CChr1on1MarkerBase::DtorStateSleep);
        SetState(STATE_APPEAR, &CChr1on1MarkerBase::CtorStateAppear, &CChr1on1MarkerBase::LoopStateAppear,
                 &CChr1on1MarkerBase::DtorStateAppear);
        SetState(STATE_MAIN, &CChr1on1MarkerBase::CtorStateMain, &CChr1on1MarkerBase::LoopStateMain,
                 &CChr1on1MarkerBase::DtorStateMain);
        SetState(STATE_DISAPPEAR, &CChr1on1MarkerBase::CtorStateDisappear, &CChr1on1MarkerBase::LoopStateDisappear,
                 &CChr1on1MarkerBase::DtorStateDisappear);
        ChangeState(STATE_SLEEP);
    }

    void CChr1on1MarkerBase::Create()
    {
        nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();
        nw4r::g3d::ResMdl mdl = file.GetResMdl(s_p1on1Mdl);
        // The original looks the character up and does not use him.
        CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
        (void)pChr;
        m_pMdl = new (0) bcs::CMdlObj();
        m_pMdl->Create(mdl, 0, 0);
        m_pMdl->OnFlag(0x4000);
        m_pMdl->m_vTrans.Set(0.0f, 0.0f, 0.0f);
        m_pMdl->m_vTrans.z = 1.0f;
        m_pMdl->MatSetZModeAll(1, GX_ALWAYS, 0);
        m_pMdl->SetDrawPriority(3, 3);
        m_pMdl->MatSetBlendModeAll(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_COPY);
        m_pMdl->SetAlphaRate(0);
        UpdateColor_();
    }

    int CChr1on1MarkerBase::CheckAppear()
    {
        return 3;
    }

    float CChr1on1MarkerBase::GetAppearFrameNum() const
    {
        return 16.0f;
    }

    float CChr1on1MarkerBase::GetDisappearFrameNum() const
    {
        return 12.0f;
    }

    float CChr1on1MarkerBase::GetDispScale() const
    {
        return 5.7f;
    }

    void CChr1on1MarkerBase::Main()
    {
        if (m_pMdl != 0) {
            ExecState();
            bcs::FVEC3 &rTrans = m_pMdl->m_vTrans;
            rTrans.x = m_fPosX;
            rTrans.y = m_fPosY;
            rTrans.z = m_fPosZ;
            m_pMdl->SetRotY(m_fRotY);
        }
    }

    void CChr1on1MarkerBase::CtorStateSleep()
    {
        if (m_pMdl != 0) {
            m_pMdl->SetAlphaRate(0);
        }
        m_fDispRate = 0.0f;
        m_fWave = HALF_PI;
        if (m_pMdl != 0) {
            m_pMdl->DetachFromParent();
        }
    }

    void CChr1on1MarkerBase::LoopStateSleep()
    {
        if (CheckAppear() == 2) {
            ChangeState(STATE_APPEAR);
        }
    }

    void CChr1on1MarkerBase::DtorStateSleep()
    {
        if (m_pMdl != 0) {
            m_pMdl->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdl, 5);
        }
    }

    void CChr1on1MarkerBase::CtorStateAppear() {}

    void CChr1on1MarkerBase::LoopStateAppear()
    {
        switch (CheckAppear()) {
            case 1:
                ChangeState(STATE_SLEEP);
                break;
            case 3:
                ChangeState(STATE_DISAPPEAR);
                break;
            default: {
                float fStep = GetFrameRateF();
                float fFrames = GetAppearFrameNum();
                fFrames = 1.0f < fFrames ? fFrames : 1.0f;
                float fScale = GetDispScale();
                fScale = 0.0f < fScale ? fScale : 0.0f;
                m_fDispRate += fStep * (1.0f / fFrames);
                m_fDispRate = Clamp01(m_fDispRate);
                float fRate = nw4r::math::SinRad(HALF_PI * m_fDispRate);
                m_pMdl->SetAlphaRateF(fRate);
                m_pMdl->m_vScale.Set(fRate * fScale, 1.0f, fRate * fScale);
                if (m_fDispRate >= 1.0f) {
                    ChangeState(STATE_MAIN);
                }
                break;
            }
        }
    }

    void CChr1on1MarkerBase::DtorStateAppear() {}

    void CChr1on1MarkerBase::CtorStateMain() {}

    void CChr1on1MarkerBase::LoopStateMain()
    {
        switch (CheckAppear()) {
            case 1:
                ChangeState(STATE_SLEEP);
                break;
            case 3:
                ChangeState(STATE_DISAPPEAR);
                break;
            default: {
                m_fWave += GetFrameRateF() * s_fWaveSpeed;
                m_fWave = NormalizeRad(m_fWave);
                float fWave = 0.1f * nw4r::math::SinRad(m_fWave) + 0.9f;
                float fScale = GetDispScale();
                fScale = 0.0f < fScale ? fScale : 0.0f;
                fScale = fWave * fScale;
                m_pMdl->m_vScale.Set(fScale, 1.0f, fScale);
                break;
            }
        }
    }

    void CChr1on1MarkerBase::DtorStateMain() {}

    void CChr1on1MarkerBase::CtorStateDisappear() {}

    void CChr1on1MarkerBase::LoopStateDisappear()
    {
        if (CheckAppear() == 1) {
            ChangeState(STATE_SLEEP);
            return;
        }
        float fStep = GetFrameRateF();
        float fFrames = GetDisappearFrameNum();
        fFrames = 1.0f < fFrames ? fFrames : 1.0f;
        float fScale = GetDispScale();
        fScale = 0.0f < fScale ? fScale : 0.0f;
        m_fDispRate -= fStep * (1.0f / fFrames);
        m_fDispRate = Clamp01(m_fDispRate);
        float fRate = nw4r::math::SinRad(HALF_PI * m_fDispRate);
        m_pMdl->SetAlphaRateF(fRate);
        m_pMdl->m_vScale.Set(fRate * fScale, 1.0f, fRate * fScale);
        if (m_fDispRate <= 0.0f) {
            ChangeState(STATE_SLEEP);
        }
    }

    void CChr1on1MarkerBase::DtorStateDisappear() {}

    void CChr1on1MarkerBase::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long)
    {
        rColor0.r = 0xFF;
        rColor0.g = 0xBE;
        rColor0.b = 0x8C;
        rColor0.a = 0x5A;
        rColor1.r = 0x00;
        rColor1.g = 0x00;
        rColor1.b = 0x00;
        rColor1.a = 0x00;
    }

    void CChr1on1MarkerBase::UpdateColor_()
    {
        if (m_pMdl != 0) {
            bcs::BCS_Color color0;
            bcs::BCS_Color color1;
            GetColor_(color0, color1, CGame::g_GameWork.m_nStage);
            m_pMdl->SetMatTevColor(s_p1on1ColorMat0, GX_TEVREG0, color0);
            m_pMdl->SetMatTevColor(s_p1on1ColorMat1, GX_TEVREG0, color1);
        }
    }

    void CChr1on1MarkerBase::Disp()
    {
        UpdateColor_();
    }
}
