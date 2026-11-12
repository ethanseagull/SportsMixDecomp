#ifndef UNLOCK_HPP_
#define UNLOCK_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    // Whether "Unlock" is a namespace or a class with a static member cannot
    // be told from the symbol; a namespace is a guess.
    namespace Unlock {
        // Creates the task of the unlock screen and returns it.
        bcs::CTask *CreateInstance(bcs::CTask *pParent);
        // (added for MenuF game/menu/CTopMenu.cpp; return types and
        // meaning guessed) Whether the unlock screen has something to show
        // on the top menu, and whether it has finished showing it.
        int IsDispMenuTop();
        int IsDispMenuEnd();
        // (added for BasketF game/basket/CGameBasketHungryBossPackun.cpp;
        // return type and meaning guessed) Checks the unlocks of party game
        // stage nStage; the party game passes 1.
        void CheckParty(long nStage, int);
        // (added for MenuF game/menu/CSaveSelect.cpp; meaning guessed from
        // the button sequence that calls it) Opens everything for player
        // nPlayer.
        void FullOpen(long nPlayer);
        // (added for BossF game/boss/GameBoss.cpp; return type and meaning
        // guessed) Checks the unlocks earned by the game just played.
        void CheckClear();
    }
}

#endif /* !UNLOCK_HPP_ */
