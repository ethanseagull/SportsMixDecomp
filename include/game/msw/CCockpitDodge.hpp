#ifndef CCOCKPITDODGE_HPP_
#define CCOCKPITDODGE_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CCockpit.hpp"

namespace bcs {
    class CTask;
    struct FVEC3;
}

namespace msw {
    class CChrCore;

    // The dodgeball game's cockpit task (DodgeF game/dodge/CCockpitDodge.cpp),
    // size 0x21C: CCockpit at 0, bcs::CFsm<CCockpitDodge> at 0x1D4 (vptr at
    // 0x218), no members of its own. Seven states, as in CCockpitBasket;
    // GameResult has two entries (4 and 5) that differ only in the objects
    // they show.
    //
    // Most functions are static: each finds the task through
    // CCockpit::GetTask and does nothing without one. The display objects
    // they talk to are found by id (see the OBJ_* list in the .cpp): 6 the
    // score board (CCockpitDodgeCoder), 7 the life gauges, 10 the coins, 11
    // the ball power, 45 the "dodge or die" board. Return types and
    // argument names are guesses where no caller shows them; nTeam is a
    // team (0 or 1), the displays are indexed by the team's colour
    // (CGame::GetTeamColorIdx).
    class CCockpitDodge : public CCockpit, public bcs::CFsm<CCockpitDodge> {
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

            CCockpitDodge(bcs::CTask *pParent);
            virtual ~CCockpitDodge();
            virtual void Main();
            virtual void Disp();
            virtual long CreateObjImpl(long nObjId, long nParam);
            virtual void SetTeamCoinCoder(long nTeam, long nCoin);
            virtual void AddTeamCoinCoder(long nTeam, long nCoin, long nAdd);
            virtual void SetIconFaceImpl();
            virtual void SetSPGaugeDispEnableImpl(int bEnable);
            // The target of a coin effect: the ball power display's.
            virtual long GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            // Overrides CCockpit's and bcs::CFsm's at once.
            virtual long ChangeState(long nState);

            // Creates the task under pParent unless a cockpit exists.
            static void Create(bcs::CTask *pParent);
            // CCockpit::GetTask (an out-of-line copy in the module).
            static CCockpit *GetTask();
            // Shows or hides every display object (ids 0 to 54), then the
            // cockpit's own.
            static void SetDisp(int bDisp);
            static void SetCoinDispEnable(int bEnable);
            // Does nothing (the dodge cockpit has no score display).
            static void SetScoreDispEnable(int bEnable);
            static void SetSetCountDispEnable(int bEnable);
            static void SetTimeDispEnable(int bEnable);
            static void SetLifeDispEnable(int bEnable);
            static void SetPassBallDispEnable(int bEnable);
            static void SetRadarIconDisp(int bDisp);
            static void SetShotClockCount(long nCount, long nArg);
            static void SetTime(float fTime, int nArg);
            // Queues a move of part nPart (0 all, 1 gauge, 2 set count, 3
            // coins); see PartMoveCheck.
            static void SetPartMove(long nPart, long nMove);
            // The sets won by each team.
            static void SetSetCount(long nCount0, long nCount1);
            // Not in the module: its link dropped it (see the .cpp).
            static void SetSetCount();
            // bSpecial 1 plays the special animation.
            static void SetCoinSpecialAnim(long nTeam, int bSpecial);
            // Plays the "coins used" animation for nNum coins.
            static void SetCoinSpendAnim(long nTeam, long nNum);
            // fTelop above 0 also shows the gauge's telop.
            static void SetSPGaugeRatio(long nTeam, float fRatio, float fTelop);
            static void SetLifeCoder(long nTeam, long nNo, long nLife, long nMax);
            static void SetLifePercent(long nTeam, long nNo, float fRate, float fRate2);
            static void SetLifeValue(long nTeam, long nNo, unsigned short nLife);
            static void SetPassCoder(long nPass);
            static void AddPassCoder(long nPass, long nAdd);
            static void AddLifeCoder(long nTeam, long nNo, long nPrevLife, long nLife, long, long);
            // nState 0 to 3; anything else does nothing.
            static void SetLifeGaugeState(long nTeam, long nNo, long nState, long, long);
            // The lock-on cursor: nType 1 picks the first group of events,
            // anything else the second; the second view (split screen)
            // gets the same.
            static void SetLockCursor(int nType, long nIdx, long nArg);
            static void SetTelopSetCount(long, long, long, long, long);
            static void SetOutSign_MuchPoint(long nObjId);
            // Only for the two "out" telops (objects 29 and 30).
            static void SetOutSign_ChrNo(long nObjId, long nChr, long nTeam);
            // Does nothing; returns 0.
            static long CreateOutEffect(long, long, bcs::FVEC3 pos);
            // A small telop (objects 43 and 44) over character nChr, moved
            // by pOffset when given. Negative on a bad argument.
            static long CreateMiniTelop(long nObjId, long nChr, bcs::FVEC3 *pOffset);
            static void SetSPContrllerDisp(long nTeam, long nState);
            // nMode 0 or 1; anything else does nothing.
            static void SetTelopInOut(long nObjId, long nMode, long nTeam, const wchar_t *pText0,
                                      const wchar_t *pText1);
            static void SetChargeGauge(long nTeam, long nNo, float fRate, float fRate2);
            static void SetFaceIcon(long nTeam, long nNo, CChrCore *pChr);
            static void SetLifeGaugeLayout(long nLayout);
            // The mission counter functions act only when nIdx is 0.
            static void SetMissionCountDisp(long nIdx, int bDisp);
            static void SetMissionCountNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountUpNum(long nIdx, long nNum, long nMax);
            static void SetMissionCountActiveColor(int bActive);
            // The character icon of object nObjId (a CCockpitChrIcon2D).
            // Object 48 stands for the pair 49 and 50: the icon is cleared
            // from both and set on the one matching the pad style.
            static void SetChrIcon(long nObjId, long nIcon, long nChr, const bcs::FVEC3 *pPos);
            static void ClearChrIcon(long nObjId, long nIcon, long nChr, int bFade);
            static void SetBallPowerValue(long nTeam, short nValue);
            static void SetBallPowerValueColor(long nTeam, int nColor);
            static void SetBallPowerState(long nTeam, long nState);
            static void SetPowerUpTelopState(long nTeam, long nState);
            static void SetBallPowerShift(long nTeam, long nShift);
            static void SetOutTelopState(long nTeam, long nState, int bArg);
            static void SetPeriod(long nPeriod);
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

#endif /* !CCOCKPITDODGE_HPP_ */
