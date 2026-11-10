// Written from src/game/boss/CItemBallBoss.cpp by tools/decomp/port_unit.py (Boss -> Volley); check against VolleyF.
#include "game/msw/CItemBallVolley.hpp"

namespace msw {

bool CItemBallVolley::DoTrg(long trg) {
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

bool CItemBallVolley::TrgRebound_() {
    return false;
}

bool CItemBallVolley::TrgReboundToFree_() {
    return false;
}

bool CItemBallVolley::TrgFreeMode_() {
    return false;
}

bool CItemBallVolley::TrgChrHit_() {
    return false;
}

bool CItemBallVolley::TrgAttach_() {
    return false;
}

bool CItemBallVolley::TrgArtsAttach_() {
    return true;
}

bool CItemBallVolley::TrgPreparationTouch_() {
    return false;
}

} // namespace msw
