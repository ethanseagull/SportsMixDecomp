#ifndef SELECTMENU_CCHARALEVEL_HPP_
#define SELECTMENU_CCHARALEVEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5AC. The rule window of the character select
        // ("me_cha_level"): one CMenuOption setting, changed with two
        // arrows. Its buttons are read from "me_cha_level.btn" by the
        // CButtonEntry base at 0x5C (a second base, as in CTeam). Member
        // names are guesses.
        class CCharaLevel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CCharaLevel(long nId);
                virtual ~CCharaLevel();
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

                // The CMenuOption option the window shows, from the owner's
                // mode and game kind.
                long GetOptionKindId();
                // Moves the option's setting by nAdd (clamped) and shows
                // its text.
                void SetRuleText(long nAdd);
                // Greys out (bDisable) or restores arrow button nBtnKindId.
                void SetArrowDisable(long nBtnKindId, int bDisable);

            private:
                // Inline in the game; only its local static has a symbol.
                void CheckButtonAnim();

                int m_bLeftDisable;   // 0x5A0, the left arrow is greyed out
                int m_bRightDisable;  // 0x5A4, the right arrow is greyed out
                float m_fUnk5A8;      // 0x5A8, set to 10.0f, not read here
        };
    }
}

#endif /* !SELECTMENU_CCHARALEVEL_HPP_ */
