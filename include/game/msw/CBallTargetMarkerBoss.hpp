#ifndef MSW_CBALLTARGETMARKERBOSS_HPP_
#define MSW_CBALLTARGETMARKERBOSS_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace bcs {
    class CMdlObj;
    class CScnGroup;
}

namespace msw {

    // The marker on the court where the boss game's ball will come down
    // (BossF), size 0x98: bcs::CTask at 0x00,
    // bcs::CFsm<CBallTargetMarkerBoss> at 0x34 (vptr at 0x78), own members
    // from 0x7C. A smaller cousin of VolleyF's CBallTargetMarker: a ground
    // mark and a chance-ball mark, no item mark and no rings. Its functions
    // are in game/boss/CBallTargetMarkerBoss.cpp; the virtual functions are
    // declared in the order of __vt__Q23msw21CBallTargetMarkerBoss. The
    // state functions are not virtual (the state table points at them
    // directly). Member names are guesses.
    class CBallTargetMarkerBoss : public bcs::CTask, public bcs::CFsm<CBallTargetMarkerBoss> {
        public:
            enum {
                STATE_SLEEP = 0,
                STATE_APPEAR = 1,
                STATE_MAIN = 2,
                STATE_DISAPPEAR = 3,
                STATE_NUM = 4
            };

            // `nBallHandle` is the task id of the ball the marker follows.
            // The int goes to Create_, which does not use it (CBallManBoss
            // passes 1).
            CBallTargetMarkerBoss(bcs::CTask *pParent, unsigned long nBallHandle, int);
            virtual ~CBallTargetMarkerBoss();
            virtual void Main();
            virtual void Disp();

            virtual void UpdateViewFlag_();
            virtual void UpdateColor_();

            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

        protected:
            void Create_(int);
            // (inline) 1: hide, 2: show, 3: let go.
            int CheckAppear_();
            void SetHeightObjState_();
            // (inline) Moves the marker to where the ball will come down.
            void UpdateTargetPos_();

            bcs::CScnGroup *m_pGroup;     // 0x7C, holds the ground mark
            bcs::CMdlObj *m_pMdlBase;     // 0x80, the ground mark
            bcs::CMdlObj *m_pMdlChance;   // 0x84, the chance-ball mark (animated)
            unsigned long m_nBallHandle;  // 0x88
            float m_fAppearRate;          // 0x8C, 0 to 1 while coming up
            int m_bChance;                // 0x90
            long m_nSpikeType;            // 0x94 (cleared, never read here)
    };
}

#endif /* !MSW_CBALLTARGETMARKERBOSS_HPP_ */
