#include "game/msw/CItemBallHockey.hpp"

namespace msw {

long CItemBallHockey::DoTrg(long trg) {
    return CItemBall::DoTrg(trg);
}

long CItemBallHockey::TrgRebound_() {
    return false;
}

long CItemBallHockey::TrgReboundToFree_() {
    return false;
}

long CItemBallHockey::TrgFreeMode_() {
    return false;
}

long CItemBallHockey::TrgChrHit_() {
    return false;
}

long CItemBallHockey::TrgAttach_() {
    return false;
}

long CItemBallHockey::TrgDamage_() {
    return false;
}

} // namespace msw
