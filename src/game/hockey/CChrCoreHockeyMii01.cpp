// msw::CChrCoreHockeyMii01: a Mii in the hockey game. The special shot is
// one extra motion (0x9D) before the common one, with its own effects.
#include "game/msw/CChrCoreHockey.hpp"

msw::CChrCoreHockeyMii01::CChrCoreHockeyMii01()
{
}

msw::CChrCoreHockeyMii01::~CChrCoreHockeyMii01()
{
}

void msw::CChrCoreHockeyMii01::Reset_()
{
    CChrCoreHockey::Reset_();
}

// When the jump motion ends, motion 0x9D starts with its effect and the
// jump sound; the shot is ready when that is over.
int msw::CChrCoreHockeyMii01::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    if (pWork->m_nTrickStep == 0) {
        if (IsAnimEnd(3)) {
            AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
            Eff_CreateAttachAuto(0x13, 0xE0, 3);
            SE_PlayArtsJumpSE(0);
            pWork->m_nTrickStep++;
        }
    } else if (IsAnimEnd(3)) {
        bReady = 1;
    }
    return bReady;
}

void msw::CChrCoreHockeyMii01::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xE1, 3);
}

void msw::CChrCoreHockeyMii01::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xE2, 3);
}
