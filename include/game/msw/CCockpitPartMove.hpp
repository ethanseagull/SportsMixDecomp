#ifndef MSW_CCOCKPITPARTMOVE_HPP_
#define MSW_CCOCKPITPARTMOVE_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {

    // A queue of cockpit part moves. Lives in the DOL as one static
    // instance (s_Instance, 0x10 bytes: the list, then the vptr at 0x0C;
    // the static initialiser stores only the vptr and registers the
    // destructor). Push allocates an entry and appends it; Pop copies the
    // first entry out and frees it. What the three values mean is not
    // known; names are guesses.
    class CCockpitPartMove {
        public:
            struct PartMoveInfo {
                nw4r::ut::Link m_Link; // 0x00
                long m_nPart;          // 0x08
                long m_nArg0;          // 0x0C
                long m_nArg1;          // 0x10
            };

            // 0 on success, -1 when no entry could be allocated.
            long Push(long nPart, long nArg0, long nArg1);
            // 0 and the first entry in rInfo, -1 when the queue is empty.
            long Pop(PartMoveInfo &rInfo);

            static CCockpitPartMove s_Instance;

        private:
            nw4r::ut::List m_List; // 0x00

        public:
            // vptr at 0x0C
            virtual ~CCockpitPartMove();
    };
}

#endif /* !MSW_CCOCKPITPARTMOVE_HPP_ */
