// msw::CChrCoreBoss's side step state and jump shot state. In the original
// these are two files: the side step one ends in its static initialiser
// (0x63E90) and the jump shot one has its own float constants.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_BOSS.hpp"
#include "game/msw/CChrIllusion.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // ---- Side step ------------------------------------------------------

    // After-images while stepping: one every ILLUSION_INTERVAL frames, each
    // fading out in ILLUSION_LIFE frames.
    static const float ILLUSION_INTERVAL = 3;
    static const float ILLUSION_LIFE = 23;

    // The side step's motions: the start, the loop and the end.
    struct SIDESTEP_MOT {
        long nStart;
        long nLoop;
        long nEnd;
    };
    const SIDESTEP_MOT s_SideStepMot = { 6, 7, 8 };

    // (no known user) Set by the file's static initialiser, so its value
    // came from an inline function (guess).
    inline float DegToRad(float fDeg)
    {
        return fDeg * 0.017453292f;
    }
    float s_fSideStepUnkRot = DegToRad(60.0f);
}

// The side step's work in the state queue.
struct msw::CChrCoreBoss::STATE_WORK_SIDESTEP {
    long m_nStep;                         // 0x00: 0 start, 1 loop, 2 end motion
    float m_fFrame;                       // 0x04: frames since the step began
    float m_Unk08;
    float m_fRotY;                        // 0x0C: the direction of the step, set by the caller
    unsigned char m_Unk10[0x18 - 0x10];
    int m_bLate;                          // 0x18: set from frame 10 on
    unsigned char m_Unk1C[0x20 - 0x1C];
    long m_nUnk20;                        // 0x20
    unsigned char m_Unk24[0x60 - 0x24];
    CC_DATA::CHR_PARAM::MOVE_PARAM m_Move; // 0x60
    DIR_MOVE m_DirMove;                   // 0x70
    float m_fMotFrame;                    // 0x80: frames of the current motion
    float m_fIllusionWait;                // 0x84: frames to the next after-image
    float m_fInvincible;                  // 0x88: frames left with status bit 30 set
    const SIDESTEP_MOT *m_pMot;           // 0x8C
};

void msw::CChrCoreBoss::CtorStateSideStep()
{
    STATE_WORK_SIDESTEP *pWork = FsmqGetWork<STATE_WORK_SIDESTEP>();

    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);

    const CHR_CORE_BOSS::CC_PARAM_BOSS *pParam = GetCcParam_();
    pWork->m_nUnk20 = 0;
    pWork->m_pMot = &s_SideStepMot;
    AnimPlay(3, pWork->m_pMot->nStart, 0, 0.0f, 0.0f);
    m_nStatusFlag |= 0x40000000;
    pWork->m_fInvincible = pParam->m_Unk1B0;

    // The move lasts the three motions' frames.
    pWork->m_Move.m_fFrame = 0.0f;
    pWork->m_Move.m_fFrameMax = 0.0f;
    if (pParam->m_Unk1B4 != 0) {
        AnimSetSpeed(3, 1.0f);
        pWork->m_Move.m_fFrameMax += 30.0f;
    } else {
        if (pParam->m_Unk1BC > 0.0f) {
            AnimSetSpeed(3, pParam->m_Unk1BC);
        } else {
            AnimSetSpeed(3, AnimGetFrameNum(3) / pParam->m_Unk1B8);
        }
        pWork->m_Move.m_fFrameMax += pParam->m_Unk1B8;
    }
    if (pParam->m_Unk1C0 != 0) {
        float fEnd = AnimGetResFrameNum(pWork->m_pMot->nEnd);
        float fSpeed = pParam->m_Unk1C8;
        float fFrame = 0.0f;
        if (fSpeed > fFrame) {
            fFrame = fEnd / fSpeed;
        }
        pWork->m_Move.m_fFrameMax += fFrame;
    } else {
        pWork->m_Move.m_fFrameMax += pParam->m_Unk1C4;
    }

    pWork->m_nStep = 0;
    pWork->m_fFrame = 0.0f;
    pWork->m_Move.m_fDist = pParam->m_Unk1AC;
    pWork->m_fMotFrame = 0.0f;
    pWork->m_fIllusionWait = (long)ILLUSION_INTERVAL;
    pWork->m_Move.m_nType = 2;
    pWork->m_bLate = 0;
    Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
    SetMoveMotType(0);
    SetRotY(pWork->m_fRotY);
    PlayVoice(0x5B, 0);
}

