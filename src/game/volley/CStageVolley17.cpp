// msw::CStageVolley17: volley stage 17, the turntable and trolley stage.
// A turntable carries the players round, a signal warns before it turns, a
// trolley runs over the court and trains pass in the background
// (CStage17Base, a DOL class, does the work; this class hands its hooks on
// and gives it the tuning and the trolley class). The net
// (CStageNetVolley17) changes its motion while the trolley passes under
// it. The volley trolley (msw::CStgGmk_Trolley_Volley) is all inline, so
// its vtable and weak functions are emitted here too.
// The trolley class is declared first: vtables come out in .data in reverse
// order of class declaration, and the original has this class's first.
// Written from the hockey version (game/hockey/CStageHockey17.cpp).
#include "game/msw/CStgGmk_Trolley_Volley.hpp"
#include "game/msw/CStageVolley17.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/VolleyPrm.hpp"
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
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_INIT_STOP_TIME, 0x00 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_STOP_TIME, 0x04 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_STOP_TIME_RND, 0x08 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_ROT_TIME, 0x0C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_ROT_TIME_RND, 0x10 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_SPEED, 0x14 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TT_DIR, 0x18 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_ACC_TIME, 0x1C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_RADIUS, 0x20 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_OPEN_RATE, 0x24 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_WARN_TIME, 0x28 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_WARN_AFTER_TIME, 0x2C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TT_WARN_SE_STOP_TIME, 0x30 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_INIT_RATE, 0x34 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_RATE, 0x38 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_PRE_WAIT, 0x3C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_SPEED, 0x40 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TROLLEY_DIR_TYPE, 0x44 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TROLLEY_NUM_MIN, 0x48 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TROLLEY_NUM_MAX, 0x4C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_LOC_OFFSET, 0x50 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_WIDTH, 0x54 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_DEPTH, 0x58 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_HEIGHT, 0x5C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_ATK_WIDTH, 0x60 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_ATK_DEPTH, 0x64 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_ATK_HEIGHT, 0x68 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_MDL_WIDTH, 0x6C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_MDL_DEPTH, 0x70 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_MDL_HEIGHT, 0x74 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_DEF_WIDTH, 0x78 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT0, 0x7C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT1, 0x80 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_TROLLEY_DEF_HEIGHT2, 0x84 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TROLLEY_PAT_TYPE, 0x88 },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_TROLLEY_PAT_NO, 0x8C },
        { S17_PRM_TYPE_LONG, &VOLLEY_PRM_S17_DEB_RELOAD, 0x90 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_HEYHO_COIN_RATE, 0x94 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_HEYHO_B_DIST_MIN, 0x98 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_HEYHO_B_DIST_MAX, 0x9C },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_HEYHO_C_DIST_MIN, 0xA0 },
        { S17_PRM_TYPE_FLOAT, &VOLLEY_PRM_S17_HEYHO_C_DIST_MAX, 0xA4 },
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
// starts a looping sound unless it is already playing; events 1 to 5 play
// another.
enum {
    SE_SLOT_LOOP = 2,
    SE_SLOT_EVENT = 3,
    SE_LOOP = 0xD9A,
    SE_EVENT = 0xD9B
};

// The effect of both effect types.
static const long EFFECT_ID = 0x6C7;

// The net's motions: the idle motions (0, or 1 while the trolley passes)
// and, from 2 on, the motion that starts each of them (names guessed).
static const long NET_MOT_CHANGE = 2;

const void *msw::CStageVolley17::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley17::CStageVolley17(bcs::CTask *pParent) : CStageManVolley(pParent), CStage17Base(), m_bRotReady(0)
{
}

msw::CStageVolley17::~CStageVolley17()
{
}

void msw::CStageVolley17::Main_(float frame)
{
    CStageManVolley::Main_(frame);
    Stage17_Main(frame);
}

void msw::CStageVolley17::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManVolley::SendMessage(msg, arg0, arg1, arg2);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley17::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_02$", MDL_TURNTABLE, 0, 0, NewMdl },
        { "^s[0-9]+_goal_01$", MDL_SIGNAL_0, 0, 0, NewMdl },
        { "^s[0-9]+_goal_02$", MDL_SIGNAL_1, 0, 0, NewMdl },
        { "^s[0-9]+_ef_steam.*", MDL_STEAM, 0, 0, NewMdl },
        { "^s[0-9]+_train04$", MDL_TRAIN, 0, 0, NewMdl },
        { "^s[0-9]+_train05$", MDL_TRAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageVolley17::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                          nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_TURNTABLE) {
        CTurnTable *pTable = GetTurnTable(0);
        if (pTable != 0) {
            pTable->Assign(0, pObj);
        }
    }
}

