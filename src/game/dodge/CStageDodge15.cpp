// msw::CStageDodge15: dodge stage 15, the ship stage. The court is the deck
// of a ship that rolls, cases slide over it, rain comes and goes and
// lightning flashes (CStage15Base, a DOL class, does the work; this class
// hands its hooks on and gives it the tuning and the case class). The
// dodgeball cases (msw::CStgGmk_Case_Dodge) are all inline, so their vtable
// and weak functions are emitted here too.
#include "game/msw/CStageDodge15.hpp"

#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CStgGmk_Case_Dodge.hpp"
#include "game/msw/DodgePrm.hpp"

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
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_WIDTH, 0x00 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEPTH, 0x04 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_HEIGHT, 0x08 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_INTERVAL, 0x0C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_OFFSET_Z, 0x10 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_POS_Y, 0x14 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_ACC_0, 0x18 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_0, 0x1C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_RND_0, 0x20 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_VEL_0, 0x24 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_LIFE_0, 0x28 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_ACC_1, 0x2C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_1, 0x30 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_RND_1, 0x34 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_VEL_1, 0x38 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_LIFE_1, 0x3C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_ACC_2, 0x40 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_2, 0x44 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_DEC_RND_2, 0x48 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_VEL_2, 0x4C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CASE_LIFE_2, 0x50 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_RAINY_INTERVAL, 0x54 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_RAINY_TIME, 0x58 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_SHIP_CYCLE, 0x5C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_SHIP_ROT_ANGLE, 0x60 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_SHIP_STOP_TIME, 0x64 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_SHIP_STOP_TIME_RND, 0x68 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_CHR_FLOW_VEL, 0x6C },
        { S15_PRM_TYPE_LONG, &DODGE_PRM_S15_FLASH_COLOR_R, 0x70 },
        { S15_PRM_TYPE_LONG, &DODGE_PRM_S15_FLASH_COLOR_G, 0x74 },
        { S15_PRM_TYPE_LONG, &DODGE_PRM_S15_FLASH_COLOR_B, 0x78 },
        { S15_PRM_TYPE_LONG, &DODGE_PRM_S15_FLASH_COLOR_A, 0x7C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_FLASH_TIME, 0x80 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_FLASH_GAIN, 0x84 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_FLASH_DECAY, 0x88 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_KARON_SCALE, 0x8C },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_KARON_COLL_SCALE, 0x90 },
        { S15_PRM_TYPE_FLOAT, &DODGE_PRM_S15_KARON_SPEED, 0x94 },
        { S15_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_SCROLL_NULL = 1, // "null_scr<n>": null 0 scrolls with the ship
    MDL_BARREL = 2,      // "s<n>_taru_": barrels
    MDL_BELL = 3         // "Bell_": rolls with the ship
};

// Messages SendMessage reacts to (names guessed).
enum {
    MSG_3 = 3,
    MSG_13 = 0x13,
    MSG_27 = 0x27,
    MSG_3C = 0x3C
};

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_MSG = 0,
    SE_SLOT_AMBIENT = 2,
    SE_SLOT_EVENT = 3,
    SE_MSG = 0xD7A,
    SE_SHIP = 0xD7B,
    SE_AMBIENT = 0xD87,
    SE_EVENT = 0xD88
};

// The most cases a stage can have.
static const long CASE_MAX_NUM = 8;

const void *msw::CStageDodge15::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge15::CStageDodge15(bcs::CTask *pParent) : CStageManDodge(pParent), CStage15Base()
{
}

msw::CStageDodge15::~CStageDodge15()
{
}

void msw::CStageDodge15::Main_(float frame)
{
    Stage15_Main_(frame);
    CStageManDodge::Main_(frame);
}

void msw::CStageDodge15::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManDodge::SendMessage(msg, arg0, arg1, arg2);

    switch (arg0) {
        case MSG_3:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_MSG)) {
                SE_Play(SE_SLOT_MSG, SE_MSG, 0.0f, 3);
            }
            break;
        case MSG_3C:
            Case_InactivateAll();
            break;
        case MSG_27:
            if (arg1 == 10) {
                Case_SetNoLocateTeam(arg2);
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge15::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_scr([0-9]+)", MDL_SCROLL_NULL, 0, 0, NewMdl },
        { "^s[0-9]+_taru_", MDL_BARREL, 0, 0, NewMdl },
        { "^Bell_", MDL_BELL, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge15::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_SCROLL_NULL) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no == 0) {
            SetScrollNull(*rHandle.Get());
        }
    }
}

void msw::CStageDodge15::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
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
}

void msw::CStageDodge15::OnSceneEntry()
{
    Stage15_OnSceneEntry();
    Case_Create();
}

void msw::CStageDodge15::OnSceneReentry()
{
    Stage15_OnSceneReentry();
}

