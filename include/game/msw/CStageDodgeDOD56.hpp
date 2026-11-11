#ifndef MSW_CSTAGEDODGEDOD56_HPP_
#define MSW_CSTAGEDODGEDOD56_HPP_

#include "game/msw/CStage15Base.hpp"
#include "game/msw/CStageManDodgeDodgeOrDie.hpp"

namespace msw {

    class CStgGmk_Case;

    // Dodge or Die stage 56, made by CStageFactoryDodge: the ship of stage 15
    // without its cases and with no rain changes (it is always rainy). Size
    // 0x2D0: CStageManDodgeDodgeOrDie (0x244), then CStage15Base at 0x244,
    // which rolls the ship with the DODGE_PRM_S56_* tuning.
    //
    // The CStage15Base functions follow in the order of the new slots of
    // __vt__Q23msw16CStageDodgeDOD56.
    class CStageDodgeDOD56 : public CStageManDodgeDodgeOrDie, public CStage15Base {
        public:
            CStageDodgeDOD56(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD56();

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
            virtual void OnTurnInit();
            virtual void OnPeriodInit();
            virtual void OnTurnEnd();
            virtual void OnGameEnd();
            virtual void OnEntranceEventStart();
            virtual void OnEntranceEventGmkStart();
            virtual void OnEntranceEventGmkEnd();
            virtual void OnSoundEvent(long event);
            virtual const void *GetPrmTbl() const;

            // The ship's roll. Declared in reverse order: they are inline, and
            // the original emits them in the order OnGimmickCancel, ...,
            // GetWorldRotZ (weak inlines come out in reverse declaration order).
            virtual float GetWorldRotZ() const { return GetWorldRotZ__(); }
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                return GetFloorAttribute__(rPos);
            }
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                GetFloorFlow__(rFlow, rPos, type);
            }
            virtual void OnGimmickReset() { Stage15_OnGimmickReset(); }
            virtual void OnGimmickCancel() { Stage15_OnGimmickCancel(); }

            virtual const CStageMan *GetStageMan() const;
            virtual void Rainy_Set(int bRainy);
            virtual void Rainy_Reset();
            virtual void Rainy_Update(float frame);
            virtual int IsRainy() const;
            virtual long GetParam_CaseNum() const;
            virtual float GetParam_ShipCycle() const;
            virtual float GetParam_ShipStopTime() const;
            virtual float GetParam_ShipStopTimeRnd() const;
            virtual float GetParam_ChrFlowVel() const;
            virtual float GetParam_ShipRotWidth() const;
            virtual long GetParam_ShipSe() const;
            virtual CStgGmk_Case *NewCase();
    };
}

#endif /* !MSW_CSTAGEDODGEDOD56_HPP_ */
