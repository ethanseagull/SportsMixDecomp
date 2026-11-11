#ifndef DWC_H_
#define DWC_H_

// Types of the Nintendo Wi-Fi Connection library, only as far as the game's
// own code needs them. Sizes come from the game's save data layout.

// The account data of the console's user (0x40 bytes in
// msw::SAVE_DATA_PLAYER). Contents not modelled.
struct DWCstAccUserData {
    unsigned char m_Data[0x40];
};

#endif /* !DWC_H_ */
