#ifndef BCS_DWC_CDWCFRIEND_HPP_
#define BCS_DWC_CDWCFRIEND_HPP_

namespace bcs {
    namespace dwc {

        // One entry of the Wi-Fi friend roster (game/bcs/dwc). Only the
        // members used so far; the size is not known. Names are guesses.
        class CDwcFriend {
            public:
                // Whether this friend has asked for a match (reads the byte
                // at 0x47).
                int IsFriendMatchChallenger() const;

                // Added for MenuF game/menu/CFriendCode.cpp (return types
                // guessed): whether the entry holds a friend, and loading
                // its data from storage.
                int IsValid() const;
                void LoadStorage() const;
                // Added for MenuF game/menu/CFriendInfo.cpp (return type
                // guessed): whether the friend has registered the player
                // back.
                int IsBuddyComplete() const;

                // (no symbol; name guessed) The friend's online state, 0
                // while m_Unk40 is clear. Inline in MenuF.
                unsigned char GetOnlineStat() const
                {
                    if (m_Unk40 == 0) {
                        return 0;
                    }
                    return m_nOnlineStat;
                }

                unsigned char m_Unk00[0x4];  // 0x00
                // 0x04 to 0x17. (added for MenuF game/menu/CFriendCode.cpp;
                // names guessed) Copied into the save data's friend roster.
                long m_Unk04;                // 0x04
                long m_Unk08;                // 0x08
                long m_Unk0C;                // 0x0C
                unsigned long long m_nFriendCode; // 0x10
                // 0x18. Passed to CDwc::EntryFriendMatch and
                // RefuseFriendMatch: the friend's index (guess).
                long m_nIndex;
                long m_Unk1C;                // 0x1C (added for game/menu/CFriendCode.cpp)
                unsigned char m_Unk20[0x4];  // 0x20
                // 0x24. The friend's name, shown by the friend match screen
                // (SelectMenu::CWiFiFrindVS). The length is a guess (a Mii
                // name: ten characters and a terminator).
                wchar_t m_Name[11];
                unsigned char m_Unk3A[0x2];  // 0x3A
                // 0x3C and 0x40 (added for MenuF game/menu/CFriendInfo.cpp;
                // names guessed): the online state CFriendInfo shows (0, 1
                // or another value), valid while the word is not zero.
                unsigned char m_nOnlineStat; // 0x3C
                unsigned char m_Unk3D[0x3];  // 0x3D
                long m_Unk40;                // 0x40
                unsigned char m_Unk44[0xE0 - 0x44]; // 0x44
                // 0xE0. (added for MenuF game/menu/CFriendCode.cpp) The
                // friend's data kept in the save data (0x260 bytes).
                unsigned char m_Data[0x260];
        };
    }
}

#endif /* !BCS_DWC_CDWCFRIEND_HPP_ */
