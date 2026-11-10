#ifndef CMENULINKLIST_HPP_
#define CMENULINKLIST_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {
    class CMenuLinkBase;

    // Size 0x10: an nw4r::ut::List of CMenuLinkBase objects, then the vptr
    // (the data member is declared before the first virtual function).
    // Embedded in msw::CMenuTask at 0x34; CMenuTask's constructor builds it
    // inline (vptr store, then List_Init with offset 0). Lives in the DOL.
    class CMenuLinkList {
        public:
            // The object with id nId after pStart (from the head when pStart
            // is null); null when there is none.
            CMenuLinkBase *FindObject(long nId, CMenuLinkBase *pStart);
            void RemoveObject(CMenuLinkBase *pObj);

        private:
            nw4r::ut::List m_List; // 0x00

        public:
            // vptr at 0x0C; the vtable has only the destructor.
            virtual ~CMenuLinkList();
    };
}

#endif /* !CMENULINKLIST_HPP_ */
