#ifndef BCS_BCS_VEC_HPP_
#define BCS_BCS_VEC_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/math/arithmetic.h"
#include "revolution/mtx.h"

namespace bcs {

    // rOut = rFrom turned towards rTo by t of the angle between them. In the
    // DOL.
    void BCS_Vec_Slerp(FVEC3 &rOut, const FVEC3 &rFrom, const FVEC3 &rTo, float t);
    // (DOL; added for VolleyF CComDrvVolleyGame; meaning guessed from the
    // name) The point at t of a beta spline from rFrom through rCtrl to
    // rTo, in the XZ plane.
    void BCS_Vec_BetaSplineCurveXZ(FVEC3 &rOut, const FVEC3 &rFrom, const FVEC3 &rCtrl, const FVEC3 &rTo, float t);
    // (DOL; added for DodgeF CCameraDodgeGame; meaning guessed from the
    // name and its use) The value at t of a beta spline through the three
    // points (x0, y0), (x1, y1), (x2, y2).
    float BCS_Vec_BetaSplineCurveInter3(float x0, float y0, float x1, float y1, float x2, float y2, float t);

    // (argument names guessed) rOut = (sin(rotY), 0, cos(rotY)) * len: a
    // vector of length len along +Z turned by rotY (radians) about Y. In the
    // DOL.
    void BCS_VecZ_RotY(FVEC3 &rOut, float len, float rotY);
    // (added for BasketF game/basket/CChrCoreBasketKinopio.cpp) The same
    // for a vector of length len along X.
    void BCS_VecX_RotY(FVEC3 &rOut, float len, float rotY);

    // (argument names guessed) rOut = rIn turned by rotY (radians) about Y.
    // In the DOL.
    void BCS_Vec_RotY(FVEC3 &rOut, const FVEC3 &rIn, float rotY);
    // (added for TourF game/tour/CTournamentRouteMan.cpp) The same about X.
    // In the DOL.
    void BCS_Vec_RotX(FVEC3 &rOut, const FVEC3 &rIn, float rotX);

    // (argument names and meaning guessed) The angle (radians) between two
    // vectors; msw::CStgGmk_WaterPillarCluster::GetZoneByPos gives it the
    // sign of the cross product's y itself. In the DOL.
    float BCS_Vec_GetRot(const FVEC3 &rFrom, const FVEC3 &rTo);

    // (argument names and meaning guessed) The foot of the perpendicular
    // from rPos to the line through rLinePos along rLineDir, in rOut; not
    // zero when there is one. In the DOL.
    int BCS_GetPosToLineCross(FVEC3 &rOut, const FVEC3 &rPos, const FVEC3 &rLinePos, const FVEC3 &rLineDir);

    // (argument names and meaning guessed) The crossing point (*pX, *pZ)
    // of the segments (x0, z0)-(x1, z1) and (x2, z2)-(x3, z3) in the XZ
    // plane; not zero when they cross. In the DOL.
    int BCS_GetSegLineToSegLineCross(float *pX, float *pZ, float x0, float z0, float x1, float z1, float x2,
                                     float z2, float x3, float z3);

    // (added for BasketF game/basket/CComManBasketGame.cpp; meaning guessed
    // from the name) The crossing point (*pX, *pZ) of the line through
    // (x0, z0)-(x1, z1) with the segment (x2, z2)-(x3, z3); not zero when
    // there is one. In the DOL.
    int BCS_GetLineToSegLineCross(float *pX, float *pZ, float x0, float z0, float x1, float z1, float x2,
                                  float z2, float x3, float z3);

    // (argument names guessed) The same with length 1. In the DOL.
    void BCS_VecOneZ_RotY(FVEC3 &rOut, float rotY);
    // (added for the DOL's pinball ball) The unit X vector turned by rotY.
    void BCS_VecOneX_RotY(FVEC3 &rOut, float rotY);
    // (added for HockeyF game/hockey/CStgGmk_IceCube_Act_Hockey.cpp;
    // meaning guessed from the name) rIn turned by rotZ about the Z axis.
    // In the DOL.
    void BCS_Vec_RotZ(FVEC3 &rOut, const FVEC3 &rIn, float rotZ);

