#ifndef MSW_CSTAGEHOCKEY09_HPP_
#define MSW_CSTAGEHOCKEY09_HPP_

#include "game/msw/CStage09Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    class CStgGmk_Container;

    // Hockey stage 09, the conveyor belt stage (HockeyF), made by
    // CStageFactoryHockey. Size 0x240: CStageManHockey (0x218), then
    // CStage09Base at 0x218, a DOL class that runs the conveyors, then this
    // class's own members. Two belts run along the two ends of the court
    // (conveyor 0 at the back, 1 at the front), each turning one way or the
    // other at random after a stop; the belts carry containers
    // (msw::CStgGmk_Container_Hockey) holding bombs, treasure or nothing,
    // which this class drops on them at intervals.
    //
    // Overrides are declared in the order of their vtable slots; the
    // CStage09Base functions and the new ones follow in the order of the new
    // slots of __vt__Q23msw14CStageHockey09.
    class CStageHockey09 : public CStageManHockey, public CStage09Base {
        public:
            // A belt with this stage's tuning. It also adds up how far it
            // has run since the last container was dropped on it.
            class CConveyorState : public CConveyor {
                public:
                    CConveyorState() { m_fInterval = 0.0f; }

                    virtual void Main(float frame);
                    virtual void Reset();
                    virtual float GetParam_CvSpeed() const;
                    virtual float GetParam_AnmFlow() const;
                    virtual float GetParam_MdlAnmDir() const;

                    virtual void SetInterval(float interval) { m_fInterval = interval; }
                    virtual float GetInterval() const { return m_fInterval; }

                private:
                    float m_fInterval; // 0x50, distance run since the last container
            };

            CStageHockey09(bcs::CTask *pParent);
            virtual ~CStageHockey09();

            virtual void Disp();
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPreInitialize();
            virtual void OnGoalIn(long goal, GOAL_IN_TYPE type);
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // The depth of the court's front and back edges.
            virtual float GetConveyorFrontPos() const { return GetCourtFront(); }
            virtual float GetConveyorBackPos() const { return GetCourtBack(); }
            virtual long GetConveyorNum() const;
            virtual CConveyor *NewConveyor() const;
            virtual void Conveyor_OnStart(long no, CConveyor::FLOW_DIR dir, int, float);
            virtual void Conveyor_OnSetSpeedLevel(long no, long, long);
            virtual void Conveyor_OnStop(long no);
            // Called from Main_ every frame: runs the belts, places their
            // models and drops the containers.
            virtual void ConveyorUpdate(float frame);
            // Removes every container.
            virtual void Container_Clear();
            virtual CStgSeSlot *GetSeSlot();
            virtual long GetSeSlotBeg() const;

            // The number of containers on the court, and container idx
            // (null past the last).
            static long GetContainerMaxNum();
            static const CStgGmk_Container *GetContainer(long idx);

        private:
            float m_afTreasureTimer[2]; // 0x230, per belt: frames until treasure may come again
            long m_anTreasureNum[2];    // 0x238, per belt: treasure containers dropped this period
    };
}

#endif /* !MSW_CSTAGEHOCKEY09_HPP_ */
