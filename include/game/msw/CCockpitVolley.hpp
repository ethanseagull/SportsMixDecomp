#ifndef CCOCKPITVOLLEY_HPP_
#define CCOCKPITVOLLEY_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CCockpit.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The volley game's cockpit task (VolleyF game/volley/CCockpitVolley.cpp),
    // size 0x21C: CCockpit at 0, bcs::CFsm<CCockpitVolley> at 0x1D4 (vptr at
    // 0x218), no members of its own (78 member functions). The frame is
    // BasketF's CCockpitBasket: seven states; GameResult has two entries
    // (4 and 5).
    //
    // Most functions are static: each finds the task through
    // CCockpit::GetTask and does nothing without one. The display objects
    // they talk to are found by id (see the .cpp). Return types and argument
    // names are guesses where no caller shows them.
    class CCockpitVolley : public CCockpit, public bcs::CFsm<CCockpitVolley> {
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

            CCockpitVolley(bcs::CTask *pParent);
            virtual ~CCockpitVolley();
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
            // Shows or hides every display object (ids 0 to 53), then the
            // cockpit's own.
            static void SetDisp(int bDisp);
            static void SetCoinDispEnable(int bEnable);
            static void SetScoreDispEnable(int bEnable);
            static void SetSetCountDispEnable(int bEnable);
            static void SetRadarIconDisp(int bDisp);
            static void SetShotClockCount(long nCount, long nArg);
            // The third argument is not read.
            static void SetTeamScore(long nTeam, long nScore, int);
            // Shows both teams' scores of the current period from CGame.
            static void SetTeamScore();
            // Queues a move of part nPart (0 all, 1 coins, 3 score, 4 combo,
            // 5 ball score nArg); see PartMoveCheck.
            static void SetPartMove(long nPart, long nMove, long nArg);
            // Both the score board and the coins play serve animation nAnim.
            static void SetServeAnim(long nAnim);
            // The set points of the two teams.
            static void SetSetCount(long nPoint0, long nPoint1);
            // Not in the module: its link dropped it (see the .cpp).
            static void SetSetCount();
            // bSpecial 1 plays the special animation.
            static void SetCoinSpecialAnim(long nTeam, int bSpecial);
            // fTelop above 0 also shows the gauge's telop.
            static void SetSPGaugeRatio(long nTeam, float fRatio, float fTelop);
            static void SetSPContrllerDisp(long nTeam, long nState);
            // Starts the 3D score count (object 7). Negative scores count
            // as 0.
            static void SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
                                             long nArg5, long nArg6, long nArg7, int bArg8, long nArg9);
            // The 3D set count (object 8).
            static void SetTelopSetCount(long nArg0, long nArg1, long nArg2, long nArg3, long nArg4);
            // The boss match's board (object 50) and life gauge (51).
            static void SetBossCoinState(long nState);
            static void SetBossLifeRatio(float fRatio);
            static void SetBossTime(float fTime, int nArg);
            static void SetBossLifeCoder(long nLife);
            // Not in the module: its link dropped it (see the .cpp).
            static void AddBossLifeCoder(long nLife, long nAdd);
            // The rhythm game's boards (objects 52 and 53).
            static void SetRhythmScore(long nScore);
            static void SetRhythmTargetScore(long nScore, int bArg);
            static void SetRhythmHiScore(long nScore, int bArg);
            static void SetRhythmCombo(long nCombo);
            static void SetRhythmReceiveCount(long nCount);
            static void SetRhythmBonus(long nBonus, float fArg);
            static void SetRhythmBallScore(long nBall, long nScore);
            static void SetRhythmBallIcon(long nBall, long nIcon);
            static void SetRhythmScorePosition(long nPos);
            static void SetRhythmComboTelop(long nArg);
            static void SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint);
            static void ClearPlayerPointUp(long nPlayer);
            // The mission counter functions (but the colour) act only when
            // the first argument is 0 (a team or a pane index).
            static void SetMissionCountDisp(long nIdx, int bDisp);
            static void SetMissionCountNum(long nIdx, long nCount, long nTarget);
            static void SetMissionCountUpNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountActiveColor(int bActive);
            // The character icon nIcon of object nObjId (a
            // CCockpitChrIcon2D) over character nChr; with a split screen
            // the second view's object as well.
            static void SetChrIcon(long nObjId, long nIcon, long nChr, const bcs::FVEC3 *pPos, const void *pArg);
            static void ClearChrIcon(long nObjId, long nIcon, long nChr, int bFade);
            static void SetCoinTelopState(long nTeam, long nState);
            // CreateObjImpl on the task; -2 without a task. Hides
            // CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            // ChangeState on the task.
            static void SetMenuState(long nState);

            // (name guessed) Builds the state table and enters Initialize.
            void InitState();
            // Removes every display object the cockpit made.
            void Finalize();
            // Carries out the queued part moves on the objects of objIdTbl.
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

#endif /* !CCOCKPITVOLLEY_HPP_ */
