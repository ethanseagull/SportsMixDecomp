#ifndef CCAMERABASKET_HPP_
#define CCAMERABASKET_HPP_

#include "game/msw/CGameCamera.hpp"

namespace msw {

    // The basketball camera: the base of CCameraBasketGame (the match) and
    // CCameraBasketHB (the party game). Adds no members of its own.
    class CCameraBasket : public CGameCamera {
        public:
            CCameraBasket(long view);
            virtual ~CCameraBasket() {}

            // Makes one camera per view, of the kind the game type needs.
            static void Initialize();
    };
}

#endif /* !CCAMERABASKET_HPP_ */
