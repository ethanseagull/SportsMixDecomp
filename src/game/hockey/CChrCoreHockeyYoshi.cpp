// msw::CChrCoreHockeyYoshi: Yoshi in the hockey game. His special shot only
// adds effects to the common one.
#include "game/msw/CChrCoreHockey.hpp"

msw::CChrCoreHockeyYoshi::CChrCoreHockeyYoshi()
{
}

msw::CChrCoreHockeyYoshi::~CChrCoreHockeyYoshi()
{
}

void msw::CChrCoreHockeyYoshi::Reset_()
{
    CChrCoreHockey::Reset_();
}

void msw::CChrCoreHockeyYoshi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x92, 3);
    Eff_CreateAttachAuto(0x14, 0x93, 3);
    SE_PlayArtsJumpSE(0);
}

void msw::CChrCoreHockeyYoshi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x15, 0x94, 3);
}

void msw::CChrCoreHockeyYoshi::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotShootBall_(pWork);

    Eff_Stop(0x14);
}
