// msw::CChrCoreVolley's spike state (CtorStateSpike ... DtorStateSpike) and
// air trick state, the volleyball team special move, with the arts event
// members after them.
//
// The spike: the character waits (a quick attack of a combination), jumps
// towards where the ball will be (SubStateSpikeDoGetHomingTargetPos_), asks
// the driver whether and how to swing, hits the ball at the motion's
// trigger frame and lands. The air trick: the character jumps with a trick
// motion, pulls the ball in, hangs in the air until the driver asks for the
// shot (or the time runs out), shoots and is pushed back by the shot. The
// characters override the motions and effects (SubStateAirTrickPlay*) and
// some of the steps.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameGuideMan.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

namespace {

    const float PI2 = 6.2831855f;

    // The steps of the spike (STATE_WORK_SPIKE::m_nStep).
    enum {
        SPIKE_STEP_WAIT,      // a combination's quick attack waits
        SPIKE_STEP_JUMP,      // the jump starts
        SPIKE_STEP_AIR,       // in the air until the swing
        SPIKE_STEP_SWING,     // the spike's swing
        SPIKE_STEP_RECEIVE,   // (set elsewhere) a swing that takes the ball in
        SPIKE_STEP_FALL,      // after the swing, until the landing
        SPIKE_STEP_LAND       // the landing motion
    };

    // The steps of the air trick (STATE_WORK_AIR_TRICK::m_nStep).
    enum {
        TRICK_STEP_INIT,
        TRICK_STEP_START_TRICK,
        TRICK_STEP_TRICK,
        TRICK_STEP_READY,
        TRICK_STEP_SHOT,
        TRICK_STEP_HITBACK,
        TRICK_STEP_LAND
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

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // The power of a super spike the driver answers with 100 + percent,
    // as a rate of 0 to 1.
    inline float GetPowerRate(long nAnswer)
    {
        long nPercent = nAnswer - 100;
        nPercent = nPercent > 100 ? 100 : (nPercent < 0 ? 0 : nPercent);
        float fRate = 0.01f * nPercent;
        return CLAMP(fRate, 0.0f, 1.0f);
    }
}

// ---- Spike --------------------------------------------------------------

void msw::CChrCoreVolley::CtorStateSpike()
{
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();

    pWork->m_nGuideType = -1;
    SetCollBodyPushType_((BODY_COLL_SIZE_SET_TYPE)0, 8.0f);
    m_nStatusFlag &= ~0x1F000000;
    if (pWork->m_nFlag & 8) {
        // A combination's quick attack: wait on the ground first.
        m_nStatusFlag &= ~0xC20;
        SetMoveMotType(0);
        m_UnkEC |= 0x10;
        SetCollBodyPushRank_(1);
        SetCollBodyPushPower_(5);
        AnimPlay(3, 0x32, 0, 4.0f, 0.0f);
        pWork->m_nStep = SPIKE_STEP_WAIT;
        pWork->m_fWait = m_fFrameStep * m_fTimeScale + GetCcParam_()->m_Unk22C;
    } else {
        pWork->m_nStep = SPIKE_STEP_JUMP;
        pWork->m_fWait = 0.0f;
    }
    pWork->m_nSpikeType = -1;
    pWork->m_fRotY = GetSpikeTargetPosRot_(0);
    pWork->m_fPower = 0.0f;
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
}

// One step of the jump and, while it rises, of the turn and of the move to
// m_TargetPos. Returns Util_DoJumpMoveEX's result (2 and up once landed).
inline int msw::CChrCoreVolley::SubStateSpikeDoJumpMove_(STATE_WORK_SPIKE *pWork)
{
    int nResult = Util_DoJumpMoveEX(0);
    if (nResult == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fMoveFrame < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
        }
    }
    return nResult;
}

// The driver chooses the kind of spike (message 0x91; it is told 1 for a
// super spike and 2 when an item is used).
inline void msw::CChrCoreVolley::SubStateSpikeAskSpikeType_(STATE_WORK_SPIKE *pWork)
{
    long nKind = 0;
    if (pWork->m_nFlag & 0x10) {
        nKind = 1;
    }
    if (pWork->m_nFlag & 0x80) {
        long nItem = CGame::GetTeamItem(GetTeamNo(), 0);
        if (nItem == 1 || nItem == 5) {
            nKind = 2;
        }
    }
    long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x91, nKind, 0, 0, 0);
    if (nAnswer != -1 && nAnswer != 6) {
        pWork->m_nSpikeType = nAnswer;
    }
}

