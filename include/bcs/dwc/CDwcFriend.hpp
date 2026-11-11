#ifndef BCS_DWC_CDWCFRIEND_HPP_
#define BCS_DWC_CDWCFRIEND_HPP_

#include "dwc.h"
#include "string.h"

namespace bcs {
    namespace dwc {

        // One entry of the Wi-Fi friend roster (game/bcs/dwc). 0x5E8 bytes
        // (CDwcFriendList::WriteBack steps through the roster by 0x5E8).
        // Names are guesses.
        class CDwcFriend {
            public:
                // The status data a friend publishes (CDwc::SetOwnStatusAppData),
                // 0xA0 bytes. Only the members used so far.
                struct StatusData {
                    // Copied member by member (single bytes at 0x04 to 0x07,
                    // single words at 0x08 to 0x18, pairs from 0x1C, one
                    // block from 0x3C), which gives the member sizes.
                    long m_nValid;                  // 0x00, not zero once received
                    unsigned char m_Unk04;          // 0x04
                    unsigned char m_Unk05;          // 0x05
                    unsigned char m_Unk06;          // 0x06
                    unsigned char m_bMatchChallenge; // 0x07, asks for a friend match
                    long m_Unk08;                   // 0x08
                    long m_Unk0C;                   // 0x0C
                    unsigned long m_Unk10;          // 0x10, smallest wins in SearchFriendMatchChallengerAll
                    long m_Unk14;                   // 0x14
                    long m_Unk18;                   // 0x18
                    unsigned char m_Unk1C[8];       // 0x1C
                    unsigned char m_Unk24[8];       // 0x24
                    unsigned char m_Unk2C[8];       // 0x2C
                    unsigned char m_Unk34[8];       // 0x34
                    unsigned char m_Unk3C[0x64];    // 0x3C
                };

                // m_nFlags bits.
                enum {
                    FLAG_VALID = 1 << 0,    // the entry holds a friend
                    FLAG_BUDDY = 1 << 1,    // registered on both sides
                    FLAG_CHANGED = 1 << 2,  // the friend data changed
                    FLAG_STORAGE = 1 << 3,  // m_Data was loaded
                };

                // m_Data starts with this word when it holds the game's data.
                enum { STORAGE_MAGIC = 0x75C9E14A };

                // fn_800B4AC4 and fn_800B4B6C (unnamed). CDwcFriendList
                // allocates 32 of them with new[] (vtable 0x8046A848).
                CDwcFriend();
                virtual ~CDwcFriend();

                // (inline; name guessed) Empties the entry.
                void Clear()
                {
                    memset(GetFriendData(), 0, sizeof(DWCstAccFriendData));
                    m_nFriendCode = 0;
                    m_nIndex = -1;
                    m_Unk1C = -1;
                    m_nFlags = 0;
                    memset(m_Name, 0, sizeof(m_Name));
                    m_nOnlineStat = 0;
                    memset(&m_Status, 0, sizeof(m_Status));
                    memset(m_Data, 0, sizeof(m_Data));
                    m_nDataSize = 0;
                    m_Unk5E4 = 0;
                }

                // (inline; name guessed) Makes the entry friend nCode, in slot
                // nIndex with number nNo. 0 when the code gives no valid
                // friend data.
                int Entry(unsigned long long nCode, long nIndex, long nNo);
                // (inline; name guessed) m_Status.m_Unk10 of a friend asking
                // for a match, else 0.
                unsigned long GetMatchChallengeKey() const;

                // Whether this friend has asked for a match (reads the byte
                // at 0x47).
                int IsFriendMatchChallenger() const;

                // Added for MenuF game/menu/CFriendCode.cpp (return types
                // guessed): whether the entry holds a friend, and loading
                // its data from storage.
                int IsValid() const;
                int LoadStorage() const;
                // Added for MenuF game/menu/CFriendInfo.cpp (return type
                // guessed): whether the friend has registered the player
                // back.
                int IsBuddyComplete() const;

                // fn_800B4C40 (unnamed; name guessed): takes the friend data
                // from the roster, noting a change and whether the friend is
                // a buddy. 0 when the entry is empty.
                int SetFriendData(const DWCstAccFriendData *pData);
                // fn_800B4D18 (unnamed; name guessed): the online state and
                // the status data received. 0 when the entry is empty.
                int SetStatus(unsigned char nOnlineStat, const StatusData *pStatus);
                // fn_800B4DF4 (unnamed; name guessed): the data loaded from
                // the storage server (at most 0x500 bytes, starting with
                // STORAGE_MAGIC); also takes the friend's name from it.
                int SetStorageData(const void *pData, long nSize);

                // (inline; name guessed) Whether the friend has registered
                // the player back.
                bool IsBuddy() const
                {
                    if (!IsValid()) {
                        return false;
                    }
                    return (m_nFlags & FLAG_BUDDY) != 0;
                }

                // The DWC friend data at 0x04 (m_Unk04 to m_Unk0C).
                const DWCstAccFriendData *GetFriendData() const
                {
                    return reinterpret_cast<const DWCstAccFriendData *>(&m_Unk04);
                }
                DWCstAccFriendData *GetFriendData()
                {
                    return reinterpret_cast<DWCstAccFriendData *>(&m_Unk04);
                }

                // (no symbol; name guessed) The friend's online state, 0
                // while m_Unk40 is clear. Inline in MenuF.
                unsigned char GetOnlineStat() const
                {
                    if (m_Status.m_nValid == 0) {
                        return 0;
                    }
                    return m_nOnlineStat;
                }

                // 0x00: vptr
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
                unsigned long m_nFlags;      // 0x20, FLAG_ bits
                // 0x24. The friend's name, shown by the friend match screen
                // (SelectMenu::CWiFiFrindVS). Cleared as 0x18 bytes and
                // filled with at most 11 characters from m_Data.
                wchar_t m_Name[12];
                // 0x3C and 0x40 (added for MenuF game/menu/CFriendInfo.cpp;
                // names guessed): the online state CFriendInfo shows (0, 1
                // or another value), valid while m_Status.m_nValid is not
                // zero.
                unsigned char m_nOnlineStat; // 0x3C
                unsigned char m_Unk3D[0x3];  // 0x3D
                StatusData m_Status;         // 0x40
                // 0xE0. (added for MenuF game/menu/CFriendCode.cpp) The
                // friend's data from the storage server; the first 0x260
                // bytes are kept in the save data.
                unsigned char m_Data[0x500];
                long m_nDataSize;            // 0x5E0, bytes in m_Data
                // 0x5E4. The sort key CDwcFriendList::Sort computes.
                long m_Unk5E4;
        };
    }
}

#endif /* !BCS_DWC_CDWCFRIEND_HPP_ */
