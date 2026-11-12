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

            // (added for game/dodge/CComManDodgeGame.cpp, the rest of the
            // file; meanings guessed from the names and the code) The
            // outfield character team nTeam has assigned; whether team nTeam
            // holds ball nBall in the infield or in the outfield.
            static CChrCore *GetAssignOuterChrCore(long nTeam);
            static int IsInnerTeamHoldBall(long nTeam, long nBall);
            static int IsOuterTeamHoldBall(long nTeam, long nBall);
            // Moves rPos (world) off the corners of the infield of the team
            // opposite nTeam's outfield. Always 1.
            static int GetOuterCrossHoseiPos(bcs::FVEC3 &rPos, long nTeam);
            // Where an outfielder of the other side stands to catch a pass
            // over the target of ball nBall's holder (bTarget: towards the
            // holder's chosen outer target); zero for none.
            static int GetBallOppositePos(bcs::FVEC3 &rPos, int bTarget, long nBall);
            // The same from the ball at rPos (world) flying along rDir, for
            // the outfield of team nTeam's court.
            static int GetBallOppositePos2(bcs::FVEC3 &rPos, const bcs::FVEC3 &rBall, const bcs::FVEC3 &rDir,
                                           long nTeam);
            // The distance to add for pChr's condition (states) and level.
            static float GetConditionHandicapDis(const CChrCore *pChr);
            // The depth of the outfield beyond team nTeam's court, and the
            // inner edges of the outfield round the other court, in the
            // world.
            static float GetStageOuterDepth(long nTeam);
            static float GetStageInOuterLeft(long nTeam);
            static float GetStageInOuterRight(long nTeam);
            static float GetStageInOuterFront(long nTeam);
            static float GetStageInOuterBack(long nTeam);
            // Whether pChr's character can dodge an item ball with a roll.
            static int CanAvoidDodgeRoll_ItemBall(const CChrCore *pChr);
    };
}

#endif /* !MSW_CCOMMANDODGEGAME_HPP_ */
