#ifndef CCAMERAVOLLEYGAME_HPP_
#define CCAMERAVOLLEYGAME_HPP_

#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x398 (from msw::CCameraVolley::Initialize). The camera of a
    // volley match (every other game type).
    // Only what CCameraVolley needs so far; members not mapped.
    class CCameraVolleyGame : public CCameraVolley {
        public:
            CCameraVolleyGame(long view);
            virtual ~CCameraVolleyGame();

        private:
            unsigned char m_Unk254[0x398 - 0x254];
    };
}

#endif /* !CCAMERAVOLLEYGAME_HPP_ */
