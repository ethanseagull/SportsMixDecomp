#ifndef BCS_EF_CEFFECT_HPP_
#define BCS_EF_CEFFECT_HPP_

namespace bcs {
    namespace EF {

        // One effect of the effect system. Lives in the DOL. Only the member
        // used so far; layout not mapped.
        class CEffect {
            public:
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
