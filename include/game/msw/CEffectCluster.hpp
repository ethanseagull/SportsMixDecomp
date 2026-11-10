#ifndef CEFFECTCLUSTER_HPP_
#define CEFFECTCLUSTER_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // A group of effects started together (DOL class). Only the functions
    // the modules call so far; layout not mapped.
    class CEffectCluster {
        public:
            void SetTranslate(const bcs::FVEC3 *pPos);
            // Rotation in radians.
            void SetRotateRad(const bcs::FVEC3 *pRot);
    };
}

#endif /* !CEFFECTCLUSTER_HPP_ */
