#ifndef MSW_CGIMMICKMANVOLLEY_HPP_
#define MSW_CGIMMICKMANVOLLEY_HPP_

#include "game/msw/CGimmickMan.hpp"

namespace msw {

    // The gimmick manager of the volley game
    // (game/volley/CGimmickManVolley.cpp). No members of its own.
    class CGimmickManVolley : public CGimmickMan {
        public:
            CGimmickManVolley(bcs::CTask *pParent) : CGimmickMan(pParent) {}
            virtual ~CGimmickManVolley();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual long CreateDropCoin_(const bcs::FVEC3 &, long); // returns the number of coins made
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            virtual void EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long);
            virtual long FloorHitCheck_(const bcs::FVEC3 &, long);
            // New here, not in CGimmickMan (its slot follows FloorHitCheck_);
            // -1 when there is no coin to pop.
            virtual long QPanelChoicePopupCoin_(long);

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CGIMMICKMANVOLLEY_HPP_ */
