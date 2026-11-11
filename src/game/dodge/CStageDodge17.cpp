// msw::CStageDodge17: dodge stage 17, the turntable and trolley stage.
// Two turntables carry the players round, a signal warns before they turn,
// a trolley runs over the court and trains pass in the background
// (CStage17Base, a DOL class, does the work; this class hands its hooks on
// and gives it the tuning and the trolley class). The dodgeball trolley
// (msw::CStgGmk_Trolley_Dodge) is all inline, so its vtable and weak
// functions are emitted here too.
#include "game/msw/CStageDodge17.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgGmk_Trolley_Dodge.hpp"
#include "game/msw/DodgePrm.hpp"

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
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_INIT_STOP_TIME, 0x00 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_STOP_TIME, 0x04 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_STOP_TIME_RND, 0x08 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_ROT_TIME, 0x0C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_ROT_TIME_RND, 0x10 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_SPEED, 0x14 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TT_DIR, 0x18 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_ACC_TIME, 0x1C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_RADIUS, 0x20 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_OPEN_RATE, 0x24 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_WARN_TIME, 0x28 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_WARN_AFTER_TIME, 0x2C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TT_WARN_SE_STOP_TIME, 0x30 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_INIT_RATE, 0x34 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_RATE, 0x38 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_PRE_WAIT, 0x3C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_SPEED, 0x40 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TROLLEY_DIR_TYPE, 0x44 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TROLLEY_NUM_MIN, 0x48 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TROLLEY_NUM_MAX, 0x4C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_LOC_OFFSET, 0x50 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_WIDTH, 0x54 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_DEPTH, 0x58 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_HEIGHT, 0x5C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_ATK_WIDTH, 0x60 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_ATK_DEPTH, 0x64 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_ATK_HEIGHT, 0x68 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_MDL_WIDTH, 0x6C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_MDL_DEPTH, 0x70 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_TROLLEY_MDL_HEIGHT, 0x74 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TROLLEY_PAT_TYPE, 0x78 },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_TROLLEY_PAT_NO, 0x7C },
        { S17_PRM_TYPE_LONG, &DODGE_PRM_S17_DEB_RELOAD, 0x80 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_HEYHO_COIN_RATE, 0x84 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_HEYHO_B_DIST_MIN, 0x88 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_HEYHO_B_DIST_MAX, 0x8C },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_HEYHO_C_DIST_MIN, 0x90 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_HEYHO_C_DIST_MAX, 0x94 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_OUTER_DEPTH, 0x98 },
        { S17_PRM_TYPE_FLOAT, &DODGE_PRM_S17_OUTER_FRONT, 0x9C },
        { S17_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table (names guessed).
enum {
    MDL_TURNTABLE_0 = 10, // "s<n>_ground_03": the first turntable
    MDL_TURNTABLE_1 = 11, // "s<n>_ground_02": the second turntable
    MDL_GROUND = 12,      // "s<n>_ground_04", "_05": drawn after the rest
    MDL_SIGNAL_0 = 20,    // "s<n>_goal_01": signals
    MDL_SIGNAL_1 = 21,    // "s<n>_goal_02"
    MDL_STEAM = 30,       // "s<n>_ef_steam...": animations rewound each turn
    MDL_TRAIN = 40        // "s<n>_train04", "_05": the trains
};

// The messages SendMessage reacts to (names guessed).
enum {
    MSG_27 = 0x27, // with 11: fix the turntables towards team arg1
    MSG_3C = 0x3C  // reset the gimmicks
};

// Flag of a model list node that OnTurnInit rewinds (see MDL_STEAM).
static const unsigned long NODE_FLAG_REWIND = 0x20;

// Sound slots and sounds of the stage (names guessed). Sound event 0
// starts a looping sound unless it is already playing; events 1 to 6 play
// another.
enum {
    SE_SLOT_LOOP = 2,
    SE_SLOT_EVENT = 3,
    SE_LOOP = 0xD9A,
    SE_EVENT = 0xD9B
};

// The effect of both effect types.
static const long EFFECT_ID = 0x67F;

// The team of the court half the area flags name: bit 0 for team 0, bit 1
// for team 1, -1 for neither (name guessed).
static inline long GetAreaTeam(unsigned long area)
{
    if (area & 1) {
        return 0;
    }
    if (area & 2) {
        return 1;
    }
    return -1;
}

const void *msw::CStageDodge17::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge17::CStageDodge17(bcs::CTask *pParent) : CStageManDodge(pParent), CStage17Base(), m_bRotReady(0)
{
}

msw::CStageDodge17::~CStageDodge17()
{
}

void msw::CStageDodge17::Main_(float frame)
{
    CStageManDodge::Main_(frame);
    Stage17_Main(frame);
}

void msw::CStageDodge17::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManDodge::SendMessage(msg, arg0, arg1, arg2);

    switch (msg) {
        case MSG_3C:
            Stage17_Reset();
            m_bRotReady = 0;
            break;
        case MSG_27:
            if (arg0 == 11) {
                SetFixTurnTableTeam(arg1);
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge17::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_03$", MDL_TURNTABLE_0, 0, 0, NewMdl },
        { "^s[0-9]+_ground_02$", MDL_TURNTABLE_1, 0, 0, NewMdl },
        { "^s[0-9]+_ground_04$", MDL_GROUND, 0, 0, NewMdl },
        { "^s[0-9]+_ground_05$", MDL_GROUND, 0, 0, NewMdl },
        { "^s[0-9]+_goal_01$", MDL_SIGNAL_0, 0, 0, NewMdl },
        { "^s[0-9]+_goal_02$", MDL_SIGNAL_1, 0, 0, NewMdl },
        { "^s[0-9]+_ef_steam.*", MDL_STEAM, 0, 0, NewMdl },
        { "^s[0-9]+_train04$", MDL_TRAIN, 0, 0, NewMdl },
        { "^s[0-9]+_train05$", MDL_TRAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManDodge::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge17::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                         nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_TURNTABLE_0: {
            CTurnTable *pTable = GetTurnTable(0);
            if (pTable != 0) {
                pTable->Assign(0, pObj);
            }
            break;
        }
        case MDL_TURNTABLE_1: {
            CTurnTable *pTable = GetTurnTable(1);
            if (pTable != 0) {
                pTable->Assign(1, pObj);
            }
            break;
        }
    }
}

void msw::CStageDodge17::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_GROUND:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
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
}

void msw::CStageDodge17::OnSceneEntry()
{
    Stage17_OnSceneEntry();
}

bool msw::CStageDodge17::Stage17_IsGimmickReady() const
{
    if (IsGimmickRunning() && IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 8)) {
        return true;
    }
    return false;
}

