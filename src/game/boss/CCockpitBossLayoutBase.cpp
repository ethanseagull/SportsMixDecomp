// msw::CCockpitBossLayoutBase: base of the boss game cockpit layouts.
#include "game/msw/CCockpitBossLayoutBase.hpp"

msw::CCockpitBossLayoutBase::CCockpitBossLayoutBase(long nId)
    : CCockpitLayoutBase(nId)
{
}

// (guess) The original destructor stores both vtable pointers before it
// calls the base destructor. The compiler leaves those stores out when the
// body is empty and keeps them when the body calls an inline function, even
// one that compiles to nothing; `;`, `(void)0;`, `return;` and an empty base
// or member with an inline destructor do not keep them. So the original
// called something that is empty in this build. What it was is not known;
// CCockpitTournamentMenuWinTrophy.cpp (TourF) has the same shape.
static inline void Nothing() {}

msw::CCockpitBossLayoutBase::~CCockpitBossLayoutBase()
{
    Nothing();
}

int msw::CCockpitBossLayoutBase::AllOut()
{
    LifeGaugeOut();
    BossGaugeOut();
    CoinOut();
    return 0;
}

int msw::CCockpitBossLayoutBase::AllIn()
{
    LifeGaugeIn();
    BossGaugeIn();
    CoinIn();
    return 0;
}

int msw::CCockpitBossLayoutBase::AllOutWait()
{
    LifeGaugeOutWait();
    BossGaugeOutWait();
    CoinOutWait();
    return 0;
}

int msw::CCockpitBossLayoutBase::AllInWait()
{
    LifeGaugeInWait();
    BossGaugeInWait();
    CoinInWait();
    return 0;
}
