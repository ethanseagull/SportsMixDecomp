#ifndef BCS_CFSMQUEUE_HPP_
#define BCS_CFSMQUEUE_HPP_

#include "bcs/BCS_Util.hpp"
#include "bcs/CState.hpp"
#include "bcs/New.hpp"

namespace bcs {

    // State machine that runs a queue of states, size 0x60. Like bcs::CFsm
    // the data members come before the first virtual function, so the vptr
    // follows them, here at 0x5C. A class of its own, not a CFsm: the null
    // state sits at 0x30 instead of 0x18.
    //
    // Known from msw::CChrCore's constructor in the DOL (which builds this
    // base inline), from the instance functions BasketF emits out of line
    // (FsmqIsEmpty, FsmqGetState, GetState, FsmqGetWork_) and from
    // msw::CStgGmk_WaterPillar_Act (DOL), which builds, runs and destroys
    // one inline and emits the instances of CStateTemplate and the
    // destructor. Member names are guesses.
    template <class T>
    class CFsmQueue {
        public:
            // One entry per state, 0x2C bytes, as in bcs::CFsm.
            typedef CStateTemplate<T> State;
            typedef void (T::*StateFunc)();

            CFsmQueue()
            {
                m_pOwner = 0;
                // The null state's hooks are set again here, from one null
                // pointer to member (as in bcs::CFsm).
                typename CStateTemplate<CFsmQueue<T> >::StateFunc pNull = 0;
                m_NullState.Set(this, pNull, pNull, pNull);
                m_pStateTable = 0;
                m_pState = &m_NullState;
                m_pNextState = 0;
                m_nStateNum = 0;
                m_nFlag = 0;
                m_nQueueHead = 0;
                m_nQueueTail = 0;
                m_nQueueSize = 0;
                m_nWorkSize = 0;
                m_pStateQueue = 0;
                m_pWorkQueue = 0;
            }

        protected:
            T *m_pOwner;          // 0x00
            State *m_pStateTable; // 0x04, m_nStateNum entries
            CState *m_pState;     // 0x08, current state; starts at m_NullState
            CState *m_pNextState; // 0x0C, the state to enter next; null when none is pending
            long m_nStateNum;     // 0x10
            // 0x14: bit 0 is set when a state was entered this frame, bit 1
            // asks FsmqExec to run another round (FsmqRetry).
            unsigned long m_nFlag;
            long m_nQueueHead;    // 0x18, index of the state now running
            long m_nQueueTail;    // 0x1C, equal to the head when the queue is empty
            long m_nQueueSize;    // 0x20, entries of the queue (a ring)
            long m_nWorkSize;     // 0x24, bytes of work memory per queue entry
            long *m_pStateQueue;  // 0x28, state number of each queue entry
            void *m_pWorkQueue;   // 0x2C, work memory of each queue entry
            CStateTemplate<CFsmQueue<T> > m_NullState; // 0x30

        public:
            // The class's only virtual function (its vtable in the DOL is
            // three words). Frees the queues and the state table.
            virtual ~CFsmQueue()
            {
                if (m_pStateQueue) {
                    delete[] m_pStateQueue;
                }
                m_pStateQueue = 0;
                if (m_pWorkQueue) {
                    delete[] static_cast<char *>(m_pWorkQueue);
                }
                m_pWorkQueue = 0;
                if (m_pStateTable) {
                    delete[] m_pStateTable;
                }
                m_pStateTable = 0;
            }

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

            // (name guessed; inline in BasketF's
            // CChrManBasket::StartScoreView_) The number of the state queued
            // after the head; zero when there is none.
            long FsmqGetNextState() const
            {
                if (FsmqIsEmpty()) {
                    return 0;
                }
                long idx = FsmqWrap(m_nQueueHead + 1, m_nQueueSize);
                if (idx == m_nQueueTail) {
                    return 0;
                }
                return m_pStateQueue[idx];
            }

            // The work memory of the state at the head of the queue; null
            // when nothing is queued or the states have none.
            void *FsmqGetWork_() const
            {
                if (FsmqIsEmpty()) {
                    return 0;
                }
                if (!m_pWorkQueue || !m_nWorkSize) {
                    return 0;
                }
                return static_cast<char *>(m_pWorkQueue) + m_nWorkSize * m_nQueueHead;
            }

