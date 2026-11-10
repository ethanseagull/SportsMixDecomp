#ifndef BCS_CMAINAPP_HPP_
#define BCS_CMAINAPP_HPP_

#include "revolution/gx/GXStruct.h"

namespace bcs {

    // Only the members used so far. All are static: their bodies in the DOL
    // read globals and take no object. Return types the callers ignore are
    // guessed as void.
    class CMainApp {
        public:
            static float GetAppFrameRateF();
            static float GetUnivSpeed();
            static void StartMasterFade(long, float);
            static int IsPlayMasterFade(); // return type guessed

            // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp: the
            // render mode in use (the cockpit reads the frame buffer size).
            // The DOL's object is 0x70 bytes; what follows the render mode
            // object is not known.
            static GXRenderModeObj g_Rmode;
    };
}

#endif /* !BCS_CMAINAPP_HPP_ */
