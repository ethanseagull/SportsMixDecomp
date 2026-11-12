// game/bcs/CTask
// The task system (.text 0x8007A94C..0x8007B774): the strap screen task,
// bcs::CTaskMan and bcs::CTask. One file: CTaskMan::Main inlines
// CTask::DeleteAllChild and CTaskMan::RequestPriority, which come after it.
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Draw2D.hpp"
#include "bcs/CHandleTable.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CPadWI.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/sc.h"
#include "revolution/tpl.h"
#include <stdio.h>

namespace bcs {

    // The strap screen shown at start-up (class and member names guessed;
    // unnamed in the DOL, vtable lbl_80468C18, 0x154 bytes): loads the
    // console language's strap.cmp, fades it in, turns its page up to three
    // times and fades out after 20 seconds or a button press (after the
    // first second).
    class CStrapTask : public CTask {
        public:
            // pLoaded is called once the file has loaded.
            CStrapTask(void (*pLoaded)());
            virtual ~CStrapTask();
            virtual void Main();
            virtual void Disp();

        private:
            long m_nState;           // 0x34
            int m_bWide;             // 0x38: the console is set to 16:9
            long m_nPage;            // 0x3C: the palette entry shown
            long m_nPageTurns;       // 0x40
            TPLPalette *m_pPalette;  // 0x44
            float m_fTime;           // 0x48
            float m_fPageTime;       // 0x4C
            char m_aPath[0x100];     // 0x50
            void (*m_pLoaded)();     // 0x150
    }; // size 0x154

    CStrapTask::CStrapTask(void (*pLoaded)()) : CTask(0, -1)
    {
        m_nState = 0;
        SetUnk08(0xFC00);
        ClearUnk08(0x400);
        m_bWide = 0;
        m_nPage = 0;
        m_nPageTurns = 0;
        m_pPalette = 0;
        m_fTime = 0.0f;
        m_fPageTime = 0.0f;
        m_pLoaded = pLoaded;
        if (SCGetAspectRatio() == 1) {
            m_bWide = 1;
        }
        sprintf(m_aPath, "strap/%s/strap.cmp", CMainApp::GetSystemLanguageName());
        CLoadMan::Load(m_aPath, 0, 2, 0, 0x20, 0);
    }

    CStrapTask::~CStrapTask()
    {
        CLoadMan::UnLoad(m_aPath, 0);
    }

    void CStrapTask::Main()
    {
        if (CMainApp::IsPause(0x400)) {
            // Paused before the first second: start counting again.
            if (m_nState <= 2 && m_fTime < 60.0f) {
                m_fTime = 0.0f;
            }
            return;
        }

        float fRate = GetFrameRateF();
        int bEnd = 0;
        switch (m_nState) {
            case 0:
                if (CLoadMan::IsComplete()) {
                    m_pPalette = static_cast<TPLPalette *>(CLoadMan::GetPtr(m_aPath, 0));
                    TPLBind(m_pPalette);
                    CMainApp::StartMasterFade(4, 1.0f / 30.0f);
                    if (m_pLoaded != 0) {
                        m_pLoaded();
                    }
                    m_nState++;
                }
                break;
            case 1:
                if (!CMainApp::IsPlayMasterFade()) {
                    m_nState++;
                }
                break;
            case 2:
                if (m_fPageTime >= 300.0f && m_nPageTurns < 3) {
                    m_nPage = 1 - m_nPage;
                    m_fPageTime = 0.0f;
                    m_nPageTurns++;
                }
                if (m_fTime >= 1200.0f) {
                    bEnd = 1;
                }
                if (m_fTime >= 60.0f) {
                    for (long i = 0; i < 4; i++) {
                        if (CPadWI::IsTrg(0x7F1F, i)) {
                            bEnd = 1;
                        }
                    }
                }
                if (bEnd) {
                    CMainApp::StartMasterFade(0, 1.0f / 30.0f);
                    m_nState++;
                }
                m_fTime += fRate;
                m_fPageTime += fRate;
                break;
            case 3:
                if (!CMainApp::IsPlayMasterFade()) {
                    Delete();
                }
                break;
        }
    }

    void CStrapTask::Disp()
    {
        if (m_pPalette == 0) {
            return;
        }
        CMainApp::ResetGP();

        // The whole texture.
        float fMin = 0.0f;
        float fMax = 1.0f;
        BCS_Color color;
        color.r = 0xFF;
        color.g = 0xFF;
        color.b = 0xFF;
        color.a = 0x80;
        unsigned short nScreenW = CMainApp::g_Rmode.fbWidth;
        unsigned short nScreenH = CMainApp::g_Rmode.efbHeight;
        Draw2D_FillRectAlpha(0, 0, nScreenW, nScreenH, color);

        color.r = 0x80;
        color.g = 0x80;
        color.b = 0x80;
        color.a = 0x80;
        long nW;
        long nH;
        if (!m_bWide) {
            nW = 0x260;
            nH = 0x1C8;
        } else {
            nW = 0x1C8;
            nH = 0x1C8;
        }
        GXTexObj texObj;
        TPLGetGXTexObjFromPalette(m_pPalette, &texObj, m_nPage);
        GXLoadTexObj(&texObj, 0);
        Draw2D_SetProjection(-1, 0);
        Draw2D_SetBlendNone();
        Draw2D_SetZModeOff();
        Draw2D_DrawTexRect((nScreenW - nW) / 2, (nScreenH - nH) / 2, nW, nH, color, fMin, fMin, fMax, fMax, 0, 0);
        CMainApp::ResetGP();
        nw4r::g3d::G3DState::Invalidate(0x7FF);
    }

