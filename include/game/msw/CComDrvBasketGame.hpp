#ifndef MSW_CCOMDRVBASKETGAME_HPP_
#define MSW_CCOMDRVBASKETGAME_HPP_

#include "game/msw/CComDrvBasket.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the normal basketball game
    // (BasketF; game/basket/CComDrvBasketGame.cpp). Only the constructor is
    // declared; the size, 0x688, is what CChrManBasket::CreateDriver_
    // allocates.
    class CComDrvBasketGame : public CComDrvBasket {
        public:
            CComDrvBasketGame();
            virtual ~CComDrvBasketGame();

            // (added for game/basket/CComManBasketGame.cpp; return types
            // guessed) The frame rate of a dunk, and the message that the
            // ghost caught a shot.
            float GetDunkFrameRate() const;
            void MsgShootCatchToGhost();

        protected:
            unsigned char m_Unk170[0x688 - 0x170];
    };
}

#endif /* !MSW_CCOMDRVBASKETGAME_HPP_ */
