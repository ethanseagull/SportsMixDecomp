// msw::CChrCoreVolley's receive state (a receive, a diving receive, a toss and
// a jump toss of the target ball, with the ball drag and the homing to the
// point the ball is taken at) and its sliding state.
// Written from BossF's game/boss/unit_1EA14.cpp (the receive and sliding
// states) and checked against VolleyF.
#include "game/msw/CChrCoreVolley.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameGuideMan.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CcEndureSmokeEffect.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

namespace unit_3B628_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float constant of this
// file with its own lis, which this compiler does when the first .rodata object of the file is
// weak (a function-local const). The module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi), and into
    // (-pi, pi] after that; an angle brought into [-pi, pi].
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

    inline float WrapRadPI(float rad)
    {
        rad = WrapRad(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    inline float WrapRadPi(float rad)
    {
        while (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        while (rad < -3.1415927f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // (guessed macro) x brought into [lo, hi]; the bounds and x are read
    // again where they are used, as the original does.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

    // The sliding state's work (names guessed).
    struct STATE_WORK_SLIDING {
        long m_nStep;       // 0x00
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fRotY;      // 0x08: the direction of the slide
        float m_fRotYTrg;   // 0x0C: the direction the stick asks for
        float m_fSpeedRate; // 0x10
        int m_bAccel;       // 0x14: the stick is held this frame
        bcs::FVEC3 m_Dir;   // 0x18: the direction a ball is sent in (only x and z read)
    };
}

void msw::CChrCoreVolley::CtorStateReceive()
{
    STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();

    SetMoveMotType(0);
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    ClearMoveParamWork_(pWork->m_MoveParam2);
    Util_SetDirMoveParam(pWork->m_DirMove2, -1.0f);
    pWork->m_fMoveFrame = 0.0f;
    pWork->m_fMoveFrame2 = 0.0f;
    pWork->m_RotLerp.m_afUnk[0] = -1.0f;
    pWork->m_fEndure = 0.0f;
    pWork->m_afCountWork[0] = 0.0f;
    pWork->m_afCountWork[1] = 0.0f;
    if (!(pWork->m_nFlag & 0x10)) {
        pWork->m_nKind = SubStateReceiveChoiceProcType_(pWork);
    }
    pWork->m_nStep = 0;
    pWork->m_fWaitFrame = 0.0f;

    switch (pWork->m_nKind) {
        case 1:
            Util_NodePatchWarpToTransNodeXZ();
            pWork->m_afCountWork[0] = 0.0f;
            pWork->m_afCountWork[1] = 40.0f;
            break;
        case 2:
            Util_NodePatchWarpToTransNodeXZ();
            pWork->m_afCountWork[0] = 0.0f;
            pWork->m_afCountWork[1] = 15.0f;
            if (!(pWork->m_nFlag & 0x20)) {
                pWork->m_nPose = GetTossPoseDir_(GetTossBaseDir_());
            }
            if (!(m_nStatusFlag & 1)) {
                SetBallCatchEnable_(6);
                pWork->m_fWaitFrame = SubStateReceiveGetHomingTargetPosToss_(
                    pWork, GetTargetBall_(), m_aTossCatchPos[pWork->m_nPose].m_Low,
                    m_aTossCatchPos[pWork->m_nPose].m_High, 0.0f, 2.0f, 9.0f, 1.5f);
                float fStart = 0.0f;
                if (pWork->m_fWaitFrame > fStart) {
                    AnimPlay(3, GetTossPoseMotID_(pWork->m_nPose, 0), 0, 8.0f, fStart);
                    pWork->m_nStep = 3;
                    pWork->m_fWaitFrame += 1.0f;
                } else {
                    float fLate = fabsf(pWork->m_fWaitFrame);
                    AnimPlay(3, GetTossPoseMotID_(pWork->m_nPose, 2), 0, 8.0f, fLate < fStart ? fStart : fLate);
                    pWork->m_fWaitFrame = 0.0f;
                    pWork->m_nStep = 2;
                }
            }
            break;
        case 3:
            Util_NodePatchWarpToTransNodeXZ();
            pWork->m_afCountWork[0] = 0.0f;
            pWork->m_afCountWork[1] = 15.0f;
            if (!(pWork->m_nFlag & 0x20)) {
                pWork->m_nPose = GetJumpTossPoseDir_(GetJumpTossBaseDir_());
            }
            if (!(m_nStatusFlag & 1)) {
                SetBallCatchEnable_(7);
            }
            break;
        case 0:
            Util_NodePatchWarpToTransNodeXZ();
            pWork->m_afCountWork[0] = 0.0f;
            pWork->m_afCountWork[1] = 15.0f;
            if (!(pWork->m_nFlag & 0x20)) {
                pWork->m_nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
            }
            SetBallCatchEnable_(4);
            if (!(m_nStatusFlag & 1)) {
                SetBallCatchEnable_(4);
                const CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *pParam = GetCcParam_();
                pWork->m_fWaitFrame = SubStateReceiveGetHomingTargetPosReceive_(
                    pWork, GetTargetBall_(), m_aReceiveCatchPos[pWork->m_nPose].m_Low,
                    m_aReceiveCatchPos[pWork->m_nPose].m_High, 0.0f, 7.0f, 16.0f, pParam->m_UnkD8);
                float fStart = 0.0f;
                if (pWork->m_fWaitFrame > fStart) {
                    AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 0), 0, 4.0f, fStart);
                    pWork->m_nStep = 4;
                } else {
                    float fLate = fabsf(pWork->m_fWaitFrame);
                    AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 2), 0, 4.0f, fLate < fStart ? fStart : fLate);
                    pWork->m_fWaitFrame = 0.0f;
                    pWork->m_nStep = 3;
                }
            } else if (pWork->m_nFlag & 0x200) {
                // Already holding the ball: step to where the pose takes it.
                bcs::FVEC3 pos;
                bcs::BCS_VecZ_RotY(pos, m_afReceiveDist[pWork->m_nPose], GetRotY_FromPosXZ(pWork->m_BallPos));
                pos.x += pWork->m_BallPos.x;
                pos.z += pWork->m_BallPos.z;
                const bcs::FVEC3 &rPos = GetPos();
                bcs::FVEC3 diff;
                diff.x = pos.x - rPos.x;
                diff.z = pos.z - rPos.z;
                float fDist = bcs::BCS_Vec_GetLenXZ(diff);
                pWork->m_MoveParam2.m_nType = 0;
                pWork->m_MoveParam2.m_fDist = fDist > 1.2f ? 1.2f : fDist;
                pWork->m_MoveParam2.m_fFrame = 0.0f;
                float fFrame = 5.0f;
                pWork->m_MoveParam2.m_fFrameMax = fFrame;
                float fRotY = GetRotY_ToPosXZ(pWork->m_BallPos);
                Util_SetDirMoveParam(pWork->m_DirMove2, WrapRad(fRotY));
                long nPose = pWork->m_nPose;
                float fRot = WrapRad(fRotY - m_afReceiveRot[nPose]);
                if (nPose == 7) {
                    fRot = WrapRad(3.1415927f + fRot);
                    if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                        fRot = WrapRad(3.1415927f + nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                    }
                }
                pWork->m_fLerpRotY = fRot;
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fLerpRotY, fFrame);
            }
            break;
    }
}

