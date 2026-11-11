#ifndef BCS_CPADWI_HPP_
#define BCS_CPADWI_HPP_

#include "bcs/FVEC2.hpp"

namespace bcs {

    // The Wii remote input of the engine. Lives in the DOL. Only what the
    // modules use so far; member names and return types are guesses.
    class CPadWI {
        public:
            // (added for MenuF game/menu/CBackIcon.cpp) Writes the screen
            // position remote nPad points at to *pPos; non-zero when it
            // points at the screen. Static: callers pass the vector first.
            static int GetScreenPosDPD_F(FVEC2 *pPos, long nPad);
    };
}

#endif /* !BCS_CPADWI_HPP_ */
