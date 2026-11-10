#ifndef MSW_CSTGGMK_WATERWORKS_HOCKEY_HPP_
#define MSW_CSTGGMK_WATERWORKS_HOCKEY_HPP_

#include "bcs/BCS_Math.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "game/msw/CStgGmk_WaterPillar_Act.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    // The water pillars of the hockey fountain stage (stage 03): the model,
    // actor, driver and gimmick of one pillar and the cluster with the
    // HOCKEY_PRM_S03_* parameters. All of them live in
    // game/hockey/CStgGmk_WaterWorks_Hockey.cpp. The classes are declared in
    // the reverse order of their vtables in that file's .data.

    // The model, size 0x130 (nothing added to msw::CStgGmk_WaterPillar_Mdl).
    // Made by CStgGmk_WaterWorks_Hockey::NewMdl; the constructor is inline.
    class CStgGmk_WaterWorks_Hockey_Mdl : public CStgGmk_WaterPillar_Mdl {
        public:
            CStgGmk_WaterWorks_Hockey_Mdl(CStgGmk *pParent, long idx) : CStgGmk_WaterPillar_Mdl(pParent, idx) {}
            virtual ~CStgGmk_WaterWorks_Hockey_Mdl();

            virtual void Create();
            virtual float GetParam_MdlHeight() const;
    };

    // The actor, size 0x210 (nothing added to msw::CStgGmk_WaterPillar_Act).
    // Made by CStgGmk_WaterWorks_Hockey::NewAct; the constructor is inline.
    class CStgGmk_WaterWorks_Hockey_Act : public CStgGmk_WaterPillar_Act {
        public:
            CStgGmk_WaterWorks_Hockey_Act(CStgGmk *pParent) : CStgGmk_WaterPillar_Act(pParent) {}
            virtual ~CStgGmk_WaterWorks_Hockey_Act();

            virtual float GetParam_Radius() const;
            virtual float GetParam_Radius2() const;
            virtual float GetParam_Radius3() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_PrestartTime() const;
            virtual float GetParam_WaitTime() const;
            virtual float GetParam_StayTime() const;
            virtual float GetParam_Interval() const;
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_MoveResist() const;
    };

    // The driver, size 0xC (nothing added to msw::CStgGmk_WaterPillar_Drv).
    // Made by CStgGmk_WaterWorks_Hockey::NewDrv; the constructor is inline.
    class CStgGmk_WaterWorks_Hockey_Drv : public CStgGmk_WaterPillar_Drv {
        public:
            CStgGmk_WaterWorks_Hockey_Drv(CStgGmk *pParent) : CStgGmk_WaterPillar_Drv(pParent) {}
            virtual ~CStgGmk_WaterWorks_Hockey_Drv();

            virtual void Main(float rate);
    };

    // One pillar, size 0x74 (nothing added to msw::CStgGmk_WaterPillar).
    // Made by CStgGmk_WaterPillarCluster_Hockey::NewGmk; the constructor is
    // inline. NewAct and NewDrv are inline in the original: they are
    // emitted with the other inline functions, after the file's own.
    class CStgGmk_WaterWorks_Hockey : public CStgGmk_WaterPillar {
        public:
            CStgGmk_WaterWorks_Hockey(bcs::CTask *pParent) : CStgGmk_WaterPillar(pParent, 0) {}
            virtual ~CStgGmk_WaterWorks_Hockey();

            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_WaterWorks_Hockey_Act(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_WaterWorks_Hockey_Drv(this); }
    };

    // The cluster, size 0x44 (nothing added to
    // msw::CStgGmk_WaterPillarCluster). Two of them are members of
    // msw::CStageHockey03. The overrides are inline in the original (they
    // are emitted after the file's own functions, in reverse order of
    // these declarations).
    class CStgGmk_WaterPillarCluster_Hockey : public CStgGmk_WaterPillarCluster {
        public:
            CStgGmk_WaterPillarCluster_Hockey(bcs::CTask *pParent);
            virtual ~CStgGmk_WaterPillarCluster_Hockey();

            virtual long GetMaxNum() const { return 12; }
            virtual float GetParam_LocOffsetZ() const { return HOCKEY_PRM_S03_WC_OFFSET_Z; }
            virtual float GetParam_LocRadius() const { return HOCKEY_PRM_S03_WC_LOC_RADIUS; }
            virtual long GetParam_LocNum() const { return HOCKEY_PRM_S03_WC_LOC_NUM; }
            virtual long GetParam_NumPerGroup() const { return HOCKEY_PRM_S03_WC_LOC_NUM_PER_GRP; }
            virtual float GetParam_ActiveMargin() const { return 1.0f; }
            // The pillars stop for one interval (in frames).
            virtual float GetParam_StopTimer() const { return 60.0f * HOCKEY_PRM_S03_WATER_INTERVAL; }
            virtual float GetParam_ZoneAngle() const { return bcs::DEG_TO_RAD(HOCKEY_PRM_S03_WC_ZONE_ANGLE); }
            virtual float GetParam_ZoneWidth() const { return HOCKEY_PRM_S03_WC_ZONE_WIDTH; }
            virtual float GetParam_ZoneMargin() const { return HOCKEY_PRM_S03_WC_ZONE_MARGIN; }
            virtual CStgGmk *NewGmk(long heap) const { return new (heap) CStgGmk_WaterWorks_Hockey(GetParent()); }
    };
}

#endif /* !MSW_CSTGGMK_WATERWORKS_HOCKEY_HPP_ */
