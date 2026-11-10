#ifndef CTOURNAMENTROUTEMAN_HPP_
#define CTOURNAMENTROUTEMAN_HPP_

#include "game/msw/CRouteBase.hpp"

namespace msw {

    class CEventModel;
    class CStageManTournament;
    class CTournamentChip;

    // Runs the routes, map animations and timed map events of one
    // tournament stage (module TourF). Each stage has a derived class
    // (CTournamentRouteMan_Stage31/32/33, 0x7A0/0x7C0/0x7CC bytes) that
    // CreateInstance picks by stage number. vptr at 0x00. Defined in
    // game/tour/CTournamentRouteMan.cpp (not written yet); only the members
    // the stage classes use are declared. Names of members and parameters
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

            static CTournamentRouteMan *CreateInstance(long nStage, CStageManTournament *pStageMan);
            static CTournamentRouteMan *GetInstance();

            virtual ~CTournamentRouteMan();
            virtual void Initialize();
            virtual CRouteBase *CreateRoute(long nType, long nFrom, long nTo);
            virtual CRouteBase *CreateWinRoute(float &fFrame, long nFrom, long nTo);
            // Called when a timer queued with _AddTimer runs out.
            virtual void TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            virtual void PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            virtual int SetParamData(long nId, const GET_PARAM &param);
            virtual int GetParamData(long nId, GET_PARAM &param);
            virtual long GetChipType(long nPoint); // base: -1
            virtual void StageDispSe(int bDisp);

            // Colour of the crystal on the map (cup model).
            static void SetCrystalColor(long nColor);

            // Called by msw::CTournamentChip::UpdateFrame once a chip has
            // been shown for two frames, with the values the route manager
            // left in the chip. (parameter names and return types guessed)
            void SetupStagePoint(long nPoint, int);
            void SetupStageChip(long nChip, long, long);

        protected:
            void _CreateTimer(long nNum);
            // Queues timer event nEvent to run at time fTime.
            void _AddTimer(long nEvent, long nGroup, float fTime, long nParam, float fParam, unsigned long ulParam);

            // The default handling of the events the stages do not handle.
            void _DefTimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b);
            void _DefPushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel);
            CRouteBase *_CreateRoute(CRouteBase::_PARAM &param, long nType, long nFrom, long nTo);

            // Map animation, effect and sound helpers for TimerEvent.
            void _StageAnim(const char *pAnim, long nParam, float fParam, unsigned long ulParam, long nEvent,
                            int b, float fFrame, unsigned long ulFlag, int bLoop);
            void _StageAnimEnd(const char *pAnim, long nParam, float fParam, unsigned long ulParam,
                               unsigned long ulFlag, int b, long n);
            void _StageSe(long nSe, long nParam, float fParam, unsigned long ulParam, long nSlot);
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

            CStageManTournament *m_pStageMan; // 0x04
            char m_Unk08[0x4];                // 0x08
            long m_nTimerGroup;               // 0x0C: group of the timer being run (-1 for none)
            float m_fTimer;                   // 0x10: time of the timer being run
            float m_fTimerOver;               // 0x14: how far the current time is past it
            void *m_pTimerPool;               // 0x18
            void *m_pTimerFree;               // 0x1C
            void *m_pTimerList;               // 0x20
            void *m_pTimerPush;               // 0x24
            char m_Unk28[0x550];              // 0x28
            float m_fUnk578;                  // 0x578
            float m_fParam0;                  // 0x57C: GetParamData(0) of Stage31/32
            float m_fUnk580;                  // 0x580
            long m_nUnk584;                   // 0x584
            char m_Unk588[0x18];              // 0x588
            CTournamentChip *m_apChip[128];   // 0x5A0: one per map point, 0 where there is none

            static CTournamentRouteMan *m_pInstance;
    }; // size 0x7A0
}

#endif /* !CTOURNAMENTROUTEMAN_HPP_ */
