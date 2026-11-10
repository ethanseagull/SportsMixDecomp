#ifndef SELECTMENU_CPLUSMINUS_HPP_
#define SELECTMENU_CPLUSMINUS_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A0. A row of plus/minus buttons ("me_minus_plus"): six
        // buttons (kind ids 0x80A0 to 0x80A5) in three pairs, each with a
        // text box. Hidden until StartUp or event 0x49 shows them. Its
        // buttons are read from "me_minus_plus.btn" by the CButtonEntry
        // base at 0x5C (a second base, as in CScoreTop).
        class CPlusMinus : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CPlusMinus(long nId);
                virtual ~CPlusMinus();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Shows the buttons of layout nType (0 to 3) with their
                // texts; a null text leaves its button hidden. Which text
                // goes to which button is read from the game; the argument
                // names are guesses.
                void StartUp(long nType, const wchar_t *pText1, const wchar_t *pText0);

            private:
                // Inline in the game (names guessed).
                void HideAllButtons();
                void ShowButton(long nButton);
                void ShowButton(long nButton, const wchar_t *pText);
        };
    }
}

#endif /* !SELECTMENU_CPLUSMINUS_HPP_ */
