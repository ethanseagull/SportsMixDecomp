#ifndef CGAMEBASKET_HPP_
#define CGAMEBASKET_HPP_

#include "game/msw/CGame.hpp"

namespace msw {
    class CBall;
    class CChrCore;
}

namespace msw {

    // Lives in module BasketF. Size 0x274.
    class CGameBasket : public CGame {
        // The ball starts the rebound camera (CBallBasket::ReboundDemoCheck_).
        friend class CBallBasket;

        public:
            CGameBasket(bcs::CTask *pParent);

            // Creates the game the current settings ask for: this class or
            // the basketball party game.
            static CGame *CreateInstance(); // return type guessed

            // Added for game/basket/CNpcGhostBasketBase.cpp (names from the
            // vtable and the DOL symbols; return types and meanings
            // guessed). Slots 0x2A8 to 0x2B0 of __vt__Q23msw11CGameBasket.
            virtual void ForceGameExit_(long);
            // Gives the ball to team nTeam.
            virtual void SetOffenseTeam_(long nTeam);
            virtual void StopEffect_(float, float, float);

            // The instance as this class (guessed inline: the callers read
            // CGame::m_pInstance directly).
            static CGameBasket *GetInstance() { return static_cast<CGameBasket *>(m_pInstance); }
            // Sets game flag bits (an inline: the original reloads the flags
            // word at each call).
            void AddGameFlag_(unsigned long nFlag) { m_nGameFlag |= nFlag; }
            unsigned long GetGameFlag_() const { return m_nGameFlag; }
            unsigned long GetBasketFlag_() const { return m_nBasketFlag; }
            // The NPC type of team nTeam's ghost (CChrMan::NpcSearch finds
            // it by that).
            static long GetTeamGhostType(long nTeam);
            // Not zero while the game's main loop runs (play is on).
            int IsNowMainLoopEnable_() const;
            // (added for game/basket/CComManBasket.cpp) The radius of the
            // three-point line.
            static float GetCourt3PointSize();
            // (added for game/basket/CChrManBasket.cpp; return type guessed)
            // Plays exceptional slow camera nType.
            void PlayExceptionalCameraSlow_(long nType);
            // (added for game/basket/unit_23654.cpp; meaning guessed from the
            // name) Turns the check of the special move's endure on or off.
            void SetCheckSpEndure_(int bCheck);
            // (added for game/basket/unit_31D0C.cpp; meaning guessed from the
            // name) Restarts team nTeam after one of its characters fell into
            // the river.
            void SetRiverFallRestart_(long nTeam);