            template <class W>
            W *FsmqGetWork() const
            {
                return static_cast<W *>(FsmqGetWork_());
            }

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
                    m_pNextState = m_pStateTable;
                } else {
                    m_pNextState = &m_NullState;
                }
                m_nQueueTail = 0;
                m_nQueueHead = 0;
            }

            // Queues state with cleared work memory; it runs at once when
            // the queue was empty.
            // (The tail is read before the head: BasketF's
            // CGameBasketHungryBossPackun::LoopStateGameSetup loads them in
            // that order.)
            void FsmqPush(long state)
            {
                long tail = m_nQueueTail;
                int bEmpty = m_nQueueHead == tail;
                long next = FsmqWrap(tail + 1, m_nQueueSize);
                m_pStateQueue[tail] = state;
                m_nQueueTail = next;
                if (bEmpty && m_pStateTable) {
                    m_pNextState = &m_pStateTable[state];
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

            // (added for BasketF game/basket/CNpcGhostBasketBase.cpp; inline
            // there, out of line as FsmqSetState__Q23bcs35CFsmQueue<Q23msw16CNpcBehemothBoss>Fl
            // in BossF, which gives this overload its name) Drops the queue
            // and runs state, as FsmqChangeState; returns the state's
            // cleared work memory (null without one). Written on
            // FsmqPushState: BossF's CChrCoreBossMethod.cpp gets the original
            // register allocation only that way (CNpcGhostBasketBase does not
            // change).
            void *FsmqSetState(long state)
            {
                FsmqClear();
                return FsmqPushState(state);
            }

            // (name guessed; added for BossF game/boss/CChrCoreBossMethod.cpp,
            // where it is inline) Queues state after the others, as
            // FsmqPush, and returns its cleared work memory (null without
            // one). FsmqSetState is FsmqClear followed by this.
            void *FsmqPushState(long state)
            {
                int bEmpty = FsmqIsEmpty();
                long tail = m_nQueueTail;
                long next = FsmqWrap(tail + 1, m_nQueueSize);
                m_pStateQueue[tail] = state;
                m_nQueueTail = next;
                if (bEmpty && m_pStateTable) {
                    m_pNextState = &m_pStateTable[state];
                }
                void *pWork;
                if (m_pWorkQueue == 0 || m_nWorkSize == 0) {
                    pWork = 0;
                } else {
                    pWork = static_cast<char *>(m_pWorkQueue) + m_nWorkSize * tail;
                    BCS_CpuClear(pWork, m_nWorkSize);
                }
                return pWork;
            }

            // (name guessed; inline in BossF's CChrCoreBoss::MethdTimeOver)
            // Drops the states queued after the one now running.
            void FsmqCutQueue()
            {
                if (!FsmqIsEmpty()) {
                    m_nQueueTail = FsmqWrap(m_nQueueHead + 1, m_nQueueSize);
                }
            }

            // (name guessed; BasketF) The work memory of queue entry idx;
            // null when the states have none.
            void *FsmqGetWorkAt_(long idx) const
            {
                if (m_pWorkQueue == 0 || m_nWorkSize == 0) {
                    return 0;
                }
                return static_cast<char *>(m_pWorkQueue) + m_nWorkSize * idx;
            }

            // (name guessed; inline in BasketF's
            // CChrCoreBasketYoshi::LoopStateSmashCut) Ends the state at the
            // head of the queue: the next queued state is entered, or the
            // first of the table when the queue has run out.
            void FsmqNext()
            {
                if (!FsmqIsEmpty()) {
                    long head = FsmqWrap(m_nQueueHead + 1, m_nQueueSize);
                    m_nQueueHead = head;
                    if (head == m_nQueueTail) {
                        if (m_pStateTable) {
                            m_pNextState = m_pStateTable;
                        } else {
                            m_pNextState = &m_NullState;
                        }
                    } else {
                        m_pNextState = &m_pStateTable[m_pStateQueue[head]];
                    }
                }
            }

            // (name guessed; inline in BasketF's
            // CGameBasketHungryBossPackun::LoopStateGameSet) Drops the
            // states queued after the one at the head of the queue.
            void FsmqCutNext()
            {
                if (!FsmqIsEmpty()) {
                    m_nQueueTail = FsmqWrap(m_nQueueHead + 1, m_nQueueSize);
                }
            }

            // (name guessed) From a state's hook: lets FsmqExec carry the
            // change out in the same frame.
            void FsmqRetry() { m_nFlag |= 2; }

            // (name from the weak copy in BasketF's
            // game/basket/CChrCoreBasketNinja.cpp) FsmqNext, then FsmqRetry:
            // the next state is entered in the same frame.
            void FsmqDoChain()
            {
                FsmqNext();
                FsmqRetry();
            }

        protected:
            // (name guessed) Makes a table of nStateNum states run on pOwner
            // and a queue of nQueueSize entries with nWorkSize bytes of work
            // memory each.
            void FsmqInit(T *pOwner, long nStateNum, long nQueueSize, long nWorkSize)
            {
                m_pOwner = pOwner;
                m_pStateTable = new (0x100) State[nStateNum];
                m_nStateNum = nStateNum;
                m_nFlag = 0;
                m_nQueueHead = 0;
                m_nQueueTail = 0;
                m_pStateQueue = new (0x100) long[nQueueSize];
                m_nQueueSize = nQueueSize;
                BCS_CpuClear(m_pStateQueue, nQueueSize * sizeof(long));
                m_pWorkQueue = new (0x100) char[nQueueSize * nWorkSize];
                m_nWorkSize = nWorkSize;
                BCS_CpuClear(m_pWorkQueue, nQueueSize * nWorkSize);
                FsmqClear();
            }

            // (name guessed) Sets the three hooks of state nState.
            void FsmqSetState(long nState, StateFunc pCtor, StateFunc pLoop, StateFunc pDtor)
            {
                m_pStateTable[nState].Set(m_pOwner, pCtor, pLoop, pDtor);
            }

            // (name guessed) One frame of the state machine, called from the
            // owner's update (msw::CStgGmk_WaterPillar_Act; inline in
            // BasketF's CNpcGhostBasketBase::MainFunc): enters a pending
            // state (leaving the current one), then runs the current state's
            // loop unless a change is pending again; repeats while a hook
            // asked for it (FsmqRetry).
            void FsmqExec()
            {
                do {
                    m_nFlag &= ~3;
                    if (m_pNextState) {
                        m_pState->DtorState();
                        m_pState = m_pNextState;
                        m_pNextState = 0;
                        m_nFlag |= 1;
                        m_pState->CtorState();
                    }
                    if (!m_pNextState && !(m_nFlag & 2)) {
                        m_pState->LoopState();
                    }
                } while (m_nFlag & 2);
            }
    };
}

#endif /* !BCS_CFSMQUEUE_HPP_ */
