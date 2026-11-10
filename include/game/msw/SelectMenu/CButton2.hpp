#ifndef SELECTMENU_CBUTTON2_HPP_
#define SELECTMENU_CBUTTON2_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. A text window ("main_pmake_06") with two buttons.
        class CButton2 : public CSelectMenuLayoutBase {
            public:
                CButton2(long nId);
                virtual ~CButton2();
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

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

            private:
                // Inline in the game; only its local statics have symbols.
                void InitPaneAnim();
        };
    }
}

#endif /* !SELECTMENU_CBUTTON2_HPP_ */
