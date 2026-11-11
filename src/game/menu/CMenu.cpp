// msw::CMenu, the root task of the menus: loads the menu's data, runs the
// select menu and, when it has gone, cleans up and moves the main step on.
#include "game/msw/CMenu.hpp"

#include "bcs/CCameraMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/G2D/CLytResource.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitMenu.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlMenu.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMainStep.hpp"
#include "game/msw/CNowLoading.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/EvDemoVs.hpp"

namespace msw {

    CMenu::CMenu(bcs::CTask *pParent)
        : CTask(pParent, -1)
    {
        Initialize();
    }

    CMenu::~CMenu()
    {
        Finalize();
    }

    void CMenu::Initialize()
    {
        m_nSeq = 0;
        m_hMainTask = 0;
        EvDemoVs::CopyResMode(0);

        InitFsm(this, STATE_NUM);
        SetState(STATE_INITIALIZE, &CMenu::StateInitialize_In, &CMenu::StateInitialize_Loop,
                 &CMenu::StateInitialize_Out);
        SetState(STATE_START_UP, &CMenu::StateStartUp_In, &CMenu::StateStartUp_Loop, &CMenu::StateStartUp_Out);
        SetState(STATE_TITLE, &CMenu::StateTitle_In, &CMenu::StateTitle_Loop, &CMenu::StateTitle_Out);
        SetState(STATE_SELECT, &CMenu::StateSelect_In, &CMenu::StateSelect_Loop, &CMenu::StateSelect_Out);
        SetState(STATE_TEST, &CMenu::StateTest_In, &CMenu::StateTest_Loop, &CMenu::StateTest_Out);
        SetState(STATE_END, &CMenu::StateEnd_In, &CMenu::StateEnd_Loop, &CMenu::StateEnd_Out);
        ChangeState(STATE_INITIALIZE);
    }

    void CMenu::Finalize()
    {
        CCockpit::Remove();
        CEffectCtrl::Dispose();
        CEffectCtrl::DetachResource();
        CEffectCtrl::Close();
        EvDemoVs::CopyRes();
        CLoadData::UnLoad();
    }

    void CMenu::LoadDataInitialize()
    {
        CLoadData::InitData();
        CLytCtrl::CreateInstance(this);
        bcs::G2D::CLytResource *pRes = bcs::G2D::CLytResource::GetInstance();
        if (pRes != 0) {
            for (long i = 0x4A; i < 0x53; i++) {
                pRes->GetResourceFromDvdMan(CLoadData::GetFilePath(i), 0);
            }
        }
        CEffectCtrlMenu::CreateInstance(this);
        CCockpitMenu::Create(this);
    }

    void CMenu::Main()
    {
        ExecState();
    }

    void CMenu::Disp() {}

    void CMenu::StateInitialize_In()
    {
        m_nSeq = 0;
        bcs::CMainApp::Start3DFade(0, 1.0f);
        bcs::CMainApp::StartMasterFade(0, 1.0f);
    }

    void CMenu::StateInitialize_Loop()
    {
        switch (m_nSeq) {
            case 0:
                CLoadData::LoadMenu();
                CNowLoading::SetDisp(1);
                m_nSeq = 1;
                break;
            case 1:
                if (CLoadData::IsComplete(-1)) {
                    CNowLoading::SetDisp(0);
                    LoadDataInitialize();
                    m_nSeq = 2;
                }
                break;
            case 2:
                if (bcs::CSnd::IsReady() == 1) {
                    CGameSnd::MenuSaveHeapState();
                    m_nSeq = 100;
                }
                break;
            default:
                if (CGamePadCheck::m_PadCheckStap < 2) {
                    ChangeState(STATE_SELECT);
                }
                break;
        }
    }

    void CMenu::StateInitialize_Out()
    {
        bcs::CMainApp::Start3DFade(4, 1.0f);
        bcs::CMainApp::StartMasterFade(4, 1.0f);
    }

    void CMenu::StateStartUp_In() {}

    void CMenu::StateStartUp_Loop() {}

    void CMenu::StateStartUp_Out() {}

    void CMenu::StateTitle_In() {}

    void CMenu::StateTitle_Loop() {}

    void CMenu::StateTitle_Out() {}

    void CMenu::StateSelect_In()
    {
        CSelectMenu::InitLoad();
        if (CGamePadCheck::GetInstance() != 0) {
            CGamePadCheck::GetInstance()->m_UnkAC = 0;
        }
        m_nSeq = 0;
    }

    void CMenu::StateSelect_Loop()
    {
        if (m_nSeq == 0) {
            if (CSelectMenu::IsCompleteOfInitLoad()) {
                CSelectMenu *pMenu = new CSelectMenu(this);
                if (pMenu != 0) {
                    m_hMainTask = pMenu->GetId();
                }
                m_nSeq = 1;
            }
        } else {
            bcs::CTask *pTask = GetMainTask();
            if (pTask == 0 || pTask->IsDelete()) {
                ChangeState(STATE_END);
            }
        }
    }

    void CMenu::StateSelect_Out() {}

    void CMenu::StateTest_In() {}

    void CMenu::StateTest_Loop() {}

    void CMenu::StateTest_Out() {}

    void CMenu::StateEnd_In()
    {
        m_nSeq = 0;
    }

    void CMenu::StateEnd_Loop()
    {
        unsigned char nSeq = m_nSeq;
        switch (nSeq) {
            case 0:
                CGameCamera::Finalize();
                bcs::CCameraMan::RemoveEntryAll();
                DeleteAllChild();
                // fall through
            case 1:
                bcs::CRenderMan::ReqCleanUp();
                // fall through
            case 2:
            case 3:
                m_nSeq = nSeq + 1;
                break;
            default:
                Delete();
                if (m_nNextStep != 3) {
                    EvDemoVs::CopyResMode(3);
                }
                CMainStep::ChangeStep(m_nNextStep);
                CGame::SetReturnStep(0);
                break;
        }
    }

    void CMenu::StateEnd_Out() {}

    bcs::CTask *CMenu::GetMainTask()
    {
        return bcs::CTaskMan::GetTask(m_hMainTask);
    }
}
