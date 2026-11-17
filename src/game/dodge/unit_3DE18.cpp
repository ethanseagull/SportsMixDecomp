// msw::CChrCoreDodge's pass states: the pass (a turn towards the receiver
// or the place the ball goes, then the throw at the motion's trigger
// frame), the catch of a pass (standing at the catch point, or jumping for
// a high one; a jump catch can turn into a shot at the nearest opponent),
// and the aerial pass state, which in DodgeF waits for the ball like the
// pass catch. The file ends in its static initialiser.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CChrMan.hpp"
#include "nw4r/math/triangular.h"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

namespace {

    // The pass state's work (names guessed).
    struct STATE_WORK_PASS {
        long m_nStep;              // 0x00: 0 and 1 set up, 4 throwing
        bcs::FVEC3 m_Target;       // 0x04: where the ball goes
        long m_nAim;               // 0x10: 1 aims at m_Target (the receiver jumps for it)
        // 0x14: the turn before the throw: 0 not chosen yet, 1 a turn
        // (large angle), 2 a small one, 3 none (no receiver).
        long m_nTurn;
        msw::CChrCore *m_pTo;      // 0x18: the receiver (null for none)
        float m_fRotY;             // 0x1C: the yaw the pass turns to
        float m_fShotFrame;        // 0x20: the motion frame the ball leaves at
        float m_fUnk24;            // 0x24
        float m_fStartRotY;        // 0x28: the turn: the yaw it starts at,
        float m_fEndRotY;          // 0x2C: the yaw it ends at,
        float m_fTurnRate;         // 0x30: how far it is (0 to 1)
        float m_fTurnStep;         // 0x34: and how far it goes each frame
        float m_fUnk38;            // 0x38
        int m_nUnk3C;              // 0x3C: passed on to Util_PassBall
    };

    // The work of the pass catch and the aerial pass (names guessed).
    struct STATE_WORK_PASS_CATCH {
        long m_nStep;              // 0x00
        unsigned long m_hBall;     // 0x04: the task id of the ball
        bcs::FVEC3 m_Pos;          // 0x08: where the ball will be caught
        float m_fJumpWait;         // 0x14: frames before the jump for a high pass
        unsigned char m_Unk18;     // 0x18
        unsigned char m_bNoJump;   // 0x19
        unsigned char m_bRun;      // 0x1A: the run motion is playing
        unsigned char m_bJump;     // 0x1B: the pass is caught in a jump
        unsigned char m_Unk1C[4];  // 0x1C
        int m_bShoot;              // 0x20: a jump catch may turn into a shot
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

