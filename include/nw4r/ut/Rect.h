#ifndef NW4R_UT_RECT_H_
#define NW4R_UT_RECT_H_

// Written for this project from what MenuF game/menu/CBackground.cpp reads
// (right - left of a text box's rectangle); the names follow the public
// ogws decompilation's description of nw4r::ut::Rect (no code copied).

namespace nw4r {
    namespace ut {

        struct Rect {
            float left;   // 0x00
            float top;    // 0x04
            float right;  // 0x08
            float bottom; // 0x0C

            float GetWidth() const { return right - left; }
        };
    }
}

#endif /* !NW4R_UT_RECT_H_ */
