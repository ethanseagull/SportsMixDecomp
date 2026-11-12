// msw::CChrCoreBasket's basic states: neutral and run (with the block work,
// the defence stance against the ball holder), the change of body size,
// the jump, the item shot, the motion play, the turn, the dash, the block
// work lost to a dribble and the sliding; and the battle and B-spline moves
// of the special moves.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

// The motions of the block work, by the direction of the move
// (GetDir8ToBlockWork: 0 forward, then round by 45 degrees).
static const short s_BlockWorkMot[8] = { 0x30, 0x2E, 0x2E, 0x2E, 0x2F, 0x2D, 0x2D, 0x2D };

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// The neutral state's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_NEUTRAL {
    long m_nStep;              // 0x00
    long m_nIdleLoop;          // 0x04: loops of the stop motion (0x77) left
    float m_fRotY;             // 0x08: the yaw to turn to while it plays
    float m_fFrame;            // 0x0C: the frame the run motion had reached
    int m_bBlockWork;          // 0x10: in the block work
    int m_bLookBall;           // 0x14: facing the ball holder
    CChrCore::DEF_COLL m_Def;  // 0x18
};

// The run state's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_RUN {
    long m_nStep;             // 0x00
    long m_nDir;              // 0x04: the direction of the block work move
    long m_nMot;              // 0x08: the motion of the block work
    // 0x0C: bit 0 the stick is held this frame, bit 1 in the block work,
    // bit 2 the block work's speed is still rising, bit 3 facing the ball
    // holder, bit 4 the run smoke is out, bit 5 the side step motion is to
    // be chosen again.
    unsigned long m_nFlag;
    long m_nIdleLoop;         // 0x10: loops of the stop motion to play
    float m_fStopWait;        // 0x14: frames before the run stops
    float m_fRotYTrg;         // 0x18: the yaw the stick asks for
    float m_fRotY;            // 0x1C: the yaw of the run
    float m_fSpeedRate;       // 0x20
    float m_fUnk24;           // 0x24
    float m_fAccelFrame;      // 0x28: frames of the block work's start
    float m_fSmokeWait;       // 0x2C: frames before the run smoke
    float m_fUnk30;           // 0x30
    float m_fBlockFrame;      // 0x34: frames in the block work
    float m_fStartFrame;      // 0x38: the frame the run motion starts at
    CChrCore::DEF_COLL m_Def; // 0x3C
};

namespace {

