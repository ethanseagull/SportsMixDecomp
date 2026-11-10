#ifndef CCOCKPITVOLLEYMODELTELOP_HPP_
#define CCOCKPITVOLLEYMODELTELOP_HPP_

#include "game/msw/CCockpitModelTelop.hpp"

namespace msw {

    // Size 0x74: adds no data. The dodgeball telops drawn as effects; m_nType
    // picks the effect and sound (game/volley/CCockpitVolleyModelTelop.cpp).
    class CCockpitVolleyModelTelop : public CCockpitModelTelop {
        public:
            CCockpitVolleyModelTelop(long nId, long nType, float fSortZ);
            virtual ~CCockpitVolleyModelTelop();
            virtual int Set();
    };
}

#endif /* !CCOCKPITVOLLEYMODELTELOP_HPP_ */
