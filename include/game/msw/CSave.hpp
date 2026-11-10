#ifndef CSAVE_HPP_
#define CSAVE_HPP_

#include "revolution/rfl.h"

namespace msw {

    // Added for MenuF game/menu/CHowTo.cpp as CSave::PlayerData. The save
    // data of one player (CSave::GetPlayerData); only the members used so
    // far.
    struct SAVE_DATA_PLAYER {
        unsigned char m_Unk0000[0x18];
        // 0x18 to 0x2B. (names guessed; added for
        // game/menu/CPlusMinus.cpp) The player's own Mii, passed to
        // CSelectMenu::Mii_SelectParam.
        unsigned char m_nMiiUnk;         // 0x18
        long m_nMiiIndex;                // 0x1C
        RFLDataSource m_MiiSource;       // 0x20
        RFLCreateID m_MiiCreateId;       // 0x24
        unsigned char m_Unk002C[0x5C50 - 0x2C];
        unsigned char m_nFlags5C50; // 0x5C50; bit 0x08 picks the second camera table in CSelectMenu::HowTo_In

        // (added for game/menu/CPlayMode.cpp) Forgets the Mii chosen on
        // the character select screen.
        void ClearSelectMii();
    };

    // Only the members used so far. All are static: their bodies in the DOL
    // work on a global instance pointer and take no object. Return types the
    // callers ignore are guessed as void.
    class CSave {
        public:
            static void Initialize();
            static int CanbeWrite(); // return type guessed
            static void SaveDataInit();

            // Renamed from PlayerData: the real name is SAVE_DATA_PLAYER
            // (SAVE_DATA_PLAYER::ClearSelectMii, game/menu/CPlayMode.cpp).
            typedef SAVE_DATA_PLAYER PlayerData;

            // The player data of player nPlayer; -1 for the current one
            // (description guessed).
            static PlayerData *GetPlayerData(long nPlayer);
    };
}

#endif /* !CSAVE_HPP_ */
