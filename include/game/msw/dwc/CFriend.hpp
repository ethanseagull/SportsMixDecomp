#ifndef MSW_DWC_CFRIEND_HPP_
#define MSW_DWC_CFRIEND_HPP_

#include "bcs/dwc/CDwcFriend.hpp"
#include "game/msw/WiFiSkill.hpp"
#include "revolution/rfl.h"

namespace msw {
    namespace dwc {

        // The game's friend roster entry. CSelectMenu keeps one at 0x700 and
        // calls both this class's and bcs::dwc::CDwcFriend's members on the
        // same pointer, so CDwcFriend is taken to be its base at 0.
        class CFriend : public bcs::dwc::CDwcFriend {
            public:
                // What GetOwnStorageData points at (the word at 0xE0). Only
                // the members used so far; names are guesses.
                struct StorageData {
                    unsigned char m_Unk00[0x1C];  // 0x00
                    RFLStoreData m_StoreData;     // 0x1C, the friend's Mii
                    unsigned char m_Unk68[0x18];  // 0x68
                    // 0x80. (added for MenuF game/menu/CFriendInfo.cpp) The
                    // friend's ratings in the four sports; the overall one
                    // follows at 0x200.
                    WiFiSkill m_Skill[4];
                    union {
                        short m_nWinNum;          // 0x200, shown as a number (meaning guessed)
                        WiFiSkill m_TotalSkill;   // 0x200, its first member is m_nWinNum
                    };
                };

                StorageData *GetOwnStorageData() const;
        };
    }
}

#endif /* !MSW_DWC_CFRIEND_HPP_ */
