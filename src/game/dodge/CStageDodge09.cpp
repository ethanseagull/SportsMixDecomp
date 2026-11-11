// msw::CStageDodge09: dodge stage 09, the conveyor belt stage. The court
// is one conveyor (CStage09Base, a DOL class, runs it); it is started at
// random between turns, and a stage message speeds it up.
#include "game/msw/CStageDodge09.hpp"

#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/BCS_Rand.hpp"
#include "game/msw/DodgePrm.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdlListNode.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S09_PRM_TYPE {
        S09_PRM_TYPE_END,
        S09_PRM_TYPE_LONG,
        S09_PRM_TYPE_FLOAT
    };

    struct S09_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S09_PRM_TBL s_PrmTbl[] = {
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_SPEED, 0x00 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_ACC_SPEED, 0x04 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_JUMP_ACC_FOLLOW, 0x08 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_JUMP_ACC_AGAINST, 0x0C },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_EV_INTERVAL, 0x10 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_EV_TIME, 0x14 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_ACC_TIME, 0x18 },
        { S09_PRM_TYPE_FLOAT, &DODGE_PRM_S09_CV_PRE_ACC_TIME, 0x1C },
        { S09_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_CONVEYOR_0 = 3, // "cvmotion", "cvgear" and "ground_00b": the first belt
    MDL_CONVEYOR_1 = 4, // "ground_00a": the second belt
    MDL_ANM_NODE = 5,   // "ground_00c": its animation follows the belt's speed
    MDL_GROUND_01 = 0xC // "ground_01c": drawn late (priority 0xC9)
};

// Sound slots and sounds of the stage. Slot 4 is the conveyor's
// (GetSeSlotBeg); SE_GetSlotNum gives the five.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT1 = 1,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_SLOT_CONVEYOR = 4,
    SE_AMBIENT0 = 0xD49,
    SE_AMBIENT1 = 0xD4D,
    SE_AMBIENT2 = 0xD51,
    SE_EVENT = 0xD52
};

// Stage messages handled here (sent through CStageMan::SendMessage).
enum {
    MSG_CONVEYOR = 0x25,  // arg1 2: start the belt; arg2's sign gives the direction
    MSG_GUARD_TEAM = 0x27 // arg1 5: arg2 is the guard team
};

const void *msw::CStageDodge09::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge09::CStageDodge09(bcs::CTask *pParent) : CStageManDodge(pParent), CStage09Base()
{
    m_bActive = 0;
    m_pMdlNode = 0;
    m_nGuardTeam = -1;
}

msw::CStageDodge09::~CStageDodge09()
{
}

void msw::CStageDodge09::Main_(float frame)
{
    ConveyorUpdate(frame);
    CStageManDodge::Main_(frame);
}

// Runs the conveyors while the gimmick runs, and keeps the model's
// animation at the belt's speed.
void msw::CStageDodge09::ConveyorUpdate(float frame)
{
    bool bRunning = false;

    if (m_bActive && IsGimmickRunning()) {
        bRunning = true;
    }

    if (bRunning) {
        Conveyor_UpdateAll(frame);
    }

    if (m_pMdlNode != 0) {
        CStageNodeAnm *pAnm = m_pMdlNode->GetAnm();

        if (pAnm != 0) {
            float speed = 0.0f;

            if (bRunning && IsConveyorParallel()) {
                speed = GetConveyor(0)->GetCurSpeed();
            }
            pAnm->SetRate(-1, speed / (1.0f / 45.0f));
        }
    }
}

bool msw::CStageDodge09::IsConveyorParallel() const
{
    if (m_nGuardTeam >= 0) {
        return false;
    }
    return Conveyor_GetRelate() == CV_RELATE_1;
}

void msw::CStageDodge09::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManDodge::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case MSG_CONVEYOR:
            if (arg1 == 2) {
                CConveyor::FLOW_DIR dir;
                if (arg2 == 0) {
                    dir = CConveyor::FLOW_DIR_NONE;
                } else if (arg2 > 0) {
                    dir = CConveyor::FLOW_DIR_2;
                } else {
                    dir = CConveyor::FLOW_DIR_1;
                }

                // The times are given in seconds.
                float accTime = 60.0f * DODGE_PRM_S09_CV_ACC_TIME;
                float preAccTime = DODGE_PRM_S09_CV_PRE_ACC_TIME;
                long team = CGame::GetTargetGoalIdx(arg3);

                if (dir != CConveyor::FLOW_DIR_NONE) {
                    if (preAccTime > 0.0f) {
                        Conveyor_StopAll();
                        Conveyor_DelayStartAll(preAccTime, dir, team, accTime);
                    } else {
                        Conveyor_StartAll(dir, team, accTime);
                    }
                } else {
                    Conveyor_StopAll();
                    Conveyor_DelayStartAll(accTime,
                                           bcs::BCS_RandRange(0L, 1L) != 0 ? CConveyor::FLOW_DIR_1 : CConveyor::FLOW_DIR_2,
                                           -1, -1.0f);
                }
            }
            break;
        case MSG_GUARD_TEAM:
            if (arg1 == 5) {
                m_nGuardTeam = arg2;
            }
            break;
    }
}