            // (added for game/basket/CGameBasketHungryBossPackun.cpp) The
            // overrides of CGame's virtual functions, from the slots of
            // __vt__Q23msw27CGameBasketHungryBossPackun that name this class
            // and from the ones that class calls directly. Return types as
            // in CGame.hpp.
            virtual ~CGameBasket();
            virtual void MainFunc();
            virtual void DispFunc();
            virtual void FinishFirstLoadData();
            virtual void GenerateInstance();
            virtual void PrepareRestartGame_();
            virtual void SendMessage_(long, long, long, long);
            virtual void CtorStateGameInit();
            virtual void CtorStateGameSetup();
            virtual void LoopStateGameSetup();
            virtual void DtorStateGameSetup();
            virtual void CtorStateWaitStart();
            virtual void LoopStateWaitStart();
            virtual void DtorStateWaitStart();
            virtual void CtorStateMainLoop();
            virtual void LoopStateMainLoop();
            virtual void DtorStateMainLoop();
            virtual void CtorStateGoalEvent();
            virtual void LoopStateGoalEvent();
            virtual void DtorStateGoalEvent();
            virtual void CtorStateTurnResult();
            virtual void LoopStateTurnResult();
            virtual void DtorStateTurnResult();
            virtual void CtorStateTimeUp();
            virtual void LoopStateTimeUp();
            virtual void DtorStateTimeUp();
            virtual void CtorStateChgCourt();
            virtual void LoopStateChgCourt();
            virtual void DtorStateChgCourt();
            virtual void CtorStateTurnOver();
            virtual void LoopStateTurnOver();
            virtual void DtorStateTurnOver();
            virtual void CtorStateChgOffense();
            virtual void LoopStateChgOffense();
            virtual void DtorStateChgOffense();
            virtual void CtorStateGameSet();
            virtual void LoopStateGameSet();
            virtual void DtorStateGameSet();
            virtual void CtorStateGameResult();
            virtual void LoopStateGameResult();
            virtual void DtorStateGameResult();
            virtual void SetGameResult_();
            virtual void CalcAddSkillPoint_();
            virtual int SetGameResultNext_();
            virtual void AddTeamCoin_(long, long);
            virtual void TrgBasketGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, long);
            virtual long GetMaxCoin_() const { return 10; }
            virtual long GetFirstGoalIdxMode_();
            virtual int IsGameSet_();
            virtual void SetCockpitArtsGaugeRatio_(long, float, float);
            virtual void SetCockpitArtsGaugeSpAnim_(long, int);
            virtual void SetDispRadarIcon_(int);
            virtual void SetCockpitCoinGetTelopMove_(long, long);
            virtual void SetCockpitDisp_(int);
            virtual void SetCockpitCoinDisp_(int);
            virtual void SetCockpitTimeDisp_(int);
            virtual void SetCockpitScoreDisp_(int);
            virtual void SetCockpitPeriodDisp_(int);
            virtual long GetGoalDemoType_(long, long) const;
            virtual long GetGoalDemoTelop_(long, long, long, int) const;
            virtual void StartGoalTelop_();
            virtual long GetExceptionalCameraType_();
            virtual void ScoreBookInitCountWork_();
            virtual void ScoreBookAddCountWork_(long, long, long);
            virtual void ScoreBookApplyCountWork_();
            virtual int MenuCallGameResult_();
            virtual int MenuCallInterval_();
            virtual int MenuCallPause_();
            virtual void CockpitCheckArrowState_();
            virtual void CameraReset_();

            // (static; return type guessed) The level of the party game.
            static long GetPartyLv();

            // Member functions of the game (added for
            // game/basket/CGameBasketHungryBossPackun.cpp; return types and
            // parameter meanings guessed).
            long DoGoalEffect_(long nChr, long, long nShootType, unsigned long, const bcs::FVEC3 &rPos);
            // Shakes the camera (quake nQuake) for character nChr's view.
            void CameraQuake_(long nQuake, long nChr);
            int CheckTimeUp_();
            void ChrPosSetNormal_();
            // Not zero when the time display changes colour.
            int IsTimeColorChange();
            void CreateTelop_(long nTelop, int, long);
            void EndTelop_(long nTelop);
            void RemoveTelop_(long nTelop);
            int IsTelopEnd_(long nTelop) const;
            void PlaySnd_(long nSnd, const bcs::FVEC3 *pPos);
            void SetShotClockCount_(long nCount, long);
            // (public: CChrCoreBasket's Canbe* members ask them; game/basket/
            // unit_581FC.cpp) The character of team nTeam that jumps at the
            // jump ball; not zero while the play restarts after a fall into
            // the river.
            long GetNowJumpBallerChrIdx_(long nTeam) const;
            unsigned long IsRiverFallRestart_() const;

