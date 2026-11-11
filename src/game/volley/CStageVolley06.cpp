// msw::CStageVolley06: volley stage 06. The lights over a goal come on as
// the team attacking it scores (CStage06Base, a DOL class, does the work;
// this class hands its hooks on and gives it the tuning).
#include "game/msw/CStageVolley06.hpp"

#include "game/msw/VolleyPrm.hpp"
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
        { S06_PRM_TYPE_LONG, &VOLLEY_PRM_S06_LIGHT_SWITCH_CNT, 0x00 },
        { S06_PRM_TYPE_FLOAT, &VOLLEY_PRM_S06_LIGHT_ON_TIME, 0x04 },
        { S06_PRM_TYPE_LONG, &VOLLEY_PRM_S06_FLASH_COLOR_R, 0x08 },
        { S06_PRM_TYPE_LONG, &VOLLEY_PRM_S06_FLASH_COLOR_G, 0x0C },
        { S06_PRM_TYPE_LONG, &VOLLEY_PRM_S06_FLASH_COLOR_B, 0x10 },
        { S06_PRM_TYPE_LONG, &VOLLEY_PRM_S06_FLASH_COLOR_A, 0x14 },
        { S06_PRM_TYPE_FLOAT, &VOLLEY_PRM_S06_FLASH_TIME, 0x18 },
        { S06_PRM_TYPE_FLOAT, &VOLLEY_PRM_S06_FLASH_GAIN, 0x1C },
        { S06_PRM_TYPE_FLOAT, &VOLLEY_PRM_S06_FLASH_DECAY, 0x20 },
        { S06_PRM_TYPE_END, 0, 0 },
    };
}

const void *msw::CStageVolley06::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley06::CStageVolley06(bcs::CTask *pParent) : CStageManVolley(pParent), CStage06Base(this)
{
}

msw::CStageVolley06::~CStageVolley06()
{
}

void msw::CStageVolley06::Main_(float frame)
{
    Stage06_Main_(frame);
    CStageManVolley::Main_(frame);
}

void msw::CStageVolley06::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();
    SceneCameraLoad(2, -1);
    SceneCameraLoad(3, -1);
    Stage06_OnPostInitialize();
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley06::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManVolley.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_light.._.*", 0xB, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageVolley06::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    CStageManVolley::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
    Stage06_OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageVolley06::OnPeriodInit()
{
    Stage06_OnPeriodInit();
}

void msw::CStageVolley06::OnGimmickReset()
{
    Stage06_OnGimmickReset();
}

void msw::CStageVolley06::OnEventStart()
{
    Stage06_OnEventStart();
}

void msw::CStageVolley06::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManVolley::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case 0x54: {
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

void msw::CStageVolley06::OnSoundEvent(long event)
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
        case 4:
        case 5:
            SE_Play(3, 0xD39, 0.0f, 0);
            break;
    }
}

bcs::BCS_Color msw::CStageVolley06::GetParam_IllumiOnColor() const
{
    return bcs::BCS_Color(GetLightMapOnColor());
}

bcs::BCS_Color msw::CStageVolley06::GetParam_IllumiOffColor() const
{
    return bcs::BCS_Color(GetLightMapOffColor());
}

long msw::CStageVolley06::GetParam_IllumiOnGoalCnt() const
{
    return VOLLEY_PRM_S06_LIGHT_SWITCH_CNT;
}

// The parameter is in seconds.
float msw::CStageVolley06::GetParam_IllumiOnTime() const
{
    return 60.0f * VOLLEY_PRM_S06_LIGHT_ON_TIME;
}

bcs::BCS_Color msw::CStageVolley06::GetParam_FlashColor() const
{
    return bcs::BCS_Color(VOLLEY_PRM_S06_FLASH_COLOR_R, VOLLEY_PRM_S06_FLASH_COLOR_G, VOLLEY_PRM_S06_FLASH_COLOR_B,
                          VOLLEY_PRM_S06_FLASH_COLOR_A / 2);
}

float msw::CStageVolley06::GetParam_FlashTime() const
{
    return VOLLEY_PRM_S06_FLASH_TIME;
}

float msw::CStageVolley06::GetParam_FlashGain() const
{
    return VOLLEY_PRM_S06_FLASH_GAIN;
}

float msw::CStageVolley06::GetParam_FlashDecay() const
{
    return VOLLEY_PRM_S06_FLASH_DECAY;
}
