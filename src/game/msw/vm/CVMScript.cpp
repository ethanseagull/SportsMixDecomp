// msw::vm::CVMScript: the script VM with bound system calls (DOL).

#include "game/msw/vm/CVMScript.hpp"

namespace msw {
    namespace vm {

        void CVMScript::SetScript(void *pImage)
        {
            ReleaseData();
            m_pOwnData = new CVMData(pImage);
            m_bOwnData = 1;
            m_pData = m_pOwnData;
            UpdateBind();
        }

        void CVMScript::SetScript(const CVMScript &script)
        {
            ReleaseData();
            m_pOwnData = script.m_pOwnData;
            m_pData = m_pOwnData;
            UpdateBind();
        }

        long CVMScript::CallEntry()
        {
            while (m_pPC != 0) {
                if ((m_nState = (this->*s_aOpFunc[*m_pPC++])()) != 0) {
                    break;
                }
            }
            return m_nState;
        }

        bcs::FVEC3 CVMScript::GetVector3(int nIndex) const
        {
            if (!IsValue(nIndex)) {
                return bcs::FVEC3(GetStackF(nIndex), GetStackF(nIndex + 1), GetStackF(nIndex + 2));
            }
            nIndex = ValueIndex(nIndex);
            return bcs::FVEC3(GetValueF(nIndex), GetValueF(nIndex + 1), GetValueF(nIndex + 2));
        }

        void CVMScript::SetVector3(int nIndex, bcs::FVEC3 v)
        {
            if (!IsValue(nIndex)) {
                SetStackF(nIndex, v.x);
                SetStackF(nIndex + 1, v.y);
                SetStackF(nIndex + 2, v.z);
            } else {
                nIndex = ValueIndex(nIndex);
                SetValueF(nIndex, v.x);
                SetValueF(nIndex + 1, v.y);
                SetValueF(nIndex + 2, v.z);
            }
        }

        long CVMScript::_OpSysCallYield()
        {
            m_nSP -= GetArgNum() + 1;
            return 1;
        }

        void CVMScript::UpdateBind()
        {
            if (m_pFunc != 0) {
                delete[] m_pFunc;
            }
            m_pFunc = 0;
            m_nFuncNum = m_pData->GetSysCallNum();
            m_pFunc = new FUNC_BUF[m_nFuncNum];
            for (long i = 0; i < m_nFuncNum; i++) {
                m_pFunc[i].Clear();
            }
            BindFunctions();
        }

        long CVMScript::OpSysCall()
        {
            // The call number follows the opcode, big-endian.
            long nCall = m_pPC[3];
            nCall |= m_pPC[2] << 8;
            nCall |= m_pPC[1] << 16;
            nCall |= m_pPC[0] << 24;
            m_pPC += 4;
            if (nCall < 0 || nCall >= m_nFuncNum) {
                return 3;
            }
            return reinterpret_cast<CFuncBase *>(&m_pFunc[nCall])->Call(this);
        }
    }
}