void msw::CChrCoreVolley::LoopStateSpike()
{
    STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
    int bEnd = 0;

    // The guides on screen (10: the spike, 11 to 13: its variations).
    if (CGameGuideMan::IsGuideDisp(10, GetPlayerNo())) {
        pWork->m_nFlag |= 0x1000;
    }
    if (!(pWork->m_nFlag & 0x4000)) {
        if (CGameGuideMan::IsGuideDisp(0xB, GetPlayerNo())) {
            pWork->m_nGuideType = 0xB;
        }
        if (CGameGuideMan::IsGuideDisp(0xC, GetPlayerNo())) {
            pWork->m_nGuideType = 0xC;
        }
        if (CGameGuideMan::IsGuideDisp(0xD, GetPlayerNo())) {
            pWork->m_nGuideType = 0xD;
        }
        if (pWork->m_nGuideType >= 0) {
            pWork->m_nFlag |= 0x2000;
        } else if (pWork->m_nFlag & 0x2000) {
            pWork->m_nFlag |= 0x4000;
        }
    }

    switch (pWork->m_nStep) {
        case SPIKE_STEP_WAIT:
            if (pWork->m_fWait > 0.0f) {
                pWork->m_fWait -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fWait > 0.0f) {
                    break;
                }
            }
            pWork->m_fWait = 0.0f;
            pWork->m_nStep = SPIKE_STEP_JUMP;
            // fall through
        case SPIKE_STEP_JUMP:
            SetCollBodyPushRank_(4);
            m_UnkEC &= ~0x10;
            SetMoveMotType(4);
            pWork->m_fRotY = GetSpikeTargetPosRot_(0);
            SubStateSpikeDoGetHomingTargetPos_(pWork, GetTargetBall_());
            if (pWork->m_nFlag & 0x40) {
                // A charged ball: the smash spike.
                Util_SetJumpParam(40.0f, 15.0f, pWork->m_TargetPos.y);
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 30.0f);
                pWork->m_fCatchRange = GetCcParam_()->m_Unk308;
                bcs::FVEC3 ofs = m_SpikeOfsCharge;
                bcs::FVEC3 range1(8.0f, 3.0f, 3.0f);
                bcs::FVEC3 range2(20.0f, 4.0f, 4.0f);
                EnableSlowCheck_(2, ofs, range1, range2, 0.7f, 40.0f, 16.0f);
                SE_PlayWrapAct(9, 0, -1);
                SubStateSpikeDoSmashSpikeJumpEffect_(pWork);
            } else {
                Util_SetJumpParam(32.0f, 11.0f, pWork->m_TargetPos.y);
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 24.0f);
                pWork->m_fCatchRange = GetCcParam_()->m_Unk204;
                bcs::FVEC3 ofs = m_SpikeOfs;
                bcs::FVEC3 range1(8.0f, 3.0f, 3.0f);
                bcs::FVEC3 range2(20.0f, 4.0f, 4.0f);
                EnableSlowCheck_(1, ofs, range1, range2, 0.75f, 40.0f, 16.0f);
            }
            m_fJumpGravity = -0.0125f;
            m_nStatusFlag |= 0x400;
            SetMoveMotStep_(1);
            PlayVoice(0x30, 0);
            PlaySePos(SubStateSpikeGetSmashSpikeSmashSe_(), 0, 0, -1, 0.0f, 1.0f, 0.0f);
            if (pWork->m_nFlag & 0x40) {
                AnimPlay(3, 0x33, 0, 8.0f, 0.0f);
            } else {
                AnimPlay(3, 0x2C, 0, 8.0f, 0.0f);
            }
            SetBallCatchDisable_();
            m_fBallCatchUnk830 = 0.1f;
            pWork->m_nStep = SPIKE_STEP_AIR;
            pWork->m_fMoveFrame = 0.0f;
            pWork->m_fAskWait = 16.0f;
            SetTargetMarkerEnable_(3, pWork->m_TargetPos, 0);
            // fall through
        case SPIKE_STEP_AIR: {
            int nJump = SubStateSpikeDoJumpMove_(pWork);
            if (m_nStatusFlag & 1) {
                // The ball is already in hand: swing at once.
                long nMot = 0x2D;
                if (pWork->m_nFlag & 0x40) {
                    nMot = 0x34;
                }
                long nMsg = 0;
                if (pWork->m_nFlag & 4) {
                    nMsg = 2;
                }
                AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x9B, GetPlayerNo(), nMsg, 0, 0);
                if (pWork->m_nFlag & 0x40) {
                    SE_PlayWrapAct(0xA, 0, -1);
                    SubStateSpikeDoSmashSpikeEffect_(pWork);
                }
                pWork->m_nFlag |= 1;
                pWork->m_fShotFrame = GetBallShotTrgFrame_(nMot, 7.0f);
                // (unused: the original reads the hang time and drops it)
                float fHang = GetCcParam_()->m_Unk214;
                if (pWork->m_nFlag & 0x40) {
                    fHang = GetCcParam_()->m_Unk320;
                }
                (void)fHang;
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
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, pWork->m_fShotFrame);
                DisableSlowCheck_(16.0f);
                pWork->m_nStep = SPIKE_STEP_SWING;
            } else {
                // Is a super spike possible? (target marker kind: 1 charged
                // ball, 2 super spike, 3 star)
                int bSuper = 1;
                pWork->m_nFlag &= ~0x10;
                m_nTargetMarkerFixedType = 0;
                CBall *pBall = GetTargetBall_();
                if (pBall) {
                    if (!IsChargeBall_(pBall)) {
                        bSuper = 0;
                    }
                    if (!(pBall->CanbeVolleySpikeVariation() & 4)) {
                        bSuper = 0;
                    }
                    if (bSuper) {
                        m_nTargetMarkerFixedType = 1;
                    }
                    if (m_nComLevel == 1) {
                        m_nTargetMarkerFixedType = 3;
                        bSuper = 0;
                    }
                } else {
                    bSuper = 0;
                }
                if (bSuper && GetCcParam_()->m_Unk00 == 1) {
                    pWork->m_nFlag |= 0x10;
                    m_nTargetMarkerFixedType = 2;
                }

                // Ask the driver whether to swing (1: spike, 2: power
                // spike, 3: charge it up, 4: charge none, 100 + percent:
                // the power of it).
                long nSwing = 0;
                pWork->m_fAskWait -= m_fFrameStep;
                if (pWork->m_fAskWait <= 0.0f) {
                    int bItem = 0;
                    if (CanbeUseItem_(GetTargetBall_()) && !IsCombinationSetter_()) {
                        bItem = 1;
                    }
                    if (!(pWork->m_nFlag & 0x800)) {
                        nSwing = DriverSendMessage((CChrDrv::MSG_ID)0x8F, bItem, 0, 0, 0);
                    }
                }
                if (!(pWork->m_nFlag & 0x20)) {
                    if (nSwing >= 2) {
                        CBall *pTarget = GetTargetBall_();
                        if (pTarget) {
                            if (!(pTarget->CanbeVolleySpikeVariation() & 0x20)) {
                                nSwing = 1;
                            } else {
                                pWork->m_nFlag |= 0x20;
                                if (nSwing >= 100) {
                                    pWork->m_fPower = GetPowerRate(nSwing) * GetCcParam_()->m_Unk344;
                                } else if (nSwing == 4) {
                                    pWork->m_fPower = 0.0f;
                                } else if (nSwing == 3) {
                                    nSwing = 0;
                                    pWork->m_fPower += m_fFrameStep;
                                    if (pWork->m_fPower >= GetCcParam_()->m_Unk344) {
                                        nSwing = 2;
                                    }
                                }
                            }
                        } else {
                            nSwing = 0;
                            pWork->m_nFlag |= 0x20;
                        }
                    }
                } else if (nSwing == 2 || nSwing == 0) {
                    nSwing = 2;
                } else if (nSwing == 4) {
                    pWork->m_fPower = 0.0f;
                } else if (nSwing >= 100) {
                    pWork->m_fPower = GetPowerRate(nSwing) * GetCcParam_()->m_Unk344;
                } else {
                    nSwing = 0;
                    pWork->m_fPower += m_fFrameStep;
                    if (pWork->m_fPower >= GetCcParam_()->m_Unk344) {
                        nSwing = 2;
                    }
                }

                int bAuto = 0;
                if (nSwing <= 0 && SubStateSpikeDoCombAutoSwingCheck_(pWork)) {
                    pWork->m_nSpikeType = 4;
                    nSwing = 1;
                    bAuto = 1;
                }

                if (nSwing > 0) {
                    CBall *pTarget = GetTargetBall_();
                    if (nSwing == 1) {
                        if (CanbeUseItem_(pTarget)) {
                            if (IsCombinationSetter_()) {
                                pWork->m_nFlag &= ~0x80;
                            } else if (!(pWork->m_nFlag & 0x80) &&
                                       DriverSendMessage((CChrDrv::MSG_ID)0x90, 0, 0, 0, 0)) {
                                pWork->m_nFlag |= 0x80;
                            }
                        }
                        if (pWork->m_nSpikeType == -1) {
                            SubStateSpikeAskSpikeType_(pWork);
                        }
                    } else {
                        pWork->m_nFlag |= 2;
                    }
                    int bSetter = IsCombinationSetter_();
                    if (!bAuto) {
                        pWork->m_nFlag |= 0x400;
                        if (IsPadCtrl() && bSetter) {
                            // (nothing left here in the original, which
                            // still makes both tests)
                        }
                    }

                    // The swing motion, and where the ball is hit from.
                    bcs::FVEC3 ofs = m_SpikeOfs;
                    if (pWork->m_nFlag & 2) {
                        pWork->m_fCatchRange = GetCcParam_()->m_Unk340;
                        AnimPlay(3, 0x2F, 0, 8.0f, 0.0f);
                        CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x9B, GetPlayerNo(), 1, 0, 0);
                        ofs = m_SpikeOfsPower;
                        pWork->m_fShotFrame = GetBallShotTrgFrame_(0x2F, 7.0f);
                    } else {
                        long nMot = 0x2D;
                        if (pWork->m_nFlag & 0x40) {
                            nMot = 0x34;
                            ofs = m_SpikeOfsCharge;
                        }
                        long nMsg = 0;
                        if (pWork->m_nFlag & 4) {
                            nMsg = 2;
                        }
                        AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                        CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x9B, GetPlayerNo(), nMsg, 0, 0);
                        if (pWork->m_nFlag & 0x40) {
                            SE_PlayWrapAct(0xA, 0, -1);
                            SubStateSpikeDoSmashSpikeEffect_(pWork);
                        }
                        pWork->m_fShotFrame = GetBallShotTrgFrame_(nMot, 7.0f);
                    }
                    pWork->m_nFlag |= 0x100;

                    if (pTarget) {
                        // Jump and move so that the ball is met at the
                        // trigger frame.
                        bcs::FVEC3 ballPos;
                        pTarget->GetPosAfterFrame(ballPos, pWork->m_fShotFrame);
                        float fHeight = ballPos.y - (ofs.y + GetPosY());
                        float fHang = GetCcParam_()->m_Unk214;
                        if (pWork->m_nFlag & 2) {
                            fHang = GetCcParam_()->m_Unk374;
                        } else if (pWork->m_nFlag & 0x40) {
                            fHang = GetCcParam_()->m_Unk320;
                        }
                        if (fHeight >= 0.01f) {
                            float fMax;
                            if (pWork->m_nFlag & 0x40) {
                                fMax = pWork->m_fHeightAdjust + GetCcParam_()->m_Unk314;
                                if (m_fJumpVelY >= 0.0f) {
                                    float fUp = 3.2f - GetPosY();
                                    fMax += 0.0f < fUp ? fUp : 0.0f;
                                }
                            } else {
                                fMax = pWork->m_fHeightAdjust + GetCcParam_()->m_Unk210;
                                if (m_fJumpVelY >= 0.0f) {
                                    float fUp = 2.9f - GetPosY();
                                    fMax += 0.0f < fUp ? fUp : 0.0f;
                                }
                            }
                            if (fHeight > fMax) {
                                fHeight = fMax;
                            }
                            Util_SetJumpParam(pWork->m_fShotFrame, fHang, fHeight);
                            SetMoveMotType(0);
                            m_fJumpGravity = -0.0125f;
                        } else {
                            if (nJump <= 1) {
                                float fFrame = fHang + pWork->m_fShotFrame;
                                if (m_fJumpUnk224 < fFrame) {
                                    m_fJumpUnk224 = fFrame;
                                }
                                if (m_fJumpVelY < 0.0f) {
                                    m_fJumpVelY = 0.0f;
                                }
                            }
                            if (pWork->m_nFlag & 2) {
                                float fFrame = fHang + pWork->m_fShotFrame;
                                if (m_fJumpUnk224 > fFrame) {
                                    m_fJumpUnk224 = fFrame;
                                }
                            }
                        }

                        pWork->m_TargetPos = ballPos;
                        bcs::FVEC3 rotOfs;
                        bcs::BCS_Vec_RotY(rotOfs, ofs, pWork->m_fRotY);
                        bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &rotOfs);
                        float fDist = DistXZ(pWork->m_TargetPos, GetPos());
                        float fMaxDist = GetCcParam_()->m_Unk20C;
                        if (pWork->m_nFlag & 0x40) {
                            fMaxDist = GetCcParam_()->m_Unk310;
                        }
                        fMaxDist += bcs::BCS_Vec_GetLenXZ(ofs);
                        if (fDist > fMaxDist) {
                            fDist = fMaxDist;
                        }
                        pWork->m_MoveParam.m_nType = 2;
                        pWork->m_MoveParam.m_fFrame = 0.0f;
                        pWork->m_MoveParam.m_fFrameMax = pWork->m_fShotFrame;
                        pWork->m_MoveParam.m_fDist = fDist;
                        pWork->m_fMoveFrame = 0.0f;
                        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                        CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, pWork->m_fShotFrame);
                    }
                    DisableSlowCheck_(16.0f);
                    pWork->m_nStep = SPIKE_STEP_SWING;
                } else if (m_fJumpVelY < 0.0f) {
                    DisableSlowCheck_(16.0f);
                }
                if (pWork->m_nStep != SPIKE_STEP_SWING && nJump >= 2) {
                    SubStateSpikeDoLandStart_(pWork);
                }
            }
            break;
        }
        case SPIKE_STEP_SWING: {
            if (!(pWork->m_nFlag & 3) && pWork->m_nSpikeType == -1) {
                SubStateSpikeAskSpikeType_(pWork);
            }
            if (pWork->m_nFlag & 1) {
                pWork->m_nFlag |= 0x200;
            }
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame - 1.0f) {
                if (m_nStatusFlag & 1) {
                    if (!(pWork->m_nFlag & 2)) {
                        // The swing pushes the character back, away from
                        // the target.
                        float fFrame = GetCcParam_()->m_Unk218;
                        float fDist = GetCcParam_()->m_Unk21C;
                        float fJump0 = GetCcParam_()->m_Unk220;
                        float fJump1 = GetCcParam_()->m_Unk224;
                        float fJump2 = GetCcParam_()->m_Unk228;
                        if (pWork->m_nFlag & 0x40) {
                            fFrame = GetCcParam_()->m_Unk324;
                            fDist = GetCcParam_()->m_Unk328;
                            fJump0 = GetCcParam_()->m_Unk32C;
                            fJump1 = GetCcParam_()->m_Unk330;
                            fJump2 = GetCcParam_()->m_Unk334;
                        }
                        pWork->m_MoveParam.m_nType = 0xB;
                        pWork->m_MoveParam.m_fFrame = 0.0f;
                        pWork->m_MoveParam.m_fFrameMax = fFrame;
                        pWork->m_MoveParam.m_fDist = fDist;
                        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                        pWork->m_fMoveFrame = 0.0f;
                        float fRotY = GetRotY();
                        if (GetVolleyTargetMarkerPos(pWork->m_TargetPos, 0)) {
                            fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
                        }
                        bcs::BCS_VecZ_RotY(pWork->m_TargetPos, -2.0f * fDist, fRotY);
                        const bcs::FVEC3 &rPos = GetPos();
                        pWork->m_TargetPos.x += rPos.x;
                        pWork->m_TargetPos.z += rPos.z;
                        Util_SetJumpParam(fJump0, fJump1, fJump2);
                        m_fJumpGravity = -0.0125f;
                    }

                    // The kind of shot (5 power spike, 3 super spike, 2 a
                    // variation, 1 charged ball, 4 flag 4).
                    long nShot = 0;
                    float fPower = 0.0f;
                    if (pWork->m_nFlag & 2) {
                        nShot = 5;
                        float fCharge = pWork->m_fPower;
                        float fMin = GetCcParam_()->m_Unk348;
                        float fMax = GetCcParam_()->m_Unk344;
                        float fRate;
                        if (fMin >= fMax || fCharge <= fMin) {
                            fRate = 0.0f;
                        } else {
                            fRate = (fCharge - fMin) / (fMax - fMin);
                        }
                        fPower = CLAMP(fRate, 0.0f, 1.0f);
                    } else if (pWork->m_nFlag & 0x10) {
                        nShot = 3;
                    } else {
                        CBall *pBall = GetAttachBall();
                        if (pBall) {
                            if ((pBall->CanbeVolleySpikeVariation() & 4) && pBall->GetVolleyPassCounter() > fPower) {
                                nShot = 2;
                            }
                            if (IsChargeBall_(pBall)) {
                                nShot = 1;
                            } else if (pWork->m_nFlag & 4) {
                                nShot = 4;
                            }
                        }
                    }

                    // The guide counts a spike of the kind it shows.
                    if ((pWork->m_nFlag & 0x400) && (pWork->m_nFlag & 0x2000) && pWork->m_nGuideType >= 0 &&
                        CGameGuideMan::IsDispEnable()) {
                        int bSuccess = 0;
                        if (nShot == 5) {
                            if (pWork->m_nGuideType == 0xC) {
                                bSuccess = 1;
                            }
                        } else if (pWork->m_nGuideType != 0xC) {
                            if (pWork->m_nGuideType == 0xD) {
                                if (IsHaveStockItem() && !(m_nStatusFlag & 0x20000) && (pWork->m_nFlag & 0x80)) {
                                    bSuccess = 1;
                                }
                            } else {
                                bSuccess = 1;
                            }
                        }
                        if (bSuccess) {
                            CGameGuideMan::AddGuideSuccessCount(pWork->m_nGuideType, GetPadNo());
                        }
                    }

                    long nSpikeType = pWork->m_nSpikeType;
                    int bItem = (pWork->m_nFlag & 0x80) != 0;
                    int bCharge = (pWork->m_nFlag & 0x40) != 0;
                    CBall *pBall = GetAttachBall();
                    DetachBall();
                    if (pBall) {
                        Util_SpikeBall_(pBall, nSpikeType, nShot, fPower, bCharge, bItem);
                    }
                    SetBallCatchDisable_();
                    pWork->m_nFlag |= 1 | 0x10000;
                }
                float fEnd = 1.0f + pWork->m_fShotFrame;
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fEnd) {
                    SetBallCatchDisable_();
                    pWork->m_nFlag |= 0x10000;
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                // The ball can be caught at the hand around the trigger.
                float fFrame = AnimGetFrame(3);
                if (fFrame >= pWork->m_fShotFrame - 3.0f && fFrame < 1.0f + pWork->m_fShotFrame) {
                    SetBallCatchEnableVNode_(pWork->m_fCatchRange, bcs::FVEC3(0.0f, 0.0f, 0.0f), 4);
                }
            }
            if (IsAnimEnd(3)) {
                pWork->m_nStep = SPIKE_STEP_FALL;
                pWork->m_nFlag |= 0x200 | 0x10000;
            }
            if (SubStateSpikeDoJumpMove_(pWork) >= 2) {
                SubStateSpikeDoLandStart_(pWork);
            }
            break;
        }
        case SPIKE_STEP_RECEIVE: {
            if (pWork->m_nFlag & 1) {
                pWork->m_nFlag |= 0x200;
            }
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame - 1.0f) {
                if (m_nStatusFlag & 1) {
                    ReceiveBall_(4, 0, 0.0f, 0, -1, 0);
                    pWork->m_nFlag |= 1;
                }
                float fEnd = 1.0f + pWork->m_fShotFrame;
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fEnd) {
                    SetBallCatchDisable_();
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                float fFrame = AnimGetFrame(3);
                if (fFrame >= pWork->m_fShotFrame - 3.0f && fFrame < 1.0f + pWork->m_fShotFrame) {
                    SetBallCatchEnableVNode_(pWork->m_fCatchRange, bcs::FVEC3(0.0f, 0.0f, 0.0f), 4);
                }
            }
            if (IsAnimEnd(3)) {
                pWork->m_nStep = SPIKE_STEP_FALL;
                pWork->m_nFlag |= 0x200;
            }
            if (SubStateSpikeDoJumpMove_(pWork) >= 2) {
                SubStateSpikeDoLandStart_(pWork);
            }
            break;
        }
        case SPIKE_STEP_FALL:
            if (SubStateSpikeDoJumpMove_(pWork) >= 2) {
                SubStateSpikeDoLandStart_(pWork);
            }
            break;
        case SPIKE_STEP_LAND:
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
        FsmqDoChain();
    }
}

