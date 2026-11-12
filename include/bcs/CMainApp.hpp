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
            static void Start3DFade(long, float);
            static int IsPlayMasterFade(); // return type guessed
            // (added for MenuF game/menu/CTitle.cpp) Called after the GX
            // state was changed by hand (msw::THPGXRestore).
            static void ResetGP();

            // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp: the
            // render mode in use (the cockpit reads the frame buffer size).
            // The DOL's object is 0x70 bytes; what follows the render mode
            // object is not known.
            static GXRenderModeObj g_Rmode;

            // Added for MenuF game/menu/CBackground.cpp: the address of
            // g_Rmode, and the region's language (a byte; what the values
            // stand for is not known, 0 gives the slower text scroll).
            static GXRenderModeObj *GetRenderModeObj();
            static unsigned char GetRegionLanguage();
            // (fn_80052194, name guessed; added for msw::CChrMarker) The
            // language of the game's text, as the byte fn_8005211C keeps
            // (0 Japanese, 1 English, 2 German, 3 French, 4 Spanish,
            // 5 Italian, read from the marker models CChrMarker picks).
            static unsigned char GetLanguage();
            // (fn_800521B8, name guessed; added for the strap screen in
            // game/bcs/CTask.cpp) The name of the console's language as
            // used in file paths: a four-byte entry of a table indexed by
            // SCGetLanguage.
            static const char *GetSystemLanguageName();
            // (added for MenuF game/menu/CTopMenu.cpp; return type
            // guessed) Non-zero when the console's parental controls forbid
            // messaging with others.
            static int CheckPCMessageRestriction();

            // (added for DOL game/msw/CGame.cpp; return types guessed) The
            // speed of the universal clock; whether the pause of the layers
            // in nMask is ready or on; pauses or resumes them; whether the
            // 3D fade runs.
            static void SetUnivSpeed(float fSpeed);
            static int IsPauseReady(unsigned long nMask);
            static int IsPause(unsigned long nMask);
            static void SetPause(unsigned long nMask, int bPause);
            static int IsPlay3DFade();
            // (added for TourF game/tour/CTournamentStep.cpp; return types
            // guessed) The cross fades.
            static int IsPlayCrossFade();
            static int IsPlay3DCrossFade();
            static void StartCrossFade(float fSpeed, long nType, int b);
            static void StopCrossFade();
            static void Start3DCrossFade(float fSpeed, long nType, int b);
            static void Stop3DCrossFade();
    };
}

#endif /* !BCS_CMAINAPP_HPP_ */
