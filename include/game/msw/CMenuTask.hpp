#ifndef CMENUTASK_HPP_
#define CMENUTASK_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // A task that keeps a list of CMenuLinkBase objects. Lives in the DOL.
    // It adds no virtual function to bcs::CTask's four (its vtable has four
    // entries). Nothing of it is used yet; the size is the offset of
    // CCockpit's first own member and may include padding.
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

        private:
            unsigned char m_Unk34[0xC]; // 0x34: a nw4r::ut::List
            unsigned char m_Unk40[0xC]; // 0x40: starts with a vptr
    };
}

#endif /* !CMENUTASK_HPP_ */
