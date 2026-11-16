// msw::CChrCoreBasket's pass states: the pass on the ground, the aerial
// pass (a pass from the air, which turns into a pass on landing when the
// ball has not left yet), and the jump pass (the pass to a partner who
// takes it in an alley-oop). CheckPassType_ picks the kind of pass and the
// motion. The file ends in its static initialiser.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcGhostBasketBase.hpp"
#include "nw4r/math/triangular.h"
#include "nw4r/math/types.h"

// 70, 45 and 135 degrees in radians. The first is not used here: nearly
// every BasketF file has it and the static initialiser that fills it (see
// CBallManBasket.cpp). The other two split the aerial pass into its
// forward and sideways throws.
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);
static const float s_Deg45 = bcs::DEG_TO_RAD(45.0f);
static const float s_Deg135 = bcs::DEG_TO_RAD(135.0f);

namespace {

    // bcs::FVEC3 derives from nw4r::math::VEC3 in the original, and the
    // vector sums go through VEC3's operators; here FVEC3 is a separate
    // struct of the same layout, so the operands are reinterpreted.
    typedef const nw4r::math::VEC3 &VEC3_CREF;
    typedef nw4r::math::VEC3 &VEC3_REF;

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

    // (guessed helper) An angle brought into (-pi, pi].
    inline float WrapRadPi(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }
}

// The work of the pass and the aerial pass (names guessed; filled by
// MethdPass_ and MethdAerialPass_).
struct msw::CChrCoreBasket::STATE_WORK_PASS {
    long m_nStep;              // 0x00: 0 and 1 set up, 5 (pass) or 1, 5, 2 (aerial) after
    long m_nUnk04;             // 0x04: 1 aims at m_Target (passed on to the receiver)
    long m_nPassType;          // 0x08: CheckPassType_'s kind of pass
    long m_nUnk0C;             // 0x0C
    CChrCoreBasket *m_pTo;     // 0x10: the receiver (null for none)
    CNpcGhostBasketBase *m_pGhost; // 0x14: or the ghost
    float m_fRotY;             // 0x18: the yaw the pass turns to
    float m_fShotFrame;        // 0x1C: the motion frame the ball leaves at
    float m_fDirRotY;          // 0x20: the yaw asked for (a pass to nobody)
    float m_fSplineRate;       // 0x24: the battle pass's move (SubBSplineMove_)
    int m_bBattle;             // 0x28: passed on to Util_PassBall
    int m_nUnk2C;              // 0x2C: passed on to the receiver and Util_PassBall
    bcs::FVEC3 m_Target;       // 0x30: where the ball goes
    float m_fStartRotY;        // 0x3C: the turn to m_fRotY: the yaw it starts at,
    float m_fEndRotY;          // 0x40: the yaw it ends at,
    float m_fTurnRate;         // 0x44: how far it is (0 to 1)
    float m_fTurnStep;         // 0x48: and how far it goes each frame
};

// The work of the jump pass (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_JUMP_PASS {
    long m_nStep;              // 0x00: 0 rising, 1 after the top
    float m_fShotFrame;        // 0x04: the motion frame the ball leaves at
    CChrCoreBasket *m_pTo;     // 0x08: the receiver
};

// ---- Pass ---------------------------------------------------------------

void msw::CChrCoreBasket::CtorStatePass()
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    m_UnkEC |= 0x10;
}

// The kind of pass from here to rPos (pTo: the receiver, if any); -1
// without the ball. Battle passes (4) go to a receiver about in front
// while an opponent who blocks stands near.
long msw::CChrCoreBasket::CheckPassType_(const bcs::FVEC3 &rPos, const CChrCore *pTo)
{
    if (!IsHoldBall()) {
        return -1;
    }

    CChrCore *pChr;
    long nType = 5;
    if (pTo) {
        if (pTo->GetState() == STATE_ALLEYOOP) {
            nType = 2;
        } else {
            bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(rPos) - reinterpret_cast<VEC3_CREF>(GetPos());
            if (bcs::BCS_Vec_GetLenXZ(dist) > 9.0f || pTo->GetState() == STATE_ALLEYOOP) {
                nType = 5;
            } else if (IsBattle() == 1) {
                float fMinDist = 9999.9f;
                nType = 6;
                CChrCore *pNear = 0;
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    pChr = CChrMan::GetChrCore(i);
                    if (this != pChr && !pChr->IsDisable() && GetTeamNo() != pChr->GetTeamNo() && pChr->IsNowBlockWork()) {
                        bcs::FVEC3 d = reinterpret_cast<VEC3_CREF>(GetPos()) - reinterpret_cast<VEC3_CREF>(pChr->GetPos());
                        float fDist = bcs::BCS_Vec_GetLenXZ(d);
                        if (fDist < fMinDist) {
                            fMinDist = fDist;
                            pNear = pChr;
                        }
                    }
                }
                if (pNear) {
                    float fRot = WrapRadPi(GetRotY_ToPosXZ(pNear->GetPos()) - GetRotY());
                    if (__fabsf(fRot) <= 1.5707964f) {
                        nType = 4;
                    }
                }
            } else {
                nType = 3;
            }
        }
    }
    return nType;
}

