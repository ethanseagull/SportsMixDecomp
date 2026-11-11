#ifndef MSW_CSTGGMK_FIREPILLAR_HOCKEY_HPP_
#define MSW_CSTGGMK_FIREPILLAR_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "game/msw/CStgGmk_WaterPillar_Act.hpp"

namespace msw {

    // The fire pillars of hockey party stage 86 (HockeyF): water pillars
    // (msw::CStgGmk_WaterPillar) that burn instead, with the
    // HOCKEY_PRM_S86_FIRE* parameters. The actor, the driver and the
    // gimmick live in game/hockey/CStgGmk_FirePillar_Hockey_Act.cpp; the
    // classes are declared in the reverse order of their vtables in that
    // file's .data. The cluster that makes the pillars is elsewhere.

    // The actor, size 0x210 (nothing added to msw::CStgGmk_WaterPillar_Act).
    // A burning pillar hurts what it touches through the attack collision
    // the water pillar does not use. The overrides are declared in the
    // order of their vtable slots.
    class CStgGmk_FirePillar_Hockey_Act : public CStgGmk_WaterPillar_Act {
        public:
            CStgGmk_FirePillar_Hockey_Act(CStgGmk *pParent) : CStgGmk_WaterPillar_Act(pParent) {}
            virtual ~CStgGmk_FirePillar_Hockey_Act();

            virtual void OnInit();
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual float GetParam_Radius() const;
            virtual float GetParam_Radius2() const;
            virtual float GetParam_Radius3() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_PrestartTime() const;
            virtual float GetParam_WaitTime() const;
            virtual float GetParam_StayTime() const;
            virtual float GetParam_Interval() const;
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_MoveResist() const;
            virtual long GetParam_SE(long type) const;
            virtual void UpdateColl();
            // Fire is never capped.
            virtual bool CheckWaterCap() const { return false; }
            virtual void WaterStartAnim_();
            virtual void CtorState_PRESTART();
            virtual void DtorState_PRESTART();
            virtual void CtorState_START();
    };

    // The driver, size 0xC (nothing added to msw::CStgGmk_WaterPillar_Drv).
    class CStgGmk_FirePillar_Hockey_Drv : public CStgGmk_WaterPillar_Drv {
        public:
            CStgGmk_FirePillar_Hockey_Drv(CStgGmk *pParent) : CStgGmk_WaterPillar_Drv(pParent) {}
            virtual ~CStgGmk_FirePillar_Hockey_Drv();

            virtual void Main(float rate);
    };

    // One pillar, size 0x74 (nothing added to msw::CStgGmk_WaterPillar). It
    // has no model. GetRadius is inline (HockeyF keeps the weak copy that
    // game/hockey/CComManHockey.cpp emits).
    class CStgGmk_FirePillar_Hockey : public CStgGmk_WaterPillar {
        public:
            CStgGmk_FirePillar_Hockey(bcs::CTask *pParent, int arg);
            virtual ~CStgGmk_FirePillar_Hockey();

            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);
            virtual float GetRadius() const { return GetPillarAct()->GetParam_Radius(); }
    };
}

#endif /* !MSW_CSTGGMK_FIREPILLAR_HOCKEY_HPP_ */
