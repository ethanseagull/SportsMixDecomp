#ifndef CSTAGEHOCKEY17_HPP_
#define CSTAGEHOCKEY17_HPP_

#include "game/msw/CStageManHockey.hpp"

namespace msw {

    // Hockey stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Size 0x274 (from CStageFactoryHockey::Create); members not
    // mapped.

    class CStageHockey17 : public CStageManHockey {
        public:
            CStageHockey17(bcs::CTask *pParent);
            virtual ~CStageHockey17();

            // Static: the caller leaves r3 unset (HockeyF
            // CEventDemoHockey::CameraEvent).
            static void TrolleyForceReset();

        private:
            unsigned char m_Unk218[0x5C]; // size 0x274
    };
}

#endif /* !CSTAGEHOCKEY17_HPP_ */
