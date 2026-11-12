#ifndef BCS_BCS_MATH_HPP_
#define BCS_BCS_MATH_HPP_

// Scalar helpers of the bcs namespace. Which header declared them is not
// known: the file name is a guess.

namespace bcs {

    // deg in radians. Inline: BasketF and DodgeF each have an out-of-line
    // copy (DEG_TO_RAD__3bcsFf, a load of pi / 180 and a multiply).
    inline float DEG_TO_RAD(float deg)
    {
        return deg * (3.14159265f / 180.0f);
    }

    // (added for BasketF game/basket/CChrCoreBasketNinja.cpp) The absolute
    // value. A template: BasketF has a weak copy named BCS_Abs<f>.
    template <class T>
    inline T BCS_Abs(T x);

    template <>
    inline float BCS_Abs<float>(float x)
    {
        return __fabsf(x);
    }

    // (added for the same unit; BasketF has a weak copy, ROUND_PI__3bcsFf)
    // An angle brought into [0, 2 pi].
    inline float ROUND_PI(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // (meaning guessed from the name) The arc cosine of x, in radians;
    // probably table based. A DOL function.
    float BCS_ACosT(float x);

    struct FMTX34;

    // (added for MenuF's msw::CTopMenuBall; field names guessed) A
    // quaternion, laid out as revolution's Quaternion.
    struct QUATERNION {
        float x, y, z, w;
    };

    // (DOL) The rotation of the matrix as a quaternion.
    void BCS_Quat_FromMatrix(QUATERNION &rQuat, const FMTX34 &rMtx);
    // (DOL fn_80054CB0, the function after it; name and argument meanings
    // guessed from bcs::CAnmBlendMotion, which blends rotations with it)
    // The spherical interpolation from rFrom to rTo by fRate (0 to 1).
    void BCS_Quat_Slerp(QUATERNION &rOut, const QUATERNION &rFrom, const QUATERNION &rTo, float fRate);
}

#endif /* !BCS_BCS_MATH_HPP_ */
