#ifndef MSW_CTOURNAMENTSTEP_HPP_
#define MSW_CTOURNAMENTSTEP_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CTmScriptHandler.hpp"

namespace msw {

    class CRouteBase;
    class CTmScript;
    class CTournamentExit;

    namespace TournamentData {
        struct FIREWORKS_INFO;
    }

    // The tournament map's root task (module TourF, game/tour/
    // CTournamentStep.cpp): a state machine that loads the map, runs the
    // stage's scripts (msw::CTmScript, which calls back through the
    // CTmScriptHandler virtuals), moves the teams, starts the matches and
    // shows the victory and unlock demos. 0x8FC bytes (Tournament::
    // CreateInstance). Member names and parameter names are guessed.
    class CTournamentStep : public CTmScriptHandler, public bcs::CFsm<CTournamentStep> {
            friend class CTournamentExit;

        public:
            // The states, in the order of the table _InitStep builds.
            enum {
                STATE_INITIALIZE,
                STATE_FINALIZE,
                STATE_LOADING,
                STATE_LOAD_END,
                STATE_EVENT_SETUP,
                STATE_EVENT_PLAY,
                STATE_EVENT_END,
                STATE_MOVE_SETUP,
                STATE_MOVE_PLAY,
                STATE_MOVE_END,
                STATE_MOVE_RESULT,
                STATE_NPC_CALL_MOVE,
                STATE_NPC_CALL_PLAY,
                STATE_GAME_CALL_MOVE,
                STATE_GAME_CALL_PLAY,
                STATE_GAME_CALL_END,
                STATE_TURN_START,
                STATE_TURN_END,
                STATE_GAME_SETUP,
                STATE_GAME_START,
                STATE_GAME_VICTORY,
                STATE_GAME_EXIT,
                STATE_GAME_SAVE,
                STATE_NEXT_CUP,
                STATE_DEMO_SETUP,
                STATE_DEMO_PLAY,
                STATE_DEMO_END,
                STATE_BOSS_CALL_PLAY,
                STATE_BOSS_CALL_END,
                STATE_NUM
            };

            // A queued timer event (_AddTimer, _PushTimer), 0x14 bytes.
            struct TIMER {
                TIMER *pNext;          // 0x00
                float fTime;           // 0x04
                short nEvent;          // 0x08
                short nParam;          // 0x0A
                unsigned long ulParam; // 0x0C
                // 0x10, 0x11: the state and the team that were current
                // when the timer was queued; -1 for a pushed timer.
                signed char nState;
                signed char nTeam;
                short nPushId;         // 0x12: the group of a pushed timer
            };

            CTournamentStep(bcs::CTask *pParent);

            virtual ~CTournamentStep();
            virtual void Main();
            virtual void Disp();

            virtual void MapObjSet(long nObj, const bcs::FVEC3 &vPos, float fRotY, const char *pName, int nFlag);
            virtual void RouteWrite(long nRoute, int bChanged);
            virtual void ChipWrite(long nPoint, unsigned long nUnk1, unsigned long nUnk2, long nUnk3, int nUnk4);
            virtual void SendRouteEvent(long nRoute, long nEvent, float fStart, unsigned long nUnk, float fEnd);
            virtual void SetRoutePoints(long nPoint, const unsigned char *pRoute, long nNum);
            virtual void RouteSelectSetup(unsigned char nPoint, const unsigned char *pRoute, long nUnk1, long nUnk2,
                                          long nUnk3);
            virtual int RouteSelect(long &rSelect);
            virtual void TeamCreate(long nTeam, long nUnk, float fUnk1, float fUnk2);
            virtual void TeamErase(long nTeam, long nUnk);
            virtual void TeamPos(long nTeam, long nPoint, long nUnk);
            virtual void MoveTarget(long nTeam, long nPoint, const unsigned char *pRoute);
            virtual void MoveResult(long nTeam, int nResult, float fUnk);
            virtual void PointCamera(long nPoint, long nCamera, float fUnk1, float fUnk2);
            virtual void FadeIn(float fSpeed, int nUnk);
            virtual int IsFadeInReq() const { return m_bFadeInReq; }
            virtual void SetMatchMake(long nMatch, long nTeam0, long nTeam1);
            virtual void VsMatchPause(long nTeam0, long nTeam1, long nMatch);

            virtual void ChangeStateProc();

        private:
            void _AddTimerRel(short nEvent, float fTime, short nParam, unsigned long ulParam);

            void _MakeRouteSelect(long nTeam);
            void _MakeRouteSelect(long nTeam0, long nTeam1);
            void _Backup();
            void _Restore();
            void _SetReslutMoveRoute(long nTeam, int nResult, int b);
            void _SetupGame(long nMatch, int b);
            int _IsStarCupDemo(long nCup);
            float _MoveSetFrame(unsigned long ulMask);
            void _CleanAfterErase();
            void _SetupTeamPos(int b);
            void _UpdeteOffsetPos();

