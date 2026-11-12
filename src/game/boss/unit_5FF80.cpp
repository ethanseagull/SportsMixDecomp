// msw::CChrCoreBoss's dunk state and air trick state (the boss battle's
// team special move), and the two arts event members after them. The file
// ends in its static initialiser (0x636C0).
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcBase.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // The dunk's motions, by kind of dunk (STATE_WORK_DUNK::m_nType): the
    // start, the run-up loop, the jump, the dunk, the hang and the landing.
    // Each comes as a pair, the motion and the mirrored one (a negative id).
    enum {
        DUNK_MOT_START,
        DUNK_MOT_LOOP,
        DUNK_MOT_JUMP,
        DUNK_MOT_DUNK,
        DUNK_MOT_HANG,
        DUNK_MOT_LAND,
        DUNK_MOT_NUM
    };
    const short s_DunkMot[4][DUNK_MOT_NUM][2] = {
        { { 0x2C, -0x2C }, { 0x30, -0x30 }, { 0x2F, -0x2F }, { 0x38, -0x38 }, { 0x3B, -0x3B }, { 0x3E, -0x3E } },
        { { 0x2E, -0x2E }, { 0x35, -0x35 }, { 0x36, -0x36 }, { 0x39, -0x39 }, { 0x3C, -0x3C }, { 0x3F, -0x3F } },
        { { 0x2C, -0x2C }, { 0x30, -0x30 }, { 0x34, -0x34 }, { 0x37, -0x37 }, { 0x3A, -0x3A }, { 0x3D, -0x3D } },
        { { 0x2D, -0x2D }, { 0x31, -0x31 }, { 0x32, -0x32 }, { 0x39, -0x39 }, { 0x3C, -0x3C }, { 0x3F, -0x3F } },
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

    // (guessed helper) The larger of a and b.
    inline float MaxF(float a, float b)
    {
        return a < b ? b : a;
    }

    // (guessed helper) The smaller of a and b.
    inline float MinF(float a, float b)
    {
        return a > b ? b : a;
    }

    // (guessed helper) Not zero when character type nType is neither 5
    // nor 6.
    inline int IsOtherThanType5And6(long nType)
    {
        int bOther = 0;
        if (nType < 5 || nType > 6) {
            bOther = 1;
        }
        return bOther;
    }

    // (guessed helper) The distance from a to b in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &a, const bcs::FVEC3 &b)
    {
        bcs::FVEC3 a2(a.x, 0.0f, a.z);
        bcs::FVEC3 b2(b.x, 0.0f, b.z);
        return bcs::FVEC3Dist(&a2, &b2);
    }

    // (guessed helper) Not zero while the character's motion plays
    // mirrored: the second index of s_DunkMot.
    inline int IsMotFlip(const msw::CChrCore *pChr)
    {
        return pChr->GetMdl()->IsAnmFlipPlay(3, 0);
    }

    // (guessed helper) The character's motion id, without the sign of a
    // mirrored motion.
    inline long GetUnflippedMotID(const msw::CChrCore *pChr)
    {
        return IsMotFlip(pChr) ? -pChr->AnimGetMotID(3, 0) : pChr->AnimGetMotID(3, 0);
    }

    // (guessed helper) rOut = v * fScale by way of a temporary: the
    // callers show the product written to one vector on the stack and
    // copied float by float to a second, as bcs::FVEC3Sum does for a sum
    // (most likely an operator* in the original).
    inline void ScaleVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &v, float fScale)
    {
        bcs::FVEC3 product;
        bcs::FVEC3Scale(&product, &v, fScale);
        rOut = product;
    }

    // (guessed helper) The length of v in the XZ plane.
    inline float LenXZ(bcs::FVEC3 v)
    {
        bcs::FVEC3 xz(v.x, 0.0f, v.z);
        return bcs::FVEC3Len(&xz);
    }

    // (no known user) Set by the file's static initialiser, so its value
    // came from an inline function (guess).
    inline float DegToRad(float fDeg)
    {
        return fDeg * 0.017453292f;
    }
    float s_fDunkUnkRot = DegToRad(70.0f);
}

// The dunk's work in the state queue (names guessed).
struct msw::CChrCoreBoss::STATE_WORK_DUNK {
    long m_nStep;            // 0x00: 0, 1 run-up, 2-4 dunk, 5 landing, 6-7 hang and drop
    long m_nType;            // 0x04: the row of s_DunkMot
    long m_nUnk08;           // 0x08
    long m_nLoop;            // 0x0C: run-up loops left
    // 0x10: 0x1 hanging on, 0x2 the jump has started, 0x40 jumping to the
    // dunk, 0x400 the run-up is timed instead of counted in loops
    unsigned long m_nFlag;
    float m_fFrame;          // 0x14: frames since the run-up began
    float m_fRestFrame;      // 0x18: frames left to the dunk
    float m_fFrameMax;       // 0x1C: frames of the run-up and jump
    float m_fSpeedRate;      // 0x20
    float m_fTime;           // 0x24
    float m_fMotSpeed;       // 0x28: the play speed of the motions
    bcs::FVEC3 m_Target;     // 0x2C: the point dunked at
    bcs::FVEC3 m_Mid;        // 0x38: the point homed in on (y: its range)
    bcs::FVEC3 m_Base;       // 0x44: the point dunked at, again
    bcs::FVEC3 m_Vel;        // 0x50: the move while hanging and dropping
};