long msw::CStageDodge09::GetQPanelEnabled_() const
{
    long flags = 0xF00;

    if (IsConveyorParallel()) {
        flags |= 0xF;
    }
    return flags & CStageManDodge::GetQPanelEnabled_();
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge09::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_cvmotion", MDL_CONVEYOR_0, 0, 0, NewMdl },
        { "^s[0-9]+_cvgear", MDL_CONVEYOR_0, 0, 0, NewMdl },
        { "^s[0-9]+_ground_01c", MDL_GROUND_01, 0, 0, NewMdl },
        { "^s[0-9]+_ground_00b$", MDL_CONVEYOR_0, 0, 0, NewMdl },
        { "^s[0-9]+_ground_00a$", MDL_CONVEYOR_1, 0, 0, NewMdl },
        { "^s[0-9]+_ground_00c$", MDL_ANM_NODE, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge09::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long)
{
    switch (pRule->m_nId) {
        case MDL_CONVEYOR_0:
            GetConveyor(0)->SetMdlNode(pNode);
            break;
        case MDL_CONVEYOR_1:
            GetConveyor(1)->SetMdlNode(pNode);
            break;
        case MDL_ANM_NODE:
            m_pMdlNode = pNode;
            break;
        case MDL_GROUND_01:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
    }
}

void msw::CStageDodge09::OnPreInitialize()
{
    Stage09_OnPreInitialize();
    Conveyor_SetRelate(CV_RELATE_1);
}

void msw::CStageDodge09::OnSceneEntry()
{
}

void msw::CStageDodge09::OnPeriodInit()
{
    m_bActive = 1;
    Conveyor_ResetAll();
}

void msw::CStageDodge09::OnTurnInit()
{
    Conveyor_ResetAll();
    CConveyor::FLOW_DIR dir = bcs::BCS_RandRange(0L, 1L) != 0 ? CConveyor::FLOW_DIR_1 : CConveyor::FLOW_DIR_2;
    Conveyor_DelayStartAll(60.0f * DODGE_PRM_S09_EV_TIME, dir, -1, -1.0f);
}

void msw::CStageDodge09::OnGimmickReset()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);

    long num = GetConveyorNum();
    for (long i = 0; i < num; i++) {
        CConveyor *pConveyor = GetConveyor(i);

        if (pConveyor != 0) {
            if (m_nGuardTeam >= 0 && m_nGuardTeam == CGame::GetTargetGoalIdx(i)) {
                pConveyor->SetForceStop(1);
            } else {
                pConveyor->SetForceStop(0);
            }
        }
    }
}

void msw::CStageDodge09::OnGimmickStop()
{
    Conveyor_StopAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

void msw::CStageDodge09::OnEventStart()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

long msw::CStageDodge09::GetFloorAttribute(const bcs::FVEC3 &, FLOW_TYPE type) const
{
    return type == 5 ? 0 : 2;
}

// The belt under rPos is found from the area flags (-1: none). Flow type 1
// also counts the flags 0x201 and 0x102.
void msw::CStageDodge09::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    if ((unsigned long)type <= 4 || type == 6) {
        unsigned long flag = GetAreaFlagByPos(rPos, 0x303);
        long idx;

        if (type == 1) {
            if (flag & 0x201) {
                idx = 0;
            } else {
                idx = -1;
                if (flag & 0x102) {
                    idx = 1;
                }
            }
        } else if (flag & 1) {
            idx = 0;
        } else {
            idx = -1;
            if (flag & 2) {
                idx = 1;
            }
        }

        if (idx < 0) {
            rFlow.x = 0.0f;
            rFlow.y = 0.0f;
            rFlow.z = 0.0f;
        } else {
            GetConveyor(idx)->GetFlow(rFlow);
        }
    } else {
        rFlow.x = 0.0f;
        rFlow.y = 0.0f;
        rFlow.z = 0.0f;
    }
}

float msw::CStageDodge09::GetFloorFlowRate(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    float rate = 1.0f;

    if (type == 4) {
        unsigned long flag = GetAreaFlagByPos(rPos, 3);
        long idx;

        if (flag & 1) {
            idx = 0;
        } else {
            idx = -1;
            if (flag & 2) {
                idx = 1;
            }
        }

        if (idx < 0) {
            rate = 0.0f;
        } else {
            rate = GetConveyor(idx)->GetFlowRate();
        }
    }
    return rate;
}

void msw::CStageDodge09::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
                SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT1)) {
                SE_Play(SE_SLOT_AMBIENT1, SE_AMBIENT1, 0.0f, 3);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
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

// The belt runs faster while its speed level is raised.
float msw::CStageDodge09::CConveyorState::GetParam_CvSpeed() const
{
    if (GetSpeedLevel() > 0) {
        return DODGE_PRM_S09_CV_ACC_SPEED;
    }
    return DODGE_PRM_S09_CV_SPEED;
}

float msw::CStageDodge09::CConveyorState::GetParam_JumpAccFollow() const
{
    return DODGE_PRM_S09_CV_JUMP_ACC_FOLLOW;
}

float msw::CStageDodge09::CConveyorState::GetParam_JumpAccAgainst() const
{
    return DODGE_PRM_S09_CV_JUMP_ACC_AGAINST;
}

float msw::CStageDodge09::CConveyorState::GetParam_AnmFlow() const
{
    return 1.0f / 45.0f;
}

float msw::CStageDodge09::CConveyorState::GetParam_MdlAnmDir() const
{
    if (GetIndex() == 0) {
        return -1.0f;
    }
    return 1.0f;
}

long msw::CStageDodge09::GetConveyorNum() const
{
    return 2;
}

msw::CStage09Base::CConveyor *msw::CStageDodge09::NewConveyor() const
{
    return new CConveyorState;
}

msw::CStgSeSlot *msw::CStageDodge09::GetSeSlot()
{
    return this;
}

long msw::CStageDodge09::GetSeSlotBeg() const
{
    return SE_SLOT_CONVEYOR;
}

long msw::CStageDodge09::SE_GetSlotNum() const
{
    return 5;
}
