#include "game/msw/CArrowSignalBase.hpp"

namespace msw {

    CArrowSignalBase::CArrowSignalBase(bcs::CTask *pParent, long nChrIdx, long nTeam)
        : bcs::CTask(pParent, -1),
          m_pMdl(0),
          m_nChrIdx(nChrIdx),
          m_nTeam(nTeam),
          m_nState(0),
          m_nCount(0),
          m_Unk48(0.0f),
          m_Unk4C(1.0f),
          m_Unk50(0.0f),
          m_Unk54(1.0f),
          m_Unk58(0.0f),
          m_Unk5C(0.0f)
    {
    }
}