// The air trick's work in the state queue (names guessed).
struct msw::CChrCoreBoss::STATE_WORK_AIR_TRICK {
    long m_nNo;              // 0x00: 0 for the leader, then 1, 2, ...
    long m_nNum;             // 0x04: characters in the air trick
    float m_fWait;           // 0x08: frames before the trick starts
    long m_nStep;            // 0x0C
    unsigned long m_nFlag;   // 0x10: bit 0 once the ball is shot
    unsigned char m_Unk14[0x18 - 0x14];
    float m_fUnk18;          // 0x18
    bcs::FVEC3 m_MovePos;    // 0x1C: where the move goes
    float m_fRotY;           // 0x28: towards the dunk target
    float m_fMoveFrame;      // 0x2C
    CC_DATA::CHR_PARAM::MOVE_PARAM m_Move; // 0x30
    DIR_MOVE m_DirMove;      // 0x40
    ROT_LERP1 m_Rot;         // 0x50
    long m_nUnk60;           // 0x60
    float m_fTrickFrame;     // 0x64: frames of the trick
    float m_fTrickTime;      // 0x68: frames into the trick
    float m_fMotFrame;       // 0x6C: frames of the trick's motions
    float m_fMotSpeed;       // 0x70
    long m_nReadyStep;       // 0x74
    unsigned char m_Unk78[0x7C - 0x78];
    short m_bAdsorbStop;     // 0x7C
    short m_bAdsorbEnd;      // 0x7E
    float m_fAdsorbTime;     // 0x80
    float m_fAdsorbFrame;    // 0x84
    bcs::FVEC3 m_AdsorbOfs;  // 0x88: where the ball ends up, from the character
};

// ---- Dunk ---------------------------------------------------------------

void msw::CChrCoreBoss::CtorStateDunk()
{
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();

    pWork->m_nFlag |= 0x1800;
    float fArtsShots = 15.0f;
    if (fArtsShots > 0.0f) {
        AddArtsGauge_(1.0f / fArtsShots, -1.0f);
    }
    CtorStateDunk_(pWork);
    CChrMan::NpcSendMessage(0, 0, GetPlayerNo(), 0, 0);
    CChrMan::NpcSendMessage(0xF, 0, GetPlayerNo(), 0, 0);
}

void msw::CChrCoreBoss::CtorStateDunk_(STATE_WORK_DUNK *pWork)
{
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(1);
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;

    const CHR_CORE_BOSS::CC_PARAM_BOSS *pParam = GetCcParam_();
    pWork->m_nType = 0;
    long nFrame = pParam->m_Unk1D0;
    float fHeight = pParam->m_Unk1CC;
    SubStateDunkCtor_(pWork, nFrame, 0, fHeight,
                      s_DunkMot[pWork->m_nType][DUNK_MOT_START][(m_nStatusFlag >> 2) & 1]);

    CBall *pBall = GetAttachBall();
    if (pBall != 0) {
        CGame::SendMessage(0x34, GetPlayerNo(), 3, pBall->GetBallNo());
    }
    Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(0x30), GetPlayerNo(), 3, 0, 0);
    pWork->m_nStep = 0;
}

void msw::CChrCoreBoss::LoopStateDunk()
{
    LoopStateDunk_(FsmqGetWork<STATE_WORK_DUNK>());
}

// Asks the driver once to let go of the run-up, when the character is
// away from the target.
inline void msw::CChrCoreBoss::LoopStateDunk_(STATE_WORK_DUNK *pWork)
{
    if (pWork->m_nStep == 0 || pWork->m_nStep == 1) {
        static const float fMinDist = 0.5f;
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &GetPos(), &pWork->m_Target);
        if (!(fMinDist > LenXZ(diff)) && !(pWork->m_nFlag & 2) &&
            DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x2B), 0, 0, 0, 0) != 0) {
            pWork->m_nFlag |= 2;
        }
    }
    SubStateDunkCmnMove_(pWork);
}

void msw::CChrCoreBoss::DtorStateDunk()
{
    m_nStatusFlag &= ~0x43000000;
    Eff_Stop(10);
    Eff_Stop(11);
    Eff_Stop(12);
    Eff_StopForce(11);
    ArtsOptDestroy();
    CChrMan::NpcSendMessage(0, 1, GetPlayerNo(), 0, 0);
    CChrMan::NpcSendMessage(0xF, 1, GetPlayerNo(), 0, 0);
}

void msw::CChrCoreBoss::GetAirTrickTargetPos_(bcs::FVEC3 &rTarget, bcs::FVEC3 &rBase, bcs::FVEC3 &rMid,
                                              float fFrame) const
{
    GetCompPos(rMid);
    bcs::FVEC3 target;
    GetDunkTargetPosRot_(&target);
    rTarget = target;
    rBase = target;

    // From the target towards the character, half a unit to one unit out
    // (the nearer the later in the jump).
    rMid.x = rMid.x - rBase.x;
    rMid.z = rMid.z - rBase.z;
    rMid.y = 0.0f;
    bcs::BCS_Vec_Normalize(rMid, rMid);
    float fScale = 1.0f;
    if (fFrame > 0.0f) {
        if (fFrame >= 30.0f) {
            fScale = 0.5f;
        } else {
            float fRate = fFrame * (1.0f / 30.0f);
            fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
            fScale = 0.5f * (1.0f - fRate) + 0.5f;
        }
    }
    rMid.x *= fScale;
    rMid.z *= fScale;
    bcs::FVEC3Add(&rMid, &rMid, &rBase);
    rMid.y = fScale;
}