void msw::CChrCoreVolley::SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreVolley::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreVolley::DtorStateSpike()
{
    Eff_StopGroup(4);
    ArtsOptDestroy();
    SetTargetMarkerDisable_();
    SetBallCatchDisable_();
    m_nStatusFlag &= ~0x800;
    DisableSlowCheck_(16.0f);
    CombinationEnd_();
    m_ExAttachIcon.StopDisp(1);
}

// In a combination (flag 0x800) the attacker swings by itself once the ball
// the setter passed is about to reach the height of the hand, is within one
// unit of it, or the jump is falling at its end.
int msw::CChrCoreVolley::SubStateSpikeDoCombAutoSwingCheck_(STATE_WORK_SPIKE *pWork)
{
    if (!(pWork->m_nFlag & 0x800)) {
        return 0;
    }
    CBall *pBall = pWork->m_pBall;
    if (pBall == 0) {
        return 0;
    }
    if (pBall->GetPassChrIdx() != GetPlayerNo()) {
        return 0;
    }

    float fHeight = 2.9f;
    bcs::FVEC3 ofs;
    GetVolleySpikeOffsetPos(0, ofs);
    if (pWork->m_nFlag & 0x40) {
        fHeight = 3.2f;
        GetVolleySpikeOffsetPos(1, ofs);
    }
    bcs::BCS_Vec_RotY(ofs, ofs, pWork->m_fRotY);
    float fFrame = 10.0f;
    fHeight += ofs.y;
    if (IsCombinationSetter_()) {
        fFrame = 0.0f;
        fHeight -= 0.3f;
    }
    if (pBall->GetHeightPassFrame(fHeight, 0) <= fFrame) {
        return 1;
    }

    bcs::FVEC3 pos = GetPos();
    bcs::FVEC3Add(&pos, &pos, &ofs);
    bcs::FVEC3Sub(&pos, &pos, &pBall->GetPos());
    if (bcs::FVEC3SquareMag(&pos) <= 1.0f) {
        return 1;
    }
    if (m_fJumpVelY <= 0.0f && m_fJumpUnk224 <= fFrame) {
        return 1;
    }
    return 0;
}

