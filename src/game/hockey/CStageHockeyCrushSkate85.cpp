// msw::CStageHockeyCrushSkate85: hockey party stage 85, a crush skate
// stage with ice cubes that slide with the tilt of the floor
// (CStgGmk_IceCubeCluster_Hockey). The rolling floor has four tunings; the
// party game's pattern (GetLocPatternNo) picks one.
#include "game/msw/CStageHockeyCrushSkate85.hpp"

#include "game/msw/HockeyPrm.hpp"

namespace {

    // (names guessed) One entry of the parameter table GetPrmTbl gives: the
    // type of a tuning value, the value, and the offset of the word it
    // fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    using namespace msw;

    const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_COURT_WIDTH, 0x00 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_COURT_DEPTH, 0x04 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_COURT_RADIUS, 0x08 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_BG_ROT_SPEED, 0x0C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_FLOOR0_ROT_ENABLE, 0x10 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_INIT_WAIT, 0x14 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_CYCLE, 0x18 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_STOP_TIME, 0x1C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND, 0x20 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_STOP_TIME2, 0x24 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND2, 0x28 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_CHR_FLOW_VEL, 0x2C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR0_ROT_ANGLE, 0x30 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE0_ACC, 0x34 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE0_DEC, 0x38 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE0_INTERVAL, 0x3C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICE0_LOC_PAT_TYPE, 0x40 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICE0_LOC_PAT_NO, 0x44 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE0_LOC_RND_X, 0x48 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE0_LOC_RND_Z, 0x4C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_FLOOR1_ROT_ENABLE, 0x50 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_INIT_WAIT, 0x54 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_CYCLE, 0x58 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_STOP_TIME, 0x5C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND, 0x60 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_STOP_TIME2, 0x64 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND2, 0x68 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_CHR_FLOW_VEL, 0x6C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_FLOOR1_ROT_ANGLE, 0x70 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE1_ACC, 0x74 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE1_DEC, 0x78 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE1_INTERVAL, 0x7C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICE1_LOC_PAT_TYPE, 0x80 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICE1_LOC_PAT_NO, 0x84 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE1_LOC_RND_X, 0x88 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICE1_LOC_RND_Z, 0x8C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_MDL_RADIUS, 0x90 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_MDL_HEIGHT, 0x94 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_RADIUS, 0x98 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_HEIGHT, 0x9C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_HEIGHT2, 0xA0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_LOC_POS_Y, 0xA4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_OFS, 0xA8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_OFS2, 0xAC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_EFF_VEL, 0xB0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_VEL, 0xB4 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICECUBE_PUSH_POWER, 0xB8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST, 0xBC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_ACC, 0xC0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_ACC2, 0xC4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_ACC3, 0xC8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_ATK_ACC4, 0xCC },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S85_ICECUBE_PUSH_POWER_2, 0xD0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST_2, 0xD4 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT2 = 2,
    SE_AMBIENT0 = 0xDB5,
    SE_AMBIENT2 = 0xDB6,
    SE_SHIP = 0xDB4
};

const void *msw::CStageHockeyCrushSkate85::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockeyCrushSkate85::CStageHockeyCrushSkate85(bcs::CTask *pParent)
    : CStageHockeyCrushSkate(pParent), m_IceCubeCluster(this)
{
}

msw::CStageHockeyCrushSkate85::~CStageHockeyCrushSkate85()
{
}

// The ice cubes on the floor are pushed along by its roll.
void msw::CStageHockeyCrushSkate85::Main_(float frame)
{
    CStageHockeyCrushSkate::Main_(frame);
    m_IceCubeCluster.Main(frame);

    long num = m_IceCubeCluster.GetMaxNum();
    bcs::FVEC3 accel(GetRotRate__(), 0.0f, 0.0f);
    for (long i = 0; i < num; i++) {
        CStgGmk_IceCube_Hockey *pCube = m_IceCubeCluster.GetIceCube(i);
        if (pCube != 0 && pCube->IsLanding()) {
            pCube->SetAccel(accel);
        }
    }
}

float msw::CStageHockeyCrushSkate85::GetBaseCourtWidth_() const
{
    return HOCKEY_PRM_S85_COURT_WIDTH;
}

float msw::CStageHockeyCrushSkate85::GetBaseCourtDepth_() const
{
    return HOCKEY_PRM_S85_COURT_DEPTH;
}

float msw::CStageHockeyCrushSkate85::GetBaseCourtRadius_() const
{
    return HOCKEY_PRM_S85_COURT_RADIUS;
}

long msw::CStageHockeyCrushSkate85::GetParam_ShipSe() const
{
    return SE_SHIP;
}

void msw::CStageHockeyCrushSkate85::OnSceneEntry()
{
    m_IceCubeCluster.CreateGmk();
}

