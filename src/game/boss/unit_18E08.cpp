// msw::CChrCoreBoss's neutral, run, turn, item shoot and motion play states,
// with the receive and toss pose helpers they share. The damage and dizzy
// states that follow in the binary are another file (game/boss/unit_1C248.cpp:
// its literals are pooled again).
#include "game/msw/CChrCoreBoss.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

namespace {

    // The run motions of the receive pose and of the receive stance, by
    // the direction of the run (Util_ChangeRot4: 0 front, 1 and 3 the
    // sides, 2 back; a negative id plays the motion mirrored), and a third
    // table nothing here reads.
    const short s_RunReceivePoseMot[4] = { 3, -3, 3, 3 };
    const short s_RunReceiveStanceMot[4] = { 78, 79, 80, -79 };
    const short s_RunUnkMot[4] = { 3, 3, 3, 3 };

    // The motions of each receive pose (GetReceivePoseDir_): the start,
    // the loop and the end.
    const short s_ReceivePoseMot[8][3] = {
        { 64, 70, 74 }, { 65, 71, 75 }, { 68, 71, 75 }, { 66, 72, 76 },
        { 69, 72, 76 }, { -66, -72, -76 }, { -69, -72, -76 }, { 67, 73, 77 },
    };

    // The same for the toss and the jump toss, which have one pose only.
    const short s_TossPoseMot[8][3] = {
        { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 },
        { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 },
    };
    const short s_JumpTossPoseMot[8][3] = {
        { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 },
        { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 }, { 64, 70, 74 },
    };

    // (guessed helper) An angle brought into [0, 2 pi).
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) Degrees to radians.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (guessed helper) x brought into [lo, hi].
    inline float Clamp(float x, float lo, float hi)
    {
        return x > hi ? hi : (x < lo ? lo : x);
    }

    // Stand-ins for unknown users of s_RunUnkMot, of 80 degrees and 0.8,
    // and of 0.23, which the original keeps in .rodata (the floats after
    // the literals of SubStateRunReceiveStance_ and of
    // SubStateGetNeutralPoseType_) but nothing reads: likely stripped
    // code. Dead comparisons, no code; called where the original has the
    // constants (see decisions.md, unreferenced data).
    inline void UnusedReferencesA(float f)
    {
        (void)s_RunUnkMot;
        if (f == 1.3962634f) {
        }
        if (f == 0.8f) {
        }
    }
    inline void UnusedReferencesB(float f)
    {
        if (f == 0.23f) {
        }
    }

    // (guessed helper) The dot product of a and b in the XZ plane.
    inline float DotXZ(const bcs::FVEC3 &a, const bcs::FVEC3 &b)
    {
        return a.x * b.x + a.z * b.z;
    }

    // (guessed helper) b - a.
    inline bcs::FVEC3 SubVec(const bcs::FVEC3 &b, const bcs::FVEC3 &a)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &b, &a);
        return diff;
    }
}

// (fn_8_18E08 in the split, a local function; name guessed) The point of
// the segment from `from` to `to` nearest pos in the XZ plane goes to rOut
// (at the ends all of it, otherwise only x and z). Returns where it
// lies on the segment, 0 at `from` and 1 at `to`. The vectors are copied in
// (by value) as the callers show; the two differences are made into
// temporaries and assigned, which gives the original's stack (and keeps
// the function out of line, as in the original).
static float GetNearestPosOnSegXZ(bcs::FVEC3 pos, bcs::FVEC3 from, bcs::FVEC3 to, bcs::FVEC3 &rOut)
{
    bcs::FVEC3 seg;
    bcs::FVEC3 toPos;
    seg = SubVec(to, from);
    toPos = SubVec(pos, from);
    float fDot = DotXZ(toPos, seg);

    if (fDot <= 0.0f) {
        rOut.Set(from.x, from.y, from.z);
        return 0.0f;
    }
    float fLenSq = seg.x * seg.x + seg.z * seg.z;
    if (fDot >= fLenSq) {
        rOut.Set(to.x, to.y, to.z);
        return 1.0f;
    }
    float fRate = fDot / fLenSq;
    float fX = seg.x * fRate;
    float fZ = seg.z * fRate;
    rOut.x = fX + from.x;
    rOut.z = fZ + from.z;
    return fRate;
}

