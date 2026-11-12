#ifndef SELECTMENU_CCONNECTNEWMESS02_HPP_
#define SELECTMENU_CCONNECTNEWMESS02_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x6C. The second message the Wi-Fi screens show a new user
        // ("me_wifi_new_02"), with the A button drawn for the remote or for
        // the classic controller. Menu object 0xFF of CSelectMenu, made by
        // CSelectMenu::WifiConnect_Loop. Member names are guesses.
        class CConnectNewMess02 : public CSelectMenuLayoutBase {
            public:
                // Inline: WifiConnect_Loop is the only place that makes one,
                // and there is no out-of-line copy. It sets nothing of its
                // own.
                CConnectNewMess02(long nId)
                    : CSelectMenuLayoutBase(nId)
                {
                }
                virtual ~CConnectNewMess02();
                virtual void Main();
                // Event 0x2E closes the window; the result is the state
                // once it is closing (0 when closed), -1 the first time.
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp() {}
                virtual int Set();
                virtual void Remove();

            private:
                // Shows the A button of the pad style CMenuPad gives pad 0,
                // when it has changed.
                void CheckPadStyle();

                unsigned char m_Unk5C[0x8];     // 0x5C
                long m_bClose;                  // 0x64, set by SendEvent
                long m_nPadStyle;               // 0x68, the style shown
        };
    }
}

#endif /* !SELECTMENU_CCONNECTNEWMESS02_HPP_ */
