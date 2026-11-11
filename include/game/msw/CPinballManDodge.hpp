#ifndef MSW_CPINBALLMANDODGE_HPP_
#define MSW_CPINBALLMANDODGE_HPP_

#include "game/msw/CPinballMan.hpp"

namespace msw {

    // The pinball manager of dodge stage 12 (CStageDodge12 makes one at
    // 0x244). Size 0x54: adds no data to CPinballMan. Its constructor is
    // inline (CStageDodge12's constructor stores the vtable after
    // CPinballMan's constructor). Its functions and vtable are in DodgeF's
    // game/dodge/CStgGmk_Pinball_Act_Dodge.cpp as weak copies, so they are
    // inline in the original; declared out of line here until that unit is
    // written (GetMaxNum gives 0x20, the GetParam_* read
    // DODGE_PRM_S12_BALL_*).
    class CPinballManDodge : public CPinballMan {
        public:
            CPinballManDodge() {}
            virtual ~CPinballManDodge();

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

#endif /* !MSW_CPINBALLMANDODGE_HPP_ */