void msw::CChrCoreVolley::LoopStateReceive()
{
    STATE_WORK_RECEIVE *pWork = FsmqGetWork<STATE_WORK_RECEIVE>();
    int bEnd;

    switch (pWork->m_nKind) {
        case 1:
            bEnd = SubStateReceiveDoProcDiveReceive_(pWork);
            break;
        case 2:
            bEnd = SubStateReceiveDoProcToss_(pWork);
            break;
        case 3:
            bEnd = SubStateReceiveDoProcJumpToss_(pWork);
            break;
        case 4: {
            // No receive possible: a short shrug.
            int bOver = 0;
            if (pWork->m_nStep == 0) {
                Util_NodePatchWarpToTransNodeXZ();
                AnimPlay(3, 0x4E, 0, 8.0f, 0.0f);
                pWork->m_nStep = 3;
            } else if (IsAnimEnd(3)) {
                bOver = 1;
            }
            bEnd = bOver;
            break;
        }
        default:
            bEnd = SubStateReceiveDoProcReceive_(pWork);
            break;
    }
    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreVolley::DtorStateReceive()
{
    ArtsOptDestroy();
    SetEndure(0);
    SetBallCatchDisable_();
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
    Eff_StopGroup(3);
    m_nStatusFlag &= ~0x800;
    m_nStatusFlag &= ~0xC000000;
    m_UnkEC &= ~4;
}

int msw::CChrCoreVolley::SubStateReceiveDoCheckReactFlag_(STATE_WORK_RECEIVE *, CBall *pBall, unsigned long nReact)
{
    if ((m_nStatusFlag & 0x10000) && (nReact & 0x200000) && pBall != 0 &&
        pBall->GetShotTeamIdx() != GetTeamNo()) {
        nReact |= 0x800;
    }
    if (m_nComLevel == 1) {
        // The star state keeps only these reactions.
        nReact &= 0xFFF80000;
    }
    return nReact;
}

int msw::CChrCoreVolley::SubStateReceiveDoCheckDiveReactFlag_(STATE_WORK_RECEIVE *, CBall *pBall,
                                                             unsigned long nReact)
{
    if ((m_nStatusFlag & 0x10000) && (nReact & 0x200000) && pBall != 0 &&
        pBall->GetShotTeamIdx() != GetTeamNo()) {
        nReact |= 0x800;
    }
    if (m_nComLevel == 1) {
        nReact &= 0xFFF80000;
    }
    return nReact;
}

long msw::CChrCoreVolley::SubStateReceiveChoiceProcType_(STATE_WORK_RECEIVE *pWork)
{
    if (!IsOffenseTeam()) {
        return 4;
    }
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 4;
    }
    if (pBall->IsVolleyArtsAttach()) {
        return 4;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 8) && !pBall->IsShootMode()) {
        return 4;
    }
    if (pBall->IsVolleyThroughUnderNet()) {
        return 4;
    }
    int bPass = 0;
    if (pBall != 0) {
        if (!IsVolleyRightBallTouch_(pBall)) {
            return 4;
        }
        // A pass from a team mate.
        if (pBall->IsPassMode() && pBall->GetTouchNum() >= 1 && pBall->GetShotTeamIdx() == GetTeamNo() &&
            pBall->GetShotChrIdx() != GetPlayerNo()) {
            bPass = 1;
        }
    }

    bcs::FVEC3 target = pBall->GetTargetPos();
    int bDive = 1;
    int bDiveDir = 1;
    if (pWork != 0) {
        pWork->m_fRotY = GetRotY_ToPosXZ(target);
        unsigned long nFlag = pWork->m_nFlag;
        if (nFlag & 0x1000) {
            bDiveDir = 0;
        }
        if (nFlag & 0x8000) {
            bDive = 0;
        }
    }
    if (GetCcParam_()->m_Unk100 != 0) {
        bDiveDir = 1;
    }
    if (GetCcParam_()->m_UnkFC != 0) {
        bDive = 0;
    }

    float fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
    if (bPass) {
        float fFrameMax = 60.0f;
        if (pBall->GetVolleyTouchNum(GetTeamNo()) >= 2) {
            fFrameMax += 20.0f;
        }
        if (fFrame > fFrameMax) {
            return 4;
        }
        if (pBall->GetTouchDownFrame(m_fTossHeight + GetPosY()) > 9.0f) {
            // High enough for a jump toss when it comes down near.
            float fHeight = 1.5f + GetCcParam_()->m_Unk44;
            bcs::FVEC3 pos = GetPos();
            bcs::FVEC3 low(pos.x, pos.y + m_JumpTossCatchPos.y, pos.z);
            pos.y += m_JumpTossCatchPos.y + fHeight;
            float fJumpFrame = pBall->GetTouchDownFrame2(low.y, (pos.y - low.y) / 29.0f);
            if (fJumpFrame >= 15.0f) {
                pBall->GetVolleyPosAfterFrame(pos, fJumpFrame, 0);
                const bcs::FVEC3 &rPos = GetPos();
                bcs::FVEC3 from(rPos.x, 0.0f, rPos.z);
                bcs::FVEC3 to(pos.x, 0.0f, pos.z);
                if (bcs::FVEC3DistSq(&from, &to) <= 2.25f) {
                    return 3;
                }
            }
        }
        if (bDiveDir && bDive) {
            bcs::FVEC3 pos = GetPos();
            bcs::FVEC3 diff;
            diff.x = pBall->GetTargetPos().x - pos.x;
            diff.z = pBall->GetTargetPos().z - pos.z;
            float fRange = GetCcParam_()->m_UnkD4 * pBall->GetVolleyCatchableRate();
            if (diff.x * diff.x + diff.z * diff.z >= fRange * fRange) {
                return 1;
            }
        }
        if (pBall->GetPos().y >= m_fTossHeight + GetPosY()) {
            return 2;
        }
    }

    if (fFrame > 90.0f) {
        return 4;
    }
    if (bDiveDir && bDive) {
        // Too far from where the ball comes down: dive for it.
        bcs::FVEC3 pos = GetPos();
        bcs::FVEC3 step;
        bcs::BCS_VecZ_RotY(step, 0.4f, GetRotY_ToPosXZ(pBall->GetPosD4()));
        pos.x += step.x;
        pos.z += step.z;
        bcs::FVEC3 ballPos;
        pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 1);
        step.x = ballPos.x - pos.x;
        step.z = ballPos.z - pos.z;
        float fRange = GetCcParam_()->m_UnkD4 * pBall->GetVolleyCatchableRate();
        if (step.x * step.x + step.z * step.z >= fRange * fRange) {
            if (pWork != 0) {
                pWork->m_fRotY = GetRotY_ToPosXZ(ballPos);
            }
            return 1;
        }
    }
    return 0;
}

