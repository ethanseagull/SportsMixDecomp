// msw::CCockpitVolleyModelTelop: the volleyball telops drawn as effects.
#include "game/msw/CCockpitVolleyModelTelop.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (names guessed) What each telop type shows.
    struct TelopData {
        long nEffectId; // effect for slot 0
        long nSe;       // sound played with it, -1 for none
    };

    const TelopData s_TelopData[4] = {
        { 0x6CF, -1 },
        { 0x6D0, -1 },
        { 0x6CD, -1 },
        { 0x6CE, -1 },
    };
}

msw::CCockpitVolleyModelTelop::CCockpitVolleyModelTelop(long nId, long nType, float fSortZ)
    : CCockpitModelTelop(nId, nType, fSortZ)
{
}

msw::CCockpitVolleyModelTelop::~CCockpitVolleyModelTelop() {}

int msw::CCockpitVolleyModelTelop::Set()
{
    if (m_nType >= 0 && m_nType < 4) {
        const TelopData &rData = s_TelopData[m_nType];

        SetEffect(0, rData.nEffectId);
        if (rData.nSe >= 0) {
            CGameSnd::Play(rData.nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_nStep = 1;
    }
    return 0;
}
