#ifndef MSW_CSTAGEHOCKEY15_HPP_
#define MSW_CSTAGEHOCKEY15_HPP_

#include "game/msw/CStage15Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    class CStgGmk_Case;

    // Hockey stage 15, the ship stage (HockeyF), made by CStageFactoryHockey.
    // CStageManHockey (0x218), then CStage15Base at 0x218 (size 0x8C), a DOL
    // class that does the work (the ship's roll, the cases, rain and
    // lightning). This class hands the stage manager's hooks on to it and
    // gives it the HOCKEY_PRM_S15_* tuning and the case class
    // (msw::CStgGmk_Case_Hockey).
    //
    // The CStage15Base functions follow in the order of the new slots of
    // __vt__Q23msw14CStageHockey15. Unlike the other modes, the goals drift
    // with the ship's roll (GoalFlowInit, GoalFlowUpdate).
    class CStageHockey15 : public CStageManHockey, public CStage15Base {
        public:
            CStageHockey15(bcs::CTask *pParent);
            virtual ~CStageHockey15();

            virtual void Main_(float frame);
            virtual void SendMessage(long msg, long arg0, long arg1, long arg2);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnTurnEnd();
            virtual void OnEntranceEventStart();
            virtual void OnEntranceEventGmkStart();
            virtual void OnEntranceEventGmkEnd();
            virtual void OnSoundEvent(long event);
            virtual const void *GetPrmTbl() const;
            virtual void OnEventEnd();
            virtual void OnEventStart();
            virtual void OnGimmickCancel();
            virtual void OnGimmickReset();
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual float GetWorldRotZ() const;

            virtual const CStageMan *GetStageMan() const;
            // Puts the goals back to their own places, or moves them with
            // the ship's roll (when HOCKEY_PRM_S15_GOAL_MOVE is set).
            virtual void GoalFlowInit();
            virtual void GoalFlowUpdate(float frame);
            virtual long GetParam_CaseNum() const;
            virtual float GetParam_CaseWidth() const;
            virtual float GetParam_CaseOffsetZ() const;
            virtual float GetParam_CaseSpacingZ() const;
            virtual void GetParam_CaseLocateRect(float *pLeft, float *pRight, float *pFront, float *pBack);
            virtual float GetParam_ShipCycle() const;
            virtual float GetParam_ShipStopTime() const;
            virtual float GetParam_ShipStopTimeRnd() const;
            virtual float GetParam_ChrFlowVel() const;
            virtual float GetParam_ShipRotWidth() const;
            virtual bool GetParam_IsShipRotEnable() const;
            virtual long GetParam_ShipSe() const;
            virtual bcs::BCS_Color GetParam_FlashColor() const;
            virtual float GetParam_FlashFrm() const;
            virtual float GetParam_FlashGain() const;
            virtual float GetParam_FlashDecay() const;
            virtual float GetParam_RainyInterval() const;
            virtual float GetParam_RainyTime() const;
            virtual CStgGmk_Case *NewCase();

            // The number of cases, and case idx of the current stage (which
            // must be this stage); for the case actors.
            static long GetCaseMaxNum();
            static const CStgGmk_Case *GetCase(long idx);
    };
}

#endif /* !MSW_CSTAGEHOCKEY15_HPP_ */
