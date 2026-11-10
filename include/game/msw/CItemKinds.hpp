#ifndef MSW_CITEMKINDS_HPP_
#define MSW_CITEMKINDS_HPP_

#include "game/msw/CItem.hpp"

namespace msw {

    // INCOMPLETE: the item kinds of the DOL, one class per kind. Each game
    // module derives its own class from each of them (CItemHockey* in
    // HockeyF). Only the constructors, the destructors and the overrides
    // with an exported name are declared; the DOL's vtables also point at
    // unnamed overrides (GetItemType and others) that are not written yet.
    // (BasketF: the GetItemType bodies and the remaining overrides were
    // added from the vtables of its CItemBasket* classes; the modules emit
    // the inline GetItemType after each derived destructor.)
    // Sizes are from the allocations in CItemManHockey::CreateItem; the
    // extra members are not mapped.

    class CItemMushroom : public CItem {
        public:
            CItemMushroom(CItemMan *pMan);
            virtual ~CItemMushroom();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_MUSHROOM; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();

        private:
            unsigned char m_Unk1A8[0xC]; // 0x1A8, size 0x1B4
    };

    class CItemToadstool : public CItem {
        public:
            CItemToadstool(CItemMan *pMan);
            virtual ~CItemToadstool();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_TOADSTOOL; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();

        private:
            unsigned char m_Unk1A8[0xC]; // 0x1A8, size 0x1B4
    };

    class CItemBomb : public CItem {
        public:
            CItemBomb(CItemMan *pMan);
            virtual ~CItemBomb();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_BOMB; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateAttach();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();

            // New in this class; the per-game bombs override it. Which
            // values it returns is not known.
            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
            virtual void StartEffect_();
            virtual void StopEffect_();
            virtual void DoExplode_();

        private:
            unsigned char m_Unk1A8[0x18]; // 0x1A8, size 0x1C0
    };

    // What the green and the red shell share (a DOL class; overrides and
    // the new Create_ read from BasketF's __vt__Q23msw17CItemBasketGShell).
    // It adds no members the module code shows: the leaf classes keep the
    // padding up to their allocated sizes.
    class CItemShellCmn : public CItem {
        public:
            CItemShellCmn(CItemMan *pMan);
            virtual ~CItemShellCmn();

            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual long GetParamThrowTrackType() const { return 1; }
            virtual void CtorStateBreak();
            virtual void LoopStateBreak();

            virtual void Create_(long, const bcs::FVEC3 &pos);
    };

    class CItemGShell : public CItemShellCmn {
        public:
            CItemGShell(CItemMan *pMan);
            virtual ~CItemGShell();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_GSHELL; }
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void DtorStateThrow();

            virtual void GetThrowDir_(const bcs::FVEC3 &, float, int, long) const;

        private:
            unsigned char m_Unk1A8[0x10]; // 0x1A8, size 0x1B8
    };

    class CItemRShell : public CItemShellCmn {
        public:
            CItemRShell(CItemMan *pMan);
            virtual ~CItemRShell();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_RSHELL; }
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void DtorStateThrow();

            virtual void DoHoming_();
            virtual void SearchTarget_(const bcs::FVEC3 &, float, int, long);
            virtual void GetHomingTargetPos(bcs::FVEC3 &);

        private:
            unsigned char m_Unk1A8[0x24]; // 0x1A8, size 0x1CC
    };

    class CItemBanana : public CItem {
        public:
            CItemBanana(CItemMan *pMan);
            virtual ~CItemBanana();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual long GetItemType() const { return KIND_BANANA; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void CtorStateBreak();
            virtual void LoopStateBreak();

        private:
            unsigned char m_Unk1A8[0x4]; // 0x1A8, size 0x1AC
    };
}

#endif /* !MSW_CITEMKINDS_HPP_ */
