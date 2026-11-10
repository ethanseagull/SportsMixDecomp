#include "game/msw/CMenuPadInfo.hpp"

namespace msw {

    long CMenuPadInfo::SelectPadLeader(int)
    {
        return m_nPadLeader;
    }

    const unsigned short *CMenuPadInfo::GetPadStates()
    {
        static unsigned short s_aState[4];

        s_aState[0] = m_aPad[0].m_nState;
        s_aState[1] = m_aPad[1].m_nState;
        s_aState[2] = m_aPad[2].m_nState;
        s_aState[3] = m_aPad[3].m_nState;
        return s_aState;
    }
}
