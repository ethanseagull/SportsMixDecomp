// msw::CComDrvBoss's think function, which picks the next action. A file
// of its own (own float constants); the file name is not known.
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CNpcBehemothBoss.hpp"

namespace msw {

void CComDrvBoss::ActInterrupt()
{
}

// Picks the next action: shoot or go home with the ball, throw an item,
// go for the ball, a "?" panel or the Behemoth, else wait at home or
// somewhere near it. Fewer attacks while the team has the ball, none while
// the Behemoth is dangerous or nearly beaten.
void CComDrvBoss::ThkMain()
{
    CNpcBehemothBoss *pBoss = GetBehemoth();
    if (pBoss == 0) {
        return;
    }

    float fHPRate = 100.0f * pBoss->GetHP() / pBoss->GetMaxHP();
    float fPanelDist = CComMan::QPanel_GetNearDisXZ(m_MyPos, -1, 1);
    long nItemRate = 1;
    long nStickRate = 20;
    if (IsMyTeamHoldBall(-1)) {
        nItemRate = 10;
    }
    if (ShoudEscFromBoss()) {
        nStickRate = 0;
    }
    if (fHPRate <= 4.0f) {
        nItemRate = 0;
        nStickRate = 0;
    }

    if (m_pChr->IsHoldBall()) {
        if (LotRate() < 20) {
            ActCallShoot();
            return;
        }
        ActCallGoHome();
        return;
    }
    if (IsHaveMyStockItem() && LotRate() < nItemRate) {
        ActCallThrowItem();
        return;
    }
    if (ShoudReceiveGo()) {
        ActCallReceiveGo();
        return;
    }
    if (fPanelDist >= 0.0f && fPanelDist < 5.0f && IsShoudGetQPanel() && LotRate() < 70) {
        ActCallGetQpanel();
        return;
    }
    if (LotRate() < nStickRate) {
        ActCallStickAttack();
        return;
    }
    if (GetActiveBall() != 0) {
        ActCallGoHome();
        return;
    }
    ActCallGoRand();
}

} // namespace msw
