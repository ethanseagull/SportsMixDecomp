#ifndef MENU_MENUUTIL_HPP_
#define MENU_MENUUTIL_HPP_

// Free functions of msw::menu that live in the DOL (0x8019B2C4 onwards).
// Which header declared them is not known: the file name is a guess. Only
// the ones used so far.

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace menu {
        // nNum as text, in a static buffer (swprintf). The second argument
        // is passed -1 by the coin counters; what it selects is not known.
        const wchar_t *GetNumString(unsigned long nNum, unsigned long nArg);

        // Added for VolleyF game/volley/CCockpitChrIcon2DVolley.cpp. Makes a
        // picture pane named pName showing character nChr's face, sized
        // like pBase, in layout pPaneCtrl; null on failure. (what the panes
        // are attached to is not known; return type guessed from the
        // caller, which writes pane members through it)
        nw4r::lyt::Pane *CreateFaceIconPicturePane(long nChr, bcs::G2D::CPaneCtrl *pPaneCtrl,
                                                   nw4r::lyt::Pane *pBase, const char *pName);
    }
}

#endif /* !MENU_MENUUTIL_HPP_ */
