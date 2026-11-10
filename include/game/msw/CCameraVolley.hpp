#ifndef CCAMERAVOLLEY_HPP_
#define CCAMERAVOLLEY_HPP_

#include "game/msw/CGameCamera.hpp"

namespace msw {

    // The volley camera: the base of CCameraVolleyGame (the match) and
    // CCameraVolleyRR (game types 2 and 3). Adds no members of its own.
    class CCameraVolley : public CGameCamera {
        public:
            CCameraVolley(long view);
            virtual ~CCameraVolley() {}

            // Makes one camera per view, of the kind the game type needs.
            static void Initialize();
    };
}

#endif /* !CCAMERAVOLLEY_HPP_ */