void msw::CChrCoreBasket::LoopStatePass()
{
    STATE_WORK_PASS *pWork = FsmqGetWork<STATE_WORK_PASS>();
    // (always 0 here; the plain pass motion would be mirrored with them)
    int bNoFlip = 0;
    int bFlip = 0;

    switch (pWork->m_nStep) {
        case 0:
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(3);
            if (pWork->m_pTo) {
                pWork->m_Target = pWork->m_pTo->GetPos();
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    const bcs::FVEC3 &rToPos = pWork->m_pTo->GetPos();
                    long nType = CheckPassType_(rToPos, pWork->m_pTo);
                    pWork->m_pTo->GetFuturePos(pWork->m_Target,
                                               pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, nType));
                    pWork->m_nUnk04 = 1;
                }
                pWork->m_pTo->DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x24), m_nPlayerNo, 0, 0, 0);
            } else if (pWork->m_pGhost) {
                pWork->m_pGhost->GetVNodePos(1, pWork->m_Target);
            } else {
                bcs::BCS_VecZ_RotY(pWork->m_Target, 5.0f, pWork->m_fDirRotY);
                reinterpret_cast<VEC3_REF>(pWork->m_Target) += reinterpret_cast<VEC3_CREF>(GetPos());
            }
            pWork->m_nStep = 1;
            // fall through
        case 1:
            if (pWork->m_pTo || pWork->m_pGhost) {
                pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_Target);
            } else {
                pWork->m_nUnk2C = 0;
            }
            pWork->m_nPassType = CheckPassType_(pWork->m_Target, pWork->m_pTo);
            switch (pWork->m_nPassType) {
                default:
                    if (!bNoFlip && bFlip) {
                        AnimPlay(3, -0x72, 0, 8.0f, 0.0f);
                    } else {
                        AnimPlay(3, 0x72, 0, 8.0f, 0.0f);
                    }
                    pWork->m_fShotFrame = 8.0f;
                    bFlip = 1;
                    break;
                case 4:
                case 6:
                    AnimPlay(3, IsHaveLeft() == 1 ? -0x4A : 0x4A, 0, 8.0f, 0.0f);
                    pWork->m_fShotFrame = 0.0f;
                    pWork->m_fSplineRate = 0.0f;
                    break;
                case 5:
                case 2:
                    AnimPlay(3, IsHaveLeft() == 1 ? -0x73 : 0x73, 0, 8.0f, 0.0f);
                    pWork->m_fShotFrame = 16.0f;
                    break;
            }
            ReattachBall(5, 0.0f, 0);
            pWork->m_fStartRotY = GetRotY();
            pWork->m_fEndRotY = pWork->m_fRotY;
            pWork->m_fTurnRate = 0.0f;
            if (pWork->m_fShotFrame > 0.0f) {
                pWork->m_fTurnStep = 1.0f / pWork->m_fShotFrame;
            } else {
                pWork->m_fTurnStep = 1.0f;
            }
            pWork->m_nStep = 5;
            break;
        default: {
            float fFrame = AnimGetFrame(3);
            // Turn to the target until the ball leaves, easing in and out.
            if (pWork->m_fTurnRate < 1.0f) {
                pWork->m_fTurnRate += pWork->m_fTurnStep * m_fTimeScale * m_fFrameStep;
                pWork->m_fTurnRate = pWork->m_fTurnRate > 1.0f ? 1.0f : (pWork->m_fTurnRate < 0.0f ? 0.0f : pWork->m_fTurnRate);
                float fDiff = WrapRadPi(pWork->m_fEndRotY - pWork->m_fStartRotY);
                float fTurn;
                if (pWork->m_fTurnRate <= 0.5f) {
                    fTurn = 0.5f * fDiff * (1.0f - nw4r::math::SinRad(3.1415927f * pWork->m_fTurnRate + 1.5707964f));
                } else {
                    fTurn = 0.5f * fDiff + 0.5f * fDiff * nw4r::math::SinRad(3.1415927f * (pWork->m_fTurnRate - 0.5f));
                }
                SetRotY(WrapRad(pWork->m_fStartRotY + fTurn));
            }

            if (IsHoldBall() && fFrame >= pWork->m_fShotFrame) {
                PlayVoice(0x1B, 0);
                float fSpeed = 0.0f;
                SetMoveMotType(0);
                if (pWork->m_pTo) {
                    CBall *pBall = GetAttachBall();
                    if (pBall) {
                        // Where the receiver will be when the ball gets
                        // there, twice over.
                        pWork->m_pTo->GetFuturePos(pWork->m_Target,
                                                   pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, pWork->m_nPassType));
                        pWork->m_pTo->GetFuturePos(pWork->m_Target,
                                                   pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, pWork->m_nPassType));
                    }
                    pWork->m_pTo->MethdSetPassCatchState(GetAttachBall(), pWork->m_nUnk2C, pWork->m_Target, pWork->m_nUnk04);
                }
                if (pWork->m_pGhost) {
                    pWork->m_pGhost->GetVNodePos(1, pWork->m_Target);
                    pWork->m_pGhost->MethdSetPassCatchState(GetAttachBall(), pWork->m_Target, pWork->m_nUnk04);
                }
                Util_PassBall(pWork->m_nPassType, pWork->m_pTo, pWork->m_Target, fSpeed, pWork->m_bBattle,
                              pWork->m_nUnk2C);
                SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                static_cast<CChrManBasket *>(CChrMan::GetInstance())->OnChgCharacterPassDisableFrame_(this);
            }

            if (!IsHoldBall()) {
                if (AnimGetMotID(3, 0) == 0x4A) {
                    bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
                    SubBSplineMove_(fFrame - pWork->m_fShotFrame, AnimGetFrameNum(3) - pWork->m_fShotFrame,
                                    pWork->m_fSplineRate, 0.8f, GetRotY(), 1.0f, ofs);
                }
                if (pWork->m_nPassType == 5) {
                    if (IsAnimEnd(3)) {
                        FsmqDoChain();
                        return;
                    }
                } else if (AnimGetFrame(3) >= 0.5f * AnimGetFrameNum(3)) {
                    FsmqDoChain();
                    return;
                }
            }
            break;
        }
    }

    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

