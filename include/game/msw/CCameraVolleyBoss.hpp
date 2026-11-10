#ifndef CCAMERAVOLLEYBOSS_HPP_
#define CCAMERAVOLLEYBOSS_HPP_

#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x274 (from msw::CCameraVolley::Initialize). The camera of game
    // type 1.
    // Only what CCameraVolley needs so far; members not mapped.
    class CCameraVolleyBoss : public CCameraVolley {
        public:
            CCameraVolleyBoss(long view);
            virtual ~CCameraVolleyBoss();

        private:
            unsigned char m_Unk254[0x274 - 0x254];
    };
}

#endif /* !CCAMERAVOLLEYBOSS_HPP_ */
