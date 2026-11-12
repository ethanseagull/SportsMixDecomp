// msw::CChrCoreBoss's spike state: the jump to the ball, the hit and the
// landing. A file of its own (split from the receive states' at 0x246C8):
// its float constants are pooled again and restart in .rodata at 0x8B8.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMan.hpp"
#include "nw4r/math/arithmetic.h"

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi); degrees to
    // radians.
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }
}

void msw::CChrCoreBoss::CtorStateSpike()
{
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();

    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    m_nStatusFlag &= ~0x1F000000;
    pWork->m_nStep = 1;
    pWork->m_fDelay = 0.0f;
    pWork->m_nCourse = -1;
    pWork->m_fTargetRotY = GetSpikeTargetPosRot_(0);
    pWork->m_fUnk28 = 0.0f;
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
}

// (inline helper) One frame of the jump's move towards the target and of
// its turn; returns what Util_DoJumpMoveEX returned (2 and up once landed).
static inline int SpikeJumpMove(msw::CChrCoreBoss *pChr, msw::CChrCoreBoss::STATE_WORK_SPIKE *pWork)
{
    int nJump = pChr->Util_DoJumpMoveEX(0);
    if (nJump == 1) {
        pChr->RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fMoveFrame < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fMoveFrame += pChr->m_fFrameStep * pChr->m_fTimeScale;
            pWork->m_DirMove.m_fRotY = pChr->GetRotY_ToPosXZ(pWork->m_TargetPos);
            pChr->Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
        }
    }
    return nJump;
}

