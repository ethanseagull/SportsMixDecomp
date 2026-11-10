#include "game/msw/CEffectCoinGetBase.hpp"

namespace msw {

    CEffectCoinGetBase::CEffectCoinGetBase(bcs::CTask *pParent)
        : bcs::CUnivTask(pParent, 0x17),
          m_pMdl(0),
          m_fGravity(0.0f),
          m_fWait(0.0f),
          m_fTopY(0.0f),
          m_fScale(0.0f),
          m_fRotSpeed(0.0f),
          m_fLife(0.0f),
          m_nState(0),
          m_hEffect(0)
    {
        m_Vel.Set(0.0f, 0.0f, 0.0f);
    }
}
