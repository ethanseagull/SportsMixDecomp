#ifndef MSW_CNPCBOSSPACKUNBASKETBASE_HPP_
#define MSW_CNPCBOSSPACKUNBASKETBASE_HPP_

#include "game/msw/CNpcBossPackun.hpp"

namespace msw {

    // The boss Piranha Plant of the basket game (BasketF), the one
    // CChrManBasket::GetBossPackun_ finds, derived from the DOL's
    // msw::CNpcBossPackun (whose bcs::CFsmQueue at 0x188 the
    // CBallManBasket::GetJustShotTiming*_ functions read).
    //
    // INCOMPLETE: only the state queue is placed; nothing else is declared.
    class CNpcBossPackunBasketBase : public CNpcBossPackun {
    };
}

#endif /* !MSW_CNPCBOSSPACKUNBASKETBASE_HPP_ */
