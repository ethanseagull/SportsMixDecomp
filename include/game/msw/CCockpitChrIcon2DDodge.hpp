#ifndef CCOCKPITCHRICON2DDODGE_HPP_
#define CCOCKPITCHRICON2DDODGE_HPP_

#include "game/msw/CCockpitChrIcon2D.hpp"

namespace msw {

    // Size 0x68: adds no data. The dodgeball character icons (crown, lock-on,
    // spin, comb); m_nType picks the layout
    // (game/dodge/CCockpitChrIcon2DDodge.cpp).
    class CCockpitChrIcon2DDodge : public CCockpitChrIcon2D {
        public:
            CCockpitChrIcon2DDodge(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitChrIcon2DDodge();
            virtual int Set();
            virtual void Remove();
    };
}

#endif /* !CCOCKPITCHRICON2DDODGE_HPP_ */