void msw::CChrCoreBoss::SubStateDunkCtor_(STATE_WORK_DUNK *pWork, float fFrame, long nType, float fHeight,
                                          long nMot)
{
    pWork->m_nType = nType;
    pWork->m_fRestFrame = fFrame;
    pWork->m_fFrameMax = fFrame;
    pWork->m_nFlag = 0;
    pWork->m_fFrame = 0.0f;
    pWork->m_nUnk08 = 0;
    pWork->m_fTime = 0.0f;
    pWork->m_fSpeedRate = 1.0f;
    GetAirTrickTargetPos_(pWork->m_Target, pWork->m_Base, pWork->m_Mid, 0.0f);
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_fJumpUnk220 = 0.0f;
    float fRest = pWork->m_fRestFrame;
    Util_SetJumpParam(fRest + 1.0f, GetPosY(), pWork->m_Target.y, fHeight);
    Util_DoJumpMove(m_fFrameStep);
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);

    // The run-up loops until the start, loop and jump motions fill the
    // frames (or, timed, plays them faster to fit).
    int nFlip = nMot < 0;
    long nStart = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_START][nFlip]);
    long nLoop = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_LOOP][nFlip]);
    long nJump = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][nFlip]);
    long nTotal = nStart + nLoop + nJump;

    switch (pWork->m_nType) {
        case 0:
            if (GetCcParam_()->m_Unk1D4 == 0) {
                pWork->m_nFlag |= 0x400;
            }
            WSE_Play(0, 0, 0, -1);
            break;
        case 1:
        case 3:
            if (GetCcParam_()->m_Unk1F0 == 0) {
                pWork->m_nFlag |= 0x400;
            }
            break;
    }

    if (pWork->m_nFlag & 0x400) {
        if (nStart + nJump > static_cast<long>(pWork->m_fFrameMax)) {
            pWork->m_nLoop = 0;
            pWork->m_fMotSpeed = (nStart + nJump) / pWork->m_fFrameMax;
        } else {
            pWork->m_fMotSpeed = 1.0f;
        }
    } else {
        pWork->m_nLoop = 1;
        if (nTotal < static_cast<long>(pWork->m_fFrameMax)) {
            long nRest = static_cast<long>(pWork->m_fFrameMax) - nTotal;
            long nCount = nRest / nLoop;
            if (nRest % nLoop != 0) {
                nCount++;
            }
            pWork->m_nLoop += nCount;
            nTotal += nLoop * nCount;
        }
        pWork->m_fMotSpeed = nTotal / pWork->m_fFrameMax;
    }
    AnimSetSpeed(3, pWork->m_fMotSpeed);
    PlayVoice(0x15, 0);
}

void msw::CChrCoreBoss::SubStateDunkCmnLandInit_(STATE_WORK_DUNK *pWork)
{
    pWork->m_nFlag &= ~1;
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    PlayLandSe(0);
    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
    if (m_nStatusFlag & 1) {
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    } else {
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    }
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0x40000000;
    m_nStatusFlag &= ~0x01000000;

    long nMot;
    if (IsMotFlip(this)) {
        nMot = -AnimGetMotID(3, 0);
    } else {
        nMot = AnimGetMotID(3, 0);
    }
    AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_LAND][nMot < 0], 0, 0.0f, 0.0f);
    pWork->m_nStep = 5;
}

// One step of the run-up: count the frames down and home in on the point
// in front of the target, turning to the target.
inline void msw::CChrCoreBoss::DunkHomingMove_()
{
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();
    float fMove = m_fFrameStep * (pWork->m_fSpeedRate * m_fTimeScale);
    float fDist = GetDist_ToPosXZ(pWork->m_Mid);
    pWork->m_fRestFrame -= fMove;
    float fRest = pWork->m_fRestFrame;
    if (fDist < pWork->m_Mid.y) {
        fRest = fRest > 8.0f ? 8.0f : fRest;
    }
    if (fRest < 1.0f) {
        fRest = 1.0f;
    }
    float fRate = fDist / fRest;
    if (fRate > 0.2f) {
        fRate = 0.2f;
    }
    bcs::FVEC3 pos = GetPos(); // (not used)
    RotateYToPosRate_(pWork->m_Target, 0.125f);
    Util_DoHomingMoveXZ(pWork->m_Mid, fRate * fMove);
}

