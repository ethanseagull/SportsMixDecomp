#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "game/msw/CEffectCtrl.hpp"

// (guessed) The original's .rodata starts with an unread "hoos_top" (the
// node name Luigi's vacuum uses) and 0.0f: constants of code the module link
// stripped, while keeping its data. Named here and kept by the module's
// force_active list in config.yml; nothing reads them. See notes/toolchain.md,
// "Unread constants of stripped code".
namespace CChrCoreDodgeMario_cpp {
    extern const char UnusedString[] = "hoos_top";
    extern const float UnusedFloat = 0.0f;
}

// 60 degrees in radians; unused here, as in the other DodgeF chr cores
// (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

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

void msw::CChrCoreDodgeMario::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}
