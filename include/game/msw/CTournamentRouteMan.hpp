#ifndef CTOURNAMENTROUTEMAN_HPP_
#define CTOURNAMENTROUTEMAN_HPP_

#include "game/msw/CRouteBase.hpp"
#include "game/msw/TournamentWork.hpp"

namespace msw {

    class CEventModel;
    class CStageManTournament;
    class CTournamentChip;

    // Runs the routes, map animations and timed map events of one
    // tournament stage (module TourF). Each stage has a derived class
    // (CTournamentRouteMan_Stage31/32/33, 0x7A0/0x7C0/0x7CC bytes) that
    // CreateInstance picks by stage number. vptr at 0x00. Defined in
    // game/tour/CTournamentRouteMan.cpp. Names of members and parameters
    // are guessed; virtual return types are read from the base bodies where
    // they return something, else guessed.
    class CTournamentRouteMan {
        public:
            // A value read or written through Get/SetParamData: Stage31/32
            // use it as a float, Stage32 also as a word.
            union GET_PARAM {
                float fValue;
                long nValue;
            };

            // A queued timer event (_AddTimer, _PushTimer), 0x1C bytes.
            // (names guessed)
            struct TIMER {
                TIMER *pNext;          // 0x00
                float fTime;           // 0x04
                long nEvent;           // 0x08
                long nGroup;           // 0x0C
                long nParam;           // 0x10
                float fParam;          // 0x14
                unsigned long ulParam; // 0x18
            };

            static CTournamentRouteMan *CreateInstance(long nStage, CStageManTournament *pStageMan);
            static CTournamentRouteMan *GetInstance();
            // (used by msw::CTournamentMan) Sets up the map models of stage
            // nStage for an event demo, without a route manager.
            static void CreateInstanceDemo(long nStage);
            static void DeleteInstance();

            // (return type guessed) Forgets the route character nChr is on.
            void ResetNowRoute(long nChr);
            // (name guessed) Route nRoute of the map (0 to 0xFF).
            CRouteBase *GetRoute(long nRoute) const { return m_apRoute[nRoute]; }