// (fn_8_18F18 in the split, a local function; name guessed) Where the line from `from` towards `to`
// first meets the circle of fRadius around center in the XZ plane:
// the distance along the line to rDist (0 when it starts inside) and
// the point to rOut. Zero when the line misses the circle.
static int GetCrossPosCircleXZ(bcs::FVEC3 from, bcs::FVEC3 to, bcs::FVEC3 center, float fRadius, float &rDist,
                               bcs::FVEC3 &rOut)
{
    bcs::FVEC3 dir;
    dir = SubVec(to, from);
    if (dir.x * dir.x + dir.z * dir.z <= 0.0000010000001f) {
        dir.Set(0.0f, 0.0f, 1.0f);
    } else {
        bcs::FVEC3 xz(dir.x, 0.0f, dir.z);
        bcs::BCS_Vec_Normalize(dir, xz);
    }

    bcs::FVEC3 toFrom;
    toFrom = SubVec(from, center);
    float fC = DotXZ(toFrom, toFrom) - fRadius * fRadius;
    float fB = toFrom.x * dir.x + toFrom.z * dir.z;
    if (fC > 0.0f && fB > 0.0f) {
        return 0;
    }
    float fDisc = fB * fB - fC;
    if (fDisc < 0.0f) {
        return 0;
    }
    rDist = -fB - nw4r::math::FSqrt(fDisc);
    if (rDist < 0.0f) {
        rDist = 0.0f;
    }
    float fDist = rDist;
    rOut.x = dir.x * fDist;
    rOut.z = dir.z * fDist;
    rOut.x += from.x;
    rOut.z += from.z;
    return 1;
}

// The neutral state's work (names guessed).
struct msw::CChrCoreBoss::STATE_WORK_NEUTRAL {
    long m_nStep;  // 0x00
    long m_nType;  // 0x04: the pose type (SubStateGetNeutralPoseType_), -1 at the start
    long m_nPose;  // 0x08: the receive or toss pose
};

// The run state's work (names guessed).
struct msw::CChrCoreBoss::STATE_WORK_RUN {
    unsigned long m_nStep;  // 0x00: 0 and 1 start the motion, then 2
    long m_nUnk04;          // 0x04: 1 in a receive pose
    int m_bRun;             // 0x08: set while the run goes on (MethdRun)
    int m_bAccel;           // 0x0C: set in the frames the run is asked for
    int m_bWarpNode;        // 0x10: brings the model's root back first
    float m_fRotY;          // 0x14: the direction asked for
    float m_fMoveRotY;      // 0x18: the direction run in
    float m_fSpeed;         // 0x1C: the speed rate now
    float m_fSpeedMax;      // 0x20: the speed rate asked for
    long m_nPoseType;       // 0x24: SubStateGetNeutralPoseType_, -1 at the start
    long m_nDir;            // 0x28: Util_ChangeRot4 of the run in a pose
    long m_nMot;            // 0x2C: the motion playing
    float m_fFrame;         // 0x30: frames run
};

namespace {

    // The turn state's work (names guessed).
    struct STATE_WORK_TURN {
        long m_nStep;                               // 0x00
        float m_fSpeedRate;                         // 0x04: the run's speed rate
        float m_fFrame;                             // 0x08
        float m_fFrameMax;                          // 0x0C
        float m_fMoveRotY;                          // 0x10: the direction run in
        float m_fRotY;                              // 0x14: the direction to turn to
        msw::CChrCore::ROT_LERP1 m_Rot;             // 0x18
        msw::CC_DATA::CHR_PARAM::MOVE_PARAM m_Move; // 0x28
        msw::CChrCore::DIR_MOVE m_DirMove;          // 0x38
    };

    // The item shoot state's work (names guessed).
    struct STATE_WORK_ITEM_SHOOT {
        long m_nStep;         // 0x00
        float m_fRotY;        // 0x04: the direction to throw in
        float m_fStartRotY;   // 0x08
        float m_fThrowFrame;  // 0x0C: the frame of the motion the item leaves the hand
    };

    // The motion play state's work (names guessed; as in
    // game/boss/CChrCoreBossMethod.cpp).
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

    enum {
        STATE_TURN = msw::CChrCore::STATE_TURN
    };
}

