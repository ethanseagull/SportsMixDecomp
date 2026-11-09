#ifndef BCS_BCS_VEC_HPP_
#define BCS_BCS_VEC_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/math/arithmetic.h"
#include "revolution/mtx.h"

namespace bcs {

    // rOut = rFrom turned towards rTo by t of the angle between them. In the
    // DOL.
    void BCS_Vec_Slerp(FVEC3 &rOut, const FVEC3 &rFrom, const FVEC3 &rTo, float t);

    // The three routines below are inline in the modules and written in
    // paired-single assembly in the original (x and y as a pair, z alone; the
    // compiler does not produce these instructions from float code). They are
    // most likely NW4R's VEC3Lerp, VEC3Add and VEC3Scale; the names here are
    // guessed and follow bcs::FVEC3Lerp. Written from what
    // msw::CTournamentCamera's SetValueCamera and MainCamera and
    // msw::CTournamentMan::SetCameraRate show after inlining. The compiler
    // reorders these instructions among the ones around them and numbers the
    // registers by the order of the declarations, so both are part of the
    // match.

    // (name guessed) pOut = p1 + (p2 - p1) * t.
    // The same routine as bcs::FVEC3Lerp of bcs/FVEC3.hpp, with the
    // instructions in the order msw::CTournamentMan::SetCameraRate has them,
    // where nothing was moved. With the order of that header
    // msw::CTournamentCamera::MainCamera comes out with two instructions
    // swapped. One of the two routines should go.
    inline FVEC3 *FVEC3Lerp2(register FVEC3 *pOut, register const FVEC3 *p1, register const FVEC3 *p2,
                             register float t)
    {
        register float from;
        register float to;
        register float work;

        asm {
            psq_l from, 0(p1), 0, 0
            psq_l to, 0(p2), 0, 0
            ps_sub work, to, from
            ps_madds0 work, work, t, from
            psq_st work, 0(pOut), 0, 0
            psq_l from, 8(p1), 1, 0
            psq_l to, 8(p2), 1, 0
            ps_sub work, to, from
            ps_madds0 work, work, t, from
            psq_st work, 8(pOut), 1, 0
        }
        return pOut;
    }

    // (name guessed) pOut = p1 + p2.
    inline FVEC3 *FVEC3Add(register FVEC3 *pOut, register const FVEC3 *p1, register const FVEC3 *p2)
    {
        register float a;
        register float b;
        register float work;

        asm {
            psq_l a, 0(p1), 0, 0
            psq_l b, 0(p2), 0, 0
            ps_add work, a, b
            psq_st work, 0(pOut), 0, 0
            psq_l a, 8(p1), 1, 0
            psq_l b, 8(p2), 1, 0
            ps_add work, a, b
            psq_st work, 8(pOut), 1, 0
        }
        return pOut;
    }

    // (name guessed) pOut = pIn * scale.
    inline FVEC3 *FVEC3Scale(register FVEC3 *pOut, register const FVEC3 *pIn, register float scale)
    {
        register float work;
        register float in;

        asm {
            psq_l in, 0(pIn), 0, 0
            ps_muls0 work, in, scale
            psq_st work, 0(pOut), 0, 0
            psq_l in, 8(pIn), 1, 0
            ps_muls0 work, in, scale
            psq_st work, 8(pOut), 1, 0
        }
        return pOut;
    }

    // (name guessed, after BCS_Vec_Slerp, which the callers use next to it
    // with the same arguments) rOut = rFrom + (rTo - rFrom) * t.
    inline void BCS_Vec_Lerp(FVEC3 &rOut, const FVEC3 &rFrom, const FVEC3 &rTo, float t)
    {
        FVEC3Lerp2(&rOut, &rFrom, &rTo, t);
    }

    // (name guessed) rOut = r1 + r2 by way of a temporary. This stands in
    // for `r1 + r2` as the original most likely had it: the callers show the
    // sum written to one vector on the stack and copied float by float to a
    // second, which is what an operator+ returning a base class (NW4R's VEC3)
    // and a converting constructor of FVEC3 give. FVEC3 has neither yet.
    inline void FVEC3Sum(FVEC3 &rOut, const FVEC3 &r1, const FVEC3 &r2)
    {
        FVEC3 sum;

        FVEC3Add(&sum, &r1, &r2);
        rOut = sum;
    }

    // (name guessed) The square of the distance between two points.
    inline float FVEC3DistSq(const FVEC3 *p1, const FVEC3 *p2)
    {
        return PSVECSquareDistance((const Vec *)p1, (const Vec *)p2);
    }

    // (name guessed) The distance between two points.
    inline float FVEC3Dist(const FVEC3 *p1, const FVEC3 *p2)
    {
        return nw4r::math::FSqrt(FVEC3DistSq(p1, p2));
    }
}

#endif /* !BCS_BCS_VEC_HPP_ */
