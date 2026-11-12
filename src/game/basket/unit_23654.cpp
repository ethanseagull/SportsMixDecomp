// msw::CChrCoreBasket's air trick state (state 0x27): the team special shot
// of the basketball game (the "arts shot"). The character jumps with a trick
// motion, moves back to a point away from the goal, hangs at the top of the
// jump until the driver asks for the shot (or the time runs out), shoots and
// is pushed back by the shot. The characters override the motions and
// effects (SubStateArtsShotPlay*) and some of the steps; the dunking ones
// write their own trick and end it with the common dunk
// (SubStateAirTrickCmnDunk_). The file also holds the arts event members and
// the answer to the other team's block (ResultAirTrickBattle).
#include "game/msw/CChrCoreBasket.hpp"

#include <string.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see
// CChrCoreBasketWaluigi.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

namespace {

    // bcs::FVEC3 derives from nw4r::math::VEC3 in the original, and the
    // vector differences go through VEC3's operators; here FVEC3 is a
    // separate struct of the same layout, so the operands are reinterpreted.
    typedef const nw4r::math::VEC3 &VEC3_CREF;

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // The air trick state.
    const long STATE_AIR_TRICK = 0x27;
    // (meaning not known) Another state that answers a block, with a work
    // of its own (StateWork0C).
    const long STATE_0C = 0xC;

    // The steps of the special shot (STATE_WORK_AIR_TRICK::m_nStep); the
    // dunking characters use their own numbers, 16 to 18 for the dunk.
    enum {
        STEP_INIT,
        STEP_START_TRICK,
        STEP_TRICK,
        STEP_READY,
        STEP_SHOT,
        STEP_HITBACK,
        STEP_HITBACK_ALT = 8,
        STEP_LAND = 10,

        STEP_DUNK_START = 16,
        STEP_DUNK_HANG,
        STEP_DUNK_FALL,
        STEP_DUNK_END = 22
    };

    // (names guessed) The work of state 0xC, as far as it is used here.
    struct StateWork0C {
        unsigned char m_Unk00[0x4];
        long m_nUnk04;          // 0x04, set to 9999 when a block is answered
        unsigned long m_nFlag;  // 0x08, bit 0x200000: the endure is on
    };