void msw::CChrCoreBoss::CtorStateNeutral()
{
    if (m_nStatusFlag & 0x80000) {
        if (MethdTimeOver(1)) {
            FsmqRetry();
            return;
        }
    } else if (m_nComLevel == 4) {
        MethdRun(GetRotY());
        FsmqRetry();
        return;
    } else {
        SetPosY(0.0f);
    }

    STATE_WORK_NEUTRAL *pWork = FsmqGetWork<STATE_WORK_NEUTRAL>();
    pWork->m_nStep = 0;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    pWork->m_nType = -1;
}

void msw::CChrCoreBoss::LoopStateNeutral()
{
    STATE_WORK_NEUTRAL *pWork = FsmqGetWork<STATE_WORK_NEUTRAL>();

    long nType = SubStateGetNeutralPoseType_();
    if (nType != pWork->m_nType) {
        switch (nType) {
            case 1:
                pWork->m_nType = 1;
                SubStateInitTossPose_(pWork);
                break;
            case 2: {
                pWork->m_nType = 2;
                long nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
                pWork->m_nPose = nPose;
                SubStateSetReceivePose_(nPose);
                break;
            }
            case 3: {
                pWork->m_nType = 3;
                long nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
                pWork->m_nPose = nPose;
                SubStateSetReceivePose_(nPose);
                break;
            }
            default:
                pWork->m_nType = 0;
                SetBallCatchDisable_();
                AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                break;
        }
    } else if (nType == 0) {
        long nMot = AnimGetMotID(3, 0);
        if (Util_GetIdleMotID() != nMot) {
            AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
        }
    }

    switch (pWork->m_nType) {
        case 2:
        case 3: {
            CBall *pBall = GetTargetBall_();
            if (pBall != 0) {
                float fOfs = m_afReceiveRot[pWork->m_nPose];
                float fRot;
                if (pWork->m_nPose == 7) {
                    fRot = GetRotY_ToPosXZ(pBall->GetPosD4());
                } else {
                    fRot = WrapRad(GetRotY_ToPosXZ(pBall->GetTargetPos()) - fOfs);
                }
                RotateYRate_(fRot, 0.2f);
            }
            break;
        }
        case 1: {
            CBall *pBall = GetTargetBall_();
            if (pBall != 0) {
                float fOfs = m_afTossRot[pWork->m_nPose];
                RotateYRate_(WrapRad(GetRotY_ToPosXZ(pBall->GetPos()) - fOfs), 0.2f);
            }
            break;
        }
    }
}

inline void msw::CChrCoreBoss::SubStateInitTossPose_(STATE_WORK_NEUTRAL *pWork)
{
    // static-order swap: the original names these @LOCAL@...@fHeight@0 and
    // @LOCAL@...@fRadius (ours fHeight and fRadius@0); the bytes are in the
    // original order (decisions.md).
    static const float fHeight = 1.0f;
    static const float fRadius = 1.5f;

    SetBallCatchEnableVNode_(fRadius, fHeight, bcs::FVEC3(0.0f, -0.7f, 0.0f), 4);
    long nPose = GetTossPoseDir_(GetTossBaseDir_());
    pWork->m_nPose = nPose;
    AnimPlay(3, GetTossPoseMotID_(nPose, 0), 1, 8.0f, 0.0f);
}

void msw::CChrCoreBoss::DtorStateNeutral()
{
    SetBallCatchDisable_();
}

void msw::CChrCoreBoss::CtorStateRun()
{
    STATE_WORK_RUN *pWork = FsmqGetWork<STATE_WORK_RUN>();

    m_UnkEC |= 0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetMoveMotType(2);
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
    pWork->m_nPoseType = -1;
    m_fJumpUnk220 = 0.0f;
    if (pWork->m_bWarpNode != 0) {
        Util_NodePatchWarpToTransNode();
        pWork->m_bWarpNode = 0;
    }
}

void msw::CChrCoreBoss::LoopStateRun()
{
    STATE_WORK_RUN *pWork = FsmqGetWork<STATE_WORK_RUN>();

    long nType = SubStateGetNeutralPoseType_();
    if (nType != pWork->m_nPoseType) {
        pWork->m_nPoseType = nType;
        switch (nType) {
            default:
                pWork->m_nUnk04 = 0;
                m_fUnk274 = 0.0f;
                m_nStatusFlag &= ~0x20;
                pWork->m_nStep = 1;
                break;
            case 1:
            case 2:
                pWork->m_nUnk04 = 1;
                pWork->m_nStep = 0;
                break;
        }
    }

    switch (pWork->m_nPoseType) {
        default:
            SubStateRunNormal_(pWork);
            break;
        case 1:
        case 2:
            SubStateRunReceivePose_(pWork);
            break;
        case 3:
            SubStateRunReceiveStance_(pWork);
            break;
    }
}