void msw::CStageHockeyCrushSkate85::OnTurnInit()
{
    CStageHockeyCrushSkate::OnTurnInit();
    m_IceCubeCluster.Reset();
}

void msw::CStageHockeyCrushSkate85::OnGimmickReset()
{
    m_IceCubeCluster.InactivateAll();
}

void msw::CStageHockeyCrushSkate85::OnGimmickStop()
{
    CStageHockeyCrushSkate::OnGimmickStop();
    m_IceCubeCluster.InactivateAll();
}

void msw::CStageHockeyCrushSkate85::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
            SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
        }
        if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
            SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
        }
    }
}

long msw::CStageHockeyCrushSkate85::GetIceCubeMaxNum()
{
    CStageHockeyCrushSkate85 *pStage = static_cast<CStageHockeyCrushSkate85 *>(CStageMan::GetInstance());
    return pStage->m_IceCubeCluster.GetMaxNum();
}

const CStgGmk_IceCube_Hockey *msw::CStageHockeyCrushSkate85::GetIceCube(long idx)
{
    const CStageHockeyCrushSkate85 *pStage = static_cast<const CStageHockeyCrushSkate85 *>(CStageMan::GetInstance());
    if (idx < 0 || idx >= pStage->m_IceCubeCluster.GetMaxNum()) {
        return 0;
    }

    const CStgGmk_IceCube_Hockey *pCube = pStage->m_IceCubeCluster.GetIceCube(idx);
    if (pCube == 0 || !pCube->IsActive()) {
        return 0;
    }
    return pCube;
}

void msw::CStageHockeyCrushSkate85::Disp()
{
    CStageHockeyCrushSkate::Disp();
}

float msw::CStageHockeyCrushSkate85::GetParam_ShipCycle() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_CYCLE;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_CYCLE;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_CYCLE;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_CYCLE;
    }
}

float msw::CStageHockeyCrushSkate85::GetParam_ShipStopTime() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_STOP_TIME;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_STOP_TIME;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_STOP_TIME;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_STOP_TIME;
    }
}

float msw::CStageHockeyCrushSkate85::GetParam_ShipStopTimeRnd() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_STOP_TIME_RND;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_STOP_TIME_RND;
    }
}

float msw::CStageHockeyCrushSkate85::GetParam_ShipStopTime2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_STOP_TIME2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_STOP_TIME2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_STOP_TIME2;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_STOP_TIME2;
    }
}

float msw::CStageHockeyCrushSkate85::GetParam_ShipStopTimeRnd2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_STOP_TIME_RND2;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_STOP_TIME_RND2;
    }
}

float msw::CStageHockeyCrushSkate85::GetParam_ChrFlowVel() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S85_FLOOR0_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S85_FLOOR1_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S85_FLOOR2_CHR_FLOW_VEL;
    } else {
        return HOCKEY_PRM_S85_FLOOR3_CHR_FLOW_VEL;
    }
}

// The angle is given in degrees.
float msw::CStageHockeyCrushSkate85::GetParam_ShipRotWidth() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S85_FLOOR0_ROT_ANGLE;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S85_FLOOR1_ROT_ANGLE;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S85_FLOOR2_ROT_ANGLE;
    } else {
        value = HOCKEY_PRM_S85_FLOOR3_ROT_ANGLE;
    }
    return 0.017453292f * value;
}

// The wait is given in seconds.
float msw::CStageHockeyCrushSkate85::GetParam_ShipRotInterval() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S85_FLOOR0_INIT_WAIT;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S85_FLOOR1_INIT_WAIT;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S85_FLOOR2_INIT_WAIT;
    } else {
        value = HOCKEY_PRM_S85_FLOOR3_INIT_WAIT;
    }
    return 60.0f * value;
}

bool msw::CStageHockeyCrushSkate85::GetParam_IsShipRotEnable() const
{
    long enable;
    if (GetLocPatternNo() == 0) {
        enable = HOCKEY_PRM_S85_FLOOR0_ROT_ENABLE;
    } else if (GetLocPatternNo() == 1) {
        enable = HOCKEY_PRM_S85_FLOOR1_ROT_ENABLE;
    } else if (GetLocPatternNo() == 2) {
        enable = HOCKEY_PRM_S85_FLOOR2_ROT_ENABLE;
    } else {
        enable = HOCKEY_PRM_S85_FLOOR3_ROT_ENABLE;
    }
    return enable;
}

// The background turns once in BG_ROT_SPEED minutes (0: not at all).
float msw::CStageHockeyCrushSkate85::GetParam_BgRotSpeed() const
{
    float minutes = HOCKEY_PRM_S85_BG_ROT_SPEED;
    if (minutes > 0.0f) {
        return 6.2831855f / (60.0f * (60.0f * minutes));
    }
    return 0.0f;
}

unsigned long msw::CStageHockeyCrushSkate85::OnCalcSum() const
{
    return m_IceCubeCluster.CalcSum();
}
