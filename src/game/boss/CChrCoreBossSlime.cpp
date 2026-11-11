// msw::CChrCoreBossSlime in the boss game.
#include "game/msw/CChrCoreBoss.hpp"

msw::CChrCoreBossSlime::CChrCoreBossSlime()
{
    m_nColorType = 0;
}

msw::CChrCoreBossSlime::~CChrCoreBossSlime()
{
}

void msw::CChrCoreBossSlime::Reset_()
{
    CChrCoreBoss::Reset_();

    m_nColorType = 0;
    switch (m_nColor) {
        case 1:
            m_nColorType = 1;
            break;
        case 2:
            m_nColorType = 2;
            break;
    }
}
