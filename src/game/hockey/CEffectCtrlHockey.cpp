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

// Passes everything on, then 3 and three zeros.
unsigned long msw::CEffectCtrlHockey::CreateAttachEffect(long id, const bcs::CScnObj *pObj, const bcs::FVEC3 *pPos,
                                                         unsigned long flags, int a, int b, unsigned long c)
{
    return CEffectCtrl::CreateAttachEffect(id, pObj, pPos, flags, a, b, c, 3, 0, 0, 0);
}

int msw::CEffectCtrlHockey::InitPatch(long, bcs::EF::CEffect *)
{
    return 0;
}
