#ifndef MSW_CSTAGEBASKET09_HPP_
#define MSW_CSTAGEBASKET09_HPP_

#include "game/msw/CStage09Base.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 09, the conveyor belt stage (BasketF), made by
    // CStageFactoryBasket. Size 0x23C: CStageManBasket (0x218), then
    // CStage09Base at 0x218, a DOL class that runs the conveyors, then this
    // class's own members. The court is one conveyor belt; it runs towards
    // the goal being attacked (or a fixed direction, BASKET_PRM_S09_CV_DIR_0)
    // and speeds up on a stage message.
    //
    // Overrides are declared in the order of their vtable slots; the
    // CStage09Base functions and the new ConveyorUpdate follow in the order
    // of the new slots of __vt__Q23msw14CStageBasket09.
    class CStageBasket09 : public CStageManBasket, public CStage09Base {
        public:
            // The court's conveyor, with this stage's tuning.
            class CConveyorState : public CConveyor {
                public:
                    virtual float GetParam_CvSpeed() const;
                    virtual float GetParam_AnmFlow() const;
                    virtual float GetParam_MdlAnmDir() const;
            };

            CStageBasket09(bcs::CTask *pParent);
            virtual ~CStageBasket09();

            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long GetQPanelEnabled_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPreInitialize();
            virtual void OnTurnInit();
            virtual void OnEventStart();
            virtual void OnArtsShootStart();
            virtual void OnArtsShootEnd();
            virtual void OnGimmickReset();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual long GetConveyorNum() const;
            virtual CConveyor *NewConveyor() const;
            virtual void Conveyor_OnStart(long no, CConveyor::FLOW_DIR dir, int, float);
            virtual void Conveyor_OnStartAll(CConveyor::FLOW_DIR dir, long, float);
            // Called from Main_ every frame: picks the belt's direction and
            // runs the conveyors.
            virtual void ConveyorUpdate(float frame);
            virtual CStgSeSlot *GetSeSlot();
            virtual long GetSeSlotBeg() const;

        private:
            long m_nGuardTeam;     // 0x230, set by message 0x27; -1 when none
            long m_Unk234;         // 0x234, not touched here
            int m_bConveyorStarted; // 0x238, the belt has been started since the last stop
    };
}

#endif /* !MSW_CSTAGEBASKET09_HPP_ */
