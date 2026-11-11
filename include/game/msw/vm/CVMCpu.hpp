#ifndef MSW_VM_CVMCPU_HPP_
#define MSW_VM_CVMCPU_HPP_

#include "game/msw/vm/CVMData.hpp"

namespace msw {
    namespace vm {

        // INCOMPLETE: only what game/msw/vm/CVMCpu.cpp needs; the
        // constructor and Run are not declared yet.
        //
        // The event script VM's processor (DOL): a code pointer into a
        // CVMData image and a word stack. vptr at 0. Its vtable
        // (__vt__Q33msw2vm6CVMCpu, 4 slots) sits after CVMScript's, and the
        // destructor after CVMScript's code, so this unit does not emit it;
        // CVMScript overrides each slot (OpSysCall and UpdateBind are named
        // there).
        // One word of the VM stack: an integer, a float or four packed
        // bytes. The empty constructor is why `new STACK_DATA[n]` keeps an
        // array cookie (TourF CTmScript's constructor and destructor); it
        // is declared outside CVMCpu because nested in the class the
        // compiler passes the constructor to __construct_new_array, which
        // the original does not. (name guessed)
        union STACK_DATA {
            STACK_DATA() {}

            long i;
            float f;
            unsigned char b[4];
        };

        class CVMCpu {
            public:
                // Inline: TourF msw::CTmScript's constructor shows it. The
                // code pointer and the words from 0x1C are left alone.
                CVMCpu(unsigned long nStackSize)
                {
                    m_pData = 0;
                    m_nFP = 0;
                    m_nSP = 0;
                    m_nUnk24 = 0;
                    m_nStackSize = nStackSize;
                    m_pStack = new STACK_DATA[nStackSize];
                }

                // DOL fn_801FADA4: returns 3. Not inline: it is the class's
                // key function, which keeps the vtable (and the weak copies
                // of the inline virtuals) out of TourF, where
                // msw::CTmScript's constructor and destructor refer to it.
                virtual long OpSysCall();
                // DOL fn_801FB58C (after CVMScript's code, a weak copy): frees
                // the stack and clears the pointer. Inline: TourF
                // msw::CTmScript's destructor inlines it.
                virtual ~CVMCpu()
                {
                    if (m_pStack != 0) {
                        delete[] m_pStack;
                    }
                    m_pStack = 0;
                }
                // DOL fn_801FB06C, amid CVMScript's code: empty.
                virtual void UpdateBind() {}
                // DOL fn_801FAE18: empty; the first thing Reset calls.
                // Defined in CVMCpu.cpp, between Reset and SetCall. The name
                // is msw::CTmScript's override of the same slot (TourF).
                virtual void ResetBind();

                void Reset();
                // Pushes a call frame (nArg, the new frame pointer, -1) and jumps
                // to function pName of group nGroup. With nState other than
                // -1 the state becomes nState. Returns 0 and clears the code
                // pointer when the function is missing. (names guessed)
                int SetCall(const char *pName, long nArg, long nState, long nGroup);
                // Runs the code until an opcode stops it; returns m_nState.
                // (return type guessed)
                long Run();

            protected:
                // One opcode: returns the new state (0 to go on).
                typedef long (CVMCpu::*OPFUNC)();
                // DOL s_aOpFunc at 0x80486BD8 (.data, 92 entries): the opcode handlers
                // fn_801F7710.., indexed by the opcode byte. Run and
                // CVMScript::CallEntry dispatch through it. (name guessed)
                static OPFUNC s_aOpFunc[];

                // (name guessed) Inline helper: SetCall scales the index
                // after subtracting, as a separate argument.
                void SetStack(long nIndex, long nValue) { m_pStack[nIndex].i = nValue; }
                // (names guessed) Word nIndex of the stack, as an integer
                // and as a float. Inline: the index is computed before it
                // is scaled, as with SetStack.
                long GetStack(long nIndex) const { return m_pStack[nIndex].i; }
                float GetStackF(long nIndex) const { return m_pStack[nIndex].f; }

                CVMData *m_pData;        // 0x04
                unsigned char *m_pPC;    // 0x08: next opcode; null when stopped
                long m_nFP;              // 0x0C: frame index into m_pStack
                long m_nSP;              // 0x10: stack index
                unsigned long m_nStackSize; // 0x14: words in m_pStack
                STACK_DATA *m_pStack;    // 0x18
                long m_nUnk1C;           // 0x1C
                short m_nArgNum;         // 0x20: FUNCTION_TAG::nArgNum of the called function
                short m_nState;          // 0x22: last opcode result (Run returns it); 4 after Reset
                long m_nUnk24;           // 0x24: cleared by the constructor
        };
    }
}

#endif /* !MSW_VM_CVMCPU_HPP_ */