            // The small virtual functions are inline: TourF has weak copies
            // of them after their first users (TimerEvent and StageDispSe
            // in an earlier file).
            virtual ~CTournamentRouteMan();
            virtual void Initialize() {}
            virtual CRouteBase *CreateRoute(long nType, long nFrom, long nTo) { return 0; }
            // The model of the line drawn along a route of kind nType, and
            // the frame its animation starts at. (meaning guessed)
            virtual CEventModel *CreateWinRoute(float &fFrame, long nType, long nTo);
            // Called when a timer queued with _AddTimer runs out.
            virtual void TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b) {}
            virtual void PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel) {}
            virtual int SetParamData(long nId, const GET_PARAM &param) { return 0; }
            virtual int GetParamData(long nId, GET_PARAM &param) { return 0; }
            virtual long GetChipType(long nPoint) { return -1; }
            virtual void StageDispSe(int bDisp) {}

            // Colour of the crystal on the map (cup model).
            static void SetCrystalColor(long nColor);

            // Called by msw::CTournamentChip::UpdateFrame once a chip has
            // been shown for two frames, with the values the route manager
            // left in the chip. (parameter names and return types guessed)
            void SetupStagePoint(long nPoint, int);
            void SetupStageChip(long nChip, long, long);

            // (names guessed) Builds the routes of the map; shows the line
            // of route nRoute; marks a point and a chip as written; shows
            // the routes the winner takes; switches point nPoint on or off.
            void SetupRoute();
            void SetRouteWrite(long nRoute, int bWrite);
            void SetChipWrite(long nPoint, unsigned long ulFlag, unsigned long ulFlag2, long nChip, int b);
            void SetWinRoute(unsigned short *pRoute, long nNum, int bWin, int bAnim);
            void ChangePoint(long nPoint, int bOn);

            // Whether a timer is queued; runs all queued timers now; drops
            // the timers pushed by the routes. (names guessed)
            static int IsTimerEvent();
            void ResetTimer();
            void CleanPushTimer();
            void _UpdateFrame(float fFrame);

            // Queues timer event nEvent for the route event nId: it runs
            // fTime frames after that event, with nParam, fParam and
            // ulParam. Called by the routes' PushRoute. (parameter names
            // guessed)
            void _PushTimer(long nEvent, long nId, float fTime, long nParam, float fParam, unsigned long ulParam);
            // The same, inline (TourF has a weak copy, called by the pipe
            // route's PushRoute).
            void PushTimer(long nEvent, long nId, float fTime, long nParam, float fParam, unsigned long ulParam)
            {
                TIMER *pTimer = m_pTimerFree;
                pTimer->fTime = fTime;
                pTimer->nEvent = nEvent;
                pTimer->nGroup = nId;
                pTimer->nParam = nParam;
                pTimer->fParam = fParam;
                pTimer->ulParam = ulParam;
                m_pTimerFree = pTimer->pNext;
                pTimer->pNext = m_pTimerPush;
                m_pTimerPush = pTimer;
            }

            // Moves the pushed timers of route event nId (-1: all) to the
            // queue, fTime frames from now.
            void _PopTimer(long nId, float fTime);

        protected:
            // The routes of game/tour/CTournamentRouteMan.cpp queue timers
            // and read the timer state directly.
            friend class CRouteBasicLine;
            friend class CRouteDokan;
            friend class CRouteJumpPoints;
            friend class CRouteMove;

            // Inline: CreateInstance constructs the stage classes in place.
            CTournamentRouteMan(CStageManTournament *pStageMan)
                : m_pStageMan(pStageMan), m_nTimerGroup(-1), m_fTimer(0.0f), m_pTimerPool(0), m_pTimerFree(0),
                  m_pTimerList(0), m_pTimerPush(0), m_nUnk584(0)
            {
            }

            void _Init();
            void _Destory();

            void _CreateTimer(long nNum);
            // Queues timer event nEvent to run at time fTime.
            void _AddTimer(long nEvent, long nGroup, float fTime, long nParam, float fParam, unsigned long ulParam);
            // Removes the queued timers of event nEvent with parameter nParam.
            void _RemoveTimer(long nEvent, long nParam);

            // (name guessed) Pushes the camera timer being built, if any
            // (_DefPushCamera; inline there and in msw::CRouteMove::SetupMove).
            void _PushCameraTimer()
            {
                if (m_nUnk584 != 0) {
                    PushTimer(m_nUnk584, m_nCameraId, m_fCameraTime, m_nCameraParam, m_fCameraFrame, m_ulCameraParam);
                }
            }

            // The default handling of the events the stages do not handle.
            void _DefTimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            void _DefPushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            CRouteBase *_CreateRoute(CRouteBase::_PARAM &param, long nType, long nFrom, long nTo);

            // A chip model at grid position (nX, nZ) next to pos, of chip
            // kind nChip (or the grid's chip when it is negative); nType goes
            // to its type, nStageChip and nStagePoint to the values
            // SetupStageChip and SetupStagePoint get back. (names guessed)
            CTournamentChip *SetupChipModel(long nType, long nX, long nZ, const bcs::FVEC3 &pos, long nChip,
                                            long nStageChip, long nStagePoint, int bDisp, long nLayer,
                                            long nSelector);
            void _SetupStagePoint(TournamentWork::POINT_INFO &info, long nPoint, unsigned long ulFlag,
                                  unsigned long ulFlag2, long nType, int b);

            // Map animation, effect and sound helpers for TimerEvent.
            void _StageAnim(const char *pAnim, long nParam, float fParam, unsigned long ulParam, long nEvent,
                            int b, float fFrame, unsigned long ulFlag, int bLoop);
            void _StageAnimEnd(const char *pAnim, long nParam, float fParam, unsigned long ulParam,
                               unsigned long ulFlag, int b, long n);
            void _StageSe(long nSe, long nParam, float fParam, unsigned long ulParam, long nSlot);
            // Puts map object nParam at pos (and plays pAnim on it, when not 0).
            void _StageSet(const bcs::FVEC3 &pos, long nParam, float fParam, const char *pAnim, float fFrame,
                           unsigned long ulFlag, int b);
            // Shakes the camera.
            void _StageQuake(const bcs::FVEC3 &power, float fFrame);
            void _StageEff(long nEff, long nParam, float fParam, unsigned long ulParam, long n,
                           unsigned long ul1, unsigned long ul2);
            void _StageEffAttach(long nEff, const char *pNode, long nParam, float fParam, unsigned long ulParam,
                                 long n, unsigned long ul1, unsigned long ul2);
            void _StageEffFadeIn(long nEff, float fFade, long nParam, float fParam, unsigned long ulParam, long n,
                                 unsigned long ul1, unsigned long ul2);
            void _StageEffFadeOut(float fFade, long nParam, float fParam, unsigned long ulParam);
            void _StageChipFade(long nChip, float fFrom, float fSpeed);
            void _StageChipEff(long nEff, long nChip, float fParam, unsigned long ulParam, long n,
                               unsigned long ul1, unsigned long ul2);
            // (parameter names guessed; used by Stage33) Plays node
            // animation pAnim on map object nParam and returns its length in
            // frames; attaches the character ulParam to map object nParam (or
            // takes it off again); hides or shows map object nParam.
            float _StageAnimNode(const char *pAnim, long nParam, float fParam, unsigned long ulParam, long nEvent,
                                 float fFrame, long n, int b);
            void _StageAttachChr(long nParam, unsigned long ulParam, long n, int bAttach);
            void _StageDisp(long nParam, int bDisp);

            CStageManTournament *m_pStageMan; // 0x04
            long m_nUnk08;                    // 0x08: passed to _StageAttachChr by Stage33
            long m_nTimerGroup;               // 0x0C: group of the timer being run (-1 for none)
            float m_fTimer;                   // 0x10: time of the timer being run
            float m_fTimerOver;               // 0x14: how far the current time is past it
            TIMER *m_pTimerPool;              // 0x18: the nodes (new[])
            TIMER *m_pTimerFree;              // 0x1C: the unused ones
            TIMER *m_pTimerList;              // 0x20: the queue, by time
            TIMER *m_pTimerPush;              // 0x24: pushed by the routes, waiting for their event
            // 0x28: the route each character is on (msw::CRouteMove::
            // UpdateFrame). (name guessed)
            CRouteBase *m_apNowRoute[20];
            CRouteBase *m_apRoute[0x100];     // 0x78: one per route of the map (name guessed)
            short m_anPointEffect[128];       // 0x478: effect slot of each map point, -1 for none (name guessed)
            float m_fUnk578;                  // 0x578: with 0x57C and 0x580, a position read in _Init
            float m_fParam0;                  // 0x57C: GetParamData(0) of Stage31/32
            float m_fUnk580;                  // 0x580
            // 0x584-0x59C: the camera timer being built by _DefPushCamera
            // (event 0x3EB or 0x3EC, 0 for none), pushed when the next one
            // starts. (names guessed)
            long m_nUnk584;
            long m_nCameraId;                 // 0x588
            long m_nCameraParam;              // 0x58C
            unsigned long m_ulCameraParam;    // 0x590
            float m_fCameraDir;               // 0x594
            float m_fCameraFrame;             // 0x598
            float m_fCameraTime;              // 0x59C
            CTournamentChip *m_apChip[128];   // 0x5A0: one per map point, 0 where there is none

            static CTournamentRouteMan *m_pInstance;
    }; // size 0x7A0
}

#endif /* !CTOURNAMENTROUTEMAN_HPP_ */
