#include "bcs/dwc/CDwcFriend.hpp"

#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/dwc/CDwc.hpp"
#include "bcs/dwc/CDwcAPI.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "string.h"
#include "wchar.h"

namespace bcs {
    namespace dwc {

        static long CompareSortKey(CDwcFriend *const *pA, CDwcFriend *const *pB);

        CDwcFriendList::CDwcFriendList()
        {
            m_pFriends = 0;
            for (long i = 0; i < FRIEND_MAX; i++) {
                m_pSortFriends[i] = 0;
            }
            m_nNum = 0;
        }

        CDwcFriendList::~CDwcFriendList()
        {
            if (m_pFriends != 0) {
                delete[] m_pFriends;
            }
            m_pFriends = 0;
        }

        int CDwcFriendList::Create()
        {
            if (m_pFriends != 0) {
                return 0;
            }
            m_pFriends = new CDwcFriend[FRIEND_MAX];
            return 1;
        }

        long CDwcFriendList::GetNum() const
        {
            return m_nNum;
        }

        int CDwcFriendList::IsNumMax() const
        {
            return m_nNum >= FRIEND_MAX;
        }

        CDwcFriend *CDwcFriendList::GetFriend(long nIndex)
        {
            return &m_pFriends[nIndex];
        }

        const CDwcFriend *CDwcFriendList::GetFriend(long nIndex) const
        {
            return &m_pFriends[nIndex];
        }

        CDwcFriend *CDwcFriendList::GetFriendFromFriendCode(unsigned long long nCode) const
        {
            for (long i = 0; i < FRIEND_MAX; i++) {
                const CDwcFriend *pFriend = GetFriend(i);
                if (pFriend->IsValid() && nCode == pFriend->m_nFriendCode) {
                    return const_cast<CDwcFriend *>(pFriend);
                }
            }
            return 0;
        }

        inline long CDwcFriendList::GetEmptyIndex() const
        {
            if (IsNumMax()) {
                return -1;
            }
            for (long i = 0; i < FRIEND_MAX; i++) {
                if (!GetFriend(i)->IsValid()) {
                    return i;
                }
            }
            return -1;
        }

        inline long CDwcFriendList::GetNextNo() const
        {
            if (IsNumMax()) {
                return -1;
            }
            long nMax = -1;
            for (long i = 0; i < FRIEND_MAX; i++) {
                const CDwcFriend *pFriend = GetFriend(i);
                if (pFriend->IsValid() && nMax < pFriend->m_Unk1C) {
                    nMax = pFriend->m_Unk1C;
                }
            }
            return nMax + 1;
        }

        inline int CDwcFriend::Entry(unsigned long long nCode, long nIndex, long nNo)
        {
            DWCstAccFriendData data;
            CDwcAPI::CreateFriendKeyToken(&data, nCode);
            if (!CDwcAPI::IsValidFriendData(&data)) {
                return 0;
            }

            Clear();
            *GetFriendData() = data;
            m_nFriendCode = nCode;
            m_nIndex = nIndex;
            m_Unk1C = nNo;
            m_nFlags |= FLAG_VALID;
            return 1;
        }

        CDwcFriend *CDwcFriendList::AddFriend(const DWCstAccUserData *pUserData, unsigned long long nCode)
        {
            if (IsNumMax()) {
                return 0;
            }
            if (!CheckFriendCode(pUserData, nCode)) {
                return 0;
            }
            if (GetFriendFromFriendCode(nCode) != 0) {
                return 0;
            }

            long nIndex = GetEmptyIndex();
            if (nIndex < 0) {
                return 0;
            }
            long nNo = GetNextNo();
            if (nNo < 0) {
                return 0;
            }

            CDwcFriend *pFriend = GetFriend(nIndex);
            if (pFriend->Entry(nCode, nIndex, nNo)) {
                m_nNum++;
                return pFriend;
            }
            return 0;
        }

        void CDwcFriendList::RemoveFriend(long nIndex)
        {
            CDwcFriend *pFriend = GetFriend(nIndex);
            if (pFriend->m_nIndex >= 0 && pFriend->IsValid()) {
                pFriend->Clear();
                m_nNum--;
            }
        }

        CDwcFriend *CDwcFriendList::WriteBack(const DWCstAccFriendData *pData, unsigned long long nCode, long nIndex, long nNo, const void *pStorage, long nSize)
        {
            CDwcFriend *pFriend = GetFriend(nIndex);
            if (pFriend->IsValid()) {
                RemoveFriend(nIndex);
            }

            pFriend->Clear();
            *pFriend->GetFriendData() = *pData;
            pFriend->m_nFriendCode = nCode;
            pFriend->m_nIndex = nIndex;
            pFriend->m_Unk1C = nNo;
            if (nCode != 0) {
                pFriend->m_nFlags |= CDwcFriend::FLAG_VALID;
            }
            if (pStorage != 0) {
                pFriend->SetStorageData(pStorage, nSize);
            }

            if (pFriend->IsValid()) {
                m_nNum++;
                return pFriend;
            }
            return 0;
        }

        inline unsigned long CDwcFriend::GetMatchChallengeKey() const
        {
            return IsFriendMatchChallenger() ? m_Status.m_Unk10 : 0;
        }

