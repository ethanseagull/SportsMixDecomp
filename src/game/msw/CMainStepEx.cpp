#include "game/msw/CMainStepEx.hpp"

#include "bcs/CCamera.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CRsoModule.hpp"
#include "game/msw/ArchiveFont.hpp"
#include "game/msw/CDataWork.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameBoss.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CGameHockey.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CGameVolley.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CMenu.hpp"
#include "game/msw/CNowLoading.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CTipsCtrl.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/EvDemoVs.hpp"
#include "game/msw/LayoutTask.hpp"
#include "game/msw/Tournament.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CMessage.hpp"

void msw::CMainStepEx::Initialize()
{
    if (g_pInstance == 0) {
        g_pInstance = new CMainStepEx;
    }
}

void msw::CMainStepEx::CtorStateInitialize()
{
    m_nSubStep = 0;
    CGame::SetDefaultParam();
    CTournament::Initialize(1);
    CTipsCtrl::ResetParam();
    CEventDemo::m_bDemonstration = 0;
    CEventDemo::m_nDemonstration = 0;
    CEventDemo::SetRandomDemoGameMode();
    EvDemoVs::Initialize();
    ArchiveFont::Initialize();
    ArchiveFont::AttachResource();
    menu::CMessage::s_Instance.Initialize();
    menu::CMessage::s_Instance.AttachResource(CLoadData::GetPtr(0x5A));
    LayoutTask::Initialize();
    CGamePadCheck::Initialize();
    if (CGamePadCheck::GetInstance() != 0) {
        CGamePadCheck::GetInstance()->m_UnkAC = 1;
    }
    bcs::CCamera::LoadQuakeData(bcs::CLoadMan::GetPtr("system/CamQuake.bin", 0));
    bcs::CLoadMan::UnLoad("system/CamQuake.bin", 0);
    CDataWork::s_Instance.Initialize();
}

void msw::CMainStepEx::LoopStateInitialize()
{
    DoChangeStep(1);
}

void msw::CMainStepEx::DtorStateInitialize()
{
}

void msw::CMainStepEx::CtorStateSaveInit()
{
    m_nSubStep = 0;
    bcs::CMainApp::StartMasterFade(4, 1.0f / 30.0f);
    CSave::Initialize();
}

void msw::CMainStepEx::LoopStateSaveInit()
{
    switch (m_nSubStep) {
        case 0:
            if (!bcs::CMainApp::IsPlayMasterFade() && CSave::CanbeWrite()) {
                m_nSubStep++;
            }
            break;
        case 1:
            CSave::SaveDataInit();
            m_nSubStep++;
            break;
        case 2:
            CNowLoading::SetType(2);
            DoChangeStep(2);
            m_nSubStep++;
            break;
    }
}

void msw::CMainStepEx::DtorStateSaveInit()
{
}

void msw::CMainStepEx::CtorStateMenu()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(0);
    GetMenuModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateMenu()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            GetMenuModule()->Mount();
            m_nSubStep++;
            m_nTaskId = (new CMenu(this))->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateMenu()
{
    GetMenuModule()->UnMount();
}

void msw::CMainStepEx::CtorStateTournament()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(0);
    GetTournamentModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateTournament()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            GetTournamentModule()->Mount();
            m_nSubStep++;
            m_nTaskId = Tournament::CreateInstance(this)->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateTournament()
{
    GetTournamentModule()->UnMount();
}

void msw::CMainStepEx::CtorStateUnlock()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(0);
}

void msw::CMainStepEx::LoopStateUnlock()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            m_nSubStep++;
            m_nTaskId = Unlock::CreateInstance(this)->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateUnlock()
{
}

void msw::CMainStepEx::CtorStateGameBasket()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(1);
    GetBasketModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateGameBasket()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            GetBasketModule()->Mount();
            m_nSubStep++;
            m_nTaskId = CGameBasket::CreateInstance()->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateGameBasket()
{
    GetBasketModule()->UnMount();
}

void msw::CMainStepEx::CtorStateGameVolley()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(1);
    GetVolleyModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateGameVolley()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            CGame *pGame;

            GetVolleyModule()->Mount();
            m_nSubStep++;
            if (CGame::g_GameWork.GetGameType() == 2) {
                pGame = new CGameVolleyRhythmReceive(0);
            } else {
                pGame = new CGameVolley(0);
            }
            m_nTaskId = pGame->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateGameVolley()
{
    GetVolleyModule()->UnMount();
}

void msw::CMainStepEx::CtorStateGameDodge()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(1);
    GetDodgeModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateGameDodge()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            CGame *pGame;

            GetDodgeModule()->Mount();
            m_nSubStep++;
            if (CGame::g_GameWork.GetGameType() == 2) {
                pGame = new CGameDodgeDodgeOrDie(0);
            } else {
                pGame = new CGameDodge(0);
            }
            m_nTaskId = pGame->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateGameDodge()
{
    GetDodgeModule()->UnMount();
}

void msw::CMainStepEx::CtorStateGameHockey()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(1);
    GetHockeyModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateGameHockey()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            CGame *pGame;

            GetHockeyModule()->Mount();
            m_nSubStep++;
            if (CGame::g_GameWork.GetGameType() == 2) {
                if (CTournament::IsExtraMatchGame()) {
                    pGame = new CGameHockeyCSMission(0);
                } else {
                    pGame = new CGameHockeyCrashSkate(0);
                }
            } else {
                pGame = new CGameHockey(0);
            }
            m_nTaskId = pGame->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateGameHockey()
{
    GetHockeyModule()->UnMount();
}

void msw::CMainStepEx::CtorStateGameBoss()
{
    m_nSubStep = 0;
    SetNextStep(2);
    bcs::CLoadMan::SetRawDataBufferMem(1);
    GetBossModule()->LoadAsync();
}

void msw::CMainStepEx::LoopStateGameBoss()
{
    if (m_nSubStep == 0) {
        if (bcs::CLoadMan::IsComplete()) {
            GetBossModule()->Mount();
            m_nSubStep++;
            m_nTaskId = (new CGameBoss(0))->GetId();
        }
    } else {
        AppLoop();
    }
}

void msw::CMainStepEx::DtorStateGameBoss()
{
    GetBossModule()->UnMount();
}

void msw::CMainStepEx::CtorStateFinalize()
{
    CGamePadCheck::Finalize();
    LayoutTask::Finalize();
    menu::CMessage::s_Instance.Finalize();
    ArchiveFont::Dispose();
    CDataWork::s_Instance.Finalize();
}

void msw::CMainStepEx::LoopStateFinalize()
{
}

void msw::CMainStepEx::DtorStateFinalize()
{
}

msw::CMainStepEx::~CMainStepEx()
{
}
