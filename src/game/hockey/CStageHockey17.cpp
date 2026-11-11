// msw::CStageHockey17: hockey stage 17, the turntable and trolley stage.
// A turntable carries the players round, a signal warns before it turns, a
// trolley runs over the court and trains pass in the background
// (CStage17Base, a DOL class, does the work; this class hands its hooks on
// and gives it the tuning and the trolley class). The goals have doors that
// open while the trolley is near them (CGoalObj17). The hockey trolley
// (msw::CStgGmk_Trolley_Hockey) is all inline, so its vtable and weak
// functions are emitted here too.
// The trolley class is declared first: vtables come out in .data in reverse
// order of class declaration, and the original has this class's first.
#include "game/msw/CStgGmk_Trolley_Hockey.hpp"
#include "game/msw/CStageHockey17.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/HockeyPrm.hpp"
#include "nw4r/math/types.h"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S17_PRM_TYPE {
        S17_PRM_TYPE_END,
        S17_PRM_TYPE_LONG,
        S17_PRM_TYPE_FLOAT
    };

    struct S17_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S17_PRM_TBL s_PrmTbl[] = {
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_INIT_STOP_TIME, 0x00 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_STOP_TIME, 0x04 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_STOP_TIME_RND, 0x08 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_ROT_TIME, 0x0C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_ROT_TIME_RND, 0x10 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_SPEED, 0x14 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TT_DIR, 0x18 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_ACC_TIME, 0x1C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_RADIUS, 0x20 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_OPEN_RATE, 0x24 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_WARN_TIME, 0x28 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_WARN_AFTER_TIME, 0x2C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TT_WARN_SE_STOP_TIME, 0x30 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_INIT_RATE, 0x34 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_RATE, 0x38 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_PRE_WAIT, 0x3C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_SPEED, 0x40 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TROLLEY_DIR_TYPE, 0x44 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TROLLEY_NUM_MIN, 0x48 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TROLLEY_NUM_MAX, 0x4C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_LOC_OFFSET, 0x50 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_WIDTH, 0x54 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_DEPTH, 0x58 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_HEIGHT, 0x5C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_ATK_WIDTH, 0x60 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_ATK_DEPTH, 0x64 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_ATK_HEIGHT, 0x68 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_MDL_WIDTH, 0x6C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_MDL_DEPTH, 0x70 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_TROLLEY_MDL_HEIGHT, 0x74 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TROLLEY_PAT_TYPE, 0x78 },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_TROLLEY_PAT_NO, 0x7C },
        { S17_PRM_TYPE_LONG, &HOCKEY_PRM_S17_DEB_RELOAD, 0x80 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_HEYHO_COIN_RATE, 0x84 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_HEYHO_B_DIST_MIN, 0x88 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_HEYHO_B_DIST_MAX, 0x8C },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_HEYHO_C_DIST_MIN, 0x90 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_HEYHO_C_DIST_MAX, 0x94 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_GOAL_X1_LENGTH, 0x98 },
        { S17_PRM_TYPE_FLOAT, &HOCKEY_PRM_S17_GOAL_RADIUS, 0x9C },
        { S17_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table (names guessed).
enum {
    MDL_TURNTABLE = 10, // "s<n>_ground_02": the turntable
    MDL_SIGNAL_0 = 20,  // "s<n>_goal_01": signals
    MDL_SIGNAL_1 = 21,  // "s<n>_goal_02"
    MDL_STEAM = 30,     // "s<n>_ef_steam...": animations rewound each turn
    MDL_TRAIN = 40      // "s<n>_train04", "_05": the trains
};

// Flag of a model list node that OnTurnInit rewinds (see MDL_STEAM).
static const unsigned long NODE_FLAG_REWIND = 0x20;

// Sound slots and sounds of the stage (names guessed). Sound event 0
// starts a looping sound unless it is already playing; events 1 to 4 play
// another. The goal doors have their own sounds.
enum {
    SE_SLOT_LOOP = 2,
    SE_SLOT_EVENT = 3,
    SE_DOOR_OPEN = 0xD98,
    SE_DOOR_CLOSE = 0xD99,
    SE_LOOP = 0xD9A,
    SE_EVENT = 0xD9B
};

// The effect of both effect types.
static const long EFFECT_ID = 0x70D;

// The goal model's animations (indices into the name table below).
enum {
    GOAL_ANM_IDLE,
    GOAL_ANM_SHOT,
    GOAL_ANM_SP_SHOT,
    GOAL_ANM_DOOR_OPEN,
    GOAL_ANM_DOOR_IDLE,
    GOAL_ANM_DOOR_CLOSE,
    GOAL_ANM_SIGNAL_OFF, // "_01_m01"
    GOAL_ANM_SIGNAL_ON   // "_01_m02"
};

const void *msw::CStageHockey17::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey17::CStageHockey17(bcs::CTask *pParent) : CStageManHockey(pParent), CStage17Base()
{
}

msw::CStageHockey17::~CStageHockey17()
{
}

// The goal doors open while the trolley's box meets the goal's box widened
// by five units; they close otherwise.
void msw::CStageHockey17::Main_(float frame)
{
    CStageManHockey::Main_(frame);
    Stage17_Main(frame);

    int abDoorOpen[2] = { 0, 0 };
    for (long t = 0; t < GetTrolleyMaxNum(); t++) {
        const CStgGmk_Trolley *pTrolley = GetTrolley(t);
        if (pTrolley == 0) {
            continue;
        }
        for (long i = 0; i < 2; i++) {
            CGoalObj *pGoal = GetGoal(i);
            if (pGoal == 0) {
                continue;
            }

            // The trolley's box: its body, turned to its direction.
            bcs::FVEC3 pos = pTrolley->GetPos();
            float width = pTrolley->GetBodyWidth();
            float depth = pTrolley->GetBodyDepth();
            bcs::FVEC3 trolleyMin(0.5f * -width, 0.0f, 0.5f * -depth);
            bcs::FVEC3 trolleyMax(0.5f * width, 0.0f, 0.5f * depth);
            bcs::BCS_Vec_RotY(trolleyMin, trolleyMin, pTrolley->GetDefaultRotY());
            bcs::BCS_Vec_RotY(trolleyMax, trolleyMax, pTrolley->GetDefaultRotY());
            bcs::FVEC3Add(&trolleyMin, &trolleyMin, &pos);
            bcs::FVEC3Add(&trolleyMax, &trolleyMax, &pos);

            // The goal's box.
            CStageMan::GetInstance(); // result not used
            bcs::FVEC3 goalPos = pGoal->GetPos();
            float left = goalPos.x - 0.5f * GetGoalWidth(i);
            float right = 0.5f * GetGoalWidth(i) + goalPos.x;
            float front = goalPos.z - GetGoalDepth(i);
            float back = goalPos.z + GetGoalDepth(i);

            nw4r::math::AABB trolleyBox(reinterpret_cast<const nw4r::math::VEC3 &>(trolleyMin),
                                        reinterpret_cast<const nw4r::math::VEC3 &>(trolleyMax));
            nw4r::math::VEC3 goalMax(right + 5.0f, 1.0f, back + 5.0f);
            nw4r::math::VEC3 goalMin(left - 5.0f, -1.0f, front - 5.0f);
            nw4r::math::AABB goalBox(goalMin, goalMax);
            if (nw4r::math::IntersectionAABB(&trolleyBox, &goalBox)) {
                abDoorOpen[i] = 1;
            }
        }
    }

    for (long i = 0; i < 2; i++) {
        CGoalObj17 *pGoal = static_cast<CGoalObj17 *>(GetGoal(i));
        if (pGoal != 0) {
            if (abDoorOpen[i]) {
                pGoal->DoorOpen();
            } else {
                pGoal->DoorClose();
            }
        }
    }
}

void msw::CStageHockey17::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManHockey::SendMessage(msg, arg0, arg1, arg2);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey17::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_02$", MDL_TURNTABLE, 0, 0, NewMdl },
        { "^s[0-9]+_goal_01$", MDL_SIGNAL_0, 0, 0, NewMdl },
        { "^s[0-9]+_goal_02$", MDL_SIGNAL_1, 0, 0, NewMdl },
        { "^s[0-9]+_ef_steam.*", MDL_STEAM, 0, 0, NewMdl },
        { "^s[0-9]+_train04$", MDL_TRAIN, 0, 0, NewMdl },
        { "^s[0-9]+_train05$", MDL_TRAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageHockey17::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                          nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_TURNTABLE) {
        CTurnTable *pTable = GetTurnTable(0);
        if (pTable != 0) {
            pTable->Assign(0, pObj);
        }
    }
}

