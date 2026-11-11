#ifndef MSW_WIFISKILL_HPP_
#define MSW_WIFISKILL_HPP_

namespace msw {

    // Size 0x60. A player's Wi-Fi rating in one game, with its history
    // (the type name is from the symbols of MenuF game/menu/CFriendInfo.cpp;
    // the member names are guesses). Kept five times in the save data and in
    // a friend's storage data: the four sports and an overall one.
    struct WiFiSkill {
        enum {
            HISTORY_MAX = 42
        };

        short m_nRate;                 // 0x00, the rating shown
        short m_nMaxRate;              // 0x02, the highest so far
        short m_nHistoryNum;           // 0x04, entries of m_History in use
        short m_nWin;                  // 0x06
        short m_nLose;                 // 0x08
        short m_nDraw;                 // 0x0A
        short m_History[HISTORY_MAX];  // 0x0C, the rating after each match
    };
}

#endif /* !MSW_WIFISKILL_HPP_ */
