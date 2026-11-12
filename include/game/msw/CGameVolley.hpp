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
            void CheckDispRemoconShake_();
            void DisableDispRemoconShake_();

            // 0x150, 0x154 (meanings guessed): the serving team and the team
            // that scored last; GameSetUp_ sets both to the user's team,
            // TrgVolleyGoalIn_ puts the scoring team in 0x154.
            long m_nServeTeam;
            long m_nScoredTeam;
            // 0x158: the serve order of the four places (0, 1, 2, 3 at the
            // start; name guessed).
            long m_anServeOrder[4];
            unsigned char m_Unk168[0x1E8 - 0x168];
            long m_nUnk1E8;     // 0x1E8 (meaning not known; cleared by GameSetUp_)
            int m_bScored;      // 0x1EC: the rally has ended in a point (IsVolleyScored_)
            unsigned char m_Unk1F0[0x1F4 - 0x1F0];
            long m_nUnk1F4;     // 0x1F4 (meaning not known; -1 from GameSetUp_)
            unsigned char m_Unk1F8[0x208 - 0x1F8];
            int m_bPartMove;    // 0x208: the cockpit parts have been moved out
            unsigned char m_Unk20C[0x21C - 0x20C];
            long m_nUnk21C;     // 0x21C (meaning not known; cleared by GameInit_)
            unsigned char m_Unk220[0x25C - 0x220];
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