    // (guessed helper) An angle brought into [-pi, pi].
    inline float WrapRadPi(float rad)
    {
        rad = bcs::ROUND_PI(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) An angle brought into [-pi, pi) by whole turns.
    inline float WrapRadPiHalfOpen(float rad)
    {
        rad = bcs::ROUND_PI(rad);
        if (rad >= 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) An angle brought into [-pi, pi] step by step.
    inline float WrapRadPiStep(float rad)
    {
        while (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        while (rad < -3.1415927f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // (guessed helpers) Radians to degrees and back.
    inline float RadToDeg(float rad)
    {
        return 57.295776f * rad;
    }

    inline float DegToRad(float deg)
    {
        return 0.017453292f * deg;
    }

    inline long AbsL(long n)
    {
        return n < 0 ? -n : n;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    // (guessed helper) A turn of fDiff cut down to fStep (both signed the
    // same way).
    inline float LimitTurn(float fDiff, float fStep)
    {
        if (fDiff < 0.0f) {
            return Max(fStep, fDiff);
        }
        return Min(fStep, fDiff);
    }

    inline float Clamp(float x, float fMin, float fMax)
    {
        return x > fMax ? fMax : (x < fMin ? fMin : x);
    }

    // (guessed helper) The yaw the block work looks in: CalcBWLookDir_,
    // turned by up to 30 degrees towards the ball holder when he is more
    // than 50 degrees off it.
    inline float GetBlockWorkLookDir(const msw::CChrCoreBasket *pChr)
    {
        float fRotY = pChr->CalcBWLookDir_();
        const msw::CChrCore *pHave = msw::CChrMan::Util_SearchHaveBallChr(0);
        if (pHave != 0) {
            float fDiff = WrapRadPi(pChr->GetRotY_ToPosXZ(pHave->GetPos()) - fRotY);
            float fDeg = RadToDeg(fDiff);
            if (__fabsf(fDeg) > 50.0f) {
                fRotY += DegToRad(Clamp(fDeg, -30.0f, 30.0f));
                fRotY = bcs::ROUND_PI(fRotY);
            }
        }
        return fRotY;
    }

    // (guessed helper) The block work motion: mirrored when the holder of
    // the ball is to the right of the look direction.
    inline long GetBlockWorkMot(const msw::CChrCoreBasket *pChr)
    {
        long nMot = 0x2C;
        const msw::CChrCore *pHold = msw::CChrMan::Util_SearchHoldBallChr(0);
        if (pHold != 0) {
            float fRotY = GetBlockWorkLookDir(pChr);
            float fDiff = WrapRadPi(pChr->GetRotY_ToPosXZ(pHold->GetPos()) - fRotY);
            if (fDiff < 0.0f) {
                nMot = -0x2C;
            }
        }
        return nMot;
    }
}

long msw::CChrCoreBasket::GetMotID_ItemShoot(long, int)
{
    return 0x2B;
}

void msw::CChrCoreBasket::CtorStateNeutral()
{
    if (MethdFall_()) {
        FsmqRetry();
        return;
    }
    if (m_nComLevel == 4) {
        MethdRun(GetRotY());
        FsmqRetry();
        return;
    }
    SetPosY(0.0f);

    if ((m_nStatusFlag & 1) && !(m_nStatusFlag & 0x8000)) {
        // Holding the ball: the dribble stance.
        SetCollBodyPushPower_(5);
        m_nStatusFlag &= ~0x1F000000;
        m_nStatusFlag &= ~0x44;
        m_UnkEC |= 0x10;
        AnimPlay(3, IsHaveLeft() == 1 ? -0x45 : 0x45, 1, 8.0f, 0.0f);
        ReattachBall(5, 0.0f, 0);
        FsmqClear();
        Util_PushBackCatch(1);
        SetMoveMotType(3);
        FsmqRetry();
        return;
    }

    STATE_WORK_NEUTRAL *pWork = FsmqGetWork<STATE_WORK_NEUTRAL>();
    pWork->m_nStep = 0;
    pWork->m_Def.m_nStep = 0;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    m_nBasketFlag &= ~0x4000;
    if (SubStateNeedBlockWork_(pWork->m_bBlockWork, 0.0f)) {
        pWork->m_bBlockWork = 1;
        SetCollBodyPushPower_(5);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
        AnimPlay(3, GetBlockWorkMot(this), 1, 4.0f, 0.0f);
        SetCollDefParam_(&pWork->m_Def, 3);
    } else {
        pWork->m_bBlockWork = 0;
        SetCollBodyPushPower_(5);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
        m_fUnk274 = 0.0f;
        if (AnimGetMotID(3, 0) != 0x77) {
            AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
            pWork->m_nIdleLoop = 0;
        }
        SetCollDefParam_(&pWork->m_Def, 1);
    }
    pWork->m_bLookBall = 0;
    m_fJumpVelX = 0.0f;
    m_fJumpVelY = 0.0f;
    m_fJumpVelZ = 0.0f;
}

void msw::CChrCoreBasket::LoopStateNeutral()
{
    STATE_WORK_NEUTRAL *pWork = FsmqGetWork<STATE_WORK_NEUTRAL>();

    if (pWork->m_bBlockWork) {
        if (!SubStateNeedBlockWork_(1, m_fFrameStep * m_fTimeScale)) {
            pWork->m_bBlockWork = 0;
            SetCollBodyPushPower_(5);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            m_fUnk274 = 0.0f;
            AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
            SetCollDefParam_(&pWork->m_Def, 1);
        } else {
            long nMot = GetBlockWorkMot(this);
            long nNow = AnimGetMotIDEx(3);
            if (nMot != nNow) {
                // (the same motion mirrored once kept its frame, it seems;
                // both ways start it from 0 here)
                float fFrame = 0.0f;
                if (AbsL(nMot) == AbsL(nNow)) {
                    fFrame = 0.0f;
                }
                AnimPlay(3, nMot, 1, 4.0f, fFrame);
            }
        }
    } else if (SubStateNeedBlockWork_(0, 0.0f)) {
        PlaySePos(0xC19, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        m_fBlockWorkWait = 20.0f;
        pWork->m_bBlockWork = 1;
        SetCollBodyPushPower_(5);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
        AnimPlay(3, GetBlockWorkMot(this), 1, 4.0f, 0.0f);
        SetCollDefParam_(&pWork->m_Def, 3);
    }

    if (!pWork->m_bBlockWork) {
        pWork->m_bLookBall = 0;
        if (AnimGetMotID(3, 0) == 0x77) {
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimLoop(3)) {
                if (--pWork->m_nIdleLoop <= 0) {
                    pWork->m_nIdleLoop = 0;
                    AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                }
            }
        } else if (Util_GetIdleMotID() != AnimGetMotID(3, 0)) {
            AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
        }
    } else {
        if (!pWork->m_bLookBall) {
            CChrCore *pHave = CChrMan::Util_SearchHaveBallChr(0);
            if (pHave != 0) {
                float fRotY = CalcBWLookDir_();
                float fDiff = WrapRadPi(GetRotY_ToPosXZ(pHave->GetPos()) - fRotY);
                if (__fabsf(fDiff) <= 0.7853982f) {
                    pWork->m_bLookBall = 1;
                }
            }
        }
        RotateYRate_(GetBlockWorkLookDir(this), 0.2f);
    }
    CalcPush1Zero();
}

void msw::CChrCoreBasket::DtorStateNeutral()
{
    m_nBasketFlag &= ~0x4000;
}

void msw::CChrCoreBasket::CtorStateRun()
{
    static const float EFF_SMOKE_ENABLE_FRAME = 10.0f;
    STATE_WORK_RUN *pWork = FsmqGetWork<STATE_WORK_RUN>();

    pWork->m_Def.m_nStep = 0;
    m_UnkEC |= 0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetCollBodyPushRank_(1);
    m_nBasketFlag &= ~0x4000;
    if (SubStateNeedBlockWork_((pWork->m_nFlag >> 1) & 1, 0.0f)) {
        if (!(pWork->m_nFlag & 2)) {
            PlaySePos(0xC19, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        }
        pWork->m_nFlag |= 2;
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(0);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
        SetMoveMotType(2);
        SetCollDefParam_(&pWork->m_Def, 4);
        pWork->m_nStep = 0;
        pWork->m_nFlag |= 0x10;
    } else {
        pWork->m_nFlag &= ~2;
        SetCollBodyPushPower_(1);
        SetCollBodyPushRank_(1);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
        m_fUnk274 = 0.0f;
        m_fJumpUnk220 = 0.0f;
        SetCollDefParam_(&pWork->m_Def, 2);
        m_fPush1GapX = 0.0f;
        m_fPush1GapY = 0.0f;
        m_fPush1GapZ = 0.0f;
        pWork->m_fSmokeWait = EFF_SMOKE_ENABLE_FRAME;
    }
    pWork->m_fBlockFrame = 0.0f;
    pWork->m_fRotY = pWork->m_fRotYTrg;
    pWork->m_nFlag |= 0x20;
}

void msw::CChrCoreBasket::LoopStateRun()
{
    STATE_WORK_RUN *pWork = FsmqGetWork<STATE_WORK_RUN>();

    // The run stops a few frames after the stick is let go.
    if (!(pWork->m_nFlag & 1)) {
        pWork->m_fStopWait -= m_fFrameStep;
        if (pWork->m_fStopWait < 0.0f) {
            pWork->m_fStopWait = 0.0f;
        }
    } else {
        pWork->m_fStopWait = 4.0f;
    }

    int bBlockWork = (pWork->m_nFlag >> 1) & 1;
    if (bBlockWork) {
        if (!SubStateNeedBlockWork_(1, m_fFrameStep * m_fTimeScale)) {
            pWork->m_nFlag &= ~2;
            SetCollBodyPushPower_(0);
            SetCollBodyPushRank_(0);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
            m_fUnk274 = 0.0f;
            bBlockWork = 0;
            m_fJumpUnk220 = 0.0f;
            m_nStatusFlag &= ~0x20;
            pWork->m_nStep = 1;
            SetCollDefParam_(&pWork->m_Def, 2);
        }
    } else if (SubStateNeedBlockWork_(0, 0.0f)) {
        PlaySePos(0xC19, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        m_fBlockWorkWait = 20.0f;
        pWork->m_nFlag |= 2;
        SetMoveMotType(2);
        m_nStatusFlag &= ~0x20;
        bBlockWork = 1;
        pWork->m_nStep = 0;
        SetCollDefParam_(&pWork->m_Def, 4);
        pWork->m_nFlag |= 0x20;
    }

    if (bBlockWork) {
        pWork->m_fBlockFrame += GetWholeFrameRate();
        if (pWork->m_fBlockFrame >= 9999.9f) {
            pWork->m_fBlockFrame = 9999.9f;
        }
        if (!(pWork->m_nFlag & 8)) {
            CChrCore *pHave = CChrMan::Util_SearchHaveBallChr(0);
            if (pHave != 0) {
                float fRotY = CalcBWLookDir_();
                float fDiff = WrapRadPi(GetRotY_ToPosXZ(pHave->GetPos()) - fRotY);
                if (__fabsf(fDiff) <= 0.7853982f) {
                    pWork->m_nFlag |= 8;
                }
            }
        }
        if (!(pWork->m_nFlag & 1) && pWork->m_fStopWait <= 0.0f) {
            // Back to the neutral state, still in the block work.
            if (FsmqGetNextState() == 0) {
                STATE_WORK_NEUTRAL *pNext = static_cast<STATE_WORK_NEUTRAL *>(FsmqPushState(STATE_NEUTRAL));
                pNext->m_bBlockWork = 1;
            }
            FsmqDoChain();
        } else {
            RotateYRate_(GetBlockWorkLookDir(this), 0.2f);
            SubStateRunBlockWorkMove_(pWork);
            pWork->m_nFlag &= ~1;
        }
        if (pWork->m_fBlockFrame >= 5.0f) {
            CalcPush1GapValue(0.06f * GetWholeFrameRate(), pWork->m_fRotY);
        } else {
            CalcPush1GapValue(0.15f * GetWholeFrameRate(), pWork->m_fRotY);
        }
        SetCollBodyPushPower_(1);
        SetCollBodyPushRank_(2);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);

        // A side step motion when the stick turns sideways to the look
        // direction.
        long nMot = -1;
        float fDiff = WrapRadPi(pWork->m_fRotYTrg - pWork->m_fRotY);
        if (__fabsf(fDiff) >= 0.5235988f) {
            pWork->m_nFlag |= 0x20;
        }
        if (pWork->m_nFlag & 0x20) {
            float fLook = GetBlockWorkLookDir(this);
            float fSide = WrapRadPi(pWork->m_fRotYTrg - fLook);
            float fAbs = __fabsf(fSide);
            if (!(fAbs <= 0.7853982f) && !(fAbs >= 2.3561945f)) {
                if (fSide > 0.0f) {
                    nMot = 0x32;
                } else {
                    nMot = 0x33;
                }
            }
            pWork->m_nFlag &= ~0x20;
        }
        long nNow = AnimGetMotIDEx(3);
        switch (nNow) {
            case 0x32:
            case -0x32:
                if (nMot == nNow) {
                    nMot = -1;
                }
                break;
            case 0x33:
            case -0x33:
                if (nMot == nNow) {
                    nMot = -1;
                }
                break;
        }
        if (nMot != -1) {
            pWork->m_nMot = nMot;
            AnimPlay(3, nMot, 0, 4.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
        }
    } else {
        SetCollBodyPushPower_(0);
        SetCollBodyPushRank_(0);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
        pWork->m_fBlockFrame = 0.0f;
        m_fPush1GapX = 0.0f;
        m_fPush1GapY = 0.0f;
        m_fPush1GapZ = 0.0f;
        if (!SubStateRunNormal_(pWork)) {
            return;
        }
        pWork->m_nFlag &= ~8;
    }

    if (!(pWork->m_nFlag & 0x10)) {
        pWork->m_fSmokeWait -= GetWholeFrameRate();
        if (pWork->m_fSmokeWait <= 0.0f) {
            Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
            pWork->m_nFlag |= 0x10;
        }
    }
    AnimSetSpeed(3, GetConditionStarMotSpeed());
    if (m_nComLevel != 4) {
        pWork->m_nFlag &= ~1;
    }
}

void msw::CChrCoreBasket::DtorStateRun()
{
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
    m_nBasketFlag &= ~0x4000;
}

int msw::CChrCoreBasket::SubStateRunNormal_(STATE_WORK_RUN *pWork)
{
    static const float fMaxSpeed = 1.0f;

    if (!(pWork->m_nFlag & 1) && pWork->m_fStopWait <= 0.0f) {
        // The stick is let go: back to the neutral state, with the stop
        // motion when the run had it.
        FsmqClear();
        long nLoop = pWork->m_nIdleLoop;
        float fFrame = AnimGetFrame(3);
        STATE_WORK_NEUTRAL *pNext = static_cast<STATE_WORK_NEUTRAL *>(FsmqSetState(STATE_NEUTRAL));
        pNext->m_nIdleLoop = 0;
        pNext->m_bBlockWork = 0;
        pNext->m_bLookBall = 0;
        switch (AnimGetMotID(3, 0)) {
            case 0x78: {
                fFrame = 0.0f;
                int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
                long nMot = -0x77;
                if (!bFlip) {
                    nMot = 0x77;
                }
                AnimPlay(3, nMot, 1, 4.0f, AnimGetFrame(3));
                pNext->m_nIdleLoop = nLoop;
                pNext->m_fRotY = GetRotY();
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall != 0) {
                    pNext->m_fRotY = GetRotY_ToPosXZ(pBall->GetPos());
                }
                break;
            }
            case 2:
                break;
            default:
                fFrame = 0.0f;
                break;
        }
        pNext->m_fFrame = fFrame;
        return 0;
    }

    pWork->m_fSpeedRate = 1.0f;
    switch (pWork->m_nStep) {
        case 0:
        case 1: {
            long nMot = Util_GetRunMotID();
            float fFrame = pWork->m_fStartFrame;
            if (AnimGetMotID(3, 0) == 0x77) {
                nMot = 0x78;
                fFrame = AnimGetFrame(3);
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall != 0) {
                    float fRotY = GetRotY();
                    if (WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fRotY) < 0.0f) {
                        nMot = -0x78;
                    }
                }
            }
            pWork->m_fStartFrame = 0.0f;
            if (nMot != AnimGetMotID(3, 0)) {
                AnimPlay(3, nMot, 1, 4.0f, fFrame);
            }
            SetMoveMotType(2);
            pWork->m_nStep = 2;
            break;
        }
        case 2: {
            long nNow = AnimGetMotIDEx(3);
            if (nNow == 0x78 || nNow == -0x78) {
                if (IsAnimLoop(3)) {
                    if (--pWork->m_nIdleLoop <= 0) {
                        pWork->m_nIdleLoop = 0;
                        AnimPlay(3, Util_GetRunMotID(), 1, 4.0f, 0.0f);
                    }
                } else {
                    const CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
                    if (pHold != 0) {
                        long nMot = 0x78;
                        float fRotY = GetRotY();
                        if (WrapRadPi(GetRotY_ToPosXZ(pHold->GetPos()) - fRotY) < 0.0f) {
                            nMot = -0x78;
                        }
                        if (nNow != nMot) {
                            AnimPlay(3, nMot, 1, 4.0f, AnimGetFrame(3));
                        }
                    }
                }
            } else {
                long nMot = Util_GetRunMotID();
                if (nNow != nMot) {
                    AnimPlay(3, nMot, 1, 4.0f, 0.0f);
                }
            }
            break;
        }
    }
    AnimSetSpeed(3, Clamp(fMaxSpeed / m_fScaleBase, 1.0f, 2.0f));

    // Steering; the weaving of conditions 4 and 7.
    float fRotY = pWork->m_fRotYTrg;
    if (m_nComLevel == 4 || m_nComLevel == 7) {
        fRotY += m_fWeaveWidth * nw4r::math::SinRad(m_fWeavePhase);
        fRotY = bcs::ROUND_PI(fRotY);
    }
    if (m_nComLevel == 4) {
        float fDiff = WrapRadPiStep(fRotY - pWork->m_fRotY);
        float fDeg = 4.0f;
        float fMax = DegToRad(fDeg);
        float fStep = Clamp(fDiff, -fMax, fMax) * m_fFrameStep;
        fDiff = LimitTurn(fDiff, fStep);
        pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + fDiff);
        fRotY = pWork->m_fRotY;
    } else {
        float fDiff = WrapRadPiHalfOpen(fRotY - pWork->m_fRotY);
        float fStep = m_fTimeScale * (0.5f * fDiff * m_fFrameStep);
        fDiff = LimitTurn(fDiff, fStep);
        pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + fDiff);
    }
    RotateYRate_(fRotY, 0.2f);

    float fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(0));
    MoveRunXZ_(pWork->m_fRotY, fSpeed, pWork->m_fSpeedRate);
    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
    m_fJumpVelX = dir.x * fSpeed;
    m_fJumpVelZ = dir.z * fSpeed;
    m_fJumpUnk220 += m_fFrameStep;
    return 1;
}

namespace {

    // The turn state's work (names guessed).
    struct STATE_WORK_TURN {
        long m_nStep;                               // 0x00
        long m_nMot;                                // 0x04: the motion of the turn
        // 0x08: bit 0 a turn from the block work, bit 1 the yaw follows
        // the motion, bits 2 and 3 its effects are out, bit 5 a quick turn.
        unsigned long m_nFlag;
        float m_fFrame;                             // 0x0C
        float m_fFrameNum;                          // 0x10
        float m_fRotYEnd;                           // 0x14: the yaw the stick asks for at the end
        float m_fMoveRotY;                          // 0x18: the direction of the move
        float m_fRotY;                              // 0x1C: the yaw to turn to
        float m_fUnk20;                             // 0x20
        float m_fTurn;                              // 0x24: the turn in all
        float m_fStartRotY;                         // 0x28
        unsigned char m_Unk2C[0x34 - 0x2C];
        msw::CC_DATA::CHR_PARAM::MOVE_PARAM m_Move; // 0x34
        msw::CChrCore::DIR_MOVE m_DirMove;          // 0x44
        msw::CChrCore::DEF_COLL m_Def;              // 0x54
    };
}

void msw::CChrCoreBasket::SubStateRunBlockWorkMove_(STATE_WORK_RUN *pWork)
{
    static const float fDown = 0.0058333334f;

    long nDir = GetDir8ToBlockWork(bcs::ROUND_PI(GetRotY() - pWork->m_fRotYTrg));
    long nMotNow = pWork->m_nMot;
    if (pWork->m_nStep == 0) {
        pWork->m_nStep++;
        nMotNow = -1;
    }
    long nMot = s_BlockWorkMot[nDir];
    if (((nMotNow >= 0x32 && nMotNow <= 0x33) || (nMotNow >= -0x33 && nMotNow <= -0x32)) && !IsAnimEnd(3)) {
        // A side step motion (0x32, 0x33, mirrored or not) plays to its
        // end.
        nMot = nMotNow;
    } else if (nMotNow != nMot) {
        if (nMot >= 0x2F && nMot <= 0x30 && m_pMdl->IsAnmFlipPlay(3, 0)) {
            nMot = -nMot;
        }
        if (nMot != nMotNow) {
            AnimPlay(3, nMot, 1, 4.0f, 0.0f);
        }
    }
    pWork->m_nDir = nDir;
    pWork->m_nMot = nMot;

    float fRate = CalcMoveSpeedRate_(0.75f, 0);
    float fAccel = m_fFrameStep * (0.1f * fRate * m_fTimeScale);
    pWork->m_fSpeedRate += fAccel;
    pWork->m_fSpeedRate = Clamp(pWork->m_fSpeedRate, 0.0f, fRate);
    long nNow = AnimGetMotID(3, 0);
    if (!(nNow >= 0x32 && nNow <= 0x33)) {
        float fSpeed = 1.0f / m_fScaleBase;
        if (m_nStatusFlag & 0x10) {
            fSpeed *= 0.8f;
        }
        AnimSetSpeed(3, fRate * Clamp(fSpeed, 1.0f, 2.0f));
    }

    // A turn round when the stick turns far back.
    if (pWork->m_fBlockFrame >= 10.0f && __fabsf(WrapRadPi(pWork->m_fRotYTrg - pWork->m_fRotY)) >= 2.7925267f &&
        __fabsf(WrapRadPi(pWork->m_fRotYTrg - pWork->m_fRotY)) > 1.7453293f && !(m_nStatusFlag & 1)) {
        m_fPush1GapX = 0.0f;
        m_fPush1GapY = 0.0f;
        m_fPush1GapZ = 0.0f;
        pWork->m_fAccelFrame = 0.0f;
        FsmqCutNext();
        STATE_WORK_TURN *pTurn = static_cast<STATE_WORK_TURN *>(FsmqPushState(STATE_TURN));
        pTurn->m_nFlag = 1;
        pTurn->m_fMoveRotY = pWork->m_fRotY;
        pTurn->m_fRotY = pWork->m_fRotYTrg;
        pTurn->m_fRotYEnd = pWork->m_fRotYTrg;
        pTurn->m_nMot = -0x31;
        if (WrapRadPi(pTurn->m_fRotY - GetRotY()) > 0.0f) {
            pTurn->m_nMot = 0x31;
        }
        FsmqDoChain();
        return;
    }

    float fDiff = WrapRadPiHalfOpen(pWork->m_fRotYTrg - pWork->m_fRotY);
    float fStep = m_fTimeScale * (0.5f * fDiff * m_fFrameStep);
    fDiff = LimitTurn(fDiff, fStep);
    pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + fDiff);

    float fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(3));
    if (((pWork->m_nFlag >> 2) & 1) == 1) {
        // The first frames of the block work are faster.
        float fMax = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(4));
        float fFrame = pWork->m_fAccelFrame;
        pWork->m_fAccelFrame = fFrame + GetWholeFrameRate();
        fSpeed = fMax - (fFrame * (fDown * fMax)) / 20.0f;
        if (20.0f <= pWork->m_fAccelFrame) {
            pWork->m_fAccelFrame = 20.0f;
            pWork->m_nFlag &= ~4;
        }
    }
    float fLen = fSpeed * GetWholeFrameRate();
    float fRotY = GetRotY();
    bcs::FVEC3 move;
    bcs::BCS_VecZ_RotY(move, fLen, bcs::ROUND_PI(pWork->m_fRotY - fRotY));
    bcs::BCS_Vec_RotY(move, move, GetRotY());
    AddPosXZ(move);

    if (IsNowBlockWork()) {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetState() == STATE_TRICK_MOVE) {
                float fRotY = pWork->m_fRotY;
                CChrManBasket::GetInstance()->CheckCameraSlowTrickMove_(fRotY, this,
                                                                        static_cast<CChrCoreBasket *>(pChr));
                return;
            }
        }
    }
}