int msw::CChrCoreVolley::SubStateReceiveDoProcReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            if (!GetFlag(1)) {
                SetBallCatchEnable_(4);
            }
            if ((pWork->m_nFlag & 4) && m_nComLevel != 1) {
                // A ball to endure.
                pWork->m_nStep = 2;
                SubStateReceiveStartEndure_(pWork);
            } else {
                pWork->m_nStep = 1;
                pWork->m_fUnk3C = 5.0f;
                if (GetFlag(1)) {
                    SubStateReceiveSetBallDragParam_(pWork->m_MoveParam, pWork->m_DirMove, pWork->m_Dir, 5.0f, 0.5f,
                                                     0.2f, 1.5f, 0);
                    pWork->m_fMoveFrame = 0.0f;
                }
            }
            pWork->m_fTrgFrame = -1.0f * pWork->m_fUnk38;
            AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 1), 0, 4.0f, 0.0f);
            if (pWork->m_nFlag & 0x200) {
                pWork->m_fMoveFrame2 += m_fFrameStep * m_fTimeScale;
                Util_DoDirMove(pWork->m_MoveParam2, pWork->m_fMoveFrame2, pWork->m_DirMove2, 0);
                RotateYParam_(pWork->m_RotLerp);
            }
            SubStateReceiveDoMoveBallDrag_(pWork);
            break;
        case 1: {
            if (!GetFlag(1) || (pWork->m_nFlag & 0x200)) {
                pWork->m_fMoveFrame2 += m_fFrameStep * m_fTimeScale;
                Util_DoDirMove(pWork->m_MoveParam2, pWork->m_fMoveFrame2, pWork->m_DirMove2, 0);
                RotateYParam_(pWork->m_RotLerp);
            }
            SubStateReceiveDoMoveBallDrag_(pWork);
            long nResult = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8C), 0, 0, 0, 0);
            if (nResult > 1) {
                nResult = 1;
            }
            if (nResult <= 0) {
                pWork->m_fTrgFrame += m_fTimeScale * m_fFrameStep;
                if (pWork->m_fTrgFrame >= pWork->m_fUnk3C) {
                    nResult = 1;
                }
            }
            if (nResult > 0) {
                DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x54), 0, 0, 0, 0);
                if (GetFlag(1)) {
                    pWork->m_nFlag |= 8;
                }
                if (nResult == 2) {
                    pWork->m_nFlag |= 2;
                }
                ClearMoveParamWork_(pWork->m_MoveParam);
                Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                pWork->m_fMoveFrame = 0.0f;
                AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 2), 0, 8.0f, 0.0f);
                pWork->m_nStep = 3;
            }
            break;
        }
        case 2:
            // Enduring: the gauge fills with the buttons mashed.
            if (pWork->m_nFlag & 0x200) {
                pWork->m_fMoveFrame2 += m_fFrameStep * m_fTimeScale;
                Util_DoDirMove(pWork->m_MoveParam2, pWork->m_fMoveFrame2, pWork->m_DirMove2, 0);
                RotateYParam_(pWork->m_RotLerp);
            }
            SubStateReceiveDoMoveBallDrag_(pWork);
            pWork->m_fEndure +=
                0.0009765625f * (float)DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x97), 0, 0, 0, 0);
            if (m_nComLevel == 1) {
                pWork->m_fEndure = pWork->m_fEndure4C;
            }
            pWork->m_fTrgFrame += m_fTimeScale * m_fFrameStep;
            if (pWork->m_fTrgFrame >= pWork->m_fUnk3C || pWork->m_fEndure >= pWork->m_fEndure4C) {
                SetEndure(0);
                Eff_StopGroup(3);
                DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x54), 0, 0, 0, 0);
                if (pWork->m_fEndure < pWork->m_fEndure48) {
                    // Knocked down by the ball, which drops.
                    float fRotY = GetRotY();
                    CBall *pBall = GetAttachBall();
                    float fPower = 0.0f;
                    if (pBall != 0) {
                        fPower = pBall->GetVolleyFlickDownPower(2);
                        fRotY = GetRotY_FromPosXZ(pBall->GetPos());
                        if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                            fRotY = WrapRad(nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                        }
                        DetachBall();
                        Util_DropBall_(pBall, GetRotY());
                    }
                    MethdDamage(0xF, fRotY, fPower, 0.0f, -1, 0.0f, 0, 0);
                    return 0;
                }
                if (pWork->m_fEndure < pWork->m_fEndure4C) {
                    // The ball gets away.
                    float fRotY = GetRotY();
                    CBall *pBall = GetAttachBall();
                    float fPower = 0.0f;
                    if (pBall != 0) {
                        fPower = pBall->GetVolleyFlickDownPower(2);
                        DetachBall();
                        Util_MissBall_(pBall, 4.0f, 8.0f);
                        fRotY = GetRotY_FromPosXZ(pBall->GetPos());
                        if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                            fRotY = WrapRad(nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                        }
                    }
                    MethdDamage(0xF, fRotY, fPower, 0.0f, -1, 0.0f, 0, 0);
                    return 0;
                }
                ClearMoveParamWork_(pWork->m_MoveParam);
                Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                pWork->m_fMoveFrame = 0.0f;
                if (GetFlag(1)) {
                    pWork->m_nFlag |= 8;
                }
                AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 2), 0, 8.0f, 0.0f);
                pWork->m_nStep = 3;
            }
            break;
        case 4:
            // Waiting in the pose for the ball.
            pWork->m_fWaitFrame -= m_fTimeScale * m_fFrameStep;
            if (GetFlag(1) || pWork->m_fWaitFrame <= 0.0f) {
                Util_NodePatchWarpToTransNodeXZ();
                AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 2), 0, 4.0f, 0.0f);
                pWork->m_nStep = 3;
            }
            break;
        case 3:
            if (GetFlag(1)) {
                if ((pWork->m_nFlag & 4) && !(pWork->m_nFlag & 8)) {
                    ClearMoveParamWork_(pWork->m_MoveParam);
                    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                    pWork->m_fMoveFrame = 0.0f;
                    AnimPlay(3, GetReceivePoseMotID_(pWork->m_nPose, 1), 0, 8.0f, 0.0f);
                    pWork->m_fTrgFrame = 0.0f;
                    SubStateReceiveStartEndure_(pWork);
                    pWork->m_nStep = 2;
                    break;
                }
                float fShotFrame = GetBallShotTrgFrame_(AnimGetMotID(3, 0), 6.0f);
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fShotFrame) {
                    // Send the ball on.
                    int bFlick = 0;
                    long nCatchType = 0;
                    long nType = (pWork->m_nFlag >> 1) & 1;
                    float fRotY = GetRotY();
                    float fPower = 0.0f;
                    long nCatch = 0;
                    long nStagger = 0;
                    float fStagger = 0.0f;
                    int bGauge = 1;
                    int bSizeChange = 0;
                    int bDropCoin = 0;
                    int bMessage = 0;
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        bMessage = 1;
                        if (pBall->GetVolleyTouchNum(m_nTeamNo) > 0) {
                            bGauge = 0;
                            bMessage = 0;
                        }
                        if (pBall->IsItemBall() && m_nComLevel != 1) {
                            bGauge = 0;
                        }
                        int bStrong = 0;
                        if (pWork->m_fUnk40 >= 0.5f) {
                            bStrong = 1;
                        }
                        unsigned long nReact = pBall->GetVolleyReceiveReaction(bStrong, GetCcParam_()->m_Unk04);
                        fPower = pBall->GetVolleyFlickDownPower(bStrong);
                        fStagger = pBall->GetVolleyReceiveStaggerFrame(GetCcParam_()->m_Unk04);
                        nReact = SubStateReceiveDoCheckReactFlag_(pWork, pBall, nReact);
                        if (nReact & 0x600) {
                            fRotY = GetRotY_ToPosXZ(pBall->GetPos());
                            nStagger = 1;
                            if (nReact & 0x400) {
                                nStagger = 2;
                            }
                            bGauge = 0;
                            bMessage = 0;
                        }
                        if (nReact & 0x100) {
                            nCatchType = 2;
                            nCatch = (unsigned char)nReact;
                        }
                        if (nReact & (0x10000 | 0x1000)) {
                            nType = 2;
                            if (nReact & 0x10000) {
                                nType = 4;
                            }
                            bMessage = 0;
                        }
                        if (nReact & 0x8800) {
                            fRotY = GetFlickDir_(pBall);
                            bFlick = 1;
                            bGauge = 0;
                            bMessage = 0;
                            if ((nReact & 0x8000) && !IsInvincible()) {
                                nType = 3;
                            }
                        }
                        if (nReact & 0x100000) {
                            CGimmickMan::EffectCoinGetCreate(pBall->GetPos(), static_cast<DROP_COIN::CoinType>(0));
                            CGame::AddTeamCoin(m_nTeamNo, 1);
                        }
                        if (nReact & 0x20000) {
                            // (a coin drop, unused in volleyball)
                            bDropCoin = 1;
                        }
                        if (nReact & 0x40000) {
                            pWork->m_fUnk40 = 0.0f;
                        }
                        if (nReact & 0x80000) {
                            pWork->m_fUnk40 = 1.0f;
                        }
                        if (nReact & 0x4000) {
                            bSizeChange = 1;
                        }
                        if (bGauge) {
                            // A good receive fills the arts gauge by a
                            // fifth, a chance ball too.
                            float fShots = pBall->IsVolleyChanceBallMode() ? 5.0f : 5.0f;
                            if (fShots > 0.0f) {
                                AddArtsGauge_(1.0f / fShots, -1.0f);
                            }
                        }
                        pBall->TrgVolleyReceiveReaction(0, 1);
                    }
                    SetEndure(0);
                    if (bMessage > 0) {
                        if (pWork->m_fUnk40 >= 1.0f) {
                            CGame::SendMessage(0x4B, m_nPlayerNo, 0, 0);
                        } else {
                            CGame::SendMessage(0x4A, m_nPlayerNo, 0, 0);
                        }
                    }
                    ReceiveBall_(nCatchType, nCatch, pWork->m_fUnk40, nType,
                                 bFlick ? pWork->m_nFlickType : pWork->m_nReceiveType, (pWork->m_nFlag >> 8) & 1);
                    if (bSizeChange) {
                        SetHookDamage(0x240D, GetRotY(), 0.0f, 600.0f, -1, 0.0f, 0, 0, -1, -1);
                    } else if (bFlick) {
                        if (GetFlag(0x10000) && fPower < 20.0f) {
                            fPower = 20.0f;
                        }
                        SetHookDamage(0xF, fRotY, 0.0f, fPower, -1, 0.0f, 0, 0, -1, -1);
                    } else if (nStagger > 0) {
                        SetRotY(fRotY);
                        SubStateReceiveStartStagger_(pWork, nStagger, fStagger);
                    }
                    pWork->m_nStep = 5;
                    pWork->m_nFlag |= 0x4000;
                }
            } else {
                RotateYRate_(GetReceiveBaseDir_(), 0.2f);
            }
            // fall through
        case 5:
            if (!(pWork->m_nFlag & 0x800)) {
                pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            }
            Util_DoDirMove(pWork->m_MoveParam, AnimGetFrame(3), pWork->m_DirMove, 0);
            // fall through
        default:
            if (IsAnimEnd(3) && !GetFlag(1)) {
                bEnd = 1;
            }
            break;
    }

    if (!bEnd) {
        // Gravity while in the air.
        bcs::FVEC3 pos = GetPos();
        float fStep = m_fFrameStep * m_fTimeScale;
        bcs::FVEC3 move;
        bcs::FVEC3Scale(&move, &GetJumpVel(), fStep);
        float fVelAdd = -0.0125f * fStep;
        move.y += 0.5f * (fVelAdd * fStep);
        m_fJumpVelY += fVelAdd;
        bcs::FVEC3Add(&pos, &pos, &move);
        if (pos.y < 0.0f) {
            pos.y = 0.0f;
        }
        SetPosY(pos.y);
    }
    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

void msw::CChrCoreVolley::SubStateSetReceivePose_(long nPose)
{
    SetBallCatchEnable_(4);
    AnimPlay(3, GetReceivePoseMotID_(nPose, 0), 0, 8.0f, 0.0f);
}

