// Written from src/game/tour/CEffectCtrlTournament.cpp by tools/decomp/port_unit.py (Tournament -> Hockey); check against HockeyF.
#include "game/msw/CEffectCtrlHockey.hpp"

msw::CEffectCtrl *msw::CEffectCtrlHockey::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlHockey(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlHockey::~CEffectCtrlHockey()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlHockey::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlHockey::Disp()
{
}

int msw::CEffectCtrlHockey::InitPatch(long, bcs::EF::CEffect *)
{
    return 0;
}

// The last parameter is not passed on: the base gets 3 and four zeros instead
// (only three of the zeros are visible as stores; see the asm).
void msw::CEffectCtrlHockey::CreateAttachEffect(long id, const bcs::CScnObj *pObj, const bcs::FVEC3 *pPos,
                                                unsigned long flags, int a, int b, unsigned long)
{
    CEffectCtrl::CreateAttachEffect(id, pObj, pPos, flags, a, b, 3, 0, 0, 0, 0);
}
