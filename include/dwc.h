#ifndef DWC_H_
#define DWC_H_

// Types of the Nintendo Wi-Fi Connection library, only as far as the game's
// own code needs them. Sizes come from the game's save data layout.

// The account data of the console's user (0x40 bytes in
// msw::SAVE_DATA_PLAYER). Contents not modelled.
struct DWCstAccUserData {
    unsigned char m_Data[0x40];
};

// A friend's entry in the console's friend roster (12 bytes in
// bcs::dwc::CDwcFriend and msw::SAVE_DATA_PLAYER; WriteBack's mangled name
// gives the type). Contents not modelled.
struct DWCstAccFriendData {
    unsigned long m_Data[3];
};

#endif /* !DWC_H_ */
