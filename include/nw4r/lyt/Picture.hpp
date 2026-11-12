#ifndef NW4R_LYT_PICTURE_HPP_
#define NW4R_LYT_PICTURE_HPP_

#include "nw4r/lyt/Pane.hpp"
#include "nw4r/math/types.h"
#include "nw4r/ut/RuntimeTypeInfo.h"

namespace nw4r {
    namespace lyt {

        // A pane that draws a textured quad. Lives in the DOL. Only what
        // MenuF game/menu/CBgDisplay.cpp needs: its type, for
        // ut::DynamicCast, and the texture coordinate functions (out of line
        // in the DOL). Names follow the public ogws decompilation's
        // description of nw4r::lyt (from memory, no code copied). Never
        // instantiated.
        class Picture : public Pane {
            public:
                static const ut::detail::RuntimeTypeInfo typeInfo;

                void ReserveTexCoord(unsigned char num);
                void SetTexCoordNum(unsigned char num);
                // The four corners of texture coordinate set idx.
                void GetTexCoord(unsigned long idx, math::VEC2 *pCoords) const;
                void SetTexCoord(unsigned long idx, const math::VEC2 *pCoords);
        };
    }
}

#endif /* !NW4R_LYT_PICTURE_HPP_ */
