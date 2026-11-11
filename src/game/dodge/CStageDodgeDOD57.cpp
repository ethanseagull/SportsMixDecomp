// msw::CStageDodgeDOD57: Dodge or Die stage 57, the turntable stage of the
// Dodge or Die party game. Two turntables carry the players round and a
// signal warns before they turn, as in stage 17 (CStage17Base, a DOL class,
// does the work; this class hands its hooks on and gives it the tuning),
// but there is no trolley and no effect.
#include "game/msw/CStageDodgeDOD57.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/DodgePrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S57_PRM_TYPE {
        S57_PRM_TYPE_END,
        S57_PRM_TYPE_LONG,
        S57_PRM_TYPE_FLOAT
    };

    struct S57_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S57_PRM_TBL s_PrmTbl[] = {
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_INIT_STOP_TIME, 0x00 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_STOP_TIME, 0x04 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_STOP_TIME_RND, 0x08 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_ROT_TIME, 0x0C },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_ROT_TIME_RND, 0x10 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_SPEED, 0x14 },
        { S57_PRM_TYPE_LONG, &DODGE_PRM_S57_TT_DIR, 0x18 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_ACC_TIME, 0x1C },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_RADIUS, 0x20 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_OPEN_RATE, 0x24 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_WARN_TIME, 0x28 },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_WARN_AFTER_TIME, 0x2C },
        { S57_PRM_TYPE_FLOAT, &DODGE_PRM_S57_TT_WARN_SE_STOP_TIME, 0x30 },
        { S57_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table (names guessed; the same
// as stage 17's).
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

// Sound slot and sounds of the stage (names guessed). Sound event 0
// starts a looping sound unless it is already playing.
enum {
    SE_SLOT_LOOP = 2,
    SE_LOOP = 0xDA6
};

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

const void *msw::CStageDodgeDOD57::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodgeDOD57::CStageDodgeDOD57(bcs::CTask *pParent)
    : CStageManDodgeDodgeOrDie(pParent), CStage17Base(), m_bRotReady(0)
{
}

msw::CStageDodgeDOD57::~CStageDodgeDOD57()
{
}

void msw::CStageDodgeDOD57::Main_(float frame)
{
    CStageManDodgeDodgeOrDie::Main_(frame);
    Stage17_Main(frame);
}

void msw::CStageDodgeDOD57::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManDodgeDodgeOrDie::SendMessage(msg, arg0, arg1, arg2);

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

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodgeDOD57::GetMdlGenRule_() const
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
        { 0, reinterpret_cast<long>(CStageManDodgeDodgeOrDie::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodgeDOD57::OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                            bcs::CScnObj *pObj, nw4r::g3d::ResFile file, CArn &rArn, long n)
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

void msw::CStageDodgeDOD57::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                             CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                             long n)
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

void msw::CStageDodgeDOD57::OnSceneEntry()
{
    Stage17_OnSceneEntry();
}

bool msw::CStageDodgeDOD57::Stage17_IsGimmickReady() const
{
    if (IsGimmickRunning() && IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 8)) {
        return true;
    }
    return false;
}

int msw::CStageDodgeDOD57::IsRotReady_() const
{
    return m_bRotReady;
}

void msw::CStageDodgeDOD57::OnGimmickReset()
{
    Stage17_OnGimmickReset();
}

void msw::CStageDodgeDOD57::OnGimmickStart()
{
    m_bRotReady = 1;
    Stage17_OnGimmickStart();
}

void msw::CStageDodgeDOD57::OnGimmickStop()
{
    m_bRotReady = 0;
    Stage17_OnGimmickStop();
}

void msw::CStageDodgeDOD57::OnGimmickCancel()
{
    Stage17_OnGimmickCancel();
}

void msw::CStageDodgeDOD57::OnGimmickPause()
{
    Stage17_OnGimmickPause();
}

void msw::CStageDodgeDOD57::OnGimmickResume()
{
    Stage17_OnGimmickResume();
}

void msw::CStageDodgeDOD57::OnGameStart()
{
    m_bRotReady = 0;
}

void msw::CStageDodgeDOD57::OnGameEnd()
{
    m_bRotReady = 0;
}

void msw::CStageDodgeDOD57::OnTurnInit()
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
}

void msw::CStageDodgeDOD57::OnEntranceEventStart()
{
    m_bRotReady = 0;
}

