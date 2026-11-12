// msw::CChrCoreBoss's jump shot state. A file of its own (split from the side
// step's at 0x63EA4): its float constants restart after the side step's
// static initialiser, and it addresses every one with its own lis.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "nw4r/math/triangular.h"

namespace unit_63EA4_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float constant of this
// file with its own lis, which this compiler does when the first .rodata object of the file is
// weak (a function-local const). The module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// The jump shot's work in the state queue.
struct msw::CChrCoreBoss::STATE_WORK_JUMP_SHOOT {
    long m_nStep;         // 0x00: 0 until the shot, then 1
    float m_fTargetRotY;  // 0x04: the yaw to the target
    float m_fStartRotY;   // 0x08: the yaw when the state began
    float m_fShotFrame;   // 0x0C: the motion frame of the shot
};

void msw::CChrCoreBoss::CtorStateJumpShoot()
{
    STATE_WORK_JUMP_SHOOT *pWork = FsmqGetWork<STATE_WORK_JUMP_SHOOT>();

    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    AnimPlay(3, 0x1F, 0, 8.0f, 6.0f);

    pWork->m_fShotFrame = GetBallShotTrgFrame_(0x1F, 15.0f);
    pWork->m_fShotFrame = pWork->m_fShotFrame > 15.0f ? 15.0f : pWork->m_fShotFrame;
    pWork->m_fStartRotY = GetRotY();
    pWork->m_fTargetRotY = GetSpikeTargetPosRot_(0);
    pWork->m_nStep = 0;
}

void msw::CChrCoreBoss::LoopStateJumpShoot()
{
    STATE_WORK_JUMP_SHOOT *pWork = FsmqGetWork<STATE_WORK_JUMP_SHOOT>();

    if (pWork->m_nStep == 0) {
        float fShotFrame;
        if (IsAnimEnd(3) || AnimGetFrame(3) >= (fShotFrame = pWork->m_fShotFrame)) {
            // Shoot: the puck flies at the target, three units up.
            bcs::FVEC3 dir;
            bcs::FVEC3 target;
            pWork->m_fTargetRotY = GetSpikeTargetPosRot_(&target);
            target.y = 3.0f;
            bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fTargetRotY);
            SetRotY(pWork->m_fTargetRotY);
            CBall *pBall = GetAttachBall();
            if (pBall != 0) {
                DetachBall();
                pBall->SetVsBossShootMode(target, GetPlayerNo(), GetCcParam_()->m_Unk230);
                float fArtsShots = 25.0f;
                if (fArtsShots > 0.0f) {
                    AddArtsGauge_(1.0f / fArtsShots, -1.0f);
                }
            }
            pWork->m_nStep++;
            return;
        }

        // Turn to the target over the motion up to the shot, easing out.
        float fDiff = pWork->m_fTargetRotY - pWork->m_fStartRotY;
        while (fDiff > 6.2831855f) {
            fDiff -= 6.2831855f;
        }
        while (fDiff < 0.0f) {
            fDiff += 6.2831855f;
        }
        float fFrame = AnimGetFrame(3);
        float fRate;
        if (fShotFrame <= 0.0f) {
            fRate = 1.0f;
        } else {
            float fRatio = fFrame / fShotFrame;
            fRate = fRatio > 1.0f ? 1.0f : (fRatio < 0.0f ? 0.0f : fRatio);
        }
        if (fDiff > 3.1415927f) {
            fDiff -= 6.2831855f;
        }
        float fTurn = fDiff * nw4r::math::SinRad(1.5707964f * fRate);
        float fRotY = fTurn + pWork->m_fStartRotY;
        while (fRotY > 6.2831855f) {
            fRotY -= 6.2831855f;
        }
        while (fRotY < 0.0f) {
            fRotY += 6.2831855f;
        }
        SetRotY(fRotY);
        return;
    }

    if (IsAnimEnd(3)) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreBoss::DtorStateJumpShoot()
{
}
