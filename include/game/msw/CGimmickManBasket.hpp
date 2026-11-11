#ifndef MSW_CGIMMICKMANBASKET_HPP_
#define MSW_CGIMMICKMANBASKET_HPP_

#include "game/msw/CGimmickMan.hpp"

namespace msw {

    // The gimmick manager of the basketball game
    // (BasketF game/basket/CGimmickManBasket.cpp), size 0x40 (CreateInstance).
    // No members of its own.
    class CGimmickManBasket : public CGimmickMan {
        public:
            CGimmickManBasket(bcs::CTask *pParent) : CGimmickMan(pParent) {}
            virtual ~CGimmickManBasket();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual long CreateDropCoin_(const bcs::FVEC3 &, long); // returns the number of coins made
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            virtual void EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long);

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CGIMMICKMANBASKET_HPP_ */
