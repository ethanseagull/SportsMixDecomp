// msw::CCockpitBasketModelTelop: the basketball telops drawn as effects.
#include "game/msw/CCockpitBasketModelTelop.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (names guessed) What each telop type shows.
    struct TelopData {
        long nEffectId; // effect for slot 0
        long nSe;       // sound played with it, -1 for none
    };

    const TelopData s_TelopData[3] = {
        { 0x6C1, -1 },
        { 0x6C0, -1 },
        { 0x6C2, -1 },
    };
}

msw::CCockpitBasketModelTelop::CCockpitBasketModelTelop(long nId, long nType, float fSortZ)
    : CCockpitModelTelop(nId, nType, fSortZ)
{
}

msw::CCockpitBasketModelTelop::~CCockpitBasketModelTelop() {}

int msw::CCockpitBasketModelTelop::Set()
{
    if (m_nType >= 0 && m_nType < 3) {
        const TelopData &rData = s_TelopData[m_nType];

        SetEffect(0, rData.nEffectId);
        if (rData.nSe >= 0) {
            CGameSnd::Play(rData.nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_nStep = 1;
    }
    return 0;
}
