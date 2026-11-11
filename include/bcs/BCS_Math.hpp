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
}

#endif /* !BCS_BCS_MATH_HPP_ */
