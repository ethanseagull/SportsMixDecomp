// The characters of the top menu: msw::CTopMenuChr holds one
// msw::CTopMenuChrObj per sport (each with the ball of its sport, an
// msw::CTopMenuBall) and an msw::CBBoxObj per character that gives its box
// on the screen. The file also holds the classes that render a Mii into a
// texture (msw::CMiiRenderBase and its three derived classes).
#include "game/msw/CTopMenuChrObj.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CScnObjInline.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBBoxObj.hpp"
#include "game/msw/CBallInline.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGenPrerendGrp.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CMiiRenderBase.hpp"
#include "game/msw/CMiiSelectRender.hpp"
#include "game/msw/CPlayerInfoRender.hpp"
#include "game/msw/CPlayerMiiRender.hpp"
#include "game/msw/CTopMenuChr.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

#include <math.h>

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The screen box of each character of msw::CTopMenuChr, around its
    // model: the height of the box's centre and its extent in X and Y.
    struct BBoxInfo {
        float fY;
        float fMinX;
        float fMaxX;
        float fMinY;
        float fMaxY;
    };

    BBoxInfo s_BBoxInfo[msw::CTopMenuChr::CHR_NUM] = {
        { 0.8f, -0.8f, 0.6f, -1.0f, 0.8f },
        { 0.8f, -0.8f, 0.6f, -1.0f, 1.0f },
        { 0.8f, -0.8f, 0.6f, -1.0f, 1.0f },
        { 0.8f, -1.0f, 1.0f, -1.0f, 1.4f },
        { 0.0f, -0.6f, 0.6f, 0.0f, 1.65f },
    };

    // The sounds of each kind; -1 for none.
    enum {
        SE_ACTIVE,     // the character starts to play
        SE_BOUND,      // the ball bounces
        SE_TRG_12,     // motion trigger 0x12
        SE_TRG_2,      // motion trigger 2, at the character
        SE_TRG_2_BALL, // motion trigger 2, at the ball
        SE_TRG_2_FREE, // motion trigger 2, without a position
        SE_NUM
    };

    const long s_SeTable[msw::CTopMenuChr::CHR_NUM][SE_NUM] = {
        { 0xBE0, 0xBE1, 0xBE2, 0xBE3, -1, 0xBF0 },
        { 0xBE4, -1, 0xBE5, 0xBE6, 0xBE7, 0xBF1 },
        { 0xBE8, -1, 0xBE9, 0xBEA, 0xBEB, 0xBF2 },
        { 0xBEC, -1, 0xBED, 0xBEE, 0xBEF, 0xBF3 },
        { -1, -1, -1, -1, -1, -1 },
    };

    // Five numbers that nothing in the file reads (likely stripped code).
    const unsigned short s_Unk78[5] = { 1, 2, 3, 4, 5 };

    // The model of the ball of each kind (none for the mixed games).
    const char *s_BallMdlName[msw::CTopMenuChr::CHR_NUM] = {
        "BG_ball_BK", "BG_ball_VB", "BG_ball_DB", "BG_ball_HK", 0,
    };

    // (guessed) Stand-in for code of the original that was optimised away
    // or dropped by the linker. Behaviour-free; see notes/decisions.md,
    // "Unreferenced data kept by dead references".
    inline void UnusedReferences()
    {
        (void)s_Unk78;
    }

    // The motions of a character (indices in its motion file).
    enum {
        MOT_IDLE = 1,
        MOT_ACTIVE_IN = 2,
        MOT_ACTIVE = 3,
        MOT_TO_IDLE = 4,
        MOT_DECIDE = 5
    };

    // The flags of bcs::CAnmMotion::Play used here (names guessed).
    enum {
        MOTFLAG_LOOP = 2,
        MOTFLAG_REVERSE = 8
    };

    // The colours that make the ball gold: the colour of TEV constant 0 and
    // the alpha of constant 1.
    bcs::BCS_Color s_GoldKColor0(0xFFDCA000);
    bcs::BCS_Color s_GoldKColor1(0x000000FF);

    // Swaps the two values if they are in the wrong order.
    inline void SortPair(float &rMin, float &rMax)
    {
        if (rMin > rMax) {
            float f = rMin;
            rMin = rMax;
            rMax = f;
        }
    }

    // An angle brought into [0, 2 pi].
    inline float WrapRad(float fRad)
    {
        while (fRad > 6.2831855f) {
            fRad -= 6.2831855f;
        }
        while (fRad < 0.0f) {
            fRad += 6.2831855f;
        }
        return fRad;
    }
}

