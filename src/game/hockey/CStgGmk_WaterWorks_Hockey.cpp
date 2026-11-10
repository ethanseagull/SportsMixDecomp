#include "game/msw/CStgGmk_WaterWorks_Hockey.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/HockeyPrm.hpp"

// The water pillars of the hockey fountain stage (stage 03). One file,
// though the cutter split it in five units by class name: the weak inline
// functions of msw::CStgGmk_WaterPillarCluster, msw::CStgGmk_WaterPillar,
// msw::CStgDrv, msw::CStgAct, msw::CStgGmkAct and the water pillar actor
// and model, and the six this-adjusting thunks, are emitted by this file,
// and its .data holds the five vtables.

namespace {

    // Suffixes of the pillar's animations, appended to the model name.
    const char *s_AnmSuffix[] = { "", "_start", "_end", "_loop", 0, 0 };

    // The pillar models; CStgGmk_WaterPillar_Mdl::GetMdlIdx picks one.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s03_gm_funsui_01", 0, 0, 0, s_AnmSuffix, "s03_gm_funsui_01", 0, 0 },
    };
}

msw::CStgGmk_WaterWorks_Hockey_Mdl::~CStgGmk_WaterWorks_Hockey_Mdl()
{
}

void msw::CStgGmk_WaterWorks_Hockey_Mdl::Create()
{
    long idx = GetMdlIdx();
    idx = idx > 0 ? 0 : (idx < 0 ? 0 : idx);
    CGenericMVAGimmickCreator creator(CLoadData::GetRes(2), &s_MdlParam[idx]);
    CStgMdl::Create(creator);
    SetAnm(0, 0, 1.0f, 0.0f, 0.0f, 8);
}

float msw::CStgGmk_WaterWorks_Hockey_Mdl::GetParam_MdlHeight() const
{
    return 2.5f;
}

msw::CStgGmk_WaterWorks_Hockey_Act::~CStgGmk_WaterWorks_Hockey_Act()
{
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Radius() const
{
    return HOCKEY_PRM_S03_WATER_RADIUS;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Radius2() const
{
    return HOCKEY_PRM_S03_WATER_RADIUS2;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Radius3() const
{
    return HOCKEY_PRM_S03_WATER_RADIUS3;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Height() const
{
    return HOCKEY_PRM_S03_WATER_HEIGHT;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_PrestartTime() const
{
    return HOCKEY_PRM_S03_WATER_PRESTART_TIME;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_WaitTime() const
{
    return 15.0f;
}

// The parameters are in seconds; the actor counts frames.

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_StayTime() const
{
    return 60.0f * HOCKEY_PRM_S03_WATER_STAY_TIME;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Interval() const
{
    return 60.0f * HOCKEY_PRM_S03_WATER_INTERVAL;
}

long msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_Effect(long type) const
{
    switch (type) {
        case 2:
            return 0x700;
        case 3:
            return 0x6FC;
        case 4:
            return 0x6FD;
        case 5:
            return 0x6FE;
        case 6:
            return 0x6FF;
    }
    return -1;
}

float msw::CStgGmk_WaterWorks_Hockey_Act::GetParam_MoveResist() const
{
    return HOCKEY_PRM_S03_WATER_RESIST;
}

msw::CStgGmk_WaterWorks_Hockey_Drv::~CStgGmk_WaterWorks_Hockey_Drv()
{
}

void msw::CStgGmk_WaterWorks_Hockey_Drv::Main(float rate)
{
    CStgGmk_WaterPillar_Drv::Main(rate);
}

msw::CStgGmk_WaterWorks_Hockey::~CStgGmk_WaterWorks_Hockey()
{
}

msw::CStgMdl *msw::CStgGmk_WaterWorks_Hockey::NewMdl(long heap, unsigned long idx)
{
    return new (heap) CStgGmk_WaterWorks_Hockey_Mdl(this, idx);
}

msw::CStgGmk_WaterPillarCluster_Hockey::CStgGmk_WaterPillarCluster_Hockey(bcs::CTask *pParent)
    : CStgGmk_WaterPillarCluster(pParent)
{
}

msw::CStgGmk_WaterPillarCluster_Hockey::~CStgGmk_WaterPillarCluster_Hockey()
{
}
