#ifndef SELECTMENU_CBUTTON1_HPP_
#define SELECTMENU_CBUTTON1_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. A text window ("main_pmake_01") with one button.
        // CButton1Code derives from it and keeps its two cursor virtuals.
        class CButton1 : public CSelectMenuLayoutBase {
            public:
                CButton1(long nId);
                virtual ~CButton1();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual unsigned short StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class; both return 0 here. The return type is
                // a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                void CheckButtonAnim();

            private:
                // Inline in the game; only its local statics have symbols.
                void InitPaneAnim();
        };
    }
}

#endif /* !SELECTMENU_CBUTTON1_HPP_ */
