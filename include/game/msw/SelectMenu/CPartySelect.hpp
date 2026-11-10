#ifndef SELECTMENU_CPARTYSELECT_HPP_
#define SELECTMENU_CPARTYSELECT_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C8. The party game selection screen ("me_ptg_sel"): four
        // party game buttons, each with a preview of a movie that is shown
        // on its icon a while after the screen opens. Its buttons are read
        // from "me_ptg_sel.btn" by the CButtonEntry base at 0x5C (a second
        // base, as in CTeam). Menu object 0x19 of CSelectMenu;
        // CSelectMenu::PartySelect_* drive it.
        class CPartySelect : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Values of m_nMovieState. The names are guesses.
                enum {
                    MOVIE_NONE = 0,
                    MOVIE_STOP = 1,  // set by Set(): stop what is playing
                    MOVIE_OPEN = 2,  // waiting to open the movie
                    MOVIE_START = 3, // waiting to start it
                    MOVIE_PLAY = 4   // started and hung under the icons
                };

                enum {
                    MOVIE_PANE_NUM = 4
                };

                CPartySelect(long nId);
                virtual ~CPartySelect();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                void MovieControl();

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                void InitPaneAnim();

                unsigned long m_Unk5A0;                // 0x5A0, not used here
                long m_nMovieState;                    // 0x5A4, MOVIE_*
                // 0x5A8. (names guessed) Per icon: non-zero while the
                // movie is still to be shown on it, and the frames left
                // until then.
                long m_bMovieWait[MOVIE_PANE_NUM];
                float m_fMovieWait[MOVIE_PANE_NUM];    // 0x5B8
        };
    }
}

#endif /* !SELECTMENU_CPARTYSELECT_HPP_ */
