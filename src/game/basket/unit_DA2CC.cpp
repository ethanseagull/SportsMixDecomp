// msw::CComDrvBasketHB: the two per-frame hooks, both handed to the script.
#include "game/msw/CComDrvBasketHB.hpp"

#include "bcs/BCS_Math.hpp"

// 70 degrees in radians, unused (see CRadarIcon2DBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

void msw::CComDrvBasketHB::ActInterrupt()
{
    CallMainScriptFunc("ActInterrupt..I.", 0);
}

void msw::CComDrvBasketHB::ThkMain()
{
    CallMainScriptFunc("ThkMain...");
}
