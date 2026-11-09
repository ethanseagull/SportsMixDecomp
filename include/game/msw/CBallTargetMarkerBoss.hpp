#ifndef MSW_CBALLTARGETMARKERBOSS_HPP_
#define MSW_CBALLTARGETMARKERBOSS_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // The marker on the court where the boss game's ball will come down
    // (BossF), size 0x98: bcs::CTask at 0x00,
    // bcs::CFsm<CBallTargetMarkerBoss> at 0x34 (vptr at 0x78), own members
    // from 0x7C (read from the head of the constructor).
    //
    // INCOMPLETE: only what game/boss/CBallManBoss.cpp needs to create one.
    // The class's own virtual functions (Main, Disp, UpdateViewFlag_,
    // UpdateColor_) and its state functions are not declared yet, and the
    // members are not mapped.
    class CBallTargetMarkerBoss : public bcs::CTask, public bcs::CFsm<CBallTargetMarkerBoss> {
        public:
            // `nBallHandle` is the task id of the ball the marker follows.
            // (what the int selects is not known; CBallManBoss passes 1)
            CBallTargetMarkerBoss(bcs::CTask *pParent, unsigned long nBallHandle, int);
            virtual ~CBallTargetMarkerBoss();
            virtual void Main();

        private:
            unsigned char m_Unk7C[0x1C]; // 0x7C (the ball's handle is at 0x88)
    };
}

#endif /* !MSW_CBALLTARGETMARKERBOSS_HPP_ */
