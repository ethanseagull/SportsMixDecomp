#ifndef BCS_FVEC4_HPP_
#define BCS_FVEC4_HPP_

namespace bcs {

    // Four floats. The name comes from the mangled names of
    // msw::CCollMan::DetectMapHit*Rgn_, which take one as a rectangle on the
    // ground: x = left, y = right, z = front, w = back, filled in that order
    // by msw::CStageMan::Get*Rect (msw::CCollManBasket::DetectMapHit_ in
    // BasketF). Only the layout is known.
    struct FVEC4 {
        float x; // 0x00
        float y; // 0x04
        float z; // 0x08
        float w; // 0x0C

        FVEC4()
        {
        }

        FVEC4(float fx, float fy, float fz, float fw)
        {
            x = fx;
            y = fy;
            z = fz;
            w = fw;
        }

        // A copy moves the floats through float registers (lfs/stfs), as
        // for FVEC3: msw::CCollManDodge::DetectMapHit_ in DodgeF swaps two
        // rectangles that way. Without it the struct is copied as words.
        FVEC4(const FVEC4 &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }
    };
}

#endif /* !BCS_FVEC4_HPP_ */
