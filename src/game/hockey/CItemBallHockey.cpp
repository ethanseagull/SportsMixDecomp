#include "game/msw/CItemBallHockey.hpp"

namespace msw {

bool CItemBallHockey::DoTrg(long trg) {
    return CItemBall::DoTrg(trg);
}

bool CItemBallHockey::TrgRebound_() {
    return false;
}

bool CItemBallHockey::TrgReboundToFree_() {
    return false;
}

bool CItemBallHockey::TrgFreeMode_() {
    return false;
}

bool CItemBallHockey::TrgChrHit_() {
    return false;
}

bool CItemBallHockey::TrgAttach_() {
    return false;
}

bool CItemBallHockey::TrgDamage_() {
    return false;
}

} // namespace msw