// msw::CTopMenuChrObj -------------------------------------------------------

msw::CTopMenuChrObj::CTopMenuChrObj(bcs::CTask *pParent, long nKind)
    : CTask(pParent, -1)
    , m_nKind(nKind)
    , m_nFlag(0)
    , m_pMdl(0)
    , m_pMotion(0)
    , m_nStep(0)
    , m_fBlend(0.0f)
    , m_pBall(0)
    , m_nBoundArg(0)
    , m_fSpeedRate(1.0f)
{
    m_vPos.Set(0.0f, 0.0f, 0.0f);
    m_vRot.Set(0.0f, 0.0f, 0.0f);

    InitState();
    Create_(nKind);

    // The ball, held on the node "ball_rot" of the character.
    if (s_BallMdlName[m_nKind] != 0) {
        bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
        nw4r::g3d::ResFile file = pRes->GetG3DResFileByMdlName(s_BallMdlName[m_nKind]);

        m_pBall = new CTopMenuBall(this, m_nKind);
        if (m_pBall != 0) {
            m_pBall->CreateMdl(file);

            unsigned long nNode = 0;
            nw4r::g3d::ResNode node = m_pMdl->GetResMdl().GetResNode("ball_rot");
            if (node.IsValid()) {
                nNode = node.GetID();
            }
            m_pBall->Attach(m_pMdl, nNode, 0.0f, 3, 0, 0);
        }
    }

    m_nFlag |= FLAG_HIDE;
}

msw::CTopMenuChrObj::~CTopMenuChrObj()
{
    if (m_pMdl != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
    }
    m_pMdl = 0;
    m_pMotion = 0;
}

// (guessed) The state table, entered in the first state. Only the
// constructor calls it, inline, so the module link dropped the function
// itself; its copy of the pointer-to-member constants is still in .data
// after the constructor's, and the template functions it uses come after
// the destructor.
void msw::CTopMenuChrObj::InitState()
{
    UnusedReferences();
    InitFsm(this, STATE_NUM);
    SetState(STATE_IDLE, &CTopMenuChrObj::CtorStateIdle, &CTopMenuChrObj::LoopStateIdle,
             &CTopMenuChrObj::DtorStateIdle);
    SetState(STATE_ACTIVE, &CTopMenuChrObj::CtorStateActive, &CTopMenuChrObj::LoopStateActive,
             &CTopMenuChrObj::DtorStateActive);
    SetState(STATE_TO_IDLE, &CTopMenuChrObj::CtorStateToIdle, &CTopMenuChrObj::LoopStateToIdle,
             &CTopMenuChrObj::DtorStateToIdle);
    SetState(STATE_DECIDE, &CTopMenuChrObj::CtorStateDecide, &CTopMenuChrObj::LoopStateDecide,
             &CTopMenuChrObj::DtorStateDecide);
    ChangeState(STATE_IDLE);
}

void msw::CTopMenuChrObj::Create_(long nKind)
{
    long nRes = nKind > 4 ? 4 : (nKind < 0 ? 0 : nKind);

    m_pMdl = new (0x100) CChrMdlObj;
    nw4r::g3d::ResFile mdlFile = CLoadData::GetRes(nRes + 0xB);
    nw4r::g3d::ResFile motFile = CLoadData::GetRes(nRes + 0x1B);
    m_pMdl->CreateTopMenuObj(mdlFile, nKind, 0);
    m_pMdl->ResetLightSelector();
    m_pMdl->OffFlag(0x4000);
    m_pMdl->OnFlag(0x100);
    m_pMdl->OnFlag(0x2);
    m_pMdl->OnFlag(0x2000);

    m_pMotion = new (0x100) bcs::CAnmMotion;
    m_pMotion->Create(m_pMdl, motFile, 0, 3, 0);
    m_pMotion->SetTrgCallback(CallbackMotTrg, this);
    m_pMdl->AttachAnmObj(m_pMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);

    // The character of the mixed games has material animations.
    if (nKind == 4) {
        if (mdlFile.HasResAnmClr()) {
            nw4r::g3d::ResAnmClr res = mdlFile.GetResAnmClr(0);
            bcs::CAnmMatClr *pAnm = new (0x100) bcs::CAnmMatClr;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
            }
        }
        if (mdlFile.HasResAnmTexSrt()) {
            nw4r::g3d::ResAnmTexSrt res = mdlFile.GetResAnmTexSrt(0);
            bcs::CAnmTexSrt *pAnm = new (0x100) bcs::CAnmTexSrt;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
            }
        }
        if (mdlFile.HasResAnmTexPat()) {
            nw4r::g3d::ResAnmTexPat res = mdlFile.GetResAnmTexPat(0);
            bcs::CAnmTexPat *pAnm = new (0x100) bcs::CAnmTexPat;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_3);
            }
        }
    }

    bcs::CRenderMan::PushBack(m_pMdl, 2);
}

