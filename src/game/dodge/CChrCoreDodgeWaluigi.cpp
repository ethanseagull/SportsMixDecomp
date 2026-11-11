// msw::CChrCoreDodgeWaluigi: Waluigi in the dodgeball game. His special
// shot splits the ball in four: three copies (CBallMan::AddBall) and the
// ball itself hang from the four "SP_item" nodes of his extra model
// (GetArtsOptMdl), and he throws them one after the other.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

// 60 degrees in radians; unused here, as in the other DodgeF chr cores
// (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // (guessed) Stand-in for code of the original that the link stripped.
    // Behaviour-free; see notes/decisions.md, "Dead float comparison". The
    // original's .rodata ends with a -1 and a 1 that nothing left in the file
    // reads.
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f || f == 1.0f) {
        }
    }
}

msw::CChrCoreDodgeWaluigi::CChrCoreDodgeWaluigi()
{
    for (int i = 0; i < DIVIDE_BALL_NUM; i++) {
        m_apDivideBall[i] = 0;
    }
}

msw::CChrCoreDodgeWaluigi::~CChrCoreDodgeWaluigi()
{
    DivideBallDeleteAll();
}

void msw::CChrCoreDodgeWaluigi::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
    DivideBallDeleteAll();
}

// Hangs the four balls from the nodes of the extra model: the three copies
// first, warped to the ball, then the ball itself.
void msw::CChrCoreDodgeWaluigi::DivideBallCreate()
{
    static const char *node_tbl[DIVIDE_BALL_NUM] = {
        "SP_item_01",
        "SP_item_04",
        "SP_item_03",
        "SP_item_02",
    };
    static const bcs::FVEC3 ofs_tbl[DIVIDE_BALL_NUM] = {
        bcs::FVEC3(-0.44f, 0.0f, 0.0f),
        bcs::FVEC3(0.44f, 0.0f, 0.0f),
        bcs::FVEC3(-0.44f, 0.0f, 0.0f),
        bcs::FVEC3(0.44f, 0.0f, 0.0f),
    };

    DivideBallDeleteAll();

    CBall *pBall = GetAttachBall();
    bcs::CMdlObj *pBallMdl = pBall ? pBall->GetMdlObj() : 0;
    if (!pBall || !pBallMdl) {
        return;
    }
    if (!pBallMdl->GetResMdl().IsValid()) {
        return;
    }

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (!pMdl) {
        return;
    }
    nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
    if (!mdl.IsValid()) {
        return;
    }

    for (int i = 0; i < DIVIDE_BALL_NUM; i++) {
        CBall *pDivide;
        if (i < DIVIDE_BALL_NUM - 1) {
            pDivide = CBallMan::AddBall(pBall);
        } else {
            pDivide = pBall;
        }
        unsigned long nNode = mdl.GetResNode(node_tbl[i]).GetID();
        if (i < DIVIDE_BALL_NUM - 1) {
            pDivide->WarpPos(pBall->GetPos());
            pDivide->Attach(pMdl, nNode, 0.0f, 3, 0, 0);
        } else {
            pDivide->DodgeReattach(pMdl, nNode);
        }
        pDivide->SetAttachPos(ofs_tbl[i], 1);
        m_apDivideBall[i] = pDivide;
    }
}

msw::CBall *msw::CChrCoreDodgeWaluigi::DivideBallGet(long nIndex)
{
    return m_apDivideBall[nIndex];
}

const msw::CBall *msw::CChrCoreDodgeWaluigi::DivideBallGet(long nIndex) const
{
    return m_apDivideBall[nIndex];
}

void msw::CChrCoreDodgeWaluigi::DivideBallDelete(long nIndex)
{
    m_apDivideBall[nIndex] = 0;
}

// Forgets a ball that has been thrown.
void msw::CChrCoreDodgeWaluigi::ReleaseBallDelete(long nIndex)
{
    m_apDivideBall[nIndex] = 0;
}

