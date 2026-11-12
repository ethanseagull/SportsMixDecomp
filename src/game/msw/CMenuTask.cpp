#include "game/msw/CMenuTask.hpp"

#include "game/msw/CMenuLinkBase.hpp"

namespace msw {

    // CMenuLinkList::FindObject(nObjId, pStart). The original has that function
    // inlined into every lookup below (it was most likely in the same
    // source file as this class, which -ipa file lets it inline); the
    // CMenuLinkList declaration here only has the out-of-line copy, so the
    // body is repeated.
    static CMenuLinkBase *FindLinkObj(const CMenuLinkList *pList, long nObjId, CMenuLinkBase *pStart = 0)
    {
        for (CMenuLinkBase *pObj = pList->GetNextObject(pStart); pObj != 0; pObj = pList->GetNextObject(pObj)) {
            if (nObjId == pObj->m_nId) {
                return pObj;
            }
        }
        return 0;
    }

    CMenuTask::CMenuTask(bcs::CTask *pParent)
        : bcs::CTask(pParent, -1)
    {
    }

    CMenuTask::~CMenuTask()
    {
    }

    void CMenuTask::ModListMainLoop()
    {
        CMenuLinkBase *pObj;
        CMenuLinkList &list = m_LinkList;
        pObj = list.GetNextObject(0);
        while (pObj != 0) {
            if (pObj->m_Unk14 == 0) {
                pObj->Main();
            }
            CMenuLinkBase *pNext = list.GetNextObject(pObj);
            if (pObj->m_Unk08 == 1) {
                pObj->m_Unk08 = 2;
            } else if (pObj->m_Unk08 == 2) {
                list.UnlinkObject(pObj);
                delete pObj;
            }
            pObj = pNext;
        }
    }

    void CMenuTask::ModListDispLoop()
    {
        CMenuLinkBase *pObj;
        CMenuLinkList &list = m_LinkList;
        pObj = list.GetNextObject(0);
        while (pObj != 0) {
            CMenuLinkBase *pNext = list.GetNextObject(pObj);
            pObj->Disp();
            pObj = pNext;
        }
    }

    long CMenuTask::AddObj(CMenuLinkBase *pObj)
    {
        if (pObj == 0) {
            return -1;
        }
        CMenuLinkList *pList = &m_LinkList;
        if (pObj != 0) {
            pList->AppendObject(pObj);
            pObj->m_Unk10 = this;
        }
        return 0;
    }

    int CMenuTask::IsEndObj(long nObjId)
    {
        CMenuLinkList *pList = &m_LinkList;
        if (FindLinkObj(pList, nObjId) == 0) {
            return 1;
        }
        return 0;
    }

    long CMenuTask::RemoveObj(long nObjId)
    {
        CMenuLinkList *pList;
        CMenuLinkList *pLoopList;
        long nCount = 0;
        pList = &m_LinkList;
        CMenuLinkBase *pObj = FindLinkObj(pList, nObjId);
        if (pObj != 0) {
            pLoopList = &m_LinkList;
            while (pObj != 0) {
                if (pObj->m_Unk08 != 2) {
                    pObj->m_Unk08 = 2;
                    nCount++;
                    // So the next lookup no longer finds it.
                    pObj->m_nId = -pObj->m_nId;
                }
                pObj = FindLinkObj(pLoopList, nObjId, pObj);
            }
        }
        return nCount;
    }

    long CMenuTask::SendObjEventTrigger(long nObjId, long nEvent, void *pParam)
    {
        CMenuLinkList *pList = &m_LinkList;
        CMenuLinkBase *pObj = FindLinkObj(pList, nObjId);
        if (pObj == 0) {
            return 0xFFFF0001;
        }
        return pObj->SendEvent(nEvent, pParam);
    }

    long CMenuTask::Build(long nObjId)
    {
        CMenuLinkList *pList = &m_LinkList;
        CMenuLinkBase *pObj = FindLinkObj(pList, nObjId);
        if (pObj == 0) {
            return -1;
        }
        return pObj->Set();
    }

    long CMenuTask::Scratch(long nObjId)
    {
        CMenuLinkList *pList = &m_LinkList;
        CMenuLinkBase *pObj = FindLinkObj(pList, nObjId);
        if (pObj == 0) {
            return -1;
        }
        pObj->Remove();
        return 0;
    }

    // (no symbol; name and body guessed) Unused, so the original link
    // dropped it; what is left of it is CMenuLinkBase::SetVisible's weak
    // copy, which the DOL has here, between Scratch and SetPauseObj.
    long CMenuTask::SetVisibleObj(long nObjId, int bVisible)
    {
        CMenuLinkList *pList = &m_LinkList;
        CMenuLinkBase *pObj = FindLinkObj(pList, nObjId);
        if (pObj == 0) {
            return -1;
        }
        pObj->SetVisible(bVisible);
        return 0;
    }

    void CMenuTask::SetPauseObj(int bPause)
    {
        CMenuLinkList &list = m_LinkList;
        for (CMenuLinkBase *pObj = list.GetNextObject(0); pObj != 0; pObj = list.GetNextObject(pObj)) {
            long nAdd = bPause ? 1 : -1;
            long nCount = nAdd + pObj->m_Unk14;
            if (nCount < 0) {
                nCount = 0;
            }
            pObj->m_Unk14 = nCount;
        }
    }
}
