// msw::CChrCoreBossNinja: the ninja (Ninja) in the boss game. It adds a puff
// of smoke when it turns visible or invisible during state 0x2C (meaning not
// known).
#include "game/msw/CChrCoreBoss.hpp"

msw::CChrCoreBossNinja::CChrCoreBossNinja()
{
}

msw::CChrCoreBossNinja::~CChrCoreBossNinja()
{
}

void msw::CChrCoreBossNinja::Reset_()
{
    CChrCoreBoss::Reset_();
}

// Called by the motion trigger that shows (bVisible != 0) or hides the body.
// Bit 0x10000000 of the status flags is set while the body is hidden
// (guessed from the two tests).
void msw::CChrCoreBossNinja::CallbackMotTrg_BodyVisible_(long bVisible, long)
{
    if (bVisible != 0) {
        if (!(m_nStatusFlag & 0x10000000)) {
            bcs::FVEC3 pos;
            GetTransNodePos(pos);
            if (GetState() == 0x2C) {
                Eff_CreateSimple(0x519, pos, 3, 0, 0);
            }
        }
    } else {
        if (m_nStatusFlag & 0x10000000) {
            bcs::FVEC3 pos;
            GetTransNodePos(pos);
            if (GetState() == 0x2C) {
                Eff_CreateSimple(0x51A, pos, 3, 0, 0);
            }
        }
    }
}
