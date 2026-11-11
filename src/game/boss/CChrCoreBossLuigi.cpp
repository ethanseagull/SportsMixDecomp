// msw::CChrCoreBossLuigi and msw::CChrCoreBossMario: Luigi and Mario in the
// boss game. Each only adds a few effects to the CChrCoreBoss states.
#include "game/msw/CChrCoreBoss.hpp"

msw::CChrCoreBossLuigi::CChrCoreBossLuigi()
{
}

msw::CChrCoreBossLuigi::~CChrCoreBossLuigi()
{
}

void msw::CChrCoreBossLuigi::Reset_()
{
    CChrCoreBoss::Reset_();
}

// The start of a diving receive: two effects on the character's root node.
void msw::CChrCoreBossLuigi::SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *)
{
    bcs::FVEC3 ofs(0.0f, 0.4f, -0.7f);

    Eff_CreateAttach(0x13, 0x4EE, 0, &ofs, 1, 3, 0);
    Eff_CreateAttach(0x14, 0x4EF, 0, &ofs, 1, 3, 0);
}

// Landing after a smash spike. The original reads the landing point and
// does nothing with it (whatever used it is not in this build).
void msw::CChrCoreBossLuigi::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    CChrCoreBoss::SubStateSpikeDoLandStart_(pWork);

    if ((pWork->m_nFlag & 0x40) && (pWork->m_nFlag & 0x100)) {
        bcs::FVEC3 pos;
        GetTransNodePos(pos);
        pos.y = 0.0f;
    }
}

msw::CChrCoreBossMario::CChrCoreBossMario()
{
}

msw::CChrCoreBossMario::~CChrCoreBossMario()
{
}

void msw::CChrCoreBossMario::Reset_()
{
    CChrCoreBoss::Reset_();
}

void msw::CChrCoreBossMario::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
}
