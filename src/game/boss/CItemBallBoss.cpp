#include "game/msw/CItemBallBoss.hpp"

namespace msw {

bool CItemBallBoss::DoTrg(long trg) {
    switch (trg) {
    case TRG_RECEIVE:
        return TrgReceive_();
    case TRG_BLOCK:
        return TrgBlock_();
    case TRG_ARTS_ATTACH:
        return TrgArtsAttach_();
    case TRG_PREPARATION_TOUCH:
        return TrgPreparationTouch_();
    default:
        return CItemBall::DoTrg(trg);
    }
}

bool CItemBallBoss::TrgRebound_() {
    return false;
}

bool CItemBallBoss::TrgReboundToFree_() {
    return false;
}

bool CItemBallBoss::TrgFreeMode_() {
    return false;
}

bool CItemBallBoss::TrgChrHit_() {
    return false;
}

bool CItemBallBoss::TrgAttach_() {
    return false;
}

bool CItemBallBoss::TrgArtsAttach_() {
    return true;
}

bool CItemBallBoss::TrgPreparationTouch_() {
    return false;
}

} // namespace msw