namespace {

    // (guessed helper) Whether a equals b, as a flag.
    inline int IsEqual(float a, float b)
    {
        return a == b;
    }
}

// (inline in the original; this file has the module's copy)
inline int msw::CChrCore::IsHoldBall() const
{
    return m_nStatusFlag & 1;
}

int msw::CChrCoreBasket::SubStateNeedBlockWork_(int, float)
{
    m_nBasketFlag &= ~0x4000;

    int bEnable;
    if (IsDisable()) {
        bEnable = 0;
    } else if (IsHoldBall()) {
        bEnable = 0;
    } else if (m_nFlagF4 & 0x20000) {
        bEnable = 0;
    } else if (m_nComLevel != 0) {
        bEnable = 0;
    } else if ((m_nStatusFlag & 0x10000) || (m_nStatusFlag & 0x8000)) {
        bEnable = 0;
    } else {
        bEnable = CGame::g_GameWork.GetGameType() != 2;
    }
    if (!bEnable) {
        m_fBlockWorkWait = 0.0f;
        return 0;
    }

    // Starts the one on one with an opponent holding a ball in reach.
    int bHit = 0;
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall == 0 || !pBall->IsActive()) {
            continue;
        }
        CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(pBall->GetBallNo());
        if (pChr == 0 || pChr == this || GetTeamNo() == pChr->GetTeamNo() || !pChr->Canbe1on1()) {
            continue;
        }
        int nSide = 0;
        bcs::FVEC3 pos;
        pChr->GetCompPos(pos);
        if (HitCheckOval_(pos, nSide)) {
            bHit = 1;
            pChr->Methd1on1(nSide);
        }
    }
    UpDate1on1ActMinFrame();
    if (!bHit && IsEqual(0.0f, m_fBlockWorkWait)) {
        return 0;
    }
    m_fUnk274 = 15.0f;
    if (bHit) {
        m_nBasketFlag |= 0x4000;
    }
    return 1;
}

