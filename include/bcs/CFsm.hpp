#ifndef BCS_CFSM_HPP_
#define BCS_CFSM_HPP_

#include "bcs/CState.hpp"
#include "bcs/New.hpp"

namespace bcs {

    // State machine mixin, size 0x48. The data members are declared before
    // the first virtual function, so the vptr follows them at 0x44.
    //
    // Offsets come from msw::CMainStep's constructor, destructor and Main in
    // the DOL; the member names are guesses. The inline bodies (constructor,
    // destructor, ExecState, InitFsm, SetState) are read from TourF's
    // msw::CCockpitTournament, which builds its CFsm inline.
    template <class T>
    class CFsm {
        public:
            // One entry per state, 0x2C bytes.
            typedef CStateTemplate<T> State;
            typedef void (T::*StateFunc)();

            CFsm()
            {
                m_pOwner = 0;
                // The null state's hooks are set again here, from one null
                // pointer to member (the original keeps a stack copy of it).
                typename CStateTemplate<CFsm<T> >::StateFunc pNull = 0;
                m_NullState.Set(this, pNull, pNull, pNull);
                m_pStateTable = 0;
                m_pState = &m_NullState;
                m_pNextState = 0;
                m_nState = -1;
                m_nStateNum = 0;
            }

        protected:
            T *m_pOwner;          // 0x00
            State *m_pStateTable; // 0x04, array of m_nStateNum entries
            CState *m_pState;     // 0x08, current state
            CState *m_pNextState; // 0x0C, null when no change is pending
            long m_nState;        // 0x10
            long m_nStateNum;     // 0x14
            CStateTemplate<CFsm<T> > m_NullState; // 0x18, current state until the first change

        public:
            // Frees the state table. Inline in the original (the DOL has it
            // inside ~CMainStep).
            virtual ~CFsm()
            {
                if (m_pStateTable != 0) {
                    delete[] m_pStateTable;
                }
                m_pStateTable = 0;
            }
            // Requests a change and returns the state that was current
            // before the call. The owner's Main carries the change out.
            virtual long ChangeState(long state)
            {
                long prev = m_nState;
                m_nState = state;
                m_pNextState = &m_pStateTable[state];
                return prev;
            }
            // Called by the owner between leaving one state and entering the
            // next. Empty here: the DOL has the body CFsm<msw::CMainStep>
            // would use, a single blr.
            virtual void ChangeStateProc() {}

        protected:
            // (name guessed) Makes a table of nNum states that run on pOwner.
            void InitFsm(T *pOwner, long nNum)
            {
                m_pOwner = pOwner;
                m_pStateTable = new (0) State[nNum];
                m_nStateNum = nNum;
            }

            // (name guessed) Sets the three hooks of state nState.
            void SetState(long nState, StateFunc pCtor, StateFunc pLoop, StateFunc pDtor)
            {
                m_pStateTable[nState].Set(m_pOwner, pCtor, pLoop, pDtor);
            }

            // (name guessed) One frame of the state machine, called from the
            // owner's Main: carries out a pending change, then runs the
            // current state's loop unless a change is pending again.
            void ExecState()
            {
                if (m_pNextState != 0) {
                    m_pState->DtorState();
                    ChangeStateProc();
                    m_pState = m_pNextState;
                    m_pNextState = 0;
                    m_pState->CtorState();
                }
                if (m_pNextState == 0) {
                    m_pState->LoopState();
                }
            }
    };
}

#endif /* !BCS_CFSM_HPP_ */
