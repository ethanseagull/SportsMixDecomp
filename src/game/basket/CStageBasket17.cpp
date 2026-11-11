// Ported from src/game/dodge/CStageDodge17.cpp and checked against BasketF.
// msw::CStageBasket17: basket stage 17, the turntable and trolley stage.
// Two turntables carry the players round, a signal warns before they turn,
// a trolley runs over the court and trains pass in the background
// (CStage17Base, a DOL class, does the work; this class hands its hooks on
// and gives it the tuning and the trolley class). The basket trolley
// (msw::CStgGmk_Trolley_Basket) is all inline, so its vtable and weak
// functions are emitted here too.
// The trolley class is declared first: vtables come out in .data in reverse
// order of class declaration, and the original has this class's first.
#include "game/msw/CStgGmk_Trolley_Basket.hpp"
#include "game/msw/CStageBasket17.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/BasketPrm.hpp"

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
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_INIT_STOP_TIME, 0x00 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_STOP_TIME, 0x04 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_STOP_TIME_RND, 0x08 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_ROT_TIME, 0x0C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_ROT_TIME_RND, 0x10 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_SPEED, 0x14 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TT_DIR, 0x18 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_ACC_TIME, 0x1C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_OPEN_RATE, 0x20 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_WARN_TIME, 0x24 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_WARN_AFTER_TIME, 0x28 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TT_WARN_SE_STOP_TIME, 0x2C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_INIT_RATE, 0x30 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_RATE, 0x34 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_PRE_WAIT, 0x38 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_SPEED, 0x3C },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TROLLEY_DIR_TYPE, 0x40 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TROLLEY_NUM_MIN, 0x44 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TROLLEY_NUM_MAX, 0x48 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_LOC_OFFSET, 0x4C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_WIDTH, 0x50 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_DEPTH, 0x54 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_HEIGHT, 0x58 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_ATK_WIDTH, 0x5C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_ATK_DEPTH, 0x60 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_ATK_HEIGHT, 0x64 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_MDL_WIDTH, 0x68 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_MDL_DEPTH, 0x6C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_TROLLEY_MDL_HEIGHT, 0x70 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TROLLEY_PAT_TYPE, 0x74 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_TROLLEY_PAT_NO, 0x78 },
        { S17_PRM_TYPE_LONG, &BASKET_PRM_S17_DEB_RELOAD, 0x7C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_HEYHO_COIN_RATE, 0x80 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_HEYHO_B_DIST_MIN, 0x84 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_HEYHO_B_DIST_MAX, 0x88 },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_HEYHO_C_DIST_MIN, 0x8C },
        { S17_PRM_TYPE_FLOAT, &BASKET_PRM_S17_HEYHO_C_DIST_MAX, 0x90 },
        { S17_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table (names guessed).
enum {
    MDL_TURNTABLE_0 = 10, // "s<n>_ground_02": the first turntable
    MDL_TURNTABLE_1 = 11, // "s<n>_ground_03": the second turntable
    MDL_SIGNAL_0 = 20,    // "s<n>_goal_01": signals
    MDL_SIGNAL_1 = 21,    // "s<n>_goal_02"
    MDL_STEAM = 30,       // "s<n>_ef_steam...": animations rewound each turn
    MDL_TRAIN = 40        // "s<n>_train04", "_05": the trains
};

// Flag of a model list node that OnTurnInit rewinds (see MDL_STEAM).
static const unsigned long NODE_FLAG_REWIND = 0x20;

// Sound slots and sounds of the stage (names guessed). Sound event 0
// starts a looping sound unless it is already playing; events 1 to 3 play
// another.
enum {
    SE_SLOT_LOOP = 2,
    SE_SLOT_EVENT = 3,
    SE_LOOP = 0xD9A,
    SE_EVENT = 0xD9B
};

// The effect of both effect types.
static const long EFFECT_ID = 0x6BF;

const void *msw::CStageBasket17::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket17::CStageBasket17(bcs::CTask *pParent) : CStageManBasket(pParent), CStage17Base(), m_bRotReady(0)
{
    m_apTurnTableObj[0] = 0;
    m_apTurnTableObj[1] = 0;
}

msw::CStageBasket17::~CStageBasket17()
{
}

void msw::CStageBasket17::Main_(float frame)
{
    CStageManBasket::Main_(frame);
    Stage17_Main(frame);
}

void msw::CStageBasket17::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManBasket::SendMessage(msg, arg0, arg1, arg2);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket17::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_02$", MDL_TURNTABLE_0, 0, 0, NewMdl },
        { "^s[0-9]+_ground_03$", MDL_TURNTABLE_1, 0, 0, NewMdl },
        { "^s[0-9]+_goal_01$", MDL_SIGNAL_0, 0, 0, NewMdl },
        { "^s[0-9]+_goal_02$", MDL_SIGNAL_1, 0, 0, NewMdl },
        { "^s[0-9]+_ef_steam.*", MDL_STEAM, 0, 0, NewMdl },
        { "^s[0-9]+_train04$", MDL_TRAIN, 0, 0, NewMdl },
        { "^s[0-9]+_train05$", MDL_TRAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageBasket17::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                          nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    // The turntable models are kept so OnTurnInit can swap them round.
    switch (pRule->m_nId) {
        case MDL_TURNTABLE_0: {
            CTurnTable *pTable = GetTurnTable(0);
            if (pTable != 0) {
                pTable->Assign(0, pObj);
                m_apTurnTableObj[0] = pObj;
            }
            break;
        }
        case MDL_TURNTABLE_1: {
            CTurnTable *pTable = GetTurnTable(1);
            if (pTable != 0) {
                pTable->Assign(1, pObj);
                m_apTurnTableObj[1] = pObj;
            }
            break;
        }
    }
}

void msw::CStageBasket17::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
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

    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasket17::OnSceneEntry()
{
    Stage17_OnSceneEntry();
}

bool msw::CStageBasket17::Stage17_IsGimmickReady() const
{
    if (IsGimmickRunning() && IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 8)) {
        return true;
    }
    return false;
}

