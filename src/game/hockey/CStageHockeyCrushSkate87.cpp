// msw::CStageHockeyCrushSkate87: hockey party stage 87, a crush skate
// stage with a hole in the court. The rolling floor has four tunings; the
// party game's pattern (GetLocPatternNo) picks one.
#include "game/msw/CStageHockeyCrushSkate87.hpp"

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
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_COURT_WIDTH, 0x00 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_COURT_DEPTH, 0x04 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_COURT_RADIUS, 0x08 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_HOLE_WIDTH, 0x0C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_HOLE_DEPTH, 0x10 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_HOLE_RADIUS, 0x14 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_BG_ROT_SPEED, 0x18 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S87_FLOOR0_ROT_ENABLE, 0x1C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_INIT_WAIT, 0x20 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_CYCLE, 0x24 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_STOP_TIME, 0x28 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND, 0x2C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_STOP_TIME2, 0x30 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND2, 0x34 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_CHR_FLOW_VEL, 0x38 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR0_ROT_ANGLE, 0x3C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S87_FLOOR1_ROT_ENABLE, 0x40 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_INIT_WAIT, 0x44 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_CYCLE, 0x48 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_STOP_TIME, 0x4C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND, 0x50 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_STOP_TIME2, 0x54 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND2, 0x58 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_CHR_FLOW_VEL, 0x5C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR1_ROT_ANGLE, 0x60 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S87_FLOOR2_ROT_ENABLE, 0x64 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_INIT_WAIT, 0x68 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_CYCLE, 0x6C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_STOP_TIME, 0x70 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND, 0x74 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_STOP_TIME2, 0x78 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND2, 0x7C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_CHR_FLOW_VEL, 0x80 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR2_ROT_ANGLE, 0x84 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S87_FLOOR3_ROT_ENABLE, 0x88 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_INIT_WAIT, 0x8C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_CYCLE, 0x90 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_STOP_TIME, 0x94 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND, 0x98 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_STOP_TIME2, 0x9C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND2, 0xA0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_CHR_FLOW_VEL, 0xA4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S87_FLOOR3_ROT_ANGLE, 0xA8 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT1 = 1,
    SE_AMBIENT0 = 0xDBB,
    SE_AMBIENT1 = 0xDBD,
    SE_SHIP = 0xDBA
};

const void *msw::CStageHockeyCrushSkate87::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockeyCrushSkate87::CStageHockeyCrushSkate87(bcs::CTask *pParent) : CStageHockeyCrushSkate(pParent)
{
}

msw::CStageHockeyCrushSkate87::~CStageHockeyCrushSkate87()
{
}

float msw::CStageHockeyCrushSkate87::GetBaseCourtWidth_() const
{
    return HOCKEY_PRM_S87_COURT_WIDTH;
}

float msw::CStageHockeyCrushSkate87::GetBaseCourtDepth_() const
{
    return HOCKEY_PRM_S87_COURT_DEPTH;
}

float msw::CStageHockeyCrushSkate87::GetBaseCourtRadius_() const
{
    return HOCKEY_PRM_S87_COURT_RADIUS;
}

float msw::CStageHockeyCrushSkate87::GetBaseOuterWidth_() const
{
    return HOCKEY_PRM_S87_HOLE_WIDTH;
}

float msw::CStageHockeyCrushSkate87::GetBaseOuterDepth_() const
{
    return HOCKEY_PRM_S87_HOLE_DEPTH;
}

float msw::CStageHockeyCrushSkate87::GetBaseOuterRadius_() const
{
    return HOCKEY_PRM_S87_HOLE_RADIUS;
}

long msw::CStageHockeyCrushSkate87::GetParam_ShipSe() const
{
    return SE_SHIP;
}

void msw::CStageHockeyCrushSkate87::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
            SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
        }
        if (!SE_IsAlive(SE_SLOT_AMBIENT1)) {
            SE_RepeatPlay(SE_SLOT_AMBIENT1, SE_AMBIENT1, 180.0f, 420.0f, -1.0f, 0.0f, 12);
        }
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ShipCycle() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_CYCLE;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_CYCLE;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_CYCLE;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_CYCLE;
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ShipStopTime() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_STOP_TIME;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_STOP_TIME;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_STOP_TIME;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_STOP_TIME;
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ShipStopTimeRnd() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND;
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ShipStopTime2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_STOP_TIME2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_STOP_TIME2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_STOP_TIME2;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_STOP_TIME2;
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ShipStopTimeRnd2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND2;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND2;
    }
}

float msw::CStageHockeyCrushSkate87::GetParam_ChrFlowVel() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S87_FLOOR0_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S87_FLOOR1_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S87_FLOOR2_CHR_FLOW_VEL;
    } else {
        return HOCKEY_PRM_S87_FLOOR3_CHR_FLOW_VEL;
    }
}

// The angle is given in degrees.
float msw::CStageHockeyCrushSkate87::GetParam_ShipRotWidth() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S87_FLOOR0_ROT_ANGLE;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S87_FLOOR1_ROT_ANGLE;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S87_FLOOR2_ROT_ANGLE;
    } else {
        value = HOCKEY_PRM_S87_FLOOR3_ROT_ANGLE;
    }
    return 0.017453292f * value;
}

// The wait is given in seconds.
float msw::CStageHockeyCrushSkate87::GetParam_ShipRotInterval() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S87_FLOOR0_INIT_WAIT;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S87_FLOOR1_INIT_WAIT;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S87_FLOOR2_INIT_WAIT;
    } else {
        value = HOCKEY_PRM_S87_FLOOR3_INIT_WAIT;
    }
    return 60.0f * value;
}

bool msw::CStageHockeyCrushSkate87::GetParam_IsShipRotEnable() const
{
    long enable;
    if (GetLocPatternNo() == 0) {
        enable = HOCKEY_PRM_S87_FLOOR0_ROT_ENABLE;
    } else if (GetLocPatternNo() == 1) {
        enable = HOCKEY_PRM_S87_FLOOR1_ROT_ENABLE;
    } else if (GetLocPatternNo() == 2) {
        enable = HOCKEY_PRM_S87_FLOOR2_ROT_ENABLE;
    } else {
        enable = HOCKEY_PRM_S87_FLOOR3_ROT_ENABLE;
    }
    return enable;
}

// The background turns once in BG_ROT_SPEED minutes (0: not at all).
float msw::CStageHockeyCrushSkate87::GetParam_BgRotSpeed() const
{
    float minutes = HOCKEY_PRM_S87_BG_ROT_SPEED;
    if (minutes > 0.0f) {
        return 6.2831855f / (60.0f * (60.0f * minutes));
    }
    return 0.0f;
}
