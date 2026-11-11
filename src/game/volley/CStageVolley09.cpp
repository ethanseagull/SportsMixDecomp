// msw::CStageVolley09: volley stage 09, the conveyor belt stage. The court
// is one conveyor (CStage09Base, a DOL class, runs it); it carries the
// players towards the goal the team with the ball attacks, or in a fixed
// direction (VOLLEY_PRM_S09_CV_DIR_0), and a stage message speeds it up.
#include "game/msw/CStageVolley09.hpp"

#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/VolleyPrm.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Rand.hpp"
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
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_CV_SPEED, 0x00 },
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_CV_ACC_SPEED, 0x04 },
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_CV_STOP_TIME, 0x08 },
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_EV_INTERVAL, 0x0C },
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_CV_ACC_TIME, 0x10 },
        { S09_PRM_TYPE_FLOAT, &VOLLEY_PRM_S09_CV_PRE_ACC_TIME, 0x14 },
        { S09_PRM_TYPE_LONG, &VOLLEY_PRM_S09_CV_FORCE_MOVE, 0x18 },
        { S09_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_CONVEYOR_0 = 3, // "s<n>_ground_00vol01": belt 0
    MDL_CONVEYOR_1 = 4, // "s<n>_ground_00vol00": belt 1
    MDL_GROUND_01 = 0xC // "s<n>_ground_01vol0<k>[a-c]": drawn late (priority 0xC9)
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
    MSG_STOP = 0x54,      // stops every belt
    MSG_CONVEYOR = 0x25,  // arg1 2: start belt GetTargetGoalIdx(arg3); arg2's sign gives the direction
    MSG_STOP_TEAM = 0x27  // arg1 5: arg2 is the team whose target goal's belt is held still
};

const void *msw::CStageVolley09::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley09::CStageVolley09(bcs::CTask *pParent) : CStageManVolley(pParent), CStage09Base()
{
    m_nStopTeam = -1;
}

msw::CStageVolley09::~CStageVolley09()
{
}

void msw::CStageVolley09::Main_(float frame)
{
    ConveyorUpdate(frame);
    CStageManVolley::Main_(frame);
}

// The belts run only while the gimmick does; VolleyF does not steer them.
void msw::CStageVolley09::ConveyorUpdate(float frame)
{
    if (IsGimmickRunning()) {
        Conveyor_UpdateAll(frame);
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley09::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_01vol00[a-c]", MDL_GROUND_01, 0, 0, NewMdl },
        { "^s[0-9]+_ground_01vol01[a-c]", MDL_GROUND_01, 0, 0, NewMdl },
        { "^s[0-9]+_ground_00vol00$", MDL_CONVEYOR_1, 0, 0, NewMdl },
        { "^s[0-9]+_ground_00vol01$", MDL_CONVEYOR_0, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageVolley09::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_CONVEYOR_0:
            GetConveyor(0)->SetMdlNode(pNode);
            break;
        case MDL_CONVEYOR_1:
            GetConveyor(1)->SetMdlNode(pNode);
            break;
        case MDL_GROUND_01:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
    }
}

void msw::CStageVolley09::OnPreInitialize()
{
    Stage09_OnPreInitialize();
}

void msw::CStageVolley09::OnSceneEntry()
{
}

// Each turn the belts start again after the stop time, in a random
// direction.
void msw::CStageVolley09::OnTurnInit()
{
    Conveyor_ResetAll();
    Conveyor_DelayStartAll(60.0f * VOLLEY_PRM_S09_CV_STOP_TIME,
                           bcs::BCS_RandRange(0L, 1L) != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, -1,
                           -1.0f);
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

// The belt of the goal m_nStopTeam attacks is held still.
void msw::CStageVolley09::OnGimmickReset()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);

    long num = GetConveyorNum();
    for (long i = 0; i < num; i++) {
        CConveyor *pConveyor = GetConveyor(i);
        if (pConveyor != 0) {
            if (m_nStopTeam >= 0 && CGame::GetTargetGoalIdx(i) == m_nStopTeam) {
                pConveyor->SetForceStop(1);
            } else {
                pConveyor->SetForceStop(0);
            }
        }
    }
}

long msw::CStageVolley09::GetConveyorNum() const
{
    return 2;
}

void msw::CStageVolley09::OnGimmickStart()
{
}

