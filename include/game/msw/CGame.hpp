#ifndef CGAME_HPP_
#define CGAME_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "revolution/rfl.h"

namespace msw {

    class CGameCamera;
    // Tasks of the game defined in game/msw/CGame.cpp (names guessed).
    class CGamePauseTask;
    class CGameStopTask;
    class CGameTaskGroup;
    class CItemRoulette;
    class CGameSpeedCtrl;

    // Base of the per-sport game tasks. Only what callers outside the class
    // need so far; the layout past the bases is not mapped.
    //
    // Bases read from __vt__Q23msw10CGameDodge (DodgeF): bcs::CUnivTask's
    // part (slots 0x08-0x14, GetFrameRateF is CUnivTask's), then a
    // three-word part whose only function is the "@52@" destructor thunk,
    // i.e. a second polymorphic base at 0x34 with nothing but a virtual
    // destructor: bcs::CFsmQueue<CGame> (0x60 bytes, its vptr at 0x34 +
    // 0x5C). Then CGame's own virtual functions from slot 0x24.
    //
    // TODO(base): the true first base is bcs::CUnivTask (same size and
    // slots; it only overrides GetFrameRateF). It is written as bcs::CTask
    // for now because including bcs/CUnivTask.hpp here reorders the weak
    // CTask inlines of SystemF's CMainStepEx.cpp (CUnivTask's inline body
    // names CTask::GetFrameRateF, and weak inlines come out in reverse order
    // of first reference), which fails SystemF's hash check. Switch back
    // before any source emits a vtable of a CGame-derived class.
    class CGame : public bcs::CTask, public bcs::CFsmQueue<CGame> {
        public:
            enum GAME_MODE {
                GAME_MODE_BASKET, // enumerator names guessed; the values are
                GAME_MODE_VOLLEY, // what each sport's constructor passes
                GAME_MODE_DODGE,
                GAME_MODE_HOCKEY,
                GAME_MODE_BOSS
            };
            // The parameter types of the static setters below. Only the
            // values passed so far are known; every enumerator name is a
            // guess.
            enum GAME_TYPE {
                GAME_TYPE_NORMAL = 0,
                GAME_TYPE_PARTY = 2   // see GameWork::m_nGameType
            };
            enum GAME_CUP {};
            enum GAME_PLAY_TYPE {};
            enum SPORTS_MODE {
                SPORTS_MODE_SINGLE = 0,
                SPORTS_MODE_MIX = 1   // passed for a mixed-sports tournament
            };
            enum GAME_TOURNAMENT_LEVEL {};
            enum GAME_LEVEL {
                GAME_LEVEL_EASY,
                GAME_LEVEL_NORMAL,
                GAME_LEVEL_HARD,
                GAME_LEVEL_EXPERT
            };
            enum QPANEL_LEVEL {};

            // Settings of the game being set up or played, 0x638 bytes.
            // (type and member names guessed)
            struct GameWork {
                // Callers compare the byte as a signed int, hence a getter
                // that widens it.
                long GetGameType() const { return m_nGameType; }
                // Widened the same way (TourF TournamentRule::SetupGame
                // compares both as signed ints). (names guessed)
                long GetGameMode() const { return m_nGameMode; }
                long GetUnk06() const { return m_nUnk06; }
                // (name guessed) msw::CCockpitTournament::CreateObjImpl turns
                // 2 and 3 into the logo telop's table values 1 and 2.
                long GetUnk07() const { return m_nUnk07; }
                // Compared as a signed int as well (VolleyF
                // CTmGameHandlerVolley::_SendMessage).
                long GetPeriod() const { return m_nPeriod; }
                // (name guessed) m_nUserTeam, with no team read as team 0;
                // the modules inline it (extsb, then srawi/andc).
                long GetUserTeam() const { return m_nUserTeam < 0 ? 0 : m_nUserTeam; }
                // Compared as a signed int too (HockeyF
                // CEventDemoHockey::CameraEvent).
                long GetStage() const { return m_nStage; }
                // Compared as a signed int (BasketF CGameBasketHungryBossPackun).
                long GetUnk150() const { return m_Unk150; }
                // (name guessed) The team that attacks first: the signed
                // byte at 0x0E (GameWorkReset_ copies it to 0x0F).
                long GetFirstOffenseTeam() const { return (signed char)m_Unk0E; }
                // (fn_80189798, name guessed) Fills the work with the
                // defaults (SetDefaultParam).
                void SetDefault();