    inline float WrapRad(float rad)
    {
        while (rad > PI2) {
            rad -= PI2;
        }
        while (rad < 0.0f) {
            rad += PI2;
        }
        return rad;
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // The square of the distance between two points in the XZ plane.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // rVec without its height, scaled to length 1 (zero when it is zero).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &vec)
    {
        bcs::FVEC3 xz(vec.x, 0.0f, vec.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }
}

// The common start of the dunking characters' tricks: they face the goal.
void msw::CChrCoreBasket::SubStateAirTrickCmnCtor_()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    memset(pWork, 0, sizeof(STATE_WORK_AIR_TRICK));
    CChrCoreBasket::CtorStateAirTrick();

    pWork->m_nStep = 0;
    pWork->m_nFlag = 0;
    pWork->m_fMoveFrame = 0.0f;
    m_fJumpUnk220 = 0.0f;
    pWork->m_fSpeed = 1.0f;

    bcs::FVEC3 axis;
    GetAirTrickTargetAxis_(axis);
    SetRotY(GetRotY_ToPosXZ(axis));
    pWork->m_nMotId[0] = -1;
    pWork->m_nMotId[1] = -1;
    pWork->m_nMotId[2] = -1;
    pWork->m_nMotId[3] = -1;
    AllStopSPSlotSE();
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(5);
    pWork->m_fUnk2C = 8.0f;
}

// One frame of a dunking trick's flight: the character turns to the goal
// and moves along m_fRotY by the motion frames that have passed. The jump
// goes faster or slower so that its first fRate1 percent (and then the
// rest, fRate2) take half the motion each.
void msw::CChrCoreBasket::SubStateAirTrickMove_(STATE_WORK_AIR_TRICK *pWork, float fRate1, float fRate2)
{
    float fFrame = AnimGetFrame(3);
    float fStep;
    if (fFrame < pWork->m_fMoveRate) {
        fStep = fFrame + AnimGetFrameNum(3) - pWork->m_fMoveRate;
    } else {
        fStep = fFrame - pWork->m_fMoveRate;
    }
    pWork->m_fMoveRate = fFrame;
    pWork->m_fMoveFrame += fStep;
    pWork->m_fWaitFrame -= fStep;
    if (pWork->m_fWaitFrame < 0.0f) {
        pWork->m_fWaitFrame = 0.0f;
    }

    RotateYRate_(GetRotY_ToGoal_NonOffset(), 0.2f);
    float fLen = pWork->m_fMoveUnk18 * fStep;
    bcs::FVEC3 move;
    bcs::BCS_VecZ_RotY(move, 1.0f, pWork->m_fRotY);
    move.x *= fLen;
    move.z *= fLen;
    AddPos(move);

    float fJumpStep = fStep;
    if (fRate1 != 50.0f) {
        float fFrameNum = AnimGetFrameNum(3);
        float fHalf = fFrameNum * 0.5f;
        float fFirst = 0.01f * (100.0f - fRate1) * fFrameNum;
        float fSpeed;
        if (AnimGetFrame(3) <= fFirst) {
            fSpeed = fHalf / fFirst;
        } else {
            fSpeed = fHalf / (0.01f * (100.0f - fRate2) * fFrameNum);
        }
        fJumpStep = fSpeed * fStep;
    }
    Util_DoJumpMove(fJumpStep);
}

// The common dunk: hangs from the ring (16, 17), lets go and falls back
// from the goal (18) and lands (returns 1, the step is then 22).
int msw::CChrCoreBasket::SubStateAirTrickCmnDunk_(STATE_WORK_AIR_TRICK *pWork)
{
    static const float fSpeed = 1.3f;

    switch (pWork->m_nStep) {
        case STEP_DUNK_START:
            Eff_Stop(0xB);
            Eff_Stop(0xC);
            Util_DunkStart(pWork->m_nMotId[0], 10, pWork->m_fUnk2C, 1);
            pWork->m_nStep = STEP_DUNK_HANG;
            // fall through
        case STEP_DUNK_HANG: {
            if (!IsAnimEnd(3)) {
                break;
            }
            Util_DunkEnd(0);
            m_nStatusFlag &= ~0x40000000;
            m_nStatusFlag &= ~0x01000000;

            // How fast and how high the character drops back from the ring.
            float fTop = 0.1f + GetPos().y;
            float fBack = 0.0f;
            switch (m_nChrId) {
                case 0:
                    fBack = 0.07f;
                    fTop = 3.5f;
                    break;
                case 7:
                    fBack = 0.09f;
                    fTop = 3.5f;
                    break;
                case 13:
                    fBack = 0.08f;
                    fTop = 4.0f;
                    break;
                case 19:
                case 21:
                case 23:
                    fBack = 0.07f;
                    fTop = 3.5f;
                    break;
                case 20:
                case 22:
                case 24:
                    fBack = 0.07f;
                    fTop = 3.5f;
                    break;
            }
            AnimPlay(3, pWork->m_nMotId[1], 0, 8.0f, 0.0f);
            AnimSetSpeed(3, fSpeed);
            const bcs::FVEC3 &rPos = GetPos();
            Util_SetJumpParam(AnimGetFrameNum(3) / fSpeed, rPos.y, 0.0f, fTop);

            // Away from the ring, in the XZ plane.
            float fVelY = m_fJumpVelY;
            long nGoal = GetTargetGoalIdx();
            CChrManBasket::GetInstance()->GetRingCSetPos_(nGoal, GetJumpVel());
            GetJumpVel() = reinterpret_cast<VEC3_CREF>(GetPos()) - reinterpret_cast<VEC3_CREF>(GetJumpVel());
            NormalizeXZ(GetJumpVel(), GetJumpVel());
            m_fJumpVelX *= fBack;
            m_fJumpVelY = fVelY;
            m_fJumpVelZ *= fBack;

            Util_DetachEffect((ATTACH_EFFECT_TYPE)6);
            SetCollBodyPushRank_(4);
            SetCollBodyPushPower_(1);
            pWork->m_nStep = STEP_DUNK_FALL;
        }
            // fall through
        case STEP_DUNK_FALL: {
            if (!Util_DoJumpMove(GetWholeFrameRate())) {
                break;
            }
            SetCollBodyPushRank_(1);
            SetCollBodyPushPower_(5);
            AnimPlay(3, pWork->m_nMotId[2], 0, 8.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
            float fFrameNum = AnimGetFrameNum(3);
            float fRate = AnimGetFrame(3) / fFrameNum;
            // (Computed and not used. The original keeps the clamp's
            // comparisons and has the constant 2.0 in its pool here, with no
            // load of it: a guess at the dropped use, which changes nothing. Dead-comparison-class stand-in, notes/decisions.md.)
            fRate = CLAMP(fRate, 0.0f, 1.0f);
            float fUnused = 2.0f * fRate;

            long nEff = -1;
            float fSlide = 0.0f;
            switch (m_nChrId) {
                case 0:
                    fSlide = 0.4f;
                    break;
                case 3:
                case 7:
                    fSlide = 0.4f;
                    break;
                case 19:
                case 21:
                case 23:
                    fSlide = 0.4f;
                    break;
                case 20:
                case 22:
                case 24:
                    fSlide = 0.4f;
                    break;
                case 13:
                    nEff = 0xA4;
                    break;
            }
            Util_SetEffect_((SET_EFFECT_TYPE_BASKET)0);
            if (nEff != -1) {
                ArtsEff_Stop((ARTS_EFF_SLOT)0x20);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x20, nEff);
            }
            PlayLandSe(0);

            float fVelY = m_fJumpVelY;
            bcs::BCS_Vec_Normalize(GetJumpVel(), GetJumpVel());
            m_fJumpVelX *= fSlide;
            m_fJumpVelY = fVelY;
            m_fJumpVelZ *= fSlide;
            pWork->m_nStep = STEP_DUNK_END;
            return 1;
        }
    }
    return 0;
}

float msw::CChrCoreBasket::GetArtsEventRot()
{
    return GetRotY_ToPosXZ(GetTargetGoalPos());
}

// The pose of the arts event (mirrored with status bit 2).
void msw::CChrCoreBasket::DoPlayArtsEvent()
{
    int bFlip = (m_nStatusFlag >> 2) & 1;
    if (bFlip == 1) {
        AnimPlay(3, -0x7D, 0, 0.0f, 0.0f);
    } else {
        AnimPlay(3, 0x7D, 0, 0.0f, 0.0f);
    }
    AnimSetSpeed(3, 1.0f);
    Eff_CreateSimple(0xF, GetPos(), GetRotY(), 3, 0, 0);
    ReattachBall(5, 0.0f, 0);
}

// The other team's block of this character's trick is over: with bWin set
// the trick goes on (the blocker is knocked away) or, in state 0xC, the
// endure ends.
void msw::CChrCoreBasket::ResultAirTrickBattle(int bWin)
{
    switch (GetState()) {
        case STATE_AIR_TRICK: {
            if (!bWin) {
                return;
            }
            STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
            if (pWork->m_pBlocker) {
                float fRotY = pWork->m_pBlocker->GetRotY();
                fRotY += PI;
                fRotY = WrapRad(fRotY);
                pWork->m_pBlocker->MethdDamage(0, fRotY);
                pWork->m_pBlocker = 0;
            }
            pWork->m_fSpeed = 1.0f;
            AnimSetSpeed(3, 1.0f);
            pWork->m_nFlag &= ~0x40;
            pWork->m_nFlag |= 0x20;
            break;
        }
        case STATE_0C: {
            if (!bWin) {
                return;
            }
            StateWork0C *pWork = FsmqGetWork<StateWork0C>();
            if (!(pWork->m_nFlag & 0x200000)) {
                return;
            }
            SetDetach_();
            SetEndure(0);
            m_nStatusFlag &= ~0x20000000;
            pWork->m_nFlag &= ~0x200000;
            Eff_Stop(0xD);
            SE_Stop(7, 0.0f);
            PlaySePos(0xC0B, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            PlayVoice(0x25, 0);
            pWork->m_nUnk04 = 9999;
            break;
        }
        default:
            return;
    }
    // The block is answered: the endure check of the special move ends.
    CGameBasket::GetInstance()->SetCheckSpEndure_(0);
}

// The other team's goal, one unit further from the middle of the court.
void msw::CChrCoreBasket::GetAirTrickTargetAxis_(bcs::FVEC3 &rAxis) const
{
    const bcs::FVEC3 &rGoal = GetTargetGoalPos();
    rAxis = rGoal;
    if (rGoal.z > 0.0f) {
        rAxis.z += 1.0f;
    } else {
        rAxis.z -= 1.0f;
    }
}

void msw::CChrCoreBasket::CtorStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    SetCollBodyPushType_((BODY_COLL_SIZE_SET_TYPE)5, 8.0f);
    SetCollBodyPushRank_(3);
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetCondition(0, 0.0f, 0);
    m_nStatusFlag |= 0x40000000;
    SetMoveMotType(4);

    pWork->m_nStep = STEP_INIT;
    pWork->m_nUnk44 = 0;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_nUnkA0 = 0;
    pWork->m_nUnkA8 = -1;
    pWork->m_nUnkAC = -1;
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());

