#ifndef CCOCKPITBASKETMODELTELOP_HPP_
#define CCOCKPITBASKETMODELTELOP_HPP_

#include "game/msw/CCockpitModelTelop.hpp"

namespace msw {

    // Size 0x74: adds no data. The basketball telops drawn as effects; m_nType
    // picks the effect and sound (game/basket/CCockpitBasketModelTelop.cpp).
    class CCockpitBasketModelTelop : public CCockpitModelTelop {
        public:
            CCockpitBasketModelTelop(long nId, long nType, float fSortZ);
            virtual ~CCockpitBasketModelTelop();
            virtual int Set();
    };
}

#endif /* !CCOCKPITBASKETMODELTELOP_HPP_ */
