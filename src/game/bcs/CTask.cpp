// game/bcs/CTask
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CHandleTable.hpp"

namespace bcs {

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
}
