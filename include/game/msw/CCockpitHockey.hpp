#ifndef CCOCKPITHOCKEY_HPP_
#define CCOCKPITHOCKEY_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CCockpit.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The hockey game's cockpit task (HockeyF game/hockey/CCockpitHockey.cpp),
    // size 0x21C: CCockpit at 0, bcs::CFsm<CCockpitHockey> at 0x1D4 (vptr at
    // 0x218), no members of its own. The frame is BasketF's CCockpitBasket:
    // seven states; GameResult has two entries (4 and 5).
    //
    // Most functions are static: each finds the task through
    // CCockpit::GetTask and does nothing without one. The display objects
    // they talk to are found by id (see the .cpp). Return types and argument
    // names are guesses where no caller shows them.
    class CCockpitHockey : public CCockpit, public bcs::CFsm<CCockpitHockey> {
        public:
            enum {
                STATE_INITIALIZE = 0,
                STATE_GAMERESTART = 1,
                STATE_GAMEPLAY = 2,
                STATE_GAMEPAUSE = 3,
                STATE_GAMERESULT = 4,
                // The same functions as STATE_GAMERESULT; the result
                // objects differ (and the online exchange is skipped).
                STATE_GAMERESULT_2 = 5,
                STATE_GAMEPARTYRESULT = 6,
                STATE_NUM = 7
            };

            CCockpitHockey(bcs::CTask *pParent);
            virtual ~CCockpitHockey();
            virtual void Main();
            virtual void Disp();
            virtual long CreateObjImpl(long nObjId, long nParam);
            // nTeam: a team (0 or 1); the displays are indexed by the team's
            // colour (CGame::GetTeamColorIdx).
            virtual void SetTeamScoreCoder(long nTeam, long nScore);
            virtual void AddTeamScoreCoder(long nTeam, long nScore);
            virtual void SetTeamScoreColorCoder(long nTeam, int nFont);
            virtual void SetTeamCoinCoder(long nTeam, long nCoin);
            virtual void AddTeamCoinCoder(long nTeam, long nCoin, long nAdd);
            virtual void SetIconFaceImpl();
            virtual void SetSPGaugeDispEnableImpl(int bEnable);
            virtual long GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            virtual bcs::CTask *Get3DCoinTaskImpl(long nTeam);
            // Overrides CCockpit's and bcs::CFsm's at once.
            virtual long ChangeState(long nState);

            // Creates the task under pParent unless a cockpit exists.
            static void Create(bcs::CTask *pParent);
            // Shows or hides every display object (ids 0 to 40), then the
            // cockpit's own.
            static void SetDisp(int bDisp);
            static void SetCoinDispEnable(int bEnable);
            static void SetScoreDispEnable(int bEnable);
            static void SetTimeDispEnable(int bEnable);
            static void SetPeriodDispEnable(int bEnable);
            // Shows the two puck markers when bEnable is 1.
            static void SetBallMarkerDispEnable(int bEnable);
            // Passed to the first board found: the score board, the party
            // board or the extra match's board.
            static void SetTime(float fTime, int nArg);
            static void SetRadarIconDisp(int bDisp);
            static void SetGoalIconDisp(long nGoal, int bDisp);
            // Shows both teams' scores from CGame.
            static void SetTeamScore();
            static void SetPeriod(long nPeriod);
            // The extra match's set points of the two teams.
            static void SetSetCount(long nPoint0, long nPoint1);
            // Not in the module: its link dropped it (see the .cpp).
            static void SetSetCount();
            // bSpecial 1 plays the special animation.
            static void SetCoinSpecialAnim(long nTeam, int bSpecial);
            // fTelop above 0 also shows the gauge's telop.
            static void SetSPGaugeRatio(long nTeam, float fRatio, float fTelop);
            static void SetSPContrllerDisp(long nTeam, long nState);
            // Starts the 3D score count (object 8). Negative scores count
            // as 0.
            static void SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
                                             long nArg5, long nArg6, long nArg7, int bArg8, long nArg9);
            // nState 0 to 3 picks an arrow animation, anything else the
            // fifth.
            static void SetArrowState(long nState);
            // Only 0 and 1 do anything.
            static void SetScoreArrowState(long nState);
            // The party board's players.
            static void SetPlayerScore(long nPlayer, long nScore, int bAnim, int bBig);
            static void SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint);
            static void ClearPlayerPointUp(long nPlayer);
            static void SetPlayerLeaveCount(long nPlayer, long nCount, const bcs::FVEC3 *pPos, int bArg);
            // On the party board and the extra match's board.
            static void SetPlayerShakeIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, int bArg);
            static void SetPlayerCrownIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, long nAction);
            // The mission counter functions (but the colour) act only when
            // the first argument is 0.
            static void SetMissionCountDisp(long nIdx, int bDisp);
            static void SetMissionCountNum(long nIdx, long nCount, long nTarget);
            static void SetMissionCountUpNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountActiveColor(int bActive);
            // The rink map (object 9).
            static void SetMapDispEnable(int bEnable);
            static void SetMapCamera(float fPos);
            static void SetMapPlayerPosition(long nPlayer, float fX, float fY, float fZ, long nArg4, long nArg5);
            static void SetMapPlayerDisp(long nPlayer, int bDisp);
            static void SetMapBallDisp(long nBall, int bDisp);
            static void SetMapDirection(int nDirection);
            static void SetMapGoalTarget(long nTarget);
            // The character icon of object nObjId (a CCockpitChrIcon2D).
            static void SetChrIcon(long nObjId, long nChr, long nUnk, const bcs::FVEC3 *pPos);
            static void ClearChrIcon(long nObjId, long nChr, long nUnk, int bUnk);
            // Added for game/hockey/CChrManHockey.cpp (CFighting; parameter
            // meanings guessed): the fight gauge of team nTeam (object 10),
            // its icon over a fighter (at *pPos), the icon's removal and the
            // gauge's fill ratio.
            static void SetFightGaugeIcon(long nTeam, const bcs::FVEC3 *pPos);
            static void SetFightGaugeRatio(long nTeam, float fRatio);
            static void ClearFightGaugeIcon(long nTeam, int);
            // The extra match's faces: nTurn0 on side 0, nTurn1 on side 1.
            static void SetIconFaceByTurnNo(long nTurn0, long nTurn1, long nArg);
            static void SetCoinTelopState(long nTeam, long nState);
            // Queues a move of part nPart (0 all, 1 score, 2 coins, 3 time,
            // 4 period); see PartMoveCheck.
            static void SetPartMove(long nPart, long nMove);
            // CreateObjImpl on the task; -2 without a task. Hides
            // CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            // ChangeState on the task.
            static void SetMenuState(long nState);

            // (name guessed) Builds the state table and enters Initialize.
            void InitState();
            // Removes every display object the cockpit made.
            void Finalize();
            // Carries out the queued part moves on objects 0, 2, 1 and 3.
            void PartMoveCheck();

            void Initialize_In();
            void Initialize_Loop();
            void Initialize_Out();
            void GameRestart_In();
            void GameRestart_Loop();
            void GameRestart_Out();
            void GamePlay_In();
            void GamePlay_Loop();
            void GamePlay_Out();
            void GamePause_In();
            void GamePause_Loop();
            void GamePause_Out();
            void GameResult_In();
            void GameResult_Loop();
            void GameResult_Out();
            void GamePartyResult_In();
            void GamePartyResult_Loop();
            void GamePartyResult_Out();
    };
}

#endif /* !CCOCKPITHOCKEY_HPP_ */
