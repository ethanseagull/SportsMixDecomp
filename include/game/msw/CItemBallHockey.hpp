#ifndef MSW_CITEMBALLHOCKEY_HPP_
#define MSW_CITEMBALLHOCKEY_HPP_

#include "game/msw/CItemBall.hpp"

namespace msw {

    // Base of the item balls of the hockey game (HockeyF). Unlike the Boss
    // version it adds no triggers: DoTrg only forwards to CItemBall's, and
    // IsDisableRebound is not overridden (the derived vtables hold
    // CItemBall's). The item kinds are in CItemBallHockeyKinds.hpp.
    class CItemBallHockey : public CItemBall {
        public:
            // Inline: the item kinds of game/hockey/CItemBallHockeyBomb.cpp
            // call CItemBall's constructor directly.
            CItemBallHockey(CBall *pBall, long type) : CItemBall(pBall, type) {}

            // Declared first and defined inline below the class (guess): the
            // original object has no CItemBallHockey vtable, and a class whose
            // first virtual is defined out of line gets its vtable emitted in
            // that unit (notes/toolchain.md, "Where a vtable is emitted").
            virtual ~CItemBallHockey();
            virtual bool DoTrg(long trg);

            virtual bool TrgRebound_();
            virtual bool TrgReboundToFree_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgAttach_();
            virtual bool TrgDamage_();
    };

    inline CItemBallHockey::~CItemBallHockey() {}
}

#endif /* !MSW_CITEMBALLHOCKEY_HPP_ */
