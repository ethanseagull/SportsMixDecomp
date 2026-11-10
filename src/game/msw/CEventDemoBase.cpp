#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CEventCamera.hpp"
#include "game/msw/CEvDemoRes.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    CEventDemoBase *CEventDemoBase::m_pInstance;

    CEventDemoBase *CEventDemoBase::GetInstance() {
        return m_pInstance;
    }

    CEventDemoBase *CEventDemoBase::GetPtr() {
        return m_pInstance;
    }

    CEventDemoBase::CEventDemoBase(bcs::CTask *pParent) : bcs::CTask(pParent, -1) {
        m_pList = 0;
        m_bFade = 0;
    }

    CEventDemoBase::~CEventDemoBase() {
        m_pInstance = 0;
    }

    void CEventDemoBase::_Init() {
        m_pCamera = CEventCamera::CreateInstance(this);
        CEvDemoRes *pRes = CEvDemoRes::CreateInstance(this);
        pRes->Setup(0x100);
        pRes->SetStage(CGame::g_GameWork.m_nStage);
        pRes->SetGameMode(CGame::g_GameWork.m_nGameMode);
    }

    void CEventDemoBase::_Destroy() {
        FadeEndFunc *p = m_pList;
        while (p != 0) {
            FadeEndFunc *pEntry = p;
            p = p->pNext;
            delete pEntry;
        }
        m_pList = 0;
        m_bFade = 0;
    }

    void CEventDemoBase::Main() {
        if (m_pList != 0) {
            int bWasFade = m_bFade;
            m_bFade = bcs::CMainApp::IsPlayMasterFade();
            if (bWasFade && !m_bFade) {
                for (FadeEndFunc *p = m_pList; p != 0; p = p->pNext) {
                    p->pFunc();
                }
                _Destroy();
            }
        }
    }

    void CEventDemoBase::SendCameraEvent(const char *pName, long nArg1, long nArg2, float fArg) {
        if (m_pInstance != 0) {
            m_pInstance->CameraEvent(pName, nArg1, nArg2, fArg);
        }
    }

    void CEventDemoBase::AddFadeEndFunc(void (*pFunc)()) {
        if (m_pInstance != 0) {
            FadeEndFunc *pEntry = new FadeEndFunc;
            pEntry->pNext = m_pInstance->m_pList;
            pEntry->pFunc = pFunc;
            m_pInstance->m_pList = pEntry;
        }
    }
}
