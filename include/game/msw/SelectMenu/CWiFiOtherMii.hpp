#ifndef SELECTMENU_CWIFIOTHERMII_HPP_
#define SELECTMENU_CWIFIOTHERMII_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A4. The two-page window of the Wi-Fi top screen that
        // explains other players' Miis ("WIN_J"), with a next and a back
        // button. Its buttons are read from "WIN_J.btn" by the CButtonEntry
        // base at 0x5C. Menu object 0xFF of CSelectMenu, made by
        // CSelectMenu::WifiConnect_Loop, which also turns its pages.
        // Member names are guesses.
        class CWiFiOtherMii : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Inline: WifiConnect_Loop is the only place that makes one,
                // and there is no out-of-line copy.
                CWiFiOtherMii(long nId)
                    : CSelectMenuLayoutBase(nId)
                {
                    m_Unk2C = 0;
                    m_Unk30 = STATE_NONE;
                }
                virtual ~CWiFiOtherMii();
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

                // Shows page nPage (0 or 1). Inlined in the game.
                void SetPage(long nPage);

            private:
                // Inlined in the game; only its local static has a symbol.
                void SetTextPaneString(long nPane, const wchar_t *pText);

                // CSelectMenu::WifiConnect_Loop turns the pages.
                friend class msw::CSelectMenu;

                long m_nPage;                           // 0x5A0, -1 before Set
        };
    }
}

#endif /* !SELECTMENU_CWIFIOTHERMII_HPP_ */
