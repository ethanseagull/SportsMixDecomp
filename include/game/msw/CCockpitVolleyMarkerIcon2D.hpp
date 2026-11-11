#ifndef MSW_CCOCKPITVOLLEYMARKERICON2D_HPP_
#define MSW_CCOCKPITVOLLEYMARKERICON2D_HPP_

#include "game/msw/CCockpitMarkerIcon2D.hpp"

namespace msw {

    // Size 0x70: the volley game's charge guide marker. Adds no data.
    class CCockpitVolleyMarkerIcon2D : public CCockpitMarkerIcon2D {
        public:
            CCockpitVolleyMarkerIcon2D(long nId, float fSortZ, long nType, long nLayer);
            virtual ~CCockpitVolleyMarkerIcon2D();
            virtual int Set();
    };
}

#endif /* !MSW_CCOCKPITVOLLEYMARKERICON2D_HPP_ */
