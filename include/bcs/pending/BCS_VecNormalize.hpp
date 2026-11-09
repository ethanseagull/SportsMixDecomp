#ifndef PENDING_BCS_VECNORMALIZE_HPP_
#define PENDING_BCS_VECNORMALIZE_HPP_

// PENDING MERGE into bcs/BCS_Vec.hpp and revolution/mtx.h, which this
// session may not edit. Everything here is an addition (nothing of those
// headers is repeated), so it can be included next to them. Once the
// declarations are in the real headers, delete this file.

#include "bcs/FVEC3.hpp"
#include "revolution/mtx.h"

#ifdef __cplusplus
extern "C" {
#endif

// For revolution/mtx.h. In the DOL.
void PSVECNormalize(const Vec *pIn, Vec *pOut);

#ifdef __cplusplus
}
#endif

namespace bcs {

    // (name guessed) The square of the length of a vector. Like the routines
    // of bcs/BCS_Vec.hpp this one is inline in the modules and written in
    // paired-single assembly in the original (y and z as a pair, x alone;
    // the compiler does not produce these instructions from float code); it
    // is most likely NW4R's VEC3LenSq. Written from what
    // msw::CCollManBoss::DetectMapHit_ (BossF) and
    // msw::CCollMan::DetectMapHitOuterCircle_ (DOL) show after inlining.
    inline float FVEC3LenSq(register const FVEC3 *p)
    {
        register float x;
        register float yz;
        register float work;

        asm {
            psq_l yz, 4(p), 0, 0
            ps_mul yz, yz, yz
            psq_l x, 0(p), 1, 0
            ps_madd work, x, x, yz
            ps_sum0 work, work, yz, yz
        }
        return work;
    }

    // (name guessed) rOut = rIn scaled to length 1, or zero when rIn is zero.
    inline void BCS_Vec_Normalize(FVEC3 &rOut, const FVEC3 &rIn)
    {
        if (FVEC3LenSq(&rIn) == 0.0f) {
            rOut.Set(0.0f, 0.0f, 0.0f);
        } else {
            PSVECNormalize((const Vec *)&rIn, (Vec *)&rOut);
        }
    }
}

#endif /* !PENDING_BCS_VECNORMALIZE_HPP_ */
