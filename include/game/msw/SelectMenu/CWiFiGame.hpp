#ifndef SELECTMENU_CWIFIGAME_HPP_
#define SELECTMENU_CWIFIGAME_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5D0. The Wi-Fi game selection screen ("me_Wi-Fi_top_B"):
        // the four game buttons, a random game button and a movie shown in
        // four picture panes. Its buttons are read from
        // "me_Wi-Fi_top_B.btn" by the CButtonEntry base at 0x5C. Menu object
        // 0x25 of CSelectMenu; CSelectMenu::WifiGame_* drive it. In a friend
        // match (CSelectMenu::m_Unk9B 4 and m_bStarMii not 0) the host's
        // cursor is sent to the guest, whose screen follows it. Member
        // names are guesses.
        class CWiFiGame : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Values of m_nMovieState, as in CGameSelect.
                enum {
                    MOVIE_NONE = 0,
                    MOVIE_STOP = 1,  // set by Set(): stop what is playing
                    MOVIE_OPEN = 2,  // waiting to open the movie
                    MOVIE_START = 3, // waiting to start it
                    MOVIE_PLAY = 4   // started and hung in the picture panes
                };

                enum {
                    MOVIE_PANE_NUM = 4
                };

                CWiFiGame(long nId);
                virtual ~CWiFiGame();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long nBtnKindId);
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
                // Inline in the game; only their local statics have symbols.
                void CheckButtonAnim();
                void InitPaneAnim();

                unsigned long m_Unk5A0;                 // 0x5A0, cleared by the constructor
                long m_nMovieState;                     // 0x5A4, MOVIE_*
                // 0x5A8. The button the host's cursor is on, given through
                // SetCursorMarkerPosition; HitCheck returns it on the
                // guest's screen.
                long m_nMarkerBtnKindId;
                // 0x5AC. Set when the screen has the plus/minus help of menu
                // object 2 (outside a friend match); events also go to
                // object 1 then.
                long m_bPlusMinus;
                // 0x5B0, 0x5C0. Per picture pane: whether it waits to be
                // shown, and the frames left before it is.
                long m_bDispWait[MOVIE_PANE_NUM];
                float m_fDispWait[MOVIE_PANE_NUM];
        };
    }
}

#endif /* !SELECTMENU_CWIFIGAME_HPP_ */
