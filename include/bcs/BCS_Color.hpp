#ifndef BCS_BCS_COLOR_HPP_
#define BCS_BCS_COLOR_HPP_

#include "nw4r/ut/Color.hpp"

namespace bcs {

    // An RGBA colour, four bytes, passed by value to the DOL's drawing and
    // material functions (bcs::CMdlObj::SetMatTevColor and others).
    //
    // Only the constructors seen in module static initialisers are written.
    // There every colour of a table is first set to opaque white as a word
    // and then given its four bytes (DodgeF game/dodge/CChrMarkerDodge.cpp,
    // BasketF CBallMarkerBasket), so the four-byte constructor starts from
    // white too. (member names guessed)
    // Derives from nw4r::ut::Color: MenuF game/menu/CChrMenuCtrl emits
    // nw4r::ut::Color's weak destructor next to its BCS_Color tables, and its
    // static initialiser is reproduced exactly only with this base.
    struct BCS_Color : nw4r::ut::Color {
        // The base's empty destructor is what makes the compiler return a
        // colour through a hidden pointer, as msw::CChrMarker::GetColor_ does
        // (VolleyF CChrMarkerVolleyJump); without one a four-byte struct
        // comes back in r3. The base also copies the colour as one word.
        BCS_Color() {}

        BCS_Color(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca)
        {
            r = cr;
            g = cg;
            b = cb;
            a = ca;
        }

        // (guessed, after nw4r::ut::Color) A colour given as one word,
        // 0xRRGGBBAA, and stored as a word:
        // msw::SelectMenu::CCourtBallMdlObj::SetGoldColor (MenuF) builds its
        // colours with one store each.
        explicit BCS_Color(unsigned long rgba) : nw4r::ut::Color(rgba) {}
    };
}

#endif /* !BCS_BCS_COLOR_HPP_ */