    CGame::ClearTeamArtsGauge(m_nTeamNo);
    CGame::TrgArtsEvent(m_nPlayerNo);
    DriverSendMessage((CChrDrv::MSG_ID)0x1E, 0, 0, 0, 0);
    CChrManBasket::GetInstance()->StartAirTrick_(this);

    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 40.0f;
    pWork->m_fTrickMotFrame = 40.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam2_()->m_fArtsWaitFrame + GetCcParam2_()->m_fArtsFallFrame;

    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->SetDisableMarker(1);
        CGame::SendMessage(0x34, m_nPlayerNo, GetArtsShootType(), pBall->GetBallNo());
    }
}

void msw::CChrCoreBasket::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    int bEnd = 0;

    SubStateArtsShotAskTargetMarkerMove_(pWork);
    switch (pWork->m_nStep) {
        case STEP_INIT:
            pWork->m_nStep = STEP_START_TRICK;
            break;
        case STEP_START_TRICK:
            Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
            SubStateArtsShotStartTrick_(pWork);
            pWork->m_nStep = STEP_TRICK;
            break;
        case STEP_TRICK:
            if (SubStateArtsShotIsReadyArtsShot_(pWork)) {
                SubStateArtsShotReadyArtsShot_(pWork);
                pWork->m_nStep = STEP_READY;
            }
            break;
        case STEP_READY:
            if (SubStateArtsShotIsStartArtsShot_(pWork)) {
                SubStateArtsShotStartArtsShot_(pWork);
                pWork->m_nStep = STEP_SHOT;
            }
            break;
        case STEP_SHOT:
            if (SubStateArtsShotIsEndArtsShot_(pWork)) {
                SubStateArtsShotStartArtsHitback_(pWork);
                pWork->m_nStep = STEP_HITBACK;
            }
            break;
        case STEP_HITBACK:
            if (SubStateArtsShotIsEndArtsShotAfter_(pWork)) {
                SubStateArtsShotDoLandStart_(pWork);
            }
            break;
        case 6:
        case 7:
        case 9:
            break;
        case STEP_HITBACK_ALT:
            if (SubStateArtsShotIsEndArtsShotAfter_(pWork)) {
                SubStateArtsShotDoLandStart_(pWork);
            }
            break;
        case STEP_LAND:
            if (!IsAnimEnd(3)) {
                break;
            }
            // the end of the landing motion ends the state
        default:
            bEnd = 1;
            break;
    }
    if (bEnd) {
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateAirTrick()
{
    Eff_StopGroup(4);
    SE_StopGroup(1, 0.0f);
    m_nStatusFlag &= ~0x800;
    ArtsOptDestroy();
    m_nStatusFlag &= ~0x40000000;
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->SetDisableMarker(0);
    }
    ArtsEff_StopAll();
    AllStopSPSlotSE();
}

