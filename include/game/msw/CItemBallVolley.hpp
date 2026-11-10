#ifndef MSW_CITEMBALLVOLLEY_HPP_
#define MSW_CITEMBALLVOLLEY_HPP_

#include "game/msw/CItemBall.hpp"

namespace msw {

    // Base of the item balls of the volley game (BossF). It adds four triggers
    // to the ten of CItemBall. No constructor is declared yet: the derived
    // classes call CItemBall's directly, so this class's is inline and its
    // arguments are not known from this unit.
    class CItemBallVolley : public CItemBall {
        public:
            enum {
                TRG_RECEIVE = TRG_NUM,
                TRG_BLOCK,
                TRG_ARTS_ATTACH,
                TRG_PREPARATION_TOUCH
            };

            // Defined inline below the class, not in it: with a body here the
            // compiler puts the class's vtable into CItemBallVolley.cpp, which
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

    inline bool CItemBallVolley::IsDisableRebound() const { return true; }
}

#endif /* !MSW_CITEMBALLVOLLEY_HPP_ */
