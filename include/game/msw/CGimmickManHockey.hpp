#ifndef MSW_CGIMMICKMANHOCKEY_HPP_
#define MSW_CGIMMICKMANHOCKEY_HPP_

#include "game/msw/CGimmickMan.hpp"

namespace msw {

    // The gimmick manager of the hockey game (HockeyF
    // game/hockey/CGimmickManHockey.cpp), size 0x40 (CreateInstance). No
    // members of its own.
    class CGimmickManHockey : public CGimmickMan {
        public:
            CGimmickManHockey(bcs::CTask *pParent) : CGimmickMan(pParent) {}
            virtual ~CGimmickManHockey();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual long QPanelChoicePopupItem_(long, int);
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual long CreateDropCoin_(const bcs::FVEC3 &, long); // returns the number of coins made
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            virtual void EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long);
            // New here, not in CGimmickMan (its slot follows FloorHitCheck_);
            // -1 when there is no coin to pop.
            virtual long QPanelChoicePopupCoin_(long);

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CGIMMICKMANHOCKEY_HPP_ */
