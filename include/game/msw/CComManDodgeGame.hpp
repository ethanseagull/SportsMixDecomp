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
            // (added for game/dodge/unit_A4AE8.cpp; meanings guessed from
            // the names) Whether ball nBall is flying at pChr's team as a
            // shot; whether pChr can dodge a shot by a reflecting roll; the
            // outfield character team nTeam has assigned (its index).
            static int IsBallOffensive(CChrCore *pChr, long nBall);
            static int IsReflectDodgeRoll(const CChrCore *pChr);
            static long GetAssignOuterChrCoreIdx(long nTeam);
            // (added for game/dodge/unit_D8380.cpp; meanings guessed) Where
            // the target marker of pSp's special shot is, and the marker
            // nearest to pChr (for shots with more than one); zero for none.
            static int GetSpTargetMarkerPos(CChrCore *pSp, bcs::FVEC3 &rPos, long);
            static int GetSpTargetMarkerNearPos(bcs::FVEC3 &rPos, CChrCore *pSp, const CChrCore *pChr);
    };
}

#endif /* !MSW_CCOMMANDODGEGAME_HPP_ */
