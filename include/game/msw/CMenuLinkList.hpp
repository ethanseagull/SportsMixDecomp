#ifndef CMENULINKLIST_HPP_
#define CMENULINKLIST_HPP_

#include "nw4r/ut/List.hpp"
#include "game/msw/CMenuLinkBase.hpp"

namespace msw {
    class CMenuLinkBase;

    // Size 0x10: an nw4r::ut::List of CMenuLinkBase objects, then the vptr
    // (the data member is declared before the first virtual function).
    // Embedded in msw::CMenuTask at 0x34; CMenuTask's constructor builds it
    // inline (vptr store, then List_Init with offset 0). Lives in the DOL.
    class CMenuLinkList {
        public:
            CMenuLinkList() { nw4r::ut::List_Init(&m_List, 0); }

            // The object with id nId after pStart (from the head when pStart
            // is null); null when there is none.
            CMenuLinkBase *FindObject(long nId, CMenuLinkBase *pStart);
            void RemoveObject(CMenuLinkBase *pObj);

            // (no symbol; name guessed) The object after pObj, the first
            // one when pObj is null.
            CMenuLinkBase *GetNextObject(const CMenuLinkBase *pObj) const
            {
                return static_cast<CMenuLinkBase *>(nw4r::ut::List_GetNext(&m_List, pObj));
            }
            void AppendObject(CMenuLinkBase *pObj) { nw4r::ut::List_Append(&m_List, pObj); }
            void UnlinkObject(CMenuLinkBase *pObj) { nw4r::ut::List_Remove(&m_List, pObj); }

        private:
            nw4r::ut::List m_List; // 0x00

        public:
            // vptr at 0x0C; the vtable has only the destructor. Deletes
            // every object. CMenuTask's destructor has it inline; the
            // out-of-line copy is fn_8013F668.
            virtual ~CMenuLinkList()
            {
                CMenuLinkBase *pObj = GetNextObject(0);
                while (pObj != 0) {
                    CMenuLinkBase *pNext = GetNextObject(pObj);
                    UnlinkObject(pObj);
                    delete pObj;
                    pObj = pNext;
                }
            }
    };
}

#endif /* !CMENULINKLIST_HPP_ */
