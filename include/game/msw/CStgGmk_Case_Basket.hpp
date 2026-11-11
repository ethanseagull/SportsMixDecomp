#ifndef MSW_CSTGGMK_CASE_BASKET_HPP_
#define MSW_CSTGGMK_CASE_BASKET_HPP_

#include "bcs/BCS_Rand.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the basketball mode.

    // The actor, size 0x214 (nothing added to msw::CStgGmk_Case_Act), with
    // the BASKET_PRM_S15_* parameters. Unlike the dodgeball actor it keeps
    // CStgGmk_Case_Act::GetParam_OuterRect. The getters are inline: the
    // original emits them after the destructor and CStgGmk_Case_Act's weak
    // functions, in reverse order of these declarations. The rest is in
    // game/basket/CStgGmk_Case_Act_Basket.cpp; the constructor is used by
    // msw::CStgGmk_Case_Basket::NewAct.
    class CStgGmk_Case_Act_Basket : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Basket(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Case_Act_Basket();

            virtual float GetParam_Width() const { return BASKET_PRM_S15_CASE_WIDTH; }
            virtual float GetParam_Depth() const { return BASKET_PRM_S15_CASE_DEPTH; }
            virtual float GetParam_Height() const { return BASKET_PRM_S15_CASE_HEIGHT; }
            virtual float GetParam_MdlWidth() const { return 2.3f; }
            virtual float GetParam_MdlDepth() const { return 1.0f; }
            virtual float GetParam_MdlHeight() const { return 1.0f; }
            // The three kinds of case (GetSubType_) move differently.
            virtual float GetParam_Acc() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return BASKET_PRM_S15_CASE_ACC_0;
                    case 1:
                        return BASKET_PRM_S15_CASE_ACC_1;
                    case 2:
                        return BASKET_PRM_S15_CASE_ACC_2;
                    default:
                        return BASKET_PRM_S15_CASE_ACC_0;
                }
            }
            virtual float GetParam_Dec() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return BASKET_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(BASKET_PRM_S15_CASE_DEC_RND_0);
                    case 1:
                        return BASKET_PRM_S15_CASE_DEC_1 + bcs::BCS_RandMax(BASKET_PRM_S15_CASE_DEC_RND_1);
                    case 2:
                        return BASKET_PRM_S15_CASE_DEC_2 + bcs::BCS_RandMax(BASKET_PRM_S15_CASE_DEC_RND_2);
                    default:
                        return BASKET_PRM_S15_CASE_DEC_0 + bcs::BCS_RandMax(BASKET_PRM_S15_CASE_DEC_RND_0);
                }
            }
            virtual float GetParam_VelLimit() const
            {
                switch (GetSubType_()) {
                    case 0:
                        return BASKET_PRM_S15_CASE_VEL_0;
                    case 1:
                        return BASKET_PRM_S15_CASE_VEL_1;
                    case 2:
                        return BASKET_PRM_S15_CASE_VEL_2;
                    default:
                        return BASKET_PRM_S15_CASE_VEL_0;
                }
            }
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_MaxLife() const
            {
                static const float lifeTbl[] = { 3.0f, 5.0f, 1.0f };
                // (guess) the table address is taken before the call in the original
                const float *life = lifeTbl;

                switch (GetSubType_()) {
                    case 0:
                        return life[0];
                    case 1:
                        return life[1];
                    case 2:
                        return life[2];
                    default:
                        return life[0];
                }
            }
            // Declared after GetParam_MaxLife: the original emits it before it
            // (weak inlines come out in reverse declaration order).
            // The interval is in seconds, returned in frames.
            virtual float GetParam_Interval() const { return 60.0f * BASKET_PRM_S15_CASE_INTERVAL; }
            virtual float GetParam_LocPosY() const { return BASKET_PRM_S15_CASE_POS_Y; }
            virtual float GetParam_KaronScale() const { return BASKET_PRM_S15_KARON_SCALE; }
            virtual float GetParam_KaronCollScale() const { return BASKET_PRM_S15_KARON_COLL_SCALE; }
            virtual float GetParam_KaronSpeed() const { return BASKET_PRM_S15_KARON_SPEED; }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBreak() { CreateKaron(); }
    };

    // One case, size 0x78 (nothing added to msw::CStgGmk_Case). Made by
    // msw::CStageBasket15::NewCase. All inline: game/basket/CStageBasket15.cpp
    // emits the vtable and, after CStgGmk_Case's weak functions, NewAct and
    // the destructor (reverse order of the declarations here).
    class CStgGmk_Case_Basket : public CStgGmk_Case {
        public:
            CStgGmk_Case_Basket(bcs::CTask *pParent) : CStgGmk_Case(pParent, 0) {}
            virtual ~CStgGmk_Case_Basket() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Case_Act_Basket(this); }
    };
}

#endif /* !MSW_CSTGGMK_CASE_BASKET_HPP_ */
