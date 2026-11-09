#include "game/msw/CEffectCtrlTournament.hpp"

msw::CEffectCtrl *msw::CEffectCtrlTournament::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlTournament(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlTournament::~CEffectCtrlTournament()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlTournament::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlTournament::Disp()
{
}

int msw::CEffectCtrlTournament::InitPatch(long, bcs::EF::CEffect *)
{
    return 0;
}
