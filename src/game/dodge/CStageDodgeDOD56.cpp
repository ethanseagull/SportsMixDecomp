// msw::CStageDodgeDOD56: Dodge or Die stage 56, the ship stage of the Dodge
// or Die party game. The deck rolls as in stage 15 (CStage15Base, a DOL
// class, does the work; this class hands its hooks on and gives it the
// tuning), but there are no cases and it rains all the time.
#include "game/msw/CStageDodgeDOD56.hpp"

#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/DodgePrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S56_PRM_TYPE {
        S56_PRM_TYPE_END,
        S56_PRM_TYPE_LONG,
        S56_PRM_TYPE_FLOAT
    };

    struct S56_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S56_PRM_TBL s_PrmTbl[] = {
        { S56_PRM_TYPE_FLOAT, &DODGE_PRM_S56_SHIP_CYCLE, 0x00 },
        { S56_PRM_TYPE_FLOAT, &DODGE_PRM_S56_SHIP_ROT_ANGLE, 0x04 },
        { S56_PRM_TYPE_FLOAT, &DODGE_PRM_S56_SHIP_STOP_TIME, 0x08 },
        { S56_PRM_TYPE_FLOAT, &DODGE_PRM_S56_SHIP_STOP_TIME_RND, 0x0C },
        { S56_PRM_TYPE_FLOAT, &DODGE_PRM_S56_CHR_FLOW_VEL, 0x10 },
        { S56_PRM_TYPE_END, 0, 0 },
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
    MSG_3C = 0x3C
};

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_MSG = 0,
    SE_SLOT_AMBIENT = 2,
    SE_MSG = 0xD7A,
    SE_AMBIENT_0 = 0xD9D,
    SE_SHIP = 0xD9E,
    SE_AMBIENT_2 = 0xD9F
};

const void *msw::CStageDodgeDOD56::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodgeDOD56::CStageDodgeDOD56(bcs::CTask *pParent) : CStageManDodgeDodgeOrDie(pParent), CStage15Base()
{
}

msw::CStageDodgeDOD56::~CStageDodgeDOD56()
{
}

void msw::CStageDodgeDOD56::Main_(float frame)
{
    Stage15_Main_(frame);
    CStageManDodgeDodgeOrDie::Main_(frame);
}

void msw::CStageDodgeDOD56::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManDodgeDodgeOrDie::SendMessage(msg, arg0, arg1, arg2);

    switch (msg) {
        case MSG_3:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_MSG)) {
                SE_Play(SE_SLOT_MSG, SE_MSG, 0.0f, 3);
            }
            break;
        case MSG_3C:
            Case_InactivateAll();
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodgeDOD56::GetMdlGenRule_() const
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

void msw::CStageDodgeDOD56::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                             CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_SCROLL_NULL) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no == 0) {
            SetScrollNull(*rHandle.Get());
        }
    }
}

void msw::CStageDodgeDOD56::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                             CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                             long n)
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

void msw::CStageDodgeDOD56::OnSceneEntry()
{
    Stage15_OnSceneEntry();
}

void msw::CStageDodgeDOD56::OnSceneReentry()
{
    Stage15_OnSceneReentry();
}

void msw::CStageDodgeDOD56::OnGimmickStart()
{
    Case_RespawnAll();
}

void msw::CStageDodgeDOD56::OnGimmickStop()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnTurnInit()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnPeriodInit()
{
    Stage15_OnPeriodInit();
}

void msw::CStageDodgeDOD56::OnTurnEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnGameEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnEntranceEventStart()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnEntranceEventGmkStart()
{
    Case_RespawnAll();
}

void msw::CStageDodgeDOD56::OnEntranceEventGmkEnd()
{
    Case_InactivateAll();
}

void msw::CStageDodgeDOD56::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_MSG)) {
            SE_Play(SE_SLOT_MSG, SE_AMBIENT_0, 0.0f, 3);
        }
        if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
            SE_Play(SE_SLOT_AMBIENT, SE_AMBIENT_2, 0.0f, 3);
        }
    }
}

const msw::CStageMan *msw::CStageDodgeDOD56::GetStageMan() const
{
    return this;
}

// The rain never changes: the stage is always rainy.
void msw::CStageDodgeDOD56::Rainy_Set(int bRainy)
{
}

void msw::CStageDodgeDOD56::Rainy_Reset()
{
}

void msw::CStageDodgeDOD56::Rainy_Update(float frame)
{
}

int msw::CStageDodgeDOD56::IsRainy() const
{
    return true;
}

// No cases.
long msw::CStageDodgeDOD56::GetParam_CaseNum() const
{
    return 0;
}

msw::CStgGmk_Case *msw::CStageDodgeDOD56::NewCase()
{
    return 0;
}

float msw::CStageDodgeDOD56::GetParam_ShipCycle() const
{
    return DODGE_PRM_S56_SHIP_CYCLE;
}

float msw::CStageDodgeDOD56::GetParam_ShipStopTime() const
{
    return DODGE_PRM_S56_SHIP_STOP_TIME;
}

float msw::CStageDodgeDOD56::GetParam_ShipStopTimeRnd() const
{
    return DODGE_PRM_S56_SHIP_STOP_TIME_RND;
}

float msw::CStageDodgeDOD56::GetParam_ChrFlowVel() const
{
    return DODGE_PRM_S56_CHR_FLOW_VEL;
}

// The angle is given in degrees.
float msw::CStageDodgeDOD56::GetParam_ShipRotWidth() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S56_SHIP_ROT_ANGLE);
}

long msw::CStageDodgeDOD56::GetParam_ShipSe() const
{
    return SE_SHIP;
}
