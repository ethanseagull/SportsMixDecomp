#ifndef MSW_CSTAGEHOCKEYCRUSHSKATE85_HPP_
#define MSW_CSTAGEHOCKEYCRUSHSKATE85_HPP_

#include "game/msw/CStageHockeyCrushSkate.hpp"
#include "game/msw/CStgGmk_IceCube_Hockey.hpp"

namespace msw {

    // Hockey party stage 85 (HockeyF), made by CStageFactoryHockey: a crush
    // skate stage with ice cubes that slide with the tilt of the floor, and
    // two floor tunings picked by the party game's pattern
    // (GetLocPatternNo). Size 0xC74: CStageHockeyCrushSkate (0x258), then
    // the ice cube cluster at 0x258.
    //
    // The CStageRotBase functions the base class does not override come
    // after its slots in the primary vtable, in declaration order
    // (IsShipRotEnable, ShipStopTime2, ShipStopTimeRnd2, ShipRotInterval,
    // ShipSe).
    class CStageHockeyCrushSkate85 : public CStageHockeyCrushSkate {
        public:
            CStageHockeyCrushSkate85(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate85();

            virtual void Disp();
            virtual void Main_(float frame);
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual void OnSceneEntry();
            virtual void OnTurnInit();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
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

            // The ice cubes of the running stage, which must be this one
            // (used by the com players).
            static long GetIceCubeMaxNum();
            // Ice cube idx if it is active, else 0.
            static const CStgGmk_IceCube_Hockey *GetIceCube(long idx);

        private:
            CStgGmk_IceCubeCluster_Hockey m_IceCubeCluster; // 0x258
    };
}

#endif /* !MSW_CSTAGEHOCKEYCRUSHSKATE85_HPP_ */