        protected:
            // (added for game/basket/CGameBasket.cpp; the members below are
            // the game's own, names and meanings guessed from the code)
            void Initialize_();
            void Finalize_();
            void CheckTimeUp__();
            void UpDateShotClock_();
            int LoopStateTurnOverToShotClock();
            int LoopStateTurnOverToRiverFall();
            long GetShootPointType_(long nChr, long nShootType) const;
            void StopEffectGoalIn_(long nShootType);
            void SetDispScoreCountResult_();
            void TrgBasketGoalIn__(long nChr, long nTeam, long nShootType, long nBall, float fRate,
                                   const bcs::FVEC3 &rPos, long nBallIdx, int bDemo, int bCoin);
            void GameInit_();
            void GameSetUp_();
            void ChrPosSetResult_();
            unsigned long IsNowShotClock_() const;
            int SetClockOver();
            void PositionChange_(long nTeam);
            void PlaySndGoalShake_(long nGoal);
            void CheckExceptionalCamera_();
            void ClearJumpBallerInfo();
            void SetUpJumpBallerInfo();
            long GetNowJumpBallerPadNo_(long nTeam) const;
            void DispOnGoalMarker();
            void DispUpDateGoalMaker();
            void DispOffGoalMarker(int bAnim);
            unsigned long IsPlayReboundEffectCamera_() const;
            int PlayReboundEffectCamera_(long nGoal);
            void StopReboundEffectCamera_(int bStop);
            int IsRingMove_(long nGoal) const;
            int IsChandelierOn_() const;
            void PlayTrGoalInQuake(const CChrCore *pChr, const CBall *pBall);
            void DispShakeRemoconTelop_(int bDisp);
            void CheckSpEndure();
            int IsTelopEndStartAnim_(long nTelop);
            long GetTelopID_(long nIdx) const;
            void SetGoalCoinEffect(long nCoin, long nChr, long nTeam, long nShootType, const bcs::FVEC3 &rPos);
            void ResetSPGauge_();
            int CheckChangeStateToRiverFall();
            long AddTeamScore_(long nChr, long nTeam, long nShootType, const bcs::FVEC3 &rPos, int bCoin);
            void DispShotClock_();
            static float GetCourtWidth();
            static float GetCourtDepth();
            static bcs::FVEC3 GetGoalPos_OffsetZ(long nGoal);

            long m_nArrowState;      // 0x150, the arrow the cockpit shows (4: none)
            long m_nScoreArrowState; // 0x154, the score board's arrow
            long m_Unk158[2];        // 0x158 (not used here)
            long m_nShotClockColor;  // 0x160, the colour of the shot clock shown (-1: none)
            long m_nShotClockCount;  // 0x164, the second the shot clock shows
            long m_nLastArrowState;  // 0x168, the last arrow other than 4
            long m_nExCameraType;    // 0x16C, the exceptional camera playing (0: none)
            long m_nShootPoint;      // 0x170, points of a bonus shot kept for the next basket
            long m_nGoalTelop;       // 0x174, the telop of the last basket (GetTelopID_ index)
            long m_nGoalMarker;      // 0x178, the cockpit's goal marker icon (-1: none)
            long m_nRiverFallTeam;   // 0x17C, the team whose player fell into the river
            long m_nForceExitTeam;   // 0x180, the team ForceGameExit_ was given
            long m_nGoalCoin;        // 0x184, coins scored with the baskets so far
            // 0x188: the score book: dunks, layups, throws, three-pointers
            // and blocks, each made and missed.
            long m_anScoreBook[10];
            unsigned long m_nGoalBallType; // 0x1B0, 1: an item ball scored, 2: a special ball
            // 0x1B4: flag bits; 1 checks the special move's endure, 2 shows
            // the goal marker, 4 a tournament mission start, 8 the rebound
            // camera plays, 0x10 it ended.
            unsigned long m_nBasketFlag;
            float m_fReboundTime;     // 0x1B8, frames the rebound camera has played
            float m_fReboundNoShoot;  // 0x1BC, frames of it without a shot in the air
        public:
            // 0x1C0 (meaning not known): CComManBasketGame::CanbeShootState_
            // lets a computer team shoot while it is above a level parameter.
            // Counts the frames of the main loop up to 9999.9.
            float m_fUnk1C0;

        protected:
            long m_anJumpBaller[4];      // 0x1C4, each team's character at the jump ball
            // 0x1D4: how each team's jump baller is chosen: 0 none, 1 by the
            // players' pads in turn, 2 by the computer.
            long m_anJumpBallType[4];
            long m_aanJumpBallPad[4][3]; // 0x1E4, the pad of each place
            long m_anTeamScore[4];       // 0x214, each team's score as the score count shows it
            long m_anArtsCount[4];       // 0x224, each team's baskets towards the special move
            long m_anJumpBallerNum[4];   // 0x234, the human players of each team
            long m_aanJumpBallerPad[4][3]; // 0x244, their pads in order
    };
}

#endif /* !CGAMEBASKET_HPP_ */
