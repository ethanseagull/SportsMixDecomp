#ifndef BCS_CWIFIPAD_HPP_
#define BCS_CWIFIPAD_HPP_

namespace bcs {

    // INCOMPLETE: the Wii Remote of pad nPad (DOL). All members are static;
    // only the ones HockeyF's game/hockey/CChrDrvPadHockey.cpp calls are
    // declared. Return types and parameter names are guesses.
    class CWifiPad {
        public:
            // Not zero after a vertical (Tate) or a horizontal (Yoko) swing
            // of the Remote.
            static int IsTateStroke(long nPad);
            static int IsYokoStroke(long nPad);
            // How hard the Remote is shaken.
            static float GetScratchPower(long nPad);
            // Not zero after a swing of both hands, of the Nunchuk, or of the
            // Remote, harder than the given power.
            static int IsBothHandStroke(float fRemocon, float fNunchuk, long nPad);
            static int IsNunchukStroke(float fPower, long nPad);
            static int IsRemoconStroke(float fPower, long nPad);
    };
}

#endif /* !BCS_CWIFIPAD_HPP_ */
