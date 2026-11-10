#ifndef CCAMERABASKETGAME_HPP_
#define CCAMERABASKETGAME_HPP_

#include "game/msw/CCameraBasket.hpp"

namespace msw {

    // Size 0x88C (from msw::CCameraBasket::Initialize). The camera of a
    // basketball match. Only what other units need so far; its own unit is
    // game/basket/CCameraBasketGame.cpp.
    class CCameraBasketGame : public CCameraBasket {
        public:
            CCameraBasketGame(long view);
            virtual ~CCameraBasketGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool IsNormalCamMode() const;

        private:
            unsigned char m_Unk254[0x88C - 0x254];
    };
}

#endif /* !CCAMERABASKETGAME_HPP_ */