void msw::CChrCoreBoss::DtorStateRun()
{
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
    SetBallCatchDisable_();
}

void msw::CChrCoreBoss::SubStateRunNormal_(STATE_WORK_RUN *pWork)
{
    int bWalk = 0;

    if (pWork->m_bRun == 0 && m_nComLevel != 4) {
        FsmqNext();
        FsmqRetry();
        return;
    }

    float fSpeedMax = pWork->m_fSpeedMax;
    float fMin = 0.0f;
    pWork->m_fFrame += m_fFrameStep;
    if (pWork->m_bAccel != 0 || m_nComLevel == 4) {
        const CHR_CORE_BOSS::CC_PARAM_BOSS *pParam = GetCcParam_();
        float fRate = pParam->m_Unk10 < 1.0f ? 1.0f : 1.0f / pParam->m_Unk10;
        float fAccel = m_fTimeScale * (m_fFrameStep * (fSpeedMax * fRate));
        pWork->m_fSpeed += fAccel;
    } else {
        float fDecel = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk14);
        pWork->m_fSpeed -= fDecel;
        if (pWork->m_fSpeed <= fMin) {
            pWork->m_bRun = 0;
        }
        fMin = 0.0f;
    }

    if (pWork->m_nStep <= 1) {
        long nMot = Util_GetRunMotID();
        if (fSpeedMax < 0.65f) {
            bWalk = 1;
            nMot = Util_GetWalkMotID();
        }
        AnimPlay(3, nMot, 1, 4.0f, 0.0f);
        SetMoveMotType(2);
        pWork->m_nStep = 2;
        pWork->m_nMot = nMot;
    } else {
        long nMot = Util_GetRunMotID();
        if (fSpeedMax < (pWork->m_nMot == nMot ? 0.4f : 0.65f)) {
            bWalk = 1;
            nMot = Util_GetWalkMotID();
        }
        if (nMot != pWork->m_nMot) {
            AnimPlay(3, nMot, 1, 8.0f, 0.0f);
            pWork->m_nMot = nMot;
        }
    }

    pWork->m_fSpeed = Clamp(pWork->m_fSpeed, fMin, fSpeedMax);
    float fScale = 1.0f / m_fScaleBase;
    if (bWalk) {
        fScale *= 1.75f;
    }
    fScale = Clamp(fScale, 1.0f, 2.0f);
    AnimSetSpeed(3, fScale * (fSpeedMax * GetCcParam_()->m_Unk28));

    if (m_nComLevel != 4 && m_nComLevel != 7 && m_nComLevel != 1) {
        if (pWork->m_fFrame >= GetCcParam_()->m_Unk18) {
            float fTurn = WrapRad(pWork->m_fRotY - pWork->m_fMoveRotY);
            if (fTurn > 3.1415927f) {
                fTurn -= 6.2831855f;
            }
            float fAbs = fabsf(fTurn);
            if (fAbs > DegToRad(GetCcParam_()->m_Unk1C) && !(m_nStatusFlag & 1)) {
                FsmqCutNext();
                STATE_WORK_TURN *pTurn = static_cast<STATE_WORK_TURN *>(FsmqPushState(STATE_TURN));
                pTurn->m_fSpeedRate = pWork->m_fSpeed;
                pTurn->m_fMoveRotY = pWork->m_fMoveRotY;
                pTurn->m_fRotY = pWork->m_fRotY;
                FsmqNext();
                FsmqRetry();
                return;
            }
        }
    }

    float fRot = pWork->m_fRotY;
    if (m_nComLevel == 4) {
        fRot = WrapRad(fRot + m_fWeaveWidth * nw4r::math::SinRad(m_fWeavePhase));
    }
    if (m_nComLevel == 4) {
        float fMaxTurn = 4.0f;
        float fTurn = fRot - pWork->m_fMoveRotY;
        while (fTurn > 3.1415927f) {
            fTurn -= 6.2831855f;
        }
        while (fTurn < -3.1415927f) {
            fTurn += 6.2831855f;
        }
        float fLimit = DegToRad(fMaxTurn);
        float fStep = Clamp(fTurn, -fLimit, fLimit) * m_fFrameStep;
        if (fTurn < 0.0f) {
            fTurn = fStep < fTurn ? fTurn : fStep;
        } else {
            fTurn = fStep > fTurn ? fTurn : fStep;
        }
        pWork->m_fMoveRotY = WrapRad(pWork->m_fMoveRotY + fTurn);
        fRot = pWork->m_fMoveRotY;
    } else {
        float fTurn = WrapRad(fRot - pWork->m_fMoveRotY);
        if (fTurn >= 3.1415927f) {
            fTurn -= 6.2831855f;
        }
        float fStep = m_fTimeScale * (0.5f * fTurn * m_fFrameStep);
        if (fTurn < 0.0f) {
            fTurn = fStep < fTurn ? fTurn : fStep;
        } else {
            fTurn = fStep > fTurn ? fTurn : fStep;
        }
        pWork->m_fMoveRotY = WrapRad(pWork->m_fMoveRotY + fTurn);
    }
    RotateYRate_(fRot, 0.2f);

    float fSpeed = CalcContiditionMoveSpeed_(0.016666668f * GetPrmMoveSpeed());
    bcs::FVEC3 pos(GetPos());
    MoveRunXZ_(pWork->m_fMoveRotY, fSpeed, pWork->m_fSpeed);
    if (!bWalk) {
        m_fJumpUnk220 += m_fFrameStep;
        if (m_fJumpUnk220 > 10.0f) {
            Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
        }
    } else {
        if (m_fJumpUnk220 > 10.0f) {
            Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
        }
        m_fJumpUnk220 = 0.0f;
    }
    pWork->m_bAccel = 0;
}