void msw::CStageHockey17::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_SIGNAL_0:
            AddSignalMdl(pNode);
            break;
        case MDL_SIGNAL_1:
            AddSignalMdl(pNode);
            break;
        case MDL_STEAM:
            pNode->OnFlag(NODE_FLAG_REWIND);
            break;
        case MDL_TRAIN:
            AddTrainMdl(pNode);
            break;
    }

    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageHockey17::OnSceneEntry()
{
    Stage17_OnSceneEntry();
}

bool msw::CStageHockey17::Stage17_IsGimmickReady() const
{
    if (IsGimmickRunning() && IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 8)) {
        return true;
    }
    return false;
}

int msw::CStageHockey17::IsRotReady_() const
{
    return m_bRotReady;
}

void msw::CStageHockey17::OnGimmickReset()
{
    Stage17_OnGimmickReset();
}

void msw::CStageHockey17::OnGimmickStart()
{
    m_bRotReady = 1;
    Stage17_OnGimmickStart();
}

void msw::CStageHockey17::OnGimmickStop()
{
    m_bRotReady = 0;
    Stage17_OnGimmickStop();
}

void msw::CStageHockey17::OnGimmickCancel()
{
    Stage17_OnGimmickCancel();
}

void msw::CStageHockey17::OnGimmickPause()
{
    Stage17_OnGimmickPause();
}

