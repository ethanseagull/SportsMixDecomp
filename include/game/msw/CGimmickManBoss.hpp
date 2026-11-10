#ifndef MSW_CGIMMICKMANBOSS_HPP_
#define MSW_CGIMMICKMANBOSS_HPP_

#include "game/msw/CGimmickMan.hpp"

namespace msw {

    // The gimmick manager of the boss game (BossF
    // game/boss/CEffectCoinScore.cpp). No members of its own.
    class CGimmickManBoss : public CGimmickMan {
        public:
            CGimmickManBoss(bcs::CTask *pParent) : CGimmickMan(pParent) {}
            virtual ~CGimmickManBoss();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual long CreateDropCoin_(const bcs::FVEC3 &, long); // returns the number of coins made
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            virtual void EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long);
            virtual long QPanelChoicePopupCoin_(long);

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CGIMMICKMANBOSS_HPP_ */
