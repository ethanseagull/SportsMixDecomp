#ifndef MSW_CSHADOWTEXMAN_HPP_
#define MSW_CSHADOWTEXMAN_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    // The shadow texture task of a game (DOL class). Only what the game
    // tasks call: each sport's GenerateInstance creates it first (BossF
    // game/boss/GameBoss.cpp). Return type guessed.
    class CShadowTexMan {
        public:
            static void CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSHADOWTEXMAN_HPP_ */
