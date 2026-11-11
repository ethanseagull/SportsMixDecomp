#ifndef MSW_CSTGGMK_CASE_HOCKEY_HPP_
#define MSW_CSTGGMK_CASE_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"
#include "bcs/BCS_Rand.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the hockey mode.

    // The actor, size 0x218 (one float added to msw::CStgGmk_Case_Act), with
    // the HOCKEY_PRM_S15_* parameters. Its out-of-line functions are in
    // game/hockey/CStgGmk_Case_Act_Hockey.cpp, which also emits the inline
    // ones here (in reverse declaration order) with the vtable; unlike the
    // dodgeball actor it keeps CStgGmk_Case_Act::GetParam_OuterRect. The
    // constructor is inline (msw::CStgGmk_Case_Hockey::NewAct).
    class CStgGmk_Case_Act_Hockey : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Hockey(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) { m_fReflectWait = 0.0f; }
            virtual ~CStgGmk_Case_Act_Hockey();

            virtual void OnUpdate(float rate);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual float GetParam_Width() const { return HOCKEY_PRM_S15_CASE_WIDTH; }
            virtual float GetParam_Depth() const { return HOCKEY_PRM_S15_CASE_DEPTH; }
            virtual float GetParam_Height() const { return HOCKEY_PRM_S15_CASE_HEIGHT; }
            virtual float GetParam_MdlWidth() const { return 4.0f; }
            virtual float GetParam_MdlDepth() const { return 1.5f; }
            virtual float GetParam_MdlHeight() const { return 1.0f; }
            virtual float GetParam_Acc() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return HOCKEY_PRM_S15_CASE_ACC_0;
                    case 1:
                        return HOCKEY_PRM_S15_CASE_ACC_1;
                    case 2:
                        return HOCKEY_PRM_S15_CASE_ACC_2;
                }
                return HOCKEY_PRM_S15_CASE_ACC_0;
            }
            virtual float GetParam_Dec() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return HOCKEY_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(HOCKEY_PRM_S15_CASE_DEC_RND_0);
                    case 1:
                        return HOCKEY_PRM_S15_CASE_DEC_1 + bcs::BCS_RandMax(HOCKEY_PRM_S15_CASE_DEC_RND_1);
                    case 2:
                        return HOCKEY_PRM_S15_CASE_DEC_2 + bcs::BCS_RandMax(HOCKEY_PRM_S15_CASE_DEC_RND_2);
                }
                return HOCKEY_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(HOCKEY_PRM_S15_CASE_DEC_RND_0);
            }
            virtual float GetParam_VelLimit() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return HOCKEY_PRM_S15_CASE_VEL_0;
                    case 1:
                        return HOCKEY_PRM_S15_CASE_VEL_1;
                    case 2:
                        return HOCKEY_PRM_S15_CASE_VEL_2;
                }
                return HOCKEY_PRM_S15_CASE_VEL_0;
            }
            virtual float GetParam_MaxLife() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return HOCKEY_PRM_S15_CASE_LIFE_0;
                    case 1:
                        return HOCKEY_PRM_S15_CASE_LIFE_1;
                    case 2:
                        return HOCKEY_PRM_S15_CASE_LIFE_2;
                }
                return HOCKEY_PRM_S15_CASE_LIFE_0;
            }
            // Seconds to frames.
            virtual float GetParam_Interval() const { return 60.0f * HOCKEY_PRM_S15_CASE_INTERVAL; }
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_LocPosY() const { return HOCKEY_PRM_S15_CASE_POS_Y; }
            virtual float GetParam_KaronScale() const { return HOCKEY_PRM_S15_KARON_SCALE; }
            virtual float GetParam_KaronCollScale() const { return HOCKEY_PRM_S15_KARON_COLL_SCALE; }
            virtual float GetParam_KaronSpeed() const { return HOCKEY_PRM_S15_KARON_SPEED; }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBreak() { CreateKaron(); }
            // The case reflects the puck (OnReflectHit).
            virtual bool IsEnableDefColl() const { return true; }
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);

        private:
            float m_fReflectWait; // 0x214, frames until the case reflects the puck again (name guessed)
    };

    // One case, size 0x78 (nothing added to msw::CStgGmk_Case). Made by
    // msw::CStageHockey15::NewCase. All inline: game/hockey/CStageHockey15.cpp
    // emits the vtable and, after CStgGmk_Case's weak functions, NewAct and
    // the destructor (reverse order of the declarations here).
    class CStgGmk_Case_Hockey : public CStgGmk_Case {
        public:
            CStgGmk_Case_Hockey(bcs::CTask *pParent) : CStgGmk_Case(pParent, 0) {}
            virtual ~CStgGmk_Case_Hockey() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Case_Act_Hockey(this); }
    };
}

#endif /* !MSW_CSTGGMK_CASE_HOCKEY_HPP_ */