void msw::CChrCoreBasket::DtorStatePass()
{
    m_nBasketFlag &= ~0x100000;
}

// Tells the receiver how long the ball takes to rPos; without one, the
// team colour blinks when the pad may change to the receiver.
void msw::CChrCoreBasket::SendPassCatchFrame_(CChrCore *pTo, const bcs::FVEC3 &rPos, float fFrame)
{
    if (pTo) {
        pTo->MethdSetPassCatchJumpWaitFrame(rPos, fFrame);
    } else if (CGame::IsPlayTypeCanChangeLeader(m_nTeamNo) && CGame::g_GameWork.GetGameType() != 2) {
        SetTeamColorBlinkCount(96.0f);
    }
}

// ---- Aerial pass --------------------------------------------------------

void msw::CChrCoreBasket::CtorStateAerialPass()
{
    SetRiverFallEnable(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(4);
    m_UnkEC &= ~0x10;
}

void msw::CChrCoreBasket::LoopStateAerialPass()
{
    STATE_WORK_PASS *pWork = FsmqGetWork<STATE_WORK_PASS>();
    m_fJumpVelX *= 0.98f;
    m_fJumpVelZ *= 0.98f;
    int bLeft = 0;
    int bLand = 0;

    switch (pWork->m_nStep) {
        case 0:
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(3);
            if (pWork->m_pTo) {
                pWork->m_Target = pWork->m_pTo->GetPos();
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    pWork->m_pTo->GetFuturePos(pWork->m_Target, pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, 0));
                }
            } else if (pWork->m_pGhost) {
                pWork->m_pGhost->GetVNodePos(1, pWork->m_Target);
            } else {
                bcs::BCS_VecZ_RotY(pWork->m_Target, 5.0f, pWork->m_fDirRotY);
                reinterpret_cast<VEC3_REF>(pWork->m_Target) += reinterpret_cast<VEC3_CREF>(GetPos());
            }
            pWork->m_nStep = 1;
            // fall through
        case 1: {
            int nJump = Util_DoJumpMove(1.0f);
            if (nJump == 1) {
                pWork->m_nStep = 3;
                bLand = 1;
                break;
            }
            if (nJump == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }

            long nSide = 0;
            float fRot;
            if (!pWork->m_pTo && !pWork->m_pGhost) {
                fRot = WrapRad(pWork->m_fDirRotY - GetRotY());
            } else if (pWork->m_nUnk04 == 1) {
                fRot = GetRotY_Diff_ToPosXZ(pWork->m_Target);
            } else if (pWork->m_pTo) {
                fRot = GetRotY_Diff_ToChrXZ(pWork->m_pTo);
            } else if (pWork->m_pGhost) {
                fRot = GetRotY_ToPosXZ(pWork->m_pGhost->GetPos());
                fRot = WrapRad(fRot - GetRotY());
            }
            if (fRot >= 3.1415927f) {
                fRot -= 6.2831855f;
            }
            if (fRot >= 0.0f) {
                bLeft = 1;
            }

            // Forward within 45 degrees: a throw straight at the target;
            // otherwise a throw to the side or behind.
            pWork->m_fRotY = GetRotY();
            float fAbs = __fabsf(fRot);
            if (fAbs < s_Deg45) {
                pWork->m_nPassType = 2;
                pWork->m_fRotY = WrapRad(pWork->m_fRotY + fRot);
            } else if (fAbs < s_Deg135) {
                pWork->m_nPassType = 1;
                nSide = 1;
            } else {
                pWork->m_nPassType = 1;
                nSide = 2;
            }

            pWork->m_fShotFrame = 15.0f;
            long nMot = 0x74;
            switch (nSide) {
                case 1:
                    pWork->m_fShotFrame = 15.0f;
                    nMot = 0x76;
                    break;
                case 2:
                    nMot = 0x75;
                    pWork->m_fShotFrame = 15.0f;
                    break;
            }
            if (bLeft) {
                nMot = -nMot;
            }
            AnimPlay(3, nMot, 0, 4.0f, 0.0f);
            ReattachBall(5, 0.0f, 0);
            pWork->m_fStartRotY = GetRotY();
            pWork->m_fEndRotY = pWork->m_fRotY;
            pWork->m_fTurnRate = 0.0f;
            if (pWork->m_fShotFrame > 0.0f) {
                pWork->m_fTurnStep = 1.0f / pWork->m_fShotFrame;
            } else {
                pWork->m_fTurnStep = 1.0f;
            }
            pWork->m_nStep = 5;
            break;
        }
        case 5: {
            float fFrame = AnimGetFrame(2);
            if (pWork->m_fTurnRate < 1.0f) {
                pWork->m_fTurnRate += pWork->m_fTurnStep * m_fTimeScale * m_fFrameStep;
                pWork->m_fTurnRate = pWork->m_fTurnRate > 1.0f ? 1.0f : (pWork->m_fTurnRate < 0.0f ? 0.0f : pWork->m_fTurnRate);
                float fDiff = WrapRadPi(pWork->m_fEndRotY - pWork->m_fStartRotY);
                float fTurn;
                if (pWork->m_fTurnRate <= 0.5f) {
                    fTurn = 0.5f * fDiff * (1.0f - nw4r::math::SinRad(3.1415927f * pWork->m_fTurnRate + 1.5707964f));
                } else {
                    fTurn = 0.5f * fDiff + 0.5f * fDiff * nw4r::math::SinRad(3.1415927f * (pWork->m_fTurnRate - 0.5f));
                }
                SetRotY(WrapRad(pWork->m_fStartRotY + fTurn));
            }

            if (fFrame >= pWork->m_fShotFrame && (m_nStatusFlag & 1)) {
                PlayVoice(0x1B, 0);
                SetMoveMotType(0);
                long nType;
                if (pWork->m_pTo) {
                    if (pWork->m_pTo->GetState() == STATE_ALLEYOOP) {
                        nType = 2;
                    } else {
                        nType = 1;
                    }
                } else {
                    pWork->m_nUnk2C = 0;
                    nType = 1;
                }
                float fSpeed = 0.0f;
                if (pWork->m_pTo) {
                    CBall *pBall = GetAttachBall();
                    if (pBall) {
                        pWork->m_pTo->GetFuturePos(pWork->m_Target, pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, nType));
                        pWork->m_pTo->GetFuturePos(pWork->m_Target, pBall->GetPassFlyFrame(GetPos(), pWork->m_Target, nType));
                    }
                    pWork->m_pTo->MethdSetPassCatchState(GetAttachBall(), pWork->m_nUnk2C, pWork->m_Target, nType);
                }
                if (pWork->m_pGhost) {
                    pWork->m_pGhost->GetVNodePos(1, pWork->m_Target);
                    pWork->m_pGhost->MethdSetPassCatchState(GetAttachBall(), pWork->m_Target, nType);
                }
                Util_PassBall(nType, pWork->m_pTo, pWork->m_Target, fSpeed, pWork->m_bBattle, pWork->m_nUnk2C);
                static_cast<CChrManBasket *>(CChrMan::GetInstance())->OnChgCharacterPassDisableFrame_(this);
                SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                m_nStatusFlag |= 0x1000;
                m_nStatusFlag &= ~0x400;
                m_fJumpUnk224 = 0.0f;
                m_fJumpAccelY = -0.025f;
                pWork->m_nStep = 2;
            }

            int nJump = Util_DoJumpMove(1.0f);
            if (nJump == 1) {
                bLand = 1;
                m_UnkEC |= 0x10;
                pWork->m_nStep = 3;
                break;
            }
            if (nJump == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            if (IsAnimEnd(3)) {
                if (m_fJumpAccelY >= 0.0f) {
                    m_fJumpAccelY = -0.025f;
                }
                pWork->m_nStep = 2;
            }
            break;
        }
        case 2: {
            int nJump = Util_DoJumpMove(1.0f);
            if (nJump == 1) {
                bLand = 1;
                m_UnkEC |= 0x10;
                pWork->m_nStep = 3;
                break;
            }
            if (nJump == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            break;
        }
        default: {
            // Landed: wait for the landing motion (with the ball, to half
            // of it, after which the pass is made on the ground).
            SetRiverFallEnable(0);
            Util_DoJumpMoveLand(1.0f);
            int bWait = !IsAnimEnd(3);
            if (bWait && pWork->m_nStep == 3) {
                SetRiverFallEnable(0);
                if (GetFlag(1)) {
                    float fFrame = AnimGetFrame(3);
                    if (fFrame >= 0.5f * AnimGetFrameNum(3) - 1.0f) {
                        bWait = 0;
                    }
                }
            }
            if (!bWait) {
                long nNext = FsmqGetNextState();
                m_nStatusFlag &= ~0xC00;
                if ((nNext == STATE_DEFAULT || nNext == STATE_CATCH) && GetFlag(1)) {
                    if (nNext == STATE_CATCH) {
                        FsmqCutNext();
                    }
                    STATE_WORK_PASS *pPass = static_cast<STATE_WORK_PASS *>(FsmqPushState(STATE_PASS));
                    pPass->m_nStep = 0;
                    pPass->m_pTo = pWork->m_pTo;
                    pPass->m_nUnk04 = pWork->m_nUnk04;
                    pPass->m_nPassType = 0;
                    pPass->m_fRotY = pWork->m_fRotY;
                    pPass->m_nUnk2C = pWork->m_nUnk2C;
                    if (!pWork->m_pTo) {
                        bcs::BCS_VecZ_RotY(pWork->m_Target, 5.0f, pWork->m_fDirRotY);
                        reinterpret_cast<VEC3_REF>(pWork->m_Target) += reinterpret_cast<VEC3_CREF>(GetPos());
                        pPass->m_fRotY = pWork->m_fDirRotY;
                        pPass->m_fDirRotY = pWork->m_fDirRotY;
                    }
                    pPass->m_Target = pWork->m_Target;
                }
                FsmqNext();
                return;
            }
            break;
        }
    }

    if (bLand) {
        AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -4 : 4, 0, -8.0f, 0.0f);
        Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
    }
    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

