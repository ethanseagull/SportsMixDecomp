#ifndef MSW_CGAMEGUIDEMANBASKET_HPP_
#define MSW_CGAMEGUIDEMANBASKET_HPP_

namespace msw {

    class CChrCore;
    class CChrMan;

    // The in-game guide of the basketball game (BasketF): static functions
    // over CGameGuideBasket's instance (game/basket/CGameGuideBasket.cpp).
    // Return types are guesses.
    class CGameGuideManBasket {
        public:
            // Makes the guide for the character manager pMan.
            static void Create(CChrMan *pMan);
            // Takes down the guide telop on screen.
            static void RemoveGuideTelop();
            // Puts up and takes down the jump ball guide (cockpit object
            // 0x1077); not zero once it has gone.
            static void StartJumpBall();
            static void EndJumpBall();
            static long IsJumpBallGuide();
            // Holds the guides back for a while after a change of the
            // controlled character.
            static void OnChrChange();
            // CGameGuideBasket::AddSuccessCount on the instance.
            static void AddSuccessCount(const CChrCore *pChr, long nType);
    };
}

#endif /* !MSW_CGAMEGUIDEMANBASKET_HPP_ */
