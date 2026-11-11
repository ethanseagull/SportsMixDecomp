#ifndef MSW_CCHRDRVPADVOLLEY_HPP_
#define MSW_CCHRDRVPADVOLLEY_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    // INCOMPLETE: the pad driver of the volleyball game (VolleyF), size 0xB4
    // (what CChrManVolley::CreateDriver_ allocates). Its base in the original
    // is the DOL's CChrDrvPad, which is not declared yet; only the
    // constructor is here, so the vtable must not be emitted from this
    // declaration.
    class CChrDrvPadVolley : public CChrDrv {
        public:
            CChrDrvPadVolley();

        protected:
            unsigned char m_Unk04[0xB4 - 0x04];
    };

    // INCOMPLETE: the pad driver of the rhythm receive party game, size
    // 0xB4 (CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive::CreateDriver_). Also
    // derived from the DOL's CChrDrvPad; only the constructor is here.
    class CChrDrvPadVolleyRhythmReceive : public CChrDrv {
        public:
            CChrDrvPadVolleyRhythmReceive();

        protected:
            unsigned char m_Unk04[0xB4 - 0x04];
    };
}

#endif /* !MSW_CCHRDRVPADVOLLEY_HPP_ */
