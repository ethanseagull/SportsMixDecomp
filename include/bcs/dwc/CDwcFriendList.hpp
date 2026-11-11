#ifndef BCS_DWC_CDWCFRIENDLIST_HPP_
#define BCS_DWC_CDWCFRIENDLIST_HPP_

#include "dwc.h"

namespace bcs {
    namespace dwc {
        class CDwcFriend;

        // The Wi-Fi friend roster (game/bcs/dwc), kept by CDwc. Only the
        // members used so far; the size is not known. Added for MenuF
        // game/menu/CFriendCode.cpp; return types and argument names are
        // guesses.
        class CDwcFriendList {
            public:
                // Not zero when the roster is full.
                int IsNumMax() const;
                long GetNum() const;
                // Entry nIndex (also the empty ones).
                CDwcFriend *GetFriend(long nIndex);
                // The friend with code nCode, or null.
                CDwcFriend *GetFriendFromFriendCode(unsigned long long nCode) const;
                // Adds the friend with code nCode; null when it cannot.
                CDwcFriend *AddFriend(const DWCstAccUserData *pUserData, unsigned long long nCode);
                // Renumbers the entries after a change.
                void NormalizeNo();
                // Not zero when nCode is a valid friend code for pUserData.
                static int CheckFriendCode(const DWCstAccUserData *pUserData, unsigned long long nCode);
        };
    }
}

#endif /* !BCS_DWC_CDWCFRIENDLIST_HPP_ */
