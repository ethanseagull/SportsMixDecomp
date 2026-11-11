#ifndef MSW_CSTGGMK_CASE_ACT_HPP_
#define MSW_CSTGGMK_CASE_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CBall;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CStgGmk;

    // (purpose guessed from the names) The actor of one case of the ship
    // stage (stage 15). A DOL class, size 0x214: CStgGmkAct at 0x000 (vptrs
    // at 0x008 and 0x014), its state queue bcs::CFsmQueue at 0x174 (vptr at
    // 0x1D0), own members from 0x1D4. Constructor
    // __ct__Q23msw16CStgGmk_Case_ActFPQ23msw7CStgGmk. Each game module
    // derives its own class from it to supply the parameters.
    //
    // The order of the new virtual functions (IsAlive to DtorState_OPEN)
    // follows __vt__Q23msw22CStgGmk_Case_Act_Dodge (DodgeF); they are reached
    // through the vptr at 0x008, after the CFsmQueue part. SE_GetSlotNum
    // overrides CStgSeSlot's, so it also gets a slot of its own where it is
    // declared. DodgeF's game/dodge/CStgGmk_Case_Act_Dodge.cpp emits weak
    // copies of some of these (IsAlive, SetAlive, IsInvincible,
    // IsEnableDefColl, GetParam_LocOffsetZ, GetParam_ItemHitMethod,
    // Eff_GetSlotNum, SE_GetSlotNum); their bodies are not written yet. Return
    // types not visible in a body are guesses.
    class CStgGmk_Case_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Case_Act> {
        public:
            CStgGmk_Case_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Case_Act();

            virtual long Eff_GetSlotNum() const;
            virtual void UpdateMdlMtx();
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual int IsAlive() const;                                  // 0x1A4
            virtual void Inactivate();                                    // 0x1A8
            virtual void ForceBreak(float delay);                         // 0x1AC
            virtual float GetParam_Width() const;                         // 0x1B0
            virtual float GetParam_Depth() const;                         // 0x1B4
            virtual float GetParam_Height() const;                        // 0x1B8
            virtual float GetParam_MdlWidth() const;                      // 0x1BC
            virtual float GetParam_MdlDepth() const;                      // 0x1C0
            virtual float GetParam_MdlHeight() const;                     // 0x1C4
            virtual float GetParam_Acc() const;                           // 0x1C8
            virtual float GetParam_Dec() const;                           // 0x1CC
            virtual float GetParam_VelLimit() const;                      // 0x1D0
            virtual float GetParam_Gravity() const;                       // 0x1D4
            virtual float GetParam_Interval() const;                      // 0x1D8
            virtual void GetParam_CourtRect(float *pLeft, float *pRight, float *pFront,
                                            float *pBack) const;          // 0x1DC
            virtual void GetParam_OuterRect(float *pLeft, float *pRight, float *pFront,
                                            float *pBack) const;          // 0x1E0
            virtual long GetParam_MdlIdx() const;                         // 0x1E4
            virtual long GetParam_Effect(long type) const;                // 0x1E8
            virtual long GetParam_SE(long type) const;                    // 0x1EC
            virtual float GetParam_MaxLife() const;                       // 0x1F0
            virtual float GetParam_LocOffsetZ() const;                    // 0x1F4
            virtual float GetParam_LocPosY() const;                       // 0x1F8
            virtual float GetParam_KaronScale() const;                    // 0x1FC
            virtual float GetParam_KaronCollScale() const;                // 0x200
            virtual float GetParam_KaronSpeed() const;                    // 0x204
            virtual float GetParam_HitCameraQuake() const;                // 0x208
            virtual float GetParam_BreakTime() const;                     // 0x20C
            virtual long GetParam_ItemHitMethod() const;                  // 0x210
            virtual long SE_GetSlotNum() const;                           // 0x214
            virtual void UpdateColl();                                    // 0x218
            virtual bool IsInvincible() const;                            // 0x21C
            virtual void SetAlive(int bAlive);                            // 0x220
            virtual void DoDamage(const bcs::FVEC3 &rPos, int, int);      // 0x224
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x228
            virtual void OnBreak();                                       // 0x22C
            virtual bool IsEnableDefColl() const;                         // 0x230
            virtual void CreateKaron();                                   // 0x234
            virtual long GetSubType_() const;                             // 0x238
            virtual void CtorState_DEFAULT();                             // 0x23C
            virtual void LoopState_DEFAULT();
            virtual void DtorState_DEFAULT();
            virtual void CtorState_STANDBY();
            virtual void LoopState_STANDBY();
            virtual void DtorState_STANDBY();
            virtual void CtorState_MOVE();
            virtual void LoopState_MOVE();
            virtual void DtorState_MOVE();
            virtual void CtorState_BREAK();
            virtual void LoopState_BREAK();
            virtual void DtorState_BREAK();
            virtual void CtorState_RESPAWN();
            virtual void LoopState_RESPAWN();
            virtual void DtorState_RESPAWN();
            virtual void CtorState_INACTIVE();
            virtual void LoopState_INACTIVE();
            virtual void DtorState_INACTIVE();
            virtual void CtorState_OPEN();
            virtual void LoopState_OPEN();
            virtual void DtorState_OPEN();                                // 0x28C

        private:
            unsigned char m_Unk1D4[0x40]; // 0x1D4, not mapped
    };
}

#endif /* !MSW_CSTGGMK_CASE_ACT_HPP_ */
