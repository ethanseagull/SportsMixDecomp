// msw::CCockpitHockeySmashVS: the board of the hockey game's extra match
// ("crash vs"): the won-set marks, the clock, the players' face icons and
// the "shake the remote" icons.
//
// Written after game/hockey/CCockpitHockeyCoder.cpp and
// game/volley/CCockpitVolleyCoder.cpp (the set counter).
#include "game/msw/CCockpitHockeySmashVS.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CCharaIcon.hpp"
#include "game/msw/cockpit/CSetCountUp.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 2;

    // The layout and its animations.
    struct LayoutRes {
        const char *pLayout;
        const char *pUnused;
        const char *pAnim[14];
        const char *pPane[71];
    } s_LayoutRes = {
        "HO_crash_vs.brlyt",
        0,
        {
            "HO_crash_vs_Set_00.brlan",
            "HO_crash_vs_Set_01.brlan",
            "HO_crash_vs_Set_02.brlan",
            "HO_crash_vs_Set_03.brlan",
            "HO_crash_vs_Time_00.brlan",
            "HO_crash_vs_Time_01.brlan",
            "HO_crash_vs_Vis_set_ico_00.brlan",
            "HO_crash_vs_Vis_set_ico_01.brlan",
            "HO_crash_vs_Vis_set_ico_02.brlan",
            "HO_crash_vs_Time_vis_00.brlan",
            "HO_crash_vs_Time_vis_01.brlan",
            "HO_crash_vs_Time_vis_02.brlan",
            "HO_crash_vs_TC_00.brlan",
            "HO_crash_vs_con_00.brlan"
        },
        {
        "set_countLNull_A",
        "set_cntLNull_A01",
        "set_cntLNull_A02",
        "set_cntLNull_A03",
        "set_countRNull_B",
        "set_cntRNull_B01",
        "set_cntRNull_B02",
        "set_cntRNull_B03",
        "set_L",
        "set_R",
        "set_line",
        "LR_Null",
        "L_A01",
        "L_A02",
        "L_A03",
        "R_B01",
        "R_B02",
        "R_B03",
        "L2_fe1",
        "L2_fe2",
        "R2_fe1",
        "R2_fe2",
        "L3_fe1",
        "L3_fe2",
        "L3_fe3",
        "R3_fe1",
        "R3_fe2",
        "R3_fe3",
        "1P",
        "2P",
        "3P",
        "com",
        "time",
        "text_time",
        "text_time02",
        "Time_co",
        "time_t",
        "time_no",
        "t_waku",
        "L_Ca_fe1",
        "L_Ca_fe2",
        "L_Ca_fe3",
        "L2_Ca_fe1",
        "L2_Ca_fe2",
        "R_Ca_fe1",
        "R_Ca_fe2",
        "R_Ca_fe3",
        "R2_Ca_fe1",
        "R2_Ca_fe2",
        "cs_ico",
        "ar_Null",
        "cont_B",
        "cont_R",
        "cont_G",
        "cont_Y",
        "con_B",
        "con_R",
        "con_G",
        "con_Y",
        "L_FcPic0",
        "L_FcPic1",
        "L_FcPic2",
        "R_FcPic0",
        "R_FcPic1",
        "R_FcPic2",
        "L_StPic1",
        "L_StPic2",
        "L_StPic3",
        "R_StPic1",
        "R_StPic2",
        "R_StPic3"
        }
    };

    // Animations (indexes into s_LayoutRes.pAnim). The board slides with the
    // first four.
    enum {
        ANIM_SLIDE_IN_WAIT = 0,
        ANIM_SLIDE_OUT = 1,
        ANIM_SLIDE_OUT_WAIT = 2,
        ANIM_SLIDE_IN = 3,
        ANIM_TIME = 4,
        ANIM_TIME_ALL = 5,
        ANIM_SET_ICO_OFF = 6,
        ANIM_SET_ICO_ON = 7,
        ANIM_SET_ICO_UP = 8,
        ANIM_TIME_NO = 9,
        ANIM_TIME_NO_HURRY = 10,
        ANIM_COUNTDOWN = 12,
        ANIM_RIMOCON = 13
    };


    // Panes (indexes into s_LayoutRes.pPane) used by name in the code.
    enum {
        PANE_set_line = 10,
        PANE_LR_Null = 11,
        PANE_time = 32,
        PANE_text_time = 33,
        PANE_text_time02 = 34,
        PANE_Time_co = 35,
        PANE_time_t = 36,
        PANE_time_no = 37,
        PANE_t_waku = 38,
        PANE_cs_ico = 49,
        PANE_ar_Null = 50
    };

    // The countdown effect.
    const long EFFECT_COUNTDOWN = 0x4D0;

    // Teams on the board, and the players of a team.
    const long TEAM_NUM = 2;
    const long PLAYER_NUM = 3;

    // The won-set marks of a three-set match, by team.
    const unsigned char sSetCountPane3[TEAM_NUM][3] = {
        { 1, 2, 3 },
        { 5, 6, 7 }
    };
    // The frames of the set marks, by team, for a three-set match.
    const unsigned char sSetCountNullPane[TEAM_NUM][1] = {
        { 0 },
        { 4 }
    };
    // The set counters.
    const unsigned char sSetPane[TEAM_NUM] = { 8, 9 };
    // Where the face icons go, by players a team (1 to 3), team and player.
    const unsigned char sFaceParentPane[PLAYER_NUM][TEAM_NUM][PLAYER_NUM] = {
        {
            { 22, 0, 0 },
            { 25, 0, 0 }
        },
        {
            { 18, 19, 0 },
            { 20, 21, 0 }
        },
        {
            { 22, 23, 24 },
            { 25, 26, 27 }
        }
    };
    const unsigned char sFacePane[TEAM_NUM][PLAYER_NUM] = {
        { 59, 60, 61 },
        { 62, 63, 64 }
    };
    // Where a star player's star goes, as sFaceParentPane.
    const unsigned char sStarParentPane[PLAYER_NUM][TEAM_NUM][PLAYER_NUM] = {
        {
            { 39, 0, 0 },
            { 44, 0, 0 }
        },
        {
            { 42, 43, 0 },
            { 47, 48, 0 }
        },
        {
            { 39, 40, 41 },
            { 44, 45, 46 }
        }
    };
    const unsigned char sStarPane[TEAM_NUM][PLAYER_NUM] = {
        { 65, 66, 67 },
        { 68, 69, 70 }
    };
    // The "1P" to "3P" labels.
    const unsigned char sPlayerPane[PLAYER_NUM] = { 28, 29, 30 };
    // The effect of a won set, by team.
    const long sSetEffect[TEAM_NUM] = { 0x45E, 0x45F };
    // The remote icons by player: the icon, and a second pane not used here.
    const unsigned char sRimoconPane[4][2] = {
        { 51, 55 },
        { 52, 56 },
        { 53, 57 },
        { 54, 58 }
    };

    // The won-set marks by number of sets (from three).
    const unsigned char (*sSetCountPane[])[3] = {
        sSetCountPane3
    };
}