namespace {

    // The size change state's work (names guessed).
    struct STATE_WORK_SIZE_CHANGE {
        long m_nStep;        // 0x00
        long m_nType;        // 0x04: 1 shrinks the character, 0 brings it back
        float m_fScaleFrom;  // 0x08
        float m_fScaleTo;    // 0x0C
        float m_fFrameNum;   // 0x10: frames of the change, 0 for the default speed
        float m_fRate;       // 0x14: how far the change has got (0 to 1)
        float m_fUnk18;      // 0x18
        bcs::FVEC3 m_From;   // 0x1C: the hop of a change in the air, from
        bcs::FVEC3 m_To;     // 0x28: and to
        float m_fHop;        // 0x34: the rest of the hop (1 to 0)
        int m_bHop;          // 0x38: hopping
    };

    // The jump state's work (names guessed).
    struct STATE_WORK_JUMP {
        long m_nStep;                  // 0x00
        unsigned long m_Unk04;         // 0x04
        // 0x08: bit 3 a jump without the jump's start (bit 5: already in
        // the air).
        unsigned long m_nFlag;
        unsigned long m_Unk0C;         // 0x0C
        float m_fRotY;                 // 0x10: the yaw to turn to, negative for none
        float m_fFrame;                // 0x14
        unsigned char m_Unk18[0x4C - 0x18];
        msw::CChrCore::DEF_COLL m_Def; // 0x4C
    };

    // The item shoot state's work (names guessed).
    struct STATE_WORK_ITEM_SHOOT {
        long m_nStep;         // 0x00
        float m_fRotY;        // 0x04: the direction to throw in
        float m_fStartRotY;   // 0x08
        float m_fThrowFrame;  // 0x0C: the frame of the motion the item leaves the hand
    };

    // The motion play state's work (names guessed; as in
    // game/basket/CChrCoreBasketMethod.cpp).
    struct STATE_WORK_MOT_PLAY {
        long m_nStep;          // 0x00
        // 0x04: 0x1 no blending in, 0x2 drop what is held, 0x4 face the
        // camera (0x8 turning to it), 0x10 timed (m_fTime), 0x40 no body
        // push
        unsigned long m_nFlag;
        long m_nMot;           // 0x08: the motion to play
        long m_nNextMot;       // 0x0C: the motion after it, -1 for none
        long m_nVoice;         // 0x10: -1 for none
        long m_nSe;            // 0x14: -1 for none
        float m_fTime;         // 0x18: frames left (flag 0x10)
    };

    // (guessed helper) b - a, by way of a temporary.
    inline void SubVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &b, const bcs::FVEC3 &a)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &b, &a);
        rOut = diff;
    }
}

// The score of a target of the item shot (contents not known; the item
// shoot state clears it).
struct msw::CChrCoreBasket::SCORE_PARAM {
    long m_anUnk00[3];
    long m_anUnk0C[3];

    SCORE_PARAM()
    {
        for (int i = 0; i < 3; i++) {
            m_anUnk00[i] = 0;
            m_anUnk0C[i] = 0;
        }
    }
};