void msw::CTopMenuChrObj::Main()
{
    if (m_nFlag & FLAG_HIDE) {
        if (m_pMdl != 0) {
            m_pMdl->SetDispEnable(0, 1);
        }
        if (m_pBall != 0) {
            m_pBall->Hide();
        }
    } else {
        ExecState();
        if (m_pMdl != 0) {
            m_pMdl->SetDispEnable(1, 1);
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->UpdateFrame(m_fSpeedRate * GetFrameRateF());
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
        if (m_pBall != 0) {
            m_pBall->Show();
        }
    }
}

void msw::CTopMenuChrObj::CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser)
{
    if (pUser != 0) {
        static_cast<CTopMenuChrObj *>(pUser)->CallbackMotTrg_(pTrg);
    }
}

void msw::CTopMenuChrObj::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
{
    switch (pTrg->m_nType) {
        case 1:
            // The ball hits the floor.
            if (!(m_nFlag & FLAG_HIDE) && m_pBall != 0) {
                m_pBall->TrgBound(m_nBoundArg);
            }
            break;
        case 2:
            if (s_SeTable[m_nKind][SE_TRG_2] > -1) {
                CGameSnd::Play(s_SeTable[m_nKind][SE_TRG_2], m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            if (m_pBall != 0) {
                long nSe = s_SeTable[m_pBall->GetKind()][SE_TRG_2_BALL];
                if (nSe >= 0) {
                    m_pBall->PlayMoveSe(nSe, &m_pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
                }
            }
            if (s_SeTable[m_nKind][SE_TRG_2_FREE] >= 0) {
                CGameSnd::Play(s_SeTable[m_nKind][SE_TRG_2_FREE], (const bcs::FVEC3 *)0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            break;
        case 0x12:
            if (s_SeTable[m_nKind][SE_TRG_12] > -1) {
                CGameSnd::Play(s_SeTable[m_nKind][SE_TRG_12], m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            break;
        case 0x40: {
            // The expression of the face, 1 to 7.
            long nType = pTrg->m_nParam0 + 1;
            if (nType < 1 || nType > 7) {
                nType = 1;
            }
            m_pMdl->SetFaceAnmType(nType);
            break;
        }
    }
}

namespace {

    // Plays motion nMot of pMotion; a negative fBlend plays it backwards.
    inline void PlayMotion(bcs::CAnmMotion *pMotion, long nMot, unsigned long nFlag, float fRate, float fBlend)
    {
        if (pMotion != 0) {
            unsigned long nPlayFlag = 0;
            if (fBlend < 0.0f) {
                fBlend = fabsf(fBlend);
                nPlayFlag |= MOTFLAG_REVERSE;
            }
            pMotion->Play(nMot, nPlayFlag | nFlag, fRate, fBlend, 0.0f);
        }
    }

    // Whether the motion has ended (also when there is none).
    inline int IsMotionEnd(bcs::CAnmMotion *pMotion)
    {
        return pMotion != 0 ? pMotion->IsEnd() : 1;
    }
}

void msw::CTopMenuChrObj::CtorStateIdle()
{
    PlayMotion(m_pMotion, MOT_IDLE, MOTFLAG_LOOP, 1.0f, 0.0f);
}

void msw::CTopMenuChrObj::LoopStateIdle()
{
    if (m_nFlag & FLAG_SELECT) {
        ChangeState(STATE_ACTIVE);
    }
}

void msw::CTopMenuChrObj::DtorStateIdle()
{
}

void msw::CTopMenuChrObj::CtorStateActive()
{
    m_nStep = 0;
    // Back from the decide motion: straight to the loop.
    if (m_nFlag & FLAG_DECIDE_END) {
        m_nFlag &= ~FLAG_DECIDE_END;
        m_nStep = 2;
    }
}

void msw::CTopMenuChrObj::LoopStateActive()
{
    switch (m_nStep) {
        case 0: {
            float fRate = m_nKind == 4 ? 1.0f : 2.0f;
            PlayMotion(m_pMotion, MOT_ACTIVE_IN, 0, fRate, m_fBlend);
            if (s_SeTable[m_nKind][SE_ACTIVE] > -1) {
                CGameSnd::Play(s_SeTable[m_nKind][SE_ACTIVE], m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            m_nStep++;
            break;
        }
        case 1:
            if (!IsMotionEnd(m_pMotion)) {
                break;
            }
            m_nStep++;
            // fall through
        case 2:
            PlayMotion(m_pMotion, MOT_ACTIVE, MOTFLAG_LOOP, 1.0f, 0.0f);
            m_nStep++;
            break;
    }

    if (!(m_nFlag & FLAG_SELECT)) {
        ChangeState(STATE_TO_IDLE);
    } else if (m_nFlag & FLAG_DECIDE) {
        ChangeState(STATE_DECIDE);
    }
}

void msw::CTopMenuChrObj::DtorStateActive()
{
}

void msw::CTopMenuChrObj::CtorStateToIdle()
{
    m_nStep = 0;
}

void msw::CTopMenuChrObj::LoopStateToIdle()
{
    if (m_nStep == 0) {
        float fRate = m_nKind == 4 ? 1.0f : 2.0f;
        PlayMotion(m_pMotion, MOT_TO_IDLE, 0, fRate, m_fBlend);
        m_nStep++;
    }

    if (m_nFlag & FLAG_SELECT) {
        ChangeState(STATE_ACTIVE);
    } else if (IsMotionEnd(m_pMotion)) {
        ChangeState(STATE_IDLE);
    }
}

void msw::CTopMenuChrObj::DtorStateToIdle()
{
}

void msw::CTopMenuChrObj::CtorStateDecide()
{
    m_nStep = 0;
    m_nFlag &= ~FLAG_DECIDE_END;
}

void msw::CTopMenuChrObj::LoopStateDecide()
{
    switch (m_nStep) {
        case 0:
            if (!(m_nFlag & FLAG_DECIDE_WAIT)) {
                PlayMotion(m_pMotion, MOT_DECIDE, 0, 1.0f, m_fBlend);
                m_nStep++;
            }
            break;
        case 1:
            if (IsMotionEnd(m_pMotion)) {
                m_nFlag |= FLAG_DECIDE_END;
                m_nStep++;
            }
            break;
    }
}

void msw::CTopMenuChrObj::DtorStateDecide()
{
    m_nFlag |= FLAG_DECIDE_END;
    m_nFlag &= ~(FLAG_SELECT | FLAG_DECIDE | FLAG_DECIDE_WAIT);
}

// msw::CTopMenuChr ----------------------------------------------------------

msw::CTopMenuChr::CTopMenuChr(bcs::CTask *pParent)
    : CTask(pParent, -1)
    , m_nSelectGame(-1)
    , m_bMixDisp(0)
{
    for (long i = 0; i < 4; i++) {
        CGame::SetTeamColorIdx(i, i);
    }

    for (long i = 0; i < CHR_NUM; i++) {
        m_pChrObj[i] = new (0x100) CTopMenuChrObj(this, i);
        m_pBBox[i] = new (0x100) CBBoxObj;
        if (m_pBBox[i] != 0) {
            m_pBBox[i]->m_vTrans.Set(0.0f, s_BBoxInfo[i].fY, 0.0f);
            m_pBBox[i]->SetSize(s_BBoxInfo[i].fMinX, s_BBoxInfo[i].fMaxX, s_BBoxInfo[i].fMinY,
                                s_BBoxInfo[i].fMaxY, 0.0f, 0.0f);
            m_pBBox[i]->AttachToObject(m_pChrObj[i]->GetMdl(), 0, bcs::CScnObj::ATTACHMODE_0);
            bcs::CRenderMan::PushBack(m_pBBox[i], 2);
        }
    }
}

msw::CTopMenuChr::~CTopMenuChr()
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pBBox[i] != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pBBox[i]);
        }
        m_pBBox[i] = 0;
    }
}

void msw::CTopMenuChr::Main()
{
}

void msw::CTopMenuChr::Disp()
{
}

void msw::CTopMenuChr::Reset()
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0) {
            m_pChrObj[i]->Reset();
        }
    }
}

void msw::CTopMenuChr::EnableDisp(int bDisp)
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0) {
            m_pChrObj[i]->EnableDisp(bDisp);
        }
    }
    // The mixed games' character only when it is switched on.
    if (m_pChrObj[4] != 0) {
        m_pChrObj[4]->EnableDisp(m_bMixDisp && bDisp);
    }
}

