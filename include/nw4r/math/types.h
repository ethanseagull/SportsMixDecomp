#ifndef NW4R_MATH_TYPES_H_
#define NW4R_MATH_TYPES_H_

// Vector types of nw4r::math. Only what the game's code needs so far. The
// file name follows how other NW4R decompilations lay the library out
// (kiwi515/ogws); written for this project, no code was taken from them.

namespace nw4r {
    namespace math {

        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp (a pane's
        // scale).
        struct VEC2 {
            float x; // 0x00
            float y; // 0x04

            VEC2()
            {
            }

            VEC2(float fx, float fy)
            {
                x = fx;
                y = fy;
            }
        };

        struct VEC3 {
            float x; // 0x00
            float y; // 0x04
            float z; // 0x08

            VEC3()
            {
            }

            VEC3(float fx, float fy, float fz)
            {
                x = fx;
                y = fy;
                z = fz;
            }
        };

        // A 3x4 matrix, row by row; column 3 is the translation. Added for
        // HockeyF game/hockey/CCockpitHockeyCoin.cpp (a pane's global
        // matrix, copied as words).
        struct MTX34 {
            float _00, _01, _02, _03;
            float _10, _11, _12, _13;
            float _20, _21, _22, _23;
        };

        // (DOL) *pOut = *pM scaled by *pS. Added for the DOL's
        // msw::CStgSrt::GetMtx.
        MTX34 *MTX34Scale(MTX34 *pOut, const MTX34 *pM, const VEC3 *pS);
    }
}

#endif /* !NW4R_MATH_TYPES_H_ */
