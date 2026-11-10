#ifndef CSTAGEVOLLEY17_HPP_
#define CSTAGEVOLLEY17_HPP_

#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // Volley stage 0x11 (CGame::GameWork::m_nStage), the one with the
    // trolley. Made by CStageFactoryVolley::Create (size from the
    // allocation there); members not mapped.
    class CStageVolley17 : public CStageManVolley {
        public:
            CStageVolley17(bcs::CTask *pParent);
            virtual ~CStageVolley17();

            // Static: the caller leaves r3 unset (VolleyF
            // CEventDemoVolley::CameraEvent).
            static void TrolleyForceReset();

        private:
            unsigned char m_Unk200[0x5C]; // size 0x25C
    };
}

#endif /* !CSTAGEVOLLEY17_HPP_ */
