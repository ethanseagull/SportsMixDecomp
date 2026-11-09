#ifndef NW4R_MATH_TRIANGULAR_H_
#define NW4R_MATH_TRIANGULAR_H_

// Angles of the DOL's NW4R sine table are "indices": 256 to a full turn.
// (macro name guessed) The constant is the 40.743664f the modules multiply
// radians by before calling SinCosFIdx.
#define NW4R_MATH_RAD_TO_FIDX(rad) ((256.0f / (2.0f * 3.14159265f)) * (rad))

namespace nw4r {
    namespace math {

        // In the DOL.
        void SinCosFIdx(float *pSin, float *pCos, float fidx);

        // (name guessed) Inline in the modules.
        inline void SinCosRad(float *pSin, float *pCos, float rad)
        {
            SinCosFIdx(pSin, pCos, NW4R_MATH_RAD_TO_FIDX(rad));
        }
    }
}

#endif /* !NW4R_MATH_TRIANGULAR_H_ */
