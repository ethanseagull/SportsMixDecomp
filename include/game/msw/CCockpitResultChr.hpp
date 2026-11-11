#ifndef CCOCKPITRESULTCHR_HPP_
#define CCOCKPITRESULTCHR_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The result screen's page of players. Lives in the DOL. Only what
    // BasketF's CCockpitBasket calls; the base is a guess, the size is not
    // known. Argument names guessed.
    class CCockpitResultChr : public CCockpitLayoutBase {
        public:
            // The face of member nMember of team nTeam (character nChr).
            void SetIconFace(long nUnk, long nTeam, long nMember, long nChr);
    };
}

#endif /* !CCOCKPITRESULTCHR_HPP_ */