void msw::CStageVolley09::OnGimmickStop()
{
    Conveyor_StopAll();
}

void msw::CStageVolley09::OnGimmickCancel()
{
}

void msw::CStageVolley09::OnGimmickPause()
{
}

void msw::CStageVolley09::OnGimmickResume()
{
}

void msw::CStageVolley09::OnEventStart()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

long msw::CStageVolley09::GetFloorAttribute(const bcs::FVEC3 &, FLOW_TYPE) const
{
    return 2;
}

// Each half of the court flows with its own belt (area flags 1 and 2; flow
// type 1 also counts 0x1000 and 0x2000) for flow types 0 to 3 and 6.
void msw::CStageVolley09::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            unsigned long area = GetAreaFlagByPos(rPos, 0x3003);
            long no;
            if (type == 1) {
                if (area & 0x1001) {
                    no = 0;
                } else {
                    no = (area & 0x2002) ? 1 : -1;
                }
            } else {
                if (area & 1) {
                    no = 0;
                } else if (area & 2) {
                    no = 1;
                } else {
                    no = -1;
                }
            }

            if (no < 0) {
                rFlow.x = 0.0f;
                rFlow.y = 0.0f;
                rFlow.z = 0.0f;
            } else {
                GetConveyor(no)->GetFlow(rFlow);
            }
            break;
        }
        default:
            rFlow.x = 0.0f;
            rFlow.y = 0.0f;
            rFlow.z = 0.0f;
            break;
    }
}

void msw::CStageVolley09::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManVolley::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case MSG_STOP:
            Conveyor_StopAll();
            break;
        case MSG_CONVEYOR:
            if (arg1 == 2) {
                CConveyor::FLOW_DIR dir;
                if (arg2 == 0) {
                    dir = CConveyor::FLOW_DIR_NONE;
                } else if (arg2 > 0) {
                    dir = CConveyor::FLOW_DIR_3;
                } else {
                    dir = CConveyor::FLOW_DIR_4;
                }

                // The acceleration time is given in seconds.
                float accTime = 60.0f * VOLLEY_PRM_S09_CV_ACC_TIME;
                float preAccTime = VOLLEY_PRM_S09_CV_PRE_ACC_TIME;
                long no = CGame::GetTargetGoalIdx(arg3);

                if (dir != CConveyor::FLOW_DIR_NONE) {
                    if (preAccTime > 0.0f) {
                        Conveyor_Stop(no);
                        Conveyor_DelayStart(preAccTime, no, dir, 1, accTime);
                    } else {
                        Conveyor_Start(no, dir, 1, accTime);
                    }
                } else {
                    // Stopped, then restarted the other way or this one
                    // after the acceleration time.
                    Conveyor_Stop(no);
                    Conveyor_DelayStart(accTime, no,
                                        bcs::BCS_RandRange(0L, 1L) != 0 ? CConveyor::FLOW_DIR_4
                                                                        : CConveyor::FLOW_DIR_3,
                                        0, -1.0f);
                }
            }
            break;
        case MSG_STOP_TEAM:
            if (arg1 == 5) {
                m_nStopTeam = arg2;
            }
            break;
    }
}

void msw::CStageVolley09::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
                SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT1)) {
                SE_RepeatPlay(SE_SLOT_AMBIENT1, SE_AMBIENT1, 180.0f, 420.0f, -1.0f, 0.0f, 12);
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
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

// The belt runs faster while its speed level is raised.
float msw::CStageVolley09::CConveyorState::GetParam_CvSpeed() const
{
    if (GetSpeedLevel() > 0) {
        return VOLLEY_PRM_S09_CV_ACC_SPEED;
    }
    return VOLLEY_PRM_S09_CV_SPEED;
}

float msw::CStageVolley09::CConveyorState::GetParam_AnmFlow() const
{
    return 1.0f / 45.0f;
}

float msw::CStageVolley09::CConveyorState::GetParam_MdlAnmDir() const
{
    return -1.0f;
}

msw::CStage09Base::CConveyor *msw::CStageVolley09::NewConveyor() const
{
    return new CConveyorState;
}

msw::CStgSeSlot *msw::CStageVolley09::GetSeSlot()
{
    return this;
}

long msw::CStageVolley09::GetSeSlotBeg() const
{
    return SE_SLOT_CONVEYOR;
}

long msw::CStageVolley09::SE_GetSlotNum() const
{
    return 5;
}
