// msw::CStageBasket09: basket stage 09, the conveyor belt stage. The court
// is one conveyor (CStage09Base, a DOL class, runs it); it carries the
// players towards the goal the team with the ball attacks, or in a fixed
// direction (BASKET_PRM_S09_CV_DIR_0), and a stage message speeds it up.
#include "game/msw/CStageBasket09.hpp"

#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/BasketPrm.hpp"
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
        { S09_PRM_TYPE_FLOAT, &BASKET_PRM_S09_EV_INTERVAL, 0x00 },
        { S09_PRM_TYPE_FLOAT, &BASKET_PRM_S09_CV_ACC_TIME, 0x04 },
        { S09_PRM_TYPE_FLOAT, &BASKET_PRM_S09_CV_PRE_ACC_TIME, 0x08 },
        { S09_PRM_TYPE_FLOAT, &BASKET_PRM_S09_CV_SPEED_0, 0x0C },
        { S09_PRM_TYPE_FLOAT, &BASKET_PRM_S09_CV_ACC_SPEED_0, 0x10 },
        { S09_PRM_TYPE_LONG, &BASKET_PRM_S09_CV_DIR_0, 0x14 },
        { S09_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_CONVEYOR = 3,   // "s<n>_ground_00bsk": the belt
    MDL_GROUND_01 = 0xC // "s<n>_ground_01bsk<k>a" and "...b": drawn late (priority 0xC9)
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
    MSG_CONVEYOR = 0x25,  // arg1 2: speed the belt up; arg2's sign gives the direction
    MSG_GUARD_TEAM = 0x27 // arg1 6: arg2 is the team the belt carries towards its goal
};

const void *msw::CStageBasket09::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket09::CStageBasket09(bcs::CTask *pParent) : CStageManBasket(pParent), CStage09Base()
{
    m_nGuardTeam = -1;
    m_bConveyorStarted = 0;
}

msw::CStageBasket09::~CStageBasket09()
{
}

void msw::CStageBasket09::Main_(float frame)
{
    ConveyorUpdate(frame);
    CStageManBasket::Main_(frame);
}

// While the gimmick runs and no delayed start or stop is pending, the belt
// is turned towards the goal the team with the ball attacks (or the fixed
// direction of BASKET_PRM_S09_CV_DIR_0). It stops whenever the game is
// paused or between plays.
void msw::CStageBasket09::ConveyorUpdate(float frame)
{
    unsigned long flags = CGame::g_GameWork.m_nFlag18;

    if (!(flags & 8) && !(flags & 0x4306) && IsGimmickRunning()) {
        CConveyor *pConveyor = GetConveyor(0);

        if (pConveyor->GetSpeedLevel() == 0 && pConveyor->GetDelayTimer() < 0.0f && !m_bConveyorStarted) {
            CConveyor::FLOW_DIR dir = CConveyor::FLOW_DIR_NONE;

            switch (BASKET_PRM_S09_CV_DIR_0) {
                case 0: {
                    CChrCore *pChr = CChrMan::Util_SearchHaveBallChr(0);
                    if (pChr != 0) {
                        if (pChr->GetTargetGoalIdx() == 0) {
                            dir = CConveyor::FLOW_DIR_2;
                        } else {
                            dir = CConveyor::FLOW_DIR_1;
                        }
                    } else {
                        dir = CConveyor::FLOW_DIR_NONE;
                    }
                    break;
                }
                case 1:
                    dir = CConveyor::FLOW_DIR_NONE;
                    break;
                case 2:
                    dir = CConveyor::FLOW_DIR_1;
                    break;
                case 3:
                    dir = CConveyor::FLOW_DIR_2;
                    break;
            }

            if (m_nGuardTeam >= 0 && dir != CConveyor::FLOW_DIR_NONE) {
                dir = CGame::GetTargetGoalIdx(m_nGuardTeam) == 0 ? CConveyor::FLOW_DIR_2 : CConveyor::FLOW_DIR_1;
            }

            if (dir != pConveyor->GetFlowDir()) {
                if (dir != CConveyor::FLOW_DIR_NONE) {
                    Conveyor_StartAll(dir, -1, -1.0f);
                    m_bConveyorStarted = 1;
                } else {
                    Conveyor_StopAll();
                }
            }
        }
    } else {
        Conveyor_StopAll();
        m_bConveyorStarted = 0;
    }

    if (IsGimmickRunning()) {
        Conveyor_UpdateAll(frame);
    }
}

