// msw::CStageBasket06: basket stage 06. The lights over a goal come on as
// the team attacking it scores (CStage06Base, a DOL class, does the work;
// this class hands its hooks on and gives it the tuning).
#include "game/msw/CStageBasket06.hpp"

#include "game/msw/BasketPrm.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, its address and an offset (four
    // bytes per entry, with two unused slots). The table ends with a zero
    // entry.
    enum S06_PRM_TYPE {
        S06_PRM_TYPE_END,
        S06_PRM_TYPE_LONG,
        S06_PRM_TYPE_FLOAT
    };

    struct S06_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S06_PRM_TBL s_PrmTbl[] = {
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_SPEED, 0x00 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_SPEED_SHOOT_CATCH, 0x04 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_ROTATE_RATE, 0x08 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_HEIGHT, 0x0C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_Y_SP_MIN, 0x10 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_MOVE_Y_SP_MAX, 0x14 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_BALL_HOLD_MOVE, 0x18 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_CATCH1_COL_R, 0x1C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_CATCH1_COL_HEIGHT, 0x20 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_CATCH2_COL_RADIUS, 0x24 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_PUSH1_COL_SIZE_R, 0x28 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_PUSH1_COL_SIZE_HEIGHT, 0x2C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_DAMAGE_COLL_BALL_R, 0x30 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_DAMAGE_COLL_TYPE, 0x34 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_BW_ALPHA_RATE, 0x38 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_ALPHA_TEST_NEAR_RANG, 0x3C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_ALPHA_TEST_FAR_RANG, 0x40 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_PASS_CALL_WAIT_MINUTES, 0x44 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_PASS_RANDUM_PARCENT, 0x48 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_PASS_MISS_TO_HAPPY_COUNT, 0x4C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_LOOSE_BALL_GET_DIST, 0x54 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_CUT_APPEND_MINUTES, 0x58 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_RIGHT_OFF_MOVE_R1, 0x60 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_RIGHT_OFF_MOVE_R2, 0x64 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_RIGHT_OFF_MOVE_R1_HEIGHT, 0x68 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_LIGHT_SWITCH_CNT, 0x6C },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_LIGHT_ON_TIME, 0x70 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_FLASH_COLOR_R, 0x74 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_FLASH_COLOR_G, 0x78 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_FLASH_COLOR_B, 0x7C },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_FLASH_COLOR_A, 0x80 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_FLASH_TIME, 0x84 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_FLASH_GAIN, 0x88 },
        { S06_PRM_TYPE_FLOAT, &BASKET_PRM_S06_FLASH_DECAY, 0x8C },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_DBG_MAIN_PROC_ENABLE, 0x90 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_DBG_TARGET_DISP, 0x94 },
        { S06_PRM_TYPE_LONG, &BASKET_PRM_S06_PUSH1_COL_DBG_ENABLE, 0x98 },
        { S06_PRM_TYPE_END, 0, 0 },
    };
}

const void *msw::CStageBasket06::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket06::CStageBasket06(bcs::CTask *pParent) : CStageManBasket(pParent), CStage06Base(this)
{
}

msw::CStageBasket06::~CStageBasket06()
{
}

void msw::CStageBasket06::Main_(float frame)
{
    Stage06_Main_(frame);
    CStageManBasket::Main_(frame);
}

void msw::CStageBasket06::OnPostInitialize()
{
    CStageManBasket::OnPostInitialize();
    SceneCameraLoad(2, -1);
    SceneCameraLoad(3, -1);
    Stage06_OnPostInitialize();
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket06::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_light.._.*", 0xB, 0, 0, 0 },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasket06::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
    Stage06_OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasket06::OnPeriodInit()
{
    Stage06_OnPeriodInit();
}

void msw::CStageBasket06::OnGimmickReset()
{
    Stage06_OnGimmickReset();
}

void msw::CStageBasket06::OnEventStart()
{
    Stage06_OnEventStart();
}

void msw::CStageBasket06::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManBasket::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case 0x36: {
            // A goal by character arg1: the lights over the goal it
            // attacked go up.
            CChrCore *pChr = arg1 >= 0 ? CChrMan::GetChrCore(arg1) : 0;
            long goal = pChr != 0 ? pChr->GetTargetGoalIdx() : -1;
            if (goal >= 0) {
                Illumi_Inc(goal, GetParam_IllumiOnTime());
            }
            break;
        }
        case 0x27:
            if (arg1 == 4) {
                Illumi_StateFix(0);
            }
            break;
    }
}

void msw::CStageBasket06::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(1)) {
                SE_RepeatPlay(1, 0xD36, 600.0f, 600.0f, -1.0f, 0.0f, 0xC);
            }
            if (!SE_IsAlive(2)) {
                SE_Play(2, 0xD38, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
            SE_Play(3, 0xD39, 0.0f, 0);
            break;
    }
}

bcs::BCS_Color msw::CStageBasket06::GetParam_IllumiOnColor() const
{
    return bcs::BCS_Color(GetLightMapOnColor());
}

bcs::BCS_Color msw::CStageBasket06::GetParam_IllumiOffColor() const
{
    return bcs::BCS_Color(GetLightMapOffColor());
}

long msw::CStageBasket06::GetParam_IllumiOnGoalCnt() const
{
    return BASKET_PRM_S06_LIGHT_SWITCH_CNT;
}

// The parameter is in seconds.
float msw::CStageBasket06::GetParam_IllumiOnTime() const
{
    return 60.0f * BASKET_PRM_S06_LIGHT_ON_TIME;
}

bcs::BCS_Color msw::CStageBasket06::GetParam_FlashColor() const
{
    return bcs::BCS_Color(BASKET_PRM_S06_FLASH_COLOR_R, BASKET_PRM_S06_FLASH_COLOR_G, BASKET_PRM_S06_FLASH_COLOR_B,
                          BASKET_PRM_S06_FLASH_COLOR_A / 2);
}

float msw::CStageBasket06::GetParam_FlashTime() const
{
    return BASKET_PRM_S06_FLASH_TIME;
}

float msw::CStageBasket06::GetParam_FlashGain() const
{
    return BASKET_PRM_S06_FLASH_GAIN;
}

float msw::CStageBasket06::GetParam_FlashDecay() const
{
    return BASKET_PRM_S06_FLASH_DECAY;
}
