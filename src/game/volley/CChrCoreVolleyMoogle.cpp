// msw::CChrCoreVolleyMoogle: the Moogle in the volleyball game.
//
// His block jump is a slow hover: after a short hang he drifts down, his
// sideways speed dragged and steered towards where the spike goes, and lands
// softly. His air trick pulls the ball in, plays its own charge sound and
// effects, and after the shot plays two more motions while he falls, still
// turning and moving along the trick's path.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>
#include "revolution/mtx.h"

namespace {

    // (guessed helpers) fValue kept within [fMin, fMax]; the share of a
    // move done in one frame when it takes fFrames frames.
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    inline float RatePerFrame(float fFrames)
    {
        return fFrames < 1.0f ? 1.0f : 1.0f / fFrames;
    }
}

msw::CChrCoreVolleyMoogle::CChrCoreVolleyMoogle()
{
}

msw::CChrCoreVolleyMoogle::~CChrCoreVolleyMoogle()
{
}

// Step 0 hangs in the air until m_fJumpUnk228 runs out. Step 1 drifts: the
// sideways speed is dragged by 7% a frame and, while the jump has a
// direction more than 30 degrees off the spike's, turned towards it over 15
// frames (and kept at most 1 long). He falls at m_fJumpAccelY, hanging again
// for m_fJumpUnk224 frames once he stops rising, and only a tenth of the
// sideways move is applied. On the ground he slides on slowly (step 2) until
// the motion ends.
void msw::CChrCoreVolleyMoogle::SubStateBlockJumpDoJumpMove_(STATE_WORK_BLOCK_JUMP *pWork)
{
    float fStep = m_fFrameStep * m_fTimeScale;

    switch (m_nJumpStep) {
        case 0:
            if (m_fJumpUnk228 > 0.0f) {
                m_fJumpUnk228 -= fStep;
            }
            if (m_fJumpUnk228 > 0.0f) {
                break;
            }
            m_nJumpStep = 1;
            m_nStatusFlag |= 0x400;
            // fall through
        case 1: {
            bcs::FVEC3 move(0.0f, 0.0f, 0.0f);

            float fDrag = 0.07f * m_fFrameStep * m_fTimeScale;
            bcs::FVEC3 vel;
            vel.x = m_fJumpVelX * fDrag;
            vel.z = m_fJumpVelZ * fDrag;
            m_fJumpVelX -= vel.x;
            m_fJumpVelZ -= vel.z;

            if (m_fJumpRotY >= 0.0f) {
                float fTargetRot = GetSpikeTargetPosRot_(0);
                bcs::FVEC3 dir;
                bcs::FVEC3 targetDir;
                bcs::BCS_VecOneZ_RotY(dir, m_fJumpRotY);
                bcs::BCS_VecOneZ_RotY(targetDir, fTargetRot);
                float fCos = Clamp(fabsf(targetDir.x * dir.x + targetDir.z * dir.z), 0.0f, 1.0f);
                // cos(30 degrees); the sine table takes 256 steps a turn
                if (fCos <= nw4r::math::CosFIdx(30.0f * (256.0f / 360.0f))) {
                    float fOff = 1.0f - fCos;
                    bcs::FVEC3 axis;
                    // The direction at right angles to the jump's, on the
                    // side of the spike's.
                    PSVECCrossProduct((const Vec *)&dir, (const Vec *)&targetDir, (Vec *)&axis);
                    PSVECCrossProduct((const Vec *)&targetDir, (const Vec *)&axis, (Vec *)&vel);
                    float fTurnFrames = 15.0f;
                    float fTurn = Clamp(RatePerFrame(fTurnFrames) * fStep, 0.0f, 1.0f) * fOff;
                    vel.x *= fTurn;
                    vel.z *= fTurn;
                    vel.x += m_fJumpVelX;
                    vel.z += m_fJumpVelZ;
                    if (vel.x * vel.x + vel.z * vel.z > 1.0f) {
                        bcs::BCS_Vec_Normalize(vel, vel);
                    }
                    m_fJumpVelX = vel.x;
                    m_fJumpVelZ = vel.z;
                }
            }

            ClearRevise();
            float fAccel = m_fJumpAccelY;
            if (m_fJumpVelY <= 0.0f) {
                if (m_fJumpUnk224 > 0.0f) {
                    m_fJumpVelY = 0.0f;
                    fAccel = 0.0f;
                    m_fJumpUnk224 -= fStep;
                } else {
                    fAccel = m_fJumpGravity;
                    m_fJumpUnk224 = 0.0f;
                    m_fJumpAccelY = fAccel;
                }
            }

            bcs::FVEC3 step;
            bcs::FVEC3Scale(&step, &GetJumpVel(), fStep);
            float fVelAdd = fAccel * fStep;
            m_fJumpVelY += fVelAdd;
            step.y += 0.5f * (fVelAdd * fStep);
            bcs::FVEC3Add(&move, &move, &step);
            // 0.10500001f (0x3DD70A3F), one step above 0.105f; the original's
            // constant.
            move.x *= 0.10500001f;
            move.z *= 0.10500001f;
            AddPos(move);

            if (GetPosY() <= 0.0f && m_fJumpVelY < 0.0f) {
                m_fJumpVelY = 0.0f;
                m_fJumpAccelY = -0.025f;
                SetPosY(0.0f);
                m_nJumpStep = 2;
                m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x800;
                m_fJumpVelX *= 0.10500001f;
                m_fJumpVelZ *= 0.10500001f;
                break;
            }
            pWork->m_fRotY = GetBlockLookDir_(GetTargetBall_());
            break;
        }
        case 2:
            if (m_nStatusFlag & 0x800) {
                Util_DoJumpMoveLand(fStep);
                if (IsAnimEnd(3)) {
                    m_nJumpStep = 3;
                    m_nStatusFlag &= ~0x800;
                }
            }
            break;
    }
}

