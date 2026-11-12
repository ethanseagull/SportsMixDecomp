#ifndef CGAMEBOSS_HPP_
#define CGAMEBOSS_HPP_

#include "game/msw/CGame.hpp"

namespace bcs {
    class CCamera;
}

namespace msw {

    // The game task of the boss battle (BossF, game/boss/GameBoss.cpp).
    // Size 0x1EC. The virtual functions after CGame's part (0x2A8 on) and
    // the members are read from that file; member names are guesses.
    class CGameBoss : public CGame {
        public:
            enum {
                CHR_MAX = 8 // the characters of the life gauges and the arts event
            };

            CGameBoss(bcs::CTask *pParent);
            virtual ~CGameBoss();

            // Overrides of CGame's virtual functions, in vtable order.
            virtual void MainFunc();
            virtual void DispFunc();
            virtual void FinishFirstLoadData();
            virtual void GenerateInstance();
            virtual void PrepareRestartGame_();
            virtual void SendMessage_(long nMsg, long, long, long);
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
            virtual void CtorStateTurnOver();
            virtual void LoopStateTurnOver();
            virtual void DtorStateTurnOver();
            virtual void CtorStateGameSet();
            virtual void LoopStateGameSet();
            virtual void DtorStateGameSet();
            virtual void CtorStateGameResult();
            virtual void LoopStateGameResult();
            virtual void DtorStateGameResult();
            virtual void SetGameResult_();
            virtual void CalcAddSkillPoint_();
            virtual void SetMissionResult_();
            virtual int SetGameResultNext_();
            virtual void AddTeamCoin_(long nTeam, long nCoin);
            virtual long GetMaxCoin_() const;
            virtual void EventDemoSetUp();
            virtual void StartArtsEvent_(long nChr);
            virtual void EndArtsEvent_(long nChr);
            virtual long GetFirstGoalIdxMode_();
            virtual void ChangeGoalIdx_();
            virtual int IsSetOver_();
            virtual int IsGameSet_();
            virtual void SetCockpitArtsGaugeRatio_(long nTeam, float fRatio, float fTelop);
            virtual void SetCockpitArtsGaugeSpAnim_(long nTeam, int bSpecial);
            virtual void SetDispRadarIcon_(int bDisp);
            virtual void SetCockpitPartMove_(long nPart, long nMove);
            virtual void SetCockpitDisp_(int bDisp);
            virtual void SetCockpitCoinDisp_(int bDisp);
            virtual void SetCockpitTimeDisp_(int bDisp);
            virtual void SetCockpitScoreDisp_(int bDisp);
            virtual void SetCockpitSetCountDisp_(int bDisp);
            virtual int MenuCallGameResult_();
            virtual int MenuCallInterval_();
            virtual int MenuCallPause_();
            virtual float GetArtsEventFrameNum_();
            virtual void ArtsEventLoop_();

            // The class's own virtual functions, 0x2A8-0x2C8.
            virtual void Initialize_();
            virtual void Finalize_();
            virtual void GameInit_();
            virtual void GameSetUp_();
            virtual void ChrPosSetNormal_(int);
            virtual void ChrPosSetResult_();
            virtual void PowerUpTelopIn_();
            virtual void UpdatePowerUpTelop_(float fFrame);
            virtual void LoopStateEntranceDemo();

        private:
            void InitWork_();
            void UpdateCockpitCount_();
            void ArtsSetCam_(long nType);
            void ArtsEndCam_();
            void SetCockpitSetting_();
            void CheckDispRemoconShake_();
            void EnableDispRemoconShake_(long);
            void DisableDispRemoconShake_();
            void CheckTimeUp_();
            void ForceGameExit_(long nState);

            long m_nUnk150;               // 0x150, -1 from InitWork_
            long m_nUnk154;               // 0x154, -1 from InitWork_ and GameSetUp_
            float m_fUnk158;              // 0x158, 120 from InitWork_ and the wait-start state
            int m_bPartMove;              // 0x15C: the cockpit parts have been moved in
            long m_nUnk160;               // 0x160 (cleared by message 0x12 and GameInit_)
            unsigned char m_Unk164[0x170 - 0x164];
            long m_nCount;                // 0x170: the count-down number shown before the start
            float m_fBossLife;            // 0x174: the boss's life, 0 to 1
            float m_fPowerUpTelop;        // 0x178: frames left of the power-up telop
            int m_abOutTelop[CHR_MAX];    // 0x17C: the "out" telop of each life gauge is shown
            float m_afUnk19C[CHR_MAX];    // 0x19C (not used here)
            int m_bSPController;          // 0x1BC: the special-shot controller is shown
            long m_nArtsChrNum;           // 0x1C0: characters placed for the arts event
            long m_anArtsChr[CHR_MAX];    // 0x1C4: their player numbers (-1: none)
            bcs::CCamera *m_pArtsCam;     // 0x1E4: the arts event's camera
            float m_fPauseWait;           // 0x1E8: frames until pausing is allowed again
    };
}

#endif /* !CGAMEBOSS_HPP_ */
