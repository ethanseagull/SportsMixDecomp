#ifndef SELECTMENU_CHOWTO_HPP_
#define SELECTMENU_CHOWTO_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64. The "how to play" screen ("me_multiplay_02"): three
        // buttons, one per tutorial, plus the back button. Menu object 10
        // of CSelectMenu; CSelectMenu::HowTo_* drive it.
        class CHowTo : public CSelectMenuLayoutBase {
            public:
                CHowTo(long nId);
                virtual ~CHowTo();
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

                void StateControl();

            private:
                // Inlined into StateControl and Set in the game, with no
                // out-of-line copy left in the module; only their local
                // statics have symbols.
                void CheckButtonAnim();
                void PaneColorInit();

                // 0x5C. Not touched by any function of the class; the size
                // comes from the allocation.
                unsigned char m_Unk5C[8];
        };
    }
}

#endif /* !SELECTMENU_CHOWTO_HPP_ */
