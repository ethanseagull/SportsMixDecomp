#ifndef MSW_CSTAGEDODGE09_HPP_
#define MSW_CSTAGEDODGE09_HPP_

#include "game/msw/CStage09Base.hpp"
#include "game/msw/CStageManDodge.hpp"

namespace msw {

    // Dodge stage 09, the conveyor belt stage (DodgeF), made by
    // CStageFactoryDodge. Size 0x268: CStageManDodge (0x244), then
    // CStage09Base at 0x244, a DOL class that runs the conveyors, then this
    // class's own members. The court is one conveyor belt; it runs towards
    // a random direction between turns and speeds up on a stage message.
    //
    // Overrides are declared in the order of their vtable slots; the
    // CStage09Base functions and the new ConveyorUpdate follow in the order
    // of the new slots of __vt__Q23msw14CStageDodge09.
    class CStageDodge09 : public CStageManDodge, public CStage09Base {
        public:
            // The court's conveyor, with this stage's tuning.
            class CConveyorState : public CConveyor {
                public:
                    virtual float GetParam_CvSpeed() const;
                    virtual float GetParam_JumpAccFollow() const;
                    virtual float GetParam_JumpAccAgainst() const;
                    virtual float GetParam_AnmFlow() const;
                    virtual float GetParam_MdlAnmDir() const;
            };

            CStageDodge09(bcs::CTask *pParent);
            virtual ~CStageDodge09();

            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetFloorFlowRate(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long GetQPanelEnabled_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPreInitialize();
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual long GetConveyorNum() const;
            virtual CConveyor *NewConveyor() const;
            // Called from Main_ every frame: picks the belt's direction and
            // runs the conveyors.
            virtual void ConveyorUpdate(float frame);
            // (name from the symbol) True while the belt flows along the
            // court: no guard team, and the conveyor relation is 1.
            virtual bool IsConveyorParallel() const;
            virtual CStgSeSlot *GetSeSlot();
            virtual long GetSeSlotBeg() const;

        private:
            int m_bActive;                // 0x25C, the conveyors run (set by a message)
            CStageMdlListNode *m_pMdlNode; // 0x260, the belt model's node
            long m_nGuardTeam;            // 0x264, set by message 0x27; -1 when none
    };
}

#endif /* !MSW_CSTAGEDODGE09_HPP_ */
