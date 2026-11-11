#ifndef MSW_CCHRCOREBASKET_HPP_
#define MSW_CCHRCOREBASKET_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    // The basketball character core (BasketF).
    //
    // INCOMPLETE: only what game/basket/CComDrvBasket.cpp calls is
    // declared. CChrCore is incomplete too, so the class must not be
    // instantiated or have its vtable emitted yet.
    class CChrCoreBasket : public CChrCore {
        public:
            virtual ~CChrCoreBasket();

            // Not zero when pChr (holding the ball) is in reach of a cut
            // (steal) by this character. (meaning guessed from the name)
            int CutHitCheck(const CChrCore *pChr) const;
    };
}

#endif /* !MSW_CCHRCOREBASKET_HPP_ */
