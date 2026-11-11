#ifndef BCS_DWC_CDWCAPI_HPP_
#define BCS_DWC_CDWCAPI_HPP_

#include "dwc.h"

namespace bcs {
    namespace dwc {

        // Wrappers around the Nintendo Wi-Fi Connection library that hold the
        // library's mutex during the call (game/bcs/dwc). Only the members
        // used so far; the unnamed ones (fn_800B....) have guessed names.
        class CDwcAPI {
            public:
                // fn_800B2AE8: DWC_IsEqualFriendData. Unnamed.
                static int IsEqualFriendData(const DWCstAccFriendData *pData1, const DWCstAccFriendData *pData2);
                // fn_800B2BA8: DWC_IsBuddyFriendData (fn_803AF310). Unnamed.
                static int IsBuddyFriendData(const DWCstAccFriendData *pData);
                // fn_800B2A00: DWC_CreateFriendKeyToken. Unnamed. Fills pData
                // with the friend data for friend code nCode.
                static void CreateFriendKeyToken(DWCstAccFriendData *pData, unsigned long long nCode);
                // fn_800B2A70: DWC_CheckFriendKey. Unnamed.
                static int CheckFriendKey(const DWCstAccUserData *pUserData, unsigned long long nCode);
                // fn_800B2B50: wraps fn_803AFCC0 (DWC_IsValidFriendData,
                // guessed). Unnamed.
                static int IsValidFriendData(const DWCstAccFriendData *pData);
        };
    }
}

#endif /* !BCS_DWC_CDWCAPI_HPP_ */
