#ifndef CMENUTASK_HPP_
#define CMENUTASK_HPP_

#include "bcs/CTask.hpp"
#include "game/msw/CMenuLinkList.hpp"

namespace msw {
    class CMenuLinkBase;

    // A task that keeps a list of CMenuLinkBase objects. Lives in the DOL.
    // It adds no virtual function to bcs::CTask's four (its vtable has four
    // entries); Main and Disp are overridden with empty inline bodies
    // (weak copies after SetPauseObj in the DOL). Size 0x44: msw::CCockpitPause has its second base,
    // bcs::CFsm<CCockpitPause>, at 0x44 (the `@68@` thunks in BossF).
    class CMenuTask : public bcs::CTask {
        public:
            CMenuTask(bcs::CTask *pParent);
            virtual ~CMenuTask();
            virtual void Main() {}
            virtual void Disp() {}

            // Each looks up the CMenuLinkBase with id nObjId in the list
            // and calls one of its virtuals (read from the DOL bodies).
            // SendEvent's result, or 0xFFFF0001 when there is no such object.
            long SendObjEventTrigger(long nObjId, long nEvent, void *pParam);
            // Set()'s result, or -1.
            long Build(long nObjId);
            // Calls Remove(); 0, or -1.
            long Scratch(long nObjId);
            // Appends pObj to the list and makes this task its owner; -1 when
            // pObj is null, else 0.
            long AddObj(CMenuLinkBase *pObj);

            // (name guessed; inline, no symbol) The object with id nObjId,
            // or null.
            CMenuLinkBase *FindObj(long nObjId) { return m_LinkList.FindObject(nObjId, 0); }
            // Run Main / Disp of every object in the list.
            void ModListMainLoop();
            void ModListDispLoop();

            // (no symbols; names from their callers CCockpit::IsEndObj and
            // CCockpit::RemoveObj) 1 when no object has id nObjId; marks
            // every object with that id for deletion and returns how many.
            int IsEndObj(long nObjId);
            long RemoveObj(long nObjId);
            // (no symbol, stripped; name guessed) Calls SetVisible(bVisible);
            // 0, or -1.
            long SetVisibleObj(long nObjId, int bVisible);
            // Adds 1 to (bPause) or takes 1 from (not below 0) every
            // object's pause count.
            void SetPauseObj(int bPause);

            // (added for MenuF game/menu/CSelectMenu.cpp; inline, emitted
            // there) SendObjEventTrigger without a parameter.
            long SendObjEvent(long nObjId, long nEvent) { return SendObjEventTrigger(nObjId, nEvent, 0); }

        protected:
            // (protected for MenuF game/menu/CSelectMenu.cpp, whose
            // RemoveObj takes the object out of the list itself)
            CMenuLinkList m_LinkList; // 0x34
    };
}

#endif /* !CMENUTASK_HPP_ */
