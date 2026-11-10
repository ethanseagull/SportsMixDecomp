#ifndef BCS_BCS_COLOR_HPP_
#define BCS_BCS_COLOR_HPP_

namespace bcs {

    // An RGBA colour, four bytes. Passed by value to the material colour
    // setters of bcs::CScnObj. (member names guessed; the layout is read
    // from msw::CChrMarker::GetColor_, which copies one word out of a table)
    // The empty destructor is what makes the compiler return it through a
    // hidden pointer, as GetColor_ does; without it a four-byte struct comes
    // back in r3.
    struct BCS_Color {
        ~BCS_Color() {}

        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
}

#endif /* !BCS_BCS_COLOR_HPP_ */
