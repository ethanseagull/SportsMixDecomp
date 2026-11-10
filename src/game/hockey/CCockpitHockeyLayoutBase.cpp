// msw::CCockpitHockeyLayoutBase: base of the hockey game cockpit layouts.
#include "game/msw/CCockpitHockeyLayoutBase.hpp"

msw::CCockpitHockeyLayoutBase::CCockpitHockeyLayoutBase(long nId)
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

msw::CCockpitHockeyLayoutBase::~CCockpitHockeyLayoutBase()
{
    Nothing();
}

int msw::CCockpitHockeyLayoutBase::AllOut()
{
    ScoreOut();
    CoinOut();
    TimeOut();
    PeriodOut();
    return 0;
}

int msw::CCockpitHockeyLayoutBase::AllIn()
{
    ScoreIn();
    CoinIn();
    TimeIn();
    PeriodIn();
    return 0;
}

int msw::CCockpitHockeyLayoutBase::AllOutWait()
{
    ScoreOutWait();
    CoinOutWait();
    TimeOutWait();
    PeriodOutWait();
    return 0;
}

int msw::CCockpitHockeyLayoutBase::AllInWait()
{
    ScoreInWait();
    CoinInWait();
    TimeInWait();
    PeriodInWait();
    return 0;
}
