// Written from src/game/tour/CEffectCtrlTournament.cpp by tools/decomp/port_unit.py (Tournament -> Volley); check against VolleyF.
#include "game/msw/CEffectCtrlVolley.hpp"

msw::CEffectCtrl *msw::CEffectCtrlVolley::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlVolley(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlVolley::~CEffectCtrlVolley()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlVolley::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlVolley::Disp()
{
}
