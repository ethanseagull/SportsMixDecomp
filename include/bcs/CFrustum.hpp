#ifndef BCS_CFRUSTUM_HPP_
#define BCS_CFRUSTUM_HPP_

#include "nw4r/math/types.h"

namespace bcs {

    // A view frustum. A DOL class; only what is used so far.
    class CFrustum {
        public:
            // fn_800548D4 (name guessed after nw4r::math::Frustum). Whether
            // the box is outside (0), partly inside (1) or inside (2).
            int IntersectAABB_Ex(const nw4r::math::AABB *pBox) const;
    };
}

#endif /* !BCS_CFRUSTUM_HPP_ */
