#ifndef BCS_CFSM_HPP_
#define BCS_CFSM_HPP_

namespace bcs {

    // State machine mixin. Size 0x48 is inferred, not proven: in
    // msw::CMainStep this subobject starts at 0x34, its vptr is at 0x78 and
    // the next member is at 0x7C, which gives 0x44 bytes of data and then the
    // vptr. The data members are declared before the first virtual function
    // on the assumption that the compiler then puts the vptr after them.
    // Field names are guesses; the offsets come from ChangeState.
    //
    // The inline bodies of ~CFsm and ChangeState below have not been compiled
    // against the original yet.
    template <class T>
    class CFsm {
        public:
            // One entry per state, 0x2C bytes. Contents not mapped yet.
            struct State {
                unsigned char m_Unk00[0x2C];
            };

        protected:
            long m_Unk00;                // 0x00
            State *m_pStateTable;        // 0x04
            long m_Unk08;                // 0x08
            State *m_pState;             // 0x0C
            long m_nState;               // 0x10
            unsigned char m_Unk14[0x30]; // 0x14

        public:
            virtual ~CFsm() {}
            // Returns the state that was current before the call.
            virtual long ChangeState(long state)
            {
                long prev = m_nState;
                m_nState = state;
                m_pState = &m_pStateTable[state];
                return prev;
            }
            virtual void ChangeStateProc() = 0;
    };
}

#endif /* !BCS_CFSM_HPP_ */
