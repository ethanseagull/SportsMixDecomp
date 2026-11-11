#include "game/msw/menu/CDispTaskList.hpp"

namespace msw {
    namespace menu {

        // One original file with CDispTaskLink: CDispTaskList's vtable comes
        // before CDispTaskLink's in .data while the code is the other way round.

        unsigned long CDispTaskLink::s_nNextId = 1;

        CDispTaskLink::CDispTaskLink()
        {
            m_nKind = 0;
            unsigned long id = s_nNextId++;
            if (s_nNextId == 0) {
                s_nNextId = 1;
            }
            m_nId = id;
            m_bEnd = 1;
            m_Unk1C = 0;
            m_Unk18 = 0;
            m_Pos = bcs::FVEC3(0.0f, 0.0f, 0.0f);
        }

        CDispTaskLink::~CDispTaskLink()
        {
        }

        void CDispTaskLink::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
        }

        void CDispTaskLink::Disp(CMenuPaneAccessor *pAccessor, long)
        {
        }

        CDispTaskList::CDispTaskList()
        {
            nw4r::ut::List_Init(&m_List, 0);
        }

        CDispTaskList::~CDispTaskList()
        {
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pLink != 0) {
                CDispTaskLink *pNext = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
                nw4r::ut::List_Remove(&m_List, pLink);
                delete pLink;
                pLink = pNext;
            }
        }

        void CDispTaskList::PushBack(CDispTaskLink *pLink)
        {
            if (pLink != 0) {
                nw4r::ut::List_Append(&m_List, pLink);
            }
        }

        // Runs every live task's StatusCheck, and deletes the tasks that
        // have ended.
        void CDispTaskList::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
            CDispTaskLink *pNext;
            if (pAccessor == 0) {
                return;
            }
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pLink != 0) {
                pNext = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
                if (!pLink->m_bEnd) {
                    pLink->StatusCheck(pAccessor);
                }
                if (pLink->m_bEnd) {
                    nw4r::ut::List_Remove(&m_List, pLink);
                    delete pLink;
                }
                pLink = pNext;
            }
        }

        // Draws every live task and counts its frames.
        void CDispTaskList::DispLoop(CMenuPaneAccessor *pAccessor, long nArg)
        {
            if (pAccessor == 0) {
                return;
            }
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pLink != 0) {
                CDispTaskLink *pNext = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
                if (!pLink->m_bEnd) {
                    pLink->Disp(pAccessor, nArg);
                    pLink->m_nFrame++;
                }
                pLink = pNext;
            }
        }

        void CDispTaskList::RemoveAll()
        {
            CDispTaskLink *pNext;
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pLink != 0) {
                pNext = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
                nw4r::ut::List_Remove(&m_List, pLink);
                delete pLink;
                pLink = pNext;
            }
        }

        void CDispTaskList::RemoveObject(const CDispTaskLink *pObj)
        {
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pLink != 0) {
                if (pLink == pObj) {
                    nw4r::ut::List_Remove(&m_List, pLink);
                    delete pLink;
                    break;
                }
                pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
            }
        }

        // The first task after pPrev (from the start when null) of kind
        // nKind, or null; also null when that task has ended.
        CDispTaskLink *CDispTaskList::FindNextKindObj(unsigned long nKind, CDispTaskLink *pPrev)
        {
            CDispTaskLink *pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pPrev));
            while (pLink != 0) {
                if (nKind == pLink->m_nKind) {
                    if (pLink->m_bEnd) {
                        return 0;
                    }
                    return pLink;
                }
                pLink = static_cast<CDispTaskLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
            }
            return 0;
        }
    }
}
