// msw::CChrCoreBasket::MethdUseItemStar. Probably the tail of the
// CChrCoreBasket method file that starts at 0x5812C (unit_581FC), together
// with unit_65AF0 and unit_65B7C.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

int msw::CChrCoreBasket::MethdUseItemStar(long)
{
    return 0;
}
