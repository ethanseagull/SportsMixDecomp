#ifndef MSW_CSTAGEVOLLEY09_HPP_
#define MSW_CSTAGEVOLLEY09_HPP_

#include "game/msw/CStage09Base.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // Volley stage 09, the conveyor belt stage, made by
    // CStageFactoryVolley. Size 0x21C: CStageManVolley (0x200), then
    // CStage09Base at 0x200, a DOL class that runs the conveyors, then this
    // class's own member. Each half of the court is a conveyor belt; each
    // turn they start in a random direction, and a stage message starts one
    // of them.
    //
    // Overrides are declared in the order of their vtable slots; the
    // CStage09Base functions and the new ConveyorUpdate follow in the order
    // of the new slots of __vt__Q23msw14CStageVolley09.
    class CStageVolley09 : public CStageManVolley, public CStage09Base {
        public:
            // The court's conveyor, with this stage's tuning.
            class CConveyorState : public CConveyor {
                public:
                    virtual float GetParam_CvSpeed() const;
                    virtual float GetParam_AnmFlow() const;
                    virtual float GetParam_MdlAnmDir() const;
            };

            CStageVolley09(bcs::CTask *pParent);
            virtual ~CStageVolley09();

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
            virtual void OnTurnInit();
            virtual void OnEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual long GetConveyorNum() const;
            virtual CConveyor *NewConveyor() const;
            // Called from Main_ every frame: picks the belt's direction and
            // runs the conveyors.
            virtual void ConveyorUpdate(float frame);
            virtual CStgSeSlot *GetSeSlot();
            virtual long GetSeSlotBeg() const;

        private:
            // (name guessed) The team whose target goal's belt OnGimmickReset
            // holds still; -1 for none. Set by stage message 0x27.
            long m_nStopTeam; // 0x218
    };
}

#endif /* !MSW_CSTAGEVOLLEY09_HPP_ */
