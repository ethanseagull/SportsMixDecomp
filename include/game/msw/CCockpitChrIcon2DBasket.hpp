#ifndef CCOCKPITCHRICON2DBASKET_HPP_
#define CCOCKPITCHRICON2DBASKET_HPP_

#include "game/msw/CCockpitChrIcon2D.hpp"

namespace msw {

    // Size 0x68: adds no data. The basketball character icons (comb);
    // m_nType picks the layout
    // (game/basket/CCockpitChrIcon2DBasket.cpp).
    class CCockpitChrIcon2DBasket : public CCockpitChrIcon2D {
        public:
            CCockpitChrIcon2DBasket(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitChrIcon2DBasket();
            virtual int Set();
            virtual void Remove();
    };
}

#endif /* !CCOCKPITCHRICON2DBASKET_HPP_ */
