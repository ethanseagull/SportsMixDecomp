#ifndef MSW_CPINBALLMANVOLLEY_HPP_
#define MSW_CPINBALLMANVOLLEY_HPP_

#include "game/msw/CPinballMan.hpp"

namespace msw {

    // The pinball manager of volley stage 12 (CStageVolley12 makes one at
    // 0x200). Size 0x54: adds no data to CPinballMan. Its constructor is
    // inline (CStageVolley12's constructor stores the vtable after
    // CPinballMan's constructor). Its functions and vtable are in VolleyF's
    // game/volley/CStgGmk_Pinball_Act_Volley.cpp as weak copies, so they are
    // inline in the original; declared out of line here until that unit is
    // written (GetMaxNum gives 0x20, the GetParam_* read
    // VOLLEY_PRM_S12_BALL_*).
    class CPinballManVolley : public CPinballMan {
        public:
            CPinballManVolley() {}
            virtual ~CPinballManVolley();

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

#endif /* !MSW_CPINBALLMANVOLLEY_HPP_ */
