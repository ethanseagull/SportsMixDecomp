#ifndef CCOCKPITBOSS_HPP_
#define CCOCKPITBOSS_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CCockpit.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    // The boss game's cockpit task (BossF game/boss/CCockpitBoss.cpp), size
    // 0x21C: CCockpit at 0, bcs::CFsm<CCockpitBoss> at 0x1D4 (vptr at
    // 0x218), no members of its own. The frame is BasketF's CCockpitBasket:
    // seven states, of which six are set (GameResult twice).
    //
    // Most functions are static: each finds the task through
    // CCockpit::GetTask and does nothing without one. The display objects
    // they talk to are found by id: 0 CCockpitBossCoder (the life gauges,
    // the boss gauge and the coins), 1 CCockpitBossCoin, 2 the radar icons,
    // 3 CCockpitBossBallPower. Return types and argument names are guesses
    // where no caller shows them.
    class CCockpitBoss : public CCockpit, public bcs::CFsm<CCockpitBoss> {
        public:
            enum {
                STATE_INITIALIZE = 0,
                STATE_GAMERESTART = 1,
                STATE_GAMEPLAY = 2,
                STATE_GAMEPAUSE = 3,
                STATE_GAMERESULT = 4,
                STATE_GAMERESULT_2 = 5, // the same functions as STATE_GAMERESULT
                STATE_NUM = 7           // (state 6 is never set)
            };

            CCockpitBoss(bcs::CTask *pParent);
            virtual ~CCockpitBoss();
            virtual void Main();
            virtual void Disp();
            virtual long CreateObjImpl(long nObjId, long nParam);
            virtual void SetTeamCoinCoder(long nTeam, long nCoin);
            virtual void AddTeamCoinCoder(long nTeam, long nCoin, long nAdd);
            virtual void SetIconFaceImpl();
            virtual void SetSPGaugeDispEnableImpl(int bEnable);
            // Asks the ball-power display (object 3), not the coins.
            virtual long GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);
            virtual bcs::CTask *Get3DCoinTaskImpl(long nTeam);
            // Passes only nArg0 == 0 on to CCockpit's.
            virtual void SetItemRouletteStateImpl(long nArg0, long nArg1, long nArg2);
            // Overrides CCockpit's and bcs::CFsm's at once.
            virtual long ChangeState(long nState);

            // Creates the task under pParent unless a cockpit exists.
            static void Create(bcs::CTask *pParent);
            // Shows or hides display objects 0 to 9, then the cockpit's own.
            static void SetDisp(int bDisp);
            static void SetCoinDispEnable(int bEnable);
            static void SetRadarIconDisp(int bDisp);
            // bSpecial 1 plays the special animation (nTeam is not used).
            static void SetCoinSpecialAnim(long nTeam, int bSpecial);
            // Only fRatio is used.
            static void SetSPGaugeRatio(long nTeam, float fRatio, float fTelop);
            static void SetSPContrllerDisp(long nTeam, long nState);
            static void SetLifeGaugeLayout(long nLayout);
            static void SetLifeGaugeState(long nPlayer, long nState);
            static void SetLifePercent(long nPlayer, float fLife, float fLife2);
            static void SetBossLifePercent(float fRate);
            static void SetBossIcon(long nIcon);
            static void SetBallPowerValue(long nTeam, short nValue);
            static void SetBallPowerValueColor(long nTeam, int bColor);
            // A nonzero nState also sets the coder's coin state of nTeam to 1.
            static void SetBallPowerState(long nTeam, long nState);
            static void SetPowerUpTelopState(long nTeam, long nPlayer);
            static void SetOutTelopState(long nPlayer, int bOut);
            // Shows mini telop nObjId (only 9) above character nChr, moved
            // by pOffset if given. Negative on failure: -13 a wrong object,
            // -11 a wrong character, -12 a missing or disabled one.
            static long CreateMiniTelop(long nObjId, long nChr, bcs::FVEC3 *pOffset);
            // Queues a move of part nPart (0 all, 1 life gauges, 2 boss
            // gauge, 3 coins); see PartMoveCheck.
            static void SetPartMove(long nPart, long nMove);
            // CreateObjImpl on the task; -2 without a task. Hides
            // CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            // ChangeState on the task.
            static void SetMenuState(long nState);

            // (name guessed) Builds the state table and enters Initialize.
            void InitState();
            // Carries out the queued part moves on objects 0 and 1.
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
    };
}

#endif /* !CCOCKPITBOSS_HPP_ */