const msw::CHR_CORE_BOSS::CC_PARAM_BOSS *msw::CChrCoreBoss::GetCcParam_() const
{
    return m_pCcParam;
}

void msw::CChrCoreBoss::SubStateRunReceivePose_(STATE_WORK_RUN *pWork)
{
    if (pWork->m_bRun == 0) {
        FsmqNext();
        FsmqRetry();
        return;
    }

    float fRot = GetReceiveBaseDir_();
    long nDir = Util_ChangeRot4(WrapRad(fRot - pWork->m_fRotY));
    if (nDir == 2) {
        RotateYRate_(fRot, 0.2f);
    } else {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }

    long nMotNow = pWork->m_nMot;
    if (pWork->m_nStep == 0) {
        pWork->m_nStep++;
        nMotNow = -1;
    }
    long nMot = s_RunReceivePoseMot[nDir];
    if (nMotNow != nMot) {
        if (nMotNow >= 0) {
            AnimPlay(3, nMot, 1, 4.0f, AnimGetFrame(3));
        } else {
            AnimPlay(3, nMot, 1, 4.0f, 0.0f);
        }
    }
    float fSpeedMax = pWork->m_fSpeedMax;
    pWork->m_nDir = nDir;
    pWork->m_nMot = nMot;

    float fAccel = m_fFrameStep * (m_fTimeScale * (0.1f * fSpeedMax));
    pWork->m_fSpeed += fAccel;
    pWork->m_fSpeed = Clamp(pWork->m_fSpeed, 0.0f, fSpeedMax);
    float fSpeedRate = pWork->m_fSpeed;
    AnimSetSpeed(3, Clamp(1.0f / m_fScaleBase, 1.0f, 2.0f) * (fSpeedMax * GetCcParam_()->m_Unk2C));

    float fTurn = WrapRad(pWork->m_fRotY - pWork->m_fMoveRotY);
    if (fTurn >= 3.1415927f) {
        fTurn -= 6.2831855f;
    }
    float fStep = m_fTimeScale * (0.5f * fTurn * m_fFrameStep);
    if (fTurn < 0.0f) {
        fTurn = fStep < fTurn ? fTurn : fStep;
    } else {
        fTurn = fStep > fTurn ? fTurn : fStep;
    }
    pWork->m_fMoveRotY = WrapRad(pWork->m_fMoveRotY + fTurn);

    float fSpeed = CalcContiditionMoveSpeed_(0.016666668f * GetPrmMoveSpeed());
    bcs::FVEC3 pos(GetPos());
    MoveRunXZ_(pWork->m_fMoveRotY, fSpeed, fSpeedRate);
    m_fJumpUnk220 += m_fFrameStep;
    if (m_fJumpUnk220 > 10.0f) {
        Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
    }
    pWork->m_bRun = 0;
}

