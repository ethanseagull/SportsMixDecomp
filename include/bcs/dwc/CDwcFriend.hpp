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

                unsigned char m_Unk00[0x18]; // 0x00
                // 0x18. Passed to CDwc::EntryFriendMatch and
                // RefuseFriendMatch: the friend's index (guess).
                long m_nIndex;
                unsigned char m_Unk1C[0x8];  // 0x1C
                // 0x24. The friend's name, shown by the friend match screen
                // (SelectMenu::CWiFiFrindVS). The length is a guess (a Mii
                // name: ten characters and a terminator).
                wchar_t m_Name[11];
        };
    }
}

#endif /* !BCS_DWC_CDWCFRIEND_HPP_ */
