// msw::CChrCoreBasketWaluigi: Waluigi in the basketball game. His special
// shot makes three copies of the ball (CBallManBasket::CreateDummyBall_)
// and hangs them and the ball itself from the hands of his option model;
// he throws the three copies one after the other at the shot frames of
// three motions, then shoots the ball as in the common special shot.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CGame.hpp"

// The option model's nodes the four balls hang from: the three copies,
// then the ball itself.
static const char *s_ArtsBallNode[msw::CChrCoreBasketWaluigi::ARTS_BALL_NODE_NUM] = {
    "R_hand_a",
    "L_hand_b",
    "R_hand_b",
    "L_hand_a",
};

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CChrCoreBasketWaluigi::CChrCoreBasketWaluigi()
{
    for (int i = 0; i < ARTS_BALL_NUM; i++) {
        m_ahArtsBall[i] = 0;
    }
}

inline void msw::CChrCoreBasketWaluigi::DeleteArtsBall_()
{
    for (int i = 0; i < ARTS_BALL_NUM; i++) {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahArtsBall[i]));
        if (pBall) {
            CBallMan::DelBall(pBall->GetBallNo());
        }
        m_ahArtsBall[i] = 0;
    }
}

msw::CChrCoreBasketWaluigi::~CChrCoreBasketWaluigi()
{
    DeleteArtsBall_();
}

// The three copies start where the ball is, turned as it is, and are
// destroyed when the game resets; each hangs from its hand of the option
// model, a little to the side.
void msw::CChrCoreBasketWaluigi::CreateArtsBall_()
{
    // static-order swap: declared in this order the values come out in the
    // original's order, but the "@0" suffix lands on uAttachFlag instead of
    // fOff_X (see notes/decisions.md).
    static const unsigned long uAttachFlag = 0x58;
    static const float fOff_X = 0.44f;

    m_nBasketFlag |= 4;

    long anNode[ARTS_BALL_NODE_NUM];
    anNode[0] = ArtsOptGetNodeIdx(s_ArtsBallNode[0]);
    anNode[1] = ArtsOptGetNodeIdx(s_ArtsBallNode[1]);
    anNode[2] = ArtsOptGetNodeIdx(s_ArtsBallNode[2]);
    anNode[3] = ArtsOptGetNodeIdx(s_ArtsBallNode[3]);

    CBall *pBall = GetAttachBall();
    if (!pBall) {
        return;
    }

    bcs::FVEC3 aOfs[msw::CChrCoreBasketWaluigi::ARTS_BALL_NODE_NUM] = {
        bcs::FVEC3(fOff_X, 0.0f, 0.0f),
        bcs::FVEC3(fOff_X, 0.0f, 0.0f),
        bcs::FVEC3(fOff_X, 0.0f, 0.0f),
        bcs::FVEC3(fOff_X, 0.0f, 0.0f),
    };

    for (int i = 0; i < ARTS_BALL_NUM; i++) {
        CBall *pCopy = CBallManBasket::GetInstance()->CreateDummyBall_();
        pCopy->WarpPos(pBall->GetPos());
        pCopy->SetRot(pBall->GetRot());
        pCopy->SetDestroyOnReset(1);
        m_ahArtsBall[i] = pCopy->GetId();
        if (anNode[i] >= 0) {
            pCopy->Attach(GetArtsOptMdl(), anNode[i], 0.0f, 3, 0x18, 0);
            pCopy->SetAttachPos(aOfs[i], 1);
        }
    }

    if (anNode[ARTS_BALL_NUM] >= 0) {
        pBall->Attach(GetArtsOptMdl(), anNode[ARTS_BALL_NUM], 0.0f, 3, uAttachFlag, 0);
        pBall->SetAttachPos(aOfs[ARTS_BALL_NUM], 1);
    }
}

void msw::CChrCoreBasketWaluigi::CtorStateAirTrick()
{
    DeleteArtsBall_();
    CChrCoreBasket::CtorStateAirTrick();
}