void msw::CChrCoreBoss::SubStateDunkCmnMove_(STATE_WORK_DUNK *pWork)
{
    static const float fScale = 0.02f;

    bcs::FVEC3 prevPos = GetPos();
    m_fJumpVelZ = 0.0f;
    m_fJumpVelX = 0.0f;
    float fTimeStep = m_fTimeScale * m_fFrameStep;
    pWork->m_fTime += fTimeStep;

    // The run-up motions: start, loop, then the jump.
    switch (AnimGetMotID(3, 0)) {
        case 0x2C:
        case 0x2D:
        case 0x2E:
            if (!IsAnimEnd(3)) {
                break;
            }
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_LOOP][IsMotFlip(this)], 1, 0.0f, 0.0f);
            AnimSetSpeed(3, pWork->m_fMotSpeed);
            // fall through
        case 0x30:
        case 0x31:
        case 0x35:
            if (pWork->m_nFlag & 0x400) {
                if (!(pWork->m_nFlag & 2)) {
                    float fRest = pWork->m_fFrameMax - pWork->m_fFrame;
                    if (fRest < 0.0f) {
                        fRest = 0.1f;
                    }
                    if (fRest >= AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)]) /
                                     pWork->m_fMotSpeed) {
                        break;
                    }
                }
            } else {
                if (!IsAnimLoop(3)) {
                    break;
                }
                if (--pWork->m_nLoop > 0) {
                    break;
                }
            }
            if (!(pWork->m_nFlag & 2)) {
                AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)], 0, 8.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fMotSpeed);
                break;
            }
            // fall through
        case 0x2F:
            if (pWork->m_nFlag & 2) {
                // Near the target: the jump of the dunk of kind 2.
                if (!(3.0f < DistXZ(pWork->m_Target, GetPos()))) {
                    float fRest = pWork->m_fFrameMax - pWork->m_fFrame;
                    if (fRest < 0.0f) {
                        fRest = 0.1f;
                    }
                    float fJump = AnimGetResFrameNum(s_DunkMot[2][DUNK_MOT_JUMP][IsMotFlip(this)]);
                    if (!(fRest > fJump)) {
                        pWork->m_nType = 2;
                        long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)];
                        float fSpeed = fJump / fRest;
                        if (fSpeed > 2.0f) {
                            fSpeed = 2.0f;
                        }
                        AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                        AnimSetSpeed(3, fSpeed);
                        PlayVoice(0x16, 0);
                        WSE_Play(1, 1, 0, -1);
                        pWork->m_nFlag |= 0x40;
                        pWork->m_nFlag &= ~1;
                    }
                }
            }
            break;
    }

    switch (pWork->m_nStep) {
        case 0: {
            long nMot = AnimGetMotID(3, 0);
            if (nMot < 0x2C || nMot > 0x2E || IsAnimEnd(3)) {
                pWork->m_nStep = 1;
            }
        }
            // fall through
        case 1: {
            if (GetChrId() == 4) {
                CEffectCluster *pEff = Eff_GetCluster(11);
                if (pEff != 0) {
                    bcs::FVEC3 rot(0.0f, GetRotY(), 0.0f);
                    pEff->SetRotateRad(&rot);
                }
            }
            GetAirTrickTargetPos_(pWork->m_Target, pWork->m_Base, pWork->m_Mid, pWork->m_fFrame);
            float fStep = m_fTimeScale * (pWork->m_fSpeedRate * m_fFrameStep);
            pWork->m_fFrame += fStep;
            DunkHomingMove_();
            if (!(pWork->m_fFrame >= pWork->m_fFrameMax && (pWork->m_nFlag & 0x40)) && Util_DoJumpMove(fStep)) {
                SubStateDunkCmnLandInit_(pWork);
            }
            // Dunk once the frames are up (after a kind 2 jump, once its
            // motion has ended).
            if (pWork->m_fFrame >= pWork->m_fFrameMax) {
                if (!(pWork->m_nFlag & 2) ||
                    (GetUnflippedMotID(this) == s_DunkMot[2][DUNK_MOT_JUMP][IsMotFlip(this)] && IsAnimEnd(3))) {
                    // At the target, half a unit in front of it.
                    Util_DoHomingMoveXZ(pWork->m_Mid, GetDist_ToPosXZ(pWork->m_Mid));
                    bcs::FVEC3 pos;
                    bcs::BCS_VecZ_RotY(pos, 0.5f, GetRotY_FromPosXZ(pWork->m_Target));
                    bcs::FVEC3Add(&pos, &pos, &pWork->m_Target);
                    SetPos(pos);
                    pWork->m_nStep = 2;
                }
            }
            break;
        }
        case 2:
        case 3:
        case 4:
            SubStateDunkCmnDunk_(pWork);
            break;
        case 6: {
            // Let go: drop from the hang.
            if (GetChrId() != 11) {
                ArtsOptDestroy();
            }
            float fBlend = 8.0f;
            pWork->m_nFlag &= ~1;
            if (GetChrId() == 10) {
                fBlend = 10.0f;
            }
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_HANG][IsMotFlip(this)], 0, fBlend, 0.0f);
            AnimSetSpeed(3, 1.0f);
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            // NOTE: the original copies m_Vel here and writes its x and z
            // back unchanged below (no effect; guess at the source).
            bcs::FVEC3 vel = pWork->m_Vel;
            Util_SetJumpParam(30.0f, GetPos().y, 0.0f, GetPos().y * 0.5f);
            Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(6));
            SetCollBodyPushRank_(4);
            SetCollBodyPushPower_(1);
            Eff_Stop(11);
            pWork->m_Vel.x = vel.x;
            pWork->m_Vel.z = vel.z;
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY());
            pWork->m_Vel.x = fScale * dir.x;
            pWork->m_Vel.z = fScale * dir.z;
            m_fJumpVelY = -0.05f;
            pWork->m_nStep = 7;
        }
            // fall through
        case 7: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            if (Util_DoJumpMove(fStep)) {
                SubStateDunkCmnLandInit_(pWork);
                SetCollBodyPushRank_(1);
                SetCollBodyPushPower_(5);
                pWork->m_nStep = 5;
            }
            break;
        }
        default:
            SubStateDunkCmnLand_(pWork);
            break;
    }

    m_fJumpVelX = GetPos().x - prevPos.x;
    m_fJumpVelZ = GetPos().z - prevPos.z;
}

