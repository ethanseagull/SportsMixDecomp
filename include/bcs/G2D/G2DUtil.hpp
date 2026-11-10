#ifndef BCS_G2D_G2DUTIL_HPP_
#define BCS_G2D_G2DUTIL_HPP_

// Free functions of the bcs::G2D namespace that build panes at run time.
// They live in the DOL. Which header declared them is not known: the file
// name is a guess. Only the ones used so far.
//
// Needs bcs::G2D::CPaneCtrl complete: include bcs/G2D/CPaneCtrl.hpp (or its
// pending copy) first.

namespace nw4r {
    namespace lyt {
        class Pane;
    }
    namespace ut {
        class ArchiveFont;
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
        // A copy, called pName, of pPaneCtrl's pane pSrcName, as an object of
        // pPaneCtrl; null on failure. The radar icons pass their layout's
        // file name as the fourth argument and 0 as the fifth. (what the
        // last two are for is not known; their names are guesses)
        CPaneObj *DuplicatePaneObj(CPaneCtrl *pPaneCtrl, const char *pSrcName, const char *pName,
                                   const char *pLayoutName, int nArg);
        // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp. Moves
        // pPane under pParent. Return type and the names of the last two
        // arguments are guesses (called with 1, 0).
        void SetPaneParent(nw4r::lyt::Pane *pPane, nw4r::lyt::Pane *pParent, int bUnk, nw4r::lyt::Pane *pUnk);

        // Added for MenuF game/menu/CChrSign.cpp. The screen position of the
        // world point rPos seen through camera nCamera, into rOut (return
        // type and argument names guessed).
        void GetWorldScreenPosition(const FVEC3 &rPos, long nCamera, FVEC3 &rOut);

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
