// msw::CCockpitHockeyModelTelop: the hockey telops drawn as effects.
#include "game/msw/CCockpitHockeyModelTelop.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (names guessed) What each telop type shows.
    struct TelopData {
        long nEffectId; // effect for slot 0
        long nSe;       // sound played with it, -1 for none
    };

    const TelopData s_TelopData[5] = {
        { 0x711, 0xC00 },
        { 0x70E, -1 },
        { 0x70F, -1 },
        { 0x710, -1 },
        { 0x712, -1 },
    };
}

msw::CCockpitHockeyModelTelop::CCockpitHockeyModelTelop(long nId, long nType, float fSortZ)
    : CCockpitModelTelop(nId, nType, fSortZ)
{
}

msw::CCockpitHockeyModelTelop::~CCockpitHockeyModelTelop() {}

int msw::CCockpitHockeyModelTelop::Set()
{
    if (m_nType >= 0 && m_nType < 5) {
        const TelopData &rData = s_TelopData[m_nType];

        SetEffect(0, rData.nEffectId);
        if (rData.nSe >= 0) {
            CGameSnd::Play(rData.nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_nStep = 1;
    }
    return 0;
}
