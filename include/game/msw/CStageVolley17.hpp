#ifndef CSTAGEVOLLEY17_HPP_
#define CSTAGEVOLLEY17_HPP_

#include "game/msw/CStage17Base.hpp"
#include "game/msw/CStageManVolley.hpp"
#include "game/msw/CStageNetVolley.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    class CStgGmk_Trolley;

    // Volley stage 0x11 (CGame::GameWork::m_nStage), the turntable and
    // trolley stage (VolleyF), made by CStageFactoryVolley. Size 0x25C:
    // CStageManVolley (0x200), then CStage17Base at 0x200, a DOL class that
    // does the work (turntable, the trolley, signals and trains), then one
    // member of its own. This class hands the stage manager's hooks on to it
    // and gives it the VOLLEY_PRM_S17_* tuning and the trolley class
    // (msw::CStgGmk_Trolley_Volley). Like the hockey version it has one
    // turntable; its net (CStageNetVolley17) plays a motion while the
    // trolley passes under it. The hockey version is msw::CStageHockey17.
    //
    // IsRotReady_ and IsTrolleyTransit are new; they and the CStage17Base
    // functions follow in the order of the new slots of
    // __vt__Q23msw14CStageVolley17.
    class CStageVolley17 : public CStageManVolley, public CStage17Base {
        public:
            // The net of this stage, size 0x74. It switches its motion
            // when the trolley starts or stops passing under it.
            class CStageNetVolley17 : public CStageNetVolley {
                public:
                    CStageNetVolley17(bcs::CTask *pParent, CStageVolley17 *pStage)
                        : CStageNetVolley(pParent), m_pStage(pStage), m_bTransit(0)
                    {
                    }
                    virtual ~CStageNetVolley17() {}

                    virtual void Main();
                    virtual void Reset();
                    // Idle motion 1 while the trolley passes, else 0.
                    virtual long GetIdleMotID() const;

                private:
                    CStageVolley17 *m_pStage; // 0x6C
                    int m_bTransit;           // 0x70, the trolley passed under the net at the last check
            };

            CStageVolley17(bcs::CTask *pParent);
            virtual ~CStageVolley17();

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
            virtual void OnPostInitialize();
            virtual void OnGameStart();
            virtual void OnGameEnd();
            virtual void OnTurnInit();
            virtual void OnEntranceEventStart();
            virtual void OnArtsShootStart();
            virtual void OnArtsShootEnd();
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual CStageNet *NewNet(bcs::CTask *pParent);
            virtual void OnSoundEvent(long event);

            // Whether the turntable may turn: from the start of the gimmick
            // until it stops (or the game ends).
            virtual int IsRotReady_() const;

            virtual CStgGmk_Trolley *NewTrolley();
            virtual long GetTurnTableNum() const;
            // Whether the trolley's box meets the court widened by five
            // units (the trolley passes under the net).
            virtual int IsTrolleyTransit() const;
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
            virtual float GetParam_TrolleyRate() const;
            virtual float GetParam_TrolleyInitRate() const;
            virtual long GetParam_TrolleyDirType() const;
            virtual float GetParam_TrolleyLocOffset() const;
            virtual long GetParam_Effect(long type) const;

            // The number of trolleys (one).
            static long GetTrolleyMaxNum();
            // Trolley idx of the current stage (which must be this stage)
            // while it runs, else null.
            static const CStgGmk_Trolley *GetTrolley(long idx);
            // Static: the caller leaves r3 unset (VolleyF
            // CEventDemoVolley::CameraEvent).
            static void TrolleyForceReset();

        private:
            int m_bRotReady; // 0x258
    };
}

#endif /* !CSTAGEVOLLEY17_HPP_ */
