// Written from src/game/boss/CItemBallBoss.cpp by tools/decomp/port_unit.py (Boss -> Volley); check against VolleyF.
#include "game/msw/CItemBallVolley.hpp"

namespace msw {

long CItemBallVolley::DoTrg(long trg) {
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

long CItemBallVolley::TrgRebound_() {
    return false;
}

long CItemBallVolley::TrgReboundToFree_() {
    return false;
}

long CItemBallVolley::TrgFreeMode_() {
    return false;
}

long CItemBallVolley::TrgChrHit_() {
    return false;
}

long CItemBallVolley::TrgAttach_() {
    return false;
}

long CItemBallVolley::TrgArtsAttach_() {
    return true;
}

long CItemBallVolley::TrgPreparationTouch_() {
    return false;
}

} // namespace msw