// With flag 0x200 the driver is asked where the shot should aim (the answer
// comes as m_fTargetRotY).
void msw::CChrCoreBasket::SubStateArtsShotAskTargetMarkerMove_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork->m_nFlag & 0x200) {
        pWork->m_fTargetRotY = -1.0f;
        DriverSendMessage((CChrDrv::MSG_ID)0x28, 0, 0, 0, 0);
    }
}

void msw::CChrCoreBasket::SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 60.0f;
    pWork->m_fTrickMotFrame = 60.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam2_()->m_fArtsWaitFrame + GetCcParam2_()->m_fArtsFallFrame;
    pWork->m_fTargetRotY = -1.0f;

    SubStateArtsShotSetTrickFrameParam_(pWork, 0);
    SubStateArtsShotPlayTrickMotion_(pWork);
    SubStateArtsShotSetTrickJumpParam_(pWork, 0);
    SubStateArtsShotSetTrickMoveParam_(pWork, 0);

    // Turns to face the goal from where the trick moves to.
    bcs::FVEC3 dir(GetTargetGoalPos());
    dir.x -= pWork->m_TargetPos.x;
    dir.z -= pWork->m_TargetPos.z;
    pWork->m_fRotY = WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z));
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, SubStateArtsShotGetTrickRotFrame_(pWork));
    PlayVoice(0x23, 0);
}

