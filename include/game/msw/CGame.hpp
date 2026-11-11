#ifndef CGAME_HPP_
#define CGAME_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/CTask.hpp"
#include "revolution/rfl.h"

namespace msw {

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

                // 0x00 (meaning not known): HockeyF's CFighting compares it
                // with a team number (the fight gauge and the shake icon).
                long m_nUnk00;
                unsigned char m_nGameMode; // 0x04, the GAME_MODE of the running game
                unsigned char m_nGameType; // 0x05, 2 selects the sport's party game
                // 0x06 (meaning not known): with 11 to 13 here the tournament
                // leaves team 0 to the computer as well.
                unsigned char m_nUnk06;
                unsigned char m_nUnk07;    // 0x07, read signed through GetUnk07
                unsigned char m_Unk08[2];
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
                unsigned char m_Unk14[0x18 - 0x14];
                // 0x18: flag bits (name guessed). BasketF's
                // CStageManBasket::Main_ skips the change-of-offense sound
                // while any of 0x4306 is set; with 0x8000 set the dodge
                // game's question panels stay off (guess).
                unsigned long m_nFlag18;
                // 0x1C, copied from the tournament's work by
                // TournamentRule::SetupGame (meaning not known)
                long m_nUnk1C;
                unsigned char m_Unk20[0x34 - 0x20];
                // 0x34 (name guessed): a speed rate of the game. The DOL's
                // goal demo (game/msw/CGameExDemoBase.cpp) sets its own rate
                // here while the demo camera plays and restores it after.
                float m_fGameSpeed;
                unsigned char m_Unk38[0x40 - 0x38];
                // 0x40, flag words whose bit 0 the boss game's "?" panels test
                // (game/boss/CQPanelManBoss.cpp): with bit 0 of 0x40 set a coin
                // panel gives no coin; with bit 0 of 0x1C8 set the panels pop up
                // again without waiting. (meanings and names guessed)
                unsigned long m_nUnk40;    // 0x40
                unsigned char m_Unk44[0x151 - 0x44];
                // 0x151 (meaning not known): BasketF's CStageBasket05 starts
                // its heavy raft swing on any turn of the rafts while it is
                // not zero, and otherwise waits for the camera's side.
                unsigned char m_Unk151;
                // 0x152 (name guessed): how many goals the court has
                // (HockeyF CCollManHockey loops over them).
                unsigned char m_nGoalNum;
                unsigned char m_Unk153[0x160 - 0x153];
                // 0x160: counted by tournament mission 0x3E (HockeyF) and
                // passed to the coin mission (0x4E) check after a basket
                // (BasketF); meaning not known.
                long m_Unk160;
                unsigned char m_Unk164[0x180 - 0x164];
                // 0x180 (meaning not known): while it is zero HockeyF's
                // CFighting shows the fight gauge icons over the fighters.
                long m_nUnk180;
                unsigned char m_Unk184[0x1C8 - 0x184];
                unsigned long m_nUnk1C8;   // 0x1C8
                // 0x1CC: screen split; msw::cockpit::GetViewSplitNum gives 1
                // view for 0, 4 for 3, else 2 (name guessed). The volley coin
                // cockpit places its counters only for 0.
                long m_nViewSplit;
                unsigned char m_Unk1D0[0x430 - 0x1D0];
                // 0x430 (meaning not known): DodgeF's CStageDodge04 places
                // its background at 14 * (m_nUnk430 % 3 - 1) in x.
                long m_nUnk430;
                unsigned char m_Unk434[0x638 - 0x434];
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
            virtual void GetMaxShotTime_() const;                        // 0x024
            virtual void MainFunc();                                     // 0x028
            virtual void DispFunc();                                     // 0x02C
            virtual void FinishFirstLoadData();                          // 0x030
            virtual void GenerateInstance();                             // 0x034
            virtual void PrepareRestartGame_();                          // 0x038
            virtual void SendMessage_(long, long, long, long);           // 0x03C
            virtual void IsLineMsgDispEnable_();                         // 0x040
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
            virtual void SetGameResultNext_();                           // 0x138
            virtual void AddTeamCoin_(long, long);                       // 0x13C
            virtual void UseTeamCoin_(long, long);                       // 0x140
            virtual void TrgGoalIn_(long, long, long, long, float, const bcs::FVEC3 &); // 0x144
            virtual void TrgBasketGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, long); // 0x148
            virtual void TrgVolleyGoalIn_(long, long, long, long, long, long, const bcs::FVEC3 &, unsigned long); // 0x14C
            virtual void TrgHockeyGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, unsigned long); // 0x150
            virtual void GetMaxCoin_() const;                            // 0x154
            virtual void SetDodgeArtsCatchMode_(int, long);              // 0x158
            virtual void IsDodgeArtsCatchMode_() const;                  // 0x15C
            virtual void GetDodgeVariableChrNum_(long) const;            // 0x160
            virtual void GetDodgeChrMaxLife_(long) const;                // 0x164
            virtual void SetPassCountLimit_(long, long);                 // 0x168
            virtual void GetPassCountLimit_(long) const;                 // 0x16C
            virtual void IsNowBonusTime_() const;                        // 0x170
            virtual void IsVolleyScored_() const;                        // 0x174
            virtual void IsVolleyServeClockOver_() const;                // 0x178
            virtual void _GetHockeyResultGoalId();                       // 0x17C
            virtual void EntranceDemoSetUp();                            // 0x180
            virtual void EventDemoSetUp();                               // 0x184
            virtual void CheckDemoSkipInput_();                          // 0x188
            virtual void TrgArtsEvent_(long);                            // 0x18C
            virtual void StartArtsEvent_(long);                          // 0x190
            virtual void EndArtsEvent_(long);                            // 0x194
            virtual void SetFirstGoalIdx_();                             // 0x198
            virtual void GetFirstGoalIdxMode_();                         // 0x19C
            virtual void ChangeGoalIdx_();                               // 0x1A0
            virtual void ChangeCameraBaseDir_();                         // 0x1A4
            virtual void IsSetOver_();                                   // 0x1A8
            virtual void IsGameSet_();                                   // 0x1AC
            virtual void RsrcLoadStart_();                               // 0x1B0
            virtual void RsrcIsLoadEnd_();                               // 0x1B4
            virtual void RsrcInitialize_();                              // 0x1B8
            virtual void RsrcUnLoad_();                                  // 0x1BC
            virtual void CheckBgmChange_();                              // 0x1C0
            virtual void GetTeamArtsGauge_(long);                        // 0x1C4
            virtual void ClearTeamArtsGauge_(long);                      // 0x1C8
            virtual void AddTeamArtsGauge_(long, float, float);          // 0x1CC
            virtual void SetTeamArtsGauge_(long, float, float);          // 0x1D0
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
            virtual void TelopIsWait_(long);                             // 0x230
            virtual void TelopIsEnd_(long);                              // 0x234
            virtual void PushTeamItem_(long, long);                      // 0x238
            virtual void PopTeamItem_(long);                             // 0x23C
            virtual void TeamItemStartRoulette_(long, long);             // 0x240
            virtual void IsTeamItemRouletteRole_(long);                  // 0x244
            virtual void CanbeTeamItemPush_(long);                       // 0x248
            virtual void GetGoalDemoType_(long, long) const;             // 0x24C
            virtual void GetGoalDemoTelop_(long, long, long, int) const; // 0x250
            virtual void StartGoalTelop_();                              // 0x254
            virtual void CanbeScoredView_();                             // 0x258
            virtual void StartScoredView_(long);                         // 0x25C
            virtual void EndScoredView_();                               // 0x260
            virtual void GetExceptionalCameraType_();                    // 0x264
            virtual void ScoreBookInitCountWork_();                      // 0x268
            virtual void ScoreBookAddCountWork_(long, long, long);       // 0x26C
            virtual void ScoreBookApplyCountWork_();                     // 0x270
            virtual void MenuCallGameResult_();                          // 0x274
            virtual void MenuCallInterval_();                            // 0x278
            virtual void MenuCallPause_();                               // 0x27C
            virtual void MenuCallGameEnd_();                             // 0x280
            virtual void MenuChoiceEndMenu_(long);                       // 0x284
            virtual void MenuChoiceNextEndMenu_(long);                   // 0x288
            virtual void MenuGetResult_();                               // 0x28C
            virtual void CockpitCheckArrowState_();                      // 0x290
            virtual void CameraReset_();                                 // 0x294
            virtual void ArtsEventBegin_();                              // 0x298
            virtual void GetArtsEventFrameNum_();                        // 0x29C
            virtual void ArtsEventLoop_();                               // 0x2A0
            virtual void ArtsEventEnd_();                                // 0x2A4

            static void SetDefaultParam();
            // (meaning guessed) The task new objects of the running game are
            // created under (BossF's bomb explosions).
            static bcs::CTask *GetTempParent();
            // (DOL, static; argument meanings not known: DodgeF's bomb blasts
            // pass a quake number and -1, -1; HockeyF's CFighting passes
            // (0x2F, -1, -1) when a fight ends)
            static void StartCameraQuake(long nType, long, long);
            // (DOL; meaning of the argument not known, CDropCoinMan passes
            // 1) Non-zero while the game is in its main loop state.
            static int IsMainLoopState(int);

            // The three below are static in the DOL: their bodies index
            // g_GameWork with the argument. (return types guessed)
            // Which character player nPlayer has: the byte at
            // g_GameWork + 0x48 + nPlayer, read signed. The radar treats 0x13
            // and above differently (most likely the Mii characters; not
            // checked).
            static long GetCharType(long nPlayer);
            // The colour of team nTeam: the byte at g_GameWork + 0x80 +
            // nTeam, read unsigned. The radar uses it as an index of four.
            static long GetTeamColorIdx(long nTeam);
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
            // The goal team nTeam attacks: the byte at g_GameWork + 0x88 +
            // nTeam, read unsigned. (return type guessed)
            static long GetTargetGoalIdx(long nTeam);
            // (DOL, static; return type guessed) The goal of team nTeam
            // (HockeyF: 1 minus it is the goal the team defends).
            static long GetGoalIdx(long nTeam);
            // (DOL, static; meanings guessed from the names and from
            // BasketF's CStageBasket03) The goal team nTeam guards, and the
            // team that goal nGoal belongs to.
            static long GetGuardGoalIdx(long nTeam);
            static long GetTeamIdxByGoalIdx(long nGoal);
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
            static void StartSlowEffect(float, float, float, float);
            static void StopSlowEffect();
            static int IsNowSlowEffect();
            // DOL, static. Not zero while an arts (special move) event plays (guess).
            static int IsArtsEventMode();
            // Added for VolleyF game/volley/CChrManVolley.cpp (DOL, static;
            // return types guessed). SetOffenseTeam makes team nTeam the one
            // attacking (CChrManVolley::StartArtsEvent_ passes the team of the
            // character starting a special move); IsVolleyScored is not zero
            // once the rally has ended in a point.
            static void SetOffenseTeam(long nTeam);
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

            static GameWork g_GameWork;
            static CGame *m_pInstance;

        private:
            // The constructor writes up to 0x14C, so the class is at least
            // 0x150 bytes; its real size is not known. 0x34-0x94 is the
            // bcs::CFsmQueue<CGame> base.
            unsigned char m_Unk94[0x150 - 0x94];
    };
}

#endif /* !CGAME_HPP_ */
