#ifndef MSW_CBALLMARKERVOLLEY_HPP_
#define MSW_CBALLMARKERVOLLEY_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The ball marker of the volley game (VolleyF), size 0x94 (the size
    // CBallManVolley allocates).
    //
    // INCOMPLETE: only what game/volley/CBallManVolley.cpp needs to create
    // one; the overridden virtual functions are not declared yet.
    class CBallMarkerVolley : public CBallMarker {
        public:
            CBallMarkerVolley(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerVolley();

        private:
            float m_Unk90; // 0x90, set from a constant by the constructor
    };
}

#endif /* !MSW_CBALLMARKERVOLLEY_HPP_ */
