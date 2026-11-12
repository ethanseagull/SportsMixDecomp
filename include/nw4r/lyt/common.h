#ifndef NW4R_LYT_COMMON_H_
#define NW4R_LYT_COMMON_H_

#include "nw4r/db/Panic.hpp"

// Helpers of the layout library. The name of this file is part of the
// binary: the position setters pass "common.h" to their assertions (MenuF
// game/menu/CBgDisplay.cpp has them inline, with their @STRING@ objects; the
// line numbers are what the original passes). Names follow the public ogws
// decompilation's description of nw4r::lyt (from memory, no code copied).

namespace nw4r {
    namespace lyt {
        namespace detail {

            enum {
                HORIZONTALPOSITION_MAX = 3,
                VERTICALPOSITION_MAX = 3
            };

            // A pane's base position packs the horizontal (0 to 2) and the
            // vertical (0 to 2) position into one byte.
            inline unsigned char GetHorizontalPosition(unsigned char var)
            {
                return (unsigned char)(var % HORIZONTALPOSITION_MAX);
            }

            inline unsigned char GetVerticalPosition(unsigned char var)
            {
                return (unsigned char)(var / HORIZONTALPOSITION_MAX);
            }

            inline void SetHorizontalPosition(unsigned char *pVar, unsigned char newVal)
            {
                NW4R_ASSERT_AT((newVal < HORIZONTALPOSITION_MAX), "common.h", 227, "NW4R:Failed assertion newVal < HORIZONTALPOSITION_MAX");
                *pVar = (unsigned char)(GetVerticalPosition(*pVar) * HORIZONTALPOSITION_MAX + newVal);
            }

            inline void SetVerticalPosition(unsigned char *pVar, unsigned char newVal)
            {
                NW4R_ASSERT_AT((newVal < VERTICALPOSITION_MAX), "common.h", 236, "NW4R:Failed assertion newVal < VERTICALPOSITION_MAX");
                *pVar = (unsigned char)(newVal * HORIZONTALPOSITION_MAX + GetHorizontalPosition(*pVar));
            }
        }
    }
}

#endif /* !NW4R_LYT_COMMON_H_ */
