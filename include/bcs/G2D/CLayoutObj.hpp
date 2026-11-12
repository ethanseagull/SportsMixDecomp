#ifndef BCS_G2D_CLAYOUTOBJ_HPP_
#define BCS_G2D_CLAYOUTOBJ_HPP_

#include "bcs/G2D/CLayoutBase.hpp"

namespace bcs {
    namespace G2D {

        // Lives in the DOL (next to CLayoutBase). A layout object whose
        // animations play by number. Only PlayAnim has a symbol; its body
        // reads CLayoutBase's 0x3C to 0x4C, so the class is taken to derive
        // from CLayoutBase. Added for DodgeF
        // game/dodge/CCockpitDodgeTelopInOutChange.cpp (msw::COutSignDodge).
        class CLayoutObj : public CLayoutBase {
            public:
                // Argument names guessed.
                void PlayAnim(long nAnim, float fSpeed, float fFrame);
                // (DOL fn_800A3390, no symbol; name guessed) Enables every
                // transform of animation nAnim (none when nAnim is negative),
                // after disabling all of the layout's when bExclusive is 1.
                // How many it enabled.
                long SetAnimEnable(long nAnim, int bExclusive);

            protected:
                // (added for DOL game/bcs/G2D/CLayoutBase.cpp, where PlayAnim
                // is; names guessed)
                float m_fSpeed; // 0x48
                long m_nAnim;   // 0x4C, the animation PlayAnim started
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTOBJ_HPP_ */