        CDwcFriend *CDwcFriendList::SearchFriendMatchChallengerAll() const
        {
            CDwcFriend *pFound = 0;
            unsigned long nMin = 0xFFFFFFFF;
            for (long i = 0; i < FRIEND_MAX; i++) {
                const CDwcFriend *pFriend = GetFriend(i);
                if (pFriend->IsFriendMatchChallenger()) {
                    unsigned long nKey = pFriend->GetMatchChallengeKey();
                    if (pFound == 0 || nKey < nMin) {
                        pFound = const_cast<CDwcFriend *>(pFriend);
                        nMin = nKey;
                    }
                }
            }
            return pFound;
        }

        void CDwcFriendList::LoadStorageAll()
        {
            for (long i = 0; i < FRIEND_MAX; i++) {
                GetFriend(i)->LoadStorage();
            }
        }

        void CDwcFriendList::NormalizeNo()
        {
            Sort(2);
            for (long i = 0; i < m_nNum; i++) {
                m_pSortFriends[i]->m_Unk1C = i;
            }
        }

        void CDwcFriendList::Sort(long nUnk)
        {
            long nNum = 0;
            for (long i = 0; i < FRIEND_MAX; i++) {
                CDwcFriend *pFriend = GetFriend(i);
                if (pFriend->IsValid()) {
                    m_pSortFriends[nNum] = pFriend;
                    nNum++;
                }
            }
            if (nUnk == 1) {
                return;
            }

            for (long i = 0; i < nNum; i++) {
                CDwcFriend *pFriend = m_pSortFriends[i];
                long nKey = 0;
                if (nUnk == 2) {
                    nKey = pFriend->m_Unk1C;
                } else if (nUnk == 0) {
                    // Buddies first, then by online state, then by number.
                    nKey = -((pFriend->IsBuddyComplete() << 9) | ((pFriend->GetOnlineStat() & 7) << 6) | (63 - pFriend->m_Unk1C));
                }
                pFriend->m_Unk5E4 = nKey;
            }
            BCS_ShakerSort<CDwcFriend *>(m_pSortFriends, nNum, CompareSortKey);
        }

        CDwcFriend *CDwcFriendList::GetSortFriend(long nIndex)
        {
            return m_pSortFriends[nIndex];
        }

        // Ascending by the key Sort computed.
        static long CompareSortKey(CDwcFriend *const *pA, CDwcFriend *const *pB)
        {
            return (*pA)->m_Unk5E4 - (*pB)->m_Unk5E4;
        }

        int CDwcFriendList::CheckFriendCode(const DWCstAccUserData *pUserData, unsigned long long nCode)
        {
            return CDwcAPI::CheckFriendKey(pUserData, nCode);
        }

        CDwcFriend::CDwcFriend()
        {
            Clear();
        }

        CDwcFriend::~CDwcFriend() {}

        int CDwcFriend::IsBuddyComplete() const
        {
            if (IsBuddy() && *reinterpret_cast<const unsigned long *>(m_Data) == STORAGE_MAGIC) {
                return 1;
            }
            return 0;
        }

        int CDwcFriend::IsFriendMatchChallenger() const
        {
            if (IsBuddyComplete()) {
                return m_Status.m_bMatchChallenge;
            }
            return 0;
        }

        int CDwcFriend::SetFriendData(const DWCstAccFriendData *pData)
        {
            if (IsValid()) {
                int bWasBuddy = CDwcAPI::IsBuddyFriendData(GetFriendData());
                int bBuddy = CDwcAPI::IsBuddyFriendData(pData);
                if (!CDwcAPI::IsEqualFriendData(GetFriendData(), pData) || bWasBuddy != bBuddy) {
                    m_nFlags |= FLAG_CHANGED;
                }

                *GetFriendData() = *pData;
                if (bBuddy) {
                    m_nFlags |= FLAG_BUDDY;
                } else {
                    m_nFlags &= ~FLAG_BUDDY;
                }
                return 1;
            }
            return 0;
        }

        int CDwcFriend::SetStatus(unsigned char nOnlineStat, const StatusData *pStatus)
        {
            if (IsValid()) {
                m_nOnlineStat = nOnlineStat;
                m_Status = *pStatus;
                return 1;
            }
            return 0;
        }

        int CDwcFriend::SetStorageData(const void *pData, long nSize)
        {
            if (IsValid()) {
                if (nSize > (long)sizeof(m_Data)) {
                    return 0;
                }
                if (*static_cast<const unsigned long *>(pData) != STORAGE_MAGIC) {
                    return 0;
                }

                unsigned char *pDst = m_Data;
                memcpy(pDst, pData, nSize);
                // The name follows the magic word.
                const wchar_t *pName = reinterpret_cast<const wchar_t *>(pDst + 4);
                m_nDataSize = nSize;
                m_nFlags |= FLAG_STORAGE;
                if (IsValid()) {
                    memset(m_Name, 0, sizeof(m_Name));
                    wcsncpy(m_Name, pName, sizeof(m_Name) / sizeof(m_Name[0]) - 1);
                }
                return 1;
            }
            return 0;
        }

        int CDwcFriend::LoadStorage() const
        {
            if (IsValid()) {
                CDwc *pDwc = CDwc::GetInstance();
                if (pDwc != 0) {
                    return pDwc->LoadFriendStorage(m_nIndex);
                }
            }
            return 0;
        }

        // The original places this last (0x800B4F08, past the unit's current
        // end) and still inlines it above, as -ipa file allows.
        int CDwcFriend::IsValid() const
        {
            return m_nFlags & FLAG_VALID;
        }
    }
}
