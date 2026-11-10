#ifndef CCOCKPITDODGEMODELTELOP_HPP_
#define CCOCKPITDODGEMODELTELOP_HPP_

#include "game/msw/CCockpitModelTelop.hpp"

namespace msw {

    // Size 0x74: adds no data. The dodgeball telops drawn as effects; m_nType
    // picks the effect and sound (game/dodge/CCockpitDodgeModelTelop.cpp).
    class CCockpitDodgeModelTelop : public CCockpitModelTelop {
        public:
            CCockpitDodgeModelTelop(long nId, long nType, float fSortZ);
            virtual ~CCockpitDodgeModelTelop();
            virtual int Set();
    };
}

#endif /* !CCOCKPITDODGEMODELTELOP_HPP_ */
