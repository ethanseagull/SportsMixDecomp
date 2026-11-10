#ifndef CCOCKPITBOSSPAUSE_HPP_
#define CCOCKPITBOSSPAUSE_HPP_

#include "game/msw/CCockpitPause.hpp"

namespace msw {

    // The pause menu of the boss games (BossF). Size 0xAC, no members of its
    // own; it overrides the pages that differ from CCockpitPause's.
    class CCockpitBossPause : public CCockpitPause {
        public:
            // Makes the task under pParent unless one is already live; returns
            // the live one in that case.
            static bcs::CTask *Create(bcs::CTask *pParent);

            CCockpitBossPause(bcs::CTask *pParent);
            virtual ~CCockpitBossPause();
            virtual void Main();
            virtual void Disp();

            virtual void BallPlay_In();
            virtual void BallPlay_Loop();
            virtual void BallPlay_Out();
            virtual void NormalPlay_In();
            virtual void NormalPlay_Loop();
            virtual void NormalPlay_Out();
            virtual void GameInfo_In();
            virtual void GameInfo_Loop();
            virtual void GameInfo_Out();
            virtual void GameInfo_Init();
    };
}

#endif /* !CCOCKPITBOSSPAUSE_HPP_ */
