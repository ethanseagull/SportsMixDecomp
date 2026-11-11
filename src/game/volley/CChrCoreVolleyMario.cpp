// msw::CChrCoreVolleyMario: Mario in the volleyball game. His smash spike
// and his air trick only add effects to the common ones.
#include "game/msw/CChrCoreVolley.hpp"

msw::CChrCoreVolleyMario::CChrCoreVolleyMario()
{
}

msw::CChrCoreVolleyMario::~CChrCoreVolleyMario()
{
}

void msw::CChrCoreVolleyMario::Reset_()
{
    CChrCoreVolley::Reset_();
}

void msw::CChrCoreVolleyMario::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x650, m_nEffNode, 0, 1, 3, 0);
}

void msw::CChrCoreVolleyMario::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x62, 3);
}

void msw::CChrCoreVolleyMario::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0x64, 3);
}

void msw::CChrCoreVolleyMario::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0x63, 3);
}
