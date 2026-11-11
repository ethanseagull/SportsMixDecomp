// msw::CStageVolley15: volley stage 15, the ship stage. The court is the deck
// of a ship that rolls, cases slide over it, rain comes and goes and
// lightning flashes (CStage15Base, a DOL class, does the work; this class
// hands its hooks on and gives it the tuning and the case class). The
// basketball cases (msw::CStgGmk_Case_Volley) are all inline, so their vtable
// and weak functions are emitted here too.
#include "game/msw/CStageVolley15.hpp"

#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CStgGmk_Case_Volley.hpp"
#include "game/msw/VolleyPrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S15_PRM_TYPE {
        S15_PRM_TYPE_END,
        S15_PRM_TYPE_LONG,
        S15_PRM_TYPE_FLOAT
    };

    struct S15_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S15_PRM_TBL s_PrmTbl[] = {
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_WIDTH, 0x00 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEPTH, 0x04 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_HEIGHT, 0x08 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_INTERVAL, 0x0C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_OFFSET_Z, 0x10 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_SPACING_Z, 0x14 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_POS_Y, 0x18 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_ACC_0, 0x1C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_0, 0x20 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_RND_0, 0x24 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_VEL_0, 0x28 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_ACC_1, 0x2C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_1, 0x30 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_RND_1, 0x34 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_VEL_1, 0x38 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_ACC_2, 0x3C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_2, 0x40 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_DEC_RND_2, 0x44 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CASE_VEL_2, 0x48 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_RAINY_INTERVAL, 0x4C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_RAINY_TIME, 0x50 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_SHIP_CYCLE, 0x54 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_SHIP_ROT_ANGLE, 0x58 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_SHIP_STOP_TIME, 0x5C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_SHIP_STOP_TIME_RND, 0x60 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_CHR_FLOW_VEL, 0x64 },
        { S15_PRM_TYPE_LONG, &VOLLEY_PRM_S15_FLASH_COLOR_R, 0x68 },
        { S15_PRM_TYPE_LONG, &VOLLEY_PRM_S15_FLASH_COLOR_G, 0x6C },
        { S15_PRM_TYPE_LONG, &VOLLEY_PRM_S15_FLASH_COLOR_B, 0x70 },
        { S15_PRM_TYPE_LONG, &VOLLEY_PRM_S15_FLASH_COLOR_A, 0x74 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_FLASH_TIME, 0x78 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_FLASH_GAIN, 0x7C },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_FLASH_DECAY, 0x80 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_KARON_SCALE, 0x84 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_KARON_COLL_SCALE, 0x88 },
        { S15_PRM_TYPE_FLOAT, &VOLLEY_PRM_S15_KARON_SPEED, 0x8C },
        { S15_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_SCROLL_NULL = 1, // "null_scr<n>": null 0 scrolls with the ship
    MDL_BARREL = 2,      // "s<n>_taru_": barrels
    MDL_BELL = 3         // "Bell_": rolls with the ship
};

// The messages SendMessage reacts to (names guessed). 0x03 and 0x13 start
// the first looping sound, unless it is already playing.
enum {
    MSG_03 = 0x03,
    MSG_13 = 0x13,
    MSG_27 = 0x27
};

// Sound slots and sounds of the stage (names guessed). Sound event 0
// starts the second looping sound, unless it is already playing.
enum {
    SE_SLOT_LOOP0 = 0,
    SE_SLOT_LOOP1 = 2,
    SE_SLOT_EVENT = 3,
    SE_LOOP0 = 0xD7A,
    SE_SHIP = 0xD7B,
    SE_LOOP1 = 0xD87,
    SE_EVENT = 0xD88
};

// The most cases a stage can have.
static const long CASE_MAX_NUM = 8;

const void *msw::CStageVolley15::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley15::CStageVolley15(bcs::CTask *pParent) : CStageManVolley(pParent), CStage15Base()
{
}

msw::CStageVolley15::~CStageVolley15()
{
}

void msw::CStageVolley15::Main_(float frame)
{
    Stage15_Main_(frame);
    CStageManVolley::Main_(frame);
}

void msw::CStageVolley15::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManVolley::SendMessage(msg, arg0, arg1, arg2);

    switch (msg) {
        case MSG_03:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_LOOP0)) {
                SE_Play(SE_SLOT_LOOP0, SE_LOOP0, 0.0f, 3);
            }
            break;
        case MSG_27:
            if (arg0 == 10) {
                Case_SetNoLocateTeam(arg1);
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley15::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_scr([0-9]+)", MDL_SCROLL_NULL, 0, 0, NewMdl },
        { "^s[0-9]+_taru_", MDL_BARREL, 0, 0, NewMdl },
        { "^Bell_", MDL_BELL, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageVolley15::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_SCROLL_NULL) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no == 0) {
            SetScrollNull(*rHandle.Get());
        }
    }
}

void msw::CStageVolley15::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_BARREL:
            AddBarrelMdlNode(pNode);
            break;
        case MDL_BELL:
            AddRollMdlNode(pNode);
            break;
    }

    CStageManVolley::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageVolley15::OnSceneEntry()
{
    Stage15_OnSceneEntry();
    Case_Create();
}