void msw::CStageVolley17::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
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
}

void msw::CStageVolley17::OnSceneEntry()
{
    Stage17_OnSceneEntry();
}

bool msw::CStageVolley17::Stage17_IsGimmickReady() const
{
    if (IsGimmickRunning() && IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 8)) {
        return true;
    }
    return false;
}

int msw::CStageVolley17::IsRotReady_() const
{
    return m_bRotReady;
}

void msw::CStageVolley17::OnGimmickReset()
{
    Stage17_OnGimmickReset();
}

void msw::CStageVolley17::OnGimmickStart()
{
    m_bRotReady = 1;
    Stage17_OnGimmickStart();
}

void msw::CStageVolley17::OnGimmickStop()
{
    m_bRotReady = 0;
    Stage17_OnGimmickStop();
}

void msw::CStageVolley17::OnGimmickCancel()
{
    Stage17_OnGimmickCancel();
}

void msw::CStageVolley17::OnGimmickPause()
{
    Stage17_OnGimmickPause();
}

void msw::CStageVolley17::OnGimmickResume()
{
    Stage17_OnGimmickResume();
}

void msw::CStageVolley17::OnGameStart()
{
    m_bRotReady = 0;
}

void msw::CStageVolley17::OnGameEnd()
{
    m_bRotReady = 0;
}

// Rewind the flagged animations of every model.
void msw::CStageVolley17::OnTurnInit()
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

// Unlike the Hockey and Dodge versions, the turntable may turn during the entrance
// event.
void msw::CStageVolley17::OnEntranceEventStart()
{
    m_bRotReady = 1;
}

long msw::CStageVolley17::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage17_GetFloorAttribute(rPos, type);
}

// The turntable moves what stands on it for flow types 0 to 3 and 6.
void msw::CStageVolley17::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageVolley17::GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

void msw::CStageVolley17::CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageVolley17::CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

long msw::CStageVolley17::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
        case 1:
            return EFFECT_ID;
    }
    return -1;
}

void msw::CStageVolley17::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();
}

void msw::CStageVolley17::OnArtsShootStart()
{
}

void msw::CStageVolley17::OnArtsShootEnd()
{
}

msw::CStageNet *msw::CStageVolley17::NewNet(bcs::CTask *pParent)
{
    return new CStageNetVolley17(pParent, this);
}

