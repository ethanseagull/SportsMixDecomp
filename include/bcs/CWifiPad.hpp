#ifndef BCS_CWIFIPAD_HPP_
#define BCS_CWIFIPAD_HPP_

namespace bcs {

    // The pads (Wii remote and nunchuk) as the game reads them, by pad
    // number. A DOL class; only the static members seen so far are declared
    // (BasketF's game/basket/CChrDrvPadBasket.cpp). nButton is a mask of
    // button bits (0x100, 0x200, 0x1000, 0x10 are used by the basketball
    // driver). Return types and parameter names are guesses.
    class CWifiPad {
        public:
            static int IsDown(unsigned long nButton, long nPad);
            static int IsTrg(unsigned long nButton, long nPad);
            static int IsRelease(unsigned long nButton, long nPad);
            // Not zero on a sideways swing of the remote.
            static int IsYokoStroke(long nPad);
            // Not zero on an up-and-down swing of the remote.
            static int IsTateStroke(long nPad);
            // A swing of the nunchuk harder than fPower.
            static int IsNunchukStroke(float fPower, long nPad);
            // A swing of both hands, each harder than its power.
            static int IsBothHandStroke(float fRemocon, float fNunchuk, long nPad);
            // How hard the remote is being shaken (the driver scales it by
            // 1024 and ignores values up to 0.2).
            static float GetScratchPower(long nPad);
    };
}

#endif /* !BCS_CWIFIPAD_HPP_ */