void msw::CChrCoreVolley::SubStateReceiveStartStagger_(STATE_WORK_RECEIVE *pWork, long nType, float fFrame)
{
    // Staggers back (further for kind 2).
    float fDist = 0.6f;
    long nMot = 0x1C;
    if (nType == 2) {
        nMot = 0x1D;
        fDist = 1.2f;
    }
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
    float fMotFrame = AnimGetResFrameNum(nMot);
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_nFlag |= 0x800;
    if (fDist > 0.0f) {
        // Back off away from the facing.
        pWork->m_MoveParam.m_nType = 11;
        pWork->m_MoveParam.m_fDist = fDist;
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = fMotFrame;
        Util_SetDirMoveParam(pWork->m_DirMove, WrapRad(3.1415927f + GetRotY()));
    }
    if (fFrame > 0.0f && fMotFrame > 0.0f) {
        AnimChgSpeed(3, fMotFrame / fFrame, 0.0f);
    }
    pWork->m_afCountWork[0] = 0.0f;
    pWork->m_afCountWork[1] = 45.0f;
}

float msw::CChrCoreVolley::SubStateReceiveGetHomingTargetPosReceive_(STATE_WORK_RECEIVE *pWork, CBall *pBall,
                                                                    const bcs::FVEC3 &rLow,
                                                                    const bcs::FVEC3 &rHigh, float fHeight,
                                                                    float fFrameMin, float fFrameMax, float fDistMax)
{
    float fWait = 0.0f;
    pWork->m_TargetPos = GetPos();
    pWork->m_TargetPos.y = fHeight;
    if (pBall == 0) {
        return fWait;
    }
    if (pBall->GetShotChrIdx() == GetPlayerNo()) {
        return fWait;
    }

    // The ball is taken between the low point (early) and the high one
    // (late in the motion).
    const bcs::FVEC3 &pos = GetPos();
    bcs::FVEC3 low;
    low.Set(pos.x, pos.y + rLow.y, pos.z);
    bcs::FVEC3 high;
    high.Set(pos.x, pos.y + (rHigh.y + fHeight), pos.z);
    bcs::FVEC3 ofs = rLow;
    float fSlope = (high.y - low.y) / (1.0f + (fFrameMax - fFrameMin));
    float fFrame = pBall->GetTouchDownFrame2(low.y - fSlope * fFrameMin, fSlope);
    bcs::FVEC3 ballPos;
    if (fFrame > fFrameMax) {
        if (CGame::g_GameWork.GetGameType() == 2) {
            // The party game: the ball is taken at the high point.
            float fHigh = pBall->GetTouchDownFrame2(high.y, 0.0f);
            fWait += (float)ceil(fHigh - fFrameMin);
            pBall->GetVolleyPosAfterFrame(ballPos, fHigh + fFrameMin, 0);
            ofs = rHigh;
        } else {
            fWait += (float)ceil(fFrame - fFrameMax);
            pBall->GetVolleyPosAfterFrame(ballPos, fWait + fFrameMax, 0);
            ofs = rHigh;
        }
    } else if (fFrame < fFrameMin + 3.0f) {
        fWait = -1.0f * fFrameMin;
        float fLow = pBall->GetTouchDownFrame2(low.y, fSlope);
        float fDown = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
        fFrame = (float)ceil(fLow > fDown ? fDown : fLow);
        pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 0);
        float fRate = (fFrame - fFrameMin) / (1.0f + (fFrameMax - fFrameMin));
        bcs::BCS_Vec_Lerp(ofs, rLow, rHigh, CLAMP(fRate, 0.0f, 1.0f));
    } else {
        fFrame = (float)ceil(fFrame);
        pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 0);
        float fRate = (fFrame - fFrameMin) / (1.0f + (fFrameMax - fFrameMin));
        bcs::BCS_Vec_Lerp(ofs, rLow, rHigh, CLAMP(fRate, 0.0f, 1.0f));
    }
    float fY = ballPos.y - rHigh.y;
    fHeight = CLAMP(fY, fHeight, fHeight);
    pWork->m_TargetPos = ballPos;

    // Stand so that the pose takes the ball, facing where it comes from.
    bcs::FVEC3 vec = pBall->GetVelocity();
    float fRotY = GetReceiveBaseDir_();
    if (vec.x != 0.0f || vec.z != 0.0f) {
        fRotY = WrapRad(3.1415927f + nw4r::math::Atan2Rad(vec.x, vec.z));
    }
    bcs::BCS_Vec_RotY(vec, ofs, fRotY);
    bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &vec);

    const bcs::FVEC3 &rPos = GetPos();
    vec.x = pWork->m_TargetPos.x - rPos.x;
    vec.z = pWork->m_TargetPos.z - rPos.z;
    bcs::FVEC3 flat(vec.x, 0.0f, vec.z);
    float fDistSq = bcs::FVEC3SquareMag(&flat);
    float fDist = fDistSq > fDistMax * fDistMax ? fDistMax : nw4r::math::FSqrt(fDistSq);
    pWork->m_MoveParam.m_nType = 11;
    pWork->m_MoveParam.m_fDist = fDist;
    pWork->m_MoveParam.m_fFrame = fFrameMin;
    pWork->m_MoveParam.m_fFrameMax = fFrameMax;
    pWork->m_TargetPos.y = fHeight;
    return fWait;
}

void msw::CChrCoreVolley::SubStateReceiveStartEndure_(STATE_WORK_RECEIVE *pWork)
{
    // Enduring a special shot: the ball drags the character back while the
    // buttons are mashed; the gauge effects and the smoke face the way the
    // ball pushes.
    pWork->m_fUnk3C = pWork->m_fEndure50;
    if (pWork->m_fEndure50 > 0.0f) {
        DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 0, 0, 0, 0);
    }
    float fRotY = GetTargetCourtDir_();
    CBall *pBall = GetAttachBall();
    if (pBall != 0) {
        BALL::VOLLEY::ENDURE_PRM prm;
        pBall->GetVolleyEndureParam(prm);
        SubStateReceiveSetBallDragParam_(pWork->m_MoveParam, pWork->m_DirMove, pWork->m_Dir, pWork->m_fEndure50,
                                         prm.m_afUnk0C[0], prm.m_afUnk0C[1], prm.m_afUnk0C[2], 1);
        pWork->m_fMoveFrame = 0.0f;
        pBall->TrgVolleyReceiveReaction(1, PlayGetOutline_(1));
        bcs::FVEC3 dir = pWork->m_Dir;
        if (dir.x * dir.x + dir.z * dir.z > 0.0f) {
            fRotY = WrapRad(3.1415927f + nw4r::math::Atan2Rad(dir.x, dir.z));
        }
        if (prm.m_anUnk18[2] >= 0) {
            CGame::StartCameraQuake(prm.m_anUnk18[2], m_nPlayerNo, -1);
        }
    }
    SetEndure(1);
    Eff_CreateAttach(11, 0x649, m_nEffNodeEndure0, 0, 0xC, 3, 0);
    Eff_CreateAttach(12, 0x64A, m_nEffNodeEndure1, 0, 0xC, 3, 0);
    new (0x100) CC_CMN::CcEndureSmokeEffect(CGame::GetTempParent(), this, 3, fRotY, 0x648, 0.04f);
    Eff_SetRotateY(11, fRotY);
    Eff_SetRotateY(12, fRotY);
}

// (inline helper) The dive's turn when it ends, by fDeg degrees (the
// character's parameters; mirrored with the dive).
static inline void DiveEndTurn(msw::CChrCoreVolley *pChr, msw::CChrCoreVolley::STATE_WORK_RECEIVE *pWork, float fDeg)
{
    if (fDeg > 0.0f) {
        if (pWork->m_nFlag & 0x40) {
            fDeg = -fDeg;
        }
        float fRotY = WrapRad(DegToRad(fDeg));
        fRotY = WrapRad(fRotY + pChr->GetRotY());
        pChr->Util_NodePatchRotY(fRotY);
        pChr->SetRotY(fRotY);
    }
}

