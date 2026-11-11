#ifndef MSW_CSTAGEDODGE11_HPP_
#define MSW_CSTAGEDODGE11_HPP_

#include "game/msw/CStage11Base.hpp"
#include "game/msw/CStageManDodge.hpp"

namespace msw {

    class CStageMdl;
    class CStageMdlListNode;

    // Dodge stage 11, the volcano stage (DodgeF), made by
    // CStageFactoryDodge. Size 0x67C: CStageManDodge (0x244), then
    // CStage11Base at 0x244, a DOL class that does the work (the court's
    // tilt and centre line, the bubbles, the volcano), then this class's
    // own members from 0x2B4 with the fire pillars at 0x2C4. This class
    // hands the stage manager's hooks on, counts the players in each
    // infield to tilt the court towards the fuller side, rolls the ground
    // model with the centre line, and gives the DODGE_PRM_S11_* tuning.
    //
    // The CStage11Base overrides are declared in the order of the slots
    // they add at the end of __vt__Q23msw13CStageDodge11 (0x564-0x5C0).
    class CStageDodge11 : public CStageManDodge, public CStage11Base {
        public:
            CStageDodge11(bcs::CTask *pParent);
            virtual ~CStageDodge11();

            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
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
            virtual void OnPeriodInit();
            virtual void OnGameStart();
            virtual void OnGameEnd();
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

            // The court only tilts while the game runs (from OnGameStart or
            // a turn's start to the game's end or message 0x40).
            virtual int IsRotReady_() const { return m_bRotReady; }
            virtual void AddCourtRotLevel_(long n);
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
            float m_Unk2B4;                             // 0x2B4
            CStageMdl *m_pBgMdl;                        // 0x2B8, the "stage<n>_bg" model
            CStageMdlListNode *m_pGroundNode;           // 0x2BC, the rolling "dod_cline_p" model
            int m_bRotReady;                            // 0x2C0
            STAGE11::CBgFirePillarCluster m_FirePillar; // 0x2C4
    };
}

#endif /* !MSW_CSTAGEDODGE11_HPP_ */