void msw::CChrCoreBoss::LoopStateSpike()
{
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();

    int bEnd = 0;
    switch (pWork->m_nStep) {
        case 0:
            // A delayed start.
            if (pWork->m_fDelay > 0.0f) {
                pWork->m_fDelay -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fDelay > 0.0f) {
                    break;
                }
            }
            pWork->m_fDelay = 0.0f;
            pWork->m_nStep = 1;
            // fall through
        case 1:
            // The jump.
            SetCollBodyPushRank_(4);
            m_UnkEC &= ~0x10;
            SetMoveMotType(4);
            pWork->m_fTargetRotY = GetSpikeTargetPosRot_(0);
            SubStateSpikeDoGetHomingTargetPos_(pWork, GetTargetBall_());
            if (pWork->m_nFlag & 0x40) {
                Util_SetJumpParam(40.0f, 15.0f, pWork->m_TargetPos.y);
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, 30.0f);
                pWork->m_fCatchRange = GetCcParam_()->m_Unk10C;
                bcs::FVEC3 ofs = m_SpikeOfsCharge;
                bcs::FVEC3 size(8.0f, 3.0f, 3.0f);
                bcs::FVEC3 size2(20.0f, 4.0f, 4.0f);
                EnableSlowCheck_(2, ofs, size, size2, 0.25f, 40.0f, 16.0f);
                SubStateSpikeDoSmashSpikeJumpEffect_(pWork);
            } else {
                Util_SetJumpParam(25.0f, 15.0f, pWork->m_TargetPos.y);
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, 18.75f);
                pWork->m_fCatchRange = GetCcParam_()->m_Unk90;
                bcs::FVEC3 ofs = m_SpikeOfs;
                bcs::FVEC3 size(8.0f, 3.0f, 3.0f);
                bcs::FVEC3 size2(20.0f, 4.0f, 4.0f);
                EnableSlowCheck_(1, ofs, size, size2, 0.75f, 40.0f, 16.0f);
            }
            m_fJumpGravity = -0.0125f;
            m_nStatusFlag |= 0x400;
            SetMoveMotStep_(1);
            PlayVoice(0x30, 0);
            PlaySePos(-1, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
            if (pWork->m_nFlag & 0x40) {
                AnimPlay(3, 0x66, 0, 8.0f, 0.0f);
            } else {
                AnimPlay(3, 0x5F, 0, 8.0f, 0.0f);
            }
            SetBallCatchDisable_();
            m_fBallCatchUnk7CC = 0.1f;
            pWork->m_nStep = 2;
            pWork->m_fMoveFrame = 0.0f;
            pWork->m_fInputWait = 16.0f;
            SetTargetMarkerEnable_(3, pWork->m_TargetPos, 0);
            // fall through
        case 2: {
            // In the air until the spike starts.
            int nJump = SpikeJumpMove(this, pWork);
            if (GetFlag(1)) {
                // The ball is caught in the air: hit it at once.
                long nMot = 0x60;
                if (pWork->m_nFlag & 0x40) {
                    nMot = 0x67;
                }
                AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                pWork->m_nFlag |= 1;
                pWork->m_fShotFrame = GetBallShotTrgFrame_(nMot, 7.0f);
                GetCcParam_();
                if (pWork->m_nFlag & 0x40) {
                    GetCcParam_();
                }
                if (nJump <= 1) {
                    float fFrame = 10.0f + pWork->m_fShotFrame;
                    if (m_fJumpUnk224 < fFrame) {
                        m_fJumpUnk224 = fFrame;
                    }
                    if (m_fJumpVelY < 0.0f) {
                        m_fJumpVelY = 0.0f;
                    }
                }
                ClearMoveParamWork_(pWork->m_MoveParam);
                Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, pWork->m_fShotFrame);
                DisableSlowCheck_(16.0f);
                pWork->m_nStep = 3;
                break;
            }

            long nInput = 0;
            pWork->m_fInputWait -= m_fFrameStep;
            if (pWork->m_fInputWait <= 0.0f) {
                nInput = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8F), 0, 0, 0, 0);
            }
            if (nInput >= 2 && GetTargetBall_() != 0) {
                nInput = 1;
            }
            if (nInput > 0) {
                // Start the spike.
                CBall *pBall = GetTargetBall_();
                if (nInput >= 1) {
                    if (CanbeUseItem_(pBall) && !(pWork->m_nFlag & 0x80) &&
                        DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x90), 0, 0, 0, 0)) {
                        pWork->m_nFlag |= 0x80;
                    }
                    if (pWork->m_nCourse == -1) {
                        long nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x91), 0, 0, 0, 0);
                        if (nCourse != -1 && nCourse != 6) {
                            pWork->m_nCourse = nCourse;
                        }
                    }
                }
                long nMot = 0x60;
                bcs::FVEC3 ofs = m_SpikeOfs;
                if (pWork->m_nFlag & 0x40) {
                    nMot = 0x67;
                    ofs = m_SpikeOfsCharge;
                }
                AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x9B), m_nPlayerNo, 0, 0, 0);
                if (pWork->m_nFlag & 0x40) {
                    SubStateSpikeDoSmashSpikeEffect_(pWork);
                }
                pWork->m_fShotFrame = GetBallShotTrgFrame_(nMot, 7.0f);
                pWork->m_nFlag |= 0x100;
                if (pBall != 0) {
                    // Jump again so that the hand meets the ball.
                    bcs::FVEC3 ballPos;
                    pBall->GetPosAfterFrame(ballPos, pWork->m_fShotFrame);
                    float fHeight = ballPos.y - (ofs.y + GetPosY());
                    float fFall = GetCcParam_()->m_UnkA0;
                    if (pWork->m_nFlag & 0x40) {
                        fFall = GetCcParam_()->m_Unk124;
                    }
                    if (fHeight >= 0.01f) {
                        float fMax;
                        if (pWork->m_nFlag & 0x40) {
                            fMax = pWork->m_fHeightOver + GetCcParam_()->m_Unk118;
                            if (m_fJumpVelY >= 0.0f) {
                                fMax += Max(0.0f, 3.2f - GetPosY());
                            }
                        } else {
                            fMax = pWork->m_fHeightOver + GetCcParam_()->m_Unk9C;
                            if (m_fJumpVelY >= 0.0f) {
                                fMax += Max(0.0f, 3.0f - GetPosY());
                            }
                        }
                        if (fHeight > fMax) {
                            fHeight = fMax;
                        }
                        Util_SetJumpParam(pWork->m_fShotFrame, fFall, fHeight);
                        SetMoveMotType(0);
                        m_fJumpGravity = -0.0125f;
                    } else if (nJump <= 1) {
                        float fFrame = fFall + pWork->m_fShotFrame;
                        if (m_fJumpUnk224 < fFrame) {
                            m_fJumpUnk224 = fFrame;
                        }
                        if (m_fJumpVelY < 0.0f) {
                            m_fJumpVelY = 0.0f;
                        }
                    }

                    // Move so that the hand is where the ball will be.
                    pWork->m_TargetPos = ballPos;
                    bcs::FVEC3 rot;
                    bcs::BCS_Vec_RotY(rot, ofs, pWork->m_fTargetRotY);
                    bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &rot);
                    const bcs::FVEC3 &rPos = GetPos();
                    bcs::FVEC3 pos;
                    bcs::FVEC3 target;
                    target.Set(pWork->m_TargetPos.x, 0.0f, pWork->m_TargetPos.z);
                    pos.Set(rPos.x, 0.0f, rPos.z);
                    float fDist = nw4r::math::FSqrt(bcs::FVEC3DistSq(&target, &pos));
                    float fRange = GetCcParam_()->m_Unk98;
                    if (pWork->m_nFlag & 0x40) {
                        fRange = GetCcParam_()->m_Unk114;
                    }
                    fRange += bcs::BCS_Vec_GetLenXZ(ofs);
                    if (fDist > fRange) {
                        fDist = fRange;
                    }
                    pWork->m_MoveParam.m_nType = 2;
                    pWork->m_MoveParam.m_fFrame = 0.0f;
                    pWork->m_MoveParam.m_fFrameMax = pWork->m_fShotFrame;
                    pWork->m_MoveParam.m_fDist = fDist;
                    pWork->m_fMoveFrame = 0.0f;
                    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                    CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, pWork->m_fShotFrame);
                }
                DisableSlowCheck_(16.0f);
                pWork->m_nStep = 3;
            } else if (m_fJumpVelY < 0.0f) {
                DisableSlowCheck_(16.0f);
            }
            if (pWork->m_nStep != 3 && nJump >= 2) {
                SubStateSpikeDoLandStart_(pWork);
            }
            break;
        }
        case 3:
            // The spike motion: the ball is hit at the shot frame.
            if (!(pWork->m_nFlag & 1) && pWork->m_nCourse == -1) {
                long nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x91), 0, 0, 0, 0);
                if (nCourse != -1 && nCourse != 6) {
                    pWork->m_nCourse = nCourse;
                }
            }
            if (pWork->m_nFlag & 1) {
                pWork->m_nFlag |= 0x200;
            }
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame - 1.0f) {
                if (m_nStatusFlag & 1) {
                    // Hit the ball, and fall back.
                    float fFrame = GetCcParam_()->m_UnkA4;
                    float fDist = GetCcParam_()->m_UnkA8;
                    float fJumpFrame = GetCcParam_()->m_UnkAC;
                    float fJumpFrom = GetCcParam_()->m_UnkB0;
                    float fJumpHeight = GetCcParam_()->m_UnkB4;
                    if (pWork->m_nFlag & 0x40) {
                        fFrame = GetCcParam_()->m_Unk128;
                        fDist = GetCcParam_()->m_Unk12C;
                        fJumpFrame = GetCcParam_()->m_Unk130;
                        fJumpFrom = GetCcParam_()->m_Unk134;
                        fJumpHeight = GetCcParam_()->m_Unk138;
                    }
                    pWork->m_MoveParam.m_nType = 11;
                    pWork->m_MoveParam.m_fFrame = 0.0f;
                    pWork->m_MoveParam.m_fFrameMax = fFrame;
                    pWork->m_MoveParam.m_fDist = fDist;
                    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                    pWork->m_fMoveFrame = 0.0f;
                    float fRotY = GetRotY();
                    if (GetBossTargetMarkerPos(pWork->m_TargetPos, 0)) {
                        fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
                    }
                    bcs::BCS_VecZ_RotY(pWork->m_TargetPos, -2.0f * fDist, fRotY);
                    const bcs::FVEC3 &rPos = GetPos();
                    pWork->m_TargetPos.x += rPos.x;
                    pWork->m_TargetPos.z += rPos.z;
                    Util_SetJumpParam(fJumpFrame, fJumpFrom, fJumpHeight);
                    m_fJumpGravity = -0.0125f;

                    long nType = 0;
                    float fRate = 0.0f;
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0 && (pBall->CanbeVolleySpikeVariation() & 4) &&
                        pBall->GetVolleyPassCounter() > fRate) {
                        nType = 2;
                    }
                    long nCourse = pWork->m_nCourse;
                    int bItem = (pWork->m_nFlag >> 7) & 1;
                    pBall = GetAttachBall();
                    DetachBall();
                    if (pBall != 0) {
                        Util_SpikeBall_(pBall, nCourse, nType, fRate, bItem);
                    }
                    SetBallCatchDisable_();
                    pWork->m_nFlag |= 1;
                }
                float fEnd = 1.0f + pWork->m_fShotFrame;
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fEnd) {
                    SetBallCatchDisable_();
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                // The ball can be hit around the shot frame.
                float fFrame = AnimGetFrame(3);
                if (fFrame >= pWork->m_fShotFrame - 3.0f && fFrame < 1.0f + pWork->m_fShotFrame) {
                    SetBallCatchEnableVNode_(pWork->m_fCatchRange, 4);
                }
            }
            if (IsAnimEnd(3)) {
                pWork->m_nStep = 4;
                pWork->m_nFlag |= 0x200;
            }
            // fall through
        case 4:
            // Falling.
            if (SpikeJumpMove(this, pWork) >= 2) {
                SubStateSpikeDoLandStart_(pWork);
            }
            break;
        case 5:
            // Landing.
            if (IsAnimEnd(3)) {
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreBoss::SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreBoss::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreBoss::DtorStateSpike()
{
    ArtsOptDestroy();
    SetTargetMarkerDisable_();
    SetBallCatchDisable_();
    m_nStatusFlag &= ~0x800;
    DisableSlowCheck_(16.0f);
}

void msw::CChrCoreBoss::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    long nMot = 0x61;
    float fTurn = 0.0f;
    float fBlend = fTurn;
    if (pWork->m_nFlag & 0x40) {
        if (pWork->m_nFlag & 0x100) {
            // After a super spike: 0x68 when the ball was hit, else 0x69.
            nMot = (pWork->m_nFlag & 1) ? 0x68 : 0x69;
            fTurn = GetCcParam_()->m_Unk140;
            fBlend = GetCcParam_()->m_Unk13C;
        } else {
            nMot = 0x6A;
            fTurn = GetCcParam_()->m_Unk120;
            fBlend = GetCcParam_()->m_Unk11C;
        }
    }
    if (fTurn != 0.0f) {
        float fRotY = WrapRad(DegToRad(fTurn));
        fRotY = WrapRad(fRotY + GetRotY());
        Util_NodePatchRotY(fRotY);
        SetRotY(fRotY);
    }
    AnimPlay(3, nMot, 0, fBlend, 0.0f);
    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
    PlayLandSe(0);
    pWork->m_nStep = 5;
}

