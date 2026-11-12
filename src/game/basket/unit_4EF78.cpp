// msw::CChrCoreBasket: the size of the character's state queue work.
// Probably the tail of the CChrCoreBasket file that starts at 0x3F4E8 (see
// the report on the unit boundaries).
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

long msw::CChrCoreBasket::GetQueueWorkSize() const
{
    return 0xB8;
}