void msw::CStageDodge15::OnGimmickStart()
{
    Case_RespawnAll();
}

void msw::CStageDodge15::OnGimmickStop()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnPeriodInit()
{
    Stage15_OnPeriodInit();
}

void msw::CStageDodge15::OnTurnInit()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnTurnEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnGameEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnEntranceEventStart()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnEntranceEventGmkStart()
{
    Case_RespawnAll();
}

void msw::CStageDodge15::OnEntranceEventGmkEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodge15::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
                SE_Play(SE_SLOT_AMBIENT, SE_AMBIENT, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

msw::CStgGmk_Case *msw::CStageDodge15::NewCase()
{
    return new (1) CStgGmk_Case_Dodge(this);
}

long msw::CStageDodge15::GetCaseMaxNum()
{
    return CASE_MAX_NUM;
}

const msw::CStgGmk_Case *msw::CStageDodge15::GetCase(long idx)
{
    return static_cast<const CStageDodge15 *>(CStageMan::GetInstance())->Case_Get(idx);
}

const msw::CStageMan *msw::CStageDodge15::GetStageMan() const
{
    return this;
}

float msw::CStageDodge15::GetParam_CaseWidth() const
{
    return DODGE_PRM_S15_CASE_WIDTH;
}

float msw::CStageDodge15::GetParam_CaseOffsetZ() const
{
    return DODGE_PRM_S15_CASE_OFFSET_Z;
}

float msw::CStageDodge15::GetParam_CaseSpacingZ() const
{
    return 5.0f;
}

// The ground, less a case width at the left and right.
void msw::CStageDodge15::GetParam_CaseLocateRect(float *pLeft, float *pRight, float *pFront, float *pBack)
{
    GetStageMan()->GetGroundRect(pLeft, pRight, pFront, pBack);
    if (pLeft != 0) {
        *pLeft -= GetParam_CaseWidth();
    }
    if (pRight != 0) {
        *pRight += GetParam_CaseWidth();
    }
}

// The alpha is halved.
bcs::BCS_Color msw::CStageDodge15::GetParam_FlashColor() const
{
    return bcs::BCS_Color(DODGE_PRM_S15_FLASH_COLOR_R, DODGE_PRM_S15_FLASH_COLOR_G, DODGE_PRM_S15_FLASH_COLOR_B,
                          DODGE_PRM_S15_FLASH_COLOR_A / 2);
}

// The flash time is given in frames.
float msw::CStageDodge15::GetParam_FlashFrm() const
{
    return DODGE_PRM_S15_FLASH_TIME;
}

float msw::CStageDodge15::GetParam_FlashGain() const
{
    return DODGE_PRM_S15_FLASH_GAIN;
}

float msw::CStageDodge15::GetParam_FlashDecay() const
{
    return DODGE_PRM_S15_FLASH_DECAY;
}

// The rain times are given in seconds.
float msw::CStageDodge15::GetParam_RainyInterval() const
{
    return 60.0f * DODGE_PRM_S15_RAINY_INTERVAL;
}

float msw::CStageDodge15::GetParam_RainyTime() const
{
    return 60.0f * DODGE_PRM_S15_RAINY_TIME;
}

float msw::CStageDodge15::GetParam_ShipCycle() const
{
    return DODGE_PRM_S15_SHIP_CYCLE;
}

float msw::CStageDodge15::GetParam_ShipStopTime() const
{
    return DODGE_PRM_S15_SHIP_STOP_TIME;
}

float msw::CStageDodge15::GetParam_ShipStopTimeRnd() const
{
    return DODGE_PRM_S15_SHIP_STOP_TIME_RND;
}

float msw::CStageDodge15::GetParam_ChrFlowVel() const
{
    return DODGE_PRM_S15_CHR_FLOW_VEL;
}

// The angle is given in degrees.
float msw::CStageDodge15::GetParam_ShipRotWidth() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S15_SHIP_ROT_ANGLE);
}

long msw::CStageDodge15::GetParam_ShipSe() const
{
    return SE_SHIP;
}

void msw::CStageDodge15::OnEventEnd()
{
    Stage15_OnEventEnd();
}

void msw::CStageDodge15::OnEventStart()
{
    Stage15_OnEventStart();
}

void msw::CStageDodge15::OnGimmickCancel()
{
    Stage15_OnGimmickCancel();
}

void msw::CStageDodge15::OnGimmickReset()
{
    Stage15_OnGimmickReset();
}

void msw::CStageDodge15::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    GetFloorFlow__(rFlow, rPos, type);
}

long msw::CStageDodge15::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return GetFloorAttribute__(rPos);
}

float msw::CStageDodge15::GetWorldRotZ() const
{
    return GetWorldRotZ__();
}