msw::CCockpitHockeySmashVS::CCockpitHockeySmashVS(long nId)
    : CCockpitHockeyLayoutBase(nId)
{
    m_nStep = 0;
    m_nPlayerNum = 0;
    m_nSetNum = 0;
    m_nSetPoint[0] = 0;
    m_nSetPoint[1] = 0;
    m_nUnk64[0] = 0;
    m_nUnk64[1] = 0;
}

msw::CCockpitHockeySmashVS::~CCockpitHockeySmashVS()
{
    Remove();
}

void msw::CCockpitHockeySmashVS::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();
        PlayAnim(ANIM_SLIDE_IN_WAIT, 0);
        m_nStep = 2;
    }
}

void msw::CCockpitHockeySmashVS::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitHockeySmashVS::Set()
{
    LayoutRes *pRes = &s_LayoutRes;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    m_nSetNum = 3;

    CCockpit *pCockpit = GetCockpit();
    long nPlayerNum = menu::GetTeamPlayerNum(CGame::g_GameWork.m_nUnk06);

    m_nPlayerNum = nPlayerNum;

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->BuildLayout(0, pRes->pLayout);
            pPaneCtrl->BuildAnim(0, pRes->pAnim, sizeof(pRes->pAnim) / sizeof(pRes->pAnim[0]));
            SetPaneAccessor(pPaneCtrl, pRes->pPane, sizeof(pRes->pPane) / sizeof(pRes->pPane[0]));
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);
            m_EffectSet.Initialize();

            SetFaceIcon(GetLytObj(), 0, 0, nPlayerNum);
            SetFaceIcon(GetLytObj(), 1, 0, nPlayerNum);

            // The remote icons are placed in screen coordinates from the
            // top left corner.
            nw4r::lyt::Pane *pRoot = m_pPaneAccessor->GetPane(PANE_ar_Null);

            if (pRoot != 0) {
                const nw4r::math::VEC3 pos(-(0.5f * (long)bcs::CMainApp::g_Rmode.fbWidth),
                                           0.5f * (long)bcs::CMainApp::g_Rmode.efbHeight, 0.0f);

                pRoot->SetTranslate(pos);
                pRoot->SetVisible(true);
                pRoot->SetLocationAdjust(false);
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitHockeySmashVS::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitHockeySmashVS::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
        m_EffectSet.SetDispFlag(bVisible);
    }
    return 0;
}

