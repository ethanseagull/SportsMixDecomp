#ifndef NW4R_MATH_TYPES_H_
#define NW4R_MATH_TYPES_H_

// Vector types of nw4r::math. Only what the game's code needs so far. The
// file name follows how other NW4R decompilations lay the library out
// (kiwi515/ogws); written for this project, no code was taken from them.

namespace nw4r {
    namespace math {

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
    }
}

#endif /* !NW4R_MATH_TYPES_H_ */