// Whether a trolley's box meets the court's, widened by five units.
int msw::CStageVolley17::IsTrolleyTransit() const
{
    int bTransit = 0;
    for (long t = 0; t < GetTrolleyMaxNum(); t++) {
        const CStgGmk_Trolley *pTrolley = GetTrolley(t);
        if (pTrolley == 0) {
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

        // The court's box.
        CStageMan::GetInstance(); // result not used
        float left;
        float right;
        float front;
        float back;
        GetCourtRect(&left, &right, &front, &back);

        nw4r::math::AABB trolleyBox(reinterpret_cast<const nw4r::math::VEC3 &>(trolleyMin),
                                    reinterpret_cast<const nw4r::math::VEC3 &>(trolleyMax));
        nw4r::math::VEC3 courtMax(right + 5.0f, 1.0f, back + 5.0f);
        nw4r::math::VEC3 courtMin(left - 5.0f, -1.0f, front - 5.0f);
        nw4r::math::AABB courtBox(courtMin, courtMax);
        if (nw4r::math::IntersectionAABB(&trolleyBox, &courtBox)) {
            bTransit = 1;
        }
    }
    return bTransit != 0;
}

void msw::CStageVolley17::OnSoundEvent(long event)
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
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

long msw::CStageVolley17::GetTrolleyMaxNum()
{
    return 1;
}

const msw::CStgGmk_Trolley *msw::CStageVolley17::GetTrolley(long idx)
{
    const CStageVolley17 *pStage = static_cast<const CStageVolley17 *>(CStageMan::GetInstance());

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

void msw::CStageVolley17::TrolleyForceReset()
{
    static_cast<CStageVolley17 *>(CStageMan::GetInstance())->TrolleyReset();
}

void msw::CStageVolley17::Disp()
{
    CStageManVolley::Disp();
}

// When the trolley starts or stops passing, the idle net plays the motion
// into the new idle motion.
void msw::CStageVolley17::CStageNetVolley17::Main()
{
    if (m_pStage != 0) {
        int bTransit = m_pStage->IsTrolleyTransit();
        if (m_bTransit != bTransit) {
            m_bTransit = bTransit;
            CStgMdl *pMdl = GetMdl(MDL_NET);
            if (pMdl != 0 && IsIdle()) {
                pMdl->SetAnmEx(GetIdleMotID() + NET_MOT_CHANGE, 1);
            }
        }
    }
    CStageNetVolley::Main();
}

void msw::CStageVolley17::CStageNetVolley17::Reset()
{
    m_bTransit = 0;
    CStageNetVolley::Reset();
}

long msw::CStageVolley17::CStageNetVolley17::GetIdleMotID() const
{
    return m_bTransit != 0;
}

msw::CStgGmk_Trolley *msw::CStageVolley17::NewTrolley()
{
    return new (1) CStgGmk_Trolley_Volley(this);
}

long msw::CStageVolley17::GetTurnTableNum() const
{
    return 1;
}

// The times are given in seconds.
float msw::CStageVolley17::GetParam_TTInitialWait() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_INIT_STOP_TIME;
}

float msw::CStageVolley17::GetParam_TTWait() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_STOP_TIME;
}

float msw::CStageVolley17::GetParam_TTWaitRnd() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_STOP_TIME_RND;
}

float msw::CStageVolley17::GetParam_TTRadius() const
{
    return VOLLEY_PRM_S17_TT_RADIUS;
}

float msw::CStageVolley17::GetParam_TTOffset() const
{
    return 0.0f;
}

float msw::CStageVolley17::GetParam_TTRotTime() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_ROT_TIME;
}

float msw::CStageVolley17::GetParam_TTRotTimeRnd() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_ROT_TIME_RND;
}

float msw::CStageVolley17::GetParam_TTRotAccTime() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_ACC_TIME;
}

// The speed is given in degrees per second.
float msw::CStageVolley17::GetParam_TTRotSpeed() const
{
    return bcs::DEG_TO_RAD(VOLLEY_PRM_S17_TT_SPEED) / 60.0f;
}

long msw::CStageVolley17::GetParam_TTRotDirType() const
{
    return VOLLEY_PRM_S17_TT_DIR;
}

// The rates are given in percent.
float msw::CStageVolley17::GetParam_TTOpenRate() const
{
    return 0.01f * VOLLEY_PRM_S17_TT_OPEN_RATE;
}

// A quarter turn.
float msw::CStageVolley17::GetParam_TTOpenAngle() const
{
    return 1.5707964f;
}

float msw::CStageVolley17::GetParam_SignalTime() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_WARN_TIME;
}

float msw::CStageVolley17::GetParam_SignalAfterTime() const
{
    return 60.0f * VOLLEY_PRM_S17_TT_WARN_AFTER_TIME;
}

float msw::CStageVolley17::GetParam_SignalSEStopTime() const
{
    return VOLLEY_PRM_S17_TT_WARN_SE_STOP_TIME;
}

float msw::CStageVolley17::GetParam_TrolleyPreWait() const
{
    return 60.0f * VOLLEY_PRM_S17_TROLLEY_PRE_WAIT;
}

float msw::CStageVolley17::GetParam_TrolleyRate() const
{
    return 0.01f * VOLLEY_PRM_S17_TROLLEY_RATE;
}

float msw::CStageVolley17::GetParam_TrolleyInitRate() const
{
    return 0.01f * VOLLEY_PRM_S17_TROLLEY_INIT_RATE;
}

long msw::CStageVolley17::GetParam_TrolleyDirType() const
{
    return VOLLEY_PRM_S17_TROLLEY_DIR_TYPE;
}

float msw::CStageVolley17::GetParam_TrolleyLocOffset() const
{
    return VOLLEY_PRM_S17_TROLLEY_LOC_OFFSET;
}
