#ifndef CCAMERABASKETHB_HPP_
#define CCAMERABASKETHB_HPP_

#include "game/msw/CCameraBasket.hpp"

namespace msw {

    // Size 0x29C (from msw::CCameraBasket::Initialize). The camera of the
    // basketball party game (game types 2 and 3). Only what other units need
    // so far; its own unit is game/basket/CCameraBasketHB.cpp.
    class CCameraBasketHB : public CCameraBasket {
        public:
            CCameraBasketHB(long view);
            virtual ~CCameraBasketHB();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            unsigned char m_Unk254[0x29C - 0x254];
    };
}

#endif /* !CCAMERABASKETHB_HPP_ */
