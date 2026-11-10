#include "game/msw/CCollDefUnit.hpp"

namespace msw {

    void CCollDefUnit::SetReflectHitCallback(ReflectHitCallback pCallback, void *pArg)
    {
        m_pReflectHitCallback = pCallback;
        m_pReflectHitArg = pArg;
    }
}
