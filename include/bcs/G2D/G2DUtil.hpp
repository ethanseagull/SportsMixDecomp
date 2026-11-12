#ifndef BCS_G2D_G2DUTIL_HPP_
#define BCS_G2D_G2DUTIL_HPP_

// Free functions of the bcs::G2D namespace that build panes at run time.
// They live in the DOL. Which header declared them is not known: the file
// name is a guess. Only the ones used so far.
//
// Needs bcs::G2D::CPaneCtrl complete: include bcs/G2D/CPaneCtrl.hpp (or its
// pending copy) first.

struct _GXTexObj;

namespace nw4r {
    namespace lyt {
        class Pane;
    }
    namespace ut {
        class ArchiveFont;
    }
    namespace math {
        struct VEC2;
    }
}

namespace bcs {
    struct FVEC3;

    namespace G2D {
        class CPaneCtrl;
        class CPaneObj;
        class CTexMap;

        // A new empty pane called pName; null on failure.
        nw4r::lyt::Pane *CreateNullPane(const char *pName);
        // A new picture pane called pName showing rTexMap; null on failure.
        nw4r::lyt::Pane *CreatePicturePane(const char *pName, CTexMap &rTexMap);
        // Added for MenuF game/menu/CGameSelect.cpp. The same without a
        // texture map; what the byte is (called with 3) is not known.
        nw4r::lyt::Pane *CreatePicturePane(const char *pName, unsigned char nArg);
        // Added for MenuF game/menu/CGameSelect.cpp. Makes pPaneObj's
        // picture pane show the movie, in the place of pFrame (return type
        // and argument names guessed).
        void SetupMoviePane(CPaneObj *pPaneObj, CPaneObj *pFrame);
        // Added for MenuF game/menu/CPartySelect.cpp. Points the picture
        // pane pPane at the current movie frame (return type guessed).
        void RemapMoviePane(nw4r::lyt::Pane *pPane);
        // A copy, called pName, of pPaneCtrl's pane pSrcName, as an object of
        // pPaneCtrl; null on failure. The radar icons pass their layout's
        // file name as the fourth argument and 0 as the fifth. (what the
        // last two are for is not known; their names are guesses)
        CPaneObj *DuplicatePaneObj(CPaneCtrl *pPaneCtrl, const char *pSrcName, const char *pName,
                                   const char *pLayoutName, int nArg);
        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp: the same
        // from a pane (the last argument is 1 where the game's local static
        // `animUse` is passed).
        CPaneObj *DuplicatePaneObj(CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pSrcPane, const char *pName,
                                   const char *pLayoutName, int bAnimUse);
        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp: a copy,
        // called pName, of pPaneCtrl's pane pSrcPane, without an object.
        nw4r::lyt::Pane *DuplicatePane(CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pSrcPane, const char *pName,
                                       const char *pLayoutName, int bAnimUse);
        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp: gives
        // pPane its parent's size (return type guessed).
        void SetPaneSizeFromParent(nw4r::lyt::Pane *pPane);
        // Added for BossF game/boss/CCockpitBossCoder.cpp (return types and
        // argument names guessed): makes picture pane pPane show what
        // picture pane pSrc shows, or the texture pTexObj in texture map
        // nTexMap.
        void ReplacePanePicture(nw4r::lyt::Pane *pPane, nw4r::lyt::Pane *pSrc);
        void ReplacePanePicture(nw4r::lyt::Pane *pPane, const _GXTexObj *pTexObj, long nTexMap);

        // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp. Moves
        // pPane under pParent. Return type and the names of the last two
        // arguments are guesses (called with 1, 0).
        void SetPaneParent(nw4r::lyt::Pane *pPane, nw4r::lyt::Pane *pParent, int bUnk, nw4r::lyt::Pane *pUnk);

        // Added for MenuF game/menu/CChrSign.cpp. The screen position of the
        // world point rPos seen through camera nCamera, into rOut (return
        // type and argument names guessed; DodgeF CLockCursorDodge::Disp
        // tests the result against 0 and uses rOut only when it is 0).
        int GetWorldScreenPosition(const FVEC3 &rPos, long nCamera, FVEC3 &rOut);

        // (added for DOL game/msw/CMenuPartBase.cpp; no symbols, names and
        // types guessed) Whether rPos is inside pPane (0 for a null or
        // hidden pane), and whether animation nAnim of pPaneObj and its
        // children has ended (1 for a null object).
        int IsHitPane(nw4r::lyt::Pane *pPane, const nw4r::math::VEC2 &rPos);
        int IsAnimEndRecursive(CPaneObj *pPaneObj, long nAnim);

        // Added for MenuF game/menu/CSelectMenuLayoutBase.cpp: releases a
        // font made from a layout archive (return type guessed).
        void RemoveArchiveFont(nw4r::ut::ArchiveFont *pFont);

        // (name guessed; no symbol) The object that drives pPane, made by
        // pPaneCtrl; null when either is missing. Inline in the game: the
        // sequence is in DuplicatePaneObj in the DOL and twice in every
        // CRadarIcon2D<Game>::Create*Icon.
        inline CPaneObj *CreatePaneObj(CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pPane)
        {
            if (pPane == 0 || pPaneCtrl == 0) {
                return 0;
            }
            return pPaneCtrl->CreatePaneObj(pPane);
        }
    }
}

#endif /* !BCS_G2D_G2DUTIL_HPP_ */
