#ifndef MSW_CSTGGMK_PYLON_ACT_HPP_
#define MSW_CSTGGMK_PYLON_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CBall;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CStgGmk;

    // (purpose guessed from the names) The actor of one pylon of stage 04
    // (msw::CStgGmk_Pylon). A DOL class, size 0x208: CStgGmkAct at 0x000
    // (vptrs at 0x008 and 0x014), its state queue bcs::CFsmQueue at 0x174
    // (vptr at 0x1D0), own members from 0x1D4. Constructor
    // __ct__Q23msw17CStgGmk_Pylon_ActFPQ23msw7CStgGmk. Each game module
    // derives its own class from it to supply the parameters.
    //
    // The order of the new virtual functions (IsAlive to SetPushVel)
    // follows __vt__Q23msw23CStgGmk_Pylon_Act_Dodge (DodgeF); they are
    // reached through the vptr at 0x008, after the CFsmQueue part.
    // SE_GetSlotNum overrides CStgSeSlot's, so it also gets a slot of its
    // own where it is declared. DodgeF's
    // game/dodge/CStgGmk_Pylon_Act_Dodge.cpp emits weak copies of the
    // functions with bodies here, in reverse order of these declarations
    // (so the overrides without a slot of their own are declared where that
    // order needs them). The DOL has no body for the parameters that every
    // module supplies, so they are pure. Return types not visible in a body
    // are guesses.
    class CStgGmk_Pylon_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Pylon_Act> {
        public:
            // The states of the queue, in the order of the state hooks.
            enum STATE {
                STATE_DEFAULT,
                STATE_STANDBY,
                STATE_MOVE,
                STATE_BREAK,
                STATE_RESPAWN,
                STATE_INACTIVE,
                STATE_FALL
            };

            CStgGmk_Pylon_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Pylon_Act();

            virtual void UpdateMdlMtx();
            virtual void OnUpdate(float rate);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual int IsAlive() const { return m_bAlive; }              // 0x1A4
            // Sleeps with no respawn time.
            virtual void Inactivate() { Sleep(-1.0f); }                   // 0x1A8
            virtual void Sleep(float frame);                              // 0x1AC
            virtual bool IsSleep() const;                                 // 0x1B0
            virtual float GetParam_MdlRadius() const = 0;                 // 0x1B4
            virtual float GetParam_MdlHeight() const = 0;                 // 0x1B8
            virtual float GetParam_Radius() const = 0;                    // 0x1BC
            virtual float GetParam_Height() const = 0;                    // 0x1C0
            virtual float GetParam_Height2() const;                       // 0x1C4
            virtual long GetParam_PushRank() const;                       // 0x1C8
            virtual long GetParam_PushPower() const = 0;                  // 0x1CC
            virtual float GetParam_PushResist() const = 0;                // 0x1D0
            virtual float GetParam_Acc() const = 0;                       // 0x1D4
            virtual float GetParam_Dec() const = 0;                       // 0x1D8
            virtual float GetParam_Resist() const;                        // 0x1DC
            virtual float GetParam_VelLimit() const;                      // 0x1E0
            virtual float GetParam_Gravity() const;                       // 0x1E4
            virtual float GetParam_Interval() const = 0;                  // 0x1E8
            virtual void GetParam_CourtRect(float *pLeft, float *pRight, float *pFront,
                                            float *pBack) const;          // 0x1EC
            virtual void GetParam_OuterRect(float *pLeft, float *pRight, float *pFront,
                                            float *pBack) const;          // 0x1F0
            virtual long GetParam_MdlIdx() const;                         // 0x1F4
            virtual long GetParam_Effect(long type) const = 0;            // 0x1F8
            virtual long GetParam_SE(long type) const;                    // 0x1FC
            virtual float GetParam_MaxLife() const = 0;                   // 0x200
            virtual float GetParam_LocOffsetZ() const { return 0.0f; }    // 0x204
            virtual float GetParam_FieldMargin() const = 0;               // 0x208
            virtual bool GetParam_IsEnableAreaCheck() const { return true; } // 0x20C
            // The turn per frame while falling over (3 degrees).
            virtual float GetParam_FallRot() const { return 0.05235988f; } // 0x210
            virtual float GetParam_LocRndX() const { return 0.0f; }       // 0x214
            virtual float GetParam_LocRndZ() const { return 0.0f; }       // 0x218
            virtual float GetParam_DragSpeed() const { return 0.7f; }     // 0x21C
            // Declared here, not with the other overrides: the original
            // emits its weak copy between GetParam_DragSpeed and IsInvincible.
            virtual long Eff_GetSlotNum() const { return 2; }
            virtual long SE_GetSlotNum() const { return 1; }              // 0x220
            virtual void ApplyLocRnd();                                   // 0x224
            virtual void UpdateColl();                                    // 0x228
            virtual bool CheckCollBg(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, const bcs::FVEC3 &,
                                     const bcs::FVEC3 &);                 // 0x22C
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x230
            virtual void DoDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x234
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x238
            virtual void OnLanding() {}                                   // 0x23C
            virtual bool IsEnableDefColl() const { return false; }        // 0x240
            virtual float GetLife() const { return m_fLife; }             // 0x244
            // One hit less, unless the pylon cannot be broken (a negative
            // maximum).
            virtual void DecLife()                                        // 0x248
            {
                if (GetMaxLife() >= 0.0f) {
                    m_fLife -= 1.0f;
                }
            }
            virtual float GetMaxLife() const { return m_fMaxLife; }       // 0x24C
            virtual void KickSplashEffect();                              // 0x250
            virtual void PlayPushSe();                                    // 0x254
            virtual void AutoPlayPushSe(const bcs::FVEC3 &rVel);          // 0x258
            virtual void UpdatePushSe(float rate);                        // 0x25C
            virtual void CtorState_DEFAULT();                             // 0x260
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
            virtual void CtorState_FALL();
            virtual void LoopState_FALL();
            virtual void DtorState_FALL();                                // 0x2B0
            // Sets the pylon sliding along rVel (name from the symbol).
            virtual void SetPushVel(const bcs::FVEC3 &rVel);              // 0x2B4

        private:
            CBodyCollision m_BodyColl;    // 0x1D4 (name guessed)
            CDefCollision m_DefColl;      // 0x1E0 (name guessed)
            float m_Unk1EC;               // 0x1EC
            int m_bAlive;                 // 0x1F0 (IsAlive)
            unsigned long m_Unk1F4;       // 0x1F4
            float m_fLife;                // 0x1F8 (GetLife, DecLife)
            float m_fMaxLife;             // 0x1FC (GetMaxLife)
            float m_fInvincibleTime;      // 0x200 (IsInvincible while above zero; name guessed)
            float m_Unk204;               // 0x204
    };
}

#endif /* !MSW_CSTGGMK_PYLON_ACT_HPP_ */