    // (added for the DOL's pinball ball) The crossing of the lines (not
    // segments) through (x0, z0)-(x1, z1) and (x2, z2)-(x3, z3); false
    // when they are parallel.
    int BCS_GetLineToLineCross(float *pX, float *pZ, float x0, float z0, float x1, float z1, float x2, float z2,
                               float x3, float z3);

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

    // (name guessed) pOut = p1 - p2. Written from what
    // msw::CBallManDodge::GetNearPosBall_ (DodgeF) shows after inlining.
    inline FVEC3 *FVEC3Sub(register FVEC3 *pOut, register const FVEC3 *p1, register const FVEC3 *p2)
    {
        register float a;
        register float b;
        register float work;

        asm {
            psq_l a, 0(p1), 0, 0
            psq_l b, 0(p2), 0, 0
            ps_sub work, a, b
            psq_st work, 0(pOut), 0, 0
            psq_l a, 8(p1), 1, 0
            psq_l b, 8(p2), 1, 0
            ps_sub work, a, b
            psq_st work, 8(pOut), 1, 0
        }
        return pOut;
    }

    // (name guessed) The length of a vector.
    inline float FVEC3Len(const FVEC3 *p)
    {
        return PSVECMag((const Vec *)p);
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

    // (name guessed) The square of the length of a vector, the other way
    // round from FVEC3LenSq: x and y as a pair, z alone and read as a plain
    // float, so the compiler can reuse a z it already holds. Most likely
    // the SDK's PSVECSquareMag inlined. Paired-single assembly like the
    // routines above; written from what msw::CDropCoinMan::DropCoinPickup
    // (DOL) shows after inlining.
    inline float FVEC3SquareMag(register const FVEC3 *p)
    {
        register float xy;
        register float z;
        register float work;

        z = p->z;
#ifdef __MWERKS__
        asm {
            psq_l xy, 0(p), 0, 0
            ps_mul xy, xy, xy
            ps_madd work, z, z, xy
            ps_sum0 work, work, xy, xy
        }
#else
        work = p->x * p->x + p->y * p->y + z * z;
#endif
        return work;
    }

    // (name guessed) The dot product of two vectors. Paired-single assembly
    // in the original like FVEC3LenSq (y and z as a pair, x alone); most
    // likely NW4R's VEC3Dot. Written from what msw::CDropCoin's bounce
    // (game/msw/CDropCoinMan.cpp) shows after inlining.
    inline float FVEC3Dot(register const FVEC3 *p1, register const FVEC3 *p2)
    {
        register float yz1;
        register float yz2;
        register float x1;
        register float x2;
        register float work;

        asm {
            psq_l yz1, 4(p1), 0, 0
            psq_l yz2, 4(p2), 0, 0
            ps_mul yz1, yz1, yz2
            psq_l x1, 0(p1), 1, 0
            psq_l x2, 0(p2), 1, 0
            ps_madd x2, x1, x2, yz1
            ps_sum0 work, x2, yz1, yz1
        }
        return work;
    }

    // (added for BasketF game/basket/CChrCoreBasketNinja.cpp; BasketF has
    // weak copies of both) The length of rVec, and of rVec in the XZ plane.
    inline float BCS_Vec_GetLen(const FVEC3 &rVec)
    {
        return PSVECMag((const Vec *)&rVec);
    }

    inline float BCS_Vec_GetLenXZ(const FVEC3 &rVec)
    {
        FVEC3 xz;

        xz.Set(rVec.x, 0.0f, rVec.z);
        return BCS_Vec_GetLen(xz);
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

#endif /* !BCS_BCS_VEC_HPP_ */
