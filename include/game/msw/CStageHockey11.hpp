#ifndef MSW_CSTAGEHOCKEY11_HPP_
#define MSW_CSTAGEHOCKEY11_HPP_

#include "game/msw/CStage11Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    class CStageMdl;
    class CStageMdlListNode;

    // Hockey stage 11, the volcano stage (HockeyF), made by
    // CStageFactoryHockey. Size 0x654: CStageManHockey (0x218), then
    // CStage11Base at 0x218, a DOL class that does the work (the court's
    // tilt, the bubbles, the volcano), then this class's own members from
    // 0x288 with the fire pillars at 0x29C. Unlike DodgeF's CStageDodge11
    // the court does not follow the players: it tilts one way, then the
    // other, on a timer (HOCKEY_PRM_S11_COURT_WAIT), and the bubbles rise
    // on the court whose turn it is, in a band in front of the other
    // team's goal and away from it.
    //
    // The CStage11Base overrides are declared in the order of the slots
    // they add at the end of __vt__Q23msw14CStageHockey11 (0x588-0x5DC).
    class CStageHockey11 : public CStageManHockey, public CStage11Base {
        public:
            CStageHockey11(bcs::CTask *pParent);
            virtual ~CStageHockey11();

            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual void Main_(float frame);
            virtual void SendMessage(long msg, long arg0, long arg1, long arg2);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnGameInit();
            virtual void OnTurnInit();
            virtual void OnGameEnd();
            virtual void OnEntranceEventStart();
            virtual const void *GetPrmTbl() const;

            // The floor is CStage11Base's.
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                return GetFloorAttribute__(rPos);
            }
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                GetFloorFlow__(rFlow, rPos, type);
            }

            // The court only tilts while the game runs (from the game's
            // init to its end or the entrance event).
            virtual int IsRotReady_() const { return m_bRotReady; }
            virtual bcs::FVEC3 Bubble_CalcCreatePos();
            virtual void OnCtorCourtStatusChange(COURT_STATUS status);
            virtual void OnLoopCourtStatusChange(COURT_STATUS status);
            virtual void OnDtorCourtStatusChange(COURT_STATUS status);
            virtual float GetParam_CourtAngle() const;
            virtual float GetParam_CourtRotSpeed() const;
            virtual float GetParam_CourtRotAcc() const;
            virtual float GetParam_ChrFlowSpeed() const;
            virtual float GetParam_CourtCenterRange() const;
            virtual long GetParam_SE(long type) const;
            virtual long GetParam_CVID(long type) const;
            virtual float GetParam_BubbleZoneZ() const;
            virtual float GetParam_BubbleInitInterval() const;
            virtual float GetParam_BubbleInterval() const;
            virtual float GetParam_BubbleScale() const;
            virtual float GetParam_BubbleDownFrm() const;
            virtual float GetParam_BubblePreStartFrm() const;
            virtual float GetParam_BubbleJumpHeight();
            virtual float GetParam_BubbleJumpFrm();
            virtual float GetParam_BubbleJumpDOF();

        private:
            float m_Unk288;                             // 0x288, set to 0 and not used here
            CStageMdl *m_pBgMdl;                        // 0x28C, the "stage<n>_bg" model
            COURT_STATUS m_nTiltStatus;                 // 0x290, the last tilt (2 or 3)
            float m_fTiltWait;                          // 0x294, frames left before the next tilt
            int m_bRotReady;                            // 0x298
            STAGE11::CBgFirePillarCluster m_FirePillar; // 0x29C
    };
}

#endif /* !MSW_CSTAGEHOCKEY11_HPP_ */
