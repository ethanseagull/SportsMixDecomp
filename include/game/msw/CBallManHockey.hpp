#ifndef MSW_CBALLMANHOCKEY_HPP_
#define MSW_CBALLMANHOCKEY_HPP_

#include "game/msw/CBallMan.hpp"

namespace msw {

    // The ball manager of the hockey game (HockeyF). It adds no members to
    // CBallMan (size 0x58) and overrides only Create_. The game has one ball
    // (the puck), at place 0 of a table of four. The four floats the
    // constructor sets are CBallMan's bounce and roll rates.
    class CBallManHockey : public CBallMan {
        public:
            CBallManHockey(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax) {
                m_fWallBoundDecRate = 0.98f;
                m_fFloorBoundDecRateXZ = 0.999f;
                m_fFloorBoundDecRateY = 0.55f;
                m_fFloorRoundDecRateXZ = 0.98f;
            }
            virtual ~CBallManHockey();

            virtual void Create_();

            // Returns the instance, creating it if there is none yet.
            static CBallMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CBALLMANHOCKEY_HPP_ */
