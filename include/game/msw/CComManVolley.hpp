#ifndef MSW_CCOMMANVOLLEY_HPP_
#define MSW_CCOMMANVOLLEY_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The volleyball computer manager's shared helpers (VolleyF
    // game/volley/CComManVolley.cpp). All static. Positions are returned in
    // the court's own frame (CComMan::World2Local).
    class CComManVolley : public CComMan {
        public:
            // Where the ball will come down; zero when there is no ball or
            // it has no target.
            static int GetBallTargetPos(CBall *pBall, bcs::FVEC3 &rPos);
            // Where a spike jump should aim for the ball; zero when none.
            static int GetBallSpikeJumpTargetPos(CBall *pBall, bcs::FVEC3 &rPos);
            // Not zero when pChr may touch ball nBall (negative: the ball in
            // play) now.
            static int CanbeTouch(CChrCore *pChr, long nBall);
            // pChr's target marker nMarker; zero when it has none.
            static int GetTargetMarkerPos(CChrCore *pChr, bcs::FVEC3 &rPos, long nMarker);
            // Where the ball touches the floor.
            static void GetTouchDownPos(CBall *pBall, bcs::FVEC3 &rPos);
    };
}

#endif /* !MSW_CCOMMANVOLLEY_HPP_ */