// The landing: the motion (and a turn of the parameters' angle after a
// smash spike), the effect and the sound.
void msw::CChrCoreVolley::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);

    long nMot = 0x2E;
    float fRotDeg = 0.0f;
    float fBlend = fRotDeg;
    if (pWork->m_nFlag & 2) {
        nMot = 0x2E;
    } else if (pWork->m_nFlag & 0x40) {
        if (pWork->m_nFlag & 0x100) {
            nMot = (pWork->m_nFlag & 1) ? 0x35 : 0x36;
            fRotDeg = GetCcParam_()->m_Unk33C;
            fBlend = GetCcParam_()->m_Unk338;
        } else {
            nMot = 0x37;
            fRotDeg = GetCcParam_()->m_Unk31C;
            fBlend = GetCcParam_()->m_Unk318;
        }
    }
    if (0.0f != fRotDeg) {
        float fRotY = WrapRad(bcs::DEG_TO_RAD(fRotDeg));
        fRotY = WrapRad(fRotY + GetRotY());
        Util_NodePatchRotY(fRotY);
        SetRotY(fRotY);
    }
    AnimPlay(3, nMot, 0, fBlend, 0.0f);
    Util_SetEffect((SET_EFFECT_TYPE)0);
    PlayLandSe(0);
    pWork->m_nStep = SPIKE_STEP_LAND;
}

