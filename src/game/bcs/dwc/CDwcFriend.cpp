#include "bcs/dwc/CDwcFriend.hpp"

#include "bcs/dwc/CDwc.hpp"
#include "bcs/dwc/CDwcAPI.hpp"
#include "string.h"
#include "wchar.h"

namespace bcs {
    namespace dwc {

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
                    wcsncpy(m_Name, pName, 11);
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
