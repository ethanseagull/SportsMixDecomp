#ifndef CGAMEVOLLEY_HPP_
#define CGAMEVOLLEY_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Both live in module VolleyF.

    // Size 0x25C. The overrides and the class's own virtual functions
    // (0x2A8-0x2C0) are read from __vt__Q23msw11CGameVolley (VolleyF);
    // return types follow CGame's declarations. The members are named from
    // VolleyF's game/volley/CGameVolleyVsBoss.cpp; names are guesses.
    class CGameVolley : public CGame {
        public:
            CGameVolley(bcs::CTask *pParent);
            virtual ~CGameVolley();

            // Overrides of CGame's virtual functions, in vtable order.
            virtual void MainFunc();
            virtual void DispFunc();
            virtual void FinishFirstLoadData();
            virtual void GenerateInstance();
            virtual void PrepareRestartGame_();
            virtual void SendMessage_(long nMsg, long, long, long);
            virtual int IsLineMsgDispEnable_();
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
            virtual void CtorStateExceptDemo();
            virtual void LoopStateExceptDemo();
            virtual void DtorStateExceptDemo();
            virtual void SetGameResult_();
            virtual void CalcAddSkillPoint_();
            virtual int SetGameResultNext_();
            virtual void AddTeamCoin_(long nTeam, long nCoin);
            virtual void TrgVolleyGoalIn_(long nChr, long nTeam, long nShootType, long nScore, long nCoin, long nPoint,
                                          const bcs::FVEC3 &rPos, unsigned long nFlag);
            virtual long GetMaxCoin_() const;
            virtual int IsVolleyScored_() const;
            virtual int IsVolleyServeClockOver_() const;
            virtual void EventDemoSetUp();
            virtual void StartArtsEvent_(long nChr);
            virtual void EndArtsEvent_(long nChr);
            virtual long GetFirstGoalIdxMode_();
            virtual int IsSetOver_();
            virtual int IsGameSet_();
            virtual void SetCockpitArtsGaugeRatio_(long nTeam, float fRatio, float fTelop);
            virtual void SetCockpitArtsGaugeSpAnim_(long nTeam, int bSpecial);
            virtual void SetDispRadarIcon_(int bDisp);
            virtual void SetCockpitPartMove_(long nPart, long nMove);
            virtual void SetCockpitCoinGetTelopMove_(long, long);
            virtual void SetCockpitDisp_(int bDisp);
            virtual void SetCockpitCoinDisp_(int bDisp);
            virtual void SetCockpitTimeDisp_(int bDisp);
            virtual void SetCockpitScoreDisp_(int bDisp);
            virtual void SetCockpitSetCountDisp_(int bDisp);
            virtual void SetScoreDemoTeamScore_(long, long);
            virtual void SetScoreDemoTeamCoin_(long, long);
            virtual long GetGoalDemoType_(long, long) const;
            virtual long GetGoalDemoTelop_(long, long, long, int) const;
            virtual void ScoreBookInitCountWork_();
            virtual void ScoreBookAddCountWork_(long, long, long);
            virtual void ScoreBookApplyCountWork_();
            virtual int MenuCallGameResult_();
            virtual int MenuCallInterval_();
            virtual int MenuCallPause_();
            virtual float GetArtsEventFrameNum_();
            virtual int ArtsEventLoop_();

            // The class's own virtual functions, 0x2A8-0x2C0.
            virtual void Initialize_();
            virtual void Finalize_();
            virtual void GameInit_();
            virtual void GameSetUp_();
            virtual void ChrPosSetNormal_(int);
            virtual void ChrPosSetResult_();
            virtual void CheckScoreDispOut_();

        protected:
            // (VolleyF member functions; meanings guessed from the names)
            void InitWork_();
            void InitServeClock_();
            void SetCockpitTeamScoreNum_();
            void SetCockpitServeClockCount_(long nColor, long nCount);
            void SetDispMatchPoint_();
            void EnableDispMatchPoint_(long nTeam);
            void DisableDispMatchPoint_();
            void SetDispScoreCountResult_();
            void SetDispSetCountResult_(int bGameSet);
            long GetGetSetPoint_(long nPeriodNum, long nTeam);
            void InitSetCountTelop_();
            void UpdateSetCountTelop_();
            void CheckDispRemoconShake_();
            void EnableDispRemoconShake_(long nTeam);
            void DisableDispRemoconShake_();
            int IsNeedDispDeuce_();
            void SetDispDeuce_();
            int IsNeedHalfFPS_();
            void CreateMiniTelop_(long nObjId, long nChr);
            void RemoveMiniTelop_(long nObjId);
            void UpdateShotClock_();
            void UpdateScoreAndCoin_();
            int IsNeedExceptDemo_();
            void ForceGameExit_(long nState);
            void ChangeServeChar_();
            void SetFirstState_();

            // 0x150, 0x154 (meanings guessed): the serving team and the team
            // that scored last; GameSetUp_ sets both to the user's team,
            // TrgVolleyGoalIn_ puts the scoring team in 0x154.
            long m_nServeTeam;
            long m_nScoredTeam;
            // 0x158: the serve order of the four places (0, 1, 2, 3 at the
            // start; name guessed). CGameVolley keeps each team's serving
            // character (player number) here, -1 before its first serve.
            long m_anServeOrder[4];
            long m_nMatchPointTeam;     // 0x168, the team at match point, -1 for none (SetDispMatchPoint_)
            long m_anUnk16C[4];         // 0x16C (meaning not known; cleared with the serve order)
            // 0x17C: per team and set, the set was given to the team (a
            // forced end; TrgVolleyGoalIn_ sets it with flag 0x40).
            long m_aabSetWin[2][5];
            long m_aanUnk1A4[2][5];     // 0x1A4 (meaning not known; cleared by GameInit_)
            // 0x1CC: each team's score as the score count last showed it.
            long m_anDispScore[4];
            long m_nRunTeam;            // 0x1DC, the team that scored the last points in a row
            long m_nRunCount;           // 0x1E0, how many
            // 0x1E4 (name guessed): while set, SetCockpitPartMove_ moves
            // nothing.
            int m_bPartMoveLock;
            long m_nUnk1E8;     // 0x1E8 (meaning not known; cleared by GameSetUp_)
            int m_bScored;      // 0x1EC: the rally has ended in a point (IsVolleyScored_)
            int m_bLineMsgDisp; // 0x1F0 (IsLineMsgDispEnable_)
            long m_nUnk1F4;     // 0x1F4 (meaning not known; -1 from GameSetUp_): the telop of a point
            long m_nPointTelop; // 0x1F8, a second telop of the point (27, 28), -1 for none
            int m_bDeuce;       // 0x1FC: the set has gone to deuce
            int m_bDeuceNext;   // 0x200: the next set starts at deuce
            float m_fScoreDispWait; // 0x204, frames before the score board moves out
            int m_bPartMove;    // 0x208: the cockpit parts have been moved out
            int m_bServeClock;      // 0x20C: the serve clock runs
            int m_bServeClockDisp;  // 0x210: the serve clock is shown
            float m_fServeClock;    // 0x214, frames left to serve
            long m_nServeClockCount; // 0x218, the second shown, -1 for none
            long m_nUnk21C;     // 0x21C (meaning not known; cleared by GameInit_)
            unsigned long m_hExDemo; // 0x220, the task id of the except demo
            long m_nExceptTelop;     // 0x224, the telop the except demo shows, -1 for none
            bcs::FVEC3 m_GoalPos;    // 0x228, where the last point fell
            // 0x234: the score book's counters: serve, spike, feint, block
            // and spike save, each made and missed (ScoreBookAddCountWork_
            // messages 13-22).
            long m_anVolleyScoreBook[10];
    };

    // The volleyball party game. Size 0x2BC.
    class CGameVolleyRhythmReceive : public CGameVolley {
        public:
            CGameVolleyRhythmReceive(bcs::CTask *pParent);

        private:
            unsigned char m_Unk25C[0x2BC - 0x25C];
    };
}

#endif /* !CGAMEVOLLEY_HPP_ */
