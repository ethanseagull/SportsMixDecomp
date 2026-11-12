#ifndef MSW_CFOOTMARKMAN_HPP_
#define MSW_CFOOTMARKMAN_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    // The footprint task of a game (DOL class). Only what the game tasks
    // call (BossF game/boss/GameBoss.cpp). Return type guessed.
    class CFootmarkMan {
        public:
            static void CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CFOOTMARKMAN_HPP_ */
