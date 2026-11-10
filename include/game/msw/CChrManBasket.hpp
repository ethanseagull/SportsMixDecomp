#ifndef MSW_CCHRMANBASKET_HPP_
#define MSW_CCHRMANBASKET_HPP_

#include "game/msw/CChrMan.hpp"

namespace msw {

    class CNpcBossPackunBasketBase;

    // The character manager of the basket game (BasketF).
    //
    // INCOMPLETE: only what game/basket/CBallManBasket.cpp calls.
    class CChrManBasket : public CChrMan {
        public:
            // (return type guessed) The boss Piranha Plant whose number
            // (virtual slot 0x80 of the NPC) is `nNo`, found with
            // CChrMan::NpcSearch among the NPCs of kind 2; null when there is
            // none.
            CNpcBossPackunBasketBase *GetBossPackun_(long nNo) const;

            // The instance as this class (guessed inline: the callers call
            // CChrMan::GetInstance and use the result directly).
            static CChrManBasket *GetInstance() { return static_cast<CChrManBasket *>(CChrMan::GetInstance()); }
    };
}

#endif /* !MSW_CCHRMANBASKET_HPP_ */
