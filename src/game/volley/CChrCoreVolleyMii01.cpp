// msw::CChrCoreVolleyMii01: the second Mii in the volleyball game. Its air
// trick waits for two motions, with a sound from frame 60 of the first.
#include "game/msw/CChrCoreVolley.hpp"

namespace {

    // (guessed) The original .rodata ends with a -1.0f that nothing left in
    // the file reads, most likely used by code the link stripped. This dead
    // comparison keeps it in place. See notes/decisions.md, "Unreferenced
    // data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
    }
}

msw::CChrCoreVolleyMii01::CChrCoreVolleyMii01()
{
}

msw::CChrCoreVolleyMii01::~CChrCoreVolleyMii01()
{
}

// Step 0 plays the sound at frame 60 of the trick motion, step 1 waits for
// the next motion to end; the shot is ready when a step's motion ends. The
// loop motion starts (once, m_nArtsLevel set to 1) when a motion ends.
int msw::CChrCoreVolleyMii01::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateAirTrickDoJumpMove_(pWork);
    SubStateAirTrickDoBallAdsorb_(pWork);

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                bReady = 1;
                pWork->m_nTrickStep++;
            } else if (AnimGetFrame(3) >= 60.0f) {
                PlaySePos(0x4A1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                bReady = 1;
                pWork->m_nTrickStep++;
            }
            break;
        default:
            bReady = 1;
            break;
    }

    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    UnusedReferences(pWork->m_fTrickCount);
    return bReady;
}

void msw::CChrCoreVolleyMii01::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xBF, 3);
}

void msw::CChrCoreVolleyMii01::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
}

void msw::CChrCoreVolleyMii01::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xC0, 3);
}