// Where the spike jump goes: under the point the ball will be at when the
// swing meets it (half way through the swing's frames), less the offset of
// the hand, and how high.
void msw::CChrCoreVolley::SubStateSpikeDoGetHomingTargetPos_(STATE_WORK_SPIKE *pWork, CBall *pBall)
{
    pWork->m_pBall = pBall;
    pWork->m_nFlag &= ~0x40;
    float fFrame = 32.0f;
    bcs::FVEC3 ofs = m_SpikeOfs;
    int bCharge = 0;
    float fHeight = 2.9f;
    float fRange = 0.5f;
    float fSwing = 11.0f;
    if (IsChargeBall_(pBall) && IsOffenseTeam()) {
        bCharge = 1;
    }
    if (bCharge) {
        fFrame = 40.0f;
        pWork->m_nFlag |= 0x40;
        fHeight = 3.2f;
        ofs = m_SpikeOfsCharge;
        fRange = 0.8f;
        fSwing = 15.0f;
    }

    float fY = fHeight + GetCcParam_()->m_Unk44;
    pWork->m_TargetPos = GetPos();
    pWork->m_TargetPos.y = fY;
    pWork->m_fHeightAdjust = 0.0f;
    // Home in only on a ball the other side shot into this team's half.
    if (pBall && pBall->GetShotChrIdx() != GetPlayerNo() && IsOffenseTeam() &&
        pBall->GetShotTeamIdx() == GetTeamNo()) {
        float fMin = fY - fRange;
        float fMax = fY + fRange;
        bcs::FVEC3 pos;
        bcs::FVEC3 pos2;
        pBall->GetPosAfterFrame(pos, fFrame);
        pBall->GetPosAfterFrame(pos2, fFrame + fSwing);
        bcs::FVEC3Add(&pos, &pos, &pos2);
        bcs::FVEC3Scale(&pos, &pos, 0.5f);
        bcs::FVEC3 vel;
        bcs::FVEC3 vel2;
        pBall->GetVelAfterFrame(vel, fFrame);
        pBall->GetVelAfterFrame(vel2, fFrame + fSwing);
        if (vel.y <= 0.0f || vel2.y <= 0.0f) {
            // The ball is falling: meet it lower down.
            float fBallY = pos.y - ofs.y;
            if (fBallY < fY) {
                pWork->m_fHeightAdjust = fY - fBallY;
            }
            fY = fBallY;
        }
        fY = CLAMP(fY, fMin, fMax);

        pWork->m_TargetPos = pos;
        bcs::FVEC3 vec;
        bcs::BCS_Vec_RotY(vec, ofs, GetSpikeTargetPosRot_(0));
        bcs::FVEC3Sub(&pWork->m_TargetPos, &pWork->m_TargetPos, &vec);
        // (the same vector again, now from the character to the target)
        const bcs::FVEC3 &rPos = GetPos();
        vec.x = pWork->m_TargetPos.x - rPos.x;
        vec.z = pWork->m_TargetPos.z - rPos.z;
        float fDist = bcs::BCS_Vec_GetLenXZ(vec);
        float fMaxDist = GetCcParam_()->m_Unk208;
        if (pWork->m_nFlag & 0x40) {
            fMaxDist = GetCcParam_()->m_Unk30C;
        }
        if (fDist > fMaxDist) {
            fDist = fMaxDist;
        }
        pWork->m_MoveParam.m_nType = 2;
        pWork->m_MoveParam.m_fDist = fDist;
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = fFrame;
        pWork->m_TargetPos.y = fY;
    }
}

