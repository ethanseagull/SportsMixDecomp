// Written from src/game/tour/CEffectCtrlTournament.cpp by tools/decomp/port_unit.py (Tournament -> Dodge); check against DodgeF.
#include "game/msw/CEffectCtrlDodge.hpp"

msw::CEffectCtrl *msw::CEffectCtrlDodge::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlDodge(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlDodge::~CEffectCtrlDodge()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlDodge::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlDodge::Disp()
{
}

void msw::CEffectCtrlDodge::CreateAttachClusterAuto(long id, const bcs::CScnObj *pObj, unsigned long flags)
{
    CEffectCtrl::CreateAttachClusterAuto(id, pObj, flags);
}
