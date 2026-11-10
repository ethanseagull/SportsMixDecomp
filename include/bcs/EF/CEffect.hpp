#ifndef BCS_EF_CEFFECT_HPP_
#define BCS_EF_CEFFECT_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    class CScnObj;

    namespace EF {

        // One effect of the effect system (DOL class). Only the members used
        // so far (TourF's Stage33 hangs the effects of a cluster on a
        // model). Parameter names and return types guessed.
        class CEffect {
            public:
                void SetParentMdl(const CScnObj *pParent, unsigned long nNode, int b);
                void SetAttachStatus(int b1, int b2, int b3);
                void SetLocalTranslate(const FVEC3 &vPos);

                // (name guessed; the DOL's fn_8001FA28) Shows the effect for
                // 1 and hides it otherwise: sets or clears bits 0x40 and
                // 0x80 of the word at 0xFC of the object at 0x44, and bit 2
                // of the word at 0x50 of each emitter. msw::cockpit::
                // CEffectSet::SetDispFlag calls it through a member pointer.
                void SetDispFlag(int bDisp);
        };
    }
}

#endif /* !BCS_EF_CEFFECT_HPP_ */
