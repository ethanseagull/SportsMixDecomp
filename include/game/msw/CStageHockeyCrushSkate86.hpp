#ifndef MSW_CSTAGEHOCKEYCRUSHSKATE86_HPP_
#define MSW_CSTAGEHOCKEYCRUSHSKATE86_HPP_

#include "game/msw/CStage11Base.hpp"
#include "game/msw/CStageHockeyCrushSkate.hpp"
#include "game/msw/CStgGmk_FirePillar_Hockey.hpp"

namespace msw {

    // Hockey party stage 86 (HockeyF), made by CStageFactoryHockey: a crush
    // skate stage with a fire pillar (CStgGmk_FirePillar_Hockey) that starts
    // burning some seconds into each turn, background fire pillars
    // (STAGE11::CBgFirePillarCluster, as on stage 11) and four floor tunings
    // picked by the party game's pattern (GetLocPatternNo). Size 0x618:
    // CStageHockeyCrushSkate (0x258), then the members from 0x258.
    //
    // The CStageRotBase functions the base class does not override come
    // after its slots in the primary vtable, in declaration order
    // (IsShipRotEnable, ShipStopTime2, ShipStopTimeRnd2, ShipRotInterval,
    // ShipSe); the Fire_* functions add slots after them, in their order.
    class CStageHockeyCrushSkate86 : public CStageHockeyCrushSkate {
        public:
            enum {
                FIRE_PILLAR_NUM = 1
            };

            CStageHockeyCrushSkate86(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate86();

            virtual void Disp();
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnTurnInit();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
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

            // Counts the start wait down and lights the pillars.
            virtual void Fire_Update(float frame);
            virtual void Fire_StopAll();
            // Back to the start, burning on their own (ResetAll) or not
            // (InvalidateAll).
            virtual void Fire_ResetAll();
            virtual bool Fire_IsSwooshAll() const;
            virtual void Fire_InvalidateAll();

            // The fire pillars of the running stage, which must be this one
            // (used by the com players).
            static long GetFirePillarMaxNum();
            // Fire pillar idx if it is burning, else 0.
            static CStgGmk_FirePillar_Hockey *GetFirePillar(long idx);

        private:
            CStgGmk_FirePillar_Hockey *m_pFirePillar[FIRE_PILLAR_NUM]; // 0x258
            float m_fFireWait;                                       // 0x25C, frames before the pillars start; negative when started
            STAGE11::CBgFirePillarCluster m_BgFirePillar;             // 0x260
    };
}

#endif /* !MSW_CSTAGEHOCKEYCRUSHSKATE86_HPP_ */