int msw::CCockpitHockeySmashVS::SetupGame()
{
    static const int recursive = 0;
    static const int reset = 1;
    static const long animId = ANIM_SLIDE_IN_WAIT;

    m_pPaneAccessor->PlayAnimation(sSetPane[0], animId, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(sSetPane[1], animId, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_set_line, animId, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_LR_Null, animId, 1.0f, 0.0f, recursive, reset);

    // Only the frame for the match's number of sets shows.
    long nIdx = m_nSetNum - 3;

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < 1; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetCountNullPane[nTeam][i]);

            if (pObj != 0) {
                pObj->GetPane()->SetVisible(i == nIdx);
            }
        }
    }
    return 0;
}

int msw::CCockpitHockeySmashVS::ScoreOut()
{
    PlayAnim(ANIM_SLIDE_OUT, 0);
    return 0;
}

int msw::CCockpitHockeySmashVS::ScoreIn()
{
    PlayAnim(ANIM_SLIDE_IN, 1);
    return 0;
}

int msw::CCockpitHockeySmashVS::ScoreOutWait()
{
    PlayAnim(ANIM_SLIDE_OUT_WAIT, 0);
    return 0;
}

int msw::CCockpitHockeySmashVS::ScoreInWait()
{
    PlayAnim(ANIM_SLIDE_IN_WAIT, 0);
    return 0;
}

