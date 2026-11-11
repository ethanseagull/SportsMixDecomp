#ifndef MSW_CSTAGEHOCKEYCRUSHSKATE_HPP_
#define MSW_CSTAGEHOCKEYCRUSHSKATE_HPP_

#include "game/msw/CStageManHockey.hpp"
#include "game/msw/CStageRotBase.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    class CStgGmk_Case;

    // (description guessed from the names) The common class of the hockey
    // "crush skate" party stages (HockeyF): CStageManHockey (0x218), then
    // CStageRotBase at 0x218 (vptr at 0x254). Size 0x258; it is not made on
    // its own: CStageHockeyCrushSkate85, 86 and 87 derive from it. It hands
    // the stage manager's hooks on to the rolling floor and gives it the
    // HOCKEY_PRM_S85_FLOOR0_* tuning; there are no cases (NewCase gives
    // none).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // slots of __vt__Q23msw22CStageHockeyCrushSkate follow in their order.
    // The inline ones are emitted in reverse declaration order, which fixes
    // where they stand.
    class CStageHockeyCrushSkate : public CStageManHockey, public CStageRotBase {
        public:
            CStageHockeyCrushSkate(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate();

            // Declared in this order because the weak copies are emitted in
            // reverse declaration order (original: GetFloorFlow,
            // GetFloorAttribute, GetWorldRotZ).
            virtual float GetWorldRotZ() const { return GetWorldRotZ__(); }
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                return GetFloorAttribute__(rPos);
            }
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
            {
                GetFloorFlow__(rFlow, rPos, type);
            }
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual long GetQPanelEnabled_() const { return 0; }
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnTurnInit();
            virtual void OnVictoryEventStart();
            virtual void OnGimmickStop();

            virtual const CStageMan *GetStageMan() const;
            // (name from the symbol) The party game's first parameter.
            virtual long GetLocPatternNo() const;
            virtual bool IsRotReady_() const;
            virtual float GetParam_CaseWidth() const { return HOCKEY_PRM_S15_CASE_WIDTH; }
            virtual float GetParam_CaseOffsetZ() const { return HOCKEY_PRM_S15_CASE_OFFSET_Z; }
            virtual float GetParam_ShipCycle() const;
            virtual float GetParam_ShipStopTime() const;
            virtual float GetParam_ShipStopTimeRnd() const;
            virtual float GetParam_ChrFlowVel() const;
            virtual float GetParam_ShipTransWidth() const;
            virtual float GetParam_ShipRotWidth() const;
            virtual float GetParam_BgRotSpeed() const;
            virtual CStgGmk_Case *NewCase() { return 0; }
    };
}

#endif /* !MSW_CSTAGEHOCKEYCRUSHSKATE_HPP_ */