int msw::CStageBasket17::IsRotReady_() const
{
    return m_bRotReady;
}

void msw::CStageBasket17::OnGimmickReset()
{
    Stage17_OnGimmickReset();
}

void msw::CStageBasket17::OnGimmickStart()
{
    m_bRotReady = 1;
    Stage17_OnGimmickStart();
}

void msw::CStageBasket17::OnGimmickStop()
{
    m_bRotReady = 0;
    Stage17_OnGimmickStop();
}

void msw::CStageBasket17::OnGimmickCancel()
{
    Stage17_OnGimmickCancel();
}

void msw::CStageBasket17::OnGimmickPause()
{
    Stage17_OnGimmickPause();
}

void msw::CStageBasket17::OnGimmickResume()
{
    Stage17_OnGimmickResume();
}

void msw::CStageBasket17::OnGameStart()
{
    m_bRotReady = 0;
}

void msw::CStageBasket17::OnGameEnd()
{
    m_bRotReady = 0;
}

void msw::CStageBasket17::OnTurnInit()
{
    // Rewind the flagged animations of every model.
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
    // Give each turntable the model on the side its team attacks: swapped
    // when team 0's target goal is not goal 0.
    long swap = CGame::GetTargetGoalIdx(CGame::GetTeamColorIdx(0) != 0) != 0;
    for (long i = 0; i < 2; i++) {
        CTurnTable *pTable = GetTurnTable(i);
        if (pTable != 0) {
            pTable->Assign(i, m_apTurnTableObj[i ^ swap]);
        }
    }
}

void msw::CStageBasket17::OnEntranceEventStart()
{
    m_bRotReady = 0;
}

void msw::CStageBasket17::OnSoundEvent(long event)
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
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

long msw::CStageBasket17::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage17_GetFloorAttribute(rPos, type);
}

// The turntables move what stands on them for flow types 0 to 3 and 6.
void msw::CStageBasket17::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageBasket17::GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

void msw::CStageBasket17::CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageBasket17::CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

long msw::CStageBasket17::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
        case 1:
            return EFFECT_ID;
    }
    return -1;
}

