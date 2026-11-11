// msw::CChrCoreBasketMoogle: the Moogle in the basketball game.
//
// His jump to the upper floor keeps a small body collision. His special
// shot plays its own sound and effects while he gets ready, and after the
// ball has left he plays two more motions while he falls, still moving
// and turning along the trick's path.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"

namespace {

    // Stand-in for an unknown user of 0.5 and -1, which the original keeps
    // in .rodata after the charge effect's frame but nothing reads (likely
    // stripped code).
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
    }

    // (name guessed) 70 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(70.0f);
}

msw::CChrCoreBasketMoogle::CChrCoreBasketMoogle()
{
}

msw::CChrCoreBasketMoogle::~CChrCoreBasketMoogle()
{
}

void msw::CChrCoreBasketMoogle::CtorStateJumpUpper()
{
    CChrCoreBasket::CtorStateJumpUpper();
}

// While motion 0x6C plays the body collision is the small one.
void msw::CChrCoreBasketMoogle::LoopStateJumpUpper()
{
    CChrCoreBasket::LoopStateJumpUpper();
    if (AnimGetMotID(3, 0) == 0x6C) {
        SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    }
}

void msw::CChrCoreBasketMoogle::DtorStateJumpUpper()
{
    CChrCoreBasket::DtorStateJumpUpper();
    SetCollBodyPushType_(static_cast<CChrCore::BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
}

void msw::CChrCoreBasketMoogle::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x9B, 3);
}

// In the first step of the trick, the charge sound starts at frame 40 of
// the motion and the charge effect at frame 132 (or when the motion ends),
// each only once.
int msw::CChrCoreBasketMoogle::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    if (!SE_IsPlay(9) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 40.0f)) {
        SE_PlayMove(9, 0x45F, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
    }
    if (!Eff_IsAlive(0x14) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 132.0f)) {
        Eff_CreateAttachAuto(0x14, 0x9C, 3);
    }
    return CChrCoreBasket::SubStateArtsShotIsReadyArtsShot_(pWork);
}

void msw::CChrCoreBasketMoogle::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x9D, 3);
    Eff_CreateAttachAuto(0x16, 0x9E, 3);
    UnusedReferences(pWork->m_fUnk58);
}

// The charge effect and sound end with the shot.
void msw::CChrCoreBasketMoogle::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    GetAttachBall(); // result not used
    CChrCoreBasket::SubStateArtsShotShootBall_(pWork);
    Eff_Erase(0x14);
    SE_Stop(9, 0.0f);
    Util_ShootBall(9, 1.0f, 1, 0);
    pWork->m_nAfterStep = 0;
}

// Step 0 waits for the shot motion and starts motion 0x84; step 1 starts
// motion 0x85 once the game allows it (bit 0 or 6 of the game flags, or
// while the basketball manager's flag 0xAC bit 1 is clear). Meanwhile the
// Moogle falls, and turns and moves along the trick's path until the move
// is over. Later steps are the common ones.
int msw::CChrCoreBasketMoogle::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nAfterStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x84, 1, 0.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        case 1:
            if ((CGame::g_GameWork.m_nFlag18 & 1) || (CGame::g_GameWork.m_nFlag18 & 0x40) ||
                CChrManBasket::GetInstance()->IsGhostAppearEnable()) {
                Eff_Stop(0x16);
                AnimPlay(3, 0x85, 0, 8.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        default:
            return CChrCoreBasket::SubStateArtsShotIsEndArtsShotAfter_(pWork);
    }

    int nJump = Util_DoJumpMoveEX(0);
    if (m_fJumpVelY <= 0.0f) {
        m_fJumpVelY = 0.0f;
        m_fJumpUnk224 += m_fFrameStep * m_fTimeScale;
    }
    if (nJump == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnk58 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnk58 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnk58, pWork->m_DirMove, 0);
        }
    }
    return 0;
}
