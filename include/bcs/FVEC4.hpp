#ifndef BCS_FVEC4_HPP_
#define BCS_FVEC4_HPP_

namespace bcs {

    // Four floats. Only the layout is known: msw::CCollMan::
    // DetectMapHitInnerRgn_ (DOL) takes one, which the VolleyF collision
    // manager fills with a rectangle (left, right, front, back) on the stack.
    struct FVEC4 {
        float x; // 0x00
        float y; // 0x04
        float z; // 0x08
        float w; // 0x0C
    };
}

#endif /* !BCS_FVEC4_HPP_ */
