#ifndef BCS_CFSMQUEUE_HPP_
#define BCS_CFSMQUEUE_HPP_

#include "bcs/BCS_Util.hpp"

namespace bcs {

    // State machine that runs a queue of states, size 0x60. Like bcs::CFsm
    // the data members come before the first virtual function, so the vptr
    // follows them, here at 0x5C. A class of its own, not a CFsm: the null
    // state sits at 0x30 instead of 0x18.
    //
    // Known from msw::CChrCore's constructor in the DOL (which builds this
    // base inline) and from the instance functions BasketF emits out of line
    // (FsmqIsEmpty, FsmqGetState, GetState, FsmqGetWork_). Member names are
    // guesses; members marked Unk are only known to be cleared by that
    // constructor.
    template <class T>
    class CFsmQueue {
        public:
            // One entry per state, 0x2C bytes, as in bcs::CFsm; opaque here
            // for the same reason.
            struct State {
                unsigned char m_Unk00[0x2C];
            };

        protected:
            T *m_pOwner;          // 0x00 (guessed by analogy with CFsm)
            State *m_pStateTable; // 0x04 (guessed by analogy with CFsm)
            State *m_pState;      // 0x08, current state; starts at m_NullState
            State *m_pRunState;   // 0x0C, the state at the head of the queue (name guessed)
            unsigned long m_Unk10;
            unsigned long m_Unk14;
            long m_nQueueHead;    // 0x18, index of the state now running
            long m_nQueueTail;    // 0x1C, equal to the head when the queue is empty
            long m_nQueueSize;    // 0x20, entries of the queue (a ring)
            long m_nWorkSize;     // 0x24, bytes of work memory per queue entry
            long *m_pStateQueue;  // 0x28, state number of each queue entry
            void *m_pWorkQueue;   // 0x2C, work memory of each queue entry
            State m_NullState;    // 0x30

        public:
            // The class's only virtual function (its vtable in the DOL is
            // three words). Not written yet, for the reason given at
            // bcs::CFsm's destructor.
            virtual ~CFsmQueue();

            int FsmqIsEmpty() const { return m_nQueueHead == m_nQueueTail; }

            // The number of the state at the head of the queue; zero when
            // nothing is queued.
            long FsmqGetState() const
            {
                if (FsmqIsEmpty()) {
                    return 0;
                }
                return m_pStateQueue[m_nQueueHead];
            }

            long GetState() const { return FsmqGetState(); }

            // (names guessed) Inline in msw::CStgGmk_WaterPillar's Activate
            // and Inactivate, which run them on the actor's queue (DOL).

            // idx brought into [0, num).
            static long FsmqWrap(long idx, long num)
            {
                if (idx >= num) {
                    return idx % num;
                }
                if (idx < 0) {
                    return idx + num;
                }
                return idx;
            }

            // Empties the queue.
            void FsmqClear()
            {
                if (m_pStateTable) {
                    m_pRunState = m_pStateTable;
                } else {
                    m_pRunState = &m_NullState;
                }
                m_nQueueTail = 0;
                m_nQueueHead = 0;
            }

            // Queues state with cleared work memory; it runs at once when
            // the queue was empty.
            void FsmqPush(long state)
            {
                int bEmpty = FsmqIsEmpty();
                long tail = m_nQueueTail;
                long next = FsmqWrap(tail + 1, m_nQueueSize);
                m_pStateQueue[tail] = state;
                m_nQueueTail = next;
                if (bEmpty && m_pStateTable) {
                    m_pRunState = &m_pStateTable[state];
                }
                if (m_pWorkQueue && m_nWorkSize) {
                    BCS_CpuClear(static_cast<char *>(m_pWorkQueue) + m_nWorkSize * tail, m_nWorkSize);
                }
            }

            // Drops the queue and runs state.
            void FsmqChangeState(long state)
            {
                FsmqClear();
                FsmqPush(state);
            }
    };
}

#endif /* !BCS_CFSMQUEUE_HPP_ */
