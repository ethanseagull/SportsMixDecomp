#ifndef MSW_CNPCBOSSPACKUNBASKETBASE_HPP_
#define MSW_CNPCBOSSPACKUNBASKETBASE_HPP_

#include "bcs/CFsmQueue.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    // The boss Piranha Plant of the basket game (BasketF), the one
    // CChrManBasket::GetBossPackun_ finds. bcs::CFsmQueue at 0x188 (read
    // from CBallManBasket::GetJustShotTiming*_, which takes its state).
    //
    // INCOMPLETE: only the state queue is placed; nothing else is declared.
    class CNpcBossPackunBasketBase : public CNpcBase, public bcs::CFsmQueue<CNpcBossPackunBasketBase> {
    };
}

#endif /* !MSW_CNPCBOSSPACKUNBASKETBASE_HPP_ */
