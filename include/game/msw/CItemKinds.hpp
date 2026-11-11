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
    // (BossF: the boss items' members and the return types of SearchTarget_
    // and GetThrowDir_ were added from game/boss/CItemManBoss.cpp.)
    // Sizes are from the allocations in CItemManHockey::CreateItem; CItem
    // itself is 0x1AC bytes.

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

        protected:
            float m_fUnk1AC; // 0x1AC
            float m_fUnk1B0; // 0x1B0, size 0x1B4
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

        protected:
            float m_fUnk1AC; // 0x1AC
            float m_fUnk1B0; // 0x1B0, size 0x1B4
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

        protected:
            // (names guessed from DodgeF's CItemDodgeBombDD_Base, which uses
            // them the way the DOL bomb does)
            float m_fExplodeRestFrame; // 0x1AC, frames until the bomb blows up
            float m_fFlashPhase;       // 0x1B0, phase of the red flashing before the blast
            long m_bEffect;            // 0x1B4, set while StartEffect_'s effect runs
            unsigned long m_hEffect;   // 0x1B8
            long m_bExplode;           // 0x1BC, the bomb blows up when the countdown is over; size 0x1C0
    };

    // What the green and the red shell share (a DOL class; overrides and
    // the new Create_ read from BasketF's __vt__Q23msw17CItemBasketGShell).
    // Its members end at 0x1B8, the size of the green shell.
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

        protected:
            unsigned char m_Unk1AC[0x1B4 - 0x1AC];
            float m_fThrowTime; // 0x1B4, (name guessed) set by the boss shells' DoThrowInit
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

            virtual float GetThrowDir_(const bcs::FVEC3 &, float, int, long) const;
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
            virtual long SearchTarget_(const bcs::FVEC3 &rPos, float fDir, int, long);
            virtual void GetHomingTargetPos(bcs::FVEC3 &rPos);

        protected:
            unsigned char m_Unk1B8[0x1C0 - 0x1B8];
            long m_nTarget;    // 0x1C0, what SearchTarget_ returned
            unsigned char m_Unk1C4[4];
            long m_nUnk1C8;    // 0x1C8, (meaning unknown) tested by the boss DoThrowInit; size 0x1CC
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
    };
}

#endif /* !MSW_CITEMKINDS_HPP_ */
