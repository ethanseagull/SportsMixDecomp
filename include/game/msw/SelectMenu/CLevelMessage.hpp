#ifndef SELECTMENU_CLEVELMESSAGE_HPP_
#define SELECTMENU_CLEVELMESSAGE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x64 (layout.py). The message window of the cup select
        // screen ("me_cup_select_mes") that explains the selected
        // difficulty; it asks CCupSelectTab (menu object 0x34) for it.
        // Defined in game/menu/CCupSelectTab.cpp. Member names are guesses.
        class CLevelMessage : public CSelectMenuLayoutBase {
            public:
                CLevelMessage(long nId);
                virtual ~CLevelMessage();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            private:
                // 0x5C. The difficulty whose text is shown.
                long m_nLevel;
                unsigned char m_Unk60[0x4];         // 0x60
        };
    }
}

#endif /* !SELECTMENU_CLEVELMESSAGE_HPP_ */