void msw::CTopMenuChr::EnableMixDisp(int bDisp)
{
    m_bMixDisp = bDisp;
    if (m_pChrObj[4] != 0) {
        m_pChrObj[4]->EnableDisp(bDisp);
    }
}

void msw::CTopMenuChr::AttachTo(long nChr, bcs::CScnObj *pParent, long nNode)
{
    if (nChr >= 0 && nChr < CHR_NUM) {
        CTopMenuChrObj *pObj = m_pChrObj[nChr];
        if (pObj != 0 && pObj->GetMdl() != 0) {
            pObj->GetMdl()->AttachToObject(pParent, nNode, bcs::CScnObj::ATTACHMODE_0);
        }
    }
}

void msw::CTopMenuChr::SetSelectGame(long nGame, float fFrame)
{
    m_nSelectGame = nGame;
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0) {
            m_pChrObj[i]->SetSelect(i == nGame, fFrame);
        }
    }
}

int msw::CTopMenuChr::PrepareDecideAnim()
{
    int bReady = 0;

    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0 && m_pChrObj[i]->IsSelect()) {
            m_pChrObj[i]->PrepareDecide();
            if (m_pChrObj[i]->IsDecide()) {
                bReady = 1;
            }
        }
    }
    return bReady;
}

int msw::CTopMenuChr::IsReadyDecideAnim() const
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0 && m_pChrObj[i]->IsDecideWait()) {
            return 1;
        }
    }
    return 0;
}