void msw::CChrCoreBoss::SubStateDunkCmnDunk_(STATE_WORK_DUNK *pWork)
{
    static const float fSpeed = 1.3f;

    switch (pWork->m_nStep) {
        case 2: {
            long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_DUNK][IsMotFlip(this)];
            Util_DunkStart_(nMot, FsmqGetState() == STATE_DUNK ? 3 : 8, 8.0f, 1);
            Eff_Stop(10);
            Eff_Stop(11);
            pWork->m_nStep = 3;
        }
            // fall through
        case 3:
            if (!IsAnimEnd(3)) {
                break;
            }
            // Let go of the goal and drop backwards.
            m_UnkEC &= ~0x400;
            m_nStatusFlag &= ~0x02000000;
            SetDetach_();
            m_nStatusFlag &= ~0x40000000;
            m_nStatusFlag &= ~0x01000000;
            Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(6));
            SetCollBodyPushRank_(4);
            SetCollBodyPushPower_(1);
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_HANG][IsMotFlip(this)], 0, 8.0f, 0.0f);
            AnimSetSpeed(3, fSpeed);
            float fTop = 0.5f + GetPos().y;
            const bcs::FVEC3 &rPos = GetPos();
            Util_SetJumpParam(AnimGetFrameNum(3) / fSpeed, rPos.y, 0.0f, fTop);
            bcs::BCS_VecZ_RotY(pWork->m_Vel, 0.1f, WrapRad(3.1415927f + GetRotY()));
            pWork->m_nStep = 4;
            // fall through
        case 4: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            if (Util_DoJumpMove(fStep)) {
                SubStateDunkCmnLandInit_(pWork);
                SetCollBodyPushRank_(1);
                SetCollBodyPushPower_(5);
                pWork->m_nStep = 5;
            }
            break;
        }
    }
}

void msw::CChrCoreBoss::SubStateDunkCmnLand_(STATE_WORK_DUNK *)
{
    Util_DoJumpMove(m_fTimeScale * m_fFrameStep);
    if (GetPos().y <= 0.5f) {
        DropBall();
    }
    if (!(m_nStatusFlag & 0xC00) && IsAnimEnd(3)) {
        switch (AnimGetMotID(3, 0)) {
            case 0x3D:
                if (IsOtherThanType5And6(GetChrTypeID())) {
                    break;
                }
                // fall through
            case 0x3F:
                if (GetChrId() != 1) {
                    // Landed facing away from the goal: turn round.
                    float fRotY = WrapRad(3.1415927f + GetRotY());
                    Util_NodePatchRotY(fRotY);
                    SetRotY(fRotY);
                }
                break;
        }
        PlayLandSe(0);
        m_nStatusFlag &= ~0x20;
        FsmqNext();
        FsmqRetry();
    }
}

// The arguments are not used.
void msw::CChrCoreBoss::Util_DunkStart_(long, long, float, long)
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(5);
    SetCollBodyPushPower_(6);
    CBall *pBall = GetAttachBall();
    m_nStatusFlag |= 0x2000000;
    if (pBall != 0) {
        DetachBall();
    }
    if (pBall != 0) {
        // Shoot it at the boss, or two units down in front of the
        // character when there is no boss.
        bcs::FVEC3 ballPos = pBall->GetPos();
        const CNpcBase *pBoss = Util_GetBoss_();
        bcs::FVEC3 target;
        if (pBoss != 0) {
            target = pBoss->GetPos();
            target.y = 0.0f;
        } else {
            bcs::BCS_VecZ_RotY(target, 1.0f, GetRotY());
            target.y = -2.0f;
            bcs::FVEC3Add(&target, &target, &ballPos);
        }
        pBall->SetVsBossDunkShotMode(target, GetPlayerNo());
    }
}

// ---- Air trick ----------------------------------------------------------

void msw::CChrCoreBoss::CtorStateAirTrick()
{
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(6);
    SetCollBodyPushRank_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;

    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    pWork->m_nStep = 0;
    if (pWork->m_nNo == 0) {
        // The leader starts the event and takes the ball.
        CChrMan::NpcSendMessage(0, 0, GetPlayerNo(), 0, 0);
        CChrMan::NpcSendMessage(0xF, 0, GetPlayerNo(), 0, 0);
        CGame::ClearTeamArtsGauge(GetTeamNo());
        CGame::TrgArtsEvent(GetPlayerNo());
        CGame::SendMessage(0xD, GetPlayerNo(), 0, 0);
        CBall *pBall = GetTargetBall_();
        if (pBall != 0) {
            AttachBall(pBall->GetBallNo(), 4, 0.0f, 0x20);
        }
    }
}

void msw::CChrCoreBoss::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nStep = 1;
            break;
        case 1:
            if (pWork->m_nNo > 0) {
                AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
            } else {
                if (IsAnimEnd(3)) {
                    AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                }
                CChrMan::NpcSendMessage(0, 0, GetPlayerNo(), 0, 0);
                CChrMan::NpcSendMessage(0xF, 0, GetPlayerNo(), 0, 0);
                CGame::SendMessage(0x63, 6, 0, 0);
            }
            pWork->m_nStep = 2;
            break;
        case 2:
            if (pWork->m_nNo > 0) {
                if (IsAnimEnd(3)) {
                    AnimPlay(3, 0xA4, 1, 8.0f, 0.0f);
                }
            } else if (IsAnimEnd(3)) {
                AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
            }
            if (pWork->m_fWait >= 0.0f) {
                pWork->m_fWait -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fWait <= 0.0f) {
                    Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(1), 0);
                    if (pWork->m_nNo == 0) {
                        if (GetChrId() == 15) {
                            PlayVoice(2, 0);
                        } else {
                            PlayVoice(0x43, 0);
                        }
                    }
                    SubStateAirTrickStartTrick_(pWork);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 3:
            if (SubStateAirTrickIsReadyArtsShot_(pWork)) {
                SubStateAirTrickReadyArtsShot_(pWork);
                pWork->m_nStep = 4;
            }
            break;
        case 4:
            if (SubStateAirTrickIsStartArtsShot_(pWork)) {
                SubStateAirTrickStartArtsShot_(pWork);
                pWork->m_nStep = 5;
            }
            break;
        case 5:
            if (SubStateAirTrickIsEndArtsShot_(pWork)) {
                SubStateAirTrickStartArtsHitback_(pWork);
                pWork->m_nStep = 6;
            }
            break;
        case 6:
            if (SubStateAirTrickIsEndArtsShotAfter_(pWork)) {
                SubStateAirTrickDoLandStart_(pWork);
            }
            break;
        case 7:
            if (!IsAnimEnd(3)) {
                break;
            }
            pWork->m_nStep = 8;
            // fall through
        default:
            if (SubStateAirTrickIsOver_(pWork)) {
                bEnd = 1;
                if (pWork->m_nNo == 0) {
                    CChrMan::NpcSendMessage(0, 1, GetPlayerNo(), 0, 0);
                    CChrMan::NpcSendMessage(0xF, 1, GetPlayerNo(), 0, 0);
                    CGame::SendMessage(0xE, 0, 0, 0);
                    CGame::SendMessage(0x16, 0, 0, 0);
                    CGame::BgSetFadeIn(60.0f, 30.0f);
                }
            }
            break;
    }

    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreBoss::DtorStateAirTrick()
{
    m_nStatusFlag &= ~0x43000000;
    ArtsOptDestroy();
}

