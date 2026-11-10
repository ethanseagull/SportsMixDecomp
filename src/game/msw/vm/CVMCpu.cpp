#include "game/msw/vm/CVMCpu.hpp"

namespace msw {
    namespace vm {

        void CVMCpu::Reset()
        {
            OnReset();
            m_nSP = 0;
            m_nFP = 0;
            m_pPC = 0;
            m_nState = 4;
            m_nUnk1C = 0;
        }

        void CVMCpu::OnReset()
        {
        }

        int CVMCpu::SetCall(const char *pName, long nArg, long nState, long nGroup)
        {
            CVMData::FUNCTION_TAG *pTag;

            m_nSP += 3;
            m_nFP = m_nSP;
            SetStack(m_nSP - 3, nArg);
            SetStack(m_nSP - 2, m_nFP);
            SetStack(m_nSP - 1, -1);
            if (nState != -1) {
                m_nState = nState;
            }
            m_nUnk1C = 0;

            pTag = m_pData->FindFunctionTag(pName, nGroup);
            if (pTag != 0 && pTag->nOffset != -1) {
                m_pPC = m_pData->GetCode() + pTag->nOffset;
                m_nArgNum = pTag->nArgNum;
                return 1;
            }
            m_pPC = 0;
            m_nArgNum = 0;
            return 0;
        }
    }
}
