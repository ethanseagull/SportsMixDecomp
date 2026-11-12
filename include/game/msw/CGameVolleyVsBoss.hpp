#ifndef MSW_CGAMEVOLLEYVSBOSS_HPP_
#define MSW_CGAMEVOLLEYVSBOSS_HPP_

#include "game/msw/CGameVolley.hpp"

namespace msw {

    // The volleyball game against a boss (VolleyF,
    // game/volley/CGameVolleyVsBoss.cpp): the boss takes a number of hits,
    // shown on the cockpit with a life gauge, and the game ends when it has
    // none left or the time runs out. Size at least 0x264 (the members its
    // functions write; no allocation site found).
    class CGameVolleyVsBoss : public CGameVolley {
        public:
            CGameVolleyVsBoss(bcs::CTask *pParent);
            virtual ~CGameVolleyVsBoss();

            // Overrides, in vtable order.
            virtual void MainFunc();
            virtual void DispFunc();
            virtual void FinishFirstLoadData();
            virtual void GenerateInstance();
            virtual void SendMessage_(long nMsg, long, long, long);
            virtual void CtorStateGameSetup();
            virtual void LoopStateGameSetup();
            virtual void LoopStateWaitStart();
            virtual void CtorStateMainLoop();
            virtual void LoopStateMainLoop();
            virtual void DtorStateMainLoop();
            virtual void CtorStateTurnResult();
            virtual void LoopStateTurnResult();
            virtual void DtorStateTurnResult();
            virtual void CtorStateTimeUp();
            virtual void LoopStateTimeUp();
            virtual void DtorStateTimeUp();
            virtual void CtorStateGameSet();
            virtual void LoopStateGameSet();
            virtual void DtorStateGameSet();
            virtual void DtorStateRestart();
            virtual void SetGameResult_();
            virtual void TrgVolleyGoalIn_(long nChr, long nTeam, long nShootType, long nScore, long nCoin, long,
                                          const bcs::FVEC3 &rPos, unsigned long nFlag);
            virtual int IsVolleyScored_() const;
            virtual int IsVolleyServeClockOver_() const;
            virtual void StartArtsEvent_(long nChr);
            virtual void EndArtsEvent_(long nChr);
            virtual long GetFirstGoalIdxMode_();
            virtual void ChangeGoalIdx_();
            virtual int IsSetOver_();
            virtual int IsGameSet_();
            virtual int MenuCallGameResult_();
            virtual void Initialize_();
            virtual void GameInit_();
            virtual void GameSetUp_();
            virtual void ChrPosSetNormal_(int);
            virtual void ChrPosSetResult_();
            virtual void CheckScoreDispOut_();

            // The class's own virtual function, 0x2C4.
            virtual void LoopStateEntranceDemo();

        private:
            void UpdateCockpitCount_();
            void CheckTimeUp_();

            long m_nBossLife;       // 0x25C: hits the boss can still take (12 at the start)
            float m_fBossLifeRatio; // 0x260: the boss's life gauge, 0 to 1
    };
}

#endif /* !MSW_CGAMEVOLLEYVSBOSS_HPP_ */