// The floor attribute does not depend on the flow type here.
long msw::CStageDodgeDOD57::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage17_GetFloorAttribute(rPos, FLOW_TYPE_0);
}

// The turntables move what stands on them for flow types 0 to 3 and 6;
// for types 0 and 6 only inside a court half.
void msw::CStageDodgeDOD57::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageDodgeDOD57::GetFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

void msw::CStageDodgeDOD57::CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

float msw::CStageDodgeDOD57::CalcFloorFlowRot(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
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

void msw::CStageDodgeDOD57::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_LOOP)) {
            SE_Play(SE_SLOT_LOOP, SE_LOOP, 0.0f, 3);
        }
    }
}

// No effects.
long msw::CStageDodgeDOD57::GetParam_Effect(long type) const
{
    return -1;
}

// The turntable and signal sounds (names of the types unknown).
long msw::CStageDodgeDOD57::GetParam_SE(long type) const
{
    switch (type) {
        case 1:
            return 0xDA1;
        case 2:
            return 0xDA2;
        case 3:
            return 0xDA5;
        case 4:
            return 0xDA4;
    }
    return -1;
}

void msw::CStageDodgeDOD57::Disp()
{
    CStageManDodgeDodgeOrDie::Disp();
}

// No trolley.
msw::CStgGmk_Trolley *msw::CStageDodgeDOD57::NewTrolley()
{
    return 0;
}

long msw::CStageDodgeDOD57::GetTurnTableNum() const
{
    return 2;
}

// The times are given in seconds.
float msw::CStageDodgeDOD57::GetParam_TTInitialWait() const
{
    return 60.0f * DODGE_PRM_S57_TT_INIT_STOP_TIME;
}

float msw::CStageDodgeDOD57::GetParam_TTWait() const
{
    return 60.0f * DODGE_PRM_S57_TT_STOP_TIME;
}

float msw::CStageDodgeDOD57::GetParam_TTWaitRnd() const
{
    return 60.0f * DODGE_PRM_S57_TT_STOP_TIME_RND;
}

float msw::CStageDodgeDOD57::GetParam_TTRadius() const
{
    return DODGE_PRM_S57_TT_RADIUS;
}

// The tables sit as far from the centre as their radius.
float msw::CStageDodgeDOD57::GetParam_TTOffset() const
{
    return DODGE_PRM_S57_TT_RADIUS;
}

float msw::CStageDodgeDOD57::GetParam_TTRotTime() const
{
    return 60.0f * DODGE_PRM_S57_TT_ROT_TIME;
}

float msw::CStageDodgeDOD57::GetParam_TTRotTimeRnd() const
{
    return 60.0f * DODGE_PRM_S57_TT_ROT_TIME_RND;
}

float msw::CStageDodgeDOD57::GetParam_TTRotAccTime() const
{
    return 60.0f * DODGE_PRM_S57_TT_ACC_TIME;
}

// The speed is given in degrees per second.
float msw::CStageDodgeDOD57::GetParam_TTRotSpeed() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S57_TT_SPEED) / 60.0f;
}

long msw::CStageDodgeDOD57::GetParam_TTRotDirType() const
{
    return DODGE_PRM_S57_TT_DIR;
}

// The rate is given in percent.
float msw::CStageDodgeDOD57::GetParam_TTOpenRate() const
{
    return 0.01f * DODGE_PRM_S57_TT_OPEN_RATE;
}

float msw::CStageDodgeDOD57::GetParam_TTOpenAngle() const
{
    return 0.0f;
}

float msw::CStageDodgeDOD57::GetParam_SignalTime() const
{
    return 60.0f * DODGE_PRM_S57_TT_WARN_TIME;
}

float msw::CStageDodgeDOD57::GetParam_SignalAfterTime() const
{
    return 60.0f * DODGE_PRM_S57_TT_WARN_AFTER_TIME;
}

float msw::CStageDodgeDOD57::GetParam_SignalSEStopTime() const
{
    return DODGE_PRM_S57_TT_WARN_SE_STOP_TIME;
}

float msw::CStageDodgeDOD57::GetParam_TrolleyRate() const
{
    return 0.0f;
}

float msw::CStageDodgeDOD57::GetParam_TrolleyInitRate() const
{
    return 0.0f;
}
