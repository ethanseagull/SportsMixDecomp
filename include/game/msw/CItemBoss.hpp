#ifndef MSW_CITEMBOSS_HPP_
#define MSW_CITEMBOSS_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // The items of the boss game (BossF). They add no members; they set
    // their own parameters and change how the item is thrown. The overrides
    // are the slots of their vtables that name them. The destructors are
    // empty; they are declared explicitly so that BossF emits each as a weak
    // function at its original place in the file (implicit ones all come
    // out at the very end).
    //
    // All are defined in game/boss/CItemManBoss.cpp, which emits their
    // vtables. The order of the declarations sets the order of those vtables
    // in .data (reverse order of declaration).

    class CItemBossMushroom : public CItemMushroom {
        public:
            CItemBossMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            virtual ~CItemBossMushroom() {}

            virtual void ExParamUpdate_();
    };

    class CItemBossToadstool : public CItemToadstool {
        public:
            CItemBossToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            virtual ~CItemBossToadstool() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &rDir, int);
    };

    class CItemBossBomb : public CItemBomb {
        public:
            CItemBossBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemBossBomb() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &rDir, int);
            virtual void DoExplode_();
    };

    class CItemBossBanana : public CItemBanana {
        public:
            CItemBossBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemBossBanana() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemBossGShell : public CItemGShell {
        public:
            CItemBossGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            virtual ~CItemBossGShell() {}

            virtual float GetBodyCollRadius() const;
            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &rDir, int);
    };

    class CItemBossRShell : public CItemRShell {
        public:
            CItemBossRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            virtual ~CItemBossRShell() {}

            virtual float GetBodyCollRadius() const;
            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &rDir, int);
    };
}

#endif /* !MSW_CITEMBOSS_HPP_ */
