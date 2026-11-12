#ifndef SELECTMENU_CCONNECTIONLEVEL_HPP_
#define SELECTMENU_CCONNECTIONLEVEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5B4. The window the Wi-Fi match-making screen shows once an
        // opponent is found (menu object 0xFF of CSelectMenu): the match
        // level and both players' data, with buttons. Its buttons are read by
        // the CButtonEntry base at 0x5C, as in CWiFiFrindVS. Only the members
        // used so far (added for MenuF game/menu/unit_4BB48.cpp); argument
        // names are guesses: nLevel is bcs::dwc::CDwc::GetMatchingLevel, the
        // two pointers this player's and the opponent's
        // CSelectMenu::WiFiSingleData.
        class CConnectionLevel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CConnectionLevel(long nId, long nLevel, void *pMyData, void *pEmData);
                virtual ~CConnectionLevel();
                virtual void Main();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

            private:
                // Inlined in the game; only its local static has a symbol
                // (added for game/menu/CWiFiTopA.cpp).
                void CheckButtonAnim();

                // (names guessed; added for game/menu/CWiFiTopA.cpp)
                long m_nLevel;               // 0x5A0, nLevel less one: the stars shown

            public:
                // 0x5A4. (name guessed) The player's answer: 1000 accepted,
                // 1001 refused; anything else while there is none.
                long m_nResult;

            private:
                // (names guessed; added for game/menu/CWiFiTopA.cpp)
                float m_fInputWait;          // 0x5A8, frames before the buttons answer
                const CSelectMenu::WiFiSingleData *m_pMyData; // 0x5AC
                const CSelectMenu::WiFiSingleData *m_pEmData; // 0x5B0
        };
    }
}

#endif /* !SELECTMENU_CCONNECTIONLEVEL_HPP_ */