    CHandleTable *CTaskMan::s_pHandleTable;
    nw4r::ut::List CTaskMan::s_TaskList[64];
    CTaskMan::PRIORITY_REQ *CTaskMan::s_pRequest;
    long CTaskMan::s_nRequestNum;

    void CTaskMan::Initialize()
    {
        s_pHandleTable = new (0) CHandleTable;
        s_pHandleTable->Init(0x1000, 0, 0x20);
        for (int i = 0; i < 64; i++) {
            nw4r::ut::List_Init(&s_TaskList[i], 0x14);
        }
        s_pRequest = new (0) PRIORITY_REQ[0x100];
        s_nRequestNum = 0;
    }

    void CTaskMan::Main(int bForce)
    {
        nw4r::ut::List *pList;
        int i;

        // Delete the tasks that asked for it, with their children.
        pList = s_TaskList;
        for (i = 0; i < 64; i++, pList++) {
            CTask *pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, 0));
            while (pTask != 0) {
                if (pTask->IsDelete()) {
                    pTask->DeleteAllChild();
                    CTask *pDelete = pTask;
                    pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, pTask));
                    delete pDelete;
                } else {
                    pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, pTask));
                }
            }
        }

        // Move the tasks to their new priority lists.
        for (long n = 0; n < s_nRequestNum; n++) {
            PRIORITY_REQ &rReq = s_pRequest[n];
            CTask *pTask = static_cast<CTask *>(s_pHandleTable->Get(rReq.nId));
            if (pTask == 0) {
                continue;
            }
            unsigned short bChild = rReq.bChild;
            short nPriority = rReq.nPriority;
            nw4r::ut::List_Remove(&s_TaskList[pTask->m_nPriority], pTask);
            nw4r::ut::List_Append(&s_TaskList[nPriority], pTask);
            pTask->m_nPriority = nPriority;
            if (bChild) {
                for (CTask *pChild = pTask->GetNextChild(0); pChild != 0; pChild = pTask->GetNextChild(pChild)) {
                    RequestPriority(pChild, nPriority, bChild);
                }
            }
        }
        s_nRequestNum = 0;

        // Run them.
        pList = s_TaskList;
        for (i = 0; i < 64; i++, pList++) {
            CTask *pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, 0));
            while (pTask != 0) {
                unsigned long nFlag = pTask->m_nFlag;
                if (!(nFlag & 1)) {
                    int bRun = 1;
                    if ((nFlag & 2) && !bForce) {
                        bRun = 0;
                    }
                    if (CMainApp::IsPause(pTask->m_Unk08)) {
                        bRun = 0;
                    }
                    if (bRun) {
                        pTask->Main();
                    }
                }
                pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, pTask));
            }
        }
    }

    void CTaskMan::Disp()
    {
        nw4r::ut::List *pList = s_TaskList;
        for (int i = 0; i < 64; i++, pList++) {
            CTask *pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, 0));
            while (pTask != 0) {
                if (!pTask->IsDelete()) {
                    pTask->Disp();
                }
                pTask = static_cast<CTask *>(nw4r::ut::List_GetNext(pList, pTask));
            }
        }
    }

    CTask *CTaskMan::GetTask(unsigned long id)
    {
        return static_cast<CTask *>(s_pHandleTable->Get(id));
    }

    CTask::CTask(CTask *pParent, short priority)
    {
        m_nId = CTaskMan::s_pHandleTable->Register(this);
        nw4r::ut::List_Init(&m_ChildList, 0x1C);
        m_Unk08 = 0xFCFF;
        m_nFlag = 2;
        m_pParent = pParent;

        if (priority < 0) {
            if (pParent == 0) {
                priority = 20;
            } else {
                priority = pParent->m_nPriority;
            }
        }
        m_nPriority = priority;

        if (m_pParent != 0) {
            nw4r::ut::List_Append(&m_pParent->m_ChildList, this);
        }
        nw4r::ut::List_Append(&CTaskMan::s_TaskList[m_nPriority], this);
    }

    CTask::~CTask()
    {
        DeleteAllChild();

        nw4r::ut::List_Remove(&CTaskMan::s_TaskList[m_nPriority], this);
        if (m_pParent != 0) {
            nw4r::ut::List_Remove(&m_pParent->m_ChildList, this);
        }
        CTaskMan::s_pHandleTable->Unregister(m_nId);
    }

    void CTask::DeleteAllChild()
    {
        CTask *pChild = GetNextChild(0);
        while (pChild != 0) {
            pChild->DeleteAllChild();
            CTask *pDelete = pChild;
            pChild = GetNextChild(pChild);
            delete pDelete;
        }
    }

    void CTaskMan::RequestPriority(CTask *pTask, short nPriority, unsigned short bChild)
    {
        if (s_nRequestNum >= 0x100) {
            return;
        }
        s_pRequest[s_nRequestNum].nId = pTask->GetId();
        s_pRequest[s_nRequestNum].nPriority = nPriority;
        s_pRequest[s_nRequestNum].bChild = bChild;
        s_nRequestNum++;
    }
}
