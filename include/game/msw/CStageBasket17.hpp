#ifndef CSTAGEBASKET17_HPP_
#define CSTAGEBASKET17_HPP_

#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Size 0x27C (from CStageFactoryBasket::Create); members not
    // mapped.

    class CStageBasket17 : public CStageManBasket {
        public:
            CStageBasket17(bcs::CTask *pParent);
            virtual ~CStageBasket17();

            // Static: the caller leaves r3 unset (BasketF
            // CEventDemoBasket::CameraEvent).
            static void TrolleyForceReset();

        private:
            unsigned char m_Unk218[0x64]; // size 0x27C
    };
}

#endif /* !CSTAGEBASKET17_HPP_ */
