#ifndef CMENUTASK_HPP_
#define CMENUTASK_HPP_

#include "bcs/CTask.hpp"
#include "game/msw/CMenuLinkList.hpp"

namespace msw {
    class CMenuLinkBase;

    // A task that keeps a list of CMenuLinkBase objects. Lives in the DOL.
    // It adds no virtual function to bcs::CTask's four (its vtable has four
    // entries). Size 0x44: msw::CCockpitPause has its second base,
    // bcs::CFsm<CCockpitPause>, at 0x44 (the `@68@` thunks in BossF).
    class CMenuTask : public bcs::CTask {
        public:
            CMenuTask(bcs::CTask *pParent);
            virtual ~CMenuTask();

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

        private:
            CMenuLinkList m_LinkList; // 0x34
    };
}

#endif /* !CMENUTASK_HPP_ */
