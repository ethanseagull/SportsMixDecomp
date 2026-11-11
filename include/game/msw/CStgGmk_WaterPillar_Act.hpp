#ifndef MSW_CSTGGMK_WATERPILLAR_ACT_HPP_
#define MSW_CSTGGMK_WATERPILLAR_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CStgGmk;

    // The actor of one water pillar. A DOL class, size 0x210: CStgGmkAct at
    // 0x000 (vptrs at 0x008 and 0x014), its state queue bcs::CFsmQueue at
    // 0x174 (vptr at 0x1D0), own members from 0x1D4 (three collision
    // objects among them; the constructor calls CBodyCollision's,
    // CAtkCollision's and CDefCollision's). Constructor
    // __ct__Q23msw23CStgGmk_WaterPillar_ActFPQ23msw7CStgGmk.
    //
    // States (the numbers are the state table's; names of the states
    // without a symbol are guesses): 0 RESET (back to the default position,
    // then STOP), 1 WAIT (Interval frames, then PRESTART while automatic),
    // 2 PRESTART (warning), 3 START (the water stands for StayTime frames),
    // 4 STAY (not entered by the DOL's code), 5 END (the water falls), 6
    // STOP (off).
    //
    // The order of the new virtual functions (GetParam_Radius to
    // DtorState_START) follows __vt__Q23msw29CStgGmk_WaterWorks_Hockey_Act
    // (HockeyF); they are reached through the vptr at 0x008, after the
    // CFsmQueue part. SE_GetSlotNum overrides CStgSeSlot's, so it also gets
    // a slot of its own where it is declared. The modules emit weak copies
    // of the functions with bodies here; the GetParam_* functions a module
    // class overrides have no DOL default with a name, so they are declared
    // pure. Return types not visible in a body are guesses.
    class CStgGmk_WaterPillar_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_WaterPillar_Act> {
        public:
            CStgGmk_WaterPillar_Act(CStgGmk *pParent);
            virtual ~CStgGmk_WaterPillar_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual unsigned long OnCalcSum() const;

            virtual float GetParam_Radius() const = 0;
            virtual float GetParam_Radius2() const = 0;
            virtual float GetParam_Radius3() const = 0;
            virtual float GetParam_Height() const = 0;
            virtual float GetParam_PrestartTime() const = 0;
            virtual float GetParam_WaitTime() const = 0;
            virtual float GetParam_StayTime() const = 0;
            virtual float GetParam_Interval() const = 0;
            virtual long GetParam_Effect(long type) const = 0;
            virtual float GetParam_MoveResist() const = 0;
            virtual long GetParam_SE(long type) const { return -1; }
            virtual void ForceStart();
            virtual void ForceStop();
            virtual bool IsSwoosh() const;
            virtual bool IsActive() const;
            virtual void ForceReset();
            virtual void SetAuto(int bAuto) { m_bAuto = bAuto; }
            virtual int IsAuto() const { return m_bAuto; }
            // An override declared among the new functions: the modules
            // emit it just before IsAuto (inline functions come out in
            // reverse order of declaration).
            virtual long Eff_GetSlotNum() const { return 7; }
            virtual bool IsWarning() const;
            virtual void UpdateColl();
            virtual void AnimPlay(long mdl, unsigned long anm, float speed, float frame, float blend,
                                  unsigned long flag);
            virtual bool IsAnimEnd();
            virtual float AnimGetFrameNum();
            virtual void SetVisibleAll(int bVisible);
            virtual void WaterSuspend();
            virtual void WaterResume();
            virtual int IsWaterSuspend() const;
            virtual bool CheckWaterCap() const;
            virtual void WaterStartAnim_();
            virtual void WaterEndAnim_();
            virtual void Eff_KickHit(const bcs::FVEC3 &rPos);
            virtual long SE_GetSlotNum() const { return 1; }
            virtual void CtorState_PRESTART();
            virtual void LoopState_PRESTART();
            virtual void DtorState_PRESTART();
            virtual void CtorState_START();
            virtual void LoopState_START();
            virtual void DtorState_START();

        private:
            // The states without a symbol in the DOL (names guessed).
            void CtorState_RESET();
            void LoopState_RESET();
            void DtorState_RESET();
            void CtorState_WAIT();
            void LoopState_WAIT();
            void DtorState_WAIT();
            void CtorState_STAY();
            void LoopState_STAY();
            void DtorState_STAY();
            void CtorState_END();
            void LoopState_END();
            void DtorState_END();
            void CtorState_STOP();
            void LoopState_STOP();
            void DtorState_STOP();

        protected:
            CBodyCollision m_BodyColl;    // 0x1D4
            CAtkCollision m_AtkColl;      // 0x1E0
            CDefCollision m_DefColl;      // 0x1EC
            float m_fTimer;               // 0x1F8, frames in the state (counting up or down)
            float m_fCollTimer;           // 0x1FC, frames before the water collides
            int m_bCollEnable;            // 0x200, see UpdateColl
            int m_bSuspend;               // 0x204, see WaterSuspend
            int m_bAuto;                  // 0x208
            unsigned long m_nTopNode;     // 0x20C, index of the model's top node (OnInit)
    };
}

#endif /* !MSW_CSTGGMK_WATERPILLAR_ACT_HPP_ */
