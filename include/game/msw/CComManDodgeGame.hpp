#ifndef MSW_CCOMMANDODGEGAME_HPP_
#define MSW_CCOMMANDODGEGAME_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // INCOMPLETE: the computer players' manager of the dodgeball game
    // (game/dodge/CComManDodgeGame.cpp). Only what
    // game/dodge/CComDrvDodgeGameOffRun.cpp calls is declared; the base is
    // taken to be CComMan as in VolleyF's CComManVolleyGame.
    class CComManDodgeGame : public CComMan {
        public:
            virtual ~CComManDodgeGame();

            // Not zero when pChr's ball is fully charged (meaning guessed
            // from the name).
            static int IsBallChargeMax(const CChrCore *pChr);
            // (added for game/dodge/unit_D7800.cpp; meaning of the int
            // arguments not known) A random infield character of team nTeam,
            // zero for none.
            static CChrCore *GetInnerChrCoreRand(long nTeam, int, int);
    };
}

#endif /* !MSW_CCOMMANDODGEGAME_HPP_ */
