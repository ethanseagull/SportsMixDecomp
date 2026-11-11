#ifndef SELECTMENU_CTOURNAMENTLOAD_HPP_
#define SELECTMENU_CTOURNAMENTLOAD_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A0. The window that offers to continue the tournament
        // saved for later ("me_to_load"): its cup, how far it had got and
        // its number of players, with three buttons (continue, start a new
        // one, back). Its buttons are read from "me_to_load.btn" by the
        // CButtonEntry base at 0x5C, as in CTeam. Menu object 0xFF of
        // CSelectMenu, made by GameMode_Loop.
        class CTournamentLoad : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Inline: GameMode_Loop is the only place that makes one,
                // and there is no out-of-line copy.
                CTournamentLoad(long nId)
                    : CSelectMenuLayoutBase(nId)
                {
                    m_Unk2C = 0;
                    m_Unk30 = STATE_NONE;
                }
                virtual ~CTournamentLoad();
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

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                // Writes pText into text pane nPane when the pane exists.
                void SetTextPaneString(long nPane, const wchar_t *pText);
        };
    }
}

#endif /* !SELECTMENU_CTOURNAMENTLOAD_HPP_ */
