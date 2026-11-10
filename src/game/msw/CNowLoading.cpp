#include "game/msw/CNowLoading.hpp"

namespace msw {

    void CNowLoading::SetType(long nType)
    {
        CNowLoading *pThis = s_pInstance;
        if (pThis != 0) {
            pThis->m_nType = nType;
            pThis->m_fTime = 0.0f;
        }
    }
}
