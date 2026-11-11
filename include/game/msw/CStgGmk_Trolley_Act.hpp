#ifndef MSW_CSTGGMK_TROLLEY_ACT_HPP_
#define MSW_CSTGGMK_TROLLEY_ACT_HPP_

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

    // (purpose guessed from the names) The actor of the trolley of stage 17
    // (msw::CStgGmk_Trolley). A DOL class, size 0x55C: CStgGmkAct at 0x000
    // (vptrs at 0x008 and 0x014), its state queue bcs::CFsmQueue at 0x174
    // (vptr at 0x1D0), own members from 0x1D4. Constructor
    // __ct__Q23msw19CStgGmk_Trolley_ActFPQ23msw7CStgGmk. Each game module
    // derives its own class from it to supply the parameters.
    //
    // The order of the new virtual functions (IsAlive to DtorState_INACTIVE)
    // follows __vt__Q23msw25CStgGmk_Trolley_Act_Dodge (DodgeF); they are
    // reached through the vptr at 0x008. SE_GetSlotNum overrides
    // CStgSeSlot's, so it also gets a slot of its own where it is declared.
    // DodgeF's game/dodge/CStgGmk_Trolley_Act_Dodge.cpp emits weak copies of
    // some of these (SE_GetSlotNum, IsInvincible, GetDispMdlNum,
    // Eff_GetSlotNum, GetParam_RailNodeName, GetParam_RailLength,
    // GetParam_IsRailMotRelate, GetParam_DefHeight, GetParam_DefWidth,
    // IsAlive); BasketF's game/basket/CStgGmk_Trolley_Act_Basket.cpp too,
    // so they are inline here. Return types not visible in a body are
    // guesses.
    class CStgGmk_Trolley_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Trolley_Act> {
        public:
            // The states of the queue, in the order of the state hooks.
            enum STATE {
                STATE_DEFAULT,
                STATE_STANDBY,
                STATE_MOVE,
                STATE_BREAK,
                STATE_RESPAWN,
                STATE_INACTIVE
            };

            CStgGmk_Trolley_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Trolley_Act();

            virtual long Eff_GetSlotNum() const { return 1; }
            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual int IsAlive() const { return m_bAlive; }            // 0x1A4
            virtual bcs::FVEC3 GetCenterPos() const;                      // 0x1A8
            virtual bcs::FVEC3 GetVirtualPos() const;                     // 0x1AC
            virtual float GetAtkTotalWidth() const;                       // 0x1B0
            virtual float GetAtkTotalDepth() const;                       // 0x1B4
            virtual float GetBodyTotalWidth() const;                      // 0x1B8
            virtual float GetBodyTotalDepth() const;                      // 0x1BC
            virtual float GetParam_MdlWidth() const;                      // 0x1C0
            virtual float GetParam_MdlDepth() const;                      // 0x1C4
            virtual float GetParam_MdlHeight() const;                     // 0x1C8
            virtual float GetParam_Width() const;                         // 0x1CC
            virtual float GetParam_Depth() const;                         // 0x1D0
            virtual float GetParam_Height(long type) const;               // 0x1D4
            virtual float GetParam_AtkWidth() const;                      // 0x1D8
            virtual float GetParam_AtkDepth() const;                      // 0x1DC
            virtual float GetParam_AtkHeight() const;                     // 0x1E0
            virtual float GetParam_DefWidth() const { return GetParam_Width(); } // 0x1E4
            virtual float GetParam_DefHeight(long type) const { return GetParam_Height(0); } // 0x1E8
            virtual float GetParam_VelLimit() const;                      // 0x1EC
            virtual float GetParam_Gravity() const;                       // 0x1F0
            virtual long GetParam_MdlMinNum() const;                      // 0x1F4
            virtual long GetParam_MdlMaxNum() const;                      // 0x1F8
            virtual long GetParam_Effect(long type) const;                // 0x1FC
            virtual long GetParam_SE(long type) const;                    // 0x200
            virtual float GetParam_MaxLife() const;                       // 0x204
            virtual float GetParam_LocRotY() const;                       // 0x208
            virtual float GetParam_RunDist() const;                       // 0x20C
            virtual bool GetParam_IsRailMotRelate() const { return true; } // 0x210
            virtual float GetParam_RailLength() const { return 180.0f; }  // 0x214
            virtual const char *GetParam_RailNodeName() const { return "train_Line_00"; } // 0x218
            virtual float GetParam_HitCameraQuake() const;                // 0x21C
            virtual long GetParam_TrolleyPatType() const;                 // 0x220
            virtual long GetParam_TrolleyPatNo() const;                   // 0x224
            virtual long GetParam_TrolleyPatValidNum() const;             // 0x228
            virtual float GetParam_CoinModeRate() const;                  // 0x22C
            virtual float GetParam_ThrowDistMin(long type) const;         // 0x230
            virtual float GetParam_ThrowDistMax(long type) const;         // 0x234
            virtual long SE_GetSlotNum() const { return 1; }             // 0x238
            virtual long GetDispMdlNum() const { return m_nDispMdlNum; } // 0x23C
            virtual void DoSpawn();                                       // 0x240
            virtual void DoInactivate();                                  // 0x244
            virtual void InitColl();                                      // 0x248
            virtual void InitDefColl();                                   // 0x24C
            virtual void UpdateColl();                                    // 0x250
            virtual void UpdateDefColl(const bcs::FVEC3 &rPos, float, float, float, float); // 0x254
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x258
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x25C
            virtual bool IsEnableCatchColl() const { return true; }       // 0x260 (HockeyF emits the weak copy)
            virtual bool IsEnableDefColl() const;                         // 0x264
            virtual void RebuildMdl();                                    // 0x268
            virtual void BuildHeightTypeTbl();                            // 0x26C
            virtual void SetNextHeightType();                             // 0x270
            virtual long GetHeightTypeNum() const;                        // 0x274
            virtual long GetHeightType(long idx) const;                   // 0x278
            virtual long GetLocRawTblIdx(long, long) const;               // 0x27C
            virtual void CtorState_DEFAULT();                             // 0x280
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
            virtual void DtorState_INACTIVE();                            // 0x2C4

        private:
            unsigned char m_Unk1D4[0x8];   // 0x1D4, not mapped
        protected:
            CBodyCollision m_BodyColl;     // 0x1DC (name guessed)
            CAtkCollision m_AtkColl;       // 0x1E8 (name guessed)
            CDefCollision m_DefColl;       // 0x1F4 (name guessed; VolleyF's InitDefColl/UpdateDefColl)
        private:
            unsigned char m_Unk200[4];     // 0x200, not mapped
            int m_bAlive;                  // 0x204 (IsAlive)
            unsigned char m_Unk208[0xC];   // 0x208, not mapped
            float m_fInvincibleTime;       // 0x214 (IsInvincible while above zero; name guessed)
            unsigned char m_Unk218[0x14];  // 0x218, not mapped
            long m_nDispMdlNum;            // 0x22C (GetDispMdlNum)
            unsigned char m_Unk230[0x32C]; // 0x230, not mapped
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_ACT_HPP_ */