void msw::CTopMenuChr::PlayDecideAnim(float fFrame)
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0 && m_pChrObj[i]->IsDecideWait()) {
            m_pChrObj[i]->PlayDecide(fFrame);
        }
    }
}

int msw::CTopMenuChr::IsEndDecideAnim() const
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0 && m_pChrObj[i]->IsDecideEnd()) {
            return 1;
        }
    }
    return 0;
}

void msw::CTopMenuChr::EnableGoldModel(long nChr, int bGold)
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (nChr < 0 || nChr == i) {
            CTopMenuChrObj *pObj = m_pChrObj[i];
            if (pObj != 0 && pObj->GetKind() != 4) {
                // The hockey player's model turns gold, the others' ball.
                if (pObj->GetKind() == 3) {
                    if (pObj->GetMdl() != 0) {
                        pObj->GetMdl()->EnableGoldModel(bGold);
                    }
                } else if (pObj->GetBall() != 0) {
                    pObj->GetBall()->EnableGoldModel(bGold);
                }
            }
        }
    }
}

long msw::CTopMenuChr::GetBBoxInfo(long nChr, nw4r::math::AABB &rBox)
{
    CBBoxObj *pBox = m_pBBox[nChr];

    if (pBox == 0) {
        return 0;
    }
    if (nChr == 4 && !m_bMixDisp) {
        return 0;
    }
    return pBox->GetResult(rBox);
}

void msw::CTopMenuChr::SetMotionSpeedRate(float fRate)
{
    for (long i = 0; i < CHR_NUM; i++) {
        if (m_pChrObj[i] != 0) {
            m_pChrObj[i]->SetMotionSpeedRate(fRate);
        }
    }
}

// msw::CTopMenuBall ---------------------------------------------------------

inline msw::CTopMenuBall::CTopMenuBall(bcs::CTask *pParent, long nKind)
    : CBall(pParent, 0)
    , m_nKind(nKind)
{
    bcs::FVEC3 vRot;
    bcs::FMTX34 mtx;

    vRot.x = WrapRad(bcs::BCS_RandMax(6.2831855f));
    vRot.y = WrapRad(bcs::BCS_RandMax(6.2831855f));
    vRot.z = WrapRad(bcs::BCS_RandMax(6.2831855f));
    // The puck lies flat.
    if (m_nKind == 3) {
        vRot.z = 0.0f;
        vRot.x = 0.0f;
    }
    nw4r::math::MTX34RotXYZRad(reinterpret_cast<nw4r::math::MTX34 *>(&mtx), vRot.x, vRot.y, vRot.z);
    bcs::BCS_Quat_FromMatrix(reinterpret_cast<bcs::QUATERNION &>(m_Rot), mtx);
    PSQUATNormalize(&m_Rot, &m_Rot);
}

inline void msw::CTopMenuBall::CreateMdl(nw4r::g3d::ResFile file)
{
    nw4r::g3d::ResFile &rFile = *reinterpret_cast<nw4r::g3d::ResFile *>(m_ResFile);

    rFile = file;
    nw4r::g3d::ResMdl mdl = rFile.GetResMdl(0);
    nw4r::g3d::ResMdl mdlSub = rFile.GetResMdl(0);
    rFile.GetResMdl(0); // (the result is not used)
    if (!mdlSub.IsValid()) {
        mdlSub = mdl;
    }
    CreateMdlObj_(rFile, mdl, nw4r::g3d::ResMdl(), mdlSub, 0x40);
}