long msw::CChrCoreVolley::SubStateSpikeGetSmashSpikeSmashSe_()
{
    return -1;
}

// ---- Air trick ----------------------------------------------------------

void msw::CChrCoreVolley::CtorStateAirTrick()
{
    // Without a free ball the air trick ends at once.
    int bNoBall = 0;
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        bNoBall = 1;
    } else {
        if (pBall->IsFlag1()) {
            bNoBall = 1;
        }
        if (pBall->IsAttach()) {
            bNoBall = 1;
        }
    }
    if (bNoBall) {
        FsmqDoChain();
        return;
    }

    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    SetCollBodyPushType_((BODY_COLL_SIZE_SET_TYPE)0, 8.0f);
    SetCollBodyPushRank_(4);
    SetCollBodyPushPower_(5);
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x1F000000;
    Eff_EraseAll();
    SetCondition(0, 0.0f, 0);
    m_nStatusFlag |= 0x40000000;
    SetMoveMotType(4);

    pWork->m_nStep = TRICK_STEP_INIT;
    pWork->m_nShotChoice = -1;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_nArtsLevel = 0;
    pWork->m_fRotY = GetSpikeTargetPosRot_(0);
    pWork->m_nAdsorbStep = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_fAdsorbFrameMax = 0.0f;
    pWork->m_AdsorbOfs = m_AdsorbOfs;

    CGame::ClearTeamArtsGauge(GetTeamNo());
    CGame::TrgArtsEvent(GetPlayerNo());
    CGame::SendMessage(0xD, GetPlayerNo(), GetCcParam_()->m_Unk378, 0);

    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 40.0f;
    pWork->m_fAdsorbUnk60 = 40.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam_()->m_Unk388 + GetCcParam_()->m_Unk38C;

    CBall *pTarget = GetTargetBall_();
    if (pTarget) {
        SubStateAirTrickStartBallAdsorb_(pWork, pTarget);
    }
}

void msw::CChrCoreVolley::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case TRICK_STEP_INIT:
            pWork->m_nStep = TRICK_STEP_START_TRICK;
            break;
        case TRICK_STEP_START_TRICK:
            Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
            PlayVoice(0x43, 0);
            SubStateAirTrickStartTrick_(pWork);
            pWork->m_nStep = TRICK_STEP_TRICK;
            break;
        case TRICK_STEP_TRICK:
            if (SubStateAirTrickIsReadyArtsShot_(pWork)) {
                SubStateAirTrickReadyArtsShot_(pWork);
                pWork->m_nStep = TRICK_STEP_READY;
            }
            break;
        case TRICK_STEP_READY:
            if (SubStateAirTrickIsStartArtsShot_(pWork)) {
                SubStateAirTrickStartArtsShot_(pWork);
                pWork->m_nStep = TRICK_STEP_SHOT;
            }
            break;
        case TRICK_STEP_SHOT:
            if (!(pWork->m_nFlag & 2) && pWork->m_nShotChoice == -1) {
                long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x94, 0, 0, 0, 0);
                if (nAnswer != -1) {
                    pWork->m_nShotChoice = nAnswer;
                }
            }
            if (SubStateAirTrickIsEndArtsShot_(pWork)) {
                SubStateAirTrickStartArtsHitback_(pWork);
                pWork->m_nStep = TRICK_STEP_HITBACK;
            }
            break;
        case TRICK_STEP_HITBACK:
            if (SubStateAirTrickIsEndArtsShotAfter_(pWork)) {
                SubStateAirTrickDoLandStart_(pWork);
            }
            break;
        case TRICK_STEP_LAND:
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