void msw::CStageBasket09::Conveyor_OnStart(long, CConveyor::FLOW_DIR, int, float)
{
    m_bConveyorStarted = 1;
}

void msw::CStageBasket09::Conveyor_OnStartAll(CConveyor::FLOW_DIR, long, float)
{
    m_bConveyorStarted = 1;
}

void msw::CStageBasket09::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManBasket::SendMessage(msg, arg1, arg2, arg3);

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

                // The acceleration time is given in seconds.
                float accTime = 60.0f * BASKET_PRM_S09_CV_ACC_TIME;
                float preAccTime = BASKET_PRM_S09_CV_PRE_ACC_TIME;

                if (dir != CConveyor::FLOW_DIR_NONE) {
                    if (preAccTime > 0.0f) {
                        Conveyor_StopAll();
                        Conveyor_DelayAccStartAll(preAccTime, dir, accTime);
                    } else {
                        Conveyor_AccStartAll(dir, accTime);
                    }
                }
            }
            break;
        case MSG_GUARD_TEAM:
            if (arg1 == 6) {
                m_nGuardTeam = arg2;
            }
            break;
    }
}

long msw::CStageBasket09::GetQPanelEnabled_() const
{
    return 0xFFFF;
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket09::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_00bsk$", MDL_CONVEYOR, 0, 0, NewMdl },
        { "^s[0-9]+_ground_01bsk[0-9]+a$", MDL_GROUND_01, 0, 0, NewMdl },
        { "^s[0-9]+_ground_01bsk[0-9]+b$", MDL_GROUND_01, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageBasket09::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_CONVEYOR:
            GetConveyor(0)->SetMdlNode(pNode);
            break;
        case MDL_GROUND_01:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
    }

    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasket09::OnPreInitialize()
{
    Stage09_OnPreInitialize();
    Conveyor_SetRelate(CV_RELATE_1);
}

void msw::CStageBasket09::OnSceneEntry()
{
}

void msw::CStageBasket09::OnTurnInit()
{
    Conveyor_ResetAll();
    m_bConveyorStarted = 0;
}

void msw::CStageBasket09::OnGimmickReset()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

void msw::CStageBasket09::OnEventStart()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

long msw::CStageBasket09::GetFloorAttribute(const bcs::FVEC3 &, FLOW_TYPE) const
{
    return 2;
}

// The whole floor flows with the belt for flow types 0 to 3 and 6.
void msw::CStageBasket09::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
            GetAreaFlagByPos(rPos, 3); // result not used
            GetConveyor(0)->GetFlow(rFlow);
            break;
        default:
            rFlow.x = 0.0f;
            rFlow.y = 0.0f;
            rFlow.z = 0.0f;
            break;
    }
}

void msw::CStageBasket09::OnArtsShootStart()
{
    Conveyor_StopAll();
}

void msw::CStageBasket09::OnArtsShootEnd()
{
}

void msw::CStageBasket09::OnSoundEvent(long event)
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
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

// The belt runs faster while its speed level is raised.
float msw::CStageBasket09::CConveyorState::GetParam_CvSpeed() const
{
    if (GetSpeedLevel() > 0) {
        return BASKET_PRM_S09_CV_ACC_SPEED_0;
    }
    return BASKET_PRM_S09_CV_SPEED_0;
}

float msw::CStageBasket09::CConveyorState::GetParam_AnmFlow() const
{
    return 1.0f / 45.0f;
}

float msw::CStageBasket09::CConveyorState::GetParam_MdlAnmDir() const
{
    if (GetIndex() == 0) {
        return -1.0f;
    }
    return 1.0f;
}

long msw::CStageBasket09::GetConveyorNum() const
{
    return 1;
}

msw::CStage09Base::CConveyor *msw::CStageBasket09::NewConveyor() const
{
    return new CConveyorState;
}

msw::CStgSeSlot *msw::CStageBasket09::GetSeSlot()
{
    return this;
}

long msw::CStageBasket09::GetSeSlotBeg() const
{
    return SE_SLOT_CONVEYOR;
}

long msw::CStageBasket09::SE_GetSlotNum() const
{
    return 5;
}