int msw::CChrCoreVolley::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0: {
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
            SE_PlayWrapAct(0xD, 0, -1);
            PlayVoice(0x3D, 0);
            SetMoveMotType(2);
            SetBallCatchEnable_(5);
            if (pWork->m_fTargetRotY >= 0.0f) {
                float fLimit = 160.0f;
                float fDiff = WrapRadPI(pWork->m_fTargetRotY - pWork->m_fRotY);
                if (fabsf(fDiff) > DegToRad(fLimit)) {
                    pWork->m_fRotY = pWork->m_fTargetRotY;
                }
            }
            // Dive to the side the ball comes from.
            long nMot = 0x1E;
            bcs::FVEC3 dir;
            bcs::BCS_VecOneZ_RotY(dir, pWork->m_fRotY);
            CBall *pBall = GetTargetBall_();
            if (pBall != 0) {
                bcs::FVEC3 vel = pBall->GetVelocity();
                PSVECCrossProduct((const Vec *)&vel, (const Vec *)&dir, (Vec *)&dir);
                if (dir.y < 0.0f) {
                    nMot = -0x1E;
                    pWork->m_nFlag |= 0x40;
                }
            }
            AnimPlay(3, nMot, 0, 4.0f, 0.0f);
            SubStateReceiveDoDiveReceiveDiveStart_(pWork);
            pWork->m_fTrgFrame = 16.0f;
            pBall = GetTargetBall_();
            if (pBall != 0) {
                float fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
                if (fFrame > 0.0f) {
                    bcs::FVEC3 ballPos;
                    pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 1);
                    float fDist = GetDist_ToPosXZ(ballPos);
                    float fSpeed = fDist / fFrame;
                    float fSpeedMax = 0.016666668f * GetCcParam_()->m_UnkE0;
                    fSpeed = CLAMP(fSpeed, 0.0f, fSpeedMax);
                    if (fSpeed > 0.0f && fDist / fSpeed < fFrame) {
                        fDist = fSpeed * fFrame;
                    }
                    float fDistMax = GetCcParam_()->m_UnkDC;
                    fDist = CLAMP(fDist, 0.0f, fDistMax);
                    float fMoveFrame = 1.0f;
                    if (fSpeed > 0.0f) {
                        fMoveFrame = fDist / fSpeed;
                    }
                    pWork->m_MoveParam.m_fDist = fDist;
                    pWork->m_MoveParam.m_fFrameMax = (float)ceil(fMoveFrame);
                    pWork->m_fDiveSpeed = fSpeed;
                } else {
                    pWork->m_MoveParam.m_fFrameMax = AnimGetResFrameNum(0x1E);
                    pWork->m_MoveParam.m_fDist = 1.0f;
                    pWork->m_fDiveSpeed = 1.0f / (1.0f + AnimGetResFrameNum(0x1E));
                }
            }
            pWork->m_MoveParam.m_fFrame = 0.0f;
            pWork->m_MoveParam.m_nType = 0;
            pWork->m_fMoveFrame = 0.0f;
            Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_nStep = 1;
            break;
        }
        case 1:
            // Flying.
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
            if (pWork->m_fMoveFrame >= pWork->m_MoveParam.m_fFrameMax || (pWork->m_nFlag & 1)) {
                if (pWork->m_nFlag & 1) {
                    SetBallCatchDisable_();
                    long nMot = 0x1F;
                    if (pWork->m_nFlag & 0x40) {
                        nMot = -0x1F;
                    }
                    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                    PlaySePos(0xC90, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
                    pWork->m_MoveParam.m_fFrameMax = AnimGetResFrameNum(nMot);
                    float fDist = GetCcParam_()->m_UnkE8;
                    pWork->m_MoveParam.m_nType = 0;
                    pWork->m_nStep = 3;
                    pWork->m_MoveParam.m_fDist = 0.65f * fDist;
                } else {
                    long nMot = 0x22;
                    if (pWork->m_nFlag & 0x40) {
                        nMot = -0x22;
                    }
                    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                    PlaySePos(0xC91, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
                    float fFrame = GetCcParam_()->m_UnkEC;
                    pWork->m_MoveParam.m_fFrameMax = fFrame;
                    pWork->m_MoveParam.m_fDist = 0.5f * pWork->m_fDiveSpeed * fFrame;
                    float fRate = GetCcParam_()->m_UnkF0;
                    pWork->m_MoveParam.m_nType = 11;
                    pWork->m_nStep = 2;
                    pWork->m_MoveParam.m_fDist *= fRate;
                }
                pWork->m_MoveParam.m_fFrame = 0.0f;
                pWork->m_fMoveFrame = m_fFrameStep * m_fTimeScale;
                Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
            }
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
            break;
        case 2:
            // Sliding on without the ball.
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (pWork->m_nFlag & 1) {
                SetBallCatchDisable_();
                long nMot = 0x1F;
                if (pWork->m_nFlag & 0x40) {
                    nMot = -0x1F;
                }
                AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                PlaySePos(0xC90, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_MoveParam.m_fFrameMax = AnimGetResFrameNum(nMot);
                float fDist = 0.65f * GetCcParam_()->m_UnkE8;
                pWork->m_MoveParam.m_nType = 0;
                pWork->m_MoveParam.m_fDist = fDist;
                pWork->m_nStep = 3;
                pWork->m_MoveParam.m_fFrame = 0.0f;
                pWork->m_fMoveFrame = m_fFrameStep * m_fTimeScale;
                Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
                Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
            } else {
                pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
                if (pWork->m_fMoveFrame >= pWork->m_MoveParam.m_fFrameMax) {
                    SetBallCatchDisable_();
                    long nMot = 0x24;
                    if (pWork->m_nFlag & 0x40) {
                        nMot = -0x24;
                    }
                    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
                    pWork->m_nStep = 4;
                } else if (IsAnimEnd(3) && AnimGetMotID(3, 0) == 0x22) {
                    long nMot = 0x23;
                    if (pWork->m_nFlag & 0x40) {
                        nMot = -0x23;
                    }
                    AnimPlay(3, nMot, 1, 0.0f, 0.0f);
                }
                Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 1);
            }
            break;
        case 3:
            // Caught: sliding on with the ball.
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
            if (pWork->m_fMoveFrame >= pWork->m_MoveParam.m_fFrameMax) {
                SetBallCatchDisable_();
                pWork->m_MoveParam.m_fFrame = 0.0f;
                float fWait = GetCcParam_()->m_UnkE4;
                pWork->m_MoveParam.m_fFrameMax = AnimGetResFrameNum(0x21) + fWait;
                float fDist = GetCcParam_()->m_UnkE8;
                pWork->m_MoveParam.m_nType = 11;
                pWork->m_MoveParam.m_fDist = 0.35f * fDist;
                Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
                long nMot = 0x20;
                pWork->m_fMoveFrame = m_fFrameStep * m_fTimeScale;
                if (pWork->m_nFlag & 0x40) {
                    nMot = -0x20;
                }
                AnimPlay(3, nMot, 1, 0.0f, 0.0f);
                Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(0), 0);
                pWork->m_nStep = 4;
            }
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 1);
            break;
        case 4:
            // Getting up.
            if (pWork->m_nFlag & 1) {
                pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
                Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 1);
                long nMot = 0x21;
                if (pWork->m_fMoveFrame >= pWork->m_MoveParam.m_fFrameMax - AnimGetResFrameNum(0x21) &&
                    AnimGetMotID(3, 0) != 0x21) {
                    if (pWork->m_nFlag & 0x40) {
                        nMot = -0x21;
                    }
                    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                }
                if (pWork->m_fMoveFrame < pWork->m_MoveParam.m_fFrameMax) {
                    break;
                }
                DiveEndTurn(this, pWork, GetCcParam_()->m_UnkF4);
            } else {
                if (!IsAnimEnd(3)) {
                    break;
                }
                float fDeg = GetCcParam_()->m_UnkF8;
                if (pWork->m_nFlag & 1) {
                    fDeg = GetCcParam_()->m_UnkF4;
                }
                DiveEndTurn(this, pWork, fDeg);
            }
            // fall through
        default:
            bEnd = 1;
            break;
    }

    if (m_nStatusFlag & 1) {
        long nType = 0;
        if (pWork->m_nFlag & 2) {
            nType = 1;
        }
        int bSizeChange = 0;
        CBall *pBall = GetAttachBall();
        if (pBall != 0) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            float fPower = pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            if (nReact & (0x10000 | 0x1000)) {
                nType = 2;
                if (nReact & 0x10000) {
                    nType = 4;
                }
            }
            int bFlick = 0;
            GetRotY();
            if (nReact & 0x8800) {
                GetFlickDir_(pBall);
                bFlick = 1;
                if ((nReact & 0x8000) && !IsStarConditionReceive_()) {
                    nType = 3;
                }
            }
            if (nReact & 0x100000) {
                CGimmickMan::EffectCoinGetCreate(pBall->GetPos(), static_cast<DROP_COIN::CoinType>(0));
                CGame::AddTeamCoin(m_nTeamNo, 1);
            }
            if (nReact & 0x4000) {
                bSizeChange = 1;
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, bFlick ? pWork->m_nFlickType : pWork->m_nReceiveType,
                         (pWork->m_nFlag >> 8) & 1);
            if (bSizeChange) {
                SetHookDamage(0x240D, GetRotY(), 0.0f, 600.0f, -1, 0.0f, 0, 0, -1, -1);
            } else if (bFlick) {
                float fRotY = GetRotY_FromPosXZ(pBall->GetPos());
                if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                    fRotY = WrapRad(nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                }
                if ((m_nStatusFlag & 0x10000) && fPower < 20.0f) {
                    fPower = 20.0f;
                }
                MethdDamage(0x1000F, fRotY, fPower, 0.0f, -1, 0.0f, 0, 0);
                return 0;
            }
        }
    }
    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