void msw::CChrCoreVolley::DtorStateAirTrick()
{
    Eff_StopGroup(4);
    SetTargetMarkerDisable_();
    SetBallCatchDisable_();
    m_nStatusFlag &= ~0x800;
    ArtsOptDestroy();
    m_nStatusFlag &= ~0x40000000;
}

// One step of the jump and, while it rises, of the turn and of the move to
// m_TargetPos. Returns Util_DoJumpMoveEX's result (2 and up once landed).
int msw::CChrCoreVolley::SubStateAirTrickDoJumpMove_(STATE_WORK_AIR_TRICK *pWork)
{
    int nResult = Util_DoJumpMoveEX(0);
    if (nResult == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnk20 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnk20 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnk20, pWork->m_DirMove, 0);
        }
    }
    return nResult;
}

void msw::CChrCoreVolley::SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 60.0f;
    pWork->m_fAdsorbUnk60 = 60.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam_()->m_Unk388 + GetCcParam_()->m_Unk38C;

    SubStateAirTrickSetTrickFrameParam_(pWork, 0);
    SubStateAirTrickPlayTrickMotion_(pWork);
    SubStateAirTrickSetTrickJumpParam_(pWork, 0);
    SubStateAirTrickSetTrickMoveParam_(pWork, 0);
    CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, SubStateAirTrickGetTrickRotFrame_(pWork));
    if (SubStateAirTrickIsTMStartAuto_(pWork)) {
        SubStateAirTrickStartTMDisp_(pWork);
    }
}

// The trick lasts the frames the character's parameters give it; its motion
// is played faster or slower to fit them.
void msw::CChrCoreVolley::SubStateAirTrickSetTrickFrameParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    float fMotFrame = SubStateAirTrickGetTrickMotFrame_(pWork, nTrick);
    fMotFrame = Max(1.0f, fMotFrame - GetCcParam_()->m_Unk384);
    pWork->m_fAdsorbUnk60 = fMotFrame;
    float fFrame = SubStateAirTrickGetTrickDoFrame_(pWork, nTrick);
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
        pWork->m_fAnimSpeed = 1.0f;
    } else {
        pWork->m_fAnimSpeed = fMotFrame / fFrame;
    }
    pWork->m_fTrickFrame = fFrame;
    pWork->m_fTrickCount = 0.0f;
}

float msw::CChrCoreVolley::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x75);
}

float msw::CChrCoreVolley::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return GetCcParam_()->m_Unk380;
}

void msw::CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    AnimPlay(3, 0x75, 0, GetCcParam_()->m_Unk3AC, 0.0f);
    AnimSetSpeed(3, pWork->m_fAnimSpeed);
    ArtsOptAnimSetSpeed(pWork->m_fAnimSpeed);
    SE_PlayArtsJumpSE(0);
}

float msw::CChrCoreVolley::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return pWork->m_fTrickFrame;
}

void msw::CChrCoreVolley::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    float fHeight = GetCcParam_()->m_Unk37C;
    float fPosY = GetPosY();
    if (fHeight < fPosY) {
        fHeight = fPosY;
    }
    Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
    m_fJumpUnk224 = pWork->m_fJumpUnk;
    m_fJumpUnk228 = 0.0f;
}

// The air trick does not move.
void msw::CChrCoreVolley::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    pWork->m_TargetPos = GetPos();
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk20 = 0.0f;
}

// The ball is pulled in to the hand over the trick motion's frames.
void msw::CChrCoreVolley::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_nAdsorbStep = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs = m_AdsorbOfs;
    float fFrame = SubStateAirTrickGetTrickMotFrame_(pWork, 0);
    fFrame = Max(1.0f, fFrame - GetCcParam_()->m_Unk384);
    pWork->m_fAdsorbFrameMax = fFrame;
    AttachBall(pBall->GetBallNo(), -1, fFrame, 0x20);
    m_nStatusFlag &= ~0x2C;
}

float msw::CChrCoreVolley::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return pWork->m_fTrickFrame;
}

int msw::CChrCoreVolley::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 1;
}

void msw::CChrCoreVolley::SubStateAirTrickStartTMDisp_(STATE_WORK_AIR_TRICK *)
{
    SetTargetMarkerEnable_(6, GetPos(), 0);
}

// Waits for the trick to end; the loop motion follows the trick motion.
int msw::CChrCoreVolley::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);
    if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
        return 1;
    }
    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return 0;
}

// The ball comes in along a quarter circle from above to the hand's offset.
void msw::CChrCoreVolley::SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *pWork)
{
    static const float fHeightMax = 10.0f;

    CBall *pBall = GetAttachBall();
    if (pBall != 0 && pWork->m_nAdsorbStep == 0) {
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_RotY(ofs, pWork->m_AdsorbOfs, pWork->m_fRotY);
        if (pWork->m_bAdsorbEnd == 0) {
            float fFrameMax = pWork->m_fAdsorbFrameMax;
            if (fFrameMax > 0.0f) {
                pWork->m_fAdsorbFrame += m_fFrameStep * m_fTimeScale;
                pWork->m_fAdsorbFrame = Min(pWork->m_fAdsorbFrame, fFrameMax);
                float fRate = pWork->m_fAdsorbFrame / fFrameMax;
                fRate = CLAMP(fRate, 0.0f, 1.0f);
                bcs::FVEC3Scale(&ofs, &ofs, fRate);
                float fHeight = fHeightMax * nw4r::math::CosRad(1.5707964f * fRate);
                fHeight = CLAMP(fHeight, 0.0f, fHeightMax);
                ofs.y += fHeight;
                if (fRate >= 1.0f) {
                    pWork->m_bAdsorbEnd = 1;
                    SubStateAirTrickTrgBallAdsorbEnd_(pWork);
                }
            }
        }
        pBall->SetAttachPos(ofs, 0);
    }
}

void msw::CChrCoreVolley::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
}

void msw::CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0x7A, 1, GetCcParam_()->m_Unk3B0, 0.0f);
}

