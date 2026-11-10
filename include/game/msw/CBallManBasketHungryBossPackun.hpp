#ifndef MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_
#define MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_

#include "game/msw/CBallManBasket.hpp"

namespace msw {

    // The ball manager of the basket party game (game type 2) against the
    // hungry boss Piranha Plant (BasketF), size 0x84 (the size
    // CBallManBasket::CreateInstance allocates).
    //
    // INCOMPLETE: only the constructor CreateInstance calls. Its functions
    // are in game/basket/CBallManBasketHungryBossPackun.cpp, not written yet.
    class CBallManBasketHungryBossPackun : public CBallManBasket {
        public:
            CBallManBasketHungryBossPackun(bcs::CTask *pParent);
            virtual ~CBallManBasketHungryBossPackun();

        private:
            unsigned char m_Unk60[0x84 - 0x60];
    };
}

#endif /* !MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_ */
