#ifndef CSAVE_HPP_
#define CSAVE_HPP_

#include "game/msw/SAVE_DATA_PLAYER.hpp"

namespace msw {

    // Only the members used so far. All are static: their bodies in the DOL
    // work on a global instance pointer and take no object. Return types the
    // callers ignore are guessed as void.
    class CSave {
        public:
            static void Initialize();
            static int CanbeWrite(); // return type guessed
            static void SaveDataInit();
            // Added for MenuF game/menu/CFriendCode.cpp (return types
            // guessed): start writing the save data, and whether that has
            // finished.
            static void WriteAsync();
            static int IsComplete();

            // The save data of one player (msw::SAVE_DATA_PLAYER); MenuF's
            // sources call it PlayerData.
            typedef SAVE_DATA_PLAYER PlayerData;

            // The save data of player nPlayer (-1: the current one); null
            // when there is none. (parameter meaning guessed)
            static SAVE_DATA_PLAYER *GetPlayerData(long nPlayer);
            // (added for MenuF game/menu/unit_4BB48.cpp, which drops the
            // result; return type guessed) The set-up data of player
            // nPlayer (-1: the current one).
            static void *GetPlayerSetUpData(long nPlayer);
    };
}

#endif /* !CSAVE_HPP_ */
