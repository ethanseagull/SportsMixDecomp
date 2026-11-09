#ifndef BCS_G2D_CPANECTRL_HPP_
#define BCS_G2D_CPANECTRL_HPP_

#include "bcs/G2D/CLayoutBase.hpp"

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Size 0x54 (the size passed to operator new in
        // msw::CSelectMenuLayoutBase::CreateLytObj). CLayoutBase is at 0:
        // the same pointer is passed to functions taking either type.
        // Only the members used so far.
        class CPaneCtrl : public CLayoutBase {
            public:
                // The names of the two ints are guesses.
                void PlayAnim(long nAnim, float fSpeed, float fFrame, int bRecursive, int bReset);

                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp,
                // which creates one with `new (0)`.
                CPaneCtrl();

            private:
                // Brings the size to the 0x54 that `new` is asked for. The
                // constructor sets up a nw4r::ut::List here.
                unsigned char m_Unk48[0xC]; // 0x48
        };
    }
}

#endif /* !BCS_G2D_CPANECTRL_HPP_ */
