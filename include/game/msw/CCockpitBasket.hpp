#ifndef CCOCKPITBASKET_HPP_
#define CCOCKPITBASKET_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CCockpit.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The basket game's cockpit task (BasketF game/basket/CCockpitBasket.cpp),
    // size 0x21C: CCockpit at 0, bcs::CFsm<CCockpitBasket> at 0x1D4 (vptr at
    // 0x218), no members of its own. Seven states; GameResult has two
    // entries (4 and 5) that differ only in the objects they show.
    //
    // Most functions are static: each finds the task through
    // CCockpit::GetTask and does nothing without one. The display objects
    // they talk to are found by id: 0 CCockpitBasketCoder (the score board),
    // 1 CCockpitBasketHarapeko, 2 CCockpitBasketCoin, 7 the 3D score count.
    // Return types and argument names are guesses where no caller shows
    // them.
    class CCockpitBasket : public CCockpit, public bcs::CFsm<CCockpitBasket> {
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

            CCockpitBasket(bcs::CTask *pParent);
            virtual ~CCockpitBasket();
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
            // Shows or hides every display object (ids 0 to 29), then the
            // cockpit's own.
            static void SetDisp(int bDisp);
            static void SetCoinDispEnable(int bEnable);
            static void SetScoreDispEnable(int bEnable);
            static void SetTimeDispEnable(int bEnable);
            static void SetPeriodDispEnable(int bEnable);
            static void SetTime(float fTime, int nArg);
            static void SetRadarIconDisp(int bDisp);
            static void SetShotClockCount(long nCount, long nArg);
            // Shows both teams' scores from CGame.
            static void SetTeamScore();
            static void SetPeriod(long nPeriod);
            // bSpecial 1 plays the special animation.
            static void SetCoinSpecialAnim(long nTeam, int bSpecial);
            // fTelop above 0 also shows the gauge's telop.
            static void SetSPGaugeRatio(long nTeam, float fRatio, float fTelop);
            static void SetSPContrllerDisp(long nTeam, long nState);
            // Starts the 3D score count (object 7). Negative scores count
            // as 0.
            static void SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
                                             long nArg5, long nArg6, long nArg7, int bArg8, long nArg9);
            // nState 0 to 3 picks an arrow animation, anything else the
            // fifth.
            static void SetArrowState(long nState);
            // Only 0 and 1 do anything.
            static void SetScoreArrowState(long nState);
            static void SetPlayerScore(long nPlayer, long nScore, int bAnim, int bBig);
            static void SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint);
            static void ClearPlayerPointUp(long nPlayer);
            static void SetPlayerCrownIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, long nAction);
            static void SetHarapekoBallScore(long nBall, long nScore);
            static void SetHarapekoDispBallFruit4(int bDisp);
            static void SetHarapekoPlayerScoreDisp(int bDisp);
            static void SetHarapekoTeamScoreDisp(int bDisp);
            // The mission counter functions act only when nIdx is 0.
            static void SetMissionCountDisp(long nIdx, int bDisp);
            static void SetMissionCountNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountUpNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountActiveColor(int bActive);
            // The character icon of object nObjId (a CCockpitChrIcon2D).
            static void SetChrIcon(long nObjId, long nChr, long nUnk, const bcs::FVEC3 *pPos);
            static void ClearChrIcon(long nObjId, long nChr, long nUnk, int bUnk);
            static void SetCoinTelopState(long nTeam, long nState);
            // Queues a move of part nPart (0 all, 1 score, 2 coins, 3 time,
            // 4 period); see PartMoveCheck.
            static void SetPartMove(long nPart, long nMove);
            // CreateObjImpl on the task; -2 without a task. Hides
            // CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            // ChangeState on the task.
            static void SetMenuState(long nState);
            // Not in the module: its link dropped it (see the .cpp).
            static void SetSetCount();

            // (name guessed) Builds the state table and enters Initialize.
            void InitState();
            // Removes every display object the cockpit made.
            void Finalize();
            // Carries out the queued part moves on objects 0 to 2.
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

#endif /* !CCOCKPITBASKET_HPP_ */