void msw::CChrCoreBasketWaluigi::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();
    SE_Stop(9, 0.0f);
}

float msw::CChrCoreBasketWaluigi::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x7E) + AnimGetResFrameNum(0x7F);
}

// Step 0 jumps until the trick motion ends, then plays the split motion;
// at its frame 60 (step 1) the copies are made. Ready once the motion is
// over.
int msw::CChrCoreBasketWaluigi::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nTrickStep) {
        case 0:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7F, 0, 0.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                Eff_CreateAttachAuto(0x14, 0x7D, 3);
                ArtsOptCreateAttachEffectAuto(1, 0x7F, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 60.0f) {
                CreateArtsBall_();
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

void msw::CChrCoreBasketWaluigi::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotStartArtsShot_(pWork);
    pWork->m_nTrickStep = 0;
}

// Three throws, each a throw (even steps: at the shot frame, copy 0, 1 then
// 2 leaves) and the next throw motion (odd steps). The last motion fades
// the option model out; at its shot frame the ball itself is shot as in
// the common special shot.
int msw::CChrCoreBasketWaluigi::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShotArtsBall_(0);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x84, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x17, 0x82, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x17, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(3, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShotArtsBall_(1);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x85, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x18, 0x83, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x18, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(4, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 4:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShotArtsBall_(2);
                pWork->m_nTrickStep++;
            }
            break;
        case 5:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x86, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x19, 0x84, 3);
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

void msw::CChrCoreBasketWaluigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x7C, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptSetAliveSE(0x424);
    ArtsOptSetFadeOutSE(0x425);
    ArtsOptCreateAttachEffectAuto(0, 0x7E, 3);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
}

void msw::CChrCoreBasketWaluigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x80, 3);
}

void msw::CChrCoreBasketWaluigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    SE_Stop(9, 0.0f);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x81, 3);
    Eff_SetSpeed(0x16, AnimGetSpeed(3));
    // The original reads the parameters here, once more below level 1, and
    // uses neither (most likely a value of them computed and left unused).
    GetCcParam2_();
    if (pWork->m_nUnkA0 < 1) {
        GetCcParam2_();
    }
    ArtsOptSetEffectSpeed(2, AnimGetSpeed(3));
}

void msw::CChrCoreBasketWaluigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// Copy nIndex leaves his hand for the goal as a special shot of his player.
void msw::CChrCoreBasketWaluigi::ShotArtsBall_(long nIndex)
{
    // static-order swap: the original names these nShootType@0 and fRate
    // (see notes/decisions.md).
    static const long nShootType = 9;
    static const float fRate = 1.0f;

    if (nIndex < 0 || nIndex >= ARTS_BALL_NUM) {
        return;
    }

    CBallBasket *pBall = static_cast<CBallBasket *>(bcs::CTaskMan::GetTask(m_ahArtsBall[nIndex]));
    if (!pBall) {
        return;
    }

    // (Written as separate statements: as one expression the function is
    // small enough to be inlined into SubStateArtsShotIsEndArtsShot_, which
    // the original's is not.)
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    float fValue = (1.0f / 60.0f) * pParam->m_fArtsBallUnk240;
    pBall->SetArtsShotUnk(fValue);
    Util_SendMsgToAll((CChrDrv::MSG_ID)7, GetPlayerNo(), nShootType, 0, 0);
    pBall->SetShootMode(nShootType, fRate, GetPlayerNo(), 1, 1 << GetPlayerNo());
    PlayVoice(0x24, 0);
    SE_PlayArtsShootSE(pBall->GetPos());
    CGame::SendMessage(0x35, GetPlayerNo(), nShootType, pBall->GetBallNo());
    m_ahArtsBall[nIndex] = 0;
}

// Not written: ArtsShotMotPlay_(long). The module link stripped it (nothing
// called it), but its constants are still in the original's .rodata after
// ShotArtsBall_'s: seven longs (0, 7, 12, 8, 13, 9, 14), its static bLoser
// (0) and the floats 0.5 and -1. This file's .rodata is 0x28 bytes shorter
// for it.