void msw::CChrCoreBoss::LoopStateSideStep()
{
    STATE_WORK_SIDESTEP *pWork = FsmqGetWork<STATE_WORK_SIDESTEP>();
    const CHR_CORE_BOSS::CC_PARAM_BOSS *pParam = GetCcParam_();

    pWork->m_fMotFrame += m_fTimeScale * m_fFrameStep;
    pWork->m_fFrame += m_fTimeScale * m_fFrameStep;
    pWork->m_fIllusionWait -= m_fTimeScale * m_fFrameStep;

    if (pWork->m_nStep == 0 && pWork->m_fInvincible > 0.0f) {
        pWork->m_fInvincible -= m_fFrameStep;
        if (pWork->m_fInvincible <= 0.0f) {
            m_nStatusFlag &= ~0x40000000;
        }
    }

    switch (pWork->m_nStep) {
        case 0:
            if ((pParam->m_Unk1B4 != 0 && IsAnimEnd(3)) ||
                (pParam->m_Unk1B4 == 0 && pWork->m_fMotFrame > pParam->m_Unk1B8)) {
                pWork->m_Move.m_fFrame = 0.0f;
                if (pParam->m_Unk1C0 != 0) {
                    AnimPlay(3, pWork->m_pMot->nEnd, 0, 0.0f, 0.0f);
                    pWork->m_nStep = 2;
                } else if (pParam->m_Unk1C8 > 0.0f) {
                    AnimPlay(3, pWork->m_pMot->nEnd, 0, 0.0f, 0.0f);
                    AnimSetSpeed(3, pParam->m_Unk1C8);
                    pWork->m_nStep = 2;
                } else {
                    long nLoop = pWork->m_pMot->nLoop;
                    long nEnd = pWork->m_pMot->nEnd;
                    if (AnimGetResFrameNum(nLoop) >= pParam->m_Unk1C4) {
                        AnimPlay(3, nEnd, 0, 0.0f, 0.0f);
                        AnimSetSpeed(3, AnimGetFrameNum(3) / pParam->m_Unk1C4);
                        pWork->m_nStep = 2;
                    } else {
                        AnimPlay(3, nLoop, 1, 0.0f, 0.0f);
                        pWork->m_nStep = 1;
                    }
                }
                pWork->m_fMotFrame = m_fTimeScale * m_fFrameStep;
                m_nStatusFlag &= ~0x40000000;
                PlayFtSe_(3, 0);
            }
            break;
        case 1: {
            // Loops until the end motion fits in what is left.
            long nEnd = pWork->m_pMot->nEnd;
            float fFrame = pWork->m_fMotFrame;
            float fStart = pParam->m_Unk1C4 - AnimGetResFrameNum(nEnd);
            if (fFrame > fStart) {
                AnimPlay(3, nEnd, 0, 3.0f, fFrame - fStart);
                pWork->m_nStep = 2;
            }
            break;
        }
        case 2:
            if ((pParam->m_Unk1C0 != 0 && IsAnimEnd(3)) ||
                (pParam->m_Unk1C0 == 0 && pWork->m_fMotFrame >= pParam->m_Unk1C4)) {
                FsmqNext();
                FsmqRetry();
                return;
            }
            break;
    }

    float fFrame = pWork->m_fFrame;
    Util_DoDirMove(pWork->m_Move, fFrame, pWork->m_DirMove, 1);
    if ((long)ILLUSION_INTERVAL > 0 && (long)ILLUSION_LIFE > 0 && pWork->m_fIllusionWait <= 0.0f) {
        new (1) CChrIllusion(this, 0.4f, 1.0f / ILLUSION_LIFE, bcs::BCS_Color(0xFFFFFF00), bcs::BCS_Color(0xFFFFFFFF),
                             static_cast<bcs::CScnObj::AlphaMode>(0));
        while (pWork->m_fIllusionWait <= 0.0f) {
            pWork->m_fIllusionWait += (long)ILLUSION_INTERVAL;
        }
    }
    Util_DoJumpMove(m_fFrameStep * m_fTimeScale);
    ClearRevise();
    if (fFrame >= 10.0f) {
        pWork->m_bLate = 1;
    }
}

void msw::CChrCoreBoss::DtorStateSideStep()
{
    m_nStatusFlag &= ~0x40000000;
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
}

// ---- Jump shot ----------------------------------------------------------

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
