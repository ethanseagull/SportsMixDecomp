#ifndef MSW_VM_CVMSCRIPT_HPP_
#define MSW_VM_CVMSCRIPT_HPP_

#include "game/msw/vm/CVMCpu.hpp"
#include "game/msw/vm/CVMData.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    namespace vm {

        // INCOMPLETE: only what game/msw/vm/CVMScript.cpp needs; the
        // BindSysCall templates the modules instantiate are not declared yet.
        //
        // A script VM whose system calls are bound to C++ functions by name
        // (DOL; derived in the modules, e.g. BasketF msw::CHBScript). Each
        // system call slot holds a small function object (CFunc0<...>,
        // CFuncS0<...>: a vptr and the bound function), built in place by
        // the BindSysCall templates. Size at least 0x38.
        class CVMScript : public CVMCpu {
            public:
                // The function object of one system call: `Call` runs it on
                // the script and returns the VM state. (names guessed)
                class CFuncBase {
                    public:
                        virtual long Call(CVMScript *pScript) = 0;
                };

                // Room for any of the function objects. (name guessed)
                struct FUNC_BUF {
                    // A signed bound: with sizeof's unsigned one the compiler
                    // unrolls the loop only partly (UpdateBind clears all 20
                    // bytes with straight stores).
                    void Clear()
                    {
                        for (int i = 0; i < (int)sizeof(aBuf); i++) {
                            aBuf[i] = 0;
                        }
                    }

                    unsigned char aBuf[0x14];
                };

                // DOL 0x801FB470 (weak, was fn_801FB470): frees the system call table and
                // the script data this object owns.
                virtual ~CVMScript()
                {
                    if (m_pFunc != 0) {
                        delete[] m_pFunc;
                    }
                    m_pFunc = 0;
                    ReleaseData();
                }

                virtual long OpSysCall();
                virtual void UpdateBind();
                // DOL 0x801FB46C (weak, was fn_801FB46C; BasketF has a copy).
                virtual void ResetBind() {}
                // Binds the system calls (BasketF CHBScript::BindFunctions).
                virtual void BindFunctions() = 0;
                // (meaning unknown) Slot 0x1C, pure.
                virtual void Unk1C() = 0;

                // Runs script image pImage, owned from now on.
                void SetScript(void *pImage);
                // Runs the script data of `script`, shared with it.
                void SetScript(const CVMScript &script);
                // Runs the code until an opcode stops it; returns the state.
                long CallEntry();
                // The vector at operand nIndex: three words of the stack, or
                // of the script values when bit 30 is set.
                bcs::FVEC3 GetVector3(int nIndex) const;
                void SetVector3(int nIndex, bcs::FVEC3 v);

            protected:
                // System call "Yield": drops the arguments and the count
                // word, and stops the VM for this frame.
                long _OpSysCallYield();

            private:
                // Deletes the script data when this object owns it.
                void ReleaseData()
                {
                    if (m_bOwnData) {
                        if (m_pOwnData != 0) {
                            delete m_pOwnData;
                        }
                        m_pOwnData = 0;
                        m_bOwnData = 0;
                    }
                }

                // Bit 30 of an operand selects the script values.
                static bool IsValue(int nIndex) { return (nIndex & 0x40000000) != 0; }
                static int ValueIndex(int nIndex) { return nIndex & 0x3FFFFFFF; }

                float GetValueF(long nIndex) const
                {
                    return *reinterpret_cast<float *>(&m_pData->Value(nIndex));
                }
                void SetValueF(long nIndex, float fValue)
                {
                    m_pData->Value(nIndex) = *reinterpret_cast<long *>(&fValue);
                }
                // The top word of a system call's frame: its argument count.
                // (name guessed)
                long GetArgNum() const { return GetStack(m_nSP - 1); }

            protected:
                FUNC_BUF *m_pFunc;       // 0x28: one per system call
                long m_nFuncNum;         // 0x2C
                CVMData *m_pOwnData;     // 0x30: the data given to SetScript
                long m_bOwnData;         // 0x34: m_pOwnData was allocated here
        };
    }
}

#endif /* !MSW_VM_CVMSCRIPT_HPP_ */
