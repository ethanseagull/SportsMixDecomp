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
            // (bcs::CStateTemplate<bcs::CFsm<T> >); it is opaque here until
            // a unit needs its members.
            struct State {
                unsigned char m_Unk00[0x2C];
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
    };
}

#endif /* !BCS_CFSM_HPP_ */
