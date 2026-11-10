#ifndef CSAVE_HPP_
#define CSAVE_HPP_

namespace msw {

    struct SAVE_DATA_PLAYER;

    // Only the members used so far. All are static: their bodies in the DOL
    // work on a global instance pointer and take no object. Return types the
    // callers ignore are guessed as void.
    class CSave {
        public:
            static void Initialize();
            static int CanbeWrite(); // return type guessed
            static void SaveDataInit();
            // The save data of player nPlayer (-1: the current one); null
            // when there is none. (parameter meaning guessed)
            static SAVE_DATA_PLAYER *GetPlayerData(long nPlayer);
    };
}

#endif /* !CSAVE_HPP_ */