void msw::CChrCoreBoss::SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_nUnk60 = 0;
    pWork->m_fTrickTime = 0.0f;
    pWork->m_fTrickFrame = 60.0f;
    pWork->m_fMotFrame = 60.0f;
    pWork->m_fMotSpeed = 1.0f;
    pWork->m_fUnk18 = GetCcParam_()->m_Unk154 + GetCcParam_()->m_Unk158;
    SubStateAirTrickSetTrickFrameParam_(pWork, 0);
    SubStateAirTrickPlayTrickMotion_(pWork);
    SubStateAirTrickSetTrickJumpParam_(pWork, 0);
    SubStateAirTrickSetTrickMoveParam_(pWork, 0);
    CalcRotParam_(pWork->m_Rot, pWork->m_fRotY, SubStateAirTrickGetTrickRotFrame_(pWork));
}

void msw::CChrCoreBoss::SubStateAirTrickSetTrickFrameParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    float fMotFrame = SubStateAirTrickGetTrickMotFrame_(pWork, nTrick);
    fMotFrame = MaxF(1.0f, fMotFrame);
    pWork->m_fMotFrame = fMotFrame;
    float fFrame = SubStateAirTrickGetTrickDoFrame_(pWork, nTrick);
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
        pWork->m_fMotSpeed = 1.0f;
    } else {
        pWork->m_fMotSpeed = fMotFrame / fFrame;
    }
    pWork->m_fTrickFrame = fFrame;
    pWork->m_fTrickTime = 0.0f;
}

float msw::CChrCoreBoss::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long nTrick)
{
    float fFrame;
    if (nTrick == 0) {
        fFrame = 60.0f;
        fFrame += AnimGetResFrameNum(0x25);
        fFrame += AnimGetResFrameNum(0x2C);
    } else {
        fFrame = 0.0f;
        fFrame += AnimGetResFrameNum(0x2F);
    }
    return fFrame;
}

float msw::CChrCoreBoss::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return 0.0f;
}

void msw::CChrCoreBoss::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork->m_nNo == 0) {
        AnimPlay(3, 0x2C, 0, 8.0f, 0.0f);
    } else {
        AnimPlay(3, 0x25, 0, 8.0f, 0.0f);
    }
    if (pWork->m_nNo == 0) {
        CGame::SendMessage(0x63, 6, 1, 0);
    }
}

float msw::CChrCoreBoss::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return pWork->m_fTrickFrame;
}

void msw::CChrCoreBoss::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    if (nTrick == 0) {
        float fHeight = 4.0f;
        float fY = GetPosY();
        Util_SetJumpParam(pWork->m_fTrickFrame - 30.0f, fY, fHeight, MaxF(fHeight, fY));
        m_fJumpUnk224 = 30.0f;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fTop = GetPosY();
        float fHeight = 3.0f;
        float fY = GetPosY();
        Util_SetJumpParam(pWork->m_fTrickFrame, fY, fHeight, MaxF(fTop, fY));
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
    }
}

void msw::CChrCoreBoss::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    pWork->m_MovePos = GetPos();
    ClearMoveParamWork_(pWork->m_Move);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fMoveFrame = 0.0f;

    // The move goes 2.5 units (the first trick) or one unit from the dunk
    // target, away from it.
    // NOTE: as in the original, the move goes to the offset itself, not to
    // the target plus the offset (the sum is computed and not used).
    bcs::FVEC3 pos = GetPos();
    GetRotY();
    if (nTrick == 0) {
        bcs::FVEC3 target;
        float fRotY = GetDunkTargetPosRot_(&target);
        float fDist = 2.5f;
        bcs::BCS_VecZ_RotY(pos, fDist, WrapRad(3.1415927f + fRotY));
        bcs::FVEC3Add(&target, &target, &pos);
        pWork->m_MovePos = pos;
        pWork->m_fRotY = WrapRad(fRotY);
        pWork->m_Move.m_nType = 0xB;
        pWork->m_Move.m_fDist = DistXZ(pWork->m_MovePos, GetPos());
        pWork->m_Move.m_fFrame = 0.0f;
        pWork->m_Move.m_fFrameMax = pWork->m_fTrickFrame;
    } else {
        bcs::FVEC3 target;
        float fRotY = GetDunkTargetPosRot_(&target);
        float fDist = 1.0f;
        bcs::BCS_VecZ_RotY(pos, fDist, WrapRad(3.1415927f + fRotY));
        bcs::FVEC3Add(&target, &target, &pos);
        pWork->m_MovePos = pos;
        pWork->m_fRotY = WrapRad(fRotY);
        pWork->m_Move.m_nType = 0xC;
        pWork->m_Move.m_fDist = DistXZ(pWork->m_MovePos, GetPos());
        pWork->m_Move.m_fFrame = 0.0f;
        pWork->m_Move.m_fFrameMax = pWork->m_fTrickFrame;
    }
}

