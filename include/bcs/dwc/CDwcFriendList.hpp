#ifndef BCS_DWC_CDWCFRIENDLIST_HPP_
#define BCS_DWC_CDWCFRIENDLIST_HPP_

#include "dwc.h"

namespace bcs {
    namespace dwc {
        class CDwcFriend;

        // The Wi-Fi friend roster (game/bcs/dwc), kept by CDwc. 0x8C bytes
        // (the constructor clears up to 0x88). Added for MenuF
        // game/menu/CFriendCode.cpp; return types and argument names are
        // guesses.
        class CDwcFriendList {
            public:
                enum { FRIEND_MAX = 32 };

                // fn_800B3C8C, fn_800B3D28 (unnamed; vtable 0x8046A858).
                CDwcFriendList();
                virtual ~CDwcFriendList();
                // fn_800B3DA8 (unnamed; name guessed), vtable slot 3:
                // allocates the FRIEND_MAX entries; 0 when already done.
                // The game's msw list overrides it.
                virtual int Create();

                // Not zero when the roster is full.
                int IsNumMax() const;
                long GetNum() const;
                // Entry nIndex (also the empty ones).
                CDwcFriend *GetFriend(long nIndex);
                // fn_800B3E48 (unnamed): the same, const.
                const CDwcFriend *GetFriend(long nIndex) const;
                // The friend with code nCode, or null.
                CDwcFriend *GetFriendFromFriendCode(unsigned long long nCode) const;
                // Adds the friend with code nCode; null when it cannot.
                CDwcFriend *AddFriend(const DWCstAccUserData *pUserData, unsigned long long nCode);
                // Fills entry nIndex from the save data (pStorage: nSize
                // bytes, or null); null when it ends up empty.
                CDwcFriend *WriteBack(const DWCstAccFriendData *pData, unsigned long long nCode, long nIndex, long nNo, const void *pStorage, long nSize);
                // The friend asking for a match, the one with the lowest
                // status word 0x10 if several; null if none.
                CDwcFriend *SearchFriendMatchChallengerAll() const;
                // CDwcFriend::LoadStorage for every entry.
                void LoadStorageAll();
                // Renumbers the entries after a change.
                void NormalizeNo();
                // (added for MenuF game/menu/CFriendList.cpp; return types
                // and argument names guessed) Collects the valid entries for
                // GetSortFriend and sorts them: nUnk 0 by buddy, online
                // state and number; 1 not at all; 2 by number.
                void Sort(long nUnk);
                CDwcFriend *GetSortFriend(long nIndex);
                // Added for MenuF game/menu/CFriendInfo.cpp (return type
                // guessed). Takes entry nIndex off the roster.
                void RemoveFriend(long nIndex);
                // Not zero when nCode is a valid friend code for pUserData.
                static int CheckFriendCode(const DWCstAccUserData *pUserData, unsigned long long nCode);

            protected:
                // (inline; names guessed) The first free slot, and the next
                // free number; -1 when the roster is full.
                long GetEmptyIndex() const;
                long GetNextNo() const;

                CDwcFriend *m_pFriends;                 // 0x04, FRIEND_MAX entries
                CDwcFriend *m_pSortFriends[FRIEND_MAX]; // 0x08, the valid ones as sorted
                long m_nNum;                            // 0x88
        };
    }
}

#endif /* !BCS_DWC_CDWCFRIENDLIST_HPP_ */
