#ifndef CSTAGEVOLLEY17_HPP_
#define CSTAGEVOLLEY17_HPP_

namespace msw {

    // Hockey stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Only what other classes call so far.
    class CStageVolley17 {
        public:
            // Static: the caller leaves r3 unset (HockeyF
            // CEventDemoVolley::CameraEvent).
            static void TrolleyForceReset();
    };
}

#endif /* !CSTAGEVOLLEY17_HPP_ */
