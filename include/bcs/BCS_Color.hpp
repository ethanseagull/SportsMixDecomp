#ifndef BCS_BCS_COLOR_HPP_
#define BCS_BCS_COLOR_HPP_

namespace bcs {

    // An RGBA colour, four bytes, passed by value to the DOL's drawing and
    // material functions (bcs::CMdlObj::SetMatTevColor and others).
    //
    // Only the constructors seen in module static initialisers are written.
    // There every colour of a table is first set to opaque white as a word
    // and then given its four bytes (DodgeF game/dodge/CChrMarkerDodge.cpp,
    // BasketF CBallMarkerBasket), so the four-byte constructor starts from
    // white too. (member names guessed)
    struct BCS_Color {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;

        // The empty destructor is what makes the compiler return a colour
        // through a hidden pointer, as msw::CChrMarker::GetColor_ does
        // (VolleyF CChrMarkerVolleyJump); without it a four-byte struct
        // comes back in r3.
        ~BCS_Color() {}

        BCS_Color() { SetWhite(); }

        // A copy moves the colour as one word (VolleyF CChrMarkerVolleyJump
        // passes GetColor_'s result on that way); the implicit copy would
        // move the four bytes one by one.
        BCS_Color(const BCS_Color &c)
        {
            *reinterpret_cast<unsigned long *>(this) = *reinterpret_cast<const unsigned long *>(&c);
        }

        BCS_Color(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca)
        {
            SetWhite();
            r = cr;
            g = cg;
            b = cb;
            a = ca;
        }

        void SetWhite() { *reinterpret_cast<unsigned long *>(this) = 0xFFFFFFFF; }
    };
}

#endif /* !BCS_BCS_COLOR_HPP_ */
