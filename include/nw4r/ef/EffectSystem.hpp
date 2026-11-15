#ifndef NW4R_EF_EFFECTSYSTEM_HPP_
#define NW4R_EF_EFFECTSYSTEM_HPP_

#include "nw4r/math/types.h"

namespace nw4r {
    namespace ef {

        // Minimal declarations written for this project from the DOL's
        // mangled names and the calls in bcs::EF::CManager. Only what is
        // used so far.
        class MemoryManagerBase {
            public:
                virtual ~MemoryManagerBase();
                // Slot 0x0C, called once per frame by bcs::EF::CManager::Main.
                virtual void GarbageCollection();
        };

        class EffectSystem {
            public:
                // (the DOL's fn_80015A34) The one effect system: the
                // address of a 0xC070-byte global built by a static
                // initialiser in the DOL.
                static EffectSystem *GetInstance();

                void SetProcessCamera(const math::VEC3 &rPos, const math::MTX34 &rMtx, float f1, float f2); // meaning of f1, f2 unknown

                MemoryManagerBase *mMemoryManager; // 0x00 (name guessed)
        };
    }
}

#endif /* !NW4R_EF_EFFECTSYSTEM_HPP_ */