                // 0x00 (meaning not known): HockeyF's CFighting compares it
                // with a team number (the fight gauge and the shake icon).
                long m_nUnk00;
                unsigned char m_nGameMode; // 0x04, the GAME_MODE of the running game
                unsigned char m_nGameType; // 0x05, 2 selects the sport's party game
                // 0x06 (meaning not known): with 11 to 13 here the tournament
                // leaves team 0 to the computer as well.
                unsigned char m_nUnk06;
                unsigned char m_nUnk07;    // 0x07, read signed through GetUnk07 (SetCup stores it)
                unsigned char m_nTournamentLevel; // 0x08 (SetTournamentLevel)
                unsigned char m_nRound;           // 0x09 (SetRound)
                // 0x0A, the stage being played (name guessed; the DOL's
                // CEventDemoBase::_Init copies it, and HockeyF tests 0x11
                // before calling CStageHockey17)
                unsigned char m_nStage;
                unsigned char m_Unk0B;
                unsigned char m_nPeriod;   // 0x0C, the period being played; 0 is the first (guess)
                // 0x0D (name guessed): the number of teams; HockeyF makes one
                // shoot marker per team and loops over the team scores with it.
                unsigned char m_nTeamNum;
                unsigned char m_Unk0E;
                // 0x0F (name guessed): a team number, -1 for none; read
                // through GetUserTeam (CComManVolley::CanbeTouch, and the
                // same read in BasketF's CComManBasketGame).
                signed char m_nUserTeam;
                long m_nGameLevel;         // 0x10, 0-3 (EASY, NORMAL, HARD, EXPERT; names guessed)
                long m_nQPanelLevel;       // 0x14 (SetQPanelLevel)
                // 0x18: flag bits (name guessed). BasketF's
                // CStageManBasket::Main_ skips the change-of-offense sound
                // while any of 0x4306 is set; with 0x8000 set the dodge
                // game's question panels stay off (guess).
                unsigned long m_nFlag18;
                // 0x1C, copied from the tournament's work by
                // TournamentRule::SetupGame (meaning not known)
                long m_nUnk1C;
                // 0x20 (meaning not known): BasketF's computer players
                // take a fixed shot time while it is not above 0.
                float m_fUnk20;
                // 0x24 (name and meaning guessed): a game time in frames;
                // HockeyF's fire pillars (stage 86) change their timing
                // when it falls below HOCKEY_PRM_S86_FIRE_TIME1/2 seconds.
                float m_fGameTime;
                // 0x28 (name guessed): the shot clock's length, read by the
                // inline CGame::GetMaxShotTime_ (BossF game/boss/GameBoss.cpp).
                float m_fMaxShotTime;
                unsigned char m_Unk2C[0x30 - 0x2C];
                // 0x30 (name guessed): the shot clock in frames (BasketF's
                // CChrManBasket counts it down and shows it in seconds).
                float m_fShotTime;
                // 0x34 (name guessed): a speed rate of the game. The DOL's
                // goal demo (game/msw/CGameExDemoBase.cpp) sets its own rate
                // here while the demo camera plays and restores it after.
                float m_fGameSpeed;
                // 0x38 (meaning not known): BasketF's party game clears it
                // when a game is set up.
                float m_fUnk38;
                // 0x3C (meaning not known): AddTeamCoin_/UseTeamCoin_ copy it
                // into m_anCoinHistTime of the team.
                long m_nUnk3C;
                // 0x40, flag words whose bit 0 the boss game's "?" panels test
                // (game/boss/CQPanelManBoss.cpp): with bit 0 of 0x40 set a coin
                // panel gives no coin; with bit 0 of 0x1C8 set the panels pop up
                // again without waiting. (meanings and names guessed)
                unsigned long m_nUnk40;    // 0x40
                // 0x44 (meaning not known): VolleyF's ready-start demo passes
                // it with m_Unk0B to the serve telop.
                long m_nUnk44;             // (SetWinPoint stores it)
                // 0x48-0x14F: per-character (8) and per-team (4) settings,
                // read and written by the static Set/Get functions of the
                // same names (DOL, game/msw/CGame.cpp).
                signed char m_anCharType[8];            // 0x48
                unsigned char m_anCharColor[8];         // 0x50
                unsigned char m_anCharExFlag[8];        // 0x58
                unsigned char m_anCharAssignPadNo[8];   // 0x60
                unsigned char m_anCharAssignPlayerNo[8]; // 0x68
                signed char m_anNpcType[8];             // 0x70
                signed char m_anUnk78[4];               // 0x78 (fn_8018A7C0 reads it)
                unsigned char m_anTeamCtrlType[4];      // 0x7C
                unsigned char m_anTeamColorIdx[4];      // 0x80
                // 0x84: the constructor stores each online pad's local pad
                // number here in a Wi-Fi game (fn_8018A828 reads it).
                unsigned char m_anUnk84[4];
                unsigned char m_anGoalIdx[4];           // 0x88
                float m_afTeamComLevel[4];              // 0x8C
                long m_anTeamCoin[4];                   // 0x9C
                unsigned long m_anTeamDisableDropCoin[4]; // 0xAC
                // 0xBC: coins changes counted since 0xCC last ran out (names
                // guessed; IncCoinHistCount counts 0xCC down)
                long m_anCoinHistCount[4];
                long m_anCoinHistTime[4];               // 0xCC
                long m_anTeamItem[4];                   // 0xDC
                long m_anPeriodStartCoin[4];            // 0xEC
                unsigned char m_anTeamResult[4];        // 0xFC
                long m_anPeriodStartScore[4];           // 0x100
                long m_anTeamMaxLife[4];                // 0x110
                long m_anTeamInitLife[4];               // 0x120
                long m_anTeamSkillPoint[4];             // 0x130
                unsigned char m_anTeamRebirthLife[4];   // 0x140
                signed char m_anTeamChrChangeCtrlType[4]; // 0x144
                bool m_abTeamAutoSpikeJump[4];     // 0x148
                unsigned char m_anPassCountLimit[4];    // 0x14C
                // 0x150 (meaning not known): with 1 here BasketF's party
                // game sets bit 0 of CChrManBasket's flags when play starts.
                unsigned char m_Unk150;
                // 0x151 (meaning not known): BasketF's CStageBasket05 starts
                // its heavy raft swing on any turn of the rafts while it is
                // not zero, and otherwise waits for the camera's side.
                unsigned char m_Unk151;
                // 0x152 (name guessed): how many goals the court has
                // (HockeyF CCollManHockey loops over them).
                unsigned char m_nGoalNum;
                unsigned char m_nSportsMode; // 0x153 (SetSportsMode)
                // 0x154-0x17C: the record of the last goal (SetGoal*; names
                // after the setters).
                long m_nGoalTeam;          // 0x154
                long m_nGoalChar;          // 0x158
                long m_nGoalBaseScore;     // 0x15C
                // 0x160: counted by tournament mission 0x3E (HockeyF) and
                // passed to the coin mission (0x4E) check after a basket
                // (BasketF); meaning not known.
                long m_Unk160;             // (SetGoalCoin and SetGoalEffectCoin store it)
                long m_nGoalScore;         // 0x164
                float m_fGoalShootRate;    // 0x168, 0-1 (SetGoalShootType clamps it)
                long m_nGoalShootType;     // 0x16C
                long m_nGoalBonusType;     // 0x170
                long m_nGoalBonusColor;    // 0x174
                long m_nGoalBonusPoint;    // 0x178 (SetGoalBonusRate stores it too)
                long m_nGoalOwnGoal;       // 0x17C
                // 0x180 (meaning not known): while it is zero HockeyF's
                // CFighting shows the fight gauge icons over the fighters.
                long m_nUnk180;
                unsigned char m_Unk184[0x188 - 0x184];
                // 0x188, 0x190 (names guessed): the pads of each team as bits,
                // offline and in an online match (CGame::GetTeamPadBits).
                unsigned short m_anTeamPadBits[4];
                unsigned short m_anTeamOnlinePadBits[4];
                signed char m_anPadDefaultType[4];      // 0x198
                unsigned char m_anGoalTeam[4];          // 0x19C, the team of each goal (SetGoalIdx)
                unsigned char m_anUnk1A0[4];            // 0x1A0 (fn_8018AA28: [a] = b)
                unsigned char m_anUnk1A4[4];            // 0x1A4 (fn_8018AA28: [b] = a)
                unsigned char m_Unk1A8[4];
                long m_nReturnStep;                     // 0x1AC
                unsigned char m_Unk1B0[4];
                long m_nStartFadeType;                  // 0x1B4
                long m_anPartyGameParam[4];             // 0x1B8
                unsigned long m_nUnk1C8;   // 0x1C8
                // 0x1CC: screen split; msw::cockpit::GetViewSplitNum gives 1
                // view for 0, 4 for 3, else 2 (name guessed). The volley coin
                // cockpit places its counters only for 0.
                long m_nViewSplit;
                RFLStoreData m_aMiiStoreData[8];        // 0x1D0, 0x4C each
                // 0x430 (meaning not known): DodgeF's CStageDodge04 places
                // its background at 14 * (m_nUnk430 % 3 - 1) in x.
                long m_nUnk430;            // (SetStageParam stores it)
                long m_nGimmickParamIdx;   // 0x434
                wchar_t m_aCharName[8][0x20]; // 0x438, each character's name
            };

