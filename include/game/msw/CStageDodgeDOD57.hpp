#ifndef MSW_CSTAGEDODGEDOD57_HPP_
#define MSW_CSTAGEDODGEDOD57_HPP_

#include "game/msw/CStage17Base.hpp"
#include "game/msw/CStageManDodgeDodgeOrDie.hpp"

namespace msw {

    class CStgGmk_Trolley;

    // Dodge or Die stage 57, made by CStageFactoryDodge: the turntables of
    // stage 17 without the trolley. Size 0x2A0: CStageManDodgeDodgeOrDie
    // (0x244), then CStage17Base at 0x244, which turns the tables with the
    // DODGE_PRM_S57_* tuning, then one member of its own.
    //
    // IsRotReady_ is new; it and the CStage17Base functions follow in the
    // order of the new slots of __vt__Q23msw16CStageDodgeDOD57.
    class CStageDodgeDOD57 : public CStageManDodgeDodgeOrDie, public CStage17Base {
        public:
            CStageDodgeDOD57(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD57();

            virtual void Disp();
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg0, long arg1, long arg2);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                         nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPreInitialize() { Stage17_Create(); }
            virtual void OnGameStart();
            virtual void OnGameEnd();
            virtual void OnTurnInit();
            virtual void OnEntranceEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // Whether the turntables may turn: from the start of the
            // gimmick until it stops (or the game, a turn's entrance event
            // or message 0x3C ends it).
            virtual int IsRotReady_() const;

            virtual CStgGmk_Trolley *NewTrolley();
            virtual long GetTurnTableNum() const;
            virtual bool Stage17_IsGimmickReady() const;
            virtual float GetParam_TTInitialWait() const;
            virtual float GetParam_TTWait() const;
            virtual float GetParam_TTWaitRnd() const;
            virtual float GetParam_TTRadius() const;
            virtual float GetParam_TTOffset() const;
            virtual float GetParam_TTRotTime() const;
            virtual float GetParam_TTRotTimeRnd() const;
            virtual float GetParam_TTRotAccTime() const;
            virtual float GetParam_TTRotSpeed() const;
            virtual long GetParam_TTRotDirType() const;
            virtual float GetParam_TTOpenRate() const;
            virtual float GetParam_TTOpenAngle() const;
            virtual float GetParam_SignalTime() const;
            virtual float GetParam_SignalAfterTime() const;
            virtual float GetParam_SignalSEStopTime() const;
            virtual float GetParam_TrolleyInitRate() const;
            virtual float GetParam_TrolleyRate() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;

        private:
            int m_bRotReady; // 0x29C
    };
}

#endif /* !MSW_CSTAGEDODGEDOD57_HPP_ */
