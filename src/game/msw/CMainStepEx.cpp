#include "game/msw/CMainStepEx.hpp"

#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CRsoModule.hpp"
#include "game/msw/CSave.hpp"

void msw::CMainStepEx::CtorStateSaveInit()
{
    m_nSubStep = 0;
    bcs::CMainApp::StartMasterFade(4, 1.0f / 30.0f);
    CSave::Initialize();
}

void msw::CMainStepEx::CtorStateMenu()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(0);
    GetMenuModule()->LoadAsync();
}

void msw::CMainStepEx::DtorStateMenu()
{
    GetMenuModule()->UnMount();
}

void msw::CMainStepEx::CtorStateUnlock()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(0);
}
