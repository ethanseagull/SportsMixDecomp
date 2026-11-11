// msw::CChrCoreDodgeMoogle: the Moogle in the dodgeball game.
//
// His special shot plays its own sound and effects while he gets ready, and
// after the ball has left he plays two more motions while he falls, still
// moving and turning along the trick's path.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "game/msw/CGame.hpp"

// The original's .rodata has a -1.0f after the 8.0f below that nothing in
// the file reads (likely a stripped function). A dead comparison in
// SubStateArtsShotIsEndArtsShotAfter_ emits it but makes that function keep a
// base register for its constants, so it is left out: .rodata is 4 bytes
// short of the original's.

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

msw::CChrCoreDodgeMoogle::CChrCoreDodgeMoogle()
{
}

msw::CChrCoreDodgeMoogle::~CChrCoreDodgeMoogle()
{
}

// The trick starts the 240-frame timer that ends with bit 1 of the work's
// flags cleared (see SubStateArtsShotIsEndArtsShotAfter_).
void msw::CChrCoreDodgeMoogle::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x97, 3);
    pWork->m_fUnkD8 = 240.0f;
}

// In the first step of the trick, the charge sound starts at frame 40 of
// the motion and the charge effect at frame 132 (or when the motion ends),
// each only once.
int msw::CChrCoreDodgeMoogle::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (!SE_IsPlay(9) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 40.0f)) {
        SE_PlayMove(9, 0x45F, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
    }
    if (!Eff_IsAlive(0x14) && pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 132.0f)) {
        Eff_CreateAttachAuto(0x14, 0x98, 3);
    }
    return CChrCoreDodge::SubStateArtsShotIsReadyArtsShot_(pWork);
}

void msw::CChrCoreDodgeMoogle::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x99, 3);
    Eff_CreateAttachAuto(0x16, 0x9A, 3);
}

// The charge effect and sound end with the shot.
void msw::CChrCoreDodgeMoogle::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotShootBall_(pWork);
    Eff_Erase(0x14);
    SE_Stop(9, 0.0f);
    pWork->m_nAfterStep = 0;
}

// Step 0 waits for the shot motion and starts motion 0x80; step 1 starts
// motion 0x81 once bit 3 of the work's flags is set or the game allows it
// (bit 0 or 6 of the game flags), clearing bit 1. Meanwhile the Moogle
// falls, turns and moves along the trick's path until the move is over,
// and counts down the timer started with the trick. Later steps are the
// common ones.
int msw::CChrCoreDodgeMoogle::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    switch (pWork->m_nAfterStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x80, 1, 0.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        case 1:
            if ((pWork->m_nFlag04 & 8) || (CGame::g_GameWork.m_nFlag18 & 1) ||
                (CGame::g_GameWork.m_nFlag18 & 0x40)) {
                pWork->m_nFlag04 &= ~2;
                Eff_Stop(0x16);
                AnimPlay(3, 0x81, 0, 8.0f, 0.0f);
                pWork->m_nAfterStep++;
            }
            break;
        default:
            return CChrCoreDodge::SubStateArtsShotIsEndArtsShotAfter_(pWork);
    }

    int nJump = Util_DoJumpMoveEX(0);
    if (m_fJumpVelY <= 0.0f) {
        m_fJumpVelY = 0.0f;
        m_fJumpUnk224 += m_fFrameStep * m_fTimeScale;
    }
    if (nJump == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnkD4 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnkD4 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_MovePos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnkD4, pWork->m_DirMove, 0);
        }
    }
    if (pWork->m_fUnkD8 > 0.0f) {
        pWork->m_fUnkD8 -= m_fFrameStep * m_fTimeScale;
        if (pWork->m_fUnkD8 <= 0.0f) {
            pWork->m_fUnkD8 = 0.0f;
            pWork->m_nFlag04 &= ~2;
        }
    }
    return 0;
}

