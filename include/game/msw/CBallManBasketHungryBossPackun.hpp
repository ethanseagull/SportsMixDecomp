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

            // The radius of a fruit bomb's blast: BASKET_PRM_HB_BOMB_EXPLODE_R.
            // Static, out of line in the original. (used by
            // game/basket/CBallBasketFruit.cpp)
            static float GetBombExplodeR();
            // (added for game/basket/CGameBasketHungryBossPackun.cpp; return
            // type guessed) Not zero during the bonus time.
            int IsNowBonusTime_() const;

        private:
            unsigned char m_Unk60[0x84 - 0x60];
    };
}

#endif /* !MSW_CBALLMANBASKETHUNGRYBOSSPACKUN_HPP_ */
