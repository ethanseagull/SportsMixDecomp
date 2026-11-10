#ifndef CSTAGEHOCKEY17_HPP_
#define CSTAGEHOCKEY17_HPP_

namespace msw {

    // Hockey stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Only what other classes call so far.
    class CStageHockey17 {
        public:
            // Static: the caller leaves r3 unset (HockeyF
            // CEventDemoHockey::CameraEvent).
            static void TrolleyForceReset();
    };
}

#endif /* !CSTAGEHOCKEY17_HPP_ */
