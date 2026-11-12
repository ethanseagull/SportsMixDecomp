#ifndef SELECTMENU_CCONNECTIONWAIT_HPP_
#define SELECTMENU_CCONNECTIONWAIT_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64. The "connecting" indicator the Wi-Fi screens put up
        // while they wait ("WIN_H", menu object 0xFF of CSelectMenu). Added
        // for MenuF game/menu/CFriendInfo.cpp; defined in
        // game/menu/CWiFiTopA.cpp. Member names are guesses.
        class CConnectionWait : public CSelectMenuLayoutBase {
            public:
                // bDisconnect shows the "disconnect" picture instead of the
                // "connect" one; fOffsetY moves three of the panes up or
                // down.
                CConnectionWait(long nId, int bDisconnect, float fOffsetY);
                virtual ~CConnectionWait();
                virtual void Main();
                // Event 0x2E closes the window (0), others give -1.
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp() {}
                virtual int Set();
                virtual void Remove();

            private:
                int m_bDisconnect;              // 0x5C
                float m_fOffsetY;               // 0x60
        };
    }
}

#endif /* !SELECTMENU_CCONNECTIONWAIT_HPP_ */
