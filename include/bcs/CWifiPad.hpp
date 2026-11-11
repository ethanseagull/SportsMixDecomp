#ifndef BCS_CWIFIPAD_HPP_
#define BCS_CWIFIPAD_HPP_

namespace bcs {

    // The pads (Wii Remote and Nunchuk) as the game reads them, by pad
    // number nPad. A DOL class; all members are static. Only the ones
    // BasketF's game/basket/CChrDrvPadBasket.cpp and HockeyF's
    // game/hockey/CChrDrvPadHockey.cpp call are declared. nButton is a mask
    // of button bits (0x100, 0x200, 0x1000, 0x10 are used by the basketball
    // driver). Return types and parameter names are guesses.
    class CWifiPad {
        public:
            static int IsDown(unsigned long nButton, long nPad);
            static int IsTrg(unsigned long nButton, long nPad);
            static int IsRelease(unsigned long nButton, long nPad);
            // Not zero after a vertical (Tate) or a horizontal (Yoko) swing
            // of the Remote.
            static int IsTateStroke(long nPad);
            static int IsYokoStroke(long nPad);
            // How hard the Remote is shaken (the basketball driver scales it
            // by 1024 and ignores values up to 0.2).
            static float GetScratchPower(long nPad);
            // Not zero after a swing of both hands, of the Nunchuk, or of the
            // Remote, harder than the given power.
            static int IsBothHandStroke(float fRemocon, float fNunchuk, long nPad);
            static int IsNunchukStroke(float fPower, long nPad);
            static int IsRemoconStroke(float fPower, long nPad);
    };
}

#endif /* !BCS_CWIFIPAD_HPP_ */
