#ifndef MSW_CSTGGMK_PINBALL_ACT_HPP_
#define MSW_CSTGGMK_PINBALL_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CBall;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CStgGmk;

    // (purpose guessed from the names) The actor of a ball of stage 12
    // (msw::CStgGmk_Pinball). A DOL class, size 0x220: CStgGmkAct at 0x000
    // (vptrs at 0x008 and 0x014), its state queue bcs::CFsmQueue at 0x174
    // (vptr at 0x1D0), own members from 0x1D4. Constructor
    // __ct__Q23msw19CStgGmk_Pinball_ActFPQ23msw7CStgGmk. Each game module
    // derives its own class from it to supply the parameters.
    //
    // The order of the new virtual functions (IsAlive to
    // GetParam_BallKickPower) follows __vt__Q23msw26CStgGmk_Pinball_Act_Volley
    // (VolleyF), which emits weak copies of IsAlive, IsInvincible,
    // SetInvincible, SE_GetSlotNum, IsEnableDefColl and GetParam_LocOffsetZ:
    // those are inline here. Return types not visible in a body are
    // guesses.
    class CStgGmk_Pinball_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Pinball_Act> {
        public:
            CStgGmk_Pinball_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Pinball_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);

            virtual int IsAlive() const { return m_bAlive; }              // 0x1A4
            virtual void Inactivate();                                    // 0x1A8
            virtual void Sleep(float frame);                              // 0x1AC
            virtual void Respawn(float frame);                            // 0x1B0
            virtual void Standby();                                       // 0x1B4
            virtual long SE_GetSlotNum() const { return 1; }             // 0x1B8
            virtual void SE_PlayByParam(long type);                       // 0x1BC
            virtual void SE_PlayByParam(long type, const bcs::FVEC3 &rPos); // 0x1C0
            virtual float GetCurRadius() const;                           // 0x1C4
            virtual float GetParam_InitRadius() const;                    // 0x1C8
            virtual float GetParam_Radius() const;                        // 0x1CC
            virtual float GetParam_Height() const;                        // 0x1D0
            virtual float GetParam_CollRadius() const;                    // 0x1D4
            virtual float GetParam_CollHeight() const;                    // 0x1D8
            virtual float GetParam_AtkCollRadius() const;                 // 0x1DC
            virtual float GetParam_Dec() const;                           // 0x1E0
            virtual float GetParam_KickPower() const;                     // 0x1E4
            virtual float GetParam_Speed() const;                         // 0x1E8
            virtual float GetParam_VelLimit() const;                      // 0x1EC
            virtual float GetParam_Gravity() const;                       // 0x1F0
            virtual float GetParam_Interval() const;                      // 0x1F4
            virtual void GetParam_CourtRect(float *, float *, float *, float *) const; // 0x1F8
            virtual void GetParam_OuterRect(float *, float *, float *, float *) const; // 0x1FC
            virtual long GetParam_MdlIdx() const;                         // 0x200
            virtual long GetParam_Effect(long type) const;                // 0x204
            virtual long GetParam_SE(long type) const;                    // 0x208
            virtual float GetParam_MaxLife() const;                       // 0x20C
            virtual float GetParam_LocOffsetZ() const { return 0.0f; }   // 0x210
            virtual float GetParam_ForceAcc() const;                      // 0x214
            virtual long GetParam_EndLineEnabled() const;                 // 0x218
            virtual float GetParam_GuideWidth() const;                    // 0x21C
            virtual float GetParam_GuideDepth() const;                    // 0x220
            virtual long GetParam_GuideBoundEnabled() const;              // 0x224
            virtual float GetParam_GuideDecay() const;                    // 0x228
            virtual float GetParam_AtkHitEffectFrame() const;             // 0x22C
            virtual float GetParam_HitCameraQuake() const;                // 0x230
            virtual void UpdateColl();                                    // 0x234
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x238
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x23C
            virtual void SetInvincible(float time) { m_fInvincibleTime = time; } // 0x240
            virtual bool IsEnableDefColl() const { return false; }       // 0x244
            virtual float GetShotAngle() const;                           // 0x248
            virtual void WallHitCheck(bcs::FVEC3 *, bcs::FVEC3 *, const bcs::FVEC3 &, const bcs::FVEC3 &,
                                      const bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &); // 0x24C
            virtual void WallHitCheckSub(bcs::FVEC3 *, bcs::FVEC3 *, const bcs::FVEC3 &, const bcs::FVEC3 &, float,
                                         const bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &) const; // 0x250
            virtual void KickHitEffect(const bcs::FVEC3 &rPos);           // 0x254
            virtual void CtorState_DEFAULT();                             // 0x258
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
            virtual void CtorState_SLEEP();
            virtual void LoopState_SLEEP();
            virtual void DtorState_SLEEP();
            virtual void CtorState_RESPAWN();
            virtual void LoopState_RESPAWN();
            virtual void DtorState_RESPAWN();
            virtual void CtorState_INACTIVE();
            virtual void LoopState_INACTIVE();
            virtual void DtorState_INACTIVE();                            // 0x2A8
            virtual float GetParam_BallKickPower() const;                 // 0x2AC

        private:
            unsigned char m_Unk1D4[0x28];  // 0x1D4, not mapped
            int m_bAlive;                  // 0x1FC (IsAlive)
            unsigned char m_Unk200[0x10];  // 0x200, not mapped
            float m_fInvincibleTime;       // 0x210 (IsInvincible while above zero; name guessed)
            unsigned char m_Unk214[0x8];   // 0x214, not mapped

        protected:
            float m_fShotAngle;            // 0x21C (name guessed; read by VolleyF's GetShotAngle)
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_ACT_HPP_ */