void msw::CChrCoreBoss::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_bAdsorbStop = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrame = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbTime = 0.0f;
    pWork->m_AdsorbOfs.x = 0.0f;
    pWork->m_AdsorbOfs.y = 0.0f;
    pWork->m_AdsorbOfs.z = 0.0f;
    float fFrame = 30.0f;
    fFrame -= GetCcParam_()->m_Unk150;
    fFrame = MaxF(1.0f, fFrame);
    pWork->m_fAdsorbFrame = fFrame;
    AttachBall(pBall->GetBallNo(), 4, fFrame, 0x20);
    m_nStatusFlag &= ~0x2C;
}

float msw::CChrCoreBoss::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return pWork->m_fTrickFrame;
}

// One step of the jump and, while it lasts, of the move to m_MovePos.
inline int msw::CChrCoreBoss::SubStateAirTrickDoJumpMove_(STATE_WORK_AIR_TRICK *pWork)
{
    int nRet = Util_DoJumpMoveEX(0);
    if (nRet == 1) {
        RotateYParam_(pWork->m_Rot);
        if (pWork->m_fMoveFrame < pWork->m_Move.m_fFrameMax) {
            pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_MovePos);
            Util_DoDirMove(pWork->m_Move, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
        }
    }
    return nRet;
}

int msw::CChrCoreBoss::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fTrickTime += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickTime = MinF(pWork->m_fTrickTime, pWork->m_fTrickFrame);
    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);
    if (pWork->m_fTrickTime >= pWork->m_fTrickFrame && pWork->m_nReadyStep >= 3) {
        return 1;
    }

    switch (pWork->m_nReadyStep) {
        case 0:
            if (IsAnimEnd(3)) {
                if (pWork->m_nNo != 0) {
                    AnimPlay(3, 0x2C, 0, 8.0f, 0.0f);
                } else {
                    AnimPlay(3, 0x30, 1, 0.0f, 0.0f);
                    pWork->m_nReadyStep++;
                }
                pWork->m_nReadyStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x30, 1, 0.0f, 0.0f);
                pWork->m_nReadyStep++;
            }
            break;
        case 2:
            if (!(m_nStatusFlag & 1)) {
                // Take the ball.
                CBall *pBall = GetTargetBall_();
                if (!pBall->IsAttach()) {
                    Eff_CreateAttach(10, 0x567, m_nEffNodeArts, 0, 0, 3, 0);
                    AttachBall(pBall->GetBallNo(), 4, 8.0f, 0);
                    PlaySePos(0xE0A, pBall->GetPos(), 0, -1, 0.0f, 1.0f, 0.0f);
                    m_nStatusFlag |= 1;
                    PlayVoice(0x12, 0);
                    if (pWork->m_nNo > 0) {
                        CGame::SendMessage(0x63, 6, pWork->m_nNo + 1, 0);
                    }
                    CGame::SetLooseBallModeFlag(0);
                }
            } else {
                pWork->m_nReadyStep++;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreBoss::SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *pWork)
{
    static const float fHeightMax = 10.0f;

    CBall *pBall = GetAttachBall();
    if (pBall != 0 && pWork->m_bAdsorbStop == 0) {
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_RotY(ofs, pWork->m_AdsorbOfs, pWork->m_fRotY);
        if (pWork->m_bAdsorbEnd == 0 && pWork->m_fAdsorbFrame > 0.0f) {
            // Pull the ball in, up along a quarter circle.
            float fFrame = pWork->m_fAdsorbFrame;
            pWork->m_fAdsorbTime += m_fFrameStep * m_fTimeScale;
            pWork->m_fAdsorbTime = MinF(pWork->m_fAdsorbTime, fFrame);
            float fRate = pWork->m_fAdsorbTime / fFrame;
            fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
            bcs::FVEC3Scale(&ofs, &ofs, fRate);
            float fHeight = fHeightMax * nw4r::math::CosRad(1.5707964f * fRate);
            fHeight = fHeight > fHeightMax ? fHeightMax : (fHeight < 0.0f ? 0.0f : fHeight);
            ofs.y += fHeight;
            if (fRate >= 1.0f) {
                pWork->m_bAdsorbEnd = 1;
                SubStateAirTrickTrgBallAdsorbEnd_(pWork);
            }
        }
        pBall->SetAttachPos(ofs, 0);
    }
}

void msw::CChrCoreBoss::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
}

void msw::CChrCoreBoss::SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *)
{
    CBall *pBall = GetAttachBall();
    if (pBall != 0) {
        pBall->TrgVolleyArtsReady(GetChrTypeID(), 1);
    }
}

int msw::CChrCoreBoss::SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *)
{
    return 1;
}

