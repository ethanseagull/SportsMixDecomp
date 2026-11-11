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
                unsigned char m_Unk5A0[0x4]; // 0x5A0

            public:
                // 0x5A4. (name guessed) The player's answer: 1000 accepted,
                // 1001 refused; anything else while there is none.
                long m_nResult;

            private:
                unsigned char m_Unk5A8[0x5B4 - 0x5A8];
        };
    }
}

#endif /* !SELECTMENU_CCONNECTIONLEVEL_HPP_ */