void msw::CChrCoreVolley::SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *)
{
}

// (inline helper of the toss) Turns the hand (virtual node 4) towards rPos.
static inline void TurnHandTo(msw::CChrCoreVolley *pChr, const bcs::FVEC3 &rPos)
{
    bcs::FVEC3 hand;
    pChr->GetVNodePos(4, hand);
    float fDiff = pChr->GetRotY_Diff_ToPosXZ(hand);
    pChr->RotateYRate_(WrapRad(pChr->GetRotY_ToPosXZ(rPos) - fDiff), 0.2f);
}

int msw::CChrCoreVolley::SubStateReceiveDoProcToss_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            SetBallCatchDisable_();
            AnimPlay(3, GetTossPoseMotID_(pWork->m_nPose, 1), 0, 8.0f, 0.0f);
            pWork->m_nStep = 1;
            pWork->m_fUnk3C = 5.0f;
            pWork->m_fTrgFrame = -1.0f * pWork->m_fUnk38;
            SubStateReceiveSetBallDragParam_(pWork->m_MoveParam, pWork->m_DirMove, pWork->m_Dir, 5.0f, 0.5f, 0.2f,
                                             1.5f, 0);
            pWork->m_fMoveFrame = 0.0f;
            break;
        case 1: {
            if (!(m_nStatusFlag & 1)) {
                pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
                Util_DoDirMove(pWork->m_MoveParam, AnimGetFrame(3), pWork->m_DirMove, 0);
            }
            SubStateReceiveDoMoveBallDrag_(pWork);
            long nResult = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8C), 0, 0, 0, 0);
            if (nResult > 1) {
                nResult = 1;
            }
            if (nResult <= 0) {
                pWork->m_fTrgFrame += m_fTimeScale * m_fFrameStep;
                if (pWork->m_fTrgFrame >= pWork->m_fUnk3C) {
                    nResult = 1;
                }
            }
            if (nResult > 0) {
                DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x54), 0, 0, 0, 0);
                if (nResult == 2) {
                    pWork->m_nFlag |= 2;
                }
                AnimPlay(3, GetTossPoseMotID_(pWork->m_nPose, 2), 0, 8.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        }
        case 3: {
            pWork->m_fWaitFrame -= m_fTimeScale * m_fFrameStep;
            CBall *pBall = GetTargetBall_();
            if (pBall != 0) {
                TurnHandTo(this, pBall->GetPos());
            }
            if ((m_nStatusFlag & 1) || pWork->m_fWaitFrame <= 0.0f) {
                AnimPlay(3, GetTossPoseMotID_(pWork->m_nPose, 2), 0, 4.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        }
        case 2:
            if (m_nStatusFlag & 1) {
                SetBallCatchDisable_();
                float fShotFrame = GetBallShotTrgFrame_(AnimGetMotID(3, 0), 9.0f);
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fShotFrame) {
                    // Toss the ball up.
                    unsigned long nReact = 0;
                    long nCatchType = 4;
                    long nType = (pWork->m_nFlag >> 1) & 1;
                    float fRotY = GetRotY();
                    float fPower = 0.0f;
                    long nCatch = 0;
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        nReact = pBall->GetVolleyReceiveReaction(0, GetCcParam_()->m_Unk04);
                        fPower = pBall->GetVolleyFlickDownPower(0);
                        if (nReact & 0x100) {
                            nCatchType = 6;
                            nCatch = (unsigned char)nReact;
                        }
                        if (nReact & 0x800) {
                            fRotY = GetFlickDir_(pBall);
                        }
                        if (nReact & (0x10000 | 0x1000)) {
                            nType = 2;
                            if (nReact & 0x10000) {
                                nType = 4;
                            }
                        }
                        if (nReact & 0x100000) {
                            CGimmickMan::EffectCoinGetCreate(pBall->GetPos(), static_cast<DROP_COIN::CoinType>(0));
                            CGame::AddTeamCoin(m_nTeamNo, 1);
                        }
                        pBall->TrgVolleyReceiveReaction(0, 1);
                    }
                    ReceiveBall_(nCatchType, nCatch, 0.0f, nType, -1, (pWork->m_nFlag >> 8) & 1);
                    if (nReact & 0x800) {
                        SetHookDamage(0xF, fRotY, 0.0f, fPower, -1, 0.0f, 0, 0, -1, -1);
                    }
                    pWork->m_nStep = 4;
                } else {
                    SubStateTossRotateOnBall_(pWork);
                }
            } else {
                CBall *pBall = GetTargetBall_();
                if (pBall != 0) {
                    TurnHandTo(this, pBall->GetPos());
                }
                float fShotFrame = 5.0f + GetBallShotTrgFrame_(AnimGetMotID(3, 0), 9.0f);
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fShotFrame) {
                    SetBallCatchDisable_();
                    if (!(pWork->m_nFlag & 1)) {
                        pWork->m_nFlag |= 0x10000;
                    }
                }
            }
            // fall through
        case 4:
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, AnimGetFrame(3), pWork->m_DirMove, 0);
            // fall through
        default:
            if (IsAnimEnd(3)) {
                bEnd = 1;
            }
            break;
    }
    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

float msw::CChrCoreVolley::SubStateReceiveGetHomingTargetPosToss_(STATE_WORK_RECEIVE *pWork, CBall *pBall,
                                                                 const bcs::FVEC3 &rLow, const bcs::FVEC3 &rHigh,
                                                                 float fHeight, float fFrameMin, float fFrameMax,
                                                                 float fDistMax)
{
    float fWait = 0.0f;
    pWork->m_TargetPos = GetPos();
    pWork->m_TargetPos.y = fHeight;
    if (pBall == 0) {
        return fWait;
    }
    if (pBall->GetShotChrIdx() == GetPlayerNo()) {
        return fWait;
    }

    const bcs::FVEC3 &pos = GetPos();
    bcs::FVEC3 low;
    low.Set(pos.x, pos.y + rLow.y, pos.z);
    bcs::FVEC3 high;
    high.Set(pos.x, pos.y + (rHigh.y + fHeight), pos.z);
    bcs::FVEC3 ofs = rLow;
    float fSlope = (high.y - low.y) / (1.0f + (fFrameMax - fFrameMin));
    float fFrame = pBall->GetTouchDownFrame2(low.y - fSlope * fFrameMin, fSlope);
    bcs::FVEC3 ballPos;
    if (fFrame > fFrameMax) {
        fWait += (float)ceil(fFrame - fFrameMax);
        pBall->GetVolleyPosAfterFrame(ballPos, fWait + fFrameMax, 0);
        ofs = rHigh;
    } else if (fFrame < fFrameMin + 3.0f) {
        fWait = -1.0f * fFrameMin;
        float fLow = pBall->GetTouchDownFrame2(low.y, fSlope);
        float fDown = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
        fFrame = (float)ceil(fLow > fDown ? fDown : fLow);
        pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 0);
        float fRate = (fFrame - fFrameMin) / (1.0f + (fFrameMax - fFrameMin));
        bcs::BCS_Vec_Lerp(ofs, rLow, rHigh, CLAMP(fRate, 0.0f, 1.0f));
    } else {
        fFrame = (float)ceil(fFrame);
        pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 0);
        float fRate = (fFrame - fFrameMin) / (1.0f + (fFrameMax - fFrameMin));
        bcs::BCS_Vec_Lerp(ofs, rLow, rHigh, CLAMP(fRate, 0.0f, 1.0f));
    }
    float fY = ballPos.y - rHigh.y;
    fHeight = CLAMP(fY, fHeight, fHeight);
    pWork->m_TargetPos = ballPos;

    // The toss faces the ball.
    bcs::FVEC3 vec;
    bcs::BCS_Vec_RotY(vec, ofs, GetRotY_ToPosXZ(ballPos));
    bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &vec);

    const bcs::FVEC3 &rPos = GetPos();
    vec.x = pWork->m_TargetPos.x - rPos.x;
    vec.z = pWork->m_TargetPos.z - rPos.z;
    bcs::FVEC3 flat(vec.x, 0.0f, vec.z);
    float fDistSq = bcs::FVEC3SquareMag(&flat);
    float fDist = fDistSq > fDistMax * fDistMax ? fDistMax : nw4r::math::FSqrt(fDistSq);
    pWork->m_MoveParam.m_nType = 11;
    pWork->m_MoveParam.m_fDist = fDist;
    pWork->m_MoveParam.m_fFrame = fFrameMin;
    pWork->m_MoveParam.m_fFrameMax = fFrameMax;
    pWork->m_TargetPos.y = fHeight;
    return fWait;
}

