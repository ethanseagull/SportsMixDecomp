#ifndef MSW_CSTGGMK_TROLLEY_ACT_HPP_
#define MSW_CSTGGMK_TROLLEY_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmkAct.hpp"
#include "game/msw/CStgMdl.hpp"
#include "game/msw/CStgSeSlot.hpp"

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

            enum {
                HEIGHT_TYPE_TBL_NUM = 10, // height patterns
                HEIGHT_TYPE_CAR_NUM = 20  // cars per pattern
            };

            // (name guessed) Gives the sound slots the trolley's centre
            // (GetCenterPos). A class without an exported name, vtable
            // lbl_80489928; declared in the actor, before CRailMdl, for the
            // order of the vtables in .data.
            class CSePosFunc : public CStgSeSlot::CSESlot::CPosFunc {
                public:
                    CSePosFunc(const CStgGmk_Trolley_Act *pAct) : m_pAct(pAct) {}
                    virtual ~CSePosFunc() {}
                    virtual void GetPos(bcs::FVEC3 *pPos) { *pPos = m_pAct->GetCenterPos(); }

                private:
                    const CStgGmk_Trolley_Act *m_pAct; // 0x04
            };

            // (name guessed) The rails the trolley runs on: a model whose
            // animation moves a node (GetParam_RailNodeName) along the
            // track, so GetMtx can place a car at a distance along it. A
            // class without exported names, size 0x134, vtable
            // lbl_804897E8. The inline functions are emitted after their
            // first caller.
            class CRailMdl : public CStgMdl {
                public:
                    CRailMdl(CStgObj *pParent) : CStgMdl(pParent), m_nNodeIdx(-1), m_fLength(0.0f) {}
                    virtual ~CRailMdl();
                    virtual void Create();

                    virtual void SetNodeIdx(long idx) { m_nNodeIdx = idx; }              // 0x124
                    virtual long GetNodeIdx() const { return m_nNodeIdx; }               // 0x128
                    virtual void SetNode(const char *pName);                            // 0x12C
                    // The length of the track the animation covers.
                    virtual void SetLength(float len) { m_fLength = len; }              // 0x130
                    virtual float GetLength() const { return m_fLength; }               // 0x134
                    virtual bool IsValidNode() const { return m_nNodeIdx >= 0; }        // 0x138
                    virtual bool GetMtx(bcs::FMTX34 &rMtx, float dist, float rotY);     // 0x13C

                private:
                    long m_nNodeIdx; // 0x12C, the node that runs along the track; -1 for none
                    float m_fLength; // 0x130
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
            virtual long GetParam_HitCameraQuake() const;                 // 0x21C
            virtual long GetParam_TrolleyPatType() const { return 0; }    // 0x220
            virtual long GetParam_TrolleyPatNo() const { return 0; }      // 0x224
            virtual long GetParam_TrolleyPatValidNum() const { return 1; } // 0x228
            virtual float GetParam_CoinModeRate() const { return 0.2f; }  // 0x22C
            virtual float GetParam_ThrowDistMin(long type) const { return 1.0f; } // 0x230
            virtual float GetParam_ThrowDistMax(long type) const { return 4.0f; } // 0x234
            virtual long SE_GetSlotNum() const { return 1; }             // 0x238
            virtual long GetDispMdlNum() const { return m_nDispMdlNum; } // 0x23C
            virtual void DoSpawn();                                       // 0x240
            virtual void DoInactivate();                                  // 0x244
            virtual void InitColl();                                      // 0x248
            virtual void InitDefColl();                                   // 0x24C
            virtual void UpdateColl();                                    // 0x250
            virtual void UpdateDefColl(const bcs::FVEC3 &rPos, float, float, float, float); // 0x254
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x258
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall) {}  // 0x25C
            virtual bool IsEnableCatchColl() const { return true; }       // 0x260 (HockeyF emits the weak copy)
            virtual bool IsEnableDefColl() const { return false; }        // 0x264
            virtual void RebuildMdl();                                    // 0x268
            virtual void BuildHeightTypeTbl();                            // 0x26C
            virtual void SetNextHeightType();                             // 0x270
            virtual long GetHeightTypeNum() const;                        // 0x274
            virtual long GetHeightType(long idx) const;                   // 0x278
            virtual long GetLocRawTblIdx(long, long) const { return 0; }  // 0x27C
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
            CSePosFunc m_SePosFunc;        // 0x1D4, where the rolling sound plays (GetCenterPos)
        protected:
            CBodyCollision m_BodyColl;     // 0x1DC: 0 and 1 the cars, 2 the catch volume (name guessed)
            CAtkCollision m_AtkColl;       // 0x1E8: two capsules along the cars
            CDefCollision m_DefColl;       // 0x1F4
        private:
            float m_fStateTimer;           // 0x200, frames left in STANDBY and INACTIVE
            int m_bAlive;                  // 0x204 (IsAlive)
            int m_bQuake;                  // 0x208, the camera quakes while the train is on the ground
            float m_fLife;                 // 0x20C, catches left
            float m_fMaxLife;              // 0x210, GetParam_MaxLife at the spawn; not positive: cannot break
            float m_fInvincibleTime;       // 0x214 (IsInvincible while above zero; name guessed)
            float m_fRunDist;              // 0x218, distance run since the spawn
            float m_fRunDistMax;           // 0x21C, GetParam_RunDist when the run starts
            bcs::FVEC3 m_SpawnPos;         // 0x220
            long m_nDispMdlNum;            // 0x22C (GetDispMdlNum), cars in the train
            CRailMdl *m_pRailMdl;          // 0x230
            // 0x234: the car types of each height pattern (GetHeightType).
            long m_aHeightTypeTbl[HEIGHT_TYPE_TBL_NUM][HEIGHT_TYPE_CAR_NUM];
            long m_nHeightTypeNum;         // 0x554 (GetHeightTypeNum)
            long m_nHeightType;            // 0x558, the pattern in use
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_ACT_HPP_ */
