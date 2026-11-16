#ifndef CCOCKPITMENUNEXT_HPP_
#define CCOCKPITMENUNEXT_HPP_

#include "game/msw/CCockpitMenuBaseButtonA.hpp"

namespace msw {

    // (class name guessed; added for game/msw/cockpit.cpp) Size 0x9C. A
    // window answered with one A button, opened by the DOL's fn_801DFB88
    // with the button text "Btn_A_Next" of CockpitTelop. Lives in the DOL
    // (constructor 0x801EF8C8, vtable lbl_804862F4); only what the callers
    // need so far.
    class CCockpitMenuNext : public CCockpitMenuBaseButtonA {
        public:
            // The last argument is stored at 0x84.
            CCockpitMenuNext(long nId, float fSortZ, unsigned long nArg);
            // 0, or a negative number that tells which step failed.
            virtual int Set();
    };
}

#endif /* !CCOCKPITMENUNEXT_HPP_ */