msw::CTopMenuBall::~CTopMenuBall()
{
}

void msw::CTopMenuBall::TrgBound(unsigned long)
{
    if (s_SeTable[m_nKind][SE_BOUND] >= 0) {
        PlaySePos(s_SeTable[m_nKind][SE_BOUND], 0, 1, -1, 0.0f, 1.0f, 0.0f);
    }
}

void msw::CTopMenuBall::EnableGoldModel(int bGold)
{
    if (bGold) {
        if (m_pMdlObj != 0) {
            m_pMdlObj->SetMatTevKColor(GX_KCOLOR0, s_GoldKColor0);
            m_pMdlObj->SetMatTevKColorA(GX_KCOLOR1, s_GoldKColor1.a);
        }
        if (m_pSubObj2 != 0) {
            m_pSubObj2->SetMatTevKColor(GX_KCOLOR0, s_GoldKColor0);
            m_pSubObj2->SetMatTevKColorA(GX_KCOLOR1, s_GoldKColor1.a);
        }
    } else {
        if (m_pMdlObj != 0) {
            m_pMdlObj->RestoreMatTevKColor(GX_KCOLOR0);
            m_pMdlObj->RestoreMatTevKColor(GX_KCOLOR1);
        }
        if (m_pSubObj2 != 0) {
            m_pSubObj2->RestoreMatTevKColor(GX_KCOLOR0);
            m_pSubObj2->RestoreMatTevKColor(GX_KCOLOR1);
        }
    }
}

// msw::CBBoxObj -------------------------------------------------------------

msw::CBBoxObj::CBBoxObj()
{
    m_bValid = 0;
    m_Box.min.x = m_Box.min.y = m_Box.min.z = 0.0f;
    m_Box.max.x = m_Box.max.y = m_Box.max.z = 0.0f;
    m_Result.min.x = m_Result.min.y = m_Result.min.z = 0.0f;
    m_Result.max.x = m_Result.max.y = m_Result.max.z = 0.0f;
}

msw::CBBoxObj::~CBBoxObj()
{
}

void msw::CBBoxObj::SetSize(float fMinX, float fMaxX, float fMinY, float fMaxY, float fMinZ, float fMaxZ)
{
    m_Box.min.x = fMinX;
    m_Box.min.y = fMinY;
    m_Box.min.z = fMinZ;
    m_Box.max.x = fMaxX;
    m_Box.max.y = fMaxY;
    m_Box.max.z = fMaxZ;
    SortPair(m_Box.min.x, m_Box.max.x);
    SortPair(m_Box.min.y, m_Box.max.y);
    SortPair(m_Box.min.z, m_Box.max.z);
}

void msw::CBBoxObj::ObjProc(unsigned long nTask, unsigned long nArg, void *pArg)
{
    if (IsProcDisabled(nTask)) {
        return;
    }
    switch (nTask) {
        case 1:
            CalcRootMtx();
            CalcWorldMtx(0);
            break;
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 0x10001:
            break;
        case 0x10003:
            SetParent(0);
            break;
        case 0x10002:
            SetParent(static_cast<CG3dObj *>(pArg));
            break;
        case 4:
            CalcView_(nArg, static_cast<const bcs::FMTX34 *>(pArg));
            // fall through
        default:
            bcs::CScnObj::ObjProc(nTask, nArg, pArg);
            break;
    }
}

