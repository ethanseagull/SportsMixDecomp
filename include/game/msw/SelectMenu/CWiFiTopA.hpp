#ifndef SELECTMENU_CWIFITOPA_HPP_
#define SELECTMENU_CWIFITOPA_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    class CPlayerMiiRender;

    namespace SelectMenu {

        // Size 0x5A4. The Wi-Fi top screen ("me_Wi-Fi_top_A"): the player's
        // Mii, name and four trophies, and the buttons of the Wi-Fi modes.
        // Its buttons are read from "me_Wi-Fi_top_A.btn" by the
        // CButtonEntry base at 0x5C. Menu object 0x23 of CSelectMenu;
        // CSelectMenu::WifiConnect_* drive it. Member names are guesses.
        class CWiFiTopA : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CWiFiTopA(long nId);
                virtual ~CWiFiTopA();
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
                // Inlined in the game; only its local static has a symbol.
                void CheckButtonAnim();

                CPlayerMiiRender *m_pMiiRender;         // 0x5A0, the player's Mii
        };
    }
}

#endif /* !SELECTMENU_CWIFITOPA_HPP_ */
