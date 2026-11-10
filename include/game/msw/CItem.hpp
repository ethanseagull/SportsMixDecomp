#ifndef MSW_CITEM_HPP_
#define MSW_CITEM_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CItemMan;
    class CChrCore;
    class CCollBodyUnit;
    class CCollAtkUnit;

    // INCOMPLETE: most data members are not mapped; return types of the
    // virtual functions are guesses unless a body is given.
    //
    // An item on the field (shell, banana, bomb, mushroom). A DOL class.
    // bcs::CUnivTask at 0x00, bcs::CFsm<CItem> at 0x34 (its vptr at 0x78),
    // own members from 0x7C. The smallest derived class (CItemBanana) is
    // 0x1AC bytes; the last offset any CItem function touches is 0x1A6, so
    // the size 0x1A8 here is a guess.
    //
    // The virtual functions are declared in the order of the slots of the
    // vtables the modules emit (HockeyF, __vt__Q23msw17CItemHockeyBanana):
    // Create is at 0x2C, after CFsm's part. Bodies given here are the ones
    // HockeyF emits as weak functions after the CItemHockeyBanana code, so
    // they are inline in the original.
    class CItem : public bcs::CUnivTask, public bcs::CFsm<CItem> {
        public:
            // Item kinds, as CItemMan::CreateItem and GetItemType use them.
            // Names follow the class each kind creates; 4 is not created by
            // the hockey game, 7 to 11 create a bomb too (their meaning is
            // not known).
            enum {
                KIND_GSHELL = 0,
                KIND_RSHELL = 1,
                KIND_TOADSTOOL = 2,
                KIND_BOMB = 3,
                KIND_BANANA = 5,
                KIND_MUSHROOM = 6
            };

            // The DOL's constructor has no exported name (fn_801278BC).
            CItem(CItemMan *pMan);
            virtual ~CItem();
            virtual void Main();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual void SetStock(CChrCore *pChr);
            virtual bool CanbePickup() const;
            virtual bool CheckPickup(const bcs::FVEC3 &pos, float, float);
            virtual bool CheckPickup(CCollBodyUnit *pBody);
            virtual long GetItemType() const;
            virtual long GetMotType() const { return 0; }
            virtual void DoExecute(CChrCore *pChr);
            virtual void DoAttach(bcs::CMdlObj *pMdl, unsigned long, float, int);
            virtual void DoDetach();
            virtual void GetThrowDir(const bcs::FVEC3 &, float, int, long) const;
            virtual void DoThrow(const bcs::FVEC3 &, unsigned long, long, long, int);
            virtual void DoParabolaThrow(const bcs::FVEC3 &, float, float);
            virtual void DoDrop(const bcs::FVEC3 &, int, int);
            virtual void DoPress();
            virtual void DoDestroy();
            virtual void SetSubParam(long) {}
            virtual void DoDrift(const bcs::FVEC3 &, float, float);
            virtual float GetBodyCollRadius() const;
            virtual float GetPickupRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetExplosionCollSize() const { return m_fExplosionCollSize; }
            virtual bool IsInfinityLife() const { return false; }
            virtual bool IsStateThrow() const;
            virtual float GetBombExplodeRestFrameDD() const { return 0.0f; }
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void CallbackBodyDamage_(CCollBodyUnit *, const CCollAtkUnit *, const bcs::FVEC3 &);
            virtual void ExParamUpdate_();
            virtual float GetParamDropLife() const { return m_fParamDropLife; }
            virtual float GetParamMainLife() const { return m_fParamMainLife; }
            virtual float GetParamGravity() const { return -0.025f; }
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual float GetParamScale() const { return m_fParamScale; }
            virtual long GetParamReboundNum() const { return m_nParamReboundNum; }
            virtual long GetParamThrowTrackType() const { return 0; }
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void DoTouchDown();

            // One Ctor/Loop/Dtor triple per state, in state order.
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

        protected:
            unsigned char m_Unk7C[0x14];   // 0x7C
            float m_fParamMainLife;        // 0x90 (name from GetParamMainLife)
            float m_fParamDropLife;        // 0x94 (name from GetParamDropLife)
            float m_fParamScale;           // 0x98 (name from GetParamScale)
            unsigned char m_Unk9C[0x08];   // 0x9C
            // 0xA4 to 0xC0 are set with the three above by the per-game
            // ExParamUpdate_ overrides; what they mean is not known.
            float m_fParamA4;              // 0xA4
            float m_fParamA8;              // 0xA8
            long m_nParamAC;               // 0xAC
            long m_nParamB0;               // 0xB0
            long m_nParamReboundNum;       // 0xB4 (name from GetParamReboundNum)
            float m_fParamB8;              // 0xB8
            float m_fParamBC;              // 0xBC
            float m_fParamC0;              // 0xC0
            float m_fExplosionCollSize;    // 0xC4 (name from GetExplosionCollSize)
            unsigned char m_UnkC8[0xE0];   // 0xC8
    };
}

#endif /* !MSW_CITEM_HPP_ */
