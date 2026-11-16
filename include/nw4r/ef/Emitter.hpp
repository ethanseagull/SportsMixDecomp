#ifndef NW4R_EF_EMITTER_HPP_
#define NW4R_EF_EMITTER_HPP_

#include "nw4r/math/types.h"

namespace nw4r {
    namespace ef {

        // Minimal declaration written for this project from the DOL's
        // mangled name. Only what is used so far.
        class Emitter {
            public:
                // (added for game/msw/cockpit.cpp; argument names guessed)
                // Rebuilds *pOut from *pIn; msw::cockpit::GetBillboardOffset
                // passes false, false, 100, false.
                static void RestructMatrix(math::MTX34 *pOut, math::MTX34 *pIn, bool b0, bool b1, signed char n,
                                           bool b2);
        };
    }
}

#endif /* !NW4R_EF_EMITTER_HPP_ */
