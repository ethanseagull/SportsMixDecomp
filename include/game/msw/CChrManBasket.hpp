#ifndef MSW_CCHRMANBASKET_HPP_
#define MSW_CCHRMANBASKET_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    class CNpcBossPackun;

    // INCOMPLETE: the character manager of the basketball game (BasketF).
    // Allocated with 0xE8 and 0xEC bytes (the second by a derived class);
    // no members are declared, so it must not be instantiated.
    class CChrManBasket : public CChrMan {
        public:
            // The boss Packun whose index (the NPC's virtual at 0x80) is
            // nIdx; null when there is none.
            CNpcBossPackun *GetBossPackun_(long nIdx) const;
            // Makes the boss Packun nIdx at rPos (the Hungry Boss Packun
            // stages call it for the boss that carries goal 0).
            CNpcBossPackun *CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx);
    };
}

#endif /* !MSW_CCHRMANBASKET_HPP_ */
