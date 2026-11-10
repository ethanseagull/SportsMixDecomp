#ifndef CCOCKPITVOLLEYPAUSE_HPP_
#define CCOCKPITVOLLEYPAUSE_HPP_

#include "game/msw/CCockpitPause.hpp"

namespace msw {

    // The pause menu of the volley games (BasketF). Size 0xAC, no members of
    // its own; it overrides the pages that differ from CCockpitPause's: the
    // two how-to-play pages and the score board.
    class CCockpitVolleyPause : public CCockpitPause {
        public:
            // Makes the task under pParent unless one is already live; returns
            // the live one in that case.
            static bcs::CTask *Create(bcs::CTask *pParent);

            CCockpitVolleyPause(bcs::CTask *pParent);
            virtual ~CCockpitVolleyPause();
            virtual void Main();
            virtual void Disp();

            virtual void BallPlay_In();
            virtual void BallPlay_Loop();
            virtual void BallPlay_Out();
            virtual void NormalPlay_In();
            virtual void NormalPlay_Loop();
            virtual void NormalPlay_Out();
            virtual void ScoreBoard_In();
            virtual void ScoreBoard_Loop();
            virtual void ScoreBoard_Out();
    };
}

#endif /* !CCOCKPITVOLLEYPAUSE_HPP_ */
