#include "game/msw/CItemBallBoss.hpp"

namespace msw {

long CItemBallBoss::DoTrg(long trg) {
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

long CItemBallBoss::TrgRebound_() {
    return false;
}

long CItemBallBoss::TrgReboundToFree_() {
    return false;
}

long CItemBallBoss::TrgFreeMode_() {
    return false;
}

long CItemBallBoss::TrgChrHit_() {
    return false;
}

long CItemBallBoss::TrgAttach_() {
    return false;
}

long CItemBallBoss::TrgArtsAttach_() {
    return true;
}

long CItemBallBoss::TrgPreparationTouch_() {
    return false;
}

} // namespace msw
