#ifndef MSW_CSTAGEHOCKEYCRUSHSKATE87_HPP_
#define MSW_CSTAGEHOCKEYCRUSHSKATE87_HPP_

#include "game/msw/CStageHockeyCrushSkate.hpp"

namespace msw {

    // Hockey party stage 87 (HockeyF), made by CStageFactoryHockey: a crush
    // skate stage with a hole in the court (GetBaseOuter*) and four floor
    // tunings picked by the party game's pattern (GetLocPatternNo). Size
    // 0x258, nothing added to msw::CStageHockeyCrushSkate.
    //
    // The CStageRotBase functions the base class does not override come
    // after its slots in the primary vtable, in declaration order
    // (IsShipRotEnable, ShipStopTime2, ShipStopTimeRnd2, ShipRotInterval,
    // ShipSe).
    class CStageHockeyCrushSkate87 : public CStageHockeyCrushSkate {
        public:
            CStageHockeyCrushSkate87(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate87();

            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual float GetBaseOuterRadius_() const;
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual float GetParam_ShipCycle() const;
            virtual float GetParam_ShipStopTime() const;
            virtual float GetParam_ShipStopTimeRnd() const;
            virtual float GetParam_ChrFlowVel() const;
            virtual float GetParam_ShipRotWidth() const;
            virtual float GetParam_BgRotSpeed() const;
            virtual bool GetParam_IsShipRotEnable() const;
            virtual float GetParam_ShipStopTime2() const;
            virtual float GetParam_ShipStopTimeRnd2() const;
            virtual float GetParam_ShipRotInterval() const;
            virtual long GetParam_ShipSe() const;
    };
}

#endif /* !MSW_CSTAGEHOCKEYCRUSHSKATE87_HPP_ */