int msw::CStageDodge17::IsRotReady_() const
{
    return m_bRotReady;
}

void msw::CStageDodge17::OnGimmickReset()
{
    Stage17_OnGimmickReset();
}

void msw::CStageDodge17::OnGimmickStart()
{
    m_bRotReady = 1;
    Stage17_OnGimmickStart();
}

void msw::CStageDodge17::OnGimmickStop()
{
    m_bRotReady = 0;
    Stage17_OnGimmickStop();
}

void msw::CStageDodge17::OnGimmickCancel()
{
    Stage17_OnGimmickCancel();
}

void msw::CStageDodge17::OnGimmickPause()
{
    Stage17_OnGimmickPause();
}

void msw::CStageDodge17::OnGimmickResume()
{
    Stage17_OnGimmickResume();
}

void msw::CStageDodge17::OnGameStart()
{
    m_bRotReady = 0;
}

void msw::CStageDodge17::OnGameEnd()
{
    m_bRotReady = 0;
}

void msw::CStageDodge17::OnTurnInit()
{
    Stage17_OnTurnInit();

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
}

void msw::CStageDodge17::OnEntranceEventStart()
{
    m_bRotReady = 0;
}

void msw::CStageDodge17::OnSoundEvent(long event)
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
        case 5:
        case 6:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

long msw::CStageDodge17::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage17_GetFloorAttribute(rPos, type);
}

// The turntables move what stands on them for flow types 0 to 3 and 6;
// for types 0 and 6 only inside a court half.
void msw::CStageDodge17::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            long team = GetAreaTeam(GetAreaFlagByPos(rPos, 3));
            if ((type == 0 || type == 6) && team < 0) {
                rFlow.Set(0.0f, 0.0f, 0.0f);
            } else {
                Stage17_GetFloorFlow(rFlow, rPos, type);
            }
            break;
        }
        default:
            rFlow.Set(0.0f, 0.0f, 0.0f);
            break;
    }
}

float msw::CStageDodge17::GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            long team = GetAreaTeam(GetAreaFlagByPos(rPos, 3));
            if ((type == 0 || type == 6) && team < 0) {
                return 0.0f;
            }
            return Stage17_GetFloorFlowRot(rPos, type);
        }
    }
    return 0.0f;
}

void msw::CStageDodge17::CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            long team = GetAreaTeam(GetAreaFlagByPos(rPos, 3));
            if ((type == 0 || type == 6) && team < 0) {
                break;
            }
            Stage17_CalcFloorFlowPos(rOut, rPos, type);
            return;
        }
    }
    rOut = rPos;
}

float msw::CStageDodge17::CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            long team = GetAreaTeam(GetAreaFlagByPos(rPos, 3));
            if ((type == 0 || type == 6) && team < 0) {
                return 0.0f;
            }
            return Stage17_CalcFloorFlowRot(rPos, type);
        }
    }
    return 0.0f;
}

