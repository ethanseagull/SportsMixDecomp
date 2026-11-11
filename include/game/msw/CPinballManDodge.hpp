#ifndef MSW_CPINBALLMANDODGE_HPP_
#define MSW_CPINBALLMANDODGE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CPinballMan.hpp"
#include "game/msw/CStgGmk_Pinball_Dodge.hpp"

namespace msw {

    // The pinball manager of dodge stage 12 (CStageDodge12 makes one at
    // 0x244). Size 0x54: adds no data to CPinballMan. Its constructor is
    // inline (CStageDodge12's constructor stores the vtable after
    // CPinballMan's constructor). The vtable and the functions are in
    // DodgeF's game/dodge/CStgGmk_Pinball_Act_Dodge.cpp: GetWinTeam and the
    // GetParam_* (the DODGE_PRM_S12_BALL_* parameters) out of line, the
    // destructor, GetMaxNum and NewGmk as weak copies after them (reverse
    // order of the declarations).
    class CPinballManDodge : public CPinballMan {
        public:
            CPinballManDodge() {}
            virtual ~CPinballManDodge() {}

            virtual long GetMaxNum() const { return 0x20; }
            virtual long GetWinTeam() const;
            virtual CStgGmk_Pinball *NewGmk(bcs::CTask *pParent, int arg)
            {
                return new (1) CStgGmk_Pinball_Dodge(pParent, arg);
            }
            virtual long GetParam_BallNum() const;
            virtual float GetParam_InitialInterval() const;
            virtual float GetParam_Interval() const;
            virtual float GetParam_ShotAngle() const;
            virtual long GetParam_ShotSideRate() const;
    };
}

#endif /* !MSW_CPINBALLMANDODGE_HPP_ */
