// msw::CChrCoreBossYoshi: Yoshi in the boss game. His diving receive is his
// own: he catches the ball with his tongue.
#include "game/msw/CChrCoreBoss.hpp"

#include "game/msw/CBall.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi), and into (-pi, pi].
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }
}

msw::CChrCoreBossYoshi::CChrCoreBossYoshi()
{
}

msw::CChrCoreBossYoshi::~CChrCoreBossYoshi()
{
}

void msw::CChrCoreBossYoshi::Reset_()
{
    CChrCoreBoss::Reset_();
}

// The diving receive, step by step: 0 starts the dive, 1 waits for the ball
// on the tongue until the motion ends, 3 (caught) and 2 (missed) play two
// more motions each, 4 waits for the last one. Returns not zero when the
// state is over. While the ball sits on the tongue it is received from
// there, or flicked away, which ends the state with a damage reaction.
int msw::CChrCoreBossYoshi::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            if (pWork->m_fTargetRotY >= 0.0f) {
                float fLimit = 160.0f;
                float fDiff = WrapRadPI(pWork->m_fTargetRotY - pWork->m_fRotY);
                if (fabsf(fDiff) > DegToRad(fLimit)) {
                    pWork->m_fRotY = pWork->m_fTargetRotY;
                }
            }
            AnimPlay(3, 0x58, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = 16.0f;
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_nStep = 1;
            break;
        case 1:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimEnd(3) || (pWork->m_nFlag & 1)) {
                SetBallCatchDisable_();
                if (pWork->m_nFlag & 1) {
                    AnimPlay(3, 0x59, 0, 8.0f, 0.0f);
                    pWork->m_nSubStep = 0;
                    pWork->m_nStep = 3;
                } else {
                    AnimPlay(3, 0x5C, 0, 4.0f, 0.0f);
                    pWork->m_nSubStep = 0;
                    pWork->m_nStep = 2;
                }
            } else {
                bcs::FVEC3 head;
                bcs::FVEC3 tongue;
                GetVNodePos(2, head);
                GetNodePos("shita", tongue);
                head.y = tongue.y;
                SetBallCatchEnableWorld_(0.5f, head, tongue);
            }
            break;
        case 3:
            if (pWork->m_nSubStep == 0) {
                if (IsAnimEnd(3)) {
                    AnimPlay(3, 0x5A, 0, 8.0f, 0.0f);
                    pWork->m_nSubStep++;
                }
            } else if (IsAnimEnd(3)) {
                AnimPlay(3, 0x5B, 0, 8.0f, 0.0f);
                pWork->m_nStep = 4;
            }
            break;
        case 2:
            if (pWork->m_nSubStep == 0) {
                if (IsAnimEnd(3)) {
                    AnimPlay(3, 0x5D, 0, 8.0f, 0.0f);
                    pWork->m_nSubStep++;
                }
            } else if (IsAnimEnd(3)) {
                AnimPlay(3, 0x5E, 0, 8.0f, 0.0f);
                pWork->m_nStep = 4;
            }
            break;
        case 4:
            if (!IsAnimEnd(3)) {
                break;
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
        CBall *pBall = GetAttachBall();
        if (pBall) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            float fPower = pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            int bFlick = 0;
            float fDir = GetRotY();
            if (nReact & 0x8800) {
                fDir = GetFlickDir_(pBall);
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
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, bFlick ? pWork->m_nFlickType : pWork->m_nReceiveType);
            if (bFlick) {
                fDir = GetRotY_FromPosXZ(pBall->GetPos());
                if (pWork->m_Dir.x != 0.0f || pWork->m_Dir.z != 0.0f) {
                    fDir = WrapRad(nw4r::math::Atan2Rad(pWork->m_Dir.x, pWork->m_Dir.z));
                }
                MethdDamage(0x11, fDir, fPower, 0.0f, -1, 0.0f, 0, 0);
                return 0;
            }
        }
    }
    return bEnd;
}