void msw::CStageHockey17::OnGimmickResume()
{
    Stage17_OnGimmickResume();
}

void msw::CStageHockey17::OnGameStart()
{
    m_bRotReady = 0;
}

void msw::CStageHockey17::OnGameEnd()
{
    m_bRotReady = 0;
}

// Rewind the flagged animations of every model.
void msw::CStageHockey17::OnTurnInit()
{
    CMdlGroup *pGroup = GetMdlGroup(0);
    for (bcs::CListNodeBase *pIt = pGroup->GetList().GetFirst(); pIt != pGroup->GetList().GetEnd();
         bcs::CListNodeBase::Step(pIt)) {
        bcs::CListBase &rList = static_cast<CMdlGroup::CNode *>(pIt)->GetMdl()->GetNodeList()->GetList();

        for (bcs::CListNodeBase *pNodeIt = rList.GetFirst(); pNodeIt != rList.GetEnd();
             bcs::CListNodeBase::Step(pNodeIt)) {
            CStageMdlListNode *pNode = static_cast<CStageMdlListNode *>(pNodeIt);

            if ((pNode->GetFlag() & NODE_FLAG_REWIND) == NODE_FLAG_REWIND) {
                CStageNodeAnm *pAnm = pNode->GetAnm();
                if (pAnm != 0) {
                    pAnm->SetFrame(-1, 0.0f);
                }
            }
        }
    }
}

void msw::CStageHockey17::OnEntranceEventStart()
{
    m_bRotReady = 0;
}

void msw::CStageHockey17::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_LOOP)) {
                SE_Play(SE_SLOT_LOOP, SE_LOOP, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

long msw::CStageHockey17::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage17_GetFloorAttribute(rPos, type);
}

// The turntable moves what stands on it for flow types 0 to 3 and 6.
void msw::CStageHockey17::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            Stage17_GetFloorFlow(rFlow, rPos, type);
            break;
        default:
            rFlow.Set(0.0f, 0.0f, 0.0f);
            break;
    }
}

float msw::CStageHockey17::GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            return Stage17_GetFloorFlowRot(rPos, type);
    }
    return 0.0f;
}

void msw::CStageHockey17::CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            Stage17_CalcFloorFlowPos(rOut, rPos, type);
            return;
    }
    rOut = rPos;
}

float msw::CStageHockey17::CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            return Stage17_CalcFloorFlowRot(rPos, type);
    }
    return 0.0f;
}

void msw::CStageHockey17::SignalStart()
{
    CStage17Base::SignalStart();
    for (long i = 0; i < 2; i++) {
        CGoalObj17 *pGoal = static_cast<CGoalObj17 *>(GetGoal(i));
        if (pGoal != 0) {
            pGoal->SignalStart();
        }
    }
}