void msw::CChrCoreBasket::CtorStateSizeChange()
{
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_SIZE_CHANGE *pWork = FsmqGetWork<STATE_WORK_SIZE_CHANGE>();
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    AnimStop(3);
    pWork->m_fFrameNum = 0.0f;
    if (pWork->m_nType == 1) {
        DropBall();
        PlaySePos(0xC45, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        SetBodyScaleState(2, 420.0f);
        pWork->m_fScaleTo = 0.45f;
        m_fScaleTo = 0.45f;
    } else {
        pWork->m_fScaleTo = 1.0f;
        m_fScaleTo = 1.0f;
    }
    pWork->m_nStep = 0;
    pWork->m_fScaleFrom = m_fScaleBase;
    pWork->m_fRate = 0.0f;
    if (m_nStatusFlag & 2) {
        bcs::FVEC3 vel;
        bcs::BCS_VecZ_RotY(vel, 0.1f, GetRotY());
        DropItem(vel, 0, 0);
    }
}

void msw::CChrCoreBasket::LoopStateSizeChange()
{
    STATE_WORK_SIZE_CHANGE *pWork = FsmqGetWork<STATE_WORK_SIZE_CHANGE>();

    int bEnd = 0;
    switch (pWork->m_nStep) {
        case 0:
            if (pWork->m_fFrameNum > 0.0f) {
                pWork->m_fRate = (1.0f + AnimGetFrame(3)) / pWork->m_fFrameNum;
            } else {
                pWork->m_fRate += (1.0f / 30.0f) * (m_fFrameStep * m_fTimeScale);
            }
            if (pWork->m_bHop) {
                if (pWork->m_fHop <= 0.0f) {
                    pWork->m_bHop = 0;
                    m_nStatusFlag &= ~0xC00;
                } else {
                    pWork->m_fHop -= (1.0f / 30.0f) * (m_fFrameStep * m_fTimeScale);
                    if (pWork->m_fHop < 0.0f) {
                        pWork->m_fHop = 0.0f;
                    }
                    float fRate = Clamp(nw4r::math::SinRad(1.5707964f * (1.0f - pWork->m_fHop)), 0.0f, 1.0f);
                    bcs::FVEC3 pos;
                    bcs::BCS_Vec_Lerp(pos, pWork->m_From, pWork->m_To, fRate);
                    SetPosY(pos.y);
                }
            }
            pWork->m_fRate = Clamp(pWork->m_fRate, 0.0f, 1.0f);
            if (pWork->m_fRate >= 1.0f) {
                m_fScaleBase = pWork->m_fScaleTo;
                if (pWork->m_nType == 0) {
                    SetBodyScaleState(0, 420.0f);
                }
                if (m_nStatusFlag & 0x400) {
                    pWork->m_nStep++;
                } else {
                    bEnd = 1;
                }
            } else {
                m_fScaleBase = pWork->m_fRate * (pWork->m_fScaleTo - pWork->m_fScaleFrom) + pWork->m_fScaleFrom;
            }
            break;
        case 1:
            // Changed in the air: falls to the ground.
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            m_fJumpAccelY = -0.025f;
            ClearRevise();
            m_fJumpUnk220 = 0.0f;
            m_nJumpStep = 0;
            m_nJumpSubStep = 0;
            m_fJumpUnk224 = 0.0f;
            pWork->m_nStep++;
        default:
            if (Util_DoJump(m_fFrameStep * m_fTimeScale, 0)) {
                bEnd = 1;
            }
            break;
    }
    if (bEnd) {
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateSizeChange()
{
    m_nStatusFlag &= ~0xC00;
    m_fScaleUnk1B0 = 1.0f;
    m_fScaleBase = m_fScaleTo;
}

void msw::CChrCoreBasket::CtorStateJump()
{
    STATE_WORK_JUMP *pWork = FsmqGetWork<STATE_WORK_JUMP>();

    pWork->m_Def.m_nStep = 0;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    m_nStatusFlag &= ~0x1F000000;
    if (pWork->m_nFlag & 0x20) {
        // Already in the air: a fall.
        m_fJumpAccelY = -0.025f;
        m_fJumpRotY = -1.0f;
        m_fJumpUnk220 = 0.0f;
        m_nJumpStep = 0;
        m_nJumpSubStep = 0;
        m_fJumpUnk224 = 0.0f;
        pWork->m_nFlag |= 8;
        pWork->m_fRotY = -1.0f;
        return;
    }
    PlayVoice(0x20, 0);
    PlayJumpSe(0);
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_fJumpUnk220 = 0.0f;
    SetCollDefParam_(&pWork->m_Def, 5);
}

void msw::CChrCoreBasket::LoopStateJump()
{
    STATE_WORK_JUMP *pWork = FsmqGetWork<STATE_WORK_JUMP>();

    float fFrameStep = m_fFrameStep;
    if (pWork->m_fRotY >= 0.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }
    pWork->m_fFrame += fFrameStep * m_fTimeScale;
    if (Util_DoJump(m_fFrameStep * m_fTimeScale, (pWork->m_nFlag >> 3) & 1) == 1) {
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(5);
        Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
    }
    if (!(m_nStatusFlag & 0xC00)) {
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateJump()
{
    m_nStatusFlag &= ~0x1000;
}

void msw::CChrCoreBasket::CtorStateItemShoot()
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    m_UnkEC |= 0x10;
    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
        SetCollBodyPushRank_(4);
    } else {
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
    }
    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();
    long nMot = GetMotID_ItemShoot(GetAttachItemMotType(), 0);
    m_nStatusFlag &= ~0x1F000000;
    PrepareShotStockItem(8.0f);
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
    float fFrame = AnimSearchTrgFrame(nMot, 2, -1.0f);
    if (fFrame < 0.0f) {
        fFrame = 6.0f;
    }
    pWork->m_fThrowFrame = fFrame;
    pWork->m_fStartRotY = GetRotY();
    pWork->m_fRotY = GetItemThrowDir(pWork->m_fRotY);
    SetMoveMotType(0);
    m_nStatusFlag &= ~0x1800;
}

void msw::CChrCoreBasket::LoopStateItemShoot()
{
    static const float fMaxDist = 5.0f;
    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();

    switch (pWork->m_nStep) {
        case 0:
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            if (AnimGetFrame(3) >= 5.0f) {
                // Aims at where the target will be when the item gets there.
                float fRotYNow = GetRotY();
                if (-1.0f != pWork->m_fRotY) {
                    SetRotY(pWork->m_fRotY);
                }
                SCORE_PARAM param;
                CChrCoreBasket *pTarget = GetTargetItemShootCore(param);
                SetRotY(fRotYNow);
                float fRotY = GetRotY();
                if (pTarget != 0) {
                    long nMot = GetMotID_ItemShoot(GetAttachItemMotType(), 0);
                    float fFrame = AnimSearchTrgFrame(nMot, 2, -1.0f) - 5.0f;
                    if (fFrame < 0.0f) {
                        fFrame = 6.0f;
                    }
                    bcs::FVEC3 diff;
                    SubVec(diff, GetPos(), pTarget->GetPos());
                    float fRate = Clamp(bcs::BCS_Vec_GetLen(diff) / fMaxDist, 0.0f, 1.0f);
                    float fAhead = 18.0f * fRate;
                    bcs::FVEC3 pos;
                    pTarget->GetFuturePos(pos, fFrame + fAhead);
                    fRotY = GetRotY_ToPosXZ(pos);
                }
                pWork->m_fRotY = bcs::ROUND_PI(fRotY);
                if (m_nStatusFlag & 0x400) {
                    SetCollBodyPushPower_(1);
                    SetCollBodyPushRank_(4);
                } else {
                    SetCollBodyPushPower_(5);
                    SetCollBodyPushRank_(1);
                }
                pWork->m_nStep = 1;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                if (m_nStatusFlag & 0x400) {
                    m_nStatusFlag &= ~0x400;
                    m_fJumpUnk224 = 0.0f;
                    m_fJumpAccelY = -0.025f;
                    pWork->m_nStep = 2;
                } else {
                    FsmqDoChain();
                }
            } else if (m_nStatusFlag & 2) {
                float fFrame = AnimGetFrame(3);
                float fThrowFrame = pWork->m_fThrowFrame;
                if (fFrame >= fThrowFrame) {
                    unsigned long nFlag = 1 << GetPlayerNo();
                    pWork->m_fRotY = GetItemThrowDir(pWork->m_fRotY);
                    bcs::FVEC3 dir;
                    bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
                    ThrowItem_(dir, nFlag, (m_nStatusFlag >> 10) & 1);
                    PlayVoice(0x21, 0);
                    SetRotY(pWork->m_fRotY);
                    if (m_nStatusFlag & 0x400) {
                        m_nStatusFlag &= ~0x400;
                        m_fJumpUnk224 = 0.0f;
                        m_fJumpAccelY = -0.025f;
                        pWork->m_nStep = 2;
                    }
                } else {
                    // Turns to the throw as the motion comes up to it.
                    float fTurn = bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fStartRotY);
                    float fRate;
                    if (fThrowFrame <= 0.0f) {
                        fRate = 1.0f;
                    } else {
                        fRate = Clamp(fFrame / fThrowFrame, 0.0f, 1.0f);
                    }
                    if (fTurn > 3.1415927f) {
                        fTurn -= 6.2831855f;
                    }
                    float fRot = fTurn * nw4r::math::SinRad(1.5707964f * fRate);
                    SetRotY(bcs::ROUND_PI(fRot + pWork->m_fStartRotY));
                }
            }
        case 2:
            if (Util_DoJumpMove(m_fFrameStep) == 1) {
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                if (pWork->m_nStep > 1) {
                    AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -4 : 4, 0, 4.0f, 0.0f);
                    pWork->m_nStep = 3;
                    FsmqRetry();
                    return;
                }
            }
            break;
        case 3:
            Util_DoJumpMove(m_fFrameStep);
            if (IsAnimEnd(3)) {
                FsmqDoChain();
            }
            break;
    }
}

void msw::CChrCoreBasket::DtorStateItemShoot()
{
    m_nStatusFlag &= ~0x1C00;
}

void msw::CChrCoreBasket::CtorStateMotPlay()
{
    STATE_WORK_MOT_PLAY *pWork = FsmqGetWork<STATE_WORK_MOT_PLAY>();

    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(1);
    if (pWork->m_nFlag & 0x40) {
        m_UnkEC &= ~0x10;
    } else {
        m_UnkEC |= 0x10;
    }
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    if (pWork->m_nVoice >= 0) {
        PlayVoice(pWork->m_nVoice, 0);
    }
    if (!(pWork->m_nFlag & 2)) {
        if (m_nStatusFlag & 3) {
            FsmqDoChain();
            return;
        }
    } else {
        Util_ExchgToWaistPos();
        Util_NodePatchZeroTrans();
        bcs::FVEC3 vel;
        bcs::BCS_VecZ_RotY(vel, 0.2f, GetRotY());
        vel.y = 0.1f;
        DropItem(vel, 0, 0);
        DropBall();
    }
    if (pWork->m_nSe >= 0) {
        PlaySe(pWork->m_nSe, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    m_UnkEC &= ~0x2;
    pWork->m_nStep = 0;

    float fInter;
    if (pWork->m_nFlag & 1) {
        fInter = 0.0f;
    } else {
        fInter = 8.0f;
    }
    if (IsAnimResLoopType(pWork->m_nMot)) {
        AnimPlay(3, pWork->m_nMot, 1, fInter, 0.0f);
        pWork->m_nStep = 1;
    } else {
        AnimPlay(3, pWork->m_nMot, 0, fInter, 0.0f);
    }
}

void msw::CChrCoreBasket::LoopStateMotPlay()
{
    STATE_WORK_MOT_PLAY *pWork = FsmqGetWork<STATE_WORK_MOT_PLAY>();

    m_UnkEC &= ~0x2;
    if (pWork->m_nFlag & 4) {
        float fCamRotY = m_fCamRotY;
        if (pWork->m_nFlag & 8) {
            RotateYRate_(bcs::ROUND_PI(3.1415927f + fCamRotY), 0.25f);
        } else {
            SetRotY(3.1415927f + fCamRotY);
        }
    }
    if (pWork->m_nStep == 0 && IsAnimEnd(3)) {
        AnimPlay(3, pWork->m_nNextMot, 1, 0.0f, 0.0f);
        pWork->m_nStep++;
    }
    if (pWork->m_nFlag & 0x10) {
        if (pWork->m_fTime > 0.0f) {
            pWork->m_fTime -= m_fFrameStep;
        }
        if (pWork->m_fTime <= 0.0f) {
            FsmqDoChain();
        }
    }
}

void msw::CChrCoreBasket::DtorStateMotPlay()
{
}

void msw::CChrCoreBasket::CtorStateTurn()
{
    STATE_WORK_TURN *pWork = FsmqGetWork<STATE_WORK_TURN>();

    pWork->m_nStep = 0;
}

// (inline in the original: the turn state calls it through the vtable and
// this file has the module's copy right after it)
inline int msw::CChrCoreBasket::IsBattle() const
{
    return (m_nBasketFlag >> 1) & 1;
}

void msw::CChrCoreBasket::LoopStateTurn()
{
    STATE_WORK_TURN *pWork = FsmqGetWork<STATE_WORK_TURN>();
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();

    switch (pWork->m_nStep) {
        case 0: {
            m_UnkEC &= ~0x10;
            SetCollBodyPushPower_(1);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(1);
            SetMoveMotType(0);
            if (m_nStatusFlag & 1) {
                pWork->m_fFrameNum = pParam->m_nTurnFrame;
                if (pWork->m_nFlag & 0x20) {
                    pWork->m_fFrameNum *= 0.7f;
                }
            } else {
                pWork->m_fFrameNum = 20.0f;
            }
            PlayTurnSe(0);
            pWork->m_fFrame = 0.0f;

            // Which way round the turn goes.
            int bLeft;
            if (IsHoldBall()) {
                if (pWork->m_nMot > 0) {
                    bLeft = 0;
                } else {
                    bLeft = 1;
                }
            } else {
                float fRotY = GetRotY();
                bLeft = WrapRadPi(GetRotY_ToGoal() - fRotY) < 0.0f;
            }
            AnimPlay(3, pWork->m_nMot, 0, 8.0f, 0.0f);
            float fFrameNum = AnimGetFrameNum(3);
            float fSpeed = 1.0f;
            if (pWork->m_fFrameNum > 0.0f) {
                fSpeed = fFrameNum / pWork->m_fFrameNum;
            }
            AnimSetSpeed(3, fSpeed);

            pWork->m_Move.m_nType = 11;
            pWork->m_Move.m_fFrame = 0.0f;
            pWork->m_Move.m_fFrameMax = pWork->m_fFrameNum;
            pWork->m_Move.m_fDist = IsHaveBall() == 1 ? pParam->m_fTurnDist : 0.8f;
            if (pWork->m_nFlag & 0x20) {
                pWork->m_Move.m_fDist *= -0.5f;
            }
            Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fMoveRotY);

            pWork->m_fTurn = bcs::ROUND_PI(pWork->m_fRotY - GetRotY());
            if (pWork->m_fTurn > 3.1415927f) {
                pWork->m_fTurn -= 6.2831855f;
            }
            if (bLeft) {
                if (pWork->m_fTurn > 0.0f) {
                    pWork->m_fTurn = -DegToRad(180.0f + (180.0f - RadToDeg(pWork->m_fTurn)));
                }
            } else if (pWork->m_fTurn < 0.0f) {
                pWork->m_fTurn = DegToRad(180.0f + (180.0f + RadToDeg(pWork->m_fTurn)));
            }
            pWork->m_fStartRotY = GetRotY();
            if (!(m_nStatusFlag & 1)) {
                SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(4), 8.0f);
            }
            SetCollDefParam_(&pWork->m_Def, 9);
            pWork->m_nStep = 1;
        }
        case 1: {
            pWork->m_fFrame += GetWholeFrameRate();
            SubStateNeedBlockWork_(1, GetWholeFrameRate());
            if (pWork->m_fFrame >= pWork->m_fFrameNum) {
                pWork->m_fFrame = 0.0f;
                if (m_nStatusFlag & 1) {
                    if (AnimGetMotIDEx(3) == -0x3F) {
                        m_nStatusFlag &= ~4;
                    } else {
                        m_nStatusFlag |= 4;
                    }
                    if (!(pWork->m_nFlag & 0x21) || !MethdDash()) {
                        Util_PushBackOnBallerBattleEnd_(IsBattle(), pWork->m_nFlag & 1, pWork->m_fRotYEnd);
                        FsmqDoChain();
                    }
                } else {
                    FsmqClear();
                    if (pWork->m_nFlag & 1) {
                        // Back to the run of the block work.
                        STATE_WORK_RUN *pRun = static_cast<STATE_WORK_RUN *>(FsmqPushState(STATE_RUN));
                        pRun->m_fAccelFrame = 0.0f;
                        pRun->m_nFlag = 7;
                    }
                }
                return;
            }
            if ((pWork->m_nFlag & 1) && pWork->m_fFrame >= 10.0f) {
                // A dash out of the turn of the block work.
                float fDeg = RadToDeg(bcs::ROUND_PI(pWork->m_fRotYEnd - GetRotY()));
                float fDeg2 = RadToDeg(bcs::ROUND_PI(pWork->m_fRotY - GetRotY()));
                if (((fDeg >= 45.0f && fDeg <= 135.0f) || (fDeg >= 225.0f && fDeg <= 315.0f)) &&
                    ((fDeg2 >= 45.0f && fDeg2 <= 135.0f) || (fDeg2 >= 225.0f && fDeg2 <= 315.0f)) &&
                    __fabsf(WrapRadPi(pWork->m_fRotYEnd - pWork->m_fRotY)) > 1.7453293f &&
                    pWork->m_fFrame <= 0.5f * pWork->m_fFrameNum && MethdDash()) {
                    return;
                }
            }
            Util_DoDirMove(pWork->m_Move, pWork->m_fFrame, pWork->m_DirMove, 1);
            if (pWork->m_nFlag & 2) {
                float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
                if (fRate >= 0.9f && !(pWork->m_nFlag & 8)) {
                    pWork->m_nFlag |= 8;
                    Eff_CreateSimple(0x4E0, GetPos(), GetRotY(), 3, 0, 0);
                }
                SetRotY(bcs::ROUND_PI(pWork->m_fTurn * fRate + pWork->m_fStartRotY));
            }
            if (!(pWork->m_nFlag & 4)) {
                float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
                if (m_nStatusFlag & 1) {
                    if (fRate >= 0.2f) {
                        pWork->m_nFlag |= 4;
                        Eff_CreateSimple(0, 0x4DF, GetPos(), GetRotY(), 3, 0, 0);
                    }
                } else if (fRate >= 0.6f) {
                    pWork->m_nFlag |= 4;
                    if (pWork->m_nMot > 0) {
                        Eff_CreateSimple(0, 0x4E3, GetPos(), GetRotY(), 3, 0, 0);
                    } else {
                        Eff_CreateSimple(0, 0x4E2, GetPos(), GetRotY(), 3, 0, 0);
                    }
                }
            }
            ClearRevise();
            break;
        }
    }
    pWork->m_nFlag &= ~1;
}

void msw::CChrCoreBasket::DtorStateTurn()
{
}

// The dash state's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_DASH {
    long m_nStep;        // 0x00
    long m_nMot;         // 0x04: the dribble motion of the dash
    // 0x08: bit 0 the stick is held, bit 1 a turn ends the dash, bit 2 it
    // turns now, bit 3 the start effect, bit 4 the dash is over, bit 5 a
    // dash of the block work.
    unsigned long m_nFlag;
    float m_fRotYTrg;    // 0x0C: the yaw the stick asks for
    float m_fRotY;       // 0x10: the yaw of the dash
    float m_fUnk14;      // 0x14
    float m_fUnk18;      // 0x18
    float m_fFrame;      // 0x1C
    float m_fFrameNum;   // 0x20
    float m_fTime;       // 0x24: frames since the start
};

