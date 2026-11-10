#ifndef CCOCKPITCHRICON2DHOCKEY_HPP_
#define CCOCKPITCHRICON2DHOCKEY_HPP_

#include "game/msw/CCockpitChrIcon2D.hpp"

namespace msw {

    // Size 0x68: adds no data. The hockey character icons (A button,
    // controller colour, comb); m_nType picks the layout
    // (game/hockey/CCockpitChrIcon2DHockey.cpp).
    class CCockpitChrIcon2DHockey : public CCockpitChrIcon2D {
        public:
            CCockpitChrIcon2DHockey(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitChrIcon2DHockey();
            virtual int Set();
            virtual void Remove();
    };
}

#endif /* !CCOCKPITCHRICON2DHOCKEY_HPP_ */