void msw::CStageHockey17::SignalStop()
{
    CStage17Base::SignalStop();
    for (long i = 0; i < 2; i++) {
        CGoalObj17 *pGoal = static_cast<CGoalObj17 *>(GetGoal(i));
        if (pGoal != 0) {
            pGoal->SignalStop();
        }
    }
}

long msw::CStageHockey17::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
        case 1:
            return EFFECT_ID;
    }
    return -1;
}

long msw::CStageHockey17::GetTrolleyMaxNum()
{
    return 1;
}

const msw::CStgGmk_Trolley *msw::CStageHockey17::GetTrolley(long idx)
{
    const CStageHockey17 *pStage = static_cast<const CStageHockey17 *>(CStageMan::GetInstance());

    if (idx < 0 || idx > 1) {
        return 0;
    }
    if (!pStage->IsTrolleyActive()) {
        return 0;
    }

    const CStgGmk_Trolley *pTrolley = pStage->GetTrolley_();
    if (pTrolley == 0 || !pTrolley->IsActive()) {
        return 0;
    }
    return pTrolley;
}

void msw::CStageHockey17::TrolleyForceReset()
{
    static_cast<CStageHockey17 *>(CStageMan::GetInstance())->TrolleyReset();
}

void msw::CStageHockey17::Disp()
{
    CStageManHockey::Disp();
}

// The suffixes of the goal animations' names.
static const char *s_GoalAnmNameTbl[] = {
    "_idle", "_shot", "_sp_shot", "_door_open", "_door_idle", "_door_close", "_01_m01", "_01_m02", 0,
};

// How each goal animation is played (the table ends with id -1).
static const msw::CGoalMdl::ANM_PARAM s_GoalAnmParamTbl[] = {
    { GOAL_ANM_IDLE, 2, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_SHOT, 0, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_SP_SHOT, 0, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_DOOR_OPEN, 0, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_DOOR_IDLE, 2, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_DOOR_CLOSE, 0, 1.0f, 0.0f, 0.0f, -3 },
    { GOAL_ANM_SIGNAL_OFF, 0, 1.0f, 0.0f, 0.0f, 2 },
    { GOAL_ANM_SIGNAL_ON, 0, 1.0f, 0.0f, 0.0f, 2 },
    { -1, 0, 0.0f, 0.0f, 0.0f, 0 },
};

msw::CStageHockey17::CGoalMdl17::~CGoalMdl17()
{
}

const msw::CGoalMdl::ANM_PARAM *msw::CStageHockey17::CGoalMdl17::GetAnmParamTbl() const
{
    return s_GoalAnmParamTbl;
}

const char *const *msw::CStageHockey17::CGoalMdl17::GetAnmNameTbl() const
{
    return s_GoalAnmNameTbl;
}

msw::CStageHockey17::CGoalObj17::~CGoalObj17()
{
}

// With its door open the goal skips CGoalObj's update.
void msw::CStageHockey17::CGoalObj17::Main()
{
    if (m_bDoorOpen) {
        CStgObj::Main();
    } else {
        CGoalObj::Main();
    }
}

void msw::CStageHockey17::CGoalObj17::DunkCancel()
{
    CGoalObjHockey::DunkCancel();
    DoorReset();
}

void msw::CStageHockey17::CGoalObj17::SignalStart()
{
    CStgMdl *pMdl = GetMdl(1);
    if (pMdl != 0) {
        pMdl->SetAnm(GOAL_ANM_SIGNAL_ON, 2, 1.0f, 0.0f, 0.0f, 2);
    }
}

void msw::CStageHockey17::CGoalObj17::SignalStop()
{
    CStgMdl *pMdl = GetMdl(1);
    if (pMdl != 0) {
        pMdl->SetAnm(GOAL_ANM_SIGNAL_OFF, 2, 1.0f, 0.0f, 0.0f, 2);
    }
}

