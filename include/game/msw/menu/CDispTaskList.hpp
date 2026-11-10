#ifndef MENU_CDISPTASKLIST_HPP_
#define MENU_CDISPTASKLIST_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {
    class CMenuPaneAccessor;

    namespace menu {

        // One display task in a CDispTaskList. Lives in the DOL. Size 0x30:
        // the list link at 0x00, own members from 0x08, vptr at 0x2C.
        // CDispTaskList::StatusCheck calls StatusCheck while m_bEnd is 0 and
        // deletes the task once it is set; DispLoop calls Disp and counts
        // m_nFrame. The two virtuals are empty in the DOL. Names are guesses.
        class CDispTaskLink {
            public:
                CDispTaskLink();

            private:
                // Ids are handed out from 1 and never 0. (name guessed)
                static unsigned long s_nNextId;

                nw4r::ut::Link m_Link;  // 0x00
                unsigned long m_nKind;  // 0x08
                unsigned long m_nId;    // 0x0C
                long m_bEnd;            // 0x10
                long m_nFrame;          // 0x14, counted by DispLoop
                long m_Unk18;           // 0x18
                long m_Unk1C;           // 0x1C
                bcs::FVEC3 m_Pos;       // 0x20

            public:
                virtual ~CDispTaskLink();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long);
        };

        // Size 0x10, vptr at 0x0C. Lives in the DOL. Nothing of it is used
        // yet; it is here for the layout of the classes that hold one.
        class CDispTaskList {
            private:
                unsigned char m_Unk00[0xC]; // 0x00: a nw4r::ut::List

            public:
                CDispTaskList();
                virtual ~CDispTaskList();
        };
    }
}

#endif /* !MENU_CDISPTASKLIST_HPP_ */
