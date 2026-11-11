#ifndef SELECTMENU_CFRIENDINFO_HPP_
#define SELECTMENU_CFRIENDINFO_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x9C0. The information window of a Wi-Fi friend, with tabs.
        // Its buttons are read by the CButtonEntry base at 0x5C (a second
        // base, as in CTeam). Menu object 0x29 of CSelectMenu;
        // CSelectMenu::FriendPlayerInfo_* and FriendInfo_* drive it.
        // Only the members used so far (added for MenuF
        // game/menu/CFriendCode.cpp); the overrides of the base's virtuals
        // are not declared yet, so do not create one through this header.
        // Names and return types are guesses.
        class CFriendInfo : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Fills the window; what bUnk selects is not known (0 from
                // FriendPlayerInfo_In, 1 from FriendInfo_In).
                void SetUpDisp(int bUnk);
                // Selects tab nTab.
                void SetTabSelect(long nTab, int bUnk);

            private:
                unsigned char m_Unk5A0[0x8];   // 0x5A0

            public:
                // 0x5A8. The selected tab.
                long m_nTabSelect;

            private:
                unsigned char m_Unk5AC[0x9C0 - 0x5AC]; // 0x5AC
        };
    }
}

#endif /* !SELECTMENU_CFRIENDINFO_HPP_ */
