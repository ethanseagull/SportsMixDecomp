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

        namespace detail {
            // (DOL; added for bcs::CAnmBlendMotion, names guessed from the
            // inline wrappers below) e to the x, and the natural logarithm.
            float FExp(float x);
            float FLog(float x);

            // A quiet NaN, 0x7FC00000 (the constant FLog returns in
            // bcs::CAnmBlendMotion; 0.0f / 0.0f folds to the negative one).
            union FloatBits {
                unsigned long u;
                float f;
            };
            static const FloatBits sQuietNaN = {0x7FC00000};
        }

        // 1 / x by the hardware estimate and one Newton step (added for
        // bcs::CAnmBlendMotion, which inlines it: fres, ps_add, ps_mul,
        // ps_nmsub). Paired-single code the compiler does not produce from
        // float code: the same exception as the VEC3 helpers of
        // nw4r/math/types.h, with a plain C++ body for other compilers.
        inline float FInv(register float x)
        {
#ifdef __MWERKS__
            register float est;
            register float twice;
            register float result;

            asm {
                fres est, x
                ps_add twice, est, est
                ps_mul est, est, est
                ps_nmsub result, x, est, twice
            }
            return result;
#else
            return 1.0f / x;
#endif
        }

        // ifPos when cond >= 0, else ifNeg, without a branch (added for
        // bcs::CFrameCtrl::SetFrame, which shows a single-precision fsel:
        // no double constant, no frsp). The compiler has no float form of
        // the instruction (its __fsel works on doubles), so this is the
        // same exception as the paired-single helpers above, with a plain
        // C++ body for other compilers.
        inline float FSelect(register float cond, register float ifPos, register float ifNeg)
        {
#ifdef __MWERKS__
            register float ret;

            asm {
                fsel ret, cond, ifPos, ifNeg
            }
            return ret;
#else
            return cond >= 0.0f ? ifPos : ifNeg;
#endif
        }

        inline float FExp(float x)
        {
            return detail::FExp(x);
        }

        // log(x), a quiet NaN for x <= 0. Inline in bcs::CAnmBlendMotion,
        // with its two strings ("arithmetic.h", line 295).
        inline float FLog(float x)
        {
#line 295
            NW4R_WARNING(x > 0.0f, "FLog: Input is out of the domain.");
            if (x > 0.0f) {
                return detail::FLog(x);
            }
            return detail::sQuietNaN.f;
        }

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
