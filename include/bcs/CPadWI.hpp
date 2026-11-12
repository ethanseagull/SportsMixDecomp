#ifndef BCS_CPADWI_HPP_
#define BCS_CPADWI_HPP_

#include "bcs/FVEC2.hpp"

namespace bcs {

    // The Wii remote input of the engine. Lives in the DOL. Only what the
    // modules use so far; member names and return types are guesses.
    class CPadWI {
        public:
            // (added for MenuF game/menu/CBackIcon.cpp) Writes the screen
            // position remote nPad points at to *pPos; non-zero when it
            // points at the screen. Static: callers pass the vector first.
            static int GetScreenPosDPD_F(FVEC2 *pPos, long nPad);
            // (DOL fn_80070B74, name guessed; added for
            // game/msw/CMenuPad.cpp) The same through KPADGetProjectionPos.
            static int GetScreenPos_F(FVEC2 *pPos, long nPad);
            // (DOL fn_800709D8, name guessed from msw::CMenuPad::IsConnect,
            // which wraps it) Non-zero while remote nPad is connected.
            static int IsConnect(long nPad);
            // (added for MenuF game/menu/CSaveSelect.cpp; return types
            // guessed) The buttons remote nPad holds down, and whether
            // buttons nButton are held down / were just pressed on it.
            static unsigned long GetDown(long nPad);
            static int IsDown(unsigned long nButton, long nPad);
            static int IsTrg(unsigned long nButton, long nPad);
            // (added for game/bcs/CWifiPad.cpp; names and return types
            // guessed from the CWifiPad functions that wrap them) Buttons
            // just released, the buttons just pressed / released on remote
            // nPad, the stick, whether the Nunchuk style is in use, and
            // stopping the rumble.
            static int IsRelease(unsigned long nButton, long nPad);
            static unsigned long GetTrg(long nPad);
            static unsigned long GetRelease(long nPad);
            static float GetStickX(long nPad);
            static float GetStickY(long nPad);
            static int IsNunchukStyle(long nPad);
            static void StopMotor(long nPad);
            // (added for DOL game/msw/CGameVib.cpp; names and return types
            // guessed) fn_80070EEC plays nNum (strength, time) float pairs
            // on remote nPad for nFrame frames; fn_80070F0C rumbles for
            // fFrame.
            static int StartMotor(const float *pData, long nNum, long nFrame, long nPad);
            static int StartMotor(float fFrame, long nPad);
            // (added for TourF game/tour/CTournamentStep.cpp and
            // game/msw/CGame.cpp) Save and
            // restore the rumble of every pad around a pause.
            static void PushMotorAll();
            static void PopMotorAll();
    };
}

#endif /* !BCS_CPADWI_HPP_ */
