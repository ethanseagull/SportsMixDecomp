// msw::CChrCoreDodgeMii01: a Mii (second body type) in the dodgeball game.
// The special shot is the common one with the Mii's own effects and a
// sound at frame 60 of the trick motion.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"

// 60 degrees in radians; unused here, as in the other DodgeF chr cores
// (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

msw::CChrCoreDodgeMii01::CChrCoreDodgeMii01()
{
}

msw::CChrCoreDodgeMii01::~CChrCoreDodgeMii01()
{
}

void msw::CChrCoreDodgeMii01::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xC0, 3);
}

// Not zero once the trick is over. Step 0 waits for the trick motion and
// plays a sound at its frame 60; the loop motion starts when it ends.
int msw::CChrCoreDodgeMii01::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount =
        pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
    SubStateArtsShotDoJumpMove_(pWork);

    if (pWork->m_nTrickStep == 0) {
        if (IsAnimEnd(3)) {
            pWork->m_nTrickStep++;
        } else if (AnimGetFrame(3) >= 60.0f) {
            PlaySePos(0x4A1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nTrickStep++;
        }
    }

    if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
        return 1;
    }

    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return 0;
}

void msw::CChrCoreDodgeMii01::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotReadyArtsShot_(pWork);
}

void msw::CChrCoreDodgeMii01::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
}

void msw::CChrCoreDodgeMii01::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xC1, 3);
}

void msw::CChrCoreDodgeMii01::DtorStateAirTrick()
{
    // Not used here. A guess: the original's .rodata has a -1 after the
    // trick's constants that nothing left in the file reads.
    float fUnused = -1.0f;

    CChrCoreDodge::DtorStateAirTrick();
}