// The trick lasts the frames the character's parameters give it; its motion
// is played faster or slower to fit them.
void msw::CChrCoreBasket::SubStateArtsShotSetTrickFrameParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    float fMotFrame = SubStateArtsShotGetTrickMotFrame_(pWork, nTrick);
    fMotFrame = Max(1.0f, fMotFrame - GetCcParam2_()->m_fArtsTrickMotCut);
    pWork->m_fTrickMotFrame = fMotFrame;
    float fFrame = SubStateArtsShotGetTrickDoFrame_(pWork, nTrick);
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
        pWork->m_fAnimSpeed = 1.0f;
    } else {
        pWork->m_fAnimSpeed = fMotFrame / fFrame;
    }
    pWork->m_fTrickFrame = fFrame;
    pWork->m_fTrickCount = 0.0f;
}

// A turn to fRot over fFrame frames, the short way round.
void msw::CChrCoreBasket::SubStateArtsShotCalcRotParam_(ROT_LERP1 &rLerp, float fRot, float fFrame)
{
    rLerp.m_afUnk[0] = WrapRad(fRot);
    rLerp.m_afUnk[2] = fFrame;
    rLerp.m_afUnk[2] = Max(rLerp.m_afUnk[2], 1.0f);
    float fDiff = WrapRad(rLerp.m_afUnk[0] - GetRotY());
    if (fDiff > PI) {
        fDiff -= PI2;
    }
    rLerp.m_afUnk[3] = 0.0f;
    rLerp.m_afUnk[1] = fDiff / rLerp.m_afUnk[2];
}

void msw::CChrCoreBasket::SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam)
{
    rParam.m_nType = 2;
    rParam.m_fFrame = 0.0f;
    rParam.m_fFrameMax = 0.0f;
    rParam.m_fDist = 0.0f;
}

// The jump, with the turn and the move of the trick while it rises.
int msw::CChrCoreBasket::SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK *pWork)
{
    int nResult = Util_DoJumpMoveEX(0);
    if (nResult == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnk58 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnk58 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnk58, pWork->m_DirMove, 0);
        }
    }
    return nResult;
}

float msw::CChrCoreBasket::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x7E);
}

float msw::CChrCoreBasket::SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return GetCcParam2_()->m_fArtsTrickFrame;
}

void msw::CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    AnimPlay(3, 0x7E, 0, GetCcParam2_()->m_fArtsTrickBlend, 0.0f);
    AnimSetSpeed(3, pWork->m_fAnimSpeed);
    ArtsOptAnimSetSpeed(pWork->m_fAnimSpeed);
    SE_PlayArtsJumpSE(0);
}

float msw::CChrCoreBasket::SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return Min(16.0f, pWork->m_fTrickFrame);
}