void msw::CChrCoreBoss::SubStateRunReceiveStance_(STATE_WORK_RUN *pWork)
{
    if (pWork->m_bRun == 0) {
        FsmqNext();
        FsmqRetry();
        return;
    }

    float fRot = GetReceiveBaseDir_();
    long nDir = Util_ChangeRot4(WrapRad(fRot - pWork->m_fRotY));
    RotateYRate_(fRot, 0.2f);

    long nMotNow = pWork->m_nMot;
    if (pWork->m_nStep == 0) {
        pWork->m_nStep++;
        nMotNow = -1;
    }
    long nMot = s_RunReceiveStanceMot[nDir];
    if (nMotNow != nMot) {
        if (nMotNow >= 0) {
            AnimPlay(3, nMot, 1, 4.0f, AnimGetFrame(3));
        } else {
            AnimPlay(3, nMot, 1, 4.0f, 0.0f);
        }
    }
    float fSpeedMax = pWork->m_fSpeedMax;
    pWork->m_nDir = nDir;
    pWork->m_nMot = nMot;

    float fAccel = m_fFrameStep * (m_fTimeScale * (0.1f * fSpeedMax));
    pWork->m_fSpeed += fAccel;
    pWork->m_fSpeed = Clamp(pWork->m_fSpeed, 0.0f, fSpeedMax);
    float fSpeedRate = pWork->m_fSpeed;
    AnimSetSpeed(3, Clamp(1.0f / m_fScaleBase, 1.0f, 2.0f) * (fSpeedMax * GetCcParam_()->m_Unk2C));

    float fTurn = WrapRad(pWork->m_fRotY - pWork->m_fMoveRotY);
    if (fTurn >= 3.1415927f) {
        fTurn -= 6.2831855f;
    }
    float fStep = m_fTimeScale * (0.5f * fTurn * m_fFrameStep);
    if (fTurn < 0.0f) {
        fTurn = fStep < fTurn ? fTurn : fStep;
    } else {
        fTurn = fStep > fTurn ? fTurn : fStep;
    }
    pWork->m_fMoveRotY = WrapRad(pWork->m_fMoveRotY + fTurn);

    MoveRunXZ_(pWork->m_fMoveRotY, CalcContiditionMoveSpeed_(0.016666668f * (0.3f * GetPrmMoveSpeed())),
               fSpeedRate);
    m_fJumpUnk220 += m_fFrameStep;
    if (m_fJumpUnk220 > 10.0f) {
        Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
    }
    pWork->m_bRun = 0;
    UnusedReferencesA(fSpeedMax);
}

long msw::CChrCoreBoss::SubStateGetNeutralPoseType_()
{
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 0;
    }
    if (pBall->IsScored()) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return 0;
    }

    // A pass from a team mate that can be tossed on.
    if (pBall->IsPassMode() && pBall->GetTouchNum() >= 1 && pBall->GetShotTeamIdx() == GetTeamNo() &&
        pBall->GetShotChrIdx() != GetPlayerNo() && pBall->GetTouchDownFrame(pBall->GetBgCollRadius()) <= 60.0f &&
        pBall->GetPos().y >= m_fTossHeight + GetPosY() &&
        IsInnerXZ(pBall->GetTargetPos(), GetCcParam_()->m_Unk60)) {
        return 1;
    }

    if (pBall->IsPassMode() || pBall->IsShootMode()) {
        float fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
        bcs::FVEC3 pos;
        pBall->GetVolleyPosAfterFrame(pos, fFrame, 1);
        float fMax = 90.0f;
        if (pBall->GetTouchNum() >= 1 || pBall->IsAttach()) {
            if (fMax > 60.0f) {
                fMax = 60.0f;
            }
        }
        if (IsInnerXZ(pos, GetCcParam_()->m_Unk60) && fFrame <= fMax) {
            return 2;
        }
    }
    return 0;
}

float msw::CChrCoreBoss::GetReceiveBaseDir_()
{
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return GetRotY();
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return GetSpikeTargetPosRot_(0);
    }
    return WrapRad(GetRotY_ToPosXZ(pBall->GetPosD4()));
}

