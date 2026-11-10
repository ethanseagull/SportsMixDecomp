#ifndef MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_
#define MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_

#include "game/msw/CBallManBasket.hpp"

namespace msw {

    // The ball manager of the Hungry Boss Packun court (BasketF), which
    // creates the fruit balls (msw::CBallBasketFruit).
    //
    // INCOMPLETE: only what game/basket/CBallBasketFruit.cpp needs. Nothing
    // of the class's own layout or virtual functions is declared, so it must
    // not be instantiated or derived from yet.
    class CBallManBasketHungryBossPackun : public CBallManBasket {
        public:
            // The radius of a fruit bomb's blast: BASKET_PRM_HB_BOMB_EXPLODE_R.
            // Static, out of line in the original.
            static float GetBombExplodeR();
    };
}

#endif /* !MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_ */