void msw::CChrCoreBasket::CtorStateDash()
{
    STATE_WORK_DASH *pWork = FsmqGetWork<STATE_WORK_DASH>();

    pWork->m_fTime = 0.0f;
    pWork->m_fRotY = GetRotY();
    AnimPlay(3, pWork->m_nMot, 1, 4.0f, GetDribbleStartFrame(pWork->m_nMot));
    if (m_nStatusFlag & 1) {
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(1);
        SetMoveMotType(8);
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(6), 8.0f);
        SetCollBodyPushRank_(1);
        PlaySePos(0xC78, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        ReattachBall(4, 0.0f, 0);
    }
    if (((pWork->m_nFlag >> 3) & 1) == 1) {
        Eff_CreateSimple(10, 0x4DE, GetPos(), GetRotY(), 3, 0, 0);
    }
}

float msw::CChrCoreBasket::GetDashActRate() const
{
    if (GetState() != STATE_DASH) {
        return 0.0f;
    }
    const STATE_WORK_DASH *pWork = FsmqGetWork<STATE_WORK_DASH>();
    return Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
}

void msw::CChrCoreBasket::LoopStateDash()
{
    STATE_WORK_DASH *pWork = FsmqGetWork<STATE_WORK_DASH>();

    pWork->m_fRotY = SubStateDashMove_(*pWork);
    long nEffFrame = GetCcParam2_()->m_nDashEffFrame;
    if (nEffFrame > 0) {
        m_nBasketFlag |= 0x80000;
        m_fDashEffFrame = nEffFrame;
    }
    pWork->m_fUnk14 = 3.0f;
    unsigned long nFlag = pWork->m_nFlag;
    if ((nFlag & 0x10) || pWork->m_fUnk14 <= 0.0f) {
        if (FsmqGetNextState() == 0) {
            Util_PushBackOnBallerBattleEnd_(IsBattle(), nFlag & 1, pWork->m_fRotYTrg);
        }
        FsmqNext();
    } else if (nFlag & 4) {
        // Turns round with the ball.
        FsmqCutNext();
        STATE_WORK_TURN *pTurn = static_cast<STATE_WORK_TURN *>(FsmqPushState(STATE_TURN));
        if (IsHaveLeft() == 1) {
            pTurn->m_nMot = -0x3F;
            m_nStatusFlag |= 4;
        } else {
            pTurn->m_nMot = 0x3F;
            m_nStatusFlag &= ~4;
        }
        pTurn->m_nFlag = 3;
        if (pWork->m_nFlag & 0x20) {
            pTurn->m_nFlag |= 0x10;
        }
        pTurn->m_fMoveRotY = GetRotY();
        pTurn->m_fRotY = pWork->m_fRotYTrg;
        pTurn->m_fRotYEnd = pWork->m_fRotYTrg;
        pTurn->m_fUnk20 = pWork->m_fFrameNum - pWork->m_fFrame;
        FsmqDoChain();
    } else {
        pWork->m_nFlag = nFlag & ~1;
    }
}

