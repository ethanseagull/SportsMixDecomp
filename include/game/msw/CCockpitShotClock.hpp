#ifndef CCOCKPITSHOTCLOCK_HPP_
#define CCOCKPITSHOTCLOCK_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The basket game's shot clock display (object 0x1002). Lives in the
    // DOL. Only what CCockpitBasket calls; the base is a guess, the size is
    // not known.
    class CCockpitShotClock : public CCockpitLayoutBase {
        public:
            void SetTimeCount(long nCount, long nArg); // return type guessed
    };
}

#endif /* !CCOCKPITSHOTCLOCK_HPP_ */
