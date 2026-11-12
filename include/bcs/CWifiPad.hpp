#ifndef BCS_CWIFIPAD_HPP_
#define BCS_CWIFIPAD_HPP_

#include "bcs/CWifiPadBase.hpp"

namespace bcs {

    // The pads (Wii Remote and Nunchuk) as the game reads them, by pad
    // number nPad. A DOL class (game/bcs/CWifiPad.cpp); all members are
    // static. During an online match every function reads the state the
    // pad's player sent (bcs::dwc::CDwc); otherwise the local pad
    // (bcs::CPadWI, bcs::CAccInputMan). nButton is a mask of button bits
    // (0x100, 0x200, 0x1000, 0x10 are used by the basketball driver).
    // Return types and parameter names are guesses; the functions without
    // an export name (GetRelease, GetCameraAngle, GetStroke*, ResetAcc)
    // are named after what they wrap.
    class CWifiPad : public CWifiPadBase {
        public:
            static int IsNunchukStyle(long nPad);
            static int IsDown(unsigned long nButton, long nPad);
            static int IsTrg(unsigned long nButton, long nPad);
            static int IsRelease(unsigned long nButton, long nPad);
            static unsigned long GetDown(long nPad);
            static unsigned long GetTrg(long nPad);
            static unsigned long GetRelease(long nPad);
            static float GetStickX(long nPad);
            static float GetStickY(long nPad);
            // The angle (radians, 0 to 2 pi) of the camera of view nView,
            // or online the one pad nPad's player sent.
            static float GetCameraAngle(long nView, long nPad);
            static void StopMotor(long nPad);
            // The results of the five stroke detectors (see
            // bcs::CAccInputMan).
            static long GetStrokeRaw0(long nPad);
            static long GetStrokeRaw1(long nPad);
            static long GetStrokeRaw2(long nPad);
            static long GetStrokeRaw3(long nPad);
            static long GetStrokeRaw4(long nPad);
            static long GetStroke0(long nPad);
            static long GetStroke1(long nPad);
            static long GetStroke2(long nPad);
            static long GetStroke3(long nPad);
            static long GetStroke4(long nPad);
            // Not zero after a vertical (Tate) or a horizontal (Yoko) swing
            // of the Remote.
            static int IsTateStroke(long nPad);
            static int IsYokoStroke(long nPad);
            // Clears the stroke detectors of pad nPad.
            static void ResetAcc(float fParam, long nPad);
            // How hard the Remote is shaken (the basketball driver scales it
            // by 1024 and ignores values up to 0.2).
            static float GetScratchPower(long nPad);
            // (DodgeF) How hard the Nunchuk is shaken.
            static float GetNunchukPower(long nPad);
            // Not zero after a swing of both hands, of the Nunchuk, or of the
            // Remote, at least as hard as the given power (0.2 when it is 0
            // or less). IsBothHandStroke compares its first power with the
            // Nunchuk and its second with the Remote.
            static int IsBothHandStroke(float fNunchuk, float fRemocon, long nPad);
            static int IsNunchukStroke(float fPower, long nPad);
            static int IsRemoconStroke(float fPower, long nPad);

        private:
            // GetMyLocalPadID, inlined: the local pad of online pad nPad, -1
            // when it belongs to another machine.
            static long ToLocalPadID(long nPad)
            {
                if (IsInMatch()) {
                    dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
                    if (pDwc != 0 && pDwc->GetMyAID() == pDwc->GetAIDFromOnlinePadID(nPad)) {
                        return pDwc->GetLocalPadIDFromOnlinePadID(nPad);
                    }
                    return -1;
                }
                return nPad;
            }
    };
}

#endif /* !BCS_CWIFIPAD_HPP_ */