int msw::CChrCoreVolley::SubStateReceiveDoProcJumpToss_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            if (!(m_nStatusFlag & 1)) {
                SubStateReceiveGetHomingTargetPosJumpToss_(pWork, GetTargetBall_());
            }
            Util_SetJumpParam(28.0f, 10.0f, pWork->m_TargetPos.y);
            m_fJumpGravity = -0.0125f;
            AnimPlay(3, GetJumpTossPoseMotID_(pWork->m_nPose, 0), 0, 4.0f, 0.0f);
            PlayVoice(0x36, 0);
            PlaySePos(-1, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
            m_nStatusFlag |= 0x400;
            SetMoveMotStep_(1);
            pWork->m_fMoveFrame = 0.0f;
            pWork->m_nStep = 1;
            pWork->m_fShotFrame = GetBallShotTrgFrame_(GetJumpTossPoseMotID_(pWork->m_nPose, 1), 6.0f);
            break;
        case 1:
            if (m_nStatusFlag & 1) {
                SetBallCatchDisable_();
                AnimPlay(3, GetJumpTossPoseMotID_(pWork->m_nPose, 1), 0, 8.0f, 0.0f);
                m_fJumpUnk224 = 0.0f;
                pWork->m_nStep = 2;
            }
            // fall through
        case 2:
            if (m_nStatusFlag & 1) {
                if (AnimGetFrame(3) > pWork->m_fShotFrame) {
                    // Toss the ball up.
                    unsigned long nReact = 0;
                    long nCatchType = 4;
                    long nType = (pWork->m_nFlag >> 1) & 1;
                    float fRotY = GetRotY();
                    float fPower = 0.0f;
                    long nCatch = 0;
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        nReact = pBall->GetVolleyReceiveReaction(0, GetCcParam_()->m_Unk04);
                        fPower = pBall->GetVolleyFlickDownPower(0);
                        if (nReact & 0x100) {
                            nCatchType = 6;
                            nCatch = (unsigned char)nReact;
                        }
                        if (nReact & 0x800) {
                            fRotY = GetFlickDir_(pBall);
                        }
                        if (nReact & (0x10000 | 0x1000)) {
                            nType = 2;
                            if (nReact & 0x10000) {
                                nType = 4;
                            }
                        }
                        if (nReact & 0x100000) {
                            CGimmickMan::EffectCoinGetCreate(pBall->GetPos(), static_cast<DROP_COIN::CoinType>(0));
                            CGame::AddTeamCoin(m_nTeamNo, 1);
                        }
                        pBall->TrgVolleyReceiveReaction(0, 1);
                    }
                    ReceiveBall_(nCatchType, nCatch, 0.0f, nType, -1, (pWork->m_nFlag >> 8) & 1);
                    if (nReact & 0x800) {
                        SetHookDamage(0xF, fRotY, 0.0f, fPower, -1, 0.0f, 0, 0, -1, -1);
                    }
                    pWork->m_nStep = 3;
                } else {
                    SubStateTossRotateOnBall_(pWork);
                }
            } else {
                CBall *pBall = GetTargetBall_();
                if (pBall != 0) {
                    TurnHandTo(this, pBall->GetPos());
                }
                if (pWork->m_nStep > 1 && AnimGetFrame(3) > pWork->m_fShotFrame) {
                    SetBallCatchDisable_();
                }
            }
            // fall through
        case 3: {
            int nJump = Util_DoJumpMoveEX(0);
            if (nJump == 1) {
                if (pWork->m_RotLerp.m_afUnk[0] >= 0.0f) {
                    RotateYParam_(pWork->m_RotLerp);
                }
                if (pWork->m_fMoveFrame < pWork->m_MoveParam.m_fFrameMax) {
                    pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
                    pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
                    Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
                }
            }
            if (nJump >= 2 && !(m_nStatusFlag & 1)) {
                // Landed.
                SetBallCatchDisable_();
                m_UnkEC |= 0x10;
                SetCollBodyPushRank_(1);
                AnimPlay(3, GetJumpTossPoseMotID_(pWork->m_nPose, 2), 0, 4.0f, 0.0f);
                PlayLandSe(0);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                pWork->m_nStep = 4;
            } else {
                bcs::FVEC3 move = pWork->m_Dir;
                float fStep = m_fFrameStep * m_fTimeScale;
                move.x *= fStep;
                move.z *= fStep;
                AddPosXZ(move);
                pWork->m_Dir.x *= 0.93f;
                pWork->m_Dir.z *= 0.93f;
            }
            break;
        }
        default:
            if (IsAnimEnd(3)) {
                bEnd = 1;
            }
            break;
    }
    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

void msw::CChrCoreVolley::SubStateReceiveGetHomingTargetPosJumpToss_(STATE_WORK_RECEIVE *pWork, CBall *pBall)
{
    // The jump toss takes the ball at the hand, above the character.
    float fHeight = 1.5f + GetCcParam_()->m_Unk44;
    pWork->m_TargetPos = GetPos();
    pWork->m_TargetPos.y = fHeight;
    if (pBall == 0) {
        return;
    }
    if (pBall->GetShotChrIdx() == GetPlayerNo()) {
        return;
    }

    const bcs::FVEC3 &ofs = m_JumpTossCatchPos;
    const bcs::FVEC3 &pos = GetPos();
    bcs::FVEC3 low(pos.x, pos.y + ofs.y, pos.z);
    bcs::FVEC3 high(pos.x, pos.y + (ofs.y + fHeight), pos.z);
    float fFrame = pBall->GetTouchDownFrame2(low.y, (high.y - low.y) / 29.0f);
    bcs::FVEC3 ballPos;
    pBall->GetVolleyPosAfterFrame(ballPos, fFrame, 0);
    float fY = ballPos.y - ofs.y;
    fHeight = CLAMP(fY, fHeight, fHeight);
    pWork->m_TargetPos = ballPos;

    // Face the way the ball came from.
    bcs::FVEC3 vec;
    vec.x = pBall->GetPosD4().x - ballPos.x;
    vec.z = pBall->GetPosD4().z - ballPos.z;
    bcs::BCS_Vec_RotY(vec, ofs, WrapRad(nw4r::math::Atan2Rad(vec.x, vec.z)));
    bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &vec);

    const bcs::FVEC3 &rPos = GetPos();
    vec.x = pWork->m_TargetPos.x - rPos.x;
    vec.z = pWork->m_TargetPos.z - rPos.z;
    float fDist = bcs::BCS_Vec_GetLenXZ(vec);
    if (fDist > 1.5f) {
        fDist = 1.5f;
    }
    float fFrameMax = fFrame > 28.0f ? 28.0f : fFrame;
    fFrameMax = fFrameMax < 10.0f ? 10.0f : fFrameMax;
    pWork->m_MoveParam.m_nType = 11;
    pWork->m_MoveParam.m_fDist = fDist;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = fFrameMax;
    pWork->m_TargetPos.y = fHeight;
}

void msw::CChrCoreVolley::SubStateReceiveSetBallDragParam_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam, DIR_MOVE &rMove,
                                                         const bcs::FVEC3 &rDir, float fFrame, float fRate,
                                                         float fMin, float fMax, int bCurve)
{
    // Without a direction (a ball coming straight down) the move goes
    // the way the character faces, as far as the ball's speed in y.
    float fSpeedY = rDir.y;
    bcs::FVEC3 dir(rDir.x, fSpeedY, rDir.z);
    if (dir.x == 0.0f && dir.z == 0.0f) {
        bcs::BCS_VecZ_RotY(dir, fSpeedY, GetRotY());
    }
    float fDist = fFrame * (fRate * bcs::BCS_Vec_GetLenXZ(dir));
    if (fMin > fMax) {
        fMax = fMin;
    }
    fDist = CLAMP(fDist, fMin, fMax);
    long nType = 11;
    if (bCurve) {
        nType = 13;
    }
    rParam.m_nType = nType;
    rParam.m_fDist = fDist;
    rParam.m_fFrame = 0.0f;
    rParam.m_fFrameMax = fFrame;
    Util_SetDirMoveParam(rMove, WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z)));
}

