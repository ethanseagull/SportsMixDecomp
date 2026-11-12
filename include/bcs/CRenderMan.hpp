#ifndef BCS_CRENDERMAN_HPP_
#define BCS_CRENDERMAN_HPP_

#include "bcs/CWorldEnv.hpp"

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace bcs {

    class CScnObj;
    struct BCS_Color;
    struct FMTX34;

    // Only the members used so far. All are static: their bodies in the DOL
    // write globals and take no object. What the three floats of
    // SetDepthOfFieldParam mean is not known.
    class CRenderMan {
        public:
            // Adds pObj to the scene. (meaning of the second argument not
            // known: a render group, 2 or 4 in the volley stage)
            static void PushBack(CScnObj *pObj, long);
            static void EnableDepthOfFieldEffect(int bEnable);
            static void SetDepthOfFieldParam(long type, float fx, float fy, float fz);
            static void ReqCleanUp();
            // Change of the screen split, applied on the next frame, and the
            // return to the split before it; the callback may be null.
            // (parameter meanings guessed)
            static void ReqPushViewSplitType(CWorldEnv::ViewSplitType type,
                                             void (*pCallback)(CWorldEnv::ViewSplitType));
            static void ReqPopViewSplitType(void (*pCallback)(CWorldEnv::ViewSplitType));
            // (added for DOL game/msw/CGame.cpp; meanings guessed) A wipe of
            // the screen with model pName of rFile, over fFrame frames.
            static void StartCrossFade(nw4r::g3d::ResFile rFile, const char *pName, float fFrame, int);
            static void ResetDepthOfFieldParam();
            // (fn_80077448, unnamed in the DOL; name and parameter meanings
            // guessed) Switches the full-screen blur on or off: its colour,
            // a mode word, a scale and an angle in radians, kept in [0, 2 pi).
            // msw::CGameBlurEffect (game/msw/CGameExDemoBase.cpp) drives it.
            static void SetBlurEffect(int bEnable, BCS_Color color, long nMode, float fScale, float fAngle);

            // (lbl_804CFD50, unnamed in the DOL; name guessed) A unit matrix
            // the render manager's 2D passes load as the position matrix
            // (GXLoadPosMtxImm with it, in .bss: filled at start-up). The
            // object there is 0x98 bytes; what follows the matrix is not
            // known.
            static FMTX34 m_mtxUnit;
    };
}

#endif /* !BCS_CRENDERMAN_HPP_ */
