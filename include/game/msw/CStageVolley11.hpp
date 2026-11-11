#ifndef MSW_CSTAGEVOLLEY11_HPP_
#define MSW_CSTAGEVOLLEY11_HPP_

#include "game/msw/CStage11Base.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    class CStageMdl;
    class CStageMdlListNode;

    // Volley stage 11, the volcano stage (VolleyF), made by
    // CStageFactoryVolley. Size 0x630: CStageManVolley (0x200), then
    // CStage11Base at 0x200, a DOL class that does the work (the court's
    // tilt and centre line, the bubbles, the volcano), then this class's
    // own members from 0x270 with the fire pillars at 0x278. This class
    // hands the stage manager's hooks on, moves the centre line (and the
    // net on it) towards the team that trails on points, rolls the net's
    // base model with it, and gives the VOLLEY_PRM_S11_* tuning. The court
    // tilt is always ready (CStage11Base's IsRotReady_).
    //
    // The CStage11Base overrides are declared in the order of the slots
    // they add at the end of __vt__Q23msw14CStageVolley11 (0x564-0x5C4).
    class CStageVolley11 : public CStageManVolley, public CStage11Base {
        public:
            CStageVolley11(bcs::CTask *pParent);
            virtual ~CStageVolley11();

            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual const bcs::FVEC3 &GetNetPos() const;
            virtual void SendMessage(long msg, long arg0, long arg1, long arg2);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnGameEnd();
            virtual void OnGameInit();
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnEventStart();
            virtual const void *GetPrmTbl() const;
            virtual CStageNet *NewNet(bcs::CTask *pParent);

            // The floor is CStage11Base's.
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                return GetFloorAttribute__(rPos);
            }
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                GetFloorFlow__(rFlow, rPos, type);
            }

            virtual void AddCourtRotLevel_(long n);
            virtual bcs::FVEC3 Bubble_CalcCreatePos();
            virtual void OnCenterMoveBegin(long team);
            virtual void OnCenterMoveEnd(long team);
            virtual float GetParam_CourtAngle() const;
            virtual float GetParam_CourtRotSpeed() const;
            virtual float GetParam_CourtRotAcc() const;
            virtual long GetParam_CourtRotDiv() const;
            virtual float GetParam_ChrFlowSpeed() const;
            virtual float GetParam_CenterMoveSpeed() const;
            virtual float GetParam_CenterMoveAcc() const;
            virtual float GetParam_CourtCenterRange() const;
            virtual long GetParam_CourtCenterDiv() const;
            virtual float GetParam_CourtCenterFixRate() const;
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
            float m_Unk270;                             // 0x270, set to 0 and not used here
            CStageMdl *m_pBgMdl;                        // 0x274, the "stage<n>_bg" model
            STAGE11::CBgFirePillarCluster m_FirePillar; // 0x278
    };
}

#endif /* !MSW_CSTAGEVOLLEY11_HPP_ */