long msw::CCockpitHockeySmashVS::PlayAnim(long nAnim, int bSetCount)
{
    long i;

    for (i = 0; i < TEAM_NUM; i++) {
        m_pPaneAccessor->PlayAnimation(sSetPane[i], nAnim, 1.0f, 0.0f, 0, 1);
    }
    m_pPaneAccessor->PlayAnimation(PANE_set_line, nAnim, 1.0f, 0.0f, 0, 1);

    long nSetNum = m_nSetNum;
    long nIdx = nSetNum - 3;

    if (nIdx >= 0) {
        if (bSetCount) {
            // The marks of the sets won light one after the other.
            for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                long nPoint = m_nSetPoint[nTeam];

                for (i = 0; i < nSetNum; i++) {
                    long nPane = sSetCountPane[nIdx][nTeam][i];
                    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                    if (pObj != 0) {
                        pObj->GetPane()->SetVisible(true);
                        pObj->PlayAnimation(ANIM_SET_ICO_OFF, 1.0f, 0.0f, 1, 1, 1);
                    }

                    if (i < nPoint) {
                        unsigned long nKind = nPane + 0x1000;
                        long nEffect = sSetEffect[nTeam];
                        long nFlag = m_bUnk66[nTeam];
                        menu::CDispTaskLink *pPrev = m_DispTaskList.FindNextKindObj(nKind, 0);

                        if (pPrev != 0) {
                            pPrev->m_bEnd = 1;
                        }

                        cockpit::CSetCountUp *pTask = new cockpit::CSetCountUp;

                        pTask->SetKind(nKind);
                        pTask->SetPane(nPane);
                        pTask->m_fDelay = 5.0f + 10.0f * i;
                        pTask->m_nUnk34 = 10;
                        pTask->m_nUnk36 = 3;
                        pTask->m_nUnk38 = nFlag;
                        pTask->m_nEffect = nEffect;
                        pTask->m_nWaitAnim = ANIM_SET_ICO_UP;
                        pTask->m_nAnim = ANIM_SET_ICO_ON;
                        m_DispTaskList.PushBack(pTask);
                    }
                }
            }
        } else {
            for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                long nPoint = m_nSetPoint[nTeam];

                for (i = 0; i < nSetNum; i++) {
                    long nPane = sSetCountPane[nIdx][nTeam][i];
                    long nSetAnim = ANIM_SET_ICO_OFF;

                    if (i < nPoint) {
                        nSetAnim = ANIM_SET_ICO_ON;
                    }

                    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                    if (pObj != 0) {
                        pObj->GetPane()->SetVisible(true);
                        pObj->PlayAnimation(nSetAnim, 1.0f, 0.0f, 1, 1, 1);
                    }
                }
            }
        }
    }

    m_pPaneAccessor->PlayAnimation(PANE_LR_Null, nAnim, 1.0f, 0.0f, 0, 1);
    m_pPaneAccessor->PlayAnimation(PANE_time, nAnim, 1.0f, 0.0f, 0, 1);
    return 0;
}

void msw::CCockpitHockeySmashVS::SetSetCountPoint(long nTeam, long nPoint)
{
    static const int reset = 1;
    static const int recursive = 1;

    if (nTeam >= TEAM_NUM) {
        return;
    }

    long nSetNum = m_nSetNum;

    if (nPoint > nSetNum) {
        nPoint = nSetNum;
    }
    m_nSetPoint[nTeam] = nPoint;

    long nIdx = nSetNum - 3;

    if (nIdx >= 0) {
        for (long i = 0; i < nSetNum; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetCountPane[nIdx][nTeam][i]);

            if (pObj != 0) {
                long nAnim = ANIM_SET_ICO_OFF;

                if (i < nPoint) {
                    nAnim = ANIM_SET_ICO_ON;
                }
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            }
        }
    }
}

void msw::CCockpitHockeySmashVS::SetFaceIcon(long nSide, long nTurn, long nArg)
{
    SetFaceIcon(GetLytObj(), nSide, nTurn, nArg);
}

void msw::CCockpitHockeySmashVS::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nSide, long nTurn, long nArg)
{
    if (pPaneCtrl == 0) {
        return;
    }

    // The characters of each team.
    long chrNo[TEAM_NUM][PLAYER_NUM] = {
        { 0, 2, 4 },
        { 1, 3, 5 }
    };
    static const long tblLen = 3;
    static const long texMapId = 0;
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long i;

    for (i = 0; i < nArg; i++) {
        nw4r::lyt::Pane *pFaceParent = m_pPaneAccessor->GetPane(sFaceParentPane[nArg - 1][nSide][i]);

        if (pFaceParent == 0) {
            continue;
        }

        // The players take turns: nTurn comes first.
        CChrCore *pChr = CChrMan::GetChrCore(chrNo[nSide][(i + nTurn) % nArg]);

        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }

        // The face picture.
        long nFace = sFacePane[nSide][i];
        nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(nFace);

        if (pFace == 0) {
            pFace = menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), pPaneCtrl, pFaceParent, s_LayoutRes.pPane[nFace]);
            bcs::G2D::SetPaneSizeFromParent(pFace);
        } else {
            const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

            if (pTexObj != 0) {
                bcs::G2D::ReplacePanePicture(pFace, pTexObj, texMapId);
            }
        }
        if (pFace != 0) {
            pFace->SetScale(menu::GetCockpitFaceIconScale(pChr->GetChrIdS()));
        }

        // The star of a star player: a copy of cs_ico.
        nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
        nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(sStarParentPane[nArg - 1][nSide][i]);

        if (pStarSrc != 0 && pStarParent != 0) {
            long nStar = sStarPane[nSide][i];
            nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

            if (pStar == 0) {
                pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, s_LayoutRes.pPane[nStar], s_LayoutRes.pLayout, 0);
                bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
                bcs::G2D::SetPaneSizeFromParent(pStar);
                pStar->SetTranslate(zero);
            }

            if (pStar != 0) {
                pStar->SetVisible(pChr->IsStarPlayer());
            }
        }
    }

    // The pad label of the player in turn, when a human plays it.
    CChrCore *pChr = CChrMan::GetChrCore(chrNo[nSide][nTurn]);

    if (pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl()) {
        long nPad = pChr->GetPadNo();

        for (i = 0; i < tblLen; i++) {
            m_pPaneAccessor->SetPaneDisp(sPlayerPane[i], i == nPad);
        }
    }
}