void msw::CChrCoreVolleyMoogle::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_nAdsorbStep = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs = m_AdsorbOfs;
    AttachBall(pBall->GetBallNo(), -1, pWork->m_fAdsorbFrameMax, 0);
    m_nStatusFlag &= ~0x2C;
}

// The ball comes in over the length of the pull motion, at most 115 frames.
float msw::CChrCoreVolleyMoogle::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    float fFrame = AnimGetResFrameNum(0x75);
    return 115.0f > fFrame ? fFrame : 115.0f;
}

void msw::CChrCoreVolleyMoogle::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x95, 3);
}

void msw::CChrCoreVolleyMoogle::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->StopTailEffect();
    }
}

// In the first step of the trick, the charge sound starts at frame 40 of
// the motion and the charge effect at frame 132 (or when the motion ends),
// each only once.
int msw::CChrCoreVolleyMoogle::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    if (!SE_IsPlay(9) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 40.0f)) {
        SE_PlayMove(9, 0x45F, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
    }
    if (!Eff_IsAlive(0x14) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 132.0f)) {
        Eff_CreateAttachAuto(0x14, 0x96, 3);
    }
    return CChrCoreVolley::SubStateAirTrickIsReadyArtsShot_(pWork);
}

void msw::CChrCoreVolleyMoogle::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x97, 3);
    Eff_CreateAttachAuto(0x16, 0x98, 3);
}

// The charge effect and sound end with the shot.
void msw::CChrCoreVolleyMoogle::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_nFlag |= 8;
    CChrCoreVolley::SubStateAirTrickShootBall_(pWork);
    Eff_Erase(0x14);
    SE_Stop(9, 0.0f);
    pWork->m_nAfterStep = 0;
}

// Step 0 waits for the shot motion and starts motion 0x7C; step 1 starts
// motion 0x7D once flag bit 2 of the work is set, hiding the target marker.
// Meanwhile the Moogle falls, and turns and moves along the trick's path
// until the move is over. Later steps are the common ones.
int msw::CChrCoreVolleyMoogle::SubStateAirTrickIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nAfterStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x7C, 1, 0.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        case 1:
            if (pWork->m_nFlag & 4) {
                SetTargetMarkerDisable_();
                Eff_Stop(0x16);
                AnimPlay(3, 0x7D, 0, 8.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        default:
            return CChrCoreVolley::SubStateAirTrickIsEndArtsShotAfter_(pWork);
    }

    int nJump = Util_DoJumpMoveEX(0);
    if (m_fJumpVelY <= 0.0f) {
        m_fJumpVelY = 0.0f;
        m_fJumpUnk224 += m_fFrameStep * m_fTimeScale;
    }
    if (nJump == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnk20 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnk20 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnk20, pWork->m_DirMove, 0);
        }
    }
    return 0;
}
