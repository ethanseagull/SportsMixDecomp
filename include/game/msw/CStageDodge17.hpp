#ifndef MSW_CSTAGEDODGE17_HPP_
#define MSW_CSTAGEDODGE17_HPP_

#include "game/msw/CStage17Base.hpp"
#include "game/msw/CStageManDodge.hpp"

namespace msw {

    class CStgGmk_Trolley;

    // Dodge stage 17, the turntable and trolley stage (DodgeF), made by
    // CStageFactoryDodge. Size 0x2A0: CStageManDodge (0x244), then
    // CStage17Base at 0x244, a DOL class that does the work (turntables,
    // the trolley, signals and trains), then one member of its own. This
    // class hands the stage manager's hooks on to it and gives it the
    // DODGE_PRM_S17_* tuning and the trolley class
    // (msw::CStgGmk_Trolley_Dodge).
    //
    // IsRotReady_ is new; it and the CStage17Base functions follow in the
    // order of the new slots of __vt__Q23msw13CStageDodge17.
    class CStageDodge17 : public CStageManDodge, public CStage17Base {
        public:
            CStageDodge17(bcs::CTask *pParent);
            virtual ~CStageDodge17();

            virtual void Disp();
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg0, long arg1, long arg2);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual float GetBaseOuterFront_() const;
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
            virtual float GetParam_TrolleyPreWait() const;
            virtual float GetParam_TrolleyInitRate() const;
            virtual float GetParam_TrolleyRate() const;
            virtual long GetParam_TrolleyDirType() const;
            virtual float GetParam_TrolleyLocOffset() const;
            virtual long GetParam_Effect(long type) const;

            // The number of trolleys, and trolley idx of the current stage
            // (which must be this stage) while it runs, else null.
            static long GetTrolleyMaxNum();
            static const CStgGmk_Trolley *GetTrolley(long idx);
            // Static: CEventDemoDodge::CameraEvent leaves r3 unset. Stage
            // 0x11 (GameWork::GetStage) is the one with the trolley.
            static void TrolleyForceReset();

        private:
            int m_bRotReady; // 0x29C
    };
}

#endif /* !MSW_CSTAGEDODGE17_HPP_ */