const msw::CStgGmk_Trolley *msw::CStageBasket17::GetTrolley(long idx)
{
    const CStageBasket17 *pStage = static_cast<const CStageBasket17 *>(CStageMan::GetInstance());

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

void msw::CStageBasket17::TrolleyForceReset()
{
    static_cast<CStageBasket17 *>(CStageMan::GetInstance())->TrolleyReset();
}

void msw::CStageBasket17::Disp()
{
    CStageManBasket::Disp();
}

msw::CStgGmk_Trolley *msw::CStageBasket17::NewTrolley()
{
    return new (1) CStgGmk_Trolley_Basket(this);
}

long msw::CStageBasket17::GetTurnTableNum() const
{
    return 2;
}

// The times are given in seconds.
float msw::CStageBasket17::GetParam_TTInitialWait() const
{
    return 60.0f * BASKET_PRM_S17_TT_INIT_STOP_TIME;
}

float msw::CStageBasket17::GetParam_TTWait() const
{
    return 60.0f * BASKET_PRM_S17_TT_STOP_TIME;
}

float msw::CStageBasket17::GetParam_TTWaitRnd() const
{
    return 60.0f * BASKET_PRM_S17_TT_STOP_TIME_RND;
}

float msw::CStageBasket17::GetParam_TTRadius() const
{
    return 5.5f;
}

// The depth of the point half a unit in front of goal 0, along its
// direction.
float msw::CStageBasket17::GetParam_TTOffset() const
{
    bcs::FVEC3 pos = GetGoalPos(0);
    bcs::FVEC3 dir = GetGoalVec(0);
    bcs::BCS_Vec_Normalize(dir, dir);
    bcs::FVEC3Scale(&dir, &dir, 0.5f);
    bcs::FVEC3Add(&pos, &pos, &dir);
    return pos.z;
}

float msw::CStageBasket17::GetParam_TTRotTime() const
{
    return 60.0f * BASKET_PRM_S17_TT_ROT_TIME;
}

float msw::CStageBasket17::GetParam_TTRotTimeRnd() const
{
    return 60.0f * BASKET_PRM_S17_TT_ROT_TIME_RND;
}

float msw::CStageBasket17::GetParam_TTRotAccTime() const
{
    return 60.0f * BASKET_PRM_S17_TT_ACC_TIME;
}

// The speed is given in degrees per second.
float msw::CStageBasket17::GetParam_TTRotSpeed() const
{
    return bcs::DEG_TO_RAD(BASKET_PRM_S17_TT_SPEED) / 60.0f;
}

long msw::CStageBasket17::GetParam_TTRotDirType() const
{
    return BASKET_PRM_S17_TT_DIR;
}

// The rates are given in percent.
float msw::CStageBasket17::GetParam_TTOpenRate() const
{
    return 0.01f * BASKET_PRM_S17_TT_OPEN_RATE;
}

float msw::CStageBasket17::GetParam_TTOpenAngle() const
{
    return 0.0f;
}

float msw::CStageBasket17::GetParam_SignalTime() const
{
    return 60.0f * BASKET_PRM_S17_TT_WARN_TIME;
}

float msw::CStageBasket17::GetParam_SignalAfterTime() const
{
    return 60.0f * BASKET_PRM_S17_TT_WARN_AFTER_TIME;
}

float msw::CStageBasket17::GetParam_SignalSEStopTime() const
{
    return BASKET_PRM_S17_TT_WARN_SE_STOP_TIME;
}

float msw::CStageBasket17::GetParam_TrolleyPreWait() const
{
    return 60.0f * BASKET_PRM_S17_TROLLEY_PRE_WAIT;
}

float msw::CStageBasket17::GetParam_TrolleyRate() const
{
    return 0.01f * BASKET_PRM_S17_TROLLEY_RATE;
}

float msw::CStageBasket17::GetParam_TrolleyInitRate() const
{
    return 0.01f * BASKET_PRM_S17_TROLLEY_INIT_RATE;
}

long msw::CStageBasket17::GetParam_TrolleyDirType() const
{
    return BASKET_PRM_S17_TROLLEY_DIR_TYPE;
}

float msw::CStageBasket17::GetParam_TrolleyLocOffset() const
{
    return BASKET_PRM_S17_TROLLEY_LOC_OFFSET;
}
