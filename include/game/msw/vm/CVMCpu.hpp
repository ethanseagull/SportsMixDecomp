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
        class CVMCpu {
            public:
                // DOL fn_801FADA4: returns 3.
                virtual long OpSysCall() { return 3; }
                // DOL fn_801FB58C (after CVMScript's code): frees the stack
                // (delete[]) and clears the pointer. Not written yet.
                virtual ~CVMCpu();
                // DOL fn_801FB06C, amid CVMScript's code: empty.
                virtual void UpdateBind() {}
                // (name guessed) DOL fn_801FAE18: empty; the first thing
                // Reset calls. Defined in CVMCpu.cpp, between Reset and
                // SetCall.
                virtual void OnReset();

                void Reset();
                // Pushes a call frame (nArg, the new frame pointer, -1) and jumps
                // to function pName of group nGroup. With nState other than
                // -1 the state becomes nState. Returns 0 and clears the code
                // pointer when the function is missing. (names guessed)
                int SetCall(const char *pName, long nArg, long nState, long nGroup);

            protected:
                // (name guessed) Inline helper: SetCall scales the index
                // after subtracting, as a separate argument.
                void SetStack(long nIndex, long nValue) { m_pStack[nIndex] = nValue; }

                CVMData *m_pData;        // 0x04
                unsigned char *m_pPC;    // 0x08: next opcode; null when stopped
                long m_nFP;              // 0x0C: frame index into m_pStack
                long m_nSP;              // 0x10: stack index
                unsigned long m_Unk14;
                long *m_pStack;          // 0x18
                long m_nUnk1C;           // 0x1C
                short m_nArgNum;         // 0x20: FUNCTION_TAG::nArgNum of the called function
                short m_nState;          // 0x22: last opcode result (Run returns it); 4 after Reset
        };
    }
}

#endif /* !MSW_VM_CVMCPU_HPP_ */