void msw::CChrCoreVolley::SubStateReceiveDoMoveBallDrag_(STATE_WORK_RECEIVE *pWork)
{
    float fZero = 0.0f;
    if (!(pWork->m_DirMove.m_fRotY >= fZero)) {
        return;
    }
    pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
    float fFrame = pWork->m_fMoveFrame;
    if (pWork->m_MoveParam.m_nType < 13) {
        Util_DoDirMove(pWork->m_MoveParam, fFrame, pWork->m_DirMove, 0);
        return;
    }

    // Type 13: speeds up over the first 15% of the frames, slows down over
    // the last 35%.
    CC_DATA::CHR_PARAM::MOVE_PARAM &rParam = pWork->m_MoveParam;
    DIR_MOVE &rMove = pWork->m_DirMove;
    float fFrom = rParam.m_fFrame;
    float fTo = rParam.m_fFrameMax;
    float fLen = fTo - fFrom;
    if (rMove.m_fRotY < fZero || rParam.m_fDist == fZero || fLen <= fZero || fFrame < fFrom || fFrame >= fTo) {
        return;
    }
    float t = (fFrame - fFrom) / fLen;
    t = t > 1.0f ? 1.0f : (t < fZero ? fZero : t);
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    float fIn = 0.14999999f;
    float fOut = 0.35f;
    if (fIn <= 0.0f || fOut <= 0.0f) {
        fOut = 0.0f;
        fIn = 0.0f;
    }
    float fInRest = 1.0f - fIn;
    float fOutRest = 1.0f - fOut;
    if (t < fIn) {
        float s = t * (1.0f / fIn);
        s = CLAMP(s, 0.0f, 1.0f);
        float r = 0.0f;
        if (fInRest > 0.0f) {
            r = fOutRest / fInRest;
        }
        float fStart = fOut - r * fIn;
        if (fStart < 0.0f) {
            fStart = 0.0f;
        }
        float a = (fOut - fStart) * rParam.m_fDist;
        float b = fStart * rParam.m_fDist;
        pos.z = b * s + 2.0f * a * s - s * (a * s);
    } else {
        float s = (1.0f / fInRest) * (t - fIn);
        s = CLAMP(s, 0.0f, 1.0f);
        float fDist = rParam.m_fDist;
        pos.z = fDist * fOut + s * (2.0f * fDist * fOutRest) - s * (s * (fDist * fOutRest));
    }
    bcs::FVEC3 &rDone = *reinterpret_cast<bcs::FVEC3 *>(rMove.m_afPos);
    bcs::FVEC3Sub(&pos, &pos, &rDone);
    bcs::FVEC3Add(&rDone, &rDone, &pos);
    bcs::BCS_Vec_RotY(pos, pos, rMove.m_fRotY);
    AddPosXZ(pos.x, pos.z);
}

void msw::CChrCoreVolley::SubStateTossRotateOnBall_(STATE_WORK_RECEIVE *)
{
    // The third touch turns to the other court; otherwise the hand turns to
    // the team mate the ball goes to.
    long nTouch = 0;
    CBall *pBall = GetTargetBall_();
    if (pBall != 0) {
        nTouch = pBall->GetVolleyTouchNum(GetTeamNo());
        if (nTouch >= 2) {
            float fRotY;
            if (GetTargetCourtIdx() != 0) {
                fRotY = 3.1415927f;
            } else {
                fRotY = 0.0f;
            }
            RotateYRate_(fRotY, 0.2f);
            return;
        }
    }
    long nPassTo = SearchPassToChrIdx_(nTouch, 1);
    CChrCore *pTo = CChrMan::GetChrCore(nPassTo);
    if (pTo != 0) {
        bcs::FVEC3 hand;
        GetVNodePos(4, hand);
        float fDiff = GetRotY_Diff_ToPosXZ(hand);
        float fRotY = WrapRad(GetRotY_ToPosXZ(pTo->GetPos()) - fDiff);
        RotateYRate_(fRotY, 0.2f);
    }
}

void msw::CChrCoreVolley::ReceiveBall_(long nType, long nCatch, float fCharge, long nReturn, long nReceiveType,
                                       int bMessage)
{
    CBall *pBall = GetAttachBall();
    DetachBall();
    if (pBall != 0) {
        int nArts = pBall->TrgVolleyArtsOver(2, GetPlayerNo(), 1);
        if (nReturn <= 0 && (pBall->GetVolleyCanbeTouchKind(GetTeamNo(), GetPlayerNo()) & 0x10000)) {
            nReturn = 1;
        }
        if (nReturn > 0) {
            Util_ReturnBall_(pBall, nType, nReturn, nCatch, nArts, nReceiveType);
        } else {
            if (fCharge >= 1.0f) {
                pBall->SetVolleyChargePower(1.0f);
            } else if (fCharge <= -1.0f) {
                pBall->ClearVolleyChargePower();
            }
            Util_ReceiveBall_(pBall, nType, nCatch, nArts, nReceiveType);
        }
        if (bMessage) {
            CGame::SendMessage(0x53, GetPlayerNo(), 0, 0);
        }
    }
}

long msw::CChrCoreVolley::IsMustReturnBall_(CBall *pBall, long nReturn)
{
    if (nReturn <= 0 && (pBall->GetVolleyCanbeTouchKind(GetTeamNo(), GetPlayerNo()) & 0x10000)) {
        nReturn = 1;
    }
    return nReturn;
}

void msw::CChrCoreVolley::CtorStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    pWork->m_nStep = 0;
    pWork->m_fSpeedRate = 1.0f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 9, 1, 0, 0);
}

void msw::CChrCoreVolley::LoopStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    int bEnd = 0;
    switch (pWork->m_nStep) {
        case 0: {
            bcs::FVEC3 ofs(0.0f, 0.0f, 0.25f);
            SetBallCatchEnableRoot_(1.2f, 0.6f, ofs);
            AnimPlay(3, 0x6B, 0, 4.0f, 0.0f);
            SetMoveMotType(2);
            PlayVoice(0x92, 0);
            SE_PlayMove(0, 0x45A, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nStep = 1;
            break;
        }
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x6C, 1, 0.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        case 2:
            if (!pWork->m_bAccel) {
                // Slowing down unless the stick is held.
                float fFrame = 99.0f;
                pWork->m_fSpeedRate -= (1.0f / Max(fFrame, 1.0f)) * m_fFrameStep;
                pWork->m_fSpeedRate = CLAMP(pWork->m_fSpeedRate, 0.0f, 1.0f);
                if (pWork->m_fSpeedRate <= 0.0f) {
                    SetBallCatchDisable_();
                    SE_Stop(0, 8.0f);
                    AnimPlay(3, 0x6D, 0, 0.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            } else {
                float fFrame = 99.0f;
                pWork->m_fSpeedRate += (1.0f / Max(fFrame, 1.0f)) * m_fFrameStep;
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
        // Steering, by at most 8.5 degrees a frame.
        float fDiff = WrapRadPi(fRotYTrg - pWork->m_fRotY);
        float fMaxDeg = 8.5f;
        float fMax = DegToRad(fMaxDeg);
        float fRot = fDiff > fMax ? fMax : (fDiff < -fMax ? -fMax : fDiff);
        float fStep = fRot * m_fFrameStep;
        if (fDiff < 0.0f) {
            fDiff = Max(fStep, fDiff);
        } else {
            fDiff = fStep > fDiff ? fDiff : fStep;
        }
        pWork->m_fRotY = WrapRad(pWork->m_fRotY + fDiff);
        RotateYRate_(fRotYTrg, 0.2f);
    }

    float fSpeed = 1.1f * (0.016666668f * GetPrmMoveSpeed());
    bcs::FVEC3 pos = GetPos();
    MoveRunXZ_(pWork->m_fRotY, fSpeed, pWork->m_fSpeedRate);
    pWork->m_bAccel = 0;

    if (m_nStatusFlag & 1) {
        // A ball caught on the slide is sent on at once.
        long nType = 2;
        CBall *pBall = GetAttachBall();
        if (pBall != 0) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            float fPower = pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            int bFlick = 0;
            GetRotY();
            if (nReact & 0x800) {
                GetFlickDir_(pBall);
                bFlick = 1;
                if ((nReact & 0x8000) && !IsInvincible()) {
                    nType = 3;
                }
            }
            if (nReact & (0x10000 | 0x1000)) {
                nType = 2;
                if (nReact & 0x10000) {
                    nType = 4;
                }
            }
            if (nReact & 0x100000) {
                CGimmickMan::EffectCoinGetCreate(pBall->GetPos(), static_cast<DROP_COIN::CoinType>(0));
                CGame::AddTeamCoin(m_nTeamNo, 1);
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, -1, 0);
            if (bFlick) {
                float fRotY = GetRotY_FromPosXZ(pBall->GetPos());
                if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                    fRotY = WrapRad(nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                }
                MethdDamage(0xF, fRotY, fPower, 0.0f, -1, 0.0f, 0, 0);
                return;
            }
        }
    }

    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreVolley::DtorStateSliding()
{
    SE_Stop(0, 0.0f);
}

