// Written from src/game/boss/CItemBallBoss.cpp by tools/decomp/port_unit.py (Boss -> Hockey); check against HockeyF.
#include "game/msw/CItemBallHockey.hpp"

namespace msw {

bool CItemBallHockey::DoTrg(long trg) {
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

bool CItemBallHockey::TrgArtsAttach_() {
    return true;
}

bool CItemBallHockey::TrgPreparationTouch_() {
    return false;
}

} // namespace msw