void msw::CBBoxObj::CalcView_(long nView, const bcs::FMTX34 *pViewMtx)
{
    nw4r::math::MTX34 invMtx;
    nw4r::math::VEC3 scrMax;
    nw4r::math::VEC3 scrMin;
    // The box's corners in the world, without the turn of the view.
    bcs::FVEC3 vOfs(m_Mtx[SCNOBJMTXTYPE_1].m[0][3], m_Mtx[SCNOBJMTXTYPE_1].m[1][3], m_Mtx[SCNOBJMTXTYPE_1].m[2][3]);
    bcs::FVEC3 vMin(m_Box.min.x, m_Box.min.y, m_Box.min.z);
    bcs::FVEC3 vMax(m_Box.max.x, m_Box.max.y, m_Box.max.z);

    PSMTXInverse(pViewMtx->m, reinterpret_cast<float(*)[4]>(&invMtx));
    nw4r::math::VEC3TransformNormal(reinterpret_cast<nw4r::math::VEC3 *>(&vMin), &invMtx,
                                    reinterpret_cast<nw4r::math::VEC3 *>(&vMin));
    nw4r::math::VEC3TransformNormal(reinterpret_cast<nw4r::math::VEC3 *>(&vMax), &invMtx,
                                    reinterpret_cast<nw4r::math::VEC3 *>(&vMax));
    bcs::FVEC3Add(&vMin, &vMin, &vOfs);
    bcs::FVEC3Add(&vMax, &vMax, &vOfs);

    // On the screen.
    nw4r::g3d::Camera camera(reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(nView)));
    camera.Project(&scrMin, *reinterpret_cast<nw4r::math::VEC3 *>(&vMin));
    camera.Project(&scrMax, *reinterpret_cast<nw4r::math::VEC3 *>(&vMax));
    if (scrMin.x < scrMax.x) {
        m_Result.min.x = scrMin.x;
        m_Result.max.x = scrMax.x;
    } else {
        m_Result.max.x = scrMin.x;
        m_Result.min.x = scrMax.x;
    }
    if (scrMin.y < scrMax.y) {
        m_Result.min.y = scrMin.y;
        m_Result.max.y = scrMax.y;
    } else {
        m_Result.max.y = scrMin.y;
        m_Result.min.y = scrMax.y;
    }
    if (scrMin.z < scrMax.z) {
        m_Result.min.z = scrMin.z;
        m_Result.max.z = scrMax.z;
    } else {
        m_Result.max.z = scrMin.z;
        m_Result.min.z = scrMax.z;
    }
    m_bValid = 1;
}

long msw::CBBoxObj::GetResult(nw4r::math::AABB &rBox) const
{
    if (m_bValid) {
        rBox = m_Result;
        return 1;
    }
    return 0;
}

// msw::CMiiRenderBase and the classes derived from it -----------------------

namespace {

    // The size of the Mii's head in all of them.
    const float MII_HEAD_SCALE = 1.6f;

    // The model numbers of the Mii (CLoadData::GetChrMdlRes).
    enum {
        CHR_MII_MALE = 0x13,
        CHR_MII_FEMALE = 0x14
    };
}

msw::CMiiRenderBase::CMiiRenderBase(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight,
                                    unsigned long nFlag)
    : CTask(pParent, -1)
    , m_pMdl(0)
    , m_pMotion(0)
    , m_pPrerend(0)
    , m_nPrerendMode(1)
    , m_Unk44(0)
{
    RFLAdditionalInfo info;

    m_pMdl = new (0x100) CChrMdlObj;
    bcs::CMiiMan::GetAdditionalInfo(info, rData);
    long nChr = CHR_MII_MALE;
    if (info.sex == 1) {
        nChr = CHR_MII_FEMALE;
    }
    m_pMdl->CreatePlayerInfoObj(CLoadData::GetChrMdlRes(0x27, nChr, 0), rData, nFlag, 0);
    m_pMdl->OnFlag(0x2000);
    m_pMdl->SetMiiHeadScale(MII_HEAD_SCALE);

    nw4r::g3d::ResFile motFile = CLoadData::GetChrMotRes(0x28, nChr);
    m_pMotion = new (0x100) bcs::CAnmMotion;
    m_pMotion->Create(m_pMdl, motFile, CC_DATA::GetMdl_NodeExInfo(nChr, 0), 3, 0);
    m_pMotion->OnFlag(8);
    m_pMotion->SetTrgCallback(CChrMdlObj::CallbackMotTrg, m_pMdl);
    m_pMotion->Play(7, 2, 1.0f, 0.0f, 0.0f);
    m_pMdl->AttachAnmObj(m_pMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);

    m_pPrerend = new (0x100) CGenPrerendGrp(m_pMdl, nWidth, nHeight, 1);
    m_pPrerend->SetCameraPos(bcs::FVEC3(0.0f, 1.5f, 5.5f));
    m_pPrerend->SetCameraTarget(bcs::FVEC3(0.0f, 1.05f, 0.0f));
}

msw::CMiiRenderBase::~CMiiRenderBase()
{
    if (m_pPrerend != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pPrerend);
    }
    m_pPrerend = 0;
    if (m_pMdl != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
    }
    m_pMdl = 0;
    m_pMotion = 0;
}

void msw::CMiiRenderBase::Main()
{
    MainFunc();
    if (m_pMdl != 0) {
        m_pMdl->UpdateFrame(GetFrameRateF());
        m_pMdl->CalcRootMtx();
        m_pMdl->CalcWorldMatrixAll(0);
    }
    if (m_pPrerend != 0) {
        m_pPrerend->SetMode(m_nPrerendMode);
    }
}