            CGame(bcs::CTask *pParent, GAME_MODE mode);
            virtual ~CGame();
            virtual void Main();
            virtual void Disp();

            // CGame's own virtual functions, 0x24-0x2A4, in the order of the
            // slots of __vt__Q23msw10CGameDodge (DodgeF). CGame's part ends
            // at 0x2A4 (ArtsEventEnd_) in CGameBasket's and CGameBoss's
            // vtables too; each sport's own functions start at 0x2A8. Only
            // the names and argument types are known: every return type here
            // is a placeholder (void) until a caller shows the real one.
            // The inline ones below have bodies because the modules emit them
            // as weak functions (BossF game/boss/GameBoss.cpp, after its own
            // code); their return types are read from those bodies.
            virtual float GetMaxShotTime_() const { return g_GameWork.m_fMaxShotTime; } // 0x024
            virtual void MainFunc();                                     // 0x028
            virtual void DispFunc();                                     // 0x02C
            virtual void FinishFirstLoadData();                          // 0x030
            virtual void GenerateInstance();                             // 0x034
            virtual void PrepareRestartGame_();                          // 0x038
            virtual void SendMessage_(long, long, long, long);           // 0x03C
            virtual int IsLineMsgDispEnable_() { return 1; }             // 0x040
            virtual void CtorStateGameInit();                            // 0x044
            virtual void LoopStateGameInit();                            // 0x048
            virtual void DtorStateGameInit();                            // 0x04C
            virtual void CtorStateGameSetup();                           // 0x050
            virtual void LoopStateGameSetup();                           // 0x054
            virtual void DtorStateGameSetup();                           // 0x058
            virtual void CtorStateWaitStart();                           // 0x05C
            virtual void LoopStateWaitStart();                           // 0x060
            virtual void DtorStateWaitStart();                           // 0x064
            virtual void CtorStateMainLoop();                            // 0x068
            virtual void LoopStateMainLoop();                            // 0x06C
            virtual void DtorStateMainLoop();                            // 0x070
            virtual void CtorStateGoalEvent();                           // 0x074
            virtual void LoopStateGoalEvent();                           // 0x078
            virtual void DtorStateGoalEvent();                           // 0x07C
            virtual void CtorStateTurnResult();                          // 0x080
            virtual void LoopStateTurnResult();                          // 0x084
            virtual void DtorStateTurnResult();                          // 0x088
            virtual void CtorStateTimeUp();                              // 0x08C
            virtual void LoopStateTimeUp();                              // 0x090
            virtual void DtorStateTimeUp();                              // 0x094
            virtual void CtorStateChgCourt();                            // 0x098
            virtual void LoopStateChgCourt();                            // 0x09C
            virtual void DtorStateChgCourt();                            // 0x0A0
            virtual void CtorStateTurnOver();                            // 0x0A4
            virtual void LoopStateTurnOver();                            // 0x0A8
            virtual void DtorStateTurnOver();                            // 0x0AC
            virtual void CtorStateChgOffense();                          // 0x0B0
            virtual void LoopStateChgOffense();                          // 0x0B4
            virtual void DtorStateChgOffense();                          // 0x0B8
            virtual void CtorStateGameSet();                             // 0x0BC
            virtual void LoopStateGameSet();                             // 0x0C0
            virtual void DtorStateGameSet();                             // 0x0C4
            virtual void CtorStateGameResult();                          // 0x0C8
            virtual void LoopStateGameResult();                          // 0x0CC
            virtual void DtorStateGameResult();                          // 0x0D0
            virtual void CtorStateResultMenu();                          // 0x0D4
            virtual void LoopStateResultMenu();                          // 0x0D8
            virtual void DtorStateResultMenu();                          // 0x0DC
            virtual void CtorStateEndMenu();                             // 0x0E0
            virtual void LoopStateEndMenu();                             // 0x0E4
            virtual void DtorStateEndMenu();                             // 0x0E8
            virtual void CtorStateRestart();                             // 0x0EC
            virtual void LoopStateRestart();                             // 0x0F0
            virtual void DtorStateRestart();                             // 0x0F4
            virtual void CtorStateChgField();                            // 0x0F8
            virtual void LoopStateChgField();                            // 0x0FC
            virtual void DtorStateChgField();                            // 0x100
            virtual void CtorStateGameExit();                            // 0x104
            virtual void LoopStateGameExit();                            // 0x108
            virtual void DtorStateGameExit();                            // 0x10C
            virtual void CtorStateEventDemo();                           // 0x110
            virtual void LoopStateEventDemo();                           // 0x114
            virtual void DtorStateEventDemo();                           // 0x118
            virtual void CtorStateExceptDemo();                          // 0x11C
            virtual void LoopStateExceptDemo();                          // 0x120
            virtual void DtorStateExceptDemo();                          // 0x124
            virtual void SetPeriodResult_();                             // 0x128
            virtual void SetGameResult_();                               // 0x12C
            virtual void CalcAddSkillPoint_();                           // 0x130
            virtual void SetMissionResult_();                            // 0x134
            virtual int SetGameResultNext_();                            // 0x138 (int: BossF returns 0)
            virtual void AddTeamCoin_(long, long);                       // 0x13C
            virtual void UseTeamCoin_(long, long);                       // 0x140
            virtual void TrgGoalIn_(long, long, long, long, float, const bcs::FVEC3 &); // 0x144
            virtual void TrgBasketGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, long); // 0x148
            virtual void TrgVolleyGoalIn_(long, long, long, long, long, long, const bcs::FVEC3 &, unsigned long); // 0x14C
            virtual void TrgHockeyGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, unsigned long); // 0x150
            virtual long GetMaxCoin_() const;                            // 0x154 (BossF returns 100)
            virtual void SetDodgeArtsCatchMode_(int, long) {}            // 0x158
            virtual int IsDodgeArtsCatchMode_() const { return 1; }      // 0x15C
            virtual long GetDodgeVariableChrNum_(long) const { return 0; } // 0x160
            virtual long GetDodgeChrMaxLife_(long) const { return 0; }   // 0x164
            virtual void SetPassCountLimit_(long, long);                 // 0x168
            virtual long GetPassCountLimit_(long) const;                 // 0x16C
            virtual int IsNowBonusTime_() const { return 0; }            // 0x170 (int: BasketF CGameBasketHungryBossPackun)
            virtual int IsVolleyScored_() const { return 0; }            // 0x174
            virtual int IsVolleyServeClockOver_() const { return 0; }    // 0x178
            virtual long _GetHockeyResultGoalId() { return -1; }         // 0x17C
            virtual void EntranceDemoSetUp() {}                          // 0x180
            virtual void EventDemoSetUp();                               // 0x184
            virtual int CheckDemoSkipInput_();                          // 0x188
            virtual void TrgArtsEvent_(long);                            // 0x18C
            virtual void StartArtsEvent_(long);                          // 0x190
            virtual void EndArtsEvent_(long);                            // 0x194
            virtual void SetFirstGoalIdx_();                             // 0x198
            virtual long GetFirstGoalIdxMode_();                         // 0x19C (BossF returns 0)
            virtual void ChangeGoalIdx_();                               // 0x1A0
            virtual void ChangeCameraBaseDir_();                         // 0x1A4
            virtual int IsSetOver_();                                    // 0x1A8 (BossF returns 1)
            virtual int IsGameSet_();                                    // 0x1AC (int: BasketF CGameBasketHungryBossPackun)
            virtual void RsrcLoadStart_();                               // 0x1B0
            virtual int RsrcIsLoadEnd_();                               // 0x1B4
            virtual void RsrcInitialize_();                              // 0x1B8
            virtual void RsrcUnLoad_();                                  // 0x1BC
            virtual void CheckBgmChange_();                              // 0x1C0
            virtual float GetTeamArtsGauge_(long);                       // 0x1C4 (float: BossF compares f1)
            virtual void ClearTeamArtsGauge_(long);                      // 0x1C8
            virtual float AddTeamArtsGauge_(long, float, float);          // 0x1CC
            virtual float SetTeamArtsGauge_(long, float, float);          // 0x1D0
            virtual void SetCockpitArtsGaugeRatio_(long, float, float);  // 0x1D4
            virtual void SetCockpitArtsGaugeSpAnim_(long, int);          // 0x1D8
            virtual void SetDispRadarIcon_(int);                         // 0x1DC
            virtual void SetDispChrHeadIcon_(int);                       // 0x1E0
            virtual void SetCockpitPartMove_(long, long);                // 0x1E4
            virtual void SetCockpitFaceIcon_();                          // 0x1E8
            virtual void SetCockpitCoinGetTelopMove_(long, long);        // 0x1EC
            virtual void SetCockpitItemIconDisp_(int, int);              // 0x1F0
            virtual void SetCockpitDisp_(int);                           // 0x1F4
            virtual void SetCockpitCoinDisp_(int);                       // 0x1F8
            virtual void SetCockpitTimeDisp_(int);                       // 0x1FC
            virtual void SetCockpitScoreDisp_(int);                      // 0x200
            virtual void SetCockpitPeriodDisp_(int);                     // 0x204
            virtual void SetCockpitSetCountDisp_(int);                   // 0x208
            virtual void SetCockpitLifeDisp_(int);                       // 0x20C
            virtual void SetCockpitPassBallDisp_(int);                   // 0x210
            virtual void SetCockpitArtsGaugeDisp_(int);                  // 0x214
            virtual void SetCockpitMapDisp_(int);                        // 0x218
            virtual void SetScoreDemoTeamScore_(long, long);             // 0x21C
            virtual void SetScoreDemoTeamCoin_(long, long);              // 0x220
            virtual void TelopCreate_(long, int, long);                  // 0x224
            virtual void TelopEnd_(long);                                // 0x228
            virtual void TelopRemove_(long);                             // 0x22C
            virtual int TelopIsWait_(long);                              // 0x230 (BossF tests r3)
            virtual int TelopIsEnd_(long);                               // 0x234 (int: BasketF CGameBasketHungryBossPackun)
            virtual void PushTeamItem_(long, long);                      // 0x238
            virtual void PopTeamItem_(long);                             // 0x23C
            virtual void TeamItemStartRoulette_(long, long);             // 0x240
            virtual int IsTeamItemRouletteRole_(long);                  // 0x244
            virtual int CanbeTeamItemPush_(long);                       // 0x248
            virtual long GetGoalDemoType_(long, long) const;             // 0x24C
            virtual long GetGoalDemoTelop_(long, long, long, int) const { return -1; } // 0x250
            virtual void StartGoalTelop_() {}                            // 0x254
            virtual int CanbeScoredView_();                             // 0x258
            virtual void StartScoredView_(long);                         // 0x25C
            virtual void EndScoredView_();                               // 0x260
            virtual long GetExceptionalCameraType_() { return 0; }       // 0x264
            virtual void ScoreBookInitCountWork_();                      // 0x268
            virtual void ScoreBookAddCountWork_(long, long, long) {}     // 0x26C
            virtual void ScoreBookApplyCountWork_();                     // 0x270
            virtual int MenuCallGameResult_();                           // 0x274 (int: BasketF CGameBasketHungryBossPackun)
            virtual int MenuCallInterval_();                             // 0x278 (BossF returns 1)
            virtual int MenuCallPause_();                                // 0x27C (BossF returns 1)
            virtual int MenuCallGameEnd_() { return 0; }                 // 0x280
            virtual long MenuChoiceEndMenu_(long) { return -1; }         // 0x284
            virtual long MenuChoiceNextEndMenu_(long) { return -1; }     // 0x288
            virtual long MenuGetResult_();                               // 0x28C
            virtual void CockpitCheckArrowState_() {}                    // 0x290
            virtual void CameraReset_();                                 // 0x294
            virtual void ArtsEventBegin_();                              // 0x298
            virtual float GetArtsEventFrameNum_();                       // 0x29C (the DOL body loads f1)
            virtual int ArtsEventLoop_();                               // 0x2A0
            virtual void ArtsEventEnd_();                                // 0x2A4

            static void SetDefaultParam();
            // (meaning guessed) The task new objects of the running game are
            // created under (BossF's bomb explosions).
            static bcs::CTask *GetTempParent();
            // (DOL, static; argument meanings not known: DodgeF's bomb blasts
            // pass a quake number and -1, -1; HockeyF's CFighting passes
            // (0x2F, -1, -1) when a fight ends)
            static int StartCameraQuake(long nType, long, long);
            // (DOL; meaning of the argument not known, CDropCoinMan passes
            // 1) Non-zero while the game is in its main loop state.
            static int IsMainLoopState(int);
            // (DOL, static; added for HockeyF game/hockey/CChrCoreHockeyLuigi.cpp)
            // The computer level of team nTeam, as CComMan::GetDatF takes it.
            static float GetTeamComLevel(long nTeam);

            // The three below are static in the DOL: their bodies index
            // g_GameWork with the argument. (return types guessed)
            // Which character player nPlayer has: the byte at
            // g_GameWork + 0x48 + nPlayer, read signed. The radar treats 0x13
            // and above differently (most likely the Mii characters; not
            // checked).
            static long GetCharType(long nPlayer);
            // (DOL, static; return type guessed) The character type of Mii
            // nIndex of a source (19 and up). Added for MenuF
            // game/menu/CCourtChr.cpp.
            static long GetMiiChrType(RFLDataSource source, long nIndex);
            // The colour of team nTeam: the byte at g_GameWork + 0x80 +
            // nTeam, read unsigned. The radar uses it as an index of four.
            static long GetTeamColorIdx(long nTeam);
            // (DOL; added for MenuF's msw::CTopMenuChr, which gives team n
            // colour n) Sets the colour of team nTeam.
            static void SetTeamColorIdx(long nTeam, long nColor);
            // The result of team nTeam: the byte at g_GameWork + 0xFC + nTeam,
            // read unsigned. What the values mean is not known;
            // CTmGameHandler::_SetBattleWin takes it and _IsBattleWin tests
            // for 1.
            static long GetTeamResult(long nTeam);
            // The type of NPC slot nSlot: the byte at g_GameWork + 0x70 +
            // nSlot, read signed. (meaning guessed from the name)
            static long GetNpcType(long nSlot);
            // Static in the DOL too (BossF CEventDemoBoss; names from the
            // symbols, return types and parameter names guessed). The colour
            // of player nPlayer's character (byte at g_GameWork + 0x50 +
            // nPlayer); its Mii data (g_GameWork + 0x1D0 + nPlayer * 0x4C);
            // word nIndex of the party game parameters (g_GameWork + 0x1B8);
            // the word at g_GameWork + 0x1B4.
            static long GetCharColor(long nPlayer);
            static void *GetMiiStoreData(long nPlayer);
            static long GetPartyGameParam(long nIndex);
            // DOL, static (return type guessed). How team nTeam is played;
            // BossF's CChrManBoss::InitLeader_ gives the characters of a
            // team of type 1 the computer driver.
            static long GetTeamCtrlType(long nTeam);
            static void SetStartFadeType(long nType);
            // (DOL, static; added for VolleyF CReadyStartDemoVolley; return
            // types and meanings guessed) Not negative when a pad asks to
            // skip the demo (-1: any pad); moves the cockpit parts; the
            // name of player nPlayer's character (for the Mii and other
            // characters from type 0x13 on).
            static long DemoSkipInputCheck(long nPad);
            static void SetCockpitPartMove(long, long);
            static const wchar_t *GetCharName(long nPlayer);
            // (DOL, static; added for DodgeF
            // game/dodge/CCockpitDodgeTelopInOutChange.cpp; return types
            // guessed) The length of character nChr's name, and the name
            // copied into pBuf (nBufLen characters at most).
            static long GetCharNameLen(long nChr);
            static long GetCharName(long nChr, wchar_t *pBuf, long nBufLen);
            // (DOL, static; added for MenuF game/menu/unit_4BB48.cpp; return
            // type guessed) Which team (0 or 1) this console plays in a
            // Wi-Fi game.
            static void SetMyTeamIdx(long nTeam);
            // The difficulty of the game (see GameWork::m_nGameLevel).
            // Inline; the parameter files emit it.
            static long GetGameLevel() { return g_GameWork.m_nGameLevel; }

            // Bit 0x02 of the byte at g_GameWork + 0x58 + nChr: the
            // character is an extra dodgeball outfielder (static in the DOL;
            // meaning guessed from the name; CTmGameHandlerDodge leaves these
            // out of the team counts).
            static int IsCharEx_DodgeOutfielder(long nChr);
            // The score of team nTeam in period nPeriod. (DOL, static; return type guessed)
            static long GetTeamPeriodScore(long nTeam, long nPeriod);
            // The coins of team nTeam: the word at g_GameWork + 0x9C +
            // 4 * nTeam, 0 for a negative team.
            static long GetTeamCoin(long nTeam);
            // The score of team nTeam, from a word array in the DOL's data.
            static long GetTeamScore(long nTeam);
            // Passes a message to the running game's virtual at vtable
            // offset 0x3C, if there is a game.
            static void SendMessage(long nMsg, long, long, long);
            // (DOL, static; added for BossF game/boss/unit_5FF80.cpp,
            // meanings guessed) Empties team nTeam's arts gauge; turns the
            // loose ball mode on or off.
            static void ClearTeamArtsGauge(long nTeam);
            static void SetLooseBallModeFlag(int bOn);
            // (DOL, static; added for BasketF game/basket/CChrManBasket.cpp;
            // return types and parameter meanings guessed) Fades the
            // background in (fFrom, fFrames); the shot clock's length and
            // the setter of the clock (g_GameWork.m_fShotTime); not zero in
            // a tournament; the arts event of player nPlayer begins.
            static void BgSetFadeIn(float fFrom, float fFrames);
            static float GetMaxShotTime();
            static void SetShotTime(float fTime);
            static int IsTournament();
            static void TrgArtsEvent(long nPlayer);
            // The goal team nTeam attacks: the byte at g_GameWork + 0x88 +
            // nTeam, read unsigned. (return type guessed)
            static long GetTargetGoalIdx(long nTeam);
            // (DOL; added for BasketF game/basket/CStageBasket10.cpp; return
            // type guessed) The team whose goal view n shows.
            static long GetViewTargetTeam(long n);
            // DOL. The yaw (radians) of goal nGoal. (added for BasketF
            // CChrDrvPadBasket; meaning guessed from the name)
            static float GetGoalYRot(long nGoal);
            // (DOL, static; return type guessed) The goal of team nTeam
            // (HockeyF: 1 minus it is the goal the team defends).
            static long GetGoalIdx(long nTeam);
            // (DOL, static; meanings guessed from the names and from
            // BasketF's CStageBasket03) The goal team nTeam guards, and the
            // team that goal nGoal belongs to.
            static long GetGuardGoalIdx(long nTeam);
            static long GetTeamIdxByGoalIdx(long nGoal);
            // (DOL, static; meaning guessed from the name) Not zero when the
            // leader of team nTeam may be changed by the player
            // (CChrManDodge::Util_SearchChgLeader_).
            static int IsPlayTypeCanChangeLeader(long nTeam);
            // (DOL, static) The position of goal nGoal. (return type guessed)
            static const bcs::FVEC3 &GetGoalPos(long nGoal);
            // DOL, static. The direction goal nGoal faces (out of the goal,
            // in the ground plane; HockeyF CCollManHockey).
            static const bcs::FVEC3 &GetGoalDir(long nGoal);
            // (DOL, static; argument meanings guessed from BasketF's
            // CBallBasketFruit::TrgGoalIn_: shooter, team, shoot type, ball
            // kind, a float, the goal's position)
            static void TrgGoalIn(long nChr, long nTeam, long nShootType, long nKind, float, const bcs::FVEC3 &rGoalPos);
            // Whether player nPlayer's character is the hockey keeper (bit 1
            // of the byte at g_GameWork + 0x58 + nPlayer). (return type guessed)
            static int IsCharEx_HockeyKeeper(long nPlayer);
            // (added for HockeyF game/hockey/CCockpitHockeyVSCount.cpp; return
            // type guessed) Whether character nChr is a star character.
            static int IsCharEx_StarChar(long nChr);
            // (name guessed; the DOL's fn_8018C300, after SetPadDefaultType)
            // The byte at g_GameWork + 0x198 + nPad, read signed. The
            // cockpit's special-shot input prompt reads 1 as a nunchaku, 0 as
            // a remote alone and -1 as "ask CMenuPad".
            static long GetPadDefaultType(long nPad);

            // Not zero when team nTeam may still take an item. (DOL)
            static int CanbeTeamItemPush(long nTeam);
            // DOL, static. CItemBallChangeEffect's constructor passes 60.0,
            // 16.0 and 0. (parameter meanings not known)
            static void StartStopEffect(float, float, long);
            // DOL, static (DodgeF CEventDemoDodge; return types and parameter
            // names guessed). The full life of a character of team nTeam;
            // the slow-motion effect (start with four parameters, stop, and
            // whether one is running).
            static long GetTeamMaxLife(long nTeam);
            static unsigned long StartSlowEffect(float, float, float, float);
            static void StopSlowEffect();
            static int IsNowSlowEffect();
            // DOL, static. Not zero while an arts (special move) event plays (guess).
            static int IsArtsEventMode();
            // DOL, static (added for BasketF game/basket/CChrCoreBasketSaboten.cpp;
            // meaning guessed from the name): not zero while an arts event is
            // asked for and has not started yet.
            static int IsReqArtsEvent();
            // (DOL, static; added for BossF game/boss/CChrCoreBossMethod.cpp;
            // meanings guessed from the names) The arts gauge of team nTeam
            // (1.0 is full); puts the leaders' faces on the cockpit again.
            static float GetTeamArtsGauge(long nTeam);
            static void SetCockpitFaceIcon();
            // Added for VolleyF game/volley/CChrManVolley.cpp (DOL, static;
            // return types guessed). SetOffenseTeam makes team nTeam the one
            // attacking (CChrManVolley::StartArtsEvent_ passes the team of the
            // character starting a special move); IsVolleyScored is not zero
            // once the rally has ended in a point.
            static void SetOffenseTeam(long nTeam);
            // (DOL; added for BasketF game/basket/CNpcGhostBasketBase.cpp,
            // meaning guessed) Switches the loose ball mode on or off.
            static void SetLooseBallMode(int bOn);
            static int IsVolleyScored();
            // (fn_8017D558, unnamed in the DOL) IsNowBonusTime_ on the
            // running game, zero without one.
            static int IsNowBonusTime();
            // Added for the DOL's game/msw/CGameExDemoBase.cpp (all static
            // in the DOL; names after the virtuals they call, return types
            // and parameter meanings guessed).
            // Stops camera quake nQuake (-1: all) on the active cameras.
            static void StopCameraQuake(long nQuake);
            // (fn_8017EB18, unnamed in the DOL) Stops every quake of the
            // game's own four cameras.
            static void StopAllCameraQuake();
            // (fn_8017E820, fn_8017E880, fn_8017E8A8, unnamed in the DOL)
            // TelopCreate_, TelopRemove_ and TelopIsEnd_ on m_pInstance;
            // TelopIsEnd gives 1 without an instance.
            static void TelopCreate(long nTelop, int, long);
            static void TelopRemove(long nTelop);
            static int TelopIsEnd(long nTelop);
            // (fn_8017F0FC, unnamed in the DOL; name guessed) IsLineMsgDispEnable_
            // on m_pInstance; 0 without an instance.
            static int IsLineMsgDispEnable();
            // The speed lines effect over the screen for fFrame frames, and
            // (fn_8018BED0, unnamed in the DOL) its early end.
            static void StartSaturatedLinework(float fFrame);
            static void EndSaturatedLinework();
            // Adds nCoin coins to team nTeam. (DOL)
            static void AddTeamCoin(long nTeam, long nCoin);

            // Setters of g_GameWork, all static in the DOL (TourF
            // TournamentRule sets a whole game up with them). Parameter
            // names guessed; the return types are void as no caller reads
            // them.
            static void SetGameMode(GAME_MODE mode);
            static void SetGameType(GAME_TYPE type);
            static void SetSportsMode(SPORTS_MODE mode);
            static void SetCup(GAME_CUP cup);
            static void SetTournamentLevel(GAME_TOURNAMENT_LEVEL level);
            static void SetGameLevel(GAME_LEVEL level);
            static void SetQPanelLevel(QPANEL_LEVEL level);
            static void SetPlayType(GAME_PLAY_TYPE type);
            static void SetStage(long nStage);
            static void SetStageParam(long nParam);
            static void SetRound(long nRound);
            static void SetReturnStep(long nStep);
            static void SetParamSemiAuto();
            static void SetGimmickParamIdx(long nIdx);
            static void SetMaxTime(float fTime);
            static void SetMaxShotTime(float fTime);
            static void SetMaxPeriod(long nPeriod);
            static void SetWinPoint(long nPoint);
            static void SetPartyGameParam(long nIndex, long nValue);
            static void EnableRuleOptionFlag(unsigned long nFlag, int bEnable);
            static void SetCharType(long nChr, long nType);
            static void SetCharColor(long nChr, long nColor);
            static void SetCharExFlag(long nChr, unsigned long nFlag);
            static void ResetCharExFlag(long nChr, unsigned long nMask);
            static void SetCharAssignPlayerNo(long nChr, long nPlayer);
            static void SetCharAssignPadNo(long nChr, long nPad);
            static void SetCharName(long nChr, const wchar_t *pName, long nLen);
            static void SetMiiStoreData(long nChr, const RFLStoreData &data);
            static void SetTeamCtrlType(long nTeam, long nType);
            static void SetTeamChrChangeCtrlType(long nTeam, long nType);
            static void SetTeamComLevel(long nTeam, float fLevel);
            static void SetGoalIdx(long nTeam, long nGoal);
            static void SetPeriodStartScore(long nTeam, long nScore);
            static void SetPeriodStartCoin(long nTeam, long nCoin);
            static void SetTeamInitLife(long nTeam, long nLife);
            static void SetTeamMaxLife(long nTeam, long nLife);
            static void SetTeamRebirthLife(long nTeam, long nLife);

            // The rest of the static functions of game/msw/CGame.cpp (DOL;
            // names from the symbols unless marked, return types guessed
            // where no caller reads them).
            static void TrgBasketGoalIn(long nChr, long nTeam, long nShootType, long nKind, float,
                                        const bcs::FVEC3 &rGoalPos, long);
            static void TrgVolleyGoalIn(long, long, long, long, long, long, const bcs::FVEC3 &, unsigned long);
            static void TrgHockeyGoalIn(long nChr, long nTeam, long nShootType, long nKind, float,
                                        const bcs::FVEC3 &rGoalPos, unsigned long);
            static int IsVolleyServeClockOver();
            static void BgSetFadeOut(float fFrom, float fFrames);
            static void ChangeCameraBaseDir();        // fn_8017DF70 (name guessed)
            static void SetGamePauseUnk38(long n);    // fn_8017DF50 (name guessed)
            static long GetExceptionalCameraType();
            static void TelopEnd(long nTelop);        // fn_8017E858 (name guessed)
            static int IsTourFinalMatch();
            static CGameCamera *GetGameCamera(long nView); // fn_8017EB78 (name guessed)
            static int IsVsMode();
            static int IsLocalVsMode();               // fn_80189CD8 (name guessed)
            static int IsExistGoalRing();
            static long GetGimmickParamIdx();         // fn_80189FE0 (name guessed)
            static void SetNowPeriod(long nPeriod);
            enum GAME_TURN_START_MODE {
                GAME_TURN_START_MODE_0 = 0 // (values not known; GameWorkReset_ passes 0)
            };
            static void SetTurnStartMode(GAME_TURN_START_MODE mode);
            static void SetChgFieldDemoMode(int bOn);
            static void SetFlag2000Mode(int bOn);     // fn_8018A174 (name guessed)
            static void SetVolleyServeMode(int bOn);
            static void SetDodgeArtsCatchMode(int bOn, long);
            static void SetReboundMode(int bOn);
            static void SetGoalEventMode(int bOn);
            static void SetGoalFinishMode(int bOn);
            static void AddCharExFlag(long nChr, unsigned long nFlag);
            static long GetTeamUnk78(long nTeam);     // fn_8018A7C0 (name guessed)
            static long GetLocalPadNo(long nPad);     // fn_8018A828 (name guessed)
            static const bcs::FVEC3 &GetBoardPos(long nGoal);
            static const bcs::FVEC3 &GetBoardDir(long nGoal);
            static void SetViewTeam(long nView, long nTeam); // fn_8018AA28 (name guessed)
            static void SetGoalBaseScore(long n);
            static void SetGoalBonusRate(long n);
            static void SetGoalBonusColor(long n);
            static void SetGoalShootType(long nType, float fRate);
            static void SetGoalTeam(long nTeam);
            static void SetGoalChar(long nChr);
            static void SetGoalOwnGoal(int bOwnGoal);
            static long GetPeriodStartScore(long nTeam);
            static long GetTeamInitLife(long nTeam);
            static long GetTeamRebirthLife(long nTeam);
            static long GetPeriodStartCoin(long nTeam);
            static void ClearTeamCoin(long nTeam);
            static unsigned long GetTeamDisableDropCoin(long nTeam);
            static void ClearTeamDisableDropCoin(long nTeam);
            static void SetTeamDisableDropCoin(long nTeam, unsigned long nFlag);
            static void UseTeamCoin(long nTeam, long nCoin);
            static void ClearTeamItem(long nTeam);    // fn_8018B09C (name guessed)
            static long GetTeamItem(long nTeam, long);
            static void PushTeamItem(long nTeam, long nItem);   // fn_8018B0D0 (name guessed)
            static long PopTeamItem(long nTeam);      // fn_8018B100 (name guessed)
            static void TeamItemStartRoulette(long nTeam, long); // fn_8018B168 (name guessed)
            static float AddTeamArtsGauge(long nTeam, float, float);
            static float SetTeamArtsGauge(long nTeam, float, float);
            static long GetCharAssignPlayerNo(long nChr);
            static long GetCharAssignPadNo(long nChr);
            static void SetPadDefaultType(long nPad, long nType);
            static long GetTeamChrChangeCtrlType(long nTeam);
            static void SetTeamAutoSpikeJumpType(long nTeam, int bAuto);
            static long GetTeamAutoSpikeJumpType(long nTeam);
            static void SetTeamSkillPoint(long nTeam, long nPoint);
            static long GetMyTeamSkillPoint();        // fn_8018C404 (name guessed)
            static void SetRuleOptionFlag(unsigned long nFlag);
            static void SetCockpitCoinGetTelopMove(long, long); // fn_8018C5B8 (name guessed)
            static long GetMiiChrType(RFLStoreData &rData);
            static void SetTeamPeriodScore(long nTeam, long nPeriod, long nScore);
            static void SetTeamCoin(long nTeam, unsigned long nCoin);
            static void AddTeamGainCoin(long nTeam, long nCoin);
            static CGameSpeedCtrl *CreateSpeedCtrl();
            static unsigned long GetTeamPadBits(long nTeam);     // fn_80187C60 (name guessed)
            static long GetMiiName(RFLStoreData &rData, wchar_t *pBuf, long nBufLen); // fn_8018C1CC (name guessed)
            static void SetAllTeamAutoSpikeJumpType(int bAuto); // fn_8018C36C (name guessed)
            static long GetMaxCoin() { return m_pInstance->GetMaxCoin_(); }

            // The score of each team and period (lbl_804D6E78, 0x1B0 bytes;
            // cleared by SetDefaultParam and GameWorkReset_). Only the parts
            // read here are named.
            struct ScoreWork {
                long m_anScore[4];               // 0x00 (GetTeamScore)
                long m_anTotalCoin[4];           // 0x10 (SetPeriodResult_, at most 9999)
                long m_aanPeriodScore[4][5];     // 0x20 (GetTeamPeriodScore)
                unsigned char m_Unk70[0x1B0 - 0x70];
            };
            static ScoreWork g_ScoreWork;

        protected:
            // (name guessed) Starts quake nQuake on the game's cameras.
            int StartCameraQuake_(long nQuake, long nChr, long nView);
            void ResetArtsEventPauseBit_(unsigned long nBit);
            int CheckGoalDemoRnd_(long nRate) const;
            void StartGoalDemo_(long nType, long nTeam, const bcs::FVEC3 &rPos, long nSide, long nTelop, float fSpeed);
            int IsEndGoalDemo_();
            void EndGoalDemo_();
            // (name guessed) Ends the speed lines, if they run.
            void EndSaturatedLinework_()
            {
                if (m_nSatLineId) {
                    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nSatLineId);
                    m_nSatLineId = 0;
                    if (pTask && !pTask->IsDelete()) {
                        pTask->Delete();
                    }
                }
            }
            // (fn_8017D7A8, name guessed) Runs the background fade each frame.
            void BgFadeUpdate_();
            static int IsPartyGame() { return g_GameWork.m_nGameType == GAME_TYPE_PARTY; }

        public:

            static GameWork g_GameWork;
            // (DOL) Pause bits; msw::CChrMarker::Main does nothing while
            // bit 2 is set, the boss ball (BossF CBallBoss) tests 0x4.
            // (meaning of the bits not known)
            static unsigned long g_uPauseBit;
            static CGame *m_pInstance;

            // Added for BasketF game/basket/CGameBasketHungryBossPackun.cpp
            // (static in the DOL; return types and parameter names guessed).
            static void AddTeamPeriodScore(long nTeam, long nPeriod, long nScore);
            static void SetTeamResult(long nTeam, long nResult);
            static void ClearCoinHistory();
            static void SetGoalCoin(long n);
            static void SetGoalEffectCoin(long n);
            static void SetGoalScore(long n);
            static void SetGoalBonusPoint(long n);
            static void SetGoalBonusType(long n);
            static void SetJumpBallMode(int bOn);
            static void SetPreparationMode(int bOn);
            // Counters advanced by fRate each frame of the main loop.
            static void IncBallModeCounter(float fRate);
            static void IncCoinHistCount(float fRate);

        protected:
            // Added for BasketF game/basket/CGameBasketHungryBossPackun.cpp
            // (DOL member functions; return types and meanings guessed).
            void KillTempChild_();
            void BgResetFade_();
            int IsScrnFadeEnd_(unsigned long nMask) const;
            void SetTurnStartTelopType_(long nType, float fFrame);
            // Counts m_fStateCount down; not zero once it has run out.
            int StateWorkDecStateCount_();
            void ClearGameResult_();
            // Static: the party game calls it without an object.
            static void SetGameResultAll_(long nResult);

            // Added for BossF game/boss/GameBoss.cpp (DOL member functions;
            // return types and parameter meanings guessed). The screen fade
            // of the layers in nMask (nType, over fFrame frames); whether
            // pausing is allowed; the slow-motion effect (start, stop); a
            // black screen; the game work's per-game reset; the music of the
            // game result; not zero while an arts event is asked for (the
            // word at 0xEC).
            void ScrnFadeStart_(unsigned long nMask, long nType, float fFrame);
            void SetGamePauseEnable_(int bEnable);
            unsigned long StartSlowEffect_(float, float, float, float);
            void StopSlowEffect_();
            void ScrnSetBlackOut_();
            void GameWorkReset_();
            void PlayGameResultBGM_();
            int IsReqArtsEvent_();
            // (non-virtual; CGameBoss's virtual of the same name calls it)
            void LoopStateEntranceDemo();

        public:
            // Added for BossF game/boss/GameBoss.cpp (DOL, static; names
            // from the symbols, meanings guessed). The ready demo, result
            // and replay modes; the game time left; the record of the last
            // goal.
            static void SetReadyDemoMode(int bOn);
            static void SetResultMode(int bOn);
            static void SetReplayMode(int bOn);
            static void SetTime(float fTime);
            static void ClearLastGoalInfo();
            // The character whose arts (special shot) event runs.
            static long g_nEventChrIdx;

        protected:
            // The constructor writes up to 0x14C, so the class is at least
            // 0x150 bytes; its real size is not known. 0x34-0x94 is the
            // bcs::CFsmQueue<CGame> base. Names of the fields below are
            // guesses from BasketF's CGameBasketHungryBossPackun.
            // 0x94: the game camera of each view (BossF's GenerateInstance
            // fills them from CGameCamera::GetInstance).
            CGameCamera *m_apCamera[4];
            // 0xA4 (names guessed): the pause controller task the
            // constructor creates (0x80 bytes); its word at 0x34 allows
            // pausing (SetGamePauseEnable_).
            CGamePauseTask *m_pPauseTask;
            // 0xA8: the hit-stop task (0x44 bytes, StartStopEffect).
            CGameStopTask *m_pStopTask;
            // 0xAC: the parent of the tasks that run during an arts event
            // (0x48 bytes; ArtsEventBegin_ empties it).
            CGameTaskGroup *m_pArtsTaskRoot;
            unsigned long m_nGoalDemoId; // 0xB0, the goal demo task's id (StartGoalDemo_)
            unsigned long m_nSatLineId;  // 0xB4, the speed lines' task id (StartSaturatedLinework)
            // 0xB8: the parent of the game's short-lived tasks (GetTempParent).
            bcs::CTask *m_pTempParent;
            // 0xBC: the id of the slow-motion task (a CGameSpeedCtrl), 0 for none.
            unsigned long m_nSlowTaskId;
            unsigned long m_nScoredViewId; // 0xC0, the scored view's task id
            // 0xC4 (name guessed): a task of the arts event; BossF's
            // EndArtsEvent_ deletes it.
            bcs::CTask *m_pArtsEventTask;
            // 0xC8 (name guessed): set when the game exits
            // (CtorStateGameExit); Main stops calling MainFunc then.
            long m_bExit;
            long m_nUnkCC;        // 0xCC
            long m_nNextState;    // 0xD0, the state the wait-start state goes on to
            long m_nStateStep;    // 0xD4, the step inside the running state
            float m_fStateCount;  // 0xD8, frames left (StateWorkDecStateCount_)
            // 0xDC: flag bits; bit 0 stops goals from scoring, bit 2 means
            // the game is set.
            unsigned long m_nGameFlag;
            // 0xE0-0xE8 (names guessed): the background fade: its mode (10
            // fading in, 20 fading out, 0 none), the frames to wait before it
            // starts and the step per frame (BgSetFadeIn/BgSetFadeOut).
            long m_nBgFadeMode;
            float m_fBgFadeWait;
            float m_fBgFadeStep;
            // 0xEC: 1 while an arts event has been asked for (TrgArtsEvent_).
            long m_nReqArtsEvent;
            float m_fArtsEventFrame; // 0xF0 (GetArtsEventFrameNum_)
            long m_nArtsEventStep;   // 0xF4
            // 0xF8 (name guessed): the online synchronisation step of the
            // arts event (ArtsEventLoop_), 0 when done.
            long m_nArtsEventSync;
            long m_nTurnStartTelopType;  // 0xFC (SetTurnStartTelopType_)
            float m_fTurnStartTelopFrame; // 0x100
            // 0x104, 0x108 (meanings not known): BossF's GameInit_ sets 1 and
            // 0; while 0x104 is above 0 the game setup queues the event demo
            // (state 19) with 0x108 in its work.
            long m_nUnk104;
            long m_nUnk108;
            float m_afTeamArtsGauge[4]; // 0x10C (GetTeamArtsGauge_)
            long m_nUnk11C;       // 0x11C (meaning not known; cleared by BossF's GameInit_)
            // 0x120-0x128: the score book's counters (ScoreBookInitCountWork_).
            long m_anScoreBookCount[3];
            // 0x12C (meaning not known): BossF's SendMessage_ stores 1 or 2
            // with messages 0x20 and 0x21, and the message's value in 0x130.
            long m_nUnk12C;
            long m_Unk130;        // 0x130, the game is set while above 0 (meaning not known)
            // 0x134 (meaning not known): a 0x28-byte object the constructor
            // creates (fn_8026B418) and the finalizer destroys.
            void *m_pUnk134;
            // 0x138: each team's item roulette (its word at 0x44 is the
            // roulette's state; names guessed).
            CItemRoulette *m_apItemRoulette[4];
            long m_nUnk148;       // 0x148 (meaning not known; BossF's game setup sets 1)
            unsigned char m_Unk14C[0x150 - 0x14C];
    };
}

#endif /* !CGAME_HPP_ */