void msw::CChrCoreBoss::SubStateSpikeDoGetHomingTargetPos_(STATE_WORK_SPIKE *pWork, CBall *pBall)
{
    pWork->m_pBall = pBall;
    pWork->m_nFlag &= ~0x40;
    float fFrame = 25.0f;
    bcs::FVEC3 ofs = m_SpikeOfs;
    float fHeight = 3.0f;
    float fRange = 0.5f;
    float fStep = 15.0f;
    if (IsChargeBall_(pBall) && IsOffenseTeam()) {
        // A super spike.
        fFrame = 40.0f;
        pWork->m_nFlag |= 0x40;
        fHeight = 3.2f;
        ofs = m_SpikeOfsCharge;
        fRange = 0.8f;
    }
    float fY = fHeight + GetCcParam_()->m_Unk44;
    pWork->m_TargetPos = GetPos();
    pWork->m_TargetPos.y = fY;
    pWork->m_fHeightOver = 0.0f;
    if (pBall == 0 || pBall->GetShotChrIdx() == GetPlayerNo() || !IsOffenseTeam()) {
        return;
    }
    if (pBall->GetShotTeamIdx() != GetTeamNo()) {
        return;
    }

    // Hit the ball where it is on average over the frames of the spike.
    float fLow = fY - fRange;
    float fHigh = fY + fRange;
    bcs::FVEC3 ballPos;
    bcs::FVEC3 ballPos2;
    pBall->GetPosAfterFrame(ballPos, fFrame);
    pBall->GetPosAfterFrame(ballPos2, fFrame + fStep);
    bcs::FVEC3Add(&ballPos, &ballPos, &ballPos2);
    bcs::FVEC3Scale(&ballPos, &ballPos, 0.5f);
    bcs::FVEC3 vel;
    bcs::FVEC3 vel2;
    pBall->GetVelAfterFrame(vel, fFrame);
    pBall->GetVelAfterFrame(vel2, fFrame + fStep);
    if (vel.y <= 0.0f || vel2.y <= 0.0f) {
        // Falling by then: take it lower down.
        float fBallY = ballPos.y - ofs.y;
        if (fBallY < fY) {
            pWork->m_fHeightOver = fY - fBallY;
        }
        fY = fBallY;
    }
    fY = fY > fHigh ? fHigh : (fY < fLow ? fLow : fY);
    pWork->m_TargetPos = ballPos;
    bcs::FVEC3 rot;
    bcs::BCS_Vec_RotY(rot, ofs, GetSpikeTargetPosRot_(0));
    bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &rot);

    const bcs::FVEC3 &rPos = GetPos();
    rot.x = pWork->m_TargetPos.x - rPos.x;
    rot.z = pWork->m_TargetPos.z - rPos.z;
    float fDist = bcs::BCS_Vec_GetLenXZ(rot);
    float fDistMax = GetCcParam_()->m_Unk94;
    if (pWork->m_nFlag & 0x40) {
        fDistMax = GetCcParam_()->m_Unk110;
    }
    if (fDist > fDistMax) {
        fDist = fDistMax;
    }
    pWork->m_MoveParam.m_nType = 2;
    pWork->m_MoveParam.m_fDist = fDist;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = fFrame;
    pWork->m_TargetPos.y = fY;
}
