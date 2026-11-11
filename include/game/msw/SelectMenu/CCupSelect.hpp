#ifndef SELECTMENU_CCUPSELECT_HPP_
#define SELECTMENU_CCUPSELECT_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A8 (layout.py). The cup select screen: three cup
        // buttons (kind ids 0x8010 to 0x8012) and two background buttons
        // (0x8020, 0x8021); its buttons come from the CButtonEntry base at
        // 0x5C. Menu object 0x33 of CSelectMenu; CCupSelectTab (object
        // 0x34) is the difficulty tab row shown with it. Defined in
        // game/menu/CCupSelectTab.cpp. Only what game/menu/unit_61C00.cpp
        // uses; member names are guesses.
        class CCupSelect : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CCupSelect(long nId);
                virtual ~CCupSelect();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimIn(long nBtnKindId);
                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long nPad, long nBtnKindId);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1). The return type is a
                // guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Plays the opening animation of the cup buttons.
                void SetAppearanceAnim();
                // Enables or disables cup button nCup; bDisp shows the
                // button's mark when it is enabled (as CCourt's).
                void SetStageEnable(long nCup, int bEnable, unsigned char bDisp);

            private:
                // (added for game/menu/CCupSelectTab.cpp; no symbols, names
                // guessed) Inline helpers: the button and event step of
                // Main(), and two text box setters that the game never calls
                // here but whose local statics are in .rodata.
                void CheckButtonAnim();
                void SetTextPaneString(long nPane, const wchar_t *pText);
                void SetTextPaneString(long nPane, long nValue);

                // 0x5A0. Not zero while the +/- buttons (menu object 2)
                // are shown and take hits.
                long m_bPlusMinus;
                // 0x5A4. Frames left before the opening animation is
                // checked.
                float m_fWait;
        };
    }
}

#endif /* !SELECTMENU_CCUPSELECT_HPP_ */
