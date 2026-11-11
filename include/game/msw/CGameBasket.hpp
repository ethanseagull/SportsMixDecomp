#ifndef CGAMEBASKET_HPP_
#define CGAMEBASKET_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Lives in module BasketF. Size 0x274.
    class CGameBasket : public CGame {
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
            virtual void DtorStateGameSetup();
            virtual void CtorStateWaitStart();
            virtual void DtorStateWaitStart();
            virtual void CtorStateMainLoop();
            virtual void DtorStateMainLoop();
            virtual void CtorStateGoalEvent();
            virtual void LoopStateGoalEvent();
            virtual void DtorStateGoalEvent();
            virtual void CtorStateTurnResult();
            virtual void LoopStateTurnResult();
            virtual void DtorStateTurnResult();
            virtual void CtorStateChgCourt();
            virtual void LoopStateChgCourt();
            virtual void DtorStateChgCourt();
            virtual void CtorStateTurnOver();
            virtual void LoopStateTurnOver();
            virtual void DtorStateTurnOver();
            virtual void CtorStateChgOffense();
            virtual void LoopStateChgOffense();
            virtual void DtorStateChgOffense();
            virtual void CtorStateGameResult();
            virtual void LoopStateGameResult();
            virtual void DtorStateGameResult();
            virtual void CalcAddSkillPoint_();
            virtual void SetGameResultNext_();
            virtual void AddTeamCoin_(long, long);
            virtual void TrgBasketGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, long);
            virtual void GetMaxCoin_() const;
            virtual void GetFirstGoalIdxMode_();
            virtual void SetCockpitArtsGaugeRatio_(long, float, float);
            virtual void SetCockpitArtsGaugeSpAnim_(long, int);
            virtual void SetDispRadarIcon_(int);
            virtual void SetCockpitCoinGetTelopMove_(long, long);
            virtual void SetCockpitDisp_(int);
            virtual void SetCockpitCoinDisp_(int);
            virtual void SetCockpitTimeDisp_(int);
            virtual void SetCockpitScoreDisp_(int);
            virtual void SetCockpitPeriodDisp_(int);
            virtual void GetGoalDemoType_(long, long) const;
            virtual void GetGoalDemoTelop_(long, long, long, int) const;
            virtual void StartGoalTelop_();
            virtual void GetExceptionalCameraType_();
            virtual void ScoreBookInitCountWork_();
            virtual void ScoreBookAddCountWork_(long, long, long);
            virtual void ScoreBookApplyCountWork_();
            virtual void MenuCallInterval_();
            virtual void MenuCallPause_();
            virtual void CockpitCheckArrowState_();
            virtual void CameraReset_();

            // (static; return type guessed) The level of the party game.
            static long GetPartyLv();

            // Member functions of the game (added for
            // game/basket/CGameBasketHungryBossPackun.cpp; return types and
            // parameter meanings guessed).
            void DoGoalEffect_(long nChr, long, long nShootType, unsigned long, const bcs::FVEC3 &rPos);
            // Shakes the camera (quake nQuake) for character nChr's view.
            void CameraQuake_(long nQuake, long nChr);
            void CheckTimeUp_();
            void ChrPosSetNormal_();
            // Not zero when the time display changes colour.
            int IsTimeColorChange();
            void CreateTelop_(long nTelop, int, long);
            void EndTelop_(long nTelop);
            void RemoveTelop_(long nTelop);
            int IsTelopEnd_(long nTelop) const;
            void PlaySnd_(long nSnd, const bcs::FVEC3 *pPos);
            void SetShotClockCount_(long nCount, long);

        private:
            unsigned char m_Unk150[0x1C0 - sizeof(CGame)];
        public:
            // 0x1C0 (meaning not known): CComManBasketGame::CanbeShootState_
            // lets a computer team shoot while it is above a level parameter.
            float m_fUnk1C0;

        private:
            unsigned char m_Unk1C4[0x274 - 0x1C4];
    };
}

#endif /* !CGAMEBASKET_HPP_ */
