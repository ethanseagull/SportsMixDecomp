#ifndef MSW_CGIMMICKMANDODGE_HPP_
#define MSW_CGIMMICKMANDODGE_HPP_

#include "game/msw/CGimmickMan.hpp"

namespace msw {

    // The gimmick manager of the dodgeball game (DodgeF
    // game/dodge/CEffectCoinGet.cpp). No members of its own.
    class CGimmickManDodge : public CGimmickMan {
        public:
            CGimmickManDodge(bcs::CTask *pParent) : CGimmickMan(pParent) {}
            virtual ~CGimmickManDodge();

            virtual void Initialize_();
            virtual void SetUp_();
            virtual void AllReset_();
            virtual long QPanelChoicePopupItem_(long, int);
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, int, int, int, long *, CItem **);
            virtual void EffectCoinGetCreate_(const bcs::FVEC3 &, DROP_COIN::CoinType);
            // New here, not in CGimmickMan (its slot follows FloorHitCheck_);
            // -1 when there is no coin to pop.
            virtual long QPanelChoicePopupCoin_(long);

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };

    // The gimmick manager of the dodgeball party game (game type 2, "Dodge
    // or Die"): the same with its own "?" panels. No members of its own.
    class CGimmickManDodgeDodgeOrDie : public CGimmickManDodge {
        public:
            CGimmickManDodgeDodgeOrDie(bcs::CTask *pParent) : CGimmickManDodge(pParent) {}
            virtual ~CGimmickManDodgeDodgeOrDie();

            virtual void Initialize_();

            // Creates the instance and returns it.
            static CGimmickMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CGIMMICKMANDODGE_HPP_ */