void msw::CCockpitHockeySmashVS::SetTimeValue(float fFrame, int nArg, int nMode)
{
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fFrame, nArg, ANIM_TIME, ANIM_TIME_ALL, PANE_text_time,
                             PANE_text_time02, nMode);

    long nSec = m_TimeCount.GetSec(fFrame);

    if (nSec == m_TimeCount.GetShowSec()) {
        return;
    }

    // The countdown of the last seconds.
    m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, ANIM_COUNTDOWN);
    m_EffectSet.RemoveEffect(0, 1);
    if (nSec >= 1 && nSec <= 9) {
        m_EffectSet.SetEffect(0, 1, EFFECT_COUNTDOWN, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_no);

    if (pObj != 0) {
        if (nSec <= 10 && !pObj->IsAnimPlay(ANIM_TIME_NO_HURRY)) {
            pObj->PlayAnimation(ANIM_TIME_NO_HURRY, 1.2f, 0.0f, 0, 1, 1);
        } else if (nSec > 10 && !pObj->IsAnimPlay(ANIM_TIME_NO)) {
            pObj->PlayAnimation(ANIM_TIME_NO, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    m_TimeCount.SetShowSec(nSec);
}

long msw::CCockpitHockeySmashVS::SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bArg)
{
    static const long animId = ANIM_RIMOCON;
    static const int reset = 1;
    static const int recursive = 1;

    if (nPlayer < 0 || nPlayer >= sizeof(sRimoconPane) / sizeof(sRimoconPane[0])) {
        return -1;
    }

    long nPane = sRimoconPane[nPlayer][0];
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

    if (pObj == 0) {
        return -1;
    }

    if (bArg == 1) {
        pObj->StopAllAnimation(1);
        pObj->GetPane()->SetVisible(false);
    } else {
        unsigned long nKind = nPlayer + 0x3000;
        menu::CDispTaskLink *pTask = m_DispTaskList.FindNextKindObj(nKind, 0);

        if (pTask == 0) {
            cockpit::CCharaIcon *pIcon = new cockpit::CCharaIcon;

            pIcon->SetKind(nKind);
            pIcon->SetPane(nPane);
            pIcon->SetPos(rPos);
            pIcon->SetUnk30(nArg);
            m_DispTaskList.PushBack(pIcon);

            nw4r::lyt::Pane *pRoot = m_pPaneAccessor->GetPane(PANE_ar_Null);

            if (pRoot != 0) {
                bcs::G2D::SetPaneParent(pObj->GetPane(), pRoot, 1, 0);
            }
        } else {
            static_cast<cockpit::CCharaIcon *>(pTask)->SetUnk30(nArg);
        }
        pObj->PlayAnimation(animId, 1.0f, 0.0f, recursive, reset, 1);
        pObj->GetPane()->SetVisible(true);
    }
    return 0;
}
