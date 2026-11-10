#ifndef MSW_CITEMMUSHROOM_HPP_
#define MSW_CITEMMUSHROOM_HPP_

#include "game/msw/CItem.hpp"

namespace msw {

    // The mushroom (item type 6). A DOL class of size 0x1B4; its constructor
    // sets two floats of its own at 0x1AC and 0x1B0. The overrides are read
    // from __vt__Q23msw17CItemBossMushroom (BossF); return types are
    // guesses except for the inline bodies, which the modules emit.
    class CItemMushroom : public CItem {
        public:
            CItemMushroom(CItemMan *pMan);
            virtual ~CItemMushroom();

            virtual void Create(const bcs::FVEC3 &rPos);
            virtual long GetItemType() const { return 6; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual bool CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();

        protected:
            float m_fUnk1AC; // 0x1AC
            float m_fUnk1B0; // 0x1B0
    };
}

#endif /* !MSW_CITEMMUSHROOM_HPP_ */
