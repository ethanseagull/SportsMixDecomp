#ifndef SELECTMENU_CCHARALEVELSTAR_HPP_
#define SELECTMENU_CCHARALEVELSTAR_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5B4. The computer level window of the character select
        // ("me_cha_level_star"): a level of 0 to 3 stars, changed with two
        // arrows. Its buttons are read from "me_cha_level_star.btn" by the
        // CButtonEntry base at 0x5C (a second base, as in CTeam). Member
        // names are guesses.
        class CCharaLevelStar : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CCharaLevelStar(long nId);
                virtual ~CCharaLevelStar();
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

                // Moves the level by nAdd (clamped to 0..3) and shows its
                // stars.
                void SetLevelText(long nAdd);
                // (inline, no symbol; added for MenuF
                // game/menu/unit_6EE84.cpp)
                long GetLevel() const { return m_nLevel; }
                // Greys out (bDisable) or restores arrow button nBtnKindId.
                void SetArrowDisable(long nBtnKindId, int bDisable);

            private:
                // Inline in the game; only its local static has a symbol.
                void CheckButtonAnim();

                int m_bLeftDisable;   // 0x5A0, the left arrow is greyed out
                int m_bRightDisable;  // 0x5A4, the right arrow is greyed out
                float m_fUnk5A8;      // 0x5A8, not touched here (as in CCharaLevel)
                long m_nLevel;        // 0x5AC, 0 to 3
                long m_Unk5B0;        // 0x5B0, cleared by Set
        };
    }
}

#endif /* !SELECTMENU_CCHARALEVELSTAR_HPP_ */
