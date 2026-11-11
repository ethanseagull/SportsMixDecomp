#ifndef MSW_CCOMMANBOSS_HPP_
#define MSW_CCOMMANBOSS_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The boss battle's computer manager helpers (BossF; defined in
    // game/boss/unit_51CE4.cpp among CComDrvBoss's functions). All static,
    // the same as CComManVolley's. Positions are returned in the court's
    // own frame (CComMan::World2Local).
    class CComManBoss : public CComMan {
        public:
            // Where the ball will come down; zero when there is no ball or
            // it has no target.
            static int GetBallTargetPos(CBall *pBall, bcs::FVEC3 &rPos);
            // Not zero when pChr may touch ball nBall (negative: the ball in
            // play) now.
            static int CanbeTouch(CChrCore *pChr, long nBall);
            // Where the ball touches the floor.
            static void GetTouchDownPos(CBall *pBall, bcs::FVEC3 &rPos);
    };
}

#endif /* !MSW_CCOMMANBOSS_HPP_ */