// The character hangs in the air at the top of the jump with the attack
// effect, then falls slowly while he waits for the shot.
void msw::CChrCoreVolley::SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->TrgVolleyArtsReady(m_nChrId, 1);
    }
    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    bcs::FVEC3 pos;
    GetVNodePos(1, pos);
    CEffectCtrl::SimpleEffect(0x4EF, pos, 3, 0, 0);
    PlaySePos(0xC07, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
    SE_PlayArtsLoopSE();

    float fWait = GetCcParam_()->m_Unk388;
    float fFrame = GetCcParam_()->m_Unk38C;
    float fY = GetPosY();
    Util_SetJumpParam(fFrame, fY, fY - GetCcParam_()->m_Unk390, fY);
    m_fJumpUnk224 = fWait;
    m_fJumpUnk228 = 0.0f;
}

// The shot starts when the driver answers (it is told the frames in all and
// the frames gone) or the time runs out (flag 2).
int msw::CChrCoreVolley::SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bStart = 0;
    SubStateAirTrickDoBallAdsorb_(pWork);
    if (pWork->m_fJumpUnk > 0.0f) {
        pWork->m_fJumpUnk -= m_fFrameStep;
    }
    long nFrame = GetCcParam_()->m_Unk388 + GetCcParam_()->m_Unk38C;
    long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x93, nFrame, nFrame - pWork->m_fJumpUnk, 0, 0);
    if (nAnswer != 0 || pWork->m_fJumpUnk <= 0.0f) {
        if (nAnswer != 0) {
            if (pWork->m_nShotChoice == -1) {
                long nChoice = DriverSendMessage((CChrDrv::MSG_ID)0x94, 0, 0, 0, 0);
                if (nChoice != -1) {
                    pWork->m_nShotChoice = nChoice;
                }
            }
        } else {
            pWork->m_nFlag |= 2;
        }
        bStart = 1;
    }
    SubStateAirTrickDoJumpMove_(pWork);
    if (!bStart && pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bStart;
}

void msw::CChrCoreVolley::SubStateAirTrickStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 8.0f);
    SubStateAirTrickPlayArtsShotMotion_(pWork);
    SubStateAirTrickDoGetBallShotTrg_(pWork);
}

void msw::CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    float fBlend = GetCcParam_()->m_Unk3B4;
    if (pWork->m_nArtsLevel < 1) {
        fBlend = GetCcParam_()->m_Unk3B8;
    }
    AnimPlay(3, 0x7B, 0, fBlend, 0.0f);
}

void msw::CChrCoreVolley::SubStateAirTrickDoGetBallShotTrg_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fShotFrame = GetBallShotTrgFrame_(AnimGetMotID(3, 0), GetCcParam_()->m_Unk3A4);
}

// The shot aims at the target marker; the ball leaves at the trigger frame.
int msw::CChrCoreVolley::SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    bcs::FVEC3 pos;
    if (GetVolleyTargetMarkerPos(pos, 0)) {
        pWork->m_fRotY = GetRotY_ToPosXZ(pos);
    }
    if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
        Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
        SubStateAirTrickShootBall_(pWork);
        return 1;
    }
    return 0;
}

void msw::CChrCoreVolley::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    DetachBall();
    if (pBall) {
        pWork->m_nFlag |= 1;
        Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, !(pWork->m_nFlag & 8));
        Util_ArtsShootBall_(pBall, pWork->m_ShootPos);
    }
    SetBallCatchDisable_();
}

// The shot pushes the character back: a jump backwards, away from the
// direction he shot in.
void msw::CChrCoreVolley::SubStateAirTrickStartArtsHitback_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = GetCcParam_()->m_Unk3C0;
    pWork->m_MoveParam.m_fDist = GetCcParam_()->m_Unk3C4;
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk20 = 0.0f;
    CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, GetCcParam_()->m_Unk3C0);

    bcs::BCS_VecZ_RotY(pWork->m_TargetPos, -2.0f * GetCcParam_()->m_Unk3C4, pWork->m_fRotY);
    const bcs::FVEC3 &rPos = GetPos();
    pWork->m_TargetPos.x += rPos.x;
    pWork->m_TargetPos.z += rPos.z;

    Util_SetJumpParam(GetCcParam_()->m_Unk3C8, GetCcParam_()->m_Unk3CC, GetCcParam_()->m_Unk3D0);
}

int msw::CChrCoreVolley::SubStateAirTrickIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    return SubStateAirTrickDoJumpMove_(pWork) >= 2;
}

void msw::CChrCoreVolley::SubStateAirTrickDoLandStart_(STATE_WORK_AIR_TRICK *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SubStateAirTrickPlayLandMotion_(pWork);
    SubStateAirTrickPlayLandEffect_(pWork);
    pWork->m_nStep = TRICK_STEP_LAND;
}

void msw::CChrCoreVolley::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0x80, 0, GetCcParam_()->m_Unk3BC, 0.0f);
}

void msw::CChrCoreVolley::SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    SE_PlayArtsLandSE();
    Util_SetEffect((SET_EFFECT_TYPE)0);
}

// ---- Arts event ---------------------------------------------------------

float msw::CChrCoreVolley::GetArtsEventRot()
{
    return GetSpikeTargetPosRot_(0);
}

void msw::CChrCoreVolley::DoPlayArtsEvent()
{
    m_fScaleFrom = 1.0f;
    m_fScaleTo = 1.0f;
    m_fScaleRate = 0.0f;
    m_fScaleBase = 1.0f;
    m_fScaleUnk1B0 = 1.0f;
    SetBodyScaleState(0, 0.0f);
    SetCondition(0, 0.0f, 0);
    SetNoPickupCount(0.0f);
    Eff_EraseAll();
    if (GetPosY() >= 0.1f) {
        AnimPlay(3, 0x74, 0, 0.0f, 0.0f);
    } else {
        AnimPlay(3, 0x73, 0, 0.0f, 0.0f);
    }
    Eff_CreateSimple(0xF, GetPos(), GetRotY(), 3, 0, 0);
    PlaySe(0xC06, 1, -1, 0.0f, 1.0f, 0.0f);
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->TrgVolleyArtsAttach(m_nTeamNo, m_nPlayerNo, m_nChrId, 1);
    }
}
