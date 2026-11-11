// msw::CChrCoreHockeyMii00: a Mii in the hockey game. The special shot is
// one extra motion (0x9D) before the common one, with its own effects.
#include "game/msw/CChrCoreHockey.hpp"

msw::CChrCoreHockeyMii00::CChrCoreHockeyMii00()
{
}

msw::CChrCoreHockeyMii00::~CChrCoreHockeyMii00()
{
}

void msw::CChrCoreHockeyMii00::Reset_()
{
    CChrCoreHockey::Reset_();
}

// When the jump motion ends, motion 0x9D starts with its effect and the
// jump sound; the shot is ready when that is over.
int msw::CChrCoreHockeyMii00::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    if (pWork->m_nTrickStep == 0) {
        if (IsAnimEnd(3)) {
            AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x13, 0xDC, 3);
            SE_PlayArtsJumpSE(0);
            pWork->m_nTrickStep++;
        }
    } else if (IsAnimEnd(3)) {
        bReady = 1;
    }
    return bReady;
}

void msw::CChrCoreHockeyMii00::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xDD, 3);
}

void msw::CChrCoreHockeyMii00::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xDE, 3);
}
