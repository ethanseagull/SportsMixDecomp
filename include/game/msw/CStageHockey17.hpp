#ifndef CSTAGEHOCKEY17_HPP_
#define CSTAGEHOCKEY17_HPP_

#include "game/msw/CGoalMdlHockey.hpp"
#include "game/msw/CGoalObjHockey.hpp"
#include "game/msw/CStage17Base.hpp"
#include "game/msw/CStageManHockey.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    class CStgGmk_Trolley;

    // Hockey stage 0x11 (CGame::GameWork::m_nStage), the turntable and
    // trolley stage (HockeyF), made by CStageFactoryHockey. Size 0x274:
    // CStageManHockey (0x218), then CStage17Base at 0x218, a DOL class that
    // does the work (turntables, the trolley, signals and trains), then one
    // member of its own. This class hands the stage manager's hooks on to it
    // and gives it the HOCKEY_PRM_S17_* tuning and the trolley class
    // (msw::CStgGmk_Trolley_Hockey). Unlike the other sports it has only
    // one turntable, and its goals (CGoalObj17) have doors that open while
    // the trolley runs past them. The basketball version is
    // msw::CStageBasket17.
    //
    // IsRotReady_ is new; it and the CStage17Base functions follow in the
    // order of the new slots of __vt__Q23msw14CStageHockey17.
    class CStageHockey17 : public CStageManHockey, public CStage17Base {
        public:
            // The goal model of this stage: the hockey goal with the door
            // animations. Size 0x340 (nothing added).
            class CGoalMdl17 : public CGoalMdlHockey {
                public:
                    CGoalMdl17(CStgGmk *pParent) : CGoalMdlHockey(pParent) {}
                    virtual ~CGoalMdl17();
                    virtual const char *const *GetAnmNameTbl() const;
                    virtual const ANM_PARAM *GetAnmParamTbl() const;
            };

            // A goal with a door: the door opens while the trolley is near
            // the goal and the goal does nothing then. Size 0x649C.
            class CGoalObj17 : public CGoalObjHockey {
                public:
                    CGoalObj17(bcs::CTask *pParent, long idx) : CGoalObjHockey(pParent, idx) { m_bDoorOpen = 0; }
                    virtual ~CGoalObj17();
                    virtual void Main();

                    // Model 1 is this stage's goal model.
                    virtual CStgMdl *NewMdl(long heap, unsigned long idx)
                    {
                        if (idx == 1) {
                            return new (heap) CGoalMdl17(this);
                        }
                        return CGoalObjHockey::NewMdl(heap, idx);
                    }

                    virtual void DunkCancel();

                    // The signal lamps on the goal (CStageHockey17's
                    // SignalStart and SignalStop).
                    virtual void SignalStart();
                    virtual void SignalStop();
                    virtual void DoorOpen();
                    virtual void DoorClose();
                    virtual void DoorReset();

                private:
                    int m_bDoorOpen; // 0x6498
            };

            CStageHockey17(bcs::CTask *pParent);
            virtual ~CStageHockey17();

            virtual void Disp();
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetGoalX1Length(long goal) const { return HOCKEY_PRM_S17_GOAL_X1_LENGTH; }
            virtual float GetGoalRRRadius(long goal) const { return HOCKEY_PRM_S17_GOAL_RADIUS; }
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
            virtual CGoalObj *NewGoalObj(bcs::CTask *pParent, long goal) { return new CGoalObj17(pParent, goal); }
            virtual void OnSoundEvent(long event);

            // Whether the turntable may turn: from the start of the gimmick
            // until it stops (or the game or a turn's entrance event ends
            // it).
            virtual int IsRotReady_() const;

            virtual CStgGmk_Trolley *NewTrolley();
            virtual long GetTurnTableNum() const;
            // The signals light the goals' lamps too.
            virtual void SignalStart();
            virtual void SignalStop();
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

            // The number of trolleys (one).
            static long GetTrolleyMaxNum();
            // Trolley idx of the current stage (which must be this stage)
            // while it runs, else null.
            static const CStgGmk_Trolley *GetTrolley(long idx);
            // Static: the caller leaves r3 unset (HockeyF
            // CEventDemoHockey::CameraEvent).
            static void TrolleyForceReset();

        private:
            int m_bRotReady; // 0x270
    };
}

#endif /* !CSTAGEHOCKEY17_HPP_ */
