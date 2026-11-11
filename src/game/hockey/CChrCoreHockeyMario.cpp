// msw::CChrCoreHockeyMario: Mario in the hockey game. His special shot only
// adds effects (and the jump sound) to the common one.
#include "game/msw/CChrCoreHockey.hpp"

msw::CChrCoreHockeyMario::CChrCoreHockeyMario()
{
}

msw::CChrCoreHockeyMario::~CChrCoreHockeyMario()
{
}

void msw::CChrCoreHockeyMario::Reset_()
{
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeyMario::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x83, 3);
    SE_PlayArtsJumpSE(0);
}

void msw::CChrCoreHockeyMario::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x84, 3);
}

void msw::CChrCoreHockeyMario::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x85, 3);
}
