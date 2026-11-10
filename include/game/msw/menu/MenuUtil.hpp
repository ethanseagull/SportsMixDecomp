#ifndef MENU_MENUUTIL_HPP_
#define MENU_MENUUTIL_HPP_

// Free functions of msw::menu that live in the DOL (0x8019B2C4 onwards).
// Which header declared them is not known: the file name is a guess. Only
// the ones used so far.

namespace msw {
    namespace menu {
        // nNum as text, in a static buffer (swprintf). The second argument
        // is passed -1 by the coin counters; what it selects is not known.
        const wchar_t *GetNumString(unsigned long nNum, unsigned long nArg);
    }
}

#endif /* !MENU_MENUUTIL_HPP_ */