            void _CreateTimer(long nNum);
            void _AddTimer(short nEvent, float fTime, short nParam, unsigned long ulParam);
            void _CheckTimer(float fFrame);
            void _PushTimer(short nId, short nEvent, float fTime, short nParam, unsigned long ulParam);
            void _PopTimer(short nId, float fTime);
            void _ResetTimer();
            void _CleanPushTimer();
            int _IsTimerEvent() const;
            int _IsTimerEvent(char nState) const;
            int _IsTimerEvent(char nState, char nTeam) const;
            int _IsTimerEventGroup(char nTeam) const;

            void _StartFade(long nType, float fSpeed, int b);
            void _StartFadeCross(float fSpeed);
            void _Start3DFadeCross(float fSpeed);
            int _WaitFade() const;
            int _IsNext(int b);
            void _SkipStateCockpit(long nSubStep);
            void _SetTrophy(long nType);
            void _EndDemoStep(long nDemo);
            void _ExitStep(long nStep, int b);
            void _EndTutorialStep(long nStep, int b);
            void _EndStep(long nStep, int b);
            void _InitStep();
            void _ChangeStateEvent(char nState);
            void _TimerEvent(char nState, char nEvent, short nParam, unsigned long ulParam, int bReset);
            void _SetupFireworks(const TournamentData::FIREWORKS_INFO *pInfo, long nNum);
            void _SetEffFireworks(const TournamentData::FIREWORKS_INFO *pInfo, long n0, long n1, long n2);
            void _TimeEventDefaultProc(char nEvent, short nParam, unsigned long ulParam);
            void _TimeEventResetDefaultProc(char nEvent, short nParam, unsigned long ulParam);

            void CtorStateInitialize();
            void LoopStateInitialize();
            void DtorStateInitialize();
            void CtorStateFinalize();
            void LoopStateFinalize();
            void DtorStateFinalize();
            void CtorStateLoadEnd();
            void LoopStateLoadEnd();
            void DtorStateLoadEnd();
            void _UnLoadData();
            void CtorStateLoading();
            void LoopStateLoading();
            void DtorStateLoading();
            int _CheckEventEntry(long n0, long n1);
            void CtorStateEventSetup();
            void LoopStateEventSetup();
            void DtorStateEventSetup();
            void CtorStateEventPlay();
            void LoopStateEventPlay();
            void DtorStateEventPlay();
            void CtorStateEventEnd();
            void LoopStateEventEnd();
            void DtorStateEventEnd();
            void CtorStateNpcCallMove();
            void LoopStateNpcCallMove();
            void DtorStateNpcCallMove();
            void TimeStateNpcCallMove(char nEvent, short nParam, unsigned long ulParam, int bReset);
            void CtorStateNpcCallPlay();
            void LoopStateNpcCallPlay();
            void TimeStateNpcCallPlay(char nEvent, short nParam, unsigned long ulParam, int bReset);
            void DtorStateNpcCallPlay();
            void CtorStateGameCallMove();
            void LoopStateGameCallMove();
            void DtorStateGameCallMove();
            void TimeStateGameCallMove(char nEvent, short nParam, unsigned long ulParam, int bReset);
            void CtorStateGameCallPlay();
            void LoopStateGameCallPlay();
            void DtorStateGameCallPlay();
            void TimeStateGameCallPlay(char nEvent, short nParam, unsigned long ulParam, int bReset);
            void CtorStateGameCallEnd();
            void LoopStateGameCallEnd();
            void DtorStateGameCallEnd();
            void CtorStateMoveResult();
            void LoopStateMoveResult();
            void DtorStateMoveResult();
            void CtorStateMoveSetup();
            void LoopStateMoveSetup();
            void DtorStateMoveSetup();
            void CtorStateMovePlay();
            void LoopStateMovePlay();
            void DtorStateMovePlay();
            void CtorStateMoveEnd();
            void LoopStateMoveEnd();
            void DtorStateMoveEnd();
            void CtorStateTurnStart();
            void LoopStateTurnStart();
            void DtorStateTurnStart();
            void CtorStateTurnEnd();
            void LoopStateTurnEnd();
            void DtorStateTurnEnd();
            void CtorStateGameStart();
            void LoopStateGameStart();
            void TimeStateGameStart(char nEvent, short nParam, unsigned long ulParam, int bReset);
            void DtorStateGameStart();
            void _LoadVictoryMotion(long nTeam);
            void _SetupVictoryGallery(long nCup);
            void _OffsetCupUpChar(long nChr, const bcs::FVEC3 &vOffset);
            void CtorStateGameVictory();
            void LoopStateGameVictory();
            void DtorStateGameVictory();
            void _SetupVictoryUnlock(int b);
            void _SetupStarUnlock();
            void _SetupUnlock();
            void _PopUnlock();
            void _SetClear();
            void CtorStateNextCup();
            void LoopStateNextCup();
            void DtorStateNextCup();
            void _SetupDefalutParam(long n0, long n1, long n2);
            void CtorStateGameSetup();
            void LoopStateGameSetup();
            void DtorStateGameSetup();
            void CtorStateGameSave();
            void LoopStateGameSave();
            void DtorStateGameSave();
            void CtorStateGameExit();
            void LoopStateGameExit();
            void DtorStateGameExit();
            void CtorStateDemoSetup();
            void LoopStateDemoSetup();
            void DtorStateDemoSetup();
            void CtorStateDemoPlay();
            void LoopStateDemoPlay();
            void DtorStateDemoPlay();
            void CtorStateDemoEnd();
            void LoopStateDemoEnd();
            void DtorStateDemoEnd();
            void CtorStateBossCallPlay();
            void LoopStateBossCallPlay();
            void DtorStateBossCallPlay();
            void CtorStateBossCallEnd();
            void LoopStateBossCallEnd();
            void DtorStateBossCallEnd();
            void _SetupVsScene(int bSkipDemo);

