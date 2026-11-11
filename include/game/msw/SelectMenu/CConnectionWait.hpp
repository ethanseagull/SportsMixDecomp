#ifndef SELECTMENU_CCONNECTIONWAIT_HPP_
#define SELECTMENU_CCONNECTIONWAIT_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64. The "connecting" indicator the Wi-Fi screens put up
        // while they wait (menu object 0xFF of CSelectMenu). Only the members
        // used so far (added for MenuF game/menu/CFriendInfo.cpp); its own
        // members are not mapped, and the meaning of the constructor's last
        // two arguments is not known.
        class CConnectionWait : public CSelectMenuLayoutBase {
            public:
                CConnectionWait(long nId, int nUnk, float fUnk);
                virtual ~CConnectionWait();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            private:
                unsigned char m_Unk5C[0x64 - 0x5C];
        };
    }
}

#endif /* !SELECTMENU_CCONNECTIONWAIT_HPP_ */
