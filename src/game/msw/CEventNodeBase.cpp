#include "game/msw/CEventNodeSimple.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"

namespace {

    // The smallest normalised float (FLT_MIN): scales and cosines below it
    // are treated as zero.
    const float SMALLEST = 1.17549435e-38f;

    // A row of a result's rotation-translation matrix, read as a vector.
    inline bcs::FVEC3 &Row(nw4r::g3d::ChrAnmResult &rResult, int nRow)
    {
        return *reinterpret_cast<bcs::FVEC3 *>(rResult.rt[nRow]);
    }

    // rOut = rA x rB, made of length 1 (or zero).
    inline void CrossNormalize(const bcs::FVEC3 &rA, const bcs::FVEC3 &rB, bcs::FVEC3 &rOut)
    {
        PSVECCrossProduct((const Vec *)&rA, (const Vec *)&rB, (Vec *)&rOut);
        bcs::BCS_Vec_Normalize(rOut, rOut);
    }

    // x clamped to [-1, 1], so asin accepts it.
    inline float ClampUnit(float x)
    {
        return x > 1.0f ? 1.0f : (x < -1.0f ? -1.0f : x);
    }
}

namespace msw {

    // Node nId of anm at frame fFrame; a looping animation wraps the frame.
    void CEventNodeBase::_GetDataResult(nw4r::g3d::ResAnmChr anm, nw4r::g3d::ChrAnmResult &rResult, float fFrame,
                                        long nId)
    {
        if (anm.GetAnmPolicy() == 1) {
            fFrame = fmodf(fFrame, anm.GetNumFrame());
        }
        anm.GetAnmResult(&rResult, nId, fFrame);
    }

    // Mirrors a result: nFlip 1 about the plane x = rCenter.x (the
    // translation becomes rCenter.x minus it), 2 turns it half round the Y
    // axis, 3 does both. The matrix is made orthonormal again after each.
    void CEventNodeBase::_GetFlipResult(nw4r::g3d::ChrAnmResult &rResult, const bcs::FVEC3 &rCenter, long nFlip)
    {
        bcs::FVEC3 &rRow0 = Row(rResult, 0);
        bcs::FVEC3 &rRow1 = Row(rResult, 1);
        bcs::FVEC3 &rRow2 = Row(rResult, 2);

        switch (nFlip) {
            case 3:
                rResult.rt[0][0] = -rResult.rt[0][0];
                rResult.rt[0][2] = -rResult.rt[0][2];
                rResult.rt[2][0] = -rResult.rt[2][0];
                rResult.rt[2][2] = -rResult.rt[2][2];
                CrossNormalize(rRow2, rRow0, rRow1);
                CrossNormalize(rRow0, rRow1, rRow2);
                rResult.flags &= ~0x80000000;
                // fall through
            case 1:
                rResult.rt[1][0] = -rResult.rt[1][0];
                rResult.rt[2][0] = -rResult.rt[2][0];
                rResult.rt[0][3] = rCenter.x - rResult.rt[0][3];
                CrossNormalize(rRow1, rRow2, rRow0);
                CrossNormalize(rRow2, rRow0, rRow1);
                rResult.flags &= ~0x80000000;
                break;
            case 2:
                rResult.rt[0][0] = -rResult.rt[0][0];
                rResult.rt[0][2] = -rResult.rt[0][2];
                rResult.rt[2][0] = -rResult.rt[2][0];
                rResult.rt[2][2] = -rResult.rt[2][2];
                CrossNormalize(rRow2, rRow0, rRow1);
                CrossNormalize(rRow0, rRow1, rRow2);
                rResult.flags &= ~0x80000000;
                break;
        }
    }

    // The X, Y and Z angles (each in [0, 2 pi]) of rMtx with the scale
    // rScale divided out. A scale below FLT_MIN gives no rotation.
    void CEventNodeBase::_GetMtxRot(bcs::FVEC3 &rRot, const bcs::FVEC3 &rScale, const bcs::FMTX34 &rMtx)
    {
        if (rScale.x < SMALLEST || rScale.y < SMALLEST || rScale.z < SMALLEST) {
            rRot.Set(0.0f, 0.0f, 0.0f);
            return;
        }

        float fRotY = asin(ClampUnit(-rMtx.m[2][0] / rScale.x));
        if (nw4r::math::CosRad(fRotY) >= SMALLEST) {
            rRot.x = bcs::ROUND_PI(nw4r::math::Atan2Rad(rMtx.m[2][1] / rScale.y, rMtx.m[2][2] / rScale.z));
            rRot.y = bcs::ROUND_PI(fRotY);
            rRot.z = bcs::ROUND_PI(nw4r::math::Atan2Rad(rMtx.m[1][0], rMtx.m[0][0]));
        } else {
            // Gimbal lock: the X angle is taken as zero.
            rRot.x = 0.0f;
            rRot.y = bcs::ROUND_PI(fRotY);
            rRot.z = bcs::ROUND_PI(nw4r::math::Atan2Rad(-rMtx.m[0][1], rMtx.m[1][1]));
        }
    }

    // The X, Y and Z angles (each in [0, 2 pi]) of a result: zero for a
    // result without rotation, its raw angles (in degrees) when flag bit 31
    // says they are valid, otherwise read from its matrix.
    void CEventNodeBase::_GetDataRot(bcs::FVEC3 &rRot, const nw4r::g3d::ChrAnmResult &rResult)
    {
        unsigned long flags = rResult.flags;

        if (flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO) {
            rRot.Set(0.0f, 0.0f, 0.0f);
        } else if (flags & 0x80000000) {
            rRot.x = bcs::ROUND_PI(bcs::DEG_TO_RAD(rResult.rawR.x));
            rRot.y = bcs::ROUND_PI(bcs::DEG_TO_RAD(rResult.rawR.y));
            rRot.z = bcs::ROUND_PI(bcs::DEG_TO_RAD(rResult.rawR.z));
        } else {
            float fRotY = asin(ClampUnit(-rResult.rt[2][0]));
            if (nw4r::math::CosRad(fRotY) >= SMALLEST) {
                rRot.x = bcs::ROUND_PI(nw4r::math::Atan2Rad(rResult.rt[2][1], rResult.rt[2][2]));
                rRot.y = bcs::ROUND_PI(fRotY);
                rRot.z = bcs::ROUND_PI(nw4r::math::Atan2Rad(rResult.rt[1][0], rResult.rt[0][0]));
            } else {
                rRot.x = 0.0f;
                rRot.y = bcs::ROUND_PI(fRotY);
                rRot.z = bcs::ROUND_PI(nw4r::math::Atan2Rad(-rResult.rt[0][1], rResult.rt[1][1]));
            }
        }
    }
}