// Where the trick moves the character: away from the goal, further the
// further out he stands (up to the three-point line). Standing to the side
// of the goal, he is turned towards the line in front of it, by up to the
// parameter's angle.
void msw::CChrCoreBasket::SubStateArtsShotCalcSetbackPos_(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 goal(GetTargetGoalPos());
    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 dir;
    bcs::FVEC3 front;
    bcs::FVEC3 cross;
    rPos = pos;

    float fDist = DistXZ(goal, pos);
    float f3Point = GetCourt3PointSize();
    float fRate = 1.0f;
    if (f3Point > 0.0f) {
        fRate = fDist / f3Point;
        fRate = CLAMP(fRate, 0.0f, 1.0f);
    }
    float fMin = GetCcParam2_()->m_fArtsSetbackMin;
    float fLen = fMin + fRate * (GetCcParam2_()->m_fArtsSetbackMax - fMin);

    dir.x = pos.x - goal.x;
    dir.z = pos.z - goal.z;
    NormalizeXZ(dir, dir);

    bcs::BCS_VecOneZ_RotY(front, GetTargetGoalYRot());
    float fAngle = bcs::DEG_TO_RAD(GetCcParam2_()->m_fArtsSetbackRotDeg);
    if (fDist < GetCcParam2_()->m_fArtsSetbackRotDist) {
        fAngle = 0.0f;
    }
    float fDot = front.x * dir.x + front.z * dir.z;
    if (fDot < nw4r::math::CosRad(fAngle)) {
        PSVECCrossProduct((const Vec *)&front, (const Vec *)&dir, (Vec *)&cross);
        if (cross.y < 0.0f) {
            fAngle = -fAngle;
        }
        bcs::BCS_Vec_RotY(dir, front, WrapRad(fAngle));
        float fScale = fDist + fLen;
        dir.x *= fScale;
        dir.z *= fScale;
        rPos.x = goal.x + dir.x;
        rPos.z = goal.z + dir.z;
    } else {
        dir.x *= fLen;
        dir.z *= fLen;
        rPos.x = pos.x + dir.x;
        rPos.z = pos.z + dir.z;
    }
}

void msw::CChrCoreBasket::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    float fTop = GetCcParam2_()->m_fArtsJumpHeight;
    float fFrame = GetCcParam2_()->m_fArtsJumpTopFrame;
    float fY = GetPosY();
    fTop = Max(fTop, fY);
    fFrame = Max(1.0f, pWork->m_fTrickFrame - fFrame);
    Util_SetJumpParam(fFrame, fY, fTop, fTop);
    float fUnk224 = pWork->m_fJumpUnk + (pWork->m_fTrickFrame - fFrame);
    m_fJumpUnk228 = 0.0f;
    m_fJumpUnk224 = fUnk224;
}

// The trick moves the character back to a place away from the goal.
void msw::CChrCoreBasket::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    SubStateArtsShotCalcSetbackPos_(pWork->m_TargetPos);
    SubStateArtsShotClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk58 = 0.0f;

    float fDistSq = DistSqXZ(pWork->m_TargetPos, GetPos());
    if (fDistSq > 0.0f) {
        float fFrame = Max(1.0f, pWork->m_fTrickFrame - GetCcParam2_()->m_fArtsMoveEndFrame);
        pWork->m_MoveParam.m_nType = 0xB;
        pWork->m_MoveParam.m_fDist = nw4r::math::FSqrt(fDistSq);
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = fFrame;
    }
}

// Waits for the trick to end; the loop motion follows the trick motion.
int msw::CChrCoreBasket::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
    SubStateArtsShotDoJumpMove_(pWork);
    if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
        return 1;
    }
    if (pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    return 0;
}

// The shot starts when the driver answers 4 (it is told the frames left and
// in all) or the time runs out.
int msw::CChrCoreBasket::SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bStart = 0;
    long nFrame = GetCcParam2_()->m_fArtsWaitFrame + GetCcParam2_()->m_fArtsFallFrame;
    int bAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fJumpUnk, nFrame, 0, 0) == 4;
    if (pWork->m_fJumpUnk > 0.0f) {
        pWork->m_fJumpUnk -= m_fFrameStep;
    }
    if (bAnswer || pWork->m_fJumpUnk <= 0.0f) {
        bStart = 1;
    }
    SubStateArtsShotDoJumpMove_(pWork);
    if (!bStart && pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    return bStart;
}

void msw::CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0x8D, 1, GetCcParam2_()->m_fArtsLoopBlend, 0.0f);
}

void msw::CChrCoreBasket::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
}