long msw::CStageDodge17::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
        case 1:
            return EFFECT_ID;
    }
    return -1;
}

float msw::CStageDodge17::GetBaseOuterFront_() const
{
    return DODGE_PRM_S17_OUTER_FRONT;
}

float msw::CStageDodge17::GetBaseOuterDepth_() const
{
    return DODGE_PRM_S17_OUTER_DEPTH;
}

float msw::CStageDodge17::GetBaseOuterWidth_() const
{
    return CStageManDodge::GetBaseOuterWidth_() - 0.2f;
}

float msw::CStageDodge17::GetBaseGroundWidth_() const
{
    return CStageManDodge::GetBaseGroundWidth_() - 0.2f;
}

long msw::CStageDodge17::GetTrolleyMaxNum()
{
    return 1;
}

const msw::CStgGmk_Trolley *msw::CStageDodge17::GetTrolley(long idx)
{
    const CStageDodge17 *pStage = static_cast<const CStageDodge17 *>(CStageMan::GetInstance());

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

void msw::CStageDodge17::TrolleyForceReset()
{
    static_cast<CStageDodge17 *>(CStageMan::GetInstance())->TrolleyReset();
}

void msw::CStageDodge17::Disp()
{
    CStageManDodge::Disp();
}

msw::CStgGmk_Trolley *msw::CStageDodge17::NewTrolley()
{
    return new (1) CStgGmk_Trolley_Dodge(this);
}

long msw::CStageDodge17::GetTurnTableNum() const
{
    return 2;
}

// The times are given in seconds.
float msw::CStageDodge17::GetParam_TTInitialWait() const
{
    return 60.0f * DODGE_PRM_S17_TT_INIT_STOP_TIME;
}

float msw::CStageDodge17::GetParam_TTWait() const
{
    return 60.0f * DODGE_PRM_S17_TT_STOP_TIME;
}

float msw::CStageDodge17::GetParam_TTWaitRnd() const
{
    return 60.0f * DODGE_PRM_S17_TT_STOP_TIME_RND;
}

float msw::CStageDodge17::GetParam_TTRadius() const
{
    return DODGE_PRM_S17_TT_RADIUS;
}

// Half the court depth, plus 1.5.
float msw::CStageDodge17::GetParam_TTOffset() const
{
    return 0.5f * GetBaseCourtDepth_() + 1.5f;
}

float msw::CStageDodge17::GetParam_TTRotTime() const
{
    return 60.0f * DODGE_PRM_S17_TT_ROT_TIME;
}

float msw::CStageDodge17::GetParam_TTRotTimeRnd() const
{
    return 60.0f * DODGE_PRM_S17_TT_ROT_TIME_RND;
}

float msw::CStageDodge17::GetParam_TTRotAccTime() const
{
    return 60.0f * DODGE_PRM_S17_TT_ACC_TIME;
}

// The speed is given in degrees per second.
float msw::CStageDodge17::GetParam_TTRotSpeed() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S17_TT_SPEED) / 60.0f;
}

long msw::CStageDodge17::GetParam_TTRotDirType() const
{
    return DODGE_PRM_S17_TT_DIR;
}

// The rates are given in percent.
float msw::CStageDodge17::GetParam_TTOpenRate() const
{
    return 0.01f * DODGE_PRM_S17_TT_OPEN_RATE;
}

float msw::CStageDodge17::GetParam_TTOpenAngle() const
{
    return 0.0f;
}

float msw::CStageDodge17::GetParam_SignalTime() const
{
    return 60.0f * DODGE_PRM_S17_TT_WARN_TIME;
}

float msw::CStageDodge17::GetParam_SignalAfterTime() const
{
    return 60.0f * DODGE_PRM_S17_TT_WARN_AFTER_TIME;
}

float msw::CStageDodge17::GetParam_SignalSEStopTime() const
{
    return DODGE_PRM_S17_TT_WARN_SE_STOP_TIME;
}

float msw::CStageDodge17::GetParam_TrolleyPreWait() const
{
    return 60.0f * DODGE_PRM_S17_TROLLEY_PRE_WAIT;
}

float msw::CStageDodge17::GetParam_TrolleyRate() const
{
    return 0.01f * DODGE_PRM_S17_TROLLEY_RATE;
}

float msw::CStageDodge17::GetParam_TrolleyInitRate() const
{
    return 0.01f * DODGE_PRM_S17_TROLLEY_INIT_RATE;
}

long msw::CStageDodge17::GetParam_TrolleyDirType() const
{
    return DODGE_PRM_S17_TROLLEY_DIR_TYPE;
}

float msw::CStageDodge17::GetParam_TrolleyLocOffset() const
{
    return DODGE_PRM_S17_TROLLEY_LOC_OFFSET;
}