            // The state the timers belong to now: the current state and the
            // team whose turn it is.
            void _SetTimerOwner();

            CTmScript *m_pScript;         // 0x80
            CTournamentExit *m_pExit;     // 0x84: the "leave the tournament" menu
            bcs::CTask *m_pDemo;          // 0x88
            bcs::FVEC3 *m_pFireworksPos; // 0x8C: where the fireworks go off
            float m_fTimer;               // 0x90: time of the timer queue
            float m_fTimerOver;           // 0x94: how far the running timer is late
            TIMER *m_pTimerPool;          // 0x98
            TIMER *m_pTimerFree;          // 0x9C
            TIMER *m_pTimerList;          // 0xA0: queued, in order of time
            TIMER *m_pTimerPush;          // 0xA4: pushed, waiting for _PopTimer
            long m_nSubStep;              // 0xA8: the step inside the state
            float m_fStepTime;            // 0xAC: time since the state began
            unsigned long m_ulMoveMask;   // 0xB0: the characters moving
            unsigned long m_UnkB4;        // 0xB4
            float m_fUnkB8;               // 0xB8: counts down
            unsigned char m_UnkBC[4];     // 0xBC
            signed char m_nTimerState;    // 0xC0: the owner of new timers (see TIMER)
            signed char m_nTimerTeam;     // 0xC1
            signed char m_UnkC2;          // 0xC2
            signed char m_bFadeInReq;     // 0xC3
            signed char m_nEndParam;      // 0xC4
            signed char m_UnkC5;          // 0xC5
            signed char m_UnkC6;          // 0xC6
            signed char m_UnkC7;          // 0xC7
            signed char m_UnkC8;          // 0xC8
            signed char m_UnkC9;          // 0xC9
            signed char m_UnkCA;          // 0xCA
            signed char m_UnkCB;          // 0xCB
            short m_nTurnAdd;             // 0xCC: added to the turn count by the next CtorStateTurnStart
            short m_UnkCE;                // 0xCE
            signed char m_UnkD0;          // 0xD0
            signed char m_nMoveTeam0;     // 0xD1
            signed char m_nMoveTeam1;     // 0xD2
            signed char m_UnkD3;          // 0xD3
            signed char m_UnkD4;          // 0xD4
            signed char m_nFadeType;      // 0xD5
            signed char m_nRouteNum0;     // 0xD6: entries of m_aRoute0
            signed char m_nRouteNum1;     // 0xD7: entries of m_aRoute1
            signed char m_UnkD8;          // 0xD8
            signed char m_UnkD9;          // 0xD9
            signed char m_UnkDA;          // 0xDA
            signed char m_nTrophy;        // 0xDB
            unsigned long m_UnkDC;        // 0xDC
            unsigned short m_aRoute0[0x200]; // 0xE0: the routes team m_nMoveTeam0 moves along
            unsigned short m_aRoute1[0x200]; // 0x4E0: and team m_nMoveTeam1
            // The route choice (RouteSelectSetup / RouteSelect).
            signed char m_nSelectNum;     // 0x8E0
            signed char m_nSelect;        // 0x8E1
            signed char m_nSelectStep;    // 0x8E2
            unsigned char m_nSelectPoint; // 0x8E3
            float m_fSelectTime;          // 0x8E4
            const unsigned char *m_pSelectRoute; // 0x8E8
            CRouteBase *m_apSelect[4];    // 0x8EC: the arrows
    }; // size 0x8FC
}

#endif /* !MSW_CTOURNAMENTSTEP_HPP_ */