void msw::CChrCoreBoss::SubStateAirTrickStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 8.0f);
    SubStateAirTrickPlayArtsShotMotion_(pWork);
    SubStateAirTrickSetTrickFrameParam_(pWork, 1);
    SubStateAirTrickSetTrickJumpParam_(pWork, 1);
    SubStateAirTrickSetTrickMoveParam_(pWork, 1);
    CalcRotParam_(pWork->m_Rot, pWork->m_fRotY, AnimGetFrameNum(3));
}

void msw::CChrCoreBoss::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    AnimPlay(3, 0x2F, 0, 4.0f, 0.0f);
    PlayVoice(0x16, 0);
    WSE_Play(1, 1, 0, -1);
    if (!(m_nStatusFlag & 1)) {
        // Take the ball if it is still free.
        CBall *pBall = GetTargetBall_();
        if (!pBall->IsAttach()) {
            Eff_CreateAttach(10, 0x567, m_nEffNodeArts, 0, 0, 3, 0);
            AttachBall(pBall->GetBallNo(), 4, 0.0f, 0);
            PlaySePos(0xE0A, pBall->GetPos(), 0, -1, 0.0f, 1.0f, 0.0f);
            m_nStatusFlag |= 1;
            if (pWork->m_nNo > 0) {
                CGame::SendMessage(0x63, 6, pWork->m_nNo + 1, 0);
            }
            CGame::SetLooseBallModeFlag(0);
        }
    }
}

int msw::CChrCoreBoss::SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SubStateAirTrickDoJumpMove_(pWork);
    if (IsAnimEnd(3)) {
        Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(1));
        SubStateAirTrickShootBall_(pWork);
        return 1;
    }
    return 0;
}

void msw::CChrCoreBoss::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    DetachBall();
    if (pBall != 0) {
        int bLast = 0;
        pWork->m_nFlag |= 1;
        // NOTE: the original copies the ball's position and then passes a
        // vector it never sets; kept as it is.
        bcs::FVEC3 ballPos = pBall->GetPos();
        (void)ballPos;
        if (pWork->m_nNo == pWork->m_nNum - 1) {
            bLast = 1;
        }
        bcs::FVEC3 target;
        pBall->SetVsBossArtsDunkMode(target, GetPlayerNo(), bLast);
        CGame::StartSaturatedLinework(30.0f);
        long nShot = (bLast != 0) + 1;
        CChrMan::NpcSendMessage(0, 2, nShot, 0, 0);
        CChrMan::NpcSendMessage(0xF, 2, nShot, 0, 0);
        if (bLast) {
            CGame::SendMessage(0x63, 5, 0, 0);
        }
    }
}

void msw::CChrCoreBoss::SubStateAirTrickStartArtsHitback_(STATE_WORK_AIR_TRICK *pWork)
{
    AnimPlay(3, 0x3B, 0, 4.0f, 0.0f);
    pWork->m_Move.m_nType = 0xB;
    pWork->m_Move.m_fFrame = 0.0f;
    pWork->m_Move.m_fFrameMax = 60.0f;
    pWork->m_Move.m_fDist = 4.0f;
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fMoveFrame = 0.0f;
    CalcRotParam_(pWork->m_Rot, pWork->m_fRotY, 60.0f);

    // Knocked back eight units from the target.
    bcs::BCS_VecZ_RotY(pWork->m_MovePos, -8.0f, pWork->m_fRotY);
    const bcs::FVEC3 &rPos = GetPos();
    pWork->m_MovePos.x += rPos.x;
    pWork->m_MovePos.z += rPos.z;
    Util_SetJumpParam(60.0f, GetPosY(), 0.0f, 2.0f + GetPosY());
}

int msw::CChrCoreBoss::SubStateAirTrickIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    return SubStateAirTrickDoJumpMove_(pWork) >= 2;
}

void msw::CChrCoreBoss::SubStateAirTrickDoLandStart_(STATE_WORK_AIR_TRICK *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    AnimPlay(3, 0x3E, 0, 4.0f, 0.0f);
    PlayLandSe(0);
    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
    pWork->m_nStep = 7;
}

int msw::CChrCoreBoss::SubStateAirTrickIsOver_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork == 0) {
        return 1;
    }
    if (pWork->m_nNo > 0) {
        return 1;
    }

    // The leader waits until no one else is in the air trick.
    int bOver = 1;
    long nNum = CChrMan::GetChrCoreNum();
    for (long i = 0; i < nNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() &&
            pChr->FsmqGetState() == STATE_AIR_TRICK) {
            bOver = 0;
            break;
        }
    }
    if (bOver) {
        CGame::SendMessage(0x63, 7, 0, 0);
    }
    return bOver;
}

// ---- Arts event ---------------------------------------------------------

float msw::CChrCoreBoss::GetArtsEventRot()
{
    return 3.1415927f;
}

void msw::CChrCoreBoss::DoPlayArtsEvent()
{
    m_fScaleFrom = 1.0f;
    m_fScaleTo = 1.0f;
    m_fScaleRate = 0.0f;
    m_fScaleBase = 1.0f;
    m_fScaleUnk1B0 = 1.0f;
    Eff_EraseAll();
    if (m_nStatusFlag & 1) {
        AnimPlay(3, 0x94, 0, 0.0f, 0.0f);
    } else {
        AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
    }
    Eff_CreateSimple(0xF, GetPos(), GetRotY(), 3, 0, 0);
    PlaySe(0xC06, 1, -1, 0.0f, 1.0f, 0.0f);
    CBall *pBall = GetAttachBall();
    if (pBall != 0) {
        pBall->TrgVsBossArtsAttach(GetTeamNo(), GetPlayerNo(), GetChrTypeID(), 1);
    }
}
