#ifndef SELECTMENU_CTEAM_HPP_
#define SELECTMENU_CTEAM_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A4. The team screen of an exhibition match
        // ("me_multiplay_01"): the player picks one of the team patterns of
        // the play mode, and the court characters show it. Its buttons are
        // read from "me_multiplay_01.btn" by the CButtonEntry base at 0x5C
        // (a second base, as in CScoreTop). Menu object 0xE of CSelectMenu;
        // CSelectMenu::Team_* drive it.
        class CTeam : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CTeam(long nId);
                virtual ~CTeam();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimPush(long nBtnKindId);
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

                // Shows the court characters of the team pattern and their
                // captions; only for button nBtnKindId 0x36 (the pattern
                // selector).
                void SetCourtCharaObj(long nBtnKindId);

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                // Writes "pattern N" when the owner's pattern has changed
                // and shows its characters.
                void SelectTeamPatternText();

                // 0x5A0. (name guessed) The pattern number shown (1-based),
                // -1 before the first.
                long m_nPatternShown;
        };
    }
}

#endif /* !SELECTMENU_CTEAM_HPP_ */