void msw::CChrCoreBasket::DtorStateAerialPass()
{
    m_nStatusFlag &= ~0x1C00;
    SetRiverFallEnable(0);
    m_nBasketFlag &= ~0x100000;
}

// ---- Jump pass ----------------------------------------------------------

void msw::CChrCoreBasket::CtorStateJumpPass()
{
    STATE_WORK_JUMP_PASS *pWork = FsmqGetWork<STATE_WORK_JUMP_PASS>();
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    m_nStatusFlag |= 0x400;
    Util_SetJumpParam(20.0f, 0.0f, 2.4f);
    AnimPlay(3, IsHaveLeft() == 1 ? -0x73 : 0x73, 0, 8.0f, 0.0f);
    pWork->m_fShotFrame = 16.0f;
    pWork->m_nStep = 0;
}

void msw::CChrCoreBasket::LoopStateJumpPass()
{
    // (not read: settings of the jump pass left in the source)
    static const float fMaxHeight = 2.4f;
    static const int bNoJumpCatch = 1;
    static const int bNoCurvePass = 0;

    STATE_WORK_JUMP_PASS *pWork = FsmqGetWork<STATE_WORK_JUMP_PASS>();
    int bEnd = 0;
    float fStep = m_fFrameStep * m_fTimeScale;

    switch (pWork->m_nStep) {
        case 0:
            // Hang at the top of the jump for a while.
            if (m_fJumpVelY <= 0.0f) {
                if (m_fJumpUnk224 > 0.0f) {
                    m_fJumpVelY = 0.0f;
                    m_fJumpUnk224 -= fStep;
                    m_fJumpAccelY = 0.0f;
                } else {
                    pWork->m_nStep = 1;
                    m_fJumpAccelY = -0.025f;
                }
            }
            // fall through
        case 1: {
            if (IsHoldBall() && AnimGetFrame(3) >= pWork->m_fShotFrame) {
                long nType = 0;
                SetMoveMotType(0);
                float fSpeed = 0.0f;
                bcs::FVEC3 target(fSpeed, fSpeed, fSpeed);
                CChrCoreBasket *pTo = pWork->m_pTo;
                if (pTo && pTo->GetState() == STATE_ALLEYOOP) {
                    // To the alley-oop: ahead of the receiver, faster the
                    // farther away he is.
                    pTo->GetPos(target);
                    if (GetAttachBall()) {
                        float fRange = 2.0f * GetCourtDepth();
                        bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(GetPos()) - reinterpret_cast<VEC3_CREF>(pWork->m_pTo->GetPos());
                        float fDist = bcs::BCS_Vec_GetLenXZ(dist);
                        float fRate = fDist / fRange;
                        fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                        fSpeed = 120.0f * fRate;
                        if (fSpeed < 22.0f) {
                            fSpeed = 22.0f;
                        }
                        CChrCoreBasket *pChr = pWork->m_pTo;
                        nw4r::math::VEC3 move = reinterpret_cast<VEC3_CREF>(pChr->GetPos()) - reinterpret_cast<VEC3_CREF>(pChr->GetPrevPos());
                        bcs::FVEC3 lead(0.0f, move.y, 0.0f);
                        const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam =
                            pWork->m_pTo->GetCcParam2_();
                        nType = 2;
                        target.x += lead.x * fSpeed;
                        target.z += lead.z * fSpeed;
                        target.y = 0.5f + pParam->m_fAlleyoopHeight;
                        if (fDist > fRange) {
                            fSpeed = 120.0f;
                        }
                    }
                }
                Util_PassBall(nType, pWork->m_pTo, target, fSpeed, 0, 1);
                SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            }
            RotateYRate_(GetRotY_ToPosXZ(pWork->m_pTo->GetPos()), 0.4f);

            bcs::FVEC3 pos = GetPos();
            float fAccel = m_fJumpAccelY * fStep;
            nw4r::math::VEC3 move = reinterpret_cast<VEC3_CREF>(m_fJumpVelX) * fStep;
            m_fJumpVelY += fAccel;
            move.y += 0.5f * (fAccel * fStep);
            reinterpret_cast<VEC3_REF>(pos) += move;
            ClearRevise();
            if (pos.y <= 0.0f && m_fJumpVelY < 0.0f) {
                pos.y = 0.0f;
                bEnd = 1;
                m_fJumpVelY = 0.0f;
                m_fJumpAccelY = -0.025f;
                m_UnkEC |= 0x10;
                m_nStatusFlag &= ~0x400;
            }
            SetPos(pos);
            break;
        }
    }

    if (bEnd == 1) {
        m_nStatusFlag &= ~0x800;
        FsmqDoChain();
    }
}

void msw::CChrCoreBasket::DtorStateJumpPass()
{
    m_nStatusFlag &= ~0x1C00;
}
