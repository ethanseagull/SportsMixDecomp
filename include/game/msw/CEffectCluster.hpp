#ifndef CEFFECTCLUSTER_HPP_
#define CEFFECTCLUSTER_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    namespace EF {
        class CEffect;
    }
}

namespace msw {

    // A group of effects started together (DOL class). Only the functions
    // the modules call so far; layout not mapped.
    class CEffectCluster {
        public:
            void SetTranslate(const bcs::FVEC3 *pPos);
            // Rotation in radians.
            void SetRotateRad(const bcs::FVEC3 *pRot);
            // (parameter names and return types guessed) Fades the cluster
            // out over fFrame frames; the effects it holds.
            void SetFadeoutTime(float fFrame);
            long GetEffectNum();
            bcs::EF::CEffect *GetEffect(long nIndex);
    };
}

#endif /* !CEFFECTCLUSTER_HPP_ */
