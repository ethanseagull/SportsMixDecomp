// Written from src/game/basket/CStageBasket16.cpp by tools/decomp/port_unit.py (Basket -> Hockey); check against HockeyF.
// Written from src/game/dodge/CStageDodge16.cpp by tools/decomp/port_unit.py (Dodge -> Basket); check against HockeyF.
// msw::CStageHockey16: hockey stage 16, the star piece stage. Star pieces
// fall on the court and the sky warps from time to time (CStage16Base, a
// DOL class, does the work; this class hands its hooks on and gives it the
// tuning).
#include "game/msw/CStageHockey16.hpp"

#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S16_PRM_TYPE {
        S16_PRM_TYPE_END,
        S16_PRM_TYPE_LONG,
        S16_PRM_TYPE_FLOAT
    };

    struct S16_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S16_PRM_TBL s_PrmTbl[] = {
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_DROP_AREA_Z, 0x00 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_MAX_NUM, 0x04 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_POP_TIME, 0x08 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_POP_TIME_RND, 0x0C },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_POP_POS_Y, 0x10 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_DROP_SPEED, 0x14 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_DROP_ANGLE, 0x18 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_BOUND_NUM, 0x1C },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_BOUND_REDUCE_Y, 0x20 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_BOUND_REDUCE_XZ, 0x24 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_ADD_COIN_0, 0x28 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_WARP_INTERVAL, 0x2C },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_WARP_TIME, 0x30 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_WARP_SPEED, 0x34 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_WARP_BLUR_SHIFT, 0x38 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_FLASH_COLOR_R, 0x3C },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_FLASH_COLOR_G, 0x40 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_FLASH_COLOR_B, 0x44 },
        { S16_PRM_TYPE_LONG, &HOCKEY_PRM_S16_FLASH_COLOR_A, 0x48 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_FLASH_TIME, 0x4C },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_FLASH_GAIN, 0x50 },
        { S16_PRM_TYPE_FLOAT, &HOCKEY_PRM_S16_FLASH_DECAY, 0x54 },
        { S16_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_SKY = 0x32 // "s<n>_enkei_sky_<k>": one of the skies a warp switches between
};

// Sound slot and sound of the stage.
enum {
    SE_SLOT_AMBIENT = 0,
    SE_AMBIENT = 0xD89
};

const void *msw::CStageHockey16::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey16::CStageHockey16(bcs::CTask *pParent) : CStageManHockey(pParent), CStage16Base()
{
}

msw::CStageHockey16::~CStageHockey16()
{
}

void msw::CStageHockey16::Main_(float frame)
{
    CStageManHockey::Main_(frame);
    Stage16_Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey16::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_enkei_sky_([0-9]+)$", MDL_SKY, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageHockey16::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_SKY) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0) {
            Stage16_AddSwitchableMdl(pNode, no);
        }
    }

    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageHockey16::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
            SE_Play(SE_SLOT_AMBIENT, SE_AMBIENT, 0.0f, 3);
        }
    }
}

bool msw::CStageHockey16::Stage16_IsGimmickReady() const
{
    return IsGimmickRunning();
}

float msw::CStageHockey16::GetParam_StarPieceAreaZ() const
{
    return HOCKEY_PRM_S16_DROP_AREA_Z;
}

// The times are given in seconds.
float msw::CStageHockey16::GetParam_StarPiecePopTime() const
{
    return 60.0f * HOCKEY_PRM_S16_POP_TIME;
}

float msw::CStageHockey16::GetParam_StarPiecePopTimeRnd() const
{
    return 60.0f * HOCKEY_PRM_S16_POP_TIME_RND;
}

float msw::CStageHockey16::GetParam_StarPiecePopPosY() const
{
    return HOCKEY_PRM_S16_POP_POS_Y;
}

// The speed is given per second.
float msw::CStageHockey16::GetParam_StarPieceSpeed() const
{
    return HOCKEY_PRM_S16_DROP_SPEED / 60.0f;
}

// The angle is given in degrees.
float msw::CStageHockey16::GetParam_StarPieceDropAngle() const
{
    return bcs::DEG_TO_RAD(HOCKEY_PRM_S16_DROP_ANGLE);
}

long msw::CStageHockey16::GetParam_StarPieceRate(long kind) const
{
    return 1;
}

long msw::CStageHockey16::GetParam_StarPieceAddCoin(long kind) const
{
    return HOCKEY_PRM_S16_ADD_COIN_0;
}

long msw::CStageHockey16::GetParam_StarPieceMaxNum() const
{
    return HOCKEY_PRM_S16_MAX_NUM;
}

long msw::CStageHockey16::GetParam_StarPieceBoundNum() const
{
    return HOCKEY_PRM_S16_BOUND_NUM;
}

float msw::CStageHockey16::GetParam_StarPieceBoundReduceY() const
{
    return HOCKEY_PRM_S16_BOUND_REDUCE_Y;
}

float msw::CStageHockey16::GetParam_StarPieceBoundReduceXZ() const
{
    return HOCKEY_PRM_S16_BOUND_REDUCE_XZ;
}

// The alpha is halved.
bcs::BCS_Color msw::CStageHockey16::GetParam_FlashColor() const
{
    return bcs::BCS_Color(HOCKEY_PRM_S16_FLASH_COLOR_R, HOCKEY_PRM_S16_FLASH_COLOR_G, HOCKEY_PRM_S16_FLASH_COLOR_B,
                          HOCKEY_PRM_S16_FLASH_COLOR_A / 2);
}

float msw::CStageHockey16::GetParam_FlashGain() const
{
    return HOCKEY_PRM_S16_FLASH_GAIN;
}

float msw::CStageHockey16::GetParam_FlashDecay() const
{
    return HOCKEY_PRM_S16_FLASH_DECAY;
}

float msw::CStageHockey16::GetParam_WarpInterval() const
{
    return 60.0f * HOCKEY_PRM_S16_WARP_INTERVAL;
}

float msw::CStageHockey16::GetParam_WarpTime() const
{
    return 60.0f * HOCKEY_PRM_S16_WARP_TIME;
}

float msw::CStageHockey16::GetParam_WarpBlurShift() const
{
    return HOCKEY_PRM_S16_WARP_BLUR_SHIFT;
}

float msw::CStageHockey16::GetParam_WarpGameSpeed() const
{
    return HOCKEY_PRM_S16_WARP_SPEED;
}

// The flash time is given in frames.
float msw::CStageHockey16::GetParam_FlashFrm() const
{
    return HOCKEY_PRM_S16_FLASH_TIME;
}

void msw::CStageHockey16::OnTurnInit()
{
    Stage16_OnTurnInit();
}

void msw::CStageHockey16::OnPeriodInit()
{
    Stage16_OnPeriodInit();
}

void msw::CStageHockey16::OnGimmickStop()
{
    Stage16_OnGimmickStop();
}

void msw::CStageHockey16::OnGimmickCancel()
{
    Stage16_OnGimmickCancel();
}

void msw::CStageHockey16::OnGimmickReset()
{
    Stage16_OnGimmickReset();
}

void msw::CStageHockey16::OnEntranceEventStart()
{
    Stage16_OnEntranceEventStart();
}

void msw::CStageHockey16::OnGameInit()
{
    Stage16_OnGameInit();
}

void msw::CStageHockey16::OnPostInitialize()
{
    CStageManHockey::OnPostInitialize();
    Stage16_OnPostInitialize();
}

void msw::CStageHockey16::OnSceneEntry()
{
    Stage16_OnSceneEntry();
}
