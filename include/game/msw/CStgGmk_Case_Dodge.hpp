#ifndef MSW_CSTGGMK_CASE_DODGE_HPP_
#define MSW_CSTGGMK_CASE_DODGE_HPP_

#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include "game/msw/DodgePrm.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the dodgeball mode.

    // The actor, size 0x214 (nothing added to msw::CStgGmk_Case_Act), with
    // the DODGE_PRM_S15_* parameters. Unlike the basketball actor it has its
    // own GetParam_OuterRect, and GetParam_Width is not inline. The other
    // getters are inline: the original emits them after the destructor and
    // CStgGmk_Case_Act's weak functions, in reverse order of these
    // declarations. The rest is in game/dodge/CStgGmk_Case_Act_Dodge.cpp; the
    // constructor is used by msw::CStgGmk_Case_Dodge::NewAct.
    class CStgGmk_Case_Act_Dodge : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Dodge(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Case_Act_Dodge();

            virtual float GetParam_Width() const;
            virtual float GetParam_Depth() const { return DODGE_PRM_S15_CASE_DEPTH; }
            virtual float GetParam_Height() const { return DODGE_PRM_S15_CASE_HEIGHT; }
            virtual float GetParam_MdlWidth() const { return 3.0f; }
            virtual float GetParam_MdlDepth() const { return 1.0f; }
            virtual float GetParam_MdlHeight() const { return 1.0f; }
            // The three kinds of case (GetSubType_) move differently.
            virtual float GetParam_Acc() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return DODGE_PRM_S15_CASE_ACC_0;
                    case 1:
                        return DODGE_PRM_S15_CASE_ACC_1;
                    case 2:
                        return DODGE_PRM_S15_CASE_ACC_2;
                    default:
                        return DODGE_PRM_S15_CASE_ACC_0;
                }
            }
            virtual float GetParam_Dec() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return DODGE_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(DODGE_PRM_S15_CASE_DEC_RND_0);
                    case 1:
                        return DODGE_PRM_S15_CASE_DEC_1 + bcs::BCS_RandMax(DODGE_PRM_S15_CASE_DEC_RND_1);
                    case 2:
                        return DODGE_PRM_S15_CASE_DEC_2 + bcs::BCS_RandMax(DODGE_PRM_S15_CASE_DEC_RND_2);
                    default:
                        return DODGE_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(DODGE_PRM_S15_CASE_DEC_RND_0);
                }
            }
            virtual float GetParam_VelLimit() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return DODGE_PRM_S15_CASE_VEL_0;
                    case 1:
                        return DODGE_PRM_S15_CASE_VEL_1;
                    case 2:
                        return DODGE_PRM_S15_CASE_VEL_2;
                    default:
                        return DODGE_PRM_S15_CASE_VEL_0;
                }
            }
            virtual void GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_MaxLife() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return DODGE_PRM_S15_CASE_LIFE_0;
                    case 1:
                        return DODGE_PRM_S15_CASE_LIFE_1;
                    case 2:
                        return DODGE_PRM_S15_CASE_LIFE_2;
                    default:
                        return DODGE_PRM_S15_CASE_LIFE_0;
                }
            }
            // Declared after GetParam_MaxLife: the original emits it before it
            // (weak inlines come out in reverse declaration order).
            // The interval is in seconds, returned in frames.
            virtual float GetParam_Interval() const { return 60.0f * DODGE_PRM_S15_CASE_INTERVAL; }
            virtual float GetParam_LocPosY() const { return DODGE_PRM_S15_CASE_POS_Y; }
            virtual float GetParam_KaronScale() const { return DODGE_PRM_S15_KARON_SCALE; }
            virtual float GetParam_KaronCollScale() const { return DODGE_PRM_S15_KARON_COLL_SCALE; }
            virtual float GetParam_KaronSpeed() const { return DODGE_PRM_S15_KARON_SPEED; }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBreak() { CreateKaron(); }
    };

    // One case, size 0x78 (nothing added to msw::CStgGmk_Case). Made by
    // msw::CStageDodge15::NewCase. All inline: game/dodge/CStageDodge15.cpp
    // emits the vtable and, after CStgGmk_Case's weak functions, NewAct and
    // the destructor (reverse order of the declarations here).
    class CStgGmk_Case_Dodge : public CStgGmk_Case {
        public:
            CStgGmk_Case_Dodge(bcs::CTask *pParent) : CStgGmk_Case(pParent, 0) {}
            virtual ~CStgGmk_Case_Dodge() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Case_Act_Dodge(this); }
    };
}

#endif /* !MSW_CSTGGMK_CASE_DODGE_HPP_ */
