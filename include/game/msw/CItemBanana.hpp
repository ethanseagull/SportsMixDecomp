#ifndef MSW_CITEMBANANA_HPP_
#define MSW_CITEMBANANA_HPP_

#include "game/msw/CItem.hpp"

namespace msw {

    // The banana (item type 5). A DOL class of size 0x1AC. The overrides are
    // read from __vt__Q23msw15CItemBossBanana (BossF); return types are
    // guesses except for the inline bodies, which the modules emit.
    class CItemBanana : public CItem {
        public:
            CItemBanana(CItemMan *pMan);
            virtual ~CItemBanana();

            virtual void Create(const bcs::FVEC3 &rPos);
            virtual long GetItemType() const { return 5; }
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

#endif /* !MSW_CITEMBANANA_HPP_ */
