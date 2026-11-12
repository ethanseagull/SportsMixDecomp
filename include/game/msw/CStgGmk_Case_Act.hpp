#ifndef MSW_CSTGGMK_CASE_ACT_HPP_
#define MSW_CSTGGMK_CASE_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
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
    // Eff_GetSlotNum, SE_GetSlotNum), and so does BasketF's
    // game/basket/CStgGmk_Case_Act_Basket.cpp, so they are inline here.
    // Return types not visible in a body are guesses.
    class CStgGmk_Case_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Case_Act> {
        public:
            // The states of the queue, in the order of the state hooks.
            enum STATE {
                STATE_DEFAULT,  // placed at the gimmick's position, then MOVE
                STATE_STANDBY,  // waits, then MOVE
                STATE_MOVE,     // slides across the court
                STATE_BREAK,    // broken: hidden after GetParam_BreakTime frames
                STATE_RESPAWN,  // placed again with a new model, then MOVE
                STATE_INACTIVE, // hidden; respawns after the interval in its work
                STATE_OPEN      // (not used in the DOL)
            };

            CStgGmk_Case_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Case_Act();

            virtual void UpdateMdlMtx();
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual int IsAlive() const { return m_bAlive; }              // 0x1A4
            virtual void Inactivate();                                    // 0x1A8
            virtual void ForceBreak(float delay);                         // 0x1AC
            virtual float GetParam_Width() const;                         // 0x1B0
            virtual float GetParam_Depth() const;                         // 0x1B4
            virtual float GetParam_Height() const;                        // 0x1B8
            virtual float GetParam_MdlWidth() const { return 1.0f; }     // 0x1BC
            virtual float GetParam_MdlDepth() const { return 1.0f; }     // 0x1C0
            virtual float GetParam_MdlHeight() const { return 1.0f; }    // 0x1C4
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
            virtual float GetParam_LocOffsetZ() const { return 0.0f; }    // 0x1F4
            virtual float GetParam_LocPosY() const { return 10.0f; }     // 0x1F8
            virtual float GetParam_KaronScale() const { return 1.0f; }   // 0x1FC
            virtual float GetParam_KaronCollScale() const { return 0.5f * GetParam_KaronScale(); } // 0x200
            virtual float GetParam_KaronSpeed() const { return 0.4f; }   // 0x204
            virtual long GetParam_HitCameraQuake() const;                 // 0x208
            virtual float GetParam_BreakTime() const;                     // 0x20C
            virtual long GetParam_ItemHitMethod() const { return 1; }     // 0x210
            virtual long SE_GetSlotNum() const { return 1; }              // 0x214
            // Declared here, not with the other overrides: the original emits its weak
            // copy between IsInvincible and GetParam_ItemHitMethod (reverse declaration order).
            virtual long Eff_GetSlotNum() const { return 1; }
            virtual void UpdateColl();                                    // 0x218
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x21C
            virtual void SetAlive(int bAlive) { m_bAlive = bAlive; }      // 0x220
            // Takes one life (when the case has lives) with the hit effect
            // (bEffect) and sound (bSE) at rPos; out of lives it breaks.
            // False while invincible.
            virtual int DoDamage(const bcs::FVEC3 &rPos, int bEffect, int bSE); // 0x224
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall) {}  // 0x228
            virtual void OnBreak() {}                                     // 0x22C
            virtual bool IsEnableDefColl() const { return false; }        // 0x230
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

        protected:
            // 0x1D4: 0 to 3 the four walls, 4 the catch volume on top, 5 to
            // 7 three spheres along the case (names guessed).
            CBodyCollision m_BodyColl;
            CAtkCollision m_AtkColl;      // 0x1E0: three capsules along the case
            CDefCollision m_DefColl;      // 0x1EC

        private:
            // 0x1F8: frames of the state (STANDBY, BREAK) or time run in
            // INACTIVE (name guessed).
            float m_fStateTimer;
            int m_bAlive;                 // 0x1FC (IsAlive, SetAlive)
            // 0x200: set once the case is inside the court; until then it
            // is held within GetParam_OuterRect (name guessed).
            int m_bInCourt;
            float m_fLife;                // 0x204, hits left
            float m_fMaxLife;             // 0x208, GetParam_MaxLife at the spawn; not positive: no lives
            float m_fInvincibleTime;      // 0x20C (IsInvincible while above zero; name guessed)
            float m_fBreakTimer;          // 0x210, ForceBreak's delay; negative when none
    };
}

#endif /* !MSW_CSTGGMK_CASE_ACT_HPP_ */
