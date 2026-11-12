// msw::CChrCoreDodgeYoshi: Yoshi in the dodgeball game. His special shot
// is the common one with his own effects.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"

namespace {

    // (name guessed) 60 degrees; nothing in the unit reads it.
    float s_fUnkRange = bcs::DEG_TO_RAD(60.0f);
}

msw::CChrCoreDodgeYoshi::CChrCoreDodgeYoshi()
{
}

msw::CChrCoreDodgeYoshi::~CChrCoreDodgeYoshi()
{
}

void msw::CChrCoreDodgeYoshi::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

void msw::CChrCoreDodgeYoshi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x71, 3);
    Eff_CreateAttachAuto(0x14, 0x72, 3);
}

void msw::CChrCoreDodgeYoshi::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotReadyArtsShot_(pWork);
}

void msw::CChrCoreDodgeYoshi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x73, 3);
}

void msw::CChrCoreDodgeYoshi::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotShootBall_(pWork);
    Eff_Stop(0x14);
}