void msw::CChrCoreBasket::DtorStateDash()
{
}

float msw::CChrCoreBasket::SubStateDashMove_(STATE_WORK_DASH &rWork)
{
    static const float fLocalFrameRate = 1.0f;

    rWork.m_fTime += GetWholeFrameRate();
    float fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(6));
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    float fRotYTrg = rWork.m_fRotYTrg;
    if ((rWork.m_nFlag & 2) && rWork.m_fFrame >= pParam->m_nDashTurnFrame &&
        __fabsf(WrapRadPi(fRotYTrg - rWork.m_fRotY)) > DegToRad(pParam->m_nDashTurnDeg)) {
        rWork.m_nFlag |= 4;
        return GetRotY();
    }

    // Steering, by at most 4 degrees a frame.
    float fDiff = WrapRadPiStep(fRotYTrg - rWork.m_fRotY);
    float fMax = bcs::DEG_TO_RAD(4.0f);
    float fStep = Clamp(fDiff, -fMax, fMax) * m_fFrameStep;
    fDiff = LimitTurn(fDiff, fStep);
    rWork.m_fRotY = bcs::ROUND_PI(rWork.m_fRotY + fDiff);
    RotateYRate_(rWork.m_fRotY, 0.2f);

    // The speed along a curve over the dash.
    rWork.m_fFrame += m_fFrameStep;
    if (rWork.m_fFrameNum <= rWork.m_fFrame) {
        rWork.m_nFlag |= 0x10;
    }
    float fRate = Clamp(rWork.m_fFrame / rWork.m_fFrameNum, 0.0f, 1.0f);
    bcs::FVEC3 out;
    {
        static const float fCurveRate = 0.125f;
        bcs::BCS_Vec_BetaSplineCurveXZ(out, bcs::FVEC3(0.0f, 0.0f, 1.0f),
                                       bcs::FVEC3(1.0f - fCurveRate, 0.0f, 1.0f - fCurveRate),
                                       bcs::FVEC3(fLocalFrameRate, 0.0f, 0.7f), fRate);
        fSpeed *= out.z;
    }
    bcs::FVEC3 pos = GetPos();
    MoveRunXZ_(rWork.m_fRotY, fSpeed, 1.0f);
    return rWork.m_fRotY;
}

