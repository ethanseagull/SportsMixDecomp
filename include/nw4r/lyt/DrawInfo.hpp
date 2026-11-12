#ifndef NW4R_LYT_DRAWINFO_HPP_
#define NW4R_LYT_DRAWINFO_HPP_

#include "nw4r/math/types.h"
#include "nw4r/ut/Rect.h"

namespace nw4r {
    namespace lyt {

        // What a layout is drawn with. Lives in the DOL (constructor and
        // destructor out of line). Added for MenuF game/menu/CBgDisplay.cpp;
        // the offsets are what its CNwLayout::GetLayoutDrawTexture writes,
        // the names follow the public ogws decompilation's description of
        // nw4r::lyt (from memory, no code copied).
        class DrawInfo {
            public:
                DrawInfo();
                virtual ~DrawInfo();

                void SetViewMtx(const math::MTX34 &rMtx) { mViewMtx = rMtx; }
                void SetViewRect(const ut::Rect &rRect) { mViewRect = rRect; }
                void SetLocationAdjustScale(const math::VEC2 &rScale) { mLocationAdjustScale = rScale; }
                void SetLocationAdjust(bool bAdjust) { mFlag.locationAdjust = bAdjust; }

            private:
                math::MTX34 mViewMtx;            // 0x04
                ut::Rect mViewRect;              // 0x34
                math::VEC2 mLocationAdjustScale; // 0x44
                float mGlobalAlpha;              // 0x4C
                struct {
                    unsigned char mulViewDraw : 1;
                    unsigned char influencedAlpha : 1;
                    unsigned char locationAdjust : 1;
                    unsigned char invisiblePaneCalculateMtx : 1;
                } mFlag;                         // 0x50
        };
    }
}

#endif /* !NW4R_LYT_DRAWINFO_HPP_ */
