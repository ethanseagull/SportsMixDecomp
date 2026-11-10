// msw::CCockpitVolleyLayoutBase: base of the volley game cockpit layouts.
#include "game/msw/CCockpitVolleyLayoutBase.hpp"

msw::CCockpitVolleyLayoutBase::CCockpitVolleyLayoutBase(long nId)
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

msw::CCockpitVolleyLayoutBase::~CCockpitVolleyLayoutBase()
{
    Nothing();
}

int msw::CCockpitVolleyLayoutBase::AllOut()
{
    ScoreOut();
    CoinOut();
    return 0;
}

int msw::CCockpitVolleyLayoutBase::AllIn()
{
    ScoreIn();
    CoinIn();
    return 0;
}

int msw::CCockpitVolleyLayoutBase::AllOutWait()
{
    ScoreOutWait();
    CoinOutWait();
    return 0;
}

int msw::CCockpitVolleyLayoutBase::AllInWait()
{
    ScoreInWait();
    CoinInWait();
    return 0;
}
