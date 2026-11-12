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
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTOBJ_HPP_ */