// Removes the copies still hanging (the last ball is the original one, which
// stays) and forgets all four.
void msw::CChrCoreDodgeWaluigi::DivideBallDeleteAll()
{
    for (int i = 0; i < DIVIDE_BALL_NUM; i++) {
        if (i < DIVIDE_BALL_NUM - 1 && m_apDivideBall[i]) {
            CBallMan::DelBall(m_apDivideBall[i]->GetBallNo());
        }
        m_apDivideBall[i] = 0;
    }
}

void msw::CChrCoreDodgeWaluigi::CtorStateAirTrick()
{
    DivideBallDeleteAll();
    CChrCoreDodge::CtorStateAirTrick();
}

float msw::CChrCoreDodgeWaluigi::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    return AnimGetResFrameNum(0x7A) + AnimGetResFrameNum(0x7B);
}

// Step 0 jumps until the trick motion ends, then plays the split motion;
// at its frame 60 (step 1) the ball splits. Ready once the motion is over.
int msw::CChrCoreDodgeWaluigi::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    switch (pWork->m_nTrickStep) {
        case 0:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7B, 0, 0.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                Eff_CreateAttachAuto(0x14, 0x79, 3);
                ArtsOptCreateAttachEffectAuto(1, 0x7A, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 60.0f) {
                DivideBallCreate();
                pWork->m_nTrickStep++;
            }
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
        default:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreDodgeWaluigi::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotStartArtsShot_(pWork);
    pWork->m_nTrickStep = 0;
}

// Three throws, each a throw (even steps: at the shot frame, ball 0, 1 then
// 2 leaves for the target) and the next throw motion (odd steps). The last
// motion fades the extra model out; at its shot frame the original ball is
// thrown as in the common shot.
int msw::CChrCoreDodgeWaluigi::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                CBall *pBall = DivideBallGet(0);
                if (pBall) {
                    ReleaseBallDelete(0);
                    pBall->Detach();
                    bcs::FVEC3 pos(pWork->m_TargetPos);
                    Util_ShootBall(pBall, pos, 9, 1);
                    PlayVoice(0x63, 0);
                    SE_PlayArtsShootSE(pBall->GetPos());
                }
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x80, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x17, 0x7D, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x17, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(3, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                CBall *pBall = DivideBallGet(1);
                if (pBall) {
                    ReleaseBallDelete(1);
                    pBall->Detach();
                    bcs::FVEC3 pos(pWork->m_TargetPos);
                    Util_ShootBall(pBall, pos, 9, 1);
                    PlayVoice(0x63, 0);
                    SE_PlayArtsShootSE(pBall->GetPos());
                }
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x81, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x18, 0x7E, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x18, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(4, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 4:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                CBall *pBall = DivideBallGet(2);
                if (pBall) {
                    ReleaseBallDelete(2);
                    pBall->Detach();
                    bcs::FVEC3 pos(pWork->m_TargetPos);
                    Util_ShootBall(pBall, pos, 9, 1);
                    PlayVoice(0x63, 0);
                    SE_PlayArtsShootSE(pBall->GetPos());
                }
                pWork->m_nTrickStep++;
            }
            break;
        case 5:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x82, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x19, 0x7F, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork);
                ArtsOptFadeOut(5.0f, 70.0f, 1);
                Eff_SetSpeed(0x19, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(5, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        default:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                SubStateArtsShotShootBall_(pWork);
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreDodgeWaluigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x77, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptSetAliveSE(0x424);
    ArtsOptSetFadeOutSE(0x425);
    ArtsOptCreateAttachEffectAuto(0, 0x78, 3);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
}

void msw::CChrCoreDodgeWaluigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x7B, 3);
}

void msw::CChrCoreDodgeWaluigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x7C, 3);
    Eff_SetSpeed(0x16, AnimGetSpeed(3));
    // The original reads the parameters here, once more below level 1, and
    // uses neither (most likely a value of them computed and left unused).
    GetCcParam_();
    if (pWork->m_nArtsLevel < 1) {
        GetCcParam_();
    }
    ArtsOptSetEffectSpeed(2, AnimGetSpeed(3));
}

void msw::CChrCoreDodgeWaluigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

long msw::CChrCoreDodgeWaluigi::GetMotIDType_SideStep() const
{
    float fUnused = 0.0f;
    UnusedReferences(fUnused);
    return 1;
}
