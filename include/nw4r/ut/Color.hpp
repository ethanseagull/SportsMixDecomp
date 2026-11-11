#ifndef NW4R_UT_COLOR_HPP_
#define NW4R_UT_COLOR_HPP_

#include "revolution/gx/GXStruct.h"

namespace nw4r {
    namespace ut {

        // An RGBA colour, four bytes. Only what bcs::BCS_Color needs is
        // written. The empty destructor is emitted weak where a table of
        // colours is destroyed (MenuF game/menu/CChrMenuCtrl keeps the copy
        // the module links); the default constructor sets opaque white as
        // one word, which is why module colour tables are first filled with
        // white and then given their bytes. (member names guessed)
        struct Color {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;

            Color() { SetWhite(); }
            explicit Color(unsigned long rgba) { *reinterpret_cast<unsigned long *>(this) = rgba; }
            // Copied as one word, not byte by byte.
            Color(const Color &c) { *reinterpret_cast<unsigned long *>(this) = *reinterpret_cast<const unsigned long *>(&c); }
            ~Color() {}

            void SetWhite() { *reinterpret_cast<unsigned long *>(this) = 0xFFFFFFFF; }

            // Added for MenuF game/menu/CFriendInfo.cpp: assigning a packed
            // RGBA value stores one word (assigning a Color copies bytes).
            Color &operator=(unsigned long rgba)
            {
                *reinterpret_cast<unsigned long *>(this) = rgba;
                return *this;
            }

            // The colour as GX takes it (added for the DOL's
            // game/msw/CGameExDemoBase.cpp, which passes colours to
            // GXSetTevColor).
            operator GXColor() const { return *reinterpret_cast<const GXColor *>(this); }
        };
    }
}

#endif /* !NW4R_UT_COLOR_HPP_ */
