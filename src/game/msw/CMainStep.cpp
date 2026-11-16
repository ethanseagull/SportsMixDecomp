#include "game/msw/CMainStep.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CPad.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/EvDemoVs.hpp"
#include "game/msw/dwc/CGameDwc.hpp"

// Functions of the DOL that have no name yet (declared with their symbol
// names until their classes are known).
extern "C" {
    void fn_800526D4();
    void fn_80071128();          // resumes after an outside pause (guess)
    // bcs::CSnd::StopAllSound, which takes the fade frames (the header and
    // symbols still say it takes nothing; notes/modules/main.md).
    void fn_800797A4(float fFadeFrames);
    void fn_800520FC(int);       // next to bcs::CMainApp::SetHalfFps
    void fn_80051AC0(int);
    void fn_8006F37C(int);
    void fn_80073FB4(int);
    void fn_80073FD0(int);
    void fn_80073FEC(int);
}

namespace msw {

    enum {
        STEP_INITIALIZE,
        STEP_SAVE_INIT,
        STEP_MENU,
        STEP_TOURNAMENT,
        STEP_UNLOCK,
        STEP_GAME_BASKET,
        STEP_GAME_VOLLEY,
        STEP_GAME_DODGE,
        STEP_GAME_HOCKEY,
        STEP_GAME_BOSS,
        STEP_FINALIZE,
        STEP_NUM
    };

    CMainStep *CMainStep::g_pInstance;

    CMainStep::CMainStep() : bcs::CTask(0, -1)
    {
        SetUnk08(0xFC00);
        m_nTaskId = 0;
        m_nNextStep = 0;
        m_nAppStep = 0;
        m_Unk8C = 0;
        InitFsm(this, STEP_NUM);
        SetState(STEP_INITIALIZE, &CMainStep::CtorStateInitialize_, &CMainStep::LoopStateInitialize_,
                 &CMainStep::DtorStateInitialize_);
        SetState(STEP_SAVE_INIT, &CMainStep::CtorStateSaveInit_, &CMainStep::LoopStateSaveInit_,
                 &CMainStep::DtorStateSaveInit_);
        SetState(STEP_MENU, &CMainStep::CtorStateMenu_, &CMainStep::LoopStateMenu_, &CMainStep::DtorStateMenu_);
        SetState(STEP_TOURNAMENT, &CMainStep::CtorStateTournament_, &CMainStep::LoopStateTournament_,
                 &CMainStep::DtorStateTournament_);
        SetState(STEP_UNLOCK, &CMainStep::CtorStateUnlock_, &CMainStep::LoopStateUnlock_,
                 &CMainStep::DtorStateUnlock_);
        SetState(STEP_GAME_BASKET, &CMainStep::CtorStateGameBasket_, &CMainStep::LoopStateGameBasket_,
                 &CMainStep::DtorStateGameBasket_);
        SetState(STEP_GAME_VOLLEY, &CMainStep::CtorStateGameVolley_, &CMainStep::LoopStateGameVolley_,
                 &CMainStep::DtorStateGameVolley_);
        SetState(STEP_GAME_DODGE, &CMainStep::CtorStateGameDodge_, &CMainStep::LoopStateGameDodge_,
                 &CMainStep::DtorStateGameDodge_);
        SetState(STEP_GAME_HOCKEY, &CMainStep::CtorStateGameHockey_, &CMainStep::LoopStateGameHockey_,
                 &CMainStep::DtorStateGameHockey_);
        SetState(STEP_GAME_BOSS, &CMainStep::CtorStateGameBoss_, &CMainStep::LoopStateGameBoss_,
                 &CMainStep::DtorStateGameBoss_);
        SetState(STEP_FINALIZE, &CMainStep::CtorStateFinalize_, &CMainStep::LoopStateFinalize_,
                 &CMainStep::DtorStateFinalize_);
        // Enters the step set as next (0 here).
        DoChangeStep(-1);
    }

    CMainStep::~CMainStep()
    {
    }

    void CMainStep::ChangeStateProc()
    {
        m_nAppStep = 0;
        bcs::CMainApp::SetHalfFps(0);
        fn_800520FC(0);
        bcs::CMainApp::SetUnivSpeed(1.0f);
        fn_8006F37C(0);
    }

    void CMainStep::Main()
    {
        ExecState();
    }

    void CMainStep::DoChangeStep(long step)
    {
        if (step >= 0) {
            m_nNextStep = step;
        }
        ChangeState(m_nNextStep);
    }

    void CMainStep::SetNextStep(long step)
    {
        if (g_pInstance != 0) {
            g_pInstance->m_nNextStep = step;
        }
    }

    void CMainStep::ChangeStep(long step)
    {
        CMainStep *pStep = g_pInstance;
        if (pStep == 0) {
            return;
        }
        if (step >= 0) {
            pStep->m_nNextStep = step;
        }
        bcs::CTask *pTask = bcs::CTaskMan::GetTask(pStep->m_nTaskId);
        if (pTask != 0) {
            pTask->Delete();
        }
    }

    long CMainStep::GetStep()
    {
        if (g_pInstance != 0) {
            return g_pInstance->m_nState;
        }
        return 0;
    }

    void CMainStep::AppLoop()
    {
        if (m_nAppStep == 0) {
            if (bcs::CTaskMan::GetTask(m_nTaskId) == 0) {
                m_nAppStep = 100;
            }
        } else if (m_nAppStep == 1) {
            if (bcs::CTaskMan::GetTask(m_nTaskId) == 0) {
                ResetSystem();
                m_nAppStep++;
            }
        } else if (m_nAppStep == 2) {
            int bReady = m_Unk8C == 0 ? 1 : bcs::CSnd::IsReady() != 0;
            if (bReady) {
                m_nAppStep++;
            }
        } else {
            m_nTaskId = 0;
            m_Unk8C = 0;
            ChangeState(m_nNextStep);
        }
    }

    void CMainStep::ResetSystem()
    {
        if (m_Unk8C != 0) {
            fn_800526D4();
            fn_80071128();
            fn_800797A4(0.0f);
            bcs::CMainApp::Start3DFade(4, 0.0f);
            bcs::CMainApp::StartMasterFade(4, 0.0f);
            bcs::CMainApp::StopCrossFade();
            dwc::CGameDwc::Finalize();
            bcs::CMainApp::SetAppSpeed(1.0f);
            CGame::SetDefaultParam();
            CTournament::Initialize(1);
            EvDemoVs::Finalize();
            CEventDemo::m_bDemonstration = 0;
            CEventDemo::m_nDemonstration = 0;
            bcs::CPad::SetNunchukStickClamp(0xF, 0x47, 0);
            bcs::CMainApp::SetHalfFps(0);
            fn_800520FC(0);
            fn_80051AC0(0);
            fn_80073FB4(0);
            fn_80073FD0(0);
            fn_80073FEC(0);
        }
    }
}
