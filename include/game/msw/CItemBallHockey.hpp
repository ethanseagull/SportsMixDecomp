#ifndef MSW_CITEMBALLHOCKEY_HPP_
#define MSW_CITEMBALLHOCKEY_HPP_

#include "game/msw/CItemBall.hpp"

namespace msw {

    // Base of the item balls of the hockey game (BossF). It adds four triggers
    // to the ten of CItemBall. No constructor is declared yet: the derived
    // classes call CItemBall's directly, so this class's is inline and its
    // arguments are not known from this unit.
    class CItemBallHockey : public CItemBall {
        public:
            enum {
                TRG_RECEIVE = TRG_NUM,
                TRG_BLOCK,
                TRG_ARTS_ATTACH,
                TRG_PREPARATION_TOUCH
            };

            // Defined inline below the class, not in it: with a body here the
            // compiler puts the class's vtable into CItemBallHockey.cpp, which
            // the original object does not have.
            virtual bool IsDisableRebound() const;
            virtual bool DoTrg(long trg);

            virtual bool TrgRebound_();
            virtual bool TrgReboundToFree_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgAttach_();

            virtual bool TrgReceive_() = 0;
            virtual bool TrgBlock_() = 0;
            virtual bool TrgArtsAttach_();
            virtual bool TrgPreparationTouch_();
    };

    inline bool CItemBallHockey::IsDisableRebound() const { return true; }
}

#endif /* !MSW_CITEMBALLHOCKEY_HPP_ */
