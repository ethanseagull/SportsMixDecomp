#ifndef CSAVE_HPP_
#define CSAVE_HPP_

namespace msw {

    // Only the members used so far. All are static: their bodies in the DOL
    // work on a global instance pointer and take no object. Return types the
    // callers ignore are guessed as void.
    class CSave {
        public:
            static void Initialize();
            static int CanbeWrite(); // return type guessed
            static void SaveDataInit();

            // Added for MenuF game/menu/CHowTo.cpp. (name guessed) The save
            // data of one player; only the member used so far.
            struct PlayerData {
                unsigned char m_Unk0000[0x5C50];
                unsigned char m_nFlags5C50; // 0x5C50; bit 0x08 picks the second camera table in CSelectMenu::HowTo_In
            };

            // The player data of player nPlayer; -1 for the current one
            // (description guessed).
            static PlayerData *GetPlayerData(long nPlayer);
    };
}

#endif /* !CSAVE_HPP_ */
