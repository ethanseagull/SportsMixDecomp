// msw::CBallMarkerDodge: the marker above a ball in the dodge game.
#include "game/msw/CBallMarkerDodge.hpp"

#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"

// 1 without a ball or for a sleeping one; 2, which brings the marker up, for
// a loose ball that moves freely or is rebounding; 3 otherwise.
int msw::CBallMarkerDodge::AppearCheck()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall == 0) {
        return 1;
    }
    if (pBall->IsSleep()) {
        return 1;
    }
    if (!pBall->IsAttach() && pBall->IsFreeMoveMode()) {
        return 2;
    }
    if (!pBall->IsAttach() && pBall->IsNowRebound()) {
        return 2;
    }
    return 3;
}

msw::CBallMarkerDodge::~CBallMarkerDodge()
{
}
