#ifndef MSW_VM_CVMCPU_HPP_
#define MSW_VM_CVMCPU_HPP_

#include "game/msw/vm/CVMData.hpp"

namespace msw {
    namespace vm {

        // The event script VM's processor (DOL game/msw/vm/CVMCpu.cpp, which
        // also holds CVMData and CVMScript): a code pointer into a CVMData
        // image and a word stack. vptr at 0. Its vtable
        // (__vt__Q33msw2vm6CVMCpu, 4 slots) sits after CVMScript's, and the
        // destructor after CVMScript's code. STACK_DATA (one word of the VM
        // stack) is in CVMData.hpp.

        class CVMCpu {
            public:
                // Inline: TourF msw::CTmScript's constructor shows it. The
                // code pointer and the words from 0x1C are left alone.
                CVMCpu(unsigned long nStackSize)
                {
                    m_pData = 0;
                    m_nFP = 0;
                    m_nSP = 0;
                    m_nMaxSP = 0;
                    m_nStackSize = nStackSize;
                    m_pStack = new STACK_DATA[nStackSize];
                }

                // DOL 0x801FADA4: returns 3 (an error state). Not inline: it is the class's
                // key function, which keeps the vtable (and the weak copies
                // of the inline virtuals) out of TourF, where
                // msw::CTmScript's constructor and destructor refer to it.
                virtual long OpSysCall();
                // DOL 0x801FB58C (after CVMScript's code, a weak copy): frees
                // the stack and clears the pointer. Inline: TourF
                // msw::CTmScript's destructor inlines it.
                virtual ~CVMCpu()
                {
                    if (m_pStack != 0) {
                        delete[] m_pStack;
                    }
                    m_pStack = 0;
                }
                // DOL 0x801FB06C (weak), amid CVMScript's code: empty.
                virtual void UpdateBind() {}
                // DOL 0x801FAE18: empty; the first thing Reset calls.
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
                // DOL s_aOpFunc at 0x80486BD8 (.data, 92 entries): the opcode
                // handlers below, indexed by the opcode byte. Run and
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

                void SetStackF(long nIndex, float fValue) { m_pStack[nIndex].f = fValue; }
                // (names guessed) Word nIndex of the stack, writable. The
                // compiler does not share a load of m_pStack between these
                // and the const getters above, which is how the original
                // shows which of them a handler used.
                long &Stack(long nIndex) { return m_pStack[nIndex].i; }
                float &StackF(long nIndex) { return m_pStack[nIndex].f; }
                // (names guessed) One word onto the stack.
                void Push(long nValue)
                {
                    m_nSP++;
                    SetStack(m_nSP - 1, nValue);
                }
                void PushF(float fValue)
                {
                    m_nSP++;
                    StackF(m_nSP - 1) = fValue;
                }
                // The word a push adds, to be written. (name guessed)
                long &NewTop()
                {
                    m_nSP++;
                    return Stack(m_nSP - 1);
                }

                // The 32-bit big-endian operand after the opcode. (name guessed)
                long FetchOperand()
                {
                    long nOperand = m_pPC[3];
                    nOperand |= m_pPC[2] << 8;
                    nOperand |= m_pPC[1] << 16;
                    nOperand |= m_pPC[0] << 24;
                    m_pPC += 4;
                    return nOperand;
                }
                // The word a VM address names: bit 29 selects the static
                // data, bit 30 the script's global values, otherwise the
                // stack. (name guessed)
                long &Var(long nAddr)
                {
                    long *pVar;
                    if (nAddr & 0x20000000) {
                        pVar = &m_pData->Static(nAddr & 0x1FFFFFFF);
                    } else if ((nAddr & 0x40000000) == 0) {
                        pVar = &Stack(nAddr);
                    } else {
                        pVar = &m_pData->Value(nAddr & 0x3FFFFFFF);
                    }
                    return *pVar;
                }
                // The same for a store: the static data is read-only, so
                // only bit 30 is tested. (name guessed)
                long &WritableVar(long nAddr)
                {
                    long *pVar;
                    if ((nAddr & 0x40000000) == 0) {
                        pVar = &Stack(nAddr);
                    } else {
                        pVar = &m_pData->Value(nAddr & 0x3FFFFFFF);
                    }
                    return *pVar;
                }