void msw::CMiiRenderBase::MainFunc()
{
}

void *msw::CMiiRenderBase::GetTexImg() const
{
    if (m_pPrerend != 0) {
        return m_pPrerend->GetTexImg();
    }
    return 0;
}

msw::CPlayerInfoRender::CPlayerInfoRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight)
    : CMiiRenderBase(pParent, rData, nWidth, nHeight, 0x3F)
{
    if (m_pMdl != 0) {
        m_pMdl->SetMiiHeadScale(MII_HEAD_SCALE);
    }
    if (m_pPrerend != 0) {
        m_pPrerend->SetCameraPos(bcs::FVEC3(0.0f, 1.5f, 4.1f));
        m_pPrerend->SetCameraTarget(bcs::FVEC3(0.0f, 1.4f, 0.0f));
    }

    InitState();
    ChangeState(STATE_OFFLINE);
}

msw::CPlayerInfoRender::~CPlayerInfoRender()
{
}

void msw::CPlayerInfoRender::MainFunc()
{
    ExecState();
}

void msw::CPlayerInfoRender::SetDispState(long nState)
{
    ChangeState(nState);
}

// (guessed) As CTopMenuChrObj::InitState, without the first state: absent
// from the binary, but its copy of the pointer-to-member constants is in
// .data, and CStateTemplate's constructor comes after SetDispState.
void msw::CPlayerInfoRender::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_OFFLINE, &CPlayerInfoRender::CtorStateOffline, &CPlayerInfoRender::LoopStateOffline,
             &CPlayerInfoRender::DtorStateOffline);
    SetState(STATE_ONLINE, &CPlayerInfoRender::CtorStateOnline, &CPlayerInfoRender::LoopStateOnline,
             &CPlayerInfoRender::DtorStateOnline);
    SetState(STATE_BATTLE, &CPlayerInfoRender::CtorStateBattle, &CPlayerInfoRender::LoopStateBattle,
             &CPlayerInfoRender::DtorStateBattle);
}

void msw::CPlayerInfoRender::CtorStateOffline()
{
    if (m_pMotion != 0) {
        m_pMotion->Play(0xE, 2, 1.0f, 0.0f, 0.0f);
    }
}

void msw::CPlayerInfoRender::LoopStateOffline()
{
}

void msw::CPlayerInfoRender::DtorStateOffline()
{
}

void msw::CPlayerInfoRender::CtorStateOnline()
{
    if (m_pMotion != 0) {
        m_pMotion->Play(7, 2, 1.0f, 0.0f, 0.0f);
    }
}

void msw::CPlayerInfoRender::LoopStateOnline()
{
}

void msw::CPlayerInfoRender::DtorStateOnline()
{
}

void msw::CPlayerInfoRender::CtorStateBattle()
{
    if (m_pMotion != 0) {
        m_pMotion->Play(9, 2, 1.0f, 0.0f, 0.0f);
    }
}

void msw::CPlayerInfoRender::LoopStateBattle()
{
}

void msw::CPlayerInfoRender::DtorStateBattle()
{
}

msw::CPlayerMiiRender::CPlayerMiiRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight)
    : CMiiRenderBase(pParent, rData, nWidth, nHeight, 0x3F)
{
    if (m_pMdl != 0) {
        m_pMdl->SetMiiHeadScale(MII_HEAD_SCALE);
    }
    if (m_pPrerend != 0) {
        m_pPrerend->SetCameraPos(bcs::FVEC3(0.0f, 1.5f, 5.5f));
        m_pPrerend->SetCameraTarget(bcs::FVEC3(0.0f, 1.05f, 0.0f));
    }
}

msw::CPlayerMiiRender::~CPlayerMiiRender()
{
}

void msw::CPlayerMiiRender::MainFunc()
{
}

msw::CMiiSelectRender::CMiiSelectRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight)
    : CMiiRenderBase(pParent, rData, nWidth, nHeight, 0x21)
{
    if (m_pMdl != 0) {
        m_pMdl->SetMiiHeadScale(MII_HEAD_SCALE);
    }
    if (m_pPrerend != 0) {
        m_pPrerend->SetCameraPos(bcs::FVEC3(0.0f, 1.5f, 3.5f));
        m_pPrerend->SetCameraTarget(bcs::FVEC3(0.0f, 1.44f, 0.0f));
    }
}

msw::CMiiSelectRender::~CMiiSelectRender()
{
}

void msw::CMiiSelectRender::MainFunc()
{
}
