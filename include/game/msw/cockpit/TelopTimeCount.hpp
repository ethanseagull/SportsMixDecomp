#ifndef COCKPIT_TELOPTIMECOUNT_HPP_
#define COCKPIT_TELOPTIMECOUNT_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {
    namespace cockpit {

        // The countdown telop of the result screen. Lives in the DOL; only
        // what BasketF's CCockpitBasket calls (a class or a namespace: the
        // symbol does not tell).
        class TelopTimeCount {
            public:
                // Sets the count of display object nObjId to nSec seconds;
                // pPos (null: unchanged) places it. Names guessed.
                static void SetParameter(long nObjId, long nSec, const bcs::FVEC3 *pPos);
        };
    }
}

#endif /* !COCKPIT_TELOPTIMECOUNT_HPP_ */