    // (guessed helper) An angle brought into (-pi, pi].
    inline float WrapRadPi(float rad)
    {
        rad = WrapRad(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) The yaw the pass turns to, from the angle fDiff
    // between the facing and the receiver: a large angle is cut by 45
    // degrees, a medium one is not turned at all.
    inline float CalcPassRotY(msw::CChrCore *pChr, STATE_WORK_PASS *pWork, float fDiff)
    {
        if (fDiff >= 3.1415927f) {
            fDiff -= 6.2831855f;
        }
        float fAbs = __fabsf(fDiff);
        if (fAbs > 0.7853982f) {
            pWork->m_nTurn = 1;
            if (fDiff < 0.0f) {
                fDiff += 0.7853982f;
            } else {
                fDiff -= 0.7853982f;
            }
        } else if (fAbs > 0.34906584f) {
            pWork->m_nTurn = 1;
            fDiff = 0.0f;
        } else {
            pWork->m_nTurn = 2;
        }
        return WrapRad(fDiff + pChr->GetRotY());
    }
}

// ---- Pass ---------------------------------------------------------------

void msw::CChrCoreDodge::CtorStatePass()
{
    STATE_WORK_PASS *pWork = FsmqGetWork<STATE_WORK_PASS>();
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    m_UnkEC |= 0x10;
    m_pPassCheckChr = pWork->m_pTo;
}

void msw::CChrCoreDodge::LoopStatePass()
{
    const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();
    STATE_WORK_PASS *pWork = FsmqGetWork<STATE_WORK_PASS>();

    if (m_nStatusFlag & 0x400) {
        Util_DoJumpMove(m_fFrameStep);
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(3);
            pWork->m_nStep = 1;
            // fall through
        case 1: {
            CChrCore *pTo = pWork->m_pTo;
            if (pTo) {
                if (pWork->m_nAim == 1) {
                    if (pWork->m_nTurn == 0) {
                        pWork->m_fRotY = CalcPassRotY(this, pWork, GetRotY_Diff_ToPosXZ(pWork->m_Target));
                    } else {
                        pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_Target);
                    }
                } else if (pWork->m_nTurn == 0) {
                    pWork->m_fRotY = CalcPassRotY(this, pWork, GetRotY_Diff_ToChrXZ(pTo));
                } else {
                    pWork->m_fRotY = GetRotY_ToChr(pTo);
                }
            } else {
                if (pWork->m_nAim == 1) {
                    pWork->m_nTurn = 3;
                } else if (pWork->m_nTurn == 0) {
                    pWork->m_nTurn = 1;
                }
                pWork->m_nUnk3C = 0;
            }

            // (the same motion with a turn or without)
            if (pWork->m_nTurn != 3) {
                AnimPlay(3, 0x54, 0, 8.0f, 0.0f);
                pWork->m_fShotFrame = 17.0f;
                float fFrame = AnimSearchTrgFrame(0x54, 2, -1.0f);
                if (fFrame >= 0.0f) {
                    pWork->m_fShotFrame = fFrame;
                }
            } else {
                AnimPlay(3, 0x54, 0, 8.0f, 0.0f);
                pWork->m_fShotFrame = 17.0f;
                float fFrame = AnimSearchTrgFrame(0x54, 2, -1.0f);
                if (fFrame >= 0.0f) {
                    pWork->m_fShotFrame = fFrame;
                }
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
            pWork->m_nStep = 4;
            break;
        }
        default: {
            float fFrame = AnimGetFrame(3);
            if (pWork->m_fTurnRate < 1.0f) {
                // Turn to m_fEndRotY, easing in and out, by the time the
                // ball leaves.
                pWork->m_fTurnRate += pWork->m_fTurnStep * m_fTimeScale * m_fFrameStep;
                pWork->m_fTurnRate = CLAMP(pWork->m_fTurnRate, 0.0f, 1.0f);
                float fDiff = WrapRadPi(pWork->m_fEndRotY - pWork->m_fStartRotY);
                float fRate = pWork->m_fTurnRate;
                float fRot;
                if (fRate <= 0.5f) {
                    float fAngle = 3.1415927f * fRate + 1.5707964f;
                    fRot = 0.5f * fDiff * (1.0f - nw4r::math::SinRad(fAngle));
                } else {
                    fRot = 0.5f * fDiff + 0.5f * fDiff * nw4r::math::SinRad(3.1415927f * (fRate - 0.5f));
                }
                SetRotY(WrapRad(pWork->m_fStartRotY + fRot));
            }

            if (fFrame >= pWork->m_fShotFrame && (m_nStatusFlag & 1)) {
                // The ball leaves.
                long nType = 0;
                SetMoveMotType(0);
                CChrCore *pTo = pWork->m_pTo;
                if (pTo) {
                    if (pWork->m_nAim != 1) {
                        pTo->GetCompPos(pWork->m_Target);
                        pWork->m_Target.y += 1.0f;
                    } else {
                        nType = 1;
                        pTo->MethdSetPassCatchJumpWaitFrame(pWork->m_Target, 60.0f - pParam->m_Unk84);
                    }
                } else {
                    nType = 1;
                }
                int bJump = (m_nStatusFlag >> 10) & 1;
                float fSpeed = pParam->m_Unk3D4;
                if (bJump) {
                    fSpeed = pParam->m_Unk3F8;
                }
                Util_PassBall(nType, pWork->m_pTo, pWork->m_Target, fSpeed, !bJump, pWork->m_nUnk3C);
                SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                // (the same sound and voice in a jump)
                long nSe = 0xC1F;
                long nVoice = 0x56;
                if (m_nStatusFlag & 0x400) {
                    nSe = 0xC1F;
                    nVoice = 0x56;
                }
                PlaySePos(nSe, static_cast<const bcs::FVEC3 *>(0), 1, -1, 0.0f, 1.0f, 0.0f);
                PlayVoice(nVoice, 0);
            }

            if (IsAnimEnd(3) && !(m_nStatusFlag & 0x400)) {
                FsmqDoChain();
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

void msw::CChrCoreDodge::DtorStatePass()
{
    m_pPassCheckChr = 0;
}

void msw::CChrCoreDodge::SendPassCatchFrame_(CChrCore *, const bcs::FVEC3 &, float)
{
}

// ---- Pass catch ---------------------------------------------------------

void msw::CChrCoreDodge::CtorStatePassCatch()
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    if (pWork->m_nStep == 0) {
        m_UnkEC |= 0x10;
        m_nStatusFlag &= ~0x1F000000;
        SetMoveMotType(0);
        pWork->m_bRun = 0;
    }
}

void msw::CChrCoreDodge::LoopStatePassCatch()
{
    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    int bFirst = 0;

    switch (pWork->m_nStep) {
        case 0:
            if (CheckPassCatchEnd_()) {
                m_nStatusFlag &= ~0x1F000000;
                FsmqDoChain();
                break;
            }
            bFirst = 1;
            pWork->m_nStep = 1;
            // fall through
        case 1: {
            int bJump = 0;
            if (!pWork->m_bNoJump && pWork->m_bJump) {
                if (pWork->m_fJumpWait > 0.0f) {
                    pWork->m_fJumpWait -= m_fTimeScale * m_fFrameStep;
                }
                if (pWork->m_fJumpWait <= 0.0f) {
                    bJump = 1;
                }
            }

            if (!bJump) {
                if (CheckPassCatchEnd_()) {
                    // The catch is over. Holding the ball after a jump
                    // catch, it may be shot at once at the nearest opponent.
                    if ((m_nStatusFlag & 1) && pWork->m_bShoot) {
                        CChrCore *pEnemy = CChrMan::Util_SearchNearPosEnemy(GetPlayerNo());
                        if (pEnemy && MethdShoot(GetRotY_ToChr(pEnemy))) {
                            return;
                        }
                    }
                    FsmqDoChain();
                    return;
                }

                if (IsInnerXZ(pWork->m_Pos, 0.1f)) {
                    // At the catch point: wait, facing the ball.
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                    if (pBall) {
                        RotateYToPosRate_(pBall->GetPos(), 0.125f);
                    }
                    if (pWork->m_bRun || bFirst) {
                        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                        SetMoveMotType(0);
                        AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                        pWork->m_bRun = 0;
                    }
                } else {
                    RotateYToPosRate_(pWork->m_Pos, 0.125f);
                }
                break;
            }

            // A high pass: jump for it.
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            m_UnkEC &= ~0x10;
            Util_SetJumpParamDodge(0);
            long nJumpMot = 0x20;
            if (m_nStatusFlag & 3) {
                nJumpMot = 0x21;
            }
            AnimPlay(3, nJumpMot, 0, 4.0f, 0.0f);
            m_nStatusFlag |= 0x400;
            SetMoveMotType(0);
            pWork->m_nStep = 2;
            float fSpeed = 0.5f * GetPrmMoveSpeed();
            bcs::FVEC3 vel;
            bcs::BCS_VecZ_RotY(vel, fSpeed, GetRotY());
            vel.y = m_fJumpVelY;
            GetJumpVel() = vel;
        }
            // fall through
        case 2:
            if (Util_DoJumpMove(1.0f) == 1) {
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                AnimPlay(3, Util_GetLandMotID(-1), 0, -4.0f, 0.0f);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                if (IsLineOver_(1) && MethdLineOver(-1.0f)) {
                    return;
                }
                pWork->m_nStep = 3;
            }
            int bAir = (m_nStatusFlag >> 10) & 1;
            if (bAir && !(m_nStatusFlag & 1)) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                if (pBall) {
                    RotateYToPosRate_(pBall->GetPos(), 0.125f);
                }
            } else if (m_nStatusFlag & 1) {
                if (!bAir) {
                    RotateYToPosRate_(GetTargetGoalPos(), 0.125f);
                } else if (pWork->m_bShoot) {
                    // Caught in the air: turn to the nearest opponent, and
                    // shoot once facing them.
                    CChrCore *pEnemy = CChrMan::Util_SearchNearPosEnemy(GetPlayerNo());
                    if (pEnemy) {
                        float fDiff = GetRotY_Diff_ToChrXZ(pEnemy);
                        if (fDiff > 3.1415927f) {
                            fDiff -= 6.2831855f;
                        }
                        float fRot = fDiff + GetRotY();
                        if (__fabsf(fDiff) < 0.17453292f) {
                            MethdShoot(fRot);
                        } else {
                            RotateYRate_(fRot, 0.2f);
                        }
                    }
                }
            }
            break;
        default:
            Util_DoJumpMoveLand(1.0f);
            if (IsAnimEnd(3)) {
                long nNext = FsmqGetNextState();
                m_nStatusFlag &= ~0xC00;
                if (nNext == 0 && (m_nStatusFlag & 1)) {
                    Util_PushBackCatch(1);
                }
                FsmqNext();
                return;
            }
            break;
    }

    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

void msw::CChrCoreDodge::DtorStatePassCatch()
{
    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

// ---- Aerial pass --------------------------------------------------------

void msw::CChrCoreDodge::CtorStateAerialPass()
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    if (pWork->m_nStep == 0) {
        m_UnkEC |= 0x10;
        m_nStatusFlag &= ~0x1F000000;
        SetMoveMotType(0);
        pWork->m_bRun = 0;
    }
}

void msw::CChrCoreDodge::LoopStateAerialPass()
{
    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    int bFirst = 0;

    switch (pWork->m_nStep) {
        case 0:
            if (CheckPassCatchEnd_()) {
                m_nStatusFlag &= ~0x1F000000;
                FsmqDoChain();
                break;
            }
            bFirst = 1;
            pWork->m_nStep = 1;
            // fall through
        case 1: {
            int bJump = 0;
            if (!pWork->m_bNoJump && pWork->m_bJump) {
                if (pWork->m_fJumpWait > 0.0f) {
                    pWork->m_fJumpWait -= m_fTimeScale * m_fFrameStep;
                }
                if (pWork->m_fJumpWait <= 0.0f) {
                    bJump = 1;
                }
            }

            if (!bJump) {
                if (CheckPassCatchEnd_()) {
                    FsmqDoChain();
                    return;
                }

                if (IsInnerXZ(pWork->m_Pos, 0.1f)) {
                    // At the catch point: wait, facing the ball.
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                    if (pBall) {
                        RotateYToPosRate_(pBall->GetPos(), 0.125f);
                    }
                    if (pWork->m_bRun || bFirst) {
                        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                        SetMoveMotType(0);
                        AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                        pWork->m_bRun = 0;
                    }
                } else {
                    RotateYToPosRate_(pWork->m_Pos, 0.125f);
                }
                break;
            }

            // A high pass: jump for it.
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            m_UnkEC &= ~0x10;
            Util_SetJumpParamDodge(0);
            long nJumpMot = 0x20;
            if (m_nStatusFlag & 3) {
                nJumpMot = 0x21;
            }
            AnimPlay(3, nJumpMot, 0, 4.0f, 0.0f);
            m_nStatusFlag |= 0x400;
            SetMoveMotType(0);
            pWork->m_nStep = 2;
            float fSpeed = 0.5f * GetPrmMoveSpeed();
            bcs::FVEC3 vel;
            bcs::BCS_VecZ_RotY(vel, fSpeed, GetRotY());
            vel.y = m_fJumpVelY;
            GetJumpVel() = vel;
        }
            // fall through
        case 2:
            if (Util_DoJumpMove(1.0f) == 1) {
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                AnimPlay(3, Util_GetLandMotID(-1), 0, -4.0f, 0.0f);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                pWork->m_nStep = 3;
            }
            int bAir = (m_nStatusFlag >> 10) & 1;
            if (bAir && !(m_nStatusFlag & 1)) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                if (pBall) {
                    RotateYToPosRate_(pBall->GetPos(), 0.125f);
                }
            } else if (m_nStatusFlag & 1) {
                if (!bAir) {
                    RotateYToPosRate_(GetTargetGoalPos(), 0.125f);
                } else if (pWork->m_bShoot) {
                    CChrCore *pEnemy = CChrMan::Util_SearchNearPosEnemy(GetPlayerNo());
                    if (pEnemy) {
                        float fDiff = GetRotY_Diff_ToChrXZ(pEnemy);
                        if (fDiff > 3.1415927f) {
                            fDiff -= 6.2831855f;
                        }
                        float fRot = fDiff + GetRotY();
                        if (__fabsf(fDiff) < 0.17453292f) {
                            MethdShoot(fRot);
                        } else {
                            RotateYRate_(fRot, 0.2f);
                        }
                    }
                }
            }
            break;
        default:
            Util_DoJumpMoveLand(1.0f);
            if (IsAnimEnd(3)) {
                long nNext = FsmqGetNextState();
                m_nStatusFlag &= ~0xC00;
                if (nNext == 0 && (m_nStatusFlag & 1)) {
                    Util_PushBackCatch(1);
                }
                FsmqNext();
                return;
            }
            break;
    }

    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

void msw::CChrCoreDodge::DtorStateAerialPass()
{
}
