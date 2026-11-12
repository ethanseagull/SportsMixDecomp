// msw::CChrCoreBasket::MethdUseItem: puts the team's item on the held ball.
#include "game/msw/CChrCoreBasket.hpp"

#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"

int msw::CChrCoreBasket::MethdUseItem()
{
    long nItem;

    if (!CanbeUseItem()) {
        return 0;
    }
    if (m_nComLevel != 1) {
        nItem = CGame::GetTeamItem(m_nTeamNo, 0);
        DeleteStockItem();
    } else {
        // The star state always gives item 4.
        nItem = 4;
    }
    if (nItem < 0) {
        return 0;
    }

    CBall *pBall = GetAttachBall();
    if (IsPadCtrl()) {
        CGameBasket::GetInstance()->PlayExceptionalCameraSlow_(4);
    }
    pBall->AttachItemEffect(nItem);
    return 1;
}
