#ifndef SELECTMENU_CBUTTONA_HPP_
#define SELECTMENU_CBUTTONA_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. A text window ("main_pmake_03") with one A button
        // ("A_botan"). CButtonACode derives from it.
        class CButtonA : public CSelectMenuLayoutBase {
            public:
                CButtonA(long nId);
                virtual ~CButtonA();
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
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // New in this class. Sets the text of message line nLine
                // (only line 0 here): 1 when set, 0 for an unknown line, -1
                // without text.
                virtual long SetMessageText(long nLine, const wchar_t *pText);

                // The return type is a guess (it returns 0; nothing reads it).
                long CheckButtonAnim();

            private:
                // Inline in the game; only its local statics have symbols.
                void InitPaneAnim();
        };
    }
}

#endif /* !SELECTMENU_CBUTTONA_HPP_ */
