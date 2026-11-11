#include "game/msw/CChrCoreDodge.hpp"

#include "game/msw/CEffectCtrl.hpp"

// The original's .rodata for this file is an unread "hoos_top" (the node
// name Luigi's vacuum uses), most likely from a pointer defined in a header
// both files include and dropped by the link here. Our compiler drops it
// entirely, so this unit is 0x10 bytes short of .rodata.

// msw::CChrCoreDodgeMario: Mario in the dodgeball game. His special shot
// only adds effects to the common one.

msw::CChrCoreDodgeMario::CChrCoreDodgeMario()
{
}

msw::CChrCoreDodgeMario::~CChrCoreDodgeMario()
{
}

void msw::CChrCoreDodgeMario::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x62, 3);
}

void msw::CChrCoreDodgeMario::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotReadyArtsShot_(pWork);
}

void msw::CChrCoreDodgeMario::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x64, 3);
}

void msw::CChrCoreDodgeMario::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x63, 3);
}
