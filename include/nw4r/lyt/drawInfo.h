#ifndef NW4R_LYT_DRAWINFO_H_
#define NW4R_LYT_DRAWINFO_H_

#include "nw4r/math/types.h"
#include "nw4r/ut/Rect.h"

// Added for game/msw/CNowLoading.cpp. The layout (0x54 bytes) is what that
// function writes into its local DrawInfo; the member names follow the
// public ogws decompilation's description of nw4r::lyt::DrawInfo (no code
// copied).

namespace nw4r {
    namespace lyt {

        class DrawInfo {
            public:
                DrawInfo();
                virtual ~DrawInfo();

                void SetViewMtx(const math::MTX34 &rMtx) { mViewMtx = rMtx; }
                void SetViewRect(const ut::Rect &rRect) { mViewRect = rRect; }
                void SetLocationAdjustScale(const math::VEC2 &rScale) { mLocationAdjustScale = rScale; }
                void SetLocationAdjust(bool bAdjust) { mFlag.locationAdjust = bAdjust; }

            private:
                math::MTX34 mViewMtx;             // 0x04
                ut::Rect mViewRect;               // 0x34
                math::VEC2 mLocationAdjustScale;  // 0x44
                float mGlobalAlpha;               // 0x4C
                struct {
                    unsigned char mulViewDraw : 1;
                    unsigned char influencedAlpha : 1;
                    unsigned char locationAdjust : 1;
                    unsigned char invisiblePaneCalculateMtx : 1;
                    unsigned char debugDrawMode : 1;
                } mFlag;                          // 0x50
        };
    }
}

#endif /* !NW4R_LYT_DRAWINFO_H_ */