// The character hangs in the air at the top of the jump, with the attack
// sign, then falls slowly while he waits for the shot.
void msw::CChrCoreBasket::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    ArtsEff_PlayAttackSign();
    SE_PlayArtsLoopSE();

    float fUnk224 = GetCcParam2_()->m_fArtsWaitFrame;
    float fFrame = GetCcParam2_()->m_fArtsFallFrame;
    float fY = GetPosY();
    Util_SetJumpParam(fFrame, fY, fY - GetCcParam2_()->m_fArtsFallHeight, fY);
    m_fJumpUnk224 = fUnk224;
    m_fJumpUnk228 = 0.0f;
}

void msw::CChrCoreBasket::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 8.0f);
    SubStateArtsShotPlayArtsShotMotion_(pWork);
    SubStateArtsShotDoGetBallShotTrg_(pWork);
}

void msw::CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    float fBlend = GetCcParam2_()->m_fArtsShotBlend;
    if (pWork->m_nUnkA0 < 1) {
        fBlend = GetCcParam2_()->m_fArtsShotBlendNoLoop;
    }
    AnimPlay(3, 0x83, 0, fBlend, 0.0f);
}

// The ball leaves at trigger 2 of the shot motion, or at frame 16 without one.
void msw::CChrCoreBasket::SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK *pWork)
{
    float fFrame = AnimSearchTrgFrame(AnimGetMotID(3, 0), 2, -1.0f);
    if (fFrame < 0.0f) {
        fFrame = 16.0f;
    }
    pWork->m_fShotFrame = fFrame;
}

int msw::CChrCoreBasket::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
    if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
        Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
        SubStateArtsShotShootBall_(pWork);
        pWork->m_nUnkA0 = 0;
        return 1;
    }
    return 0;
}

// Shoots the ball as a special shot (kind 9), with the camera quake of the
// work when it has one.
void msw::CChrCoreBasket::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        static_cast<CBallBasket *>(pBall)->SetArtsShotUnk((1.0f / 60.0f) * GetCcParam2_()->m_fArtsBallUnk240);
        Util_SendMsgToAll((CChrDrv::MSG_ID)7, m_nPlayerNo, 9, 0, 0);
        Util_ShootBall(9, 1.0f, 1, DriverSendMessage((CChrDrv::MSG_ID)0x2D, 0, 0, 0, 0));
        PlayVoice(0x24, 0);
        SE_PlayArtsShootSE(pBall->GetPos());
        if (pWork->m_nUnkA8 >= 0) {
            CGame::StartCameraQuake(pWork->m_nUnkA8, pWork->m_nUnkAC, -1);
        }
    }
}

// The shot pushes the character back: a jump backwards, away from the
// direction he shot in.
void msw::CChrCoreBasket::SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = GetCcParam2_()->m_fArtsHitbackFrame;
    pWork->m_MoveParam.m_fDist = GetCcParam2_()->m_fArtsHitbackDist;
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk58 = 0.0f;
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, GetCcParam2_()->m_fArtsHitbackFrame);

    bcs::BCS_VecZ_RotY(pWork->m_TargetPos, -2.0f * GetCcParam2_()->m_fArtsHitbackDist, pWork->m_fRotY);
    const bcs::FVEC3 &rPos = GetPos();
    pWork->m_TargetPos.x += rPos.x;
    pWork->m_TargetPos.z += rPos.z;

    Util_SetJumpParam(GetCcParam2_()->m_fArtsHitbackJump0, GetCcParam2_()->m_fArtsHitbackJump1,
                      GetCcParam2_()->m_fArtsHitbackJump2);
}

int msw::CChrCoreBasket::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    return SubStateArtsShotDoJumpMove_(pWork) >= 2;
}

void msw::CChrCoreBasket::SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SubStateArtsShotPlayLandMotion_(pWork);
    SubStateArtsShotPlayLandEffect_(pWork);
    pWork->m_nStep = STEP_LAND;
}

void msw::CChrCoreBasket::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0x8C, 0, GetCcParam2_()->m_fArtsLandBlend, 0.0f);
}

void msw::CChrCoreBasket::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    SE_PlayArtsLandSE();
    Util_SetEffect((SET_EFFECT_TYPE)0);
}

// No target marker in basketball.
int msw::CChrCoreBasket::GetTargetMarkerPos(bcs::FVEC3 &)
{
    return 0;
}

int msw::CChrCoreBasket::MoveTargetMarkerPos(float)
{
    return 0;
}
