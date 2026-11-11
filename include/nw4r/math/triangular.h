#ifndef NW4R_MATH_TRIANGULAR_H_
#define NW4R_MATH_TRIANGULAR_H_

// Angles of the DOL's NW4R sine table are "indices": 256 to a full turn.
// (macro name guessed) The constant is the 40.743664f the modules multiply
// radians by before calling SinCosFIdx.
#define NW4R_MATH_RAD_TO_FIDX(rad) ((256.0f / (2.0f * 3.14159265f)) * (rad))
// (macro name guessed) The other way: the 0.024543693f the modules multiply
// Atan2FIdx's result by.
#define NW4R_MATH_FIDX_TO_RAD(fidx) (((2.0f * 3.14159265f) / 256.0f) * (fidx))
// (macro names guessed; added for MenuF game/menu/unit_6EE84.cpp) Degrees
// to indices (the 0.7111111f the modules multiply by), and to radians
// through them.
#define NW4R_MATH_DEG_TO_FIDX(deg) ((256.0f / 360.0f) * (deg))
#define NW4R_MATH_DEG_TO_RAD(deg) NW4R_MATH_FIDX_TO_RAD(NW4R_MATH_DEG_TO_FIDX(deg))

namespace nw4r {
    namespace math {

        // In the DOL.
        void SinCosFIdx(float *pSin, float *pCos, float fidx);

        // In the DOL.
        float SinFIdx(float fidx);
        float CosFIdx(float fidx);

        // (names guessed) Inline in the modules.
        inline float SinRad(float rad)
        {
            return SinFIdx(NW4R_MATH_RAD_TO_FIDX(rad));
        }

        inline float CosRad(float rad)
        {
            return CosFIdx(NW4R_MATH_RAD_TO_FIDX(rad));
        }

        // (name guessed) Inline in the modules.
        inline void SinCosRad(float *pSin, float *pCos, float rad)
        {
            SinCosFIdx(pSin, pCos, NW4R_MATH_RAD_TO_FIDX(rad));
        }

        // In the DOL. The angle of (x, y), as an index.
        float Atan2FIdx(float y, float x);

        // (name guessed) Inline in the modules.
        inline float Atan2Rad(float y, float x)
        {
            return NW4R_MATH_FIDX_TO_RAD(Atan2FIdx(y, x));
        }
    }
}

#endif /* !NW4R_MATH_TRIANGULAR_H_ */
