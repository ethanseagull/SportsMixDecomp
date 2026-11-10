#include "game/msw/CComManVolley.hpp"

#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    int CComManVolley::GetBallTargetPos(CBall *pBall, bcs::FVEC3 &rPos)
    {
        if (pBall == 0) {
            return 0;
        }
        if (!pBall->GetTargetPos(rPos)) {
            return 0;
        }
        rPos = World2Local(rPos, -1);
        return 1;
    }

    int CComManVolley::GetBallSpikeJumpTargetPos(CBall *pBall, bcs::FVEC3 &rPos)
    {
        if (pBall == 0) {
            return 0;
        }
        if (!pBall->GetVolleySpikeJumpTargetPos(rPos)) {
            return 0;
        }
        rPos = World2Local(rPos, -1);
        return 1;
    }

    int CComManVolley::CanbeTouch(CChrCore *pChr, long nBall)
    {
        if (nBall < 0) {
            nBall = GetActiveBallIdx();
        }
        if (nBall < 0) {
            return 0;
        }
        if (pChr->GetTeamNo() != CGame::g_GameWork.GetUserTeam()) {
            return 1;
        }
        return pChr->IsVolleyRightBallTouch(nBall);
    }

    int CComManVolley::GetTargetMarkerPos(CChrCore *pChr, bcs::FVEC3 &rPos, long nMarker)
    {
        if (!pChr->GetVolleyTargetMarkerPos(rPos, nMarker)) {
            return 0;
        }
        rPos = World2Local(rPos, -1);
        return 1;
    }

    void CComManVolley::GetTouchDownPos(CBall *pBall, bcs::FVEC3 &rPos)
    {
        pBall->GetVolleyTouchDownPos(rPos, 1);
        rPos = World2Local(rPos, -1);
    }
}
