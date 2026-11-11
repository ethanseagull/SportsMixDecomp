#ifndef MSW_CGAMEGUIDEMANBASKET_HPP_
#define MSW_CGAMEGUIDEMANBASKET_HPP_

namespace msw {

    class CChrMan;

    // INCOMPLETE: the in-game guide of the basketball game (BasketF). Only
    // the static functions game/basket/CChrManBasket.cpp calls are declared;
    // return types are guesses.
    class CGameGuideManBasket {
        public:
            // Makes the guide for the character manager pMan.
            static void Create(CChrMan *pMan);
            // Takes down the guide telop on screen.
            static void RemoveGuideTelop();
    };
}

#endif /* !MSW_CGAMEGUIDEMANBASKET_HPP_ */
