// msw::CCockpitDodgeLayoutBase: base of the dodge game cockpit layouts.
#include "game/msw/CCockpitDodgeLayoutBase.hpp"

msw::CCockpitDodgeLayoutBase::CCockpitDodgeLayoutBase(long nId)
    : CCockpitLayoutBase(nId)
{
}

// The original stores the vtables here; see notes/decisions.md, "Empty
// inline call in a destructor".
static inline void Nothing() {}

msw::CCockpitDodgeLayoutBase::~CCockpitDodgeLayoutBase()
{
    Nothing();
}

int msw::CCockpitDodgeLayoutBase::AllOut()
{
    GaugeOut();
    SetCountOut();
    CoinOut();
    return 0;
}

int msw::CCockpitDodgeLayoutBase::AllIn()
{
    GaugeIn();
    SetCountIn();
    CoinIn();
    return 0;
}

int msw::CCockpitDodgeLayoutBase::AllOutWait()
{
    GaugeOutWait();
    SetCountOutWait();
    CoinOutWait();
    return 0;
}

int msw::CCockpitDodgeLayoutBase::AllInWait()
{
    GaugeInWait();
    SetCountInWait();
    CoinInWait();
    return 0;
}
