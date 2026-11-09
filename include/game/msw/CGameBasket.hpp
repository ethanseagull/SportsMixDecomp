#ifndef CGAMEBASKET_HPP_
#define CGAMEBASKET_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Lives in module BasketF. Size 0x274.
    class CGameBasket : public CGame {
        public:
            CGameBasket(bcs::CTask *pParent);

            // Creates the game the current settings ask for: this class or
            // the basketball party game.
            static CGame *CreateInstance(); // return type guessed

        private:
            unsigned char m_Unk150[0x274 - sizeof(CGame)];
    };
}

#endif /* !CGAMEBASKET_HPP_ */
