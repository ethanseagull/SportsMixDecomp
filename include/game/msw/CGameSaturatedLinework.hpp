#ifndef CGAMESATURATEDLINEWORK_HPP_
#define CGAMESATURATEDLINEWORK_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // The speed lines effect over the screen (DOL,
    // game/msw/CGameExDemoBase.cpp; 0x3C bytes, vtable lbl_8048D550).
    // msw::CGame::StartSaturatedLinework makes it. The class name is taken
    // from that function; the member names are guesses.
    class CGameSaturatedLinework : public bcs::CTask {
        public:
            CGameSaturatedLinework(bcs::CTask *pParent, float fFrame);
            virtual ~CGameSaturatedLinework();
            virtual void Main();

        private:
            long m_nFrame;           // 0x34: frames left
            unsigned long m_hEffect; // 0x38: effect 0x461
    };
}

#endif /* !CGAMESATURATEDLINEWORK_HPP_ */