// The parameters of a special move's battle move (names guessed).
struct msw::CChrCoreBasket::BATTLEPARAM {
    unsigned char m_Unk00[0x08];
    float m_fTurn;      // 0x08: the turn over the move
    float m_fRotY;      // 0x0C: the yaw of the side move
    float m_fFrame;     // 0x10
    float m_fUnk14;     // 0x14
    float m_fFrameNum;  // 0x18
    float m_fSide;      // 0x1C: the length of the side move
    float m_fCurve;     // 0x20: the curve (-1 to 1)
    float m_fRate;      // 0x24: how far the move had got
};

int msw::CChrCoreBasket::SubBattleMove_(BATTLEPARAM &rParam)
{
    float afRate[2];
    afRate[0] = rParam.m_fFrame / rParam.m_fFrameNum;
    afRate[1] = rParam.m_fRate;
    afRate[0] = Clamp(afRate[0], 0.0f, 1.0f);
    if (afRate[0] == afRate[1]) {
        return 1;
    }

    bcs::FVEC3 aSide[2];
    bcs::FVEC3 aTurn[2];
    for (long i = 0; i < 2; i++) {
        aSide[i].Set(0.0f, 0.0f, rParam.m_fSide);
        aTurn[i].Set(0.0f, 0.0f, rParam.m_fTurn);
    }
    for (long i = 0; i < 2; i++) {
        float fCtrl = (1.0f + rParam.m_fCurve) * 0.5f;
        {
            bcs::FVEC3 to(rParam.m_fFrameNum, 0.0f, rParam.m_fTurn);
            bcs::FVEC3 ctrl(rParam.m_fFrameNum * fCtrl, 0.0f, rParam.m_fTurn * fCtrl);
            bcs::FVEC3 from(0.0f, 0.0f, 0.0f);
            bcs::BCS_Vec_BetaSplineCurveXZ(aTurn[i], from, ctrl, to, afRate[i]);
        }
        if (0.0f != rParam.m_fSide) {
            bcs::FVEC3 to(rParam.m_fFrameNum, 0.0f, rParam.m_fSide);
            bcs::FVEC3 ctrl(rParam.m_fFrameNum * fCtrl, 0.0f, rParam.m_fSide * fCtrl);
            bcs::FVEC3 from(0.0f, 0.0f, 0.0f);
            bcs::BCS_Vec_BetaSplineCurveXZ(aSide[i], from, ctrl, to, afRate[i]);
            aSide[i].y = 0.0f;
            aSide[i].x = 0.0f;
            bcs::BCS_Vec_RotY(aSide[i], aSide[i], rParam.m_fRotY);
        }
    }
    bcs::FVEC3 move;
    SubVec(move, aSide[0], aSide[1]);
    AddPos(move);
    SetRotY(bcs::ROUND_PI(aTurn[0].z + GetRotY() - aTurn[1].z));
    rParam.m_fRate = afRate[0];
    return 0;
}

void msw::CChrCoreBasket::SubBSplineMove_(float fFrame, float fFrameNum, float &rRate, float fLen, float fRotY,
                                          float fCurve, const bcs::FVEC3 &)
{
    float afRate[2];
    afRate[0] = fFrame / fFrameNum;
    afRate[1] = rRate;
    afRate[0] = Clamp(afRate[0], 0.0f, 1.0f);
    if (afRate[0] == afRate[1]) {
        return;
    }

    bcs::FVEC3 aPos[2];
    aPos[0].Set(0.0f, 0.0f, fLen);
    aPos[1].Set(0.0f, 0.0f, fLen);
    if (0.0f != fLen) {
        float fCtrl = (1.0f + fCurve) * 0.5f;
        float fCtrlX = fFrameNum * fCtrl;
        float fCtrlZ = fLen * fCtrl;
        for (long i = 0; i < 2; i++) {
            bcs::FVEC3 to(fFrameNum, 0.0f, fLen);
            bcs::FVEC3 ctrl(fCtrlX, 0.0f, fCtrlZ);
            bcs::FVEC3 from(0.0f, 0.0f, 0.0f);
            bcs::BCS_Vec_BetaSplineCurveXZ(aPos[i], from, ctrl, to, afRate[i]);
            aPos[i].y = 0.0f;
            aPos[i].x = 0.0f;
            bcs::BCS_Vec_RotY(aPos[i], aPos[i], fRotY);
        }
    }
    bcs::FVEC3 move;
    SubVec(move, aPos[0], aPos[1]);
    AddPos(move);
    rRate = afRate[0];
}

namespace {

    // The state's work of a block work lost to a dribble (names guessed).
    struct STATE_WORK_BLOCK_WORK_LOSE {
        long m_nStep;      // 0x00
        long m_nUnk04;     // 0x04
        float m_fRotY;     // 0x08: the yaw to turn to
    };

    // The sliding state's work (names guessed).
    struct STATE_WORK_SLIDING {
        long m_nStep;       // 0x00
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fRotY;      // 0x08: the direction of the slide
        float m_fRotYTrg;   // 0x0C: the direction the stick asks for
        float m_fSpeedRate; // 0x10
        int m_bAccel;       // 0x14: the stick is held this frame
    };
}

// (guessed) x brought into [lo, hi]; a macro: the original reads x again
// for the second test.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

void msw::CChrCoreBasket::CtorStateBlockWorkLose()
{
    m_nFlagF4 |= 1;
    AnimSetSpeed(3, 1.0f);
}

void msw::CChrCoreBasket::LoopStateBlockWorkLose()
{
    STATE_WORK_BLOCK_WORK_LOSE *pWork = FsmqGetWork<STATE_WORK_BLOCK_WORK_LOSE>();

    float fFrameRate = GetWholeFrameRate();
    RotateYRate_(pWork->m_fRotY, 0.2f);
    bcs::FVEC3 move;
    bcs::BCS_VecZ_RotY(move, 1.0f, bcs::ROUND_PI(GetRotY()));
    float fSpeed = 0.03f * fFrameRate;
    move.x *= fSpeed;
    move.z *= fSpeed;
    AddPos(move);
    if (IsAnimEnd(3) == 1) {
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateBlockWorkLose()
{
    m_nFlagF4 &= ~1;
}

void msw::CChrCoreBasket::CtorStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    pWork->m_nStep = 0;
    pWork->m_fSpeedRate = 1.0f;
    PlayVoice(0x92, 0);
}

void msw::CChrCoreBasket::LoopStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    int bEnd = 0;
    switch (pWork->m_nStep) {
        case 0:
            AnimPlay(3, 0x28, 0, 4.0f, 0.0f);
            SetMoveMotType(2);
            SE_PlayMove(0, 0x45A, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nStep = 1;
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x29, 1, 0.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        case 2:
            if (!pWork->m_bAccel) {
                // Slowing down unless the stick is held.
                float fFrame = 99.0f;
                pWork->m_fSpeedRate -= m_fFrameStep * (1.0f / Max(fFrame, 1.0f));
                pWork->m_fSpeedRate = CLAMP(pWork->m_fSpeedRate, 0.0f, 1.0f);
                if (pWork->m_fSpeedRate <= 0.0f) {
                    SE_Stop(0, 8.0f);
                    AnimPlay(3, 0x2A, 0, 0.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            } else {
                float fFrame = 99.0f;
                pWork->m_fSpeedRate += m_fFrameStep * (1.0f / Max(fFrame, 1.0f));
                pWork->m_fSpeedRate = CLAMP(pWork->m_fSpeedRate, 0.0f, 1.0f);
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                bEnd = 1;
            }
            break;
    }

    float fRotYTrg = pWork->m_fRotYTrg;
    if (pWork->m_nStep < 3) {
        // Steering, by at most 6 degrees a frame.
        float fDiff = WrapRadPiStep(fRotYTrg - pWork->m_fRotY);
        float fMaxDeg = 6.0f;
        float fMax = DegToRad(fMaxDeg);
        float fRot = Clamp(fDiff, -fMax, fMax);
        float fStep = fRot * m_fFrameStep;
        fDiff = LimitTurn(fDiff, fStep);
        pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + fDiff);
        RotateYRate_(fRotYTrg, 0.2f);
    }

    float fSpeed = 0.7f * GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(0));
    bcs::FVEC3 pos = GetPos();
    MoveRunXZ_(pWork->m_fRotY, fSpeed, pWork->m_fSpeedRate);
    pWork->m_bAccel = 0;
    if (bEnd) {
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateSliding()
{
    SE_Stop(0, 0.0f);
}
