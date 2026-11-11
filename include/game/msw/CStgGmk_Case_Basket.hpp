#ifndef MSW_CSTGGMK_CASE_BASKET_HPP_
#define MSW_CSTGGMK_CASE_BASKET_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"

namespace msw {

    // The cases of the ship stage (stage 15) in the basketball mode.

    // The actor, size 0x214 (nothing added to msw::CStgGmk_Case_Act), with
    // the BASKET_PRM_S15_* parameters. Its functions are in
    // game/basket/CStgGmk_Case_Act_Basket.cpp (not written yet; unlike the
    // dodgeball actor it keeps CStgGmk_Case_Act::GetParam_OuterRect); only the
    // constructor is inline (msw::CStgGmk_Case_Basket::NewAct).
    class CStgGmk_Case_Act_Basket : public CStgGmk_Case_Act {
        public:
            CStgGmk_Case_Act_Basket(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Case_Act_Basket();

            virtual float GetParam_Width() const;
            virtual float GetParam_Depth() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlDepth() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_Acc() const;
            virtual float GetParam_Dec() const;
            virtual float GetParam_VelLimit() const;
            virtual float GetParam_Interval() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_MaxLife() const;
            virtual float GetParam_LocPosY() const;
            virtual float GetParam_KaronScale() const;
            virtual float GetParam_KaronCollScale() const;
            virtual float GetParam_KaronSpeed() const;
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBreak();
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
