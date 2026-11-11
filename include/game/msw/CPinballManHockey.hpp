#ifndef MSW_CPINBALLMANHOCKEY_HPP_
#define MSW_CPINBALLMANHOCKEY_HPP_

#include "game/msw/CPinballMan.hpp"

namespace msw {

    // The pinball manager of hockey stage 12 (CStageHockey12 makes one at
    // 0x218). Size 0x54: adds no data to CPinballMan. Its constructor is
    // inline (CStageHockey12's constructor stores the vtable after
    // CPinballMan's constructor). Its functions and vtable are in DodgeF's
    // game/hockey/CStgGmk_Pinball_Act_Hockey.cpp as weak copies, so they are
    // inline in the original; declared out of line here until that unit is
    // written (GetMaxNum gives 0x20, the GetParam_* read
    // HOCKEY_PRM_S12_BALL_*).
    class CPinballManHockey : public CPinballMan {
        public:
            CPinballManHockey() {}
            virtual ~CPinballManHockey();

            virtual long GetMaxNum() const;
            virtual long GetWinTeam() const;
            virtual CStgGmk_Pinball *NewGmk(bcs::CTask *pParent, int arg);
            virtual long GetParam_BallNum() const;
            virtual float GetParam_InitialInterval() const;
            virtual float GetParam_Interval() const;
            virtual float GetParam_ShotAngle() const;
            virtual long GetParam_ShotSideRate() const;
    };
}

#endif /* !MSW_CPINBALLMANHOCKEY_HPP_ */
