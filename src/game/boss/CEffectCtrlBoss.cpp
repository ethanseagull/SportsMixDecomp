#include "game/msw/CEffectCtrlBoss.hpp"

msw::CEffectCtrl *msw::CEffectCtrlBoss::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlBoss(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlBoss::~CEffectCtrlBoss()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlBoss::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlBoss::Disp()
{
}
