#include "game/msw/CGameSpeedCtrl.hpp"

namespace msw {

    void CGameSpeedCtrl::StartSpeedAnim(float from, float to, float time, int nUnk68, long nUnk60)
    {
        if (time > 0.0f) {
            m_fSpeed = m_fFrom = from;
            m_fTo = to;
            m_fElapsed = 0.0f;
            m_fTime = time;
            m_nUnk60 = nUnk60;
        } else {
            m_fFrom = m_fTo = m_fSpeed = to;
            m_nUnk60 = 0;
        }
        m_nUnk5C = 0;
        m_nUnk64 = 0;
        m_nUnk68 = nUnk68;
    }
}
