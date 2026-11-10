#ifndef MSW_CITEM_HPP_
#define MSW_CITEM_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CChrCore;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CItemMan;

    // An item on the court (shells, banana, bomb, mushrooms). A DOL class;
    // every game module derives its own items from the DOL's CItemGShell,
    // CItemRShell, ... (BossF: CItemBossGShell and so on).
    //
    // CFsm<CItem> is at 0x34, so its vptr is at 0x78; overrides of its
    // virtuals get @52@ thunks.
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw15CItemBossRShell (BossF), with the slot offset of the
    // first ones noted. Return types are not part of the mangled names: the
    // ones of the inline bodies come from the copies the modules emit, the
    // others are guesses, as are all member names.
    //
    // INCOMPLETE: only the members module code uses are named. Size 0x1AC:
    // CItemBanana is allocated with that size, and CItemMushroom's own
    // members start there.
    class CItem : public bcs::CUnivTask, public bcs::CFsm<CItem> {
        public:
            virtual ~CItem();
            virtual void Main();

            virtual void Create(const bcs::FVEC3 &rPos); // 0x2C
            virtual void SetStock(CChrCore *pChr);
            virtual bool CanbePickup() const;
            virtual bool CheckPickup(const bcs::FVEC3 &rPos, float, float);
            virtual bool CheckPickup(CCollBodyUnit *pBody);
            virtual long GetItemType() const = 0; // 0x40 (pure: guessed)
            virtual long GetMotType() const { return 0; }
            virtual void DoExecute(CChrCore *pChr);
            virtual void DoAttach(bcs::CMdlObj *pMdl, unsigned long, float, int);
            virtual void DoDetach();
            virtual float GetThrowDir(const bcs::FVEC3 &, float, int, long) const;
            virtual void DoThrow(const bcs::FVEC3 &, unsigned long, long, long, int);
            virtual void DoParabolaThrow(const bcs::FVEC3 &, float, float);
            virtual void DoDrop(const bcs::FVEC3 &, int, int);
            virtual void DoPress();
            virtual void DoDestroy();
            virtual void SetSubParam(long) {}
            virtual void DoDrift(const bcs::FVEC3 &, float, float);
            virtual float GetBodyCollRadius() const; // 0x74
            virtual float GetPickupRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetExplosionCollSize() const { return m_fExplosionCollSize; }
            virtual bool IsInfinityLife() const { return false; }
            virtual bool IsStateThrow() const;
            virtual float GetBombExplodeRestFrameDD() const { return 0.0f; }
            virtual bool CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual bool CallbackBodyDamage_(CCollBodyUnit *, const CCollAtkUnit *, const bcs::FVEC3 &);
            virtual void ExParamUpdate_(); // 0x98
            virtual float GetParamDropLife() const { return m_fParamDropLife; }
            virtual float GetParamMainLife() const { return m_fParamMainLife; }
            virtual float GetParamGravity() const { return -0.025f; }
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual float GetParamScale() const { return m_fParamScale; } // 0xB0
            virtual long GetParamReboundNum() const { return m_nParamReboundNum; }
            virtual long GetParamThrowTrackType() const { return 0; }
            virtual long GetParamThrowType() const { return 0; }
            virtual float GetParamThrowSearchDirRangeMax() const { return 0.0f; }
            virtual float GetParamThrowSearchDistRangeMax() const { return 0.0f; }
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void DoTouchDown();

            // One constructor, loop and destructor per state.
            virtual void CtorStateStock();
            virtual void LoopStateStock();
            virtual void DtorStateStock();
            virtual void CtorStatePopup();
            virtual void LoopStatePopup();
            virtual void DtorStatePopup();
            virtual void CtorStateDrift();
            virtual void LoopStateDrift();
            virtual void DtorStateDrift();
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void DtorStateDrop();
            virtual void CtorStateAttach();
            virtual void LoopStateAttach();
            virtual void DtorStateAttach();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void DtorStateThrow();
            virtual void CtorStateDropBreak();
            virtual void LoopStateDropBreak();
            virtual void DtorStateDropBreak();
            virtual void CtorStateBreak();
            virtual void LoopStateBreak();
            virtual void DtorStateBreak();
            virtual void CtorStatePress();
            virtual void LoopStatePress();
            virtual void DtorStatePress();
            virtual void CtorStateDelete();
            virtual void LoopStateDelete();
            virtual void DtorStateDelete();

            // Sets the attack type of attack collision `index`.
            void CollAtkSetAtkType(long index, unsigned long type);
            // The same setting for every attack collision of the item.
            void CollAtkSetTargetFlagAll(unsigned long flag);
            void CollAtkSetHitEffectFrameAll(float frame);
            void CollAtkSetHitDropCoinRateAll(float rate);
            void CollAtkSetHitDropCoinNumAll(long min, long max);
            // Turns every body collision of the item on or off.
            void CollBodySetEnableAll(int enable);

        protected:
            // The parameters from 0x90 to 0xC4 are what ExParamUpdate_
            // sets for each kind of item; the names of the ones no getter
            // returns come from what DoThrowInit passes them to.
            unsigned char m_Unk7C[0x90 - 0x7C];
            float m_fParamMainLife;         // 0x90
            float m_fParamDropLife;         // 0x94
            float m_fParamScale;            // 0x98
            float m_fParamThrowSpeed;       // 0x9C, per second (scaled by 1/60)
            unsigned char m_UnkA0[4];
            float m_fParamHitEffectFrame;   // 0xA4
            float m_fParamHitDropCoinRate;  // 0xA8
            long m_nParamHitDropCoinNumMin; // 0xAC
            long m_nParamHitDropCoinNumMax; // 0xB0
            long m_nParamReboundNum;        // 0xB4
            float m_fParamB8;               // 0xB8, (meaning unknown)
            float m_fParamBC;               // 0xBC, (meaning unknown)
            float m_fParamC0;               // 0xC0, (meaning unknown)
            float m_fExplosionCollSize;     // 0xC4
            unsigned char m_UnkC8[0xF8 - 0xC8];
            bcs::FVEC3 m_vPos;              // 0xF8
            unsigned char m_Unk104[0x11C - 0x104];
            bcs::FVEC3 m_vMove;             // 0x11C, copy of m_vVelocity at the throw (name guessed)
            bcs::FVEC3 m_vVelocity;         // 0x128
            unsigned char m_Unk134[0x15C - 0x134];
            float m_fGravity;               // 0x15C (name guessed)
            unsigned char m_Unk160[0x170 - 0x160];
            CAtkCollision m_AtkColl;        // 0x170
            CBodyCollision m_BodyColl;      // 0x17C
            unsigned char m_Unk188[0x1AA - 0x188];
            short m_nUnk1AA;                // 0x1AA, passed to SearchTarget_
    };
}

#endif /* !MSW_CITEM_HPP_ */
