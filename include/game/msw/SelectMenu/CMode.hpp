#ifndef SELECTMENU_CMODE_HPP_
#define SELECTMENU_CMODE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64. The game mode screen ("me_multiplay_00"): three
        // buttons plus the back button, in one of two sets picked by
        // m_nType (the tutorials, or the tournament modes). Menu object 8
        // of CSelectMenu; CSelectMenu::GameMode_* drive it. The layout is
        // the one of CHowTo with other names.
        class CMode : public CSelectMenuLayoutBase {
            public:
                // Values of m_nType (names guessed).
                enum {
                    TYPE_TUTORIAL = 2,
                    TYPE_MODE = 3
                };

                CMode(long nId);
                virtual ~CMode();
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

            public:
                // 0x5C. (name guessed) Which button set Set() builds:
                // TYPE_*. Written by CSelectMenu before it builds the
                // screen.
                long m_nType;
                // 0x60. (name guessed) The step of CSelectMenu::GameMode_Loop
                // to go back to once the screen has been built again.
                long m_nReturnStep;
        };
    }
}

#endif /* !SELECTMENU_CMODE_HPP_ */