void msw::CStageVolley15::OnSceneReentry()
{
    Stage15_OnSceneReentry();
}

void msw::CStageVolley15::OnGimmickStart()
{
}

void msw::CStageVolley15::OnGimmickStop()
{
    Case_InactivateAll();
}

void msw::CStageVolley15::OnPeriodInit()
{
    Stage15_OnPeriodInit();
}

void msw::CStageVolley15::OnTurnInit()
{
    Case_RespawnAll();
}

void msw::CStageVolley15::OnTurnEnd()
{
    Case_InactivateAll();
}

void msw::CStageVolley15::OnEntranceEventStart()
{
    Case_InactivateAll();
}

void msw::CStageVolley15::OnEntranceEventGmkStart()
{
    Case_RespawnAll();
}

void msw::CStageVolley15::OnEntranceEventGmkEnd()
{
    Case_InactivateAll();
}

void msw::CStageVolley15::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_LOOP1)) {
                SE_Play(SE_SLOT_LOOP1, SE_LOOP1, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

msw::CStgGmk_Case *msw::CStageVolley15::NewCase()
{
    return new (1) CStgGmk_Case_Volley(this);
}

long msw::CStageVolley15::GetCaseMaxNum()
{
    return CASE_MAX_NUM;
}

const msw::CStgGmk_Case *msw::CStageVolley15::GetCase(long idx)
{
    return static_cast<const CStageVolley15 *>(CStageMan::GetInstance())->Case_Get(idx);
}

const msw::CStageMan *msw::CStageVolley15::GetStageMan() const
{
    return this;
}

long msw::CStageVolley15::GetParam_CaseNum() const
{
    return 4;
}

float msw::CStageVolley15::GetParam_CaseWidth() const
{
    return VOLLEY_PRM_S15_CASE_WIDTH;
}

float msw::CStageVolley15::GetParam_CaseOffsetZ() const
{
    return VOLLEY_PRM_S15_CASE_OFFSET_Z;
}

float msw::CStageVolley15::GetParam_CaseSpacingZ() const
{
    return VOLLEY_PRM_S15_CASE_SPACING_Z;
}

// The alpha is halved.
bcs::BCS_Color msw::CStageVolley15::GetParam_FlashColor() const
{
    return bcs::BCS_Color(VOLLEY_PRM_S15_FLASH_COLOR_R, VOLLEY_PRM_S15_FLASH_COLOR_G, VOLLEY_PRM_S15_FLASH_COLOR_B,
                          VOLLEY_PRM_S15_FLASH_COLOR_A / 2);
}

// The flash time is given in frames.
float msw::CStageVolley15::GetParam_FlashFrm() const
{
    return VOLLEY_PRM_S15_FLASH_TIME;
}

float msw::CStageVolley15::GetParam_FlashGain() const
{
    return VOLLEY_PRM_S15_FLASH_GAIN;
}

float msw::CStageVolley15::GetParam_FlashDecay() const
{
    return VOLLEY_PRM_S15_FLASH_DECAY;
}

// The rain times are given in seconds.
float msw::CStageVolley15::GetParam_RainyInterval() const
{
    return 60.0f * VOLLEY_PRM_S15_RAINY_INTERVAL;
}

float msw::CStageVolley15::GetParam_RainyTime() const
{
    return 60.0f * VOLLEY_PRM_S15_RAINY_TIME;
}

float msw::CStageVolley15::GetParam_ShipCycle() const
{
    return VOLLEY_PRM_S15_SHIP_CYCLE;
}

float msw::CStageVolley15::GetParam_ShipStopTime() const
{
    return VOLLEY_PRM_S15_SHIP_STOP_TIME;
}

float msw::CStageVolley15::GetParam_ShipStopTimeRnd() const
{
    return VOLLEY_PRM_S15_SHIP_STOP_TIME_RND;
}

float msw::CStageVolley15::GetParam_ChrFlowVel() const
{
    return VOLLEY_PRM_S15_CHR_FLOW_VEL;
}

// The angle is given in degrees.
float msw::CStageVolley15::GetParam_ShipRotWidth() const
{
    return bcs::DEG_TO_RAD(VOLLEY_PRM_S15_SHIP_ROT_ANGLE);
}

long msw::CStageVolley15::GetParam_ShipSe() const
{
    return SE_SHIP;
}

void msw::CStageVolley15::OnEventEnd()
{
    Stage15_OnEventEnd();
}

void msw::CStageVolley15::OnEventStart()
{
    Stage15_OnEventStart();
}

void msw::CStageVolley15::OnGimmickCancel()
{
    Stage15_OnGimmickCancel();
}

void msw::CStageVolley15::OnGimmickReset()
{
    Stage15_OnGimmickReset();
}

void msw::CStageVolley15::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    GetFloorFlow__(rFlow, rPos, type);
}

long msw::CStageVolley15::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return GetFloorAttribute__(rPos);
}

float msw::CStageVolley15::GetWorldRotZ() const
{
    return GetWorldRotZ__();
}
