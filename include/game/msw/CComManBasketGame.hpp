#ifndef MSW_CCOMMANBASKETGAME_HPP_
#define MSW_CCOMMANBASKETGAME_HPP_

namespace msw {

    // INCOMPLETE: the computer players' manager of the basketball game
    // (BasketF). Only the one static function the ghost calls is declared.
    class CComManBasketGame {
        public:
            // (static, meaning guessed from the name) Tells the computer
            // players that the ghost caught a shot.
            static void MsgShootCatchToGhost();
    };
}

#endif /* !MSW_CCOMMANBASKETGAME_HPP_ */