void msw::CStageHockey17::CGoalObj17::DoorOpen()
{
    if (!m_bDoorOpen) {
        CStgMdl *pMdl = GetMdl(1);
        if (pMdl != 0) {
            pMdl->SetAnm(GOAL_ANM_DOOR_OPEN, 0, 1.0f, 0.0f, 0.0f, 1);
            pMdl->PushAnm(GOAL_ANM_DOOR_IDLE, 2, 1.0f, 0.0f, 0.0f, 1);
        }
        CGameSnd::Play(SE_DOOR_OPEN, GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
    }
    m_bDoorOpen = 1;
}

void msw::CStageHockey17::CGoalObj17::DoorClose()
{
    if (m_bDoorOpen) {
        CStgMdl *pMdl = GetMdl(1);
        if (pMdl != 0) {
            pMdl->SetAnm(GOAL_ANM_DOOR_CLOSE, 0, 1.0f, 0.0f, 0.0f, 1);
        }
        CGameSnd::Play(SE_DOOR_CLOSE, GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
    }
    m_bDoorOpen = 0;
}

void msw::CStageHockey17::CGoalObj17::DoorReset()
{
    CStgMdl *pMdl = GetMdl(1);
    if (pMdl != 0) {
        pMdl->SetAnm(GOAL_ANM_IDLE, 2, 1.0f, 0.0f, 0.0f, 1);
    }
    m_bDoorOpen = 0;
}

msw::CStgGmk_Trolley *msw::CStageHockey17::NewTrolley()
{
    return new (1) CStgGmk_Trolley_Hockey(this);
}

long msw::CStageHockey17::GetTurnTableNum() const
{
    return 1;
}

// The times are given in seconds.
float msw::CStageHockey17::GetParam_TTInitialWait() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_INIT_STOP_TIME;
}

float msw::CStageHockey17::GetParam_TTWait() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_STOP_TIME;
}

float msw::CStageHockey17::GetParam_TTWaitRnd() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_STOP_TIME_RND;
}

float msw::CStageHockey17::GetParam_TTRadius() const
{
    return HOCKEY_PRM_S17_TT_RADIUS;
}

float msw::CStageHockey17::GetParam_TTOffset() const
{
    return 0.0f;
}

float msw::CStageHockey17::GetParam_TTRotTime() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_ROT_TIME;
}

float msw::CStageHockey17::GetParam_TTRotTimeRnd() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_ROT_TIME_RND;
}

float msw::CStageHockey17::GetParam_TTRotAccTime() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_ACC_TIME;
}

// The speed is given in degrees per second.
float msw::CStageHockey17::GetParam_TTRotSpeed() const
{
    return bcs::DEG_TO_RAD(HOCKEY_PRM_S17_TT_SPEED) / 60.0f;
}

long msw::CStageHockey17::GetParam_TTRotDirType() const
{
    return HOCKEY_PRM_S17_TT_DIR;
}

// The rates are given in percent.
float msw::CStageHockey17::GetParam_TTOpenRate() const
{
    return 0.01f * HOCKEY_PRM_S17_TT_OPEN_RATE;
}

float msw::CStageHockey17::GetParam_TTOpenAngle() const
{
    return 0.0f;
}

float msw::CStageHockey17::GetParam_SignalTime() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_WARN_TIME;
}

float msw::CStageHockey17::GetParam_SignalAfterTime() const
{
    return 60.0f * HOCKEY_PRM_S17_TT_WARN_AFTER_TIME;
}

float msw::CStageHockey17::GetParam_SignalSEStopTime() const
{
    return HOCKEY_PRM_S17_TT_WARN_SE_STOP_TIME;
}

float msw::CStageHockey17::GetParam_TrolleyPreWait() const
{
    return 60.0f * HOCKEY_PRM_S17_TROLLEY_PRE_WAIT;
}

float msw::CStageHockey17::GetParam_TrolleyRate() const
{
    return 0.01f * HOCKEY_PRM_S17_TROLLEY_RATE;
}

float msw::CStageHockey17::GetParam_TrolleyInitRate() const
{
    return 0.01f * HOCKEY_PRM_S17_TROLLEY_INIT_RATE;
}

long msw::CStageHockey17::GetParam_TrolleyDirType() const
{
    return HOCKEY_PRM_S17_TROLLEY_DIR_TYPE;
}

float msw::CStageHockey17::GetParam_TrolleyLocOffset() const
{
    return HOCKEY_PRM_S17_TROLLEY_LOC_OFFSET;
}
