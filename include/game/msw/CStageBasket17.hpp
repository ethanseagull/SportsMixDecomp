#ifndef CSTAGEBASKET17_HPP_
#define CSTAGEBASKET17_HPP_

namespace msw {

    // Hockey stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Only what other classes call so far.
    class CStageBasket17 {
        public:
            // Static: the caller leaves r3 unset (HockeyF
            // CEventDemoBasket::CameraEvent).
            static void TrolleyForceReset();
    };
}

#endif /* !CSTAGEBASKET17_HPP_ */
