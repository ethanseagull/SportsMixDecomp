#ifndef MSW_CNPCBOSSPACKUN_HPP_
#define MSW_CNPCBOSSPACKUN_HPP_

#include "bcs/CFsmQueue.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    // INCOMPLETE: the boss Packun (Petey Piranha) NPC. A DOL class; BasketF
    // derives CNpcBossPackunBasketBase from it. Its state queue
    // bcs::CFsmQueue<CNpcBossPackun> is at 0x188 (InitFsm_ in the DOL).
    // Nothing else is declared; it must not be instantiated.
    class CNpcBossPackun : public CNpcBase, public bcs::CFsmQueue<CNpcBossPackun> {
        public:
            virtual ~CNpcBossPackun();
    };
}

#endif /* !MSW_CNPCBOSSPACKUN_HPP_ */