long msw::CChrCoreBoss::GetReceivePoseDir_(float fDir)
{
    UnusedReferencesB(fDir);
    long nPose = 0;
    CBall *pBall = GetTargetBall_();
    if (pBall != 0 && !IsBossRightBallTouch(pBall->GetBallNo())) {
        return 0;
    }
    if (pBall != 0) {
        bcs::FVEC3 ballPos = pBall->GetPosD4();
        bcs::FVEC3 target = pBall->GetTargetPos();
        bcs::FVEC3 pos = GetPos();
        bcs::FVEC3 ofs;
        bcs::BCS_VecZ_RotY(ofs, 0.4f, GetRotY_ToPosXZ(ballPos));
        pos.x += ofs.x;
        pos.z += ofs.z;

        float fDist;
        bcs::FVEC3 cross;
        if (GetCrossPosCircleXZ(ballPos, target, pos, GetCcParam_()->m_Unk58, fDist, cross)) {
            ofs.x = target.x - pos.x;
            ofs.z = target.z - pos.z;
            float fDistSq = ofs.x * ofs.x + ofs.z * ofs.z;
            float fNear = GetCcParam_()->m_Unk54;
            if (!(fDistSq <= GetCcParam_()->m_Unk54 * fNear)) {
                fDist = GetNearestPosOnSegXZ(pos, ballPos, target, cross);
                if (fDist >= 1.0f) {
                    float fFar = GetCcParam_()->m_Unk5C;
                    if (fDistSq <= GetCcParam_()->m_Unk5C * fFar) {
                        nPose = 1;
                    } else {
                        nPose = 2;
                    }
                } else {
                    nPose = 7;
                }
            }
        } else {
            ofs.z = target.z - pos.z;
            ofs.x = target.x - pos.x;
            float fDistSq = ofs.x * ofs.x + ofs.z * ofs.z;
            float fRot = WrapRad(fDir - WrapRad(nw4r::math::Atan2Rad(ofs.x, ofs.z)));
            float fFar = GetCcParam_()->m_Unk5C;
            float fFarSq = GetCcParam_()->m_Unk5C * fFar;
            if (fRot <= 0.7853982f || fRot >= 5.497787f) {
                if (fDistSq <= fFarSq) {
                    nPose = 1;
                } else {
                    nPose = 2;
                }
            } else if (fRot <= 3.1415927f) {
                if (fDistSq <= fFarSq) {
                    nPose = 3;
                } else {
                    nPose = 4;
                }
            } else {
                if (fDistSq <= fFarSq) {
                    nPose = 5;
                } else {
                    nPose = 6;
                }
            }
        }
    }
    return nPose;
}

long msw::CChrCoreBoss::GetReceivePoseMotID_(long nPose, long nPart)
{
    return s_ReceivePoseMot[nPose][nPart];
}

float msw::CChrCoreBoss::GetTossBaseDir_()
{
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return GetRotY();
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return GetSpikeTargetPosRot_(0);
    }
    return WrapRad(GetRotY_ToPosXZ(pBall->GetPosD4()));
}

long msw::CChrCoreBoss::GetTossPoseDir_(float)
{
    return 0;
}

long msw::CChrCoreBoss::GetTossPoseMotID_(long nPose, long nPart)
{
    return s_TossPoseMot[nPose][nPart];
}

float msw::CChrCoreBoss::GetJumpTossBaseDir_()
{
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return GetRotY();
    }
    if (!IsBossRightBallTouch(pBall->GetBallNo())) {
        return GetSpikeTargetPosRot_(0);
    }
    return WrapRad(GetRotY_ToPosXZ(pBall->GetPosD4()));
}

long msw::CChrCoreBoss::GetJumpTossPoseDir_(float)
{
    return 0;
}

long msw::CChrCoreBoss::GetJumpTossPoseMotID_(long nPose, long nPart)
{
    return s_JumpTossPoseMot[nPose][nPart];
}

