// msw::CCockpitDodgeModelTelop: the dodgeball telops drawn as effects.
#include "game/msw/CCockpitDodgeModelTelop.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (names guessed) What each telop type shows.
    struct TelopData {
        long nEffectId; // effect for slot 0
        long nSe;       // sound played with it, -1 for none
    };

    const TelopData s_TelopData[3] = {
        { 0x684, -1 },
        { 0x685, -1 },
        { 0x686, -1 },
    };
}

msw::CCockpitDodgeModelTelop::CCockpitDodgeModelTelop(long nId, long nType, float fSortZ)
    : CCockpitModelTelop(nId, nType, fSortZ)
{
}

msw::CCockpitDodgeModelTelop::~CCockpitDodgeModelTelop() {}

int msw::CCockpitDodgeModelTelop::Set()
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
