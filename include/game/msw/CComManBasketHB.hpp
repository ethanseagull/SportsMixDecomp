#ifndef MSW_CCOMMANBASKETHB_HPP_
#define MSW_CCOMMANBASKETHB_HPP_

namespace msw {

    // INCOMPLETE: computer helpers of the "HB" basketball game (the Hungry
    // Boss Packun court). Only one static function is known; the base class
    // is not known (probably CComManBasket, like the other per-game
    // computer managers), so none is declared.
    class CComManBasketHB {
        public:
            // Not zero when the boss Packun is asleep (its state queue is
            // empty or runs state 0), zero when there is no boss Packun.
            static int IsSleepBossPackun();
    };
}

#endif /* !MSW_CCOMMANBASKETHB_HPP_ */
