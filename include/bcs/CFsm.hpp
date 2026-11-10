#ifndef BCS_CFSM_HPP_
#define BCS_CFSM_HPP_

namespace bcs {

    // State machine mixin, size 0x48. The data members are declared before
    // the first virtual function, so the vptr follows them at 0x44.
    //
    // Offsets come from msw::CMainStep's constructor, destructor and Main in
    // the DOL; the member names are guesses.
    template <class T>
    class CFsm {
        public:
            // One entry per state, 0x2C bytes: a vptr, the owner and three
            // pointers to member functions (constructor, loop, destructor of
            // the state). The original is a class template of its own
            // (bcs::CStateTemplate<bcs::CFsm<T> >); only its virtual
            // functions are written here. The vtable (msw::CCockpitPause's
            // null state, lbl_80480528 in the DOL) is the destructor and
            // then the three below, which FsmUpdate calls; their names are
            // guesses after CMainStep's CtorState/LoopState/DtorState.
            struct State {
                virtual ~State();
                virtual void Ctor(); // entering the state
                virtual void Loop(); // once per frame while no change is pending
                virtual void Dtor(); // leaving the state

                unsigned char m_Unk04[0x28]; // owner, three member pointers
            };

        protected:
            T *m_pOwner;          // 0x00
            State *m_pStateTable; // 0x04, array of m_nStateNum entries
            State *m_pState;      // 0x08, current state
            State *m_pNextState;  // 0x0C, null when no change is pending
            long m_nState;        // 0x10
            long m_nStateNum;     // 0x14
            State m_NullState;    // 0x18, current state until the first change

        public:
            // Frees the state table. Inline in the original (the DOL has it
            // inside ~CMainStep); the body is not written yet because it
            // needs the real state class, so a unit that destroys a CFsm
            // cannot be compiled from this header.
            virtual ~CFsm();
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
            // (name guessed; inline, no symbol) Carries out a pending change
            // (leave the old state, ChangeStateProc, enter the new one), then
            // runs the current state's loop unless another change is pending.
            // msw::CMainStep::Main in the DOL is exactly this;
            // msw::CCockpitBossPause::Main (BossF) ends with it.
            void FsmUpdate()
            {
                if (m_pNextState) {
                    m_pState->Dtor();
                    ChangeStateProc();
                    m_pState = m_pNextState;
                    m_pNextState = 0;
                    m_pState->Ctor();
                }
                if (!m_pNextState) {
                    m_pState->Loop();
                }
            }
    };
}

#endif /* !BCS_CFSM_HPP_ */
