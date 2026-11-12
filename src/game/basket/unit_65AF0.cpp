// msw::CChrCoreBasket::CanbeUseItem: whether the character may use an item now.
#include "game/msw/CChrCoreBasket.hpp"

#include "game/msw/CBall.hpp"

int msw::CChrCoreBasket::CanbeUseItem()
{
    // In the star state (condition 1) no held ball or stocked item is needed.
    if (m_nComLevel != 1) {
        if (!IsHoldBall()) {
            return 0;
        }
        if (!IsHaveStockItem()) {
            return 0;
        }
    }
    if (GetAttachBall()->IsItemBall()) {
        return 0;
    }
    return 1;
}