void msw::CChrCoreBoss::CtorStateTurn()
{
    STATE_WORK_TURN *pWork = FsmqGetWork<STATE_WORK_TURN>();

    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    SetMoveMotType(0);

    long nMot = 4;
    float fTurn = WrapRad(pWork->m_fRotY - GetRotY());
    if (fTurn > 3.1415927f) {
        fTurn -= 6.2831855f;
    }
    if (fTurn < 0.0f) {
        nMot = -4;
    }
    pWork->m_fFrameMax = GetCcParam_()->m_Unk20;
    pWork->m_fFrame = 0.0f;
    pWork->m_fSpeedRate = 1.0f;
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
    float fNum = AnimGetFrameNum(3);
    float fSpeed = 1.0f;
    if (pWork->m_fFrameMax > 0.0f) {
        fSpeed = fNum / pWork->m_fFrameMax;
    }
    AnimSetSpeed(3, fSpeed);

    pWork->m_Move.m_nType = 11;
    pWork->m_Move.m_fFrame = 0.0f;
    pWork->m_Move.m_fFrameMax = pWork->m_fFrameMax;
    pWork->m_Move.m_fDist = pWork->m_fSpeedRate * GetCcParam_()->m_Unk24;
    Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fMoveRotY);
    CalcRotParam_(pWork->m_Rot, pWork->m_fRotY, pWork->m_fFrameMax);
    PlayTurnSe(0);
    Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
}

void msw::CChrCoreBoss::LoopStateTurn()
{
    STATE_WORK_TURN *pWork = FsmqGetWork<STATE_WORK_TURN>();

    pWork->m_fFrame += m_fTimeScale * m_fFrameStep;
    if (pWork->m_fFrame > pWork->m_fFrameMax) {
        SetRotY(pWork->m_fRotY);
        FsmqNext();
        FsmqRetry();
        return;
    }
    Util_DoDirMove(pWork->m_Move, pWork->m_fFrame, pWork->m_DirMove, 1);
    RotateYParam_(pWork->m_Rot);
    ClearRevise();
}

void msw::CChrCoreBoss::DtorStateTurn()
{
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
}

void msw::CChrCoreBoss::CtorStateItemShoot()
{
    static const float infInterFrame = 8.0f;
    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();

    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), infInterFrame);
    PrepareShotStockItem(infInterFrame);
    AnimPlay(3, 0x93, 0, infInterFrame, 6.0f);
    pWork->m_fThrowFrame = GetBallShotTrgFrame_(0x93, 18.0f);
    pWork->m_fStartRotY = GetRotY();
    pWork->m_fRotY = GetItemThrowDir(pWork->m_fRotY);
    pWork->m_nStep = 0;
}

void msw::CChrCoreBoss::LoopStateItemShoot()
{
    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();

    if (pWork->m_nStep == 0) {
        float fThrowFrame;
        if (IsAnimEnd(3) || AnimGetFrame(3) >= (fThrowFrame = pWork->m_fThrowFrame)) {
            unsigned long nFlag = 1 << GetPlayerNo();
            pWork->m_fRotY = GetItemThrowDir(pWork->m_fRotY);
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
            ThrowItem(dir, nFlag, (m_nStatusFlag >> 10) & 1);
            SetRotY(pWork->m_fRotY);
            pWork->m_nStep++;
            return;
        }

        // Turns to the throw as the motion comes up to it.
        float fTurn = WrapRad(pWork->m_fRotY - pWork->m_fStartRotY);
        float fFrame = AnimGetFrame(3);
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
        SetRotY(WrapRad(fRot + pWork->m_fStartRotY));
        return;
    }

    if (IsAnimEnd(3)) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreBoss::DtorStateItemShoot()
{
}

void msw::CChrCoreBoss::CtorStateMotPlay()
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
            FsmqNext();
            FsmqRetry();
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

void msw::CChrCoreBoss::LoopStateMotPlay()
{
    STATE_WORK_MOT_PLAY *pWork = FsmqGetWork<STATE_WORK_MOT_PLAY>();

    m_UnkEC &= ~0x2;
    if (pWork->m_nFlag & 4) {
        float fCamRotY = m_fCamRotY;
        if (pWork->m_nFlag & 8) {
            RotateYRate_(WrapRad(3.1415927f + fCamRotY), 0.25f);
        } else {
            SetRotY(3.1415927f + fCamRotY);
        }
    }
    if (pWork->m_nStep == 0 && IsAnimEnd(3)) {
        if (pWork->m_nNextMot >= 0) {
            AnimPlay(3, pWork->m_nNextMot, 1, 0.0f, 0.0f);
        }
        pWork->m_nStep++;
    }
    if (pWork->m_nFlag & 0x10) {
        if (pWork->m_fTime > 0.0f) {
            pWork->m_fTime -= m_fFrameStep;
        }
        if (pWork->m_fTime <= 0.0f) {
            FsmqNext();
            FsmqRetry();
        }
    }
}

void msw::CChrCoreBoss::DtorStateMotPlay()
{
}
