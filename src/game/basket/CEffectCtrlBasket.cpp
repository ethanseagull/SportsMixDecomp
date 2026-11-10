// Written from src/game/tour/CEffectCtrlTournament.cpp by tools/decomp/port_unit.py (Tournament -> Basket); check against BasketF.
#include "game/msw/CEffectCtrlBasket.hpp"

msw::CEffectCtrl *msw::CEffectCtrlBasket::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlBasket(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlBasket::~CEffectCtrlBasket()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlBasket::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlBasket::Disp()
{
}

int msw::CEffectCtrlBasket::InitPatch(long, bcs::EF::CEffect *)
{
    return 0;
}
