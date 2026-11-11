#ifndef MSW_CCHRILLUSION_HPP_
#define MSW_CCHRILLUSION_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CScnObj.hpp"

namespace msw {

    class CChrCore;

    // INCOMPLETE (DOL class; added for BossF game/boss/unit_636D4.cpp). An
    // after-image of a character: a copy of its model that fades out by
    // itself. 0x3C bytes (CChrCoreBoss::LoopStateSideStep allocates it with
    // that size). Only the constructor is declared; the members are not
    // mapped. Argument meanings guessed: the starting alpha, how much it
    // fades per frame, and two colours.
    class CChrIllusion {
        public:
            CChrIllusion(CChrCore *pChr, float fAlpha, float fFadeRate, bcs::BCS_Color color0,
                         bcs::BCS_Color color1, bcs::CScnObj::AlphaMode mode);

        private:
            unsigned char m_Unk00[0x3C];
    };
}

#endif /* !MSW_CCHRILLUSION_HPP_ */
