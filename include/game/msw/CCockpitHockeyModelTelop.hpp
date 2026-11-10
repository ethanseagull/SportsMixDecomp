#ifndef CCOCKPITHOCKEYMODELTELOP_HPP_
#define CCOCKPITHOCKEYMODELTELOP_HPP_

#include "game/msw/CCockpitModelTelop.hpp"

namespace msw {

    // Size 0x74: adds no data. The dodgeball telops drawn as effects; m_nType
    // picks the effect and sound (game/hockey/CCockpitHockeyModelTelop.cpp).
    class CCockpitHockeyModelTelop : public CCockpitModelTelop {
        public:
            CCockpitHockeyModelTelop(long nId, long nType, float fSortZ);
            virtual ~CCockpitHockeyModelTelop();
            virtual int Set();
    };
}

#endif /* !CCOCKPITHOCKEYMODELTELOP_HPP_ */
