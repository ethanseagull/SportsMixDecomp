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

                // (names guessed; no symbols) Inline in the modules: BasketF's
                // CCockpitBasketHarapeko fills a new task through them before
                // CDispTaskList::PushBack.
                void SetKind(unsigned long nKind) { m_nKind = nKind; }
                void SetPane(long nPane) { m_Unk18 = nPane; }
                void SetPos(const bcs::FVEC3 &rPos) { m_Pos = rPos; }

            private:
                // Ids are handed out from 1 and never 0. (name guessed)
                static unsigned long s_nNextId;

            public:
                // Public: MenuF's CChrSign::SetSign and CChrCaption::SetCaption
                // fill the fields of the tasks they create directly. The
                // SelectMenu sign and caption tasks keep a pane index in
                // m_Unk18 and their step in m_Unk1C.
                nw4r::ut::Link m_Link;  // 0x00
                unsigned long m_nKind;  // 0x08, what CDispTaskList::FindNextKindObj looks for
                unsigned long m_nId;    // 0x0C
                long m_bEnd;            // 0x10
                long m_nFrame;          // 0x14, counted by DispLoop
                long m_Unk18;           // 0x18, a pane index in the derived tasks seen so far
                long m_Unk1C;           // 0x1C
                bcs::FVEC3 m_Pos;       // 0x20

            public:
                virtual ~CDispTaskLink();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long);
        };

        // Size 0x10, vptr at 0x0C. Lives in the DOL
        // (src/game/msw/menu/CDispTaskList.cpp): an owning list of
        // CDispTaskLink; removing a task deletes it.
        class CDispTaskList {
            private:
                nw4r::ut::List m_List; // 0x00

            public:
                CDispTaskList();
                virtual ~CDispTaskList();

                // Added for HockeyF game/hockey/CCockpitHockeyCoin.cpp (see
                // CDispTaskLink above; return types guessed).
                void StatusCheck(CMenuPaneAccessor *pAccessor);
                void DispLoop(CMenuPaneAccessor *pAccessor, long nArg);

                // Added for MenuF game/menu/CChrSign.cpp (return types
                // guessed). Appends pLink; deletes every task; the first task
                // after pPrev (from the start when null) whose kind is nKind,
                // or null.
                void PushBack(CDispTaskLink *pLink);
                void RemoveAll();
                CDispTaskLink *FindNextKindObj(unsigned long nKind, CDispTaskLink *pPrev);
                void RemoveObject(const CDispTaskLink *pLink);
        };
    }
}

#endif /* !MENU_CDISPTASKLIST_HPP_ */
