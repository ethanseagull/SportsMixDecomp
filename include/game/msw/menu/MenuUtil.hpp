#ifndef MENU_MENUUTIL_HPP_
#define MENU_MENUUTIL_HPP_

// Free functions of msw::menu that live in the DOL (0x8019B2C4 onwards).
// Which header declared them is not known: the file name is a guess. Only
// the ones used so far.

struct _GXTexObj;

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
    struct FVEC3;
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
    namespace math {
        struct VEC2;
    }
}

namespace msw {
    namespace menu {
        // Added for BasketF game/basket/CCockpitBasket.cpp: the players per
        // team for play type nType (GameWork byte 6). Types guessed.
        long GetTeamPlayerNum(long nType);
        // Added for VolleyF game/volley/CCockpitVolleyCoder.cpp: the players
        // per team of the current game (type guessed).
        long GetTeamPlayerNum();
        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp (return
        // types and argument names guessed):
        // A picture pane called pName showing player nPlayer's face, made
        // under pParent; null on failure.
        nw4r::lyt::Pane *CreateFaceIconPicturePane(long nPlayer, bcs::G2D::CPaneCtrl *pPaneCtrl,
                                                   nw4r::lyt::Pane *pParent, const char *pName);
        // The scale of a cockpit face icon for character nChr.
        const nw4r::math::VEC2 &GetCockpitFaceIconScale(long nChr);
        // nValue as text in pOut, through the swprintf format pFormat.
        void GetNumericText(long nValue, const wchar_t *pFormat, wchar_t *pOut);
        // The screen position of the world position rPos for view nView.
        void GetWorldScreenPosition(const bcs::FVEC3 &rPos, long nView, bcs::FVEC3 &rOut);

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

        // (added for game/menu/CBackIcon.cpp; return type and argument
        // name guessed) Fills text pane pPane with the key text of the
        // current controller style; nKind picks one of two texts (the back
        // button passes 1 while the remote of a pad in pointer style
        // points off the screen).
        void ReplaceKeyTextByPane(nw4r::lyt::Pane *pPane, int nKind);

        // (added for game/menu/CWiFiFrindVS.cpp; argument names guessed)
        // Shows the texture pTexObj in a new picture pane named pName, made
        // in pPaneCtrl's layout over pPane. Returns the new pane
        // (game/menu/CFriendList.cpp keeps it; the type is a guess).
        nw4r::lyt::Pane *CreateMiiPicturePane(const _GXTexObj *pTexObj, bcs::G2D::CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pPane,
                                  const char *pName);
    }
}

#endif /* !MENU_MENUUTIL_HPP_ */
