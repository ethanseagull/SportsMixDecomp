#ifndef MSW_CBALLMANBOSS_HPP_
#define MSW_CBALLMANBOSS_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the boss game (BossF). It adds no members to
    // CBallMan (size 0x58). The game has one ball, at place 0 of a table of
    // eight.
    class CBallManBoss : public CBallMan {
        public:
            CBallManBoss(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax) {}
            virtual ~CBallManBoss();

            virtual void MainFunc_();
            // The ball in play whose position is nearest to, or furthest
            // from, rPos.
            virtual CBall *GetNearPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetFarPosBall_(const bcs::FVEC3 &rPos);
            // Both return ball 0 here.
            virtual CBall *GetNearTargetPosBall_(const bcs::FVEC3 &rPos);
            virtual CBall *GetFarTargetPosBall_(const bcs::FVEC3 &rPos);
            virtual void Create_();
            virtual CBall *AddBall_(CBall *pBall);

            // Returns the instance, creating it if there is none yet.
            static CBallMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CBALLMANBOSS_HPP_ */
