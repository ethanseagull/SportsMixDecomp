#ifndef SELECTMENU_CPARTYINFO_HPP_
#define SELECTMENU_CPARTYINFO_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5BC. The party game information screen
        // ("me_ptg_operation_w"): a row of two to four tabs, each with a
        // help page (a CPartyHelpPage, menu objects 0x47 to 0x4A, one per
        // party game) and the controls of the game, drawn for the current
        // pad style. Its buttons are read from "me_ptg_operation_w.btn" by
        // the CButtonEntry base at 0x5C. Menu object 0x1A of CSelectMenu;
        // CSelectMenu::PartyInfo_* drive it. Member names are guesses.
        class CPartyInfo : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    TAB_MAX = 4
                };

                CPartyInfo(long nId);
                virtual ~CPartyInfo();
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

                // Selects tab nTab (ignored when it is already selected or
                // out of range) and tells the help pages.
                void SetTabSelect(long nTab);
                // Shows the controls of the current pad style, when it has
                // changed.
                void DispControlType();
                // Takes the pointer's highlight off every tab.
                void SelectAnimAllOff();

            private:
                // Inlined in the game; only its local static has a symbol.
                void CheckButtonAnim();

                friend class msw::CSelectMenu;

                long m_nHelpPageObj;             // 0x5A0, menu object of the help page (0x47 to 0x4A)
                long m_nTabSelect;               // 0x5A4, -1 before the first
                long m_bTabHit[TAB_MAX];         // 0x5A8, the pointer is on tab i
                long m_nPadStyle;                // 0x5B8, the pad style shown, -1 before the first
        };
    }
}

#endif /* !SELECTMENU_CPARTYINFO_HPP_ */