                // The opcode handlers, in the order of the original code;
                // the opcode is the index in s_aOpFunc. All names are
                // guesses from what the code does. "Value" is a global of
                // the script (CVMData::m_pValue), "Local" a word of the
                // current frame (m_nFP + operand), "Ref" a local holding a
                // VM address (see Var), "Block" several words: the
                // operand then holds a word offset in its high half and the
                // word count in its low half.
                long OpPushInt();               // 0x00
                long OpPushFloat();             // 0x31
                long OpPushStaticPtr();         // 0x01
                long OpPushValue();             // 0x02
                long OpPushLocal();             // 0x03
                long OpLoadValueIndex();        // 0x04
                long OpLoadLocalIndex();        // 0x05
                long OpPushValueBlock();        // 0x37
                long OpPushLocalBlock();        // 0x38
                long OpPushValueBlockIndex();   // 0x39
                long OpPushStaticBlockIndex();  // 0x57
                long OpPushLocalBlockIndex();   // 0x3A
                long OpPushRef();               // 0x06
                long OpLoadRefIndex();          // 0x07
                long OpPushRefBlock();          // 0x3B
                long OpPushRefBlockIndex();     // 0x3C
                long OpPushAddrBlock();         // 0x59
                long OpRemoveBlock();           // 0x5B
                long OpPushAddress();           // 0x08
                long OpAddAddress();            // 0x09
                long OpPopValue();              // 0x0A
                long OpPopLocal();              // 0x0B
                long OpPopValueIndex();         // 0x0C
                long OpPopLocalIndex();         // 0x0D
                long OpPopRef();                // 0x0E
                long OpPopRefIndex();           // 0x0F
                long OpPopValueBlock();         // 0x3D
                long OpPopLocalBlock();         // 0x3E
                long OpPopValueBlockIndex();    // 0x3F
                long OpPopLocalBlockIndex();    // 0x40
                long OpPopRefBlock();           // 0x41
                long OpPopRefBlockIndex();      // 0x42
                long OpPopAddrBlock();          // 0x5A
                long OpFillInt();               // 0x10
                long OpFillFloat();             // 0x45
                long OpEnter();                 // 0x11
                long OpDrop();                  // 0x12
                long OpDropN();                 // 0x43
                long OpMulImm();                // 0x58
                long OpNeg();                   // 0x13
                long OpNegF();                  // 0x47
                long OpNot();                   // 0x32
                long OpNotF();                  // 0x52
                long OpInc();                   // 0x33
                long OpIncF();                  // 0x53
                long OpDec();                   // 0x34
                long OpDecF();                  // 0x54
                long OpPostInc();               // 0x35
                long OpPostIncF();              // 0x55
                long OpPostDec();               // 0x36
                long OpPostDecF();              // 0x56
                long OpEq();                    // 0x14
                long OpEqF();                   // 0x48
                long OpNe();                    // 0x15
                long OpNeF();                   // 0x49
                long OpGt();                    // 0x16
                long OpGtF();                   // 0x4A
                long OpGe();                    // 0x17
                long OpGeF();                   // 0x4B
                long OpLt();                    // 0x18
                long OpLtF();                   // 0x4C
                long OpLe();                    // 0x19
                long OpLeF();                   // 0x4D
                long OpLogicalAnd();            // 0x1A
                long OpLogicalOr();             // 0x1B
                long OpAnd();                   // 0x1C
                long OpOr();                    // 0x1D
                long OpShl();                   // 0x1E
                long OpShr();                   // 0x1F
                long OpAdd();                   // 0x20
                long OpAddF();                  // 0x4E
                long OpSub();                   // 0x21
                long OpSubF();                  // 0x4F
                long OpMul();                   // 0x22
                long OpMulF();                  // 0x50
                long OpDiv();                   // 0x23
                long OpDivF();                  // 0x51
                long OpMod();                   // 0x24
                long OpStrEq();                 // 0x25
                long OpStrNe();                 // 0x26
                long OpJump();                  // 0x27
                long OpJumpTrue();              // 0x28
                long OpJumpFalse();             // 0x29
                long OpCase();                  // 0x2A
                long OpCall();                  // 0x2B
                // 0x2C is the virtual OpSysCall.
                long OpReturn();                // 0x2D
                long OpReturnValue();           // 0x2E
                long OpReturnBlock();           // 0x44
                long OpUnused2F();              // 0x2F: returns 3
                long OpUnused30();              // 0x30: returns 3
                long OpUnused46();              // 0x46: returns 3

                CVMData *m_pData;        // 0x04
                unsigned char *m_pPC;    // 0x08: next opcode; null when stopped
                long m_nFP;              // 0x0C: frame index into m_pStack
                long m_nSP;              // 0x10: stack index
                unsigned long m_nStackSize; // 0x14: words in m_pStack
                STACK_DATA *m_pStack;    // 0x18
                long m_nUnk1C;          // 0x1C: the value a script function returned to the host
                short m_nArgNum;         // 0x20: FUNCTION_TAG::nArgNum of the called function
                short m_nState;          // 0x22: last opcode result (Run returns it); 4 after Reset
                long m_nMaxSP;           // 0x24: highest stack index reached (OpEnter); cleared by the constructor
        };
    }
}

#endif /* !MSW_VM_CVMCPU_HPP_ */
