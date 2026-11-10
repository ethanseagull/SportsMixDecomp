#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    CCockpitResultTime::CCockpitResultTime(long nKind)
    {
        if (nKind == 1) {
            m_fUnk00 = 75.0f;
        } else {
            m_fUnk00 = 30.0f;
        }
        m_fUnk04 = 15.0f;
    }
}
