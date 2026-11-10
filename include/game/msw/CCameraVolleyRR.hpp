#ifndef CCAMERAVOLLEYRR_HPP_
#define CCAMERAVOLLEYRR_HPP_

#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x290 (from msw::CCameraVolley::Initialize). The camera of game
    // types 2 and 3.
    // Only what CCameraVolley needs so far; members not mapped.
    class CCameraVolleyRR : public CCameraVolley {
        public:
            CCameraVolleyRR(long view);
            virtual ~CCameraVolleyRR();

        private:
            unsigned char m_Unk254[0x290 - 0x254];
    };
}

#endif /* !CCAMERAVOLLEYRR_HPP_ */
