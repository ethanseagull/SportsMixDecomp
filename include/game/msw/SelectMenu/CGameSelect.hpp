#ifndef SELECTMENU_CGAMESELECT_HPP_
#define SELECTMENU_CGAMESELECT_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64. The game selection screen ("main_game"): four game
        // buttons, the rules and score book buttons, and a movie shown on
        // the four game icons.
        class CGameSelect : public CSelectMenuLayoutBase {
            public:
                // Values of m_nMovieState. The names are guesses.
                enum {
                    MOVIE_NONE = 0,
                    MOVIE_STOP = 1,  // set by Set(): stop what is playing
                    MOVIE_OPEN = 2,  // waiting to open the movie
                    MOVIE_START = 3, // waiting to start it
                    MOVIE_PLAY = 4   // started and hung under the icons
                };

                CGameSelect(long nId);
                virtual ~CGameSelect();
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
                // Returns the result of the back event sent to the menu.
                long CheckButtonAnim();

            private:
                // Inline in the game; only its local statics have symbols.
                void InitPaneAnim();
                // (no symbol; name guessed) Shows or hides the icon of the
                // button found by FindButtonParam.
                void SetIconVisible(unsigned short nBtnKindId, bool bVisible);

                unsigned long m_Unk5C; // 0x5C, cleared by the constructor and Set()
                long m_nMovieState;    // 0x60, MOVIE_*
        };
    }
}

#endif /* !SELECTMENU_CGAMESELECT_HPP_ */
