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
            // Added for MenuF game/menu/CBgDisplay.cpp (emitted out of line
            // there as __ct__Q34nw4r2ut5ColorFiiii).
            Color(int red, int green, int blue, int alpha) { Set(red, green, blue, alpha); }
            // Copied as one word, not byte by byte.
            Color(const Color &c) { *reinterpret_cast<unsigned long *>(this) = *reinterpret_cast<const unsigned long *>(&c); }
            ~Color() {}

            void Set(int red, int green, int blue, int alpha)
            {
                r = (unsigned char)red;
                g = (unsigned char)green;
                b = (unsigned char)blue;
                a = (unsigned char)alpha;
            }

            void SetWhite() { *reinterpret_cast<unsigned long *>(this) = 0xFFFFFFFF; }

            // Added for MenuF game/menu/CFriendInfo.cpp: assigning a packed
            // RGBA value stores one word (assigning a Color copies bytes).
            Color &operator=(unsigned long rgba)
            {
                *reinterpret_cast<unsigned long *>(this) = rgba;
                return *this;
            }

            // (added for nw4r::g3d::ApplyClrAnmResult in the DOL's
            // animation file) Takes a GX colour as one word.
            Color &operator=(const GXColor &c)
            {
                *reinterpret_cast<unsigned long *>(this) = *reinterpret_cast<const unsigned long *>(&c);
                return *this;
            }

            // (added for nw4r::g3d::ApplyClrAnmResult) The colour as an
            // output argument of the GX getters.
            GXColor *ToGXColorPtr() { return reinterpret_cast<GXColor *>(this); }

            // The colour as GX takes it (added for the DOL's
            // game/msw/CGameExDemoBase.cpp, which passes colours to
            // GXSetTevColor).
            operator GXColor() const { return *reinterpret_cast<const GXColor *>(this); }

            // The colour as one packed RGBA word (added for the DOL's
            // game/bcs/G2D/CLineStripDL.cpp, which writes it to a display
            // list).
            unsigned long ToU32() const { return *reinterpret_cast<const unsigned long *>(this); }
        };
    }
}

#endif /* !NW4R_UT_COLOR_HPP_ */
