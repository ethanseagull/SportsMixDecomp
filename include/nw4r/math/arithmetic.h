#ifndef NW4R_MATH_ARITHMETIC_H_
#define NW4R_MATH_ARITHMETIC_H_

#include "nw4r/db/Warning.hpp"

// The name of this file is part of the binary: FSqrt passes __FILE__
// ("arithmetic.h") to its warning. Written for this project from the code the
// modules emit for FSqrt. That nw4r/math is split into an arithmetic and a
// triangular header is how other NW4R decompilations lay it out
// (kiwi515/ogws); no code was taken from them.

namespace nw4r {
    namespace math {

        // 1 / sqrt(x). In the DOL.
        float FrSqrt(float x);

        // sqrt(x), 0 for x <= 0. Emitted in each module that uses it, with
        // its two strings as @STRING@FSqrt__Q24nw4r4mathFf objects.
        inline float FSqrt(float x)
        {
// The original has this check on line 627 (the value passed in r4).
#line 627
            NW4R_WARNING(x >= 0.0f, "FSqrt: Input is out of the domain.");
            return x <= 0.0f ? 0.0f : x * FrSqrt(x);
        }
    }
}

#endif /* !NW4R_MATH_ARITHMETIC_H_ */
