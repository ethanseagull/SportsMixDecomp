// msw::CChrCoreBossSiroma (the White Mage) in the boss game.
#include "game/msw/CChrCoreBoss.hpp"

#include "game/msw/CBall.hpp"

#include <math.h>

namespace {

    // (guessed helpers) An angle brought into (-pi, pi], and degrees to
    // radians.
    inline float WrapRadPI(float rad)
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }
}

msw::CChrCoreBossSiroma::CChrCoreBossSiroma()
{
}

msw::CChrCoreBossSiroma::~CChrCoreBossSiroma()
{
}

void msw::CChrCoreBossSiroma::Reset_()
{
    CChrCoreBoss::Reset_();
}

// The diving receive: she does not dive but catches the ball with a spell.
// Step 0 starts it, 1 waits for the motion, 3 waits for the catch to end.
// Once the ball is held from step 3 on, the normal receive state takes over
// with the ball's landing point. Returns not zero when the state is over.
int msw::CChrCoreBossSiroma::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0: {
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
            SetBallCatchEnableRoot_(3.0f, 0.5f, ofs);
            if (pWork->m_fTargetRotY >= 0.0f) {
                float fLimit = 160.0f;
                float fDiff = WrapRadPI(pWork->m_fTargetRotY - pWork->m_fRotY);
                if (fabsf(fDiff) > DegToRad(fLimit)) {
                    pWork->m_fRotY = pWork->m_fTargetRotY;
                }
            }
            Eff_CreateSimple(0x51E, GetPos(), pWork->m_fRotY, 3, 0, 0);
            AnimPlay(3, 0x58, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = 16.0f;
            pWork->m_nStep = 1;
            break;
        }
        case 1:
            if (IsAnimEnd(3)) {
                SetBallCatchDisable_();
                if (pWork->m_nFlag & 1) {
                    AnimPlay(3, 0x59, 0, 8.0f, 0.0f);
                    pWork->m_nStep = 3;
                } else {
                    AnimPlay(3, 0x5C, 0, 8.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                pWork->m_nStep = 4;
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (m_nStatusFlag & 1) {
        if (pWork->m_nStep >= 3) {
            CBall *pBall = GetAttachBall();
            long nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
            STATE_WORK_RECEIVE *pReceive = SetStateReceive_(pBall, 0, nPose);
            float fFrame = pBall->GetTouchDownFrame(m_afReceiveHeight[nPose] + GetPosY());
            pBall->GetPosAfterFrame(pReceive->m_BallPos, fFrame);
            pReceive->m_nFlag |= 0x200;
            return 0;
        }

        long nType = 0;
        if (pWork->m_nFlag & 2) {
            nType = 1;
        }
        CBall *pBall = GetAttachBall();
        if (pBall) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            GetRotY();
            if (nReact & (0x10000 | 0x1000)) {
                nType = 2;
                if (nReact & 0x10000) {
                    nType = 4;
                }
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, pWork->m_nReceiveType);
        }
    }
    return bEnd;
}

void msw::CChrCoreBossSiroma::SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreBossSiroma::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
}

void msw::CChrCoreBossSiroma::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    CChrCoreBoss::SubStateSpikeDoLandStart_(pWork);
}

void msw::CChrCoreBossSiroma::DtorStateSpike()
{
    CChrCoreBoss::DtorStateSpike();
}
