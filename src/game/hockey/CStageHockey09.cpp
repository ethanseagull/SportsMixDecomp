// msw::CStageHockey09: hockey stage 09, the conveyor belt stage. Two belts
// run along the back and front ends of the court (CStage09Base, a DOL class,
// runs them). After each stop a belt starts again in a random direction;
// while it runs it carries the containers this class drops on it
// (msw::CStgGmk_Container_Hockey: plain, bomb or treasure), and a treasure
// container keeps both belts running a while longer. The containers are all
// inline, so their vtables and weak functions are emitted here too.
// The container header comes first: vtables come out in .data in reverse
// order of class declaration, and the original has this class's first.
#include "game/msw/CStgGmk_Container_Hockey.hpp"
#include "game/msw/CStageHockey09.hpp"

#include <math.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgGmkMan.hpp"
#include "game/msw/HockeyPrm.hpp"

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
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_EV_INTERVAL, 0x00 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_EV_INTERVAL_RND, 0x04 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_STOP_TIME, 0x08 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_ACC_TIME, 0x0C },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_PRE_ACC_TIME, 0x10 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_POS, 0x14 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_WIDTH, 0x18 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_SPEED_0, 0x1C },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CV_ACC_SPEED_0, 0x20 },
        { S09_PRM_TYPE_LONG, &HOCKEY_PRM_S09_CV_DIR_0, 0x24 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_OBJ_INTERVAL, 0x28 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_OBJ_BOMB_RATE, 0x2C },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_TREASURE_INTERVAL, 0x30 },
        { S09_PRM_TYPE_LONG, &HOCKEY_PRM_S09_TREASURE_NUM, 0x34 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_TREASURE_RUN_TIME, 0x38 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CONTAINER_WIDTH, 0x3C },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_CONTAINER_HEIGHT, 0x40 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_BOMB_SIZE, 0x44 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_BOMB_EXPLODE_SIZE, 0x48 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_BOMB_EXPLODE_EFF_SCALE, 0x4C },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_BOMB_EXPLODE_TIME, 0x50 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_TREASURE_WIDTH, 0x54 },
        { S09_PRM_TYPE_FLOAT, &HOCKEY_PRM_S09_TREASURE_HEIGHT, 0x58 },
        { S09_PRM_TYPE_LONG, &HOCKEY_PRM_S09_TREASURE_COIN_NUM, 0x5C },
        { S09_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table (names guessed).
enum {
    MDL_BELT_B = 100,   // "s<n>_ground_02_b01hck": conveyor 0's belt
    MDL_BELT_B_2 = 101, // "s<n>_ground_02_b00hck": its second model
    MDL_BELT_A = 110,   // "s<n>_ground_02_a01hck": conveyor 1's belt
    MDL_BELT_A_2 = 111, // "s<n>_ground_02_a00hck": its second model
    MDL_LATE = 20       // "s<n>_ground_01hck..." and "s<n>_ground_05...": drawn late (priority 0xC9)
};

// Kinds of container (the type given to its constructor).
enum {
    CONTAINER_NORMAL,
    CONTAINER_BOMB,
    CONTAINER_TREASURE
};

// The gimmick type of the containers (CStgGmk_Container sets it).
static const int GMK_TYPE_CONTAINER = 6;

// Sound slots and sounds of the stage. Slot 4 is the conveyors'
// (GetSeSlotBeg); SE_GetSlotNum gives the five.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT1 = 1,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_SLOT_CONVEYOR = 4,
    SE_AMBIENT0 = 0xD49,
    SE_TREASURE = 0xD4C,
    SE_AMBIENT1 = 0xD4D,
    SE_AMBIENT2 = 0xD51,
    SE_EVENT = 0xD52
};

// Stage message handled here (sent through CStageMan::SendMessage): arg1 2
// speeds the belts up (arg2's sign gives the direction) or, with arg2 0,
// stops them for a while.
static const long MSG_CONVEYOR = 0x25;


const void *msw::CStageHockey09::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey09::CStageHockey09(bcs::CTask *pParent) : CStageManHockey(pParent), CStage09Base()
{
    m_anTreasureNum[0] = 0;
    m_anTreasureNum[1] = 0;
}

msw::CStageHockey09::~CStageHockey09()
{
}

void msw::CStageHockey09::Main_(float frame)
{
    ConveyorUpdate(frame);
    CStageManHockey::Main_(frame);
}

void msw::CStageHockey09::ConveyorUpdate(float frame)
{
    if (IsGimmickRunning()) {
        Conveyor_UpdateAll(frame);
    }

    // Place each belt's model at its end of the court, scaled to the belt's
    // width (the model is 17 units wide).
    long num = GetTblSize();
    for (long i = 0; i < num; i++) {
        CConveyor *pConveyor = GetConveyor(i);
        if (pConveyor == 0) {
            continue;
        }

        CStageMdlListNode *pNode = pConveyor->GetMdlNode();
        bcs::CScnObj *pObj = pNode != 0 ? pNode->GetObj() : 0;
        if (pObj != 0) {
            float front;
            float back;
            GetCourtRect(0, 0, &front, &back);

            bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
            float offset = HOCKEY_PRM_S09_CV_POS;
            float width = HOCKEY_PRM_S09_CV_WIDTH;
            pos.z = i == 0 ? back - offset : front + offset;
            float scaleZ = width / 17.0f;
            pObj->m_vTrans = pos;
            pObj->m_vScale.x = 1.0f;
            pObj->m_vScale.y = 1.0f;
            pObj->m_vScale.z = scaleZ;
            pObj->CalcRootMtx();
        }
    }

    if (IsGimmickStop()) {
        return;
    }
    if (!(frame > 0.0f)) {
        return;
    }

    // Each running belt gets a new container whenever it has run
    // HOCKEY_PRM_S09_OBJ_INTERVAL since the last one, dropped just off the
    // side of the court it runs from.
    bool bTreasure = false;
    for (long i = 0; i < 2; i++) {
        m_afTreasureTimer[i] -= frame;
        if (m_afTreasureTimer[i] < 0.0f) {
            m_afTreasureTimer[i] = 0.0f;
        }

        CConveyorState *pConveyor = static_cast<CConveyorState *>(GetConveyor(i));
        CConveyor::FLOW_DIR dir = pConveyor != 0 ? pConveyor->GetFlowDir() : CConveyor::FLOW_DIR_NONE;
        if (dir == CConveyor::FLOW_DIR_NONE || fabsf(pConveyor->GetInterval()) < HOCKEY_PRM_S09_OBJ_INTERVAL) {
            continue;
        }
        pConveyor->SetInterval(0.0f);

        float offset = HOCKEY_PRM_S09_CONTAINER_OFFSET_X;
        float left;
        float right;
        float front;
        float back;
        GetCourtRect(&left, &right, &front, &back);

        float z = i == 0 ? back - HOCKEY_PRM_S09_CV_POS : front + HOCKEY_PRM_S09_CV_POS;
        float x;
        if (dir == CConveyor::FLOW_DIR_4) {
            x = right + (offset - 0.1f);
        } else {
            x = left - (offset - 0.1f);
        }

        // A bomb at HOCKEY_PRM_S09_OBJ_BOMB_RATE percent; treasure instead
        // when this belt's treasure timer has run out.
        float bombRate = HOCKEY_PRM_S09_OBJ_BOMB_RATE;
        int hit = 0;
        if (bombRate >= 100.0f || bcs::BCS_RandMax(100.0f) < bombRate) {
            hit = 1;
        }
        bool bBomb = hit;
        int type = bBomb;
        if (m_afTreasureTimer[i] <= 0.0f && m_anTreasureNum[i] < HOCKEY_PRM_S09_TREASURE_NUM) {
            type = CONTAINER_TREASURE;
            bTreasure = true;
            m_afTreasureTimer[i] = 60.0f * HOCKEY_PRM_S09_TREASURE_INTERVAL;
            m_anTreasureNum[i]++;
        }

        CStgGmk_Container *pContainer = new (1) CStgGmk_Container_Hockey(this, type);
        if (pContainer != 0) {
            pContainer->Create();
            pContainer->SceneEntry();
            pContainer->SetDefaultPos(bcs::FVEC3(x, 0.0f, z));
        }
    }

    if (bTreasure) {
        // Keep the running belts going while the treasure rides them.
        float runTime = 60.0f * HOCKEY_PRM_S09_TREASURE_RUN_TIME;
        for (long i = 0; i < 2; i++) {
            CConveyor *pConveyor = GetConveyor(i);
            CConveyor::FLOW_DIR dir = pConveyor != 0 ? pConveyor->GetFlowDir() : CConveyor::FLOW_DIR_NONE;
            if (dir != CConveyor::FLOW_DIR_NONE && pConveyor->GetDelayTimer() < runTime) {
                pConveyor->DelayStop(runTime);
            }
        }
        CGameSnd::Play(SE_TREASURE, 0, 1, -1, 0.0f, 1.0f, 0.0f);
    }
}

// When conveyor 0 starts on its own, conveyor 1 starts too, in a random
// direction, and both stop again after HOCKEY_PRM_S09_EV_INTERVAL seconds
// (plus up to HOCKEY_PRM_S09_EV_INTERVAL_RND).
void msw::CStageHockey09::Conveyor_OnStart(long no, CConveyor::FLOW_DIR dir, int bAll, float time)
{
    if (bAll == 0 && no == 0) {
        long rnd = bcs::BCS_RandRange(0L, 1L);
        Conveyor_Start(no ^ 1, rnd != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, 0, -1.0f);

        float rndTime = 60.0f * HOCKEY_PRM_S09_EV_INTERVAL_RND;
        float delay = 60.0f * HOCKEY_PRM_S09_EV_INTERVAL;
        delay += bcs::BCS_RandMax(rndTime);
        Conveyor_DelayStopAll(delay);
    }
}

void msw::CStageHockey09::Conveyor_OnSetSpeedLevel(long no, long oldLevel, long level)
{
    if (level > 0 && oldLevel == 0) {
        CConveyor *pConveyor = GetConveyor(no);
        if (pConveyor != 0 && !pConveyor->IsStopped()) {
            Conveyor_DelayStop(60.0f * HOCKEY_PRM_S09_EV_INTERVAL, no);
        }
    }
}

// A stopped belt starts again in a random direction after
// HOCKEY_PRM_S09_CV_STOP_TIME seconds.
void msw::CStageHockey09::Conveyor_OnStop(long no)
{
    long rnd = bcs::BCS_RandRange(0L, 1L);
    Conveyor_DelayStart(60.0f * HOCKEY_PRM_S09_CV_STOP_TIME, no, rnd != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, 0, -1.0f);
}

void msw::CStageHockey09::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManHockey::SendMessage(msg, arg1, arg2, arg3);

    if (msg == MSG_CONVEYOR && arg1 == 2) {
        CConveyor::FLOW_DIR dir;
        if (arg2 == 0) {
            dir = CConveyor::FLOW_DIR_NONE;
        } else if (arg2 > 0) {
            dir = CConveyor::FLOW_DIR_3;
        } else {
            dir = CConveyor::FLOW_DIR_4;
        }

        // The times are given in seconds; the pre-acceleration time is used
        // as it is.
        float accTime = 60.0f * HOCKEY_PRM_S09_CV_ACC_TIME;
        float preAccTime = HOCKEY_PRM_S09_CV_PRE_ACC_TIME;

        if (dir != CConveyor::FLOW_DIR_NONE) {
            if (preAccTime > 0.0f) {
                Conveyor_StopAll();
                Conveyor_DelayAccStartAll(preAccTime, dir, accTime);
            } else {
                Conveyor_AccStartAll(dir, accTime);
            }
        } else {
            Conveyor_StopAll();
            long rnd = bcs::BCS_RandRange(0L, 1L);
            Conveyor_DelayStartAll(accTime, rnd != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, -1, -1.0f);
        }
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey09::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ground_02_b01hck", MDL_BELT_B, 0, 0, NewMdl },
        { "^s[0-9]+_ground_02_b00hck", MDL_BELT_B_2, 0, 0, NewMdl },
        { "^s[0-9]+_ground_02_a01hck", MDL_BELT_A, 0, 0, NewMdl },
        { "^s[0-9]+_ground_02_a00hck", MDL_BELT_A_2, 0, 0, NewMdl },
        { "^s[0-9]+_ground_01hck.*", MDL_LATE, 0, 0, NewMdl },
        { "^s[0-9]+_ground_05.*", MDL_LATE, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageHockey09::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_BELT_B:
            GetConveyor(0)->SetMdlNode(pNode);
            break;
        case MDL_BELT_B_2:
            GetConveyor(0)->SetMdlNode2(pNode);
            break;
        case MDL_BELT_A:
            GetConveyor(1)->SetMdlNode(pNode);
            break;
        case MDL_BELT_A_2:
            GetConveyor(1)->SetMdlNode2(pNode);
            break;
        case MDL_LATE:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
    }

    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageHockey09::OnPreInitialize()
{
    Stage09_OnPreInitialize();
}

void msw::CStageHockey09::OnSceneEntry()
{
}

void msw::CStageHockey09::OnGoalIn(long, GOAL_IN_TYPE)
{
}

void msw::CStageHockey09::OnPeriodInit()
{
    for (long i = 0; i < 2; i++) {
        m_afTreasureTimer[i] = 60.0f * HOCKEY_PRM_S09_TREASURE_INTERVAL;
        m_anTreasureNum[i] = 0;
    }
}

void msw::CStageHockey09::OnTurnInit()
{
    Conveyor_ResetAll();
    long rnd = bcs::BCS_RandRange(0L, 1L);
    Conveyor_DelayStartAll(60.0f * HOCKEY_PRM_S09_CV_STOP_TIME, rnd != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, -1,
                           -1.0f);
}

void msw::CStageHockey09::OnGimmickReset()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
    Container_Clear();
}

void msw::CStageHockey09::OnGimmickCancel()
{
    Container_Clear();
    Conveyor_StopAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

void msw::CStageHockey09::OnGimmickResume()
{
    Conveyor_ResetAll();
    long rnd = bcs::BCS_RandRange(0L, 1L);
    Conveyor_DelayStartAll(60.0f * HOCKEY_PRM_S09_CV_STOP_TIME, rnd != 0 ? CConveyor::FLOW_DIR_4 : CConveyor::FLOW_DIR_3, -1,
                           -1.0f);
}

void msw::CStageHockey09::OnGimmickStop()
{
    Container_Clear();
    Conveyor_StopAll();
}

void msw::CStageHockey09::OnEventStart()
{
    Conveyor_ResetAll();
    SE_Stop(SE_SLOT_CONVEYOR, 0.0f);
}

// The floor is a belt (attribute 2) within half HOCKEY_PRM_S09_CV_WIDTH of
// a belt's centre line, HOCKEY_PRM_S09_CV_POS inside the court's front or
// back edge; elsewhere it is plain floor (0).
long msw::CStageHockey09::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE) const
{
    float front;
    float back;
    GetCourtRect(0, 0, &front, &back);

    float offset = HOCKEY_PRM_S09_CV_POS;
    float halfWidth = 0.5f * HOCKEY_PRM_S09_CV_WIDTH;
    if (rPos.z < 0.0f) {
        float center = front + offset;
        if (rPos.z < center - halfWidth || rPos.z > halfWidth + center) {
            return 0;
        }
    } else {
        float center = back - offset;
        if (rPos.z > halfWidth + center || rPos.z < center - halfWidth) {
            return 0;
        }
    }
    return 2;
}

// On a belt the floor flows with it for flow types 0 to 3 and 6. Which belt
// is read from the area flags (type 1 counts the wider areas too).
void msw::CStageHockey09::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6: {
            bcs::FVEC3 pos(0.0f, rPos.y, rPos.z);
            if (!(GetFloorAttribute(pos, FLOW_TYPE_0) & 2)) {
                rFlow.Set(0.0f, 0.0f, 0.0f);
                break;
            }

            unsigned long area = GetAreaFlagByPos(pos, 0x3003);
            long no;
            if (type == 1) {
                if (area & 0x1001) {
                    no = 0;
                } else {
                    no = -1;
                    if (area & 0x2002) {
                        no = 1;
                    }
                }
            } else {
                if (area & 1) {
                    no = 0;
                } else {
                    no = -1;
                    if (area & 2) {
                        no = 1;
                    }
                }
            }

            if (no < 0) {
                rFlow.Set(0.0f, 0.0f, 0.0f);
            } else {
                GetConveyor(no)->GetFlow(rFlow);
            }
            break;
        }
        default:
            rFlow.Set(0.0f, 0.0f, 0.0f);
            break;
    }
}

void msw::CStageHockey09::OnSoundEvent(long event)
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
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

void msw::CStageHockey09::CConveyorState::Main(float frame)
{
    CConveyor::Main(frame);
    m_fInterval += GetLastSpeed();
}

void msw::CStageHockey09::CConveyorState::Reset()
{
    CConveyor::Reset();
    m_fInterval = 0.0f;
}

// The belt runs faster while its speed level is raised.
float msw::CStageHockey09::CConveyorState::GetParam_CvSpeed() const
{
    if (GetSpeedLevel() > 0) {
        return HOCKEY_PRM_S09_CV_ACC_SPEED_0;
    }
    return HOCKEY_PRM_S09_CV_SPEED_0;
}

float msw::CStageHockey09::CConveyorState::GetParam_AnmFlow() const
{
    return 1.0f / 45.0f;
}

float msw::CStageHockey09::CConveyorState::GetParam_MdlAnmDir() const
{
    return -1.0f;
}

void msw::CStageHockey09::Container_Clear()
{
    CStgGmkMan *pMan = CStgGmkMan::GetInstance();
    for (CStgGmk *pGmk = pMan->GetFirst(GMK_TYPE_CONTAINER); pGmk != 0;
         pGmk = pMan->GetNext(pGmk, GMK_TYPE_CONTAINER)) {
        pGmk->Delete();
    }
}

long msw::CStageHockey09::GetContainerMaxNum()
{
    const CStgGmkMan *pMan = CStgGmkMan::GetInstance();
    long num = 0;
    for (const CStgGmk *pGmk = pMan->GetFirst(GMK_TYPE_CONTAINER); pGmk != 0;
         pGmk = pMan->GetNext(pGmk, GMK_TYPE_CONTAINER)) {
        num++;
    }
    return num;
}

const msw::CStgGmk_Container *msw::CStageHockey09::GetContainer(long idx)
{
    const CStgGmkMan *pMan = CStgGmkMan::GetInstance();
    long i = 0;
    for (const CStgGmk *pGmk = pMan->GetFirst(GMK_TYPE_CONTAINER); pGmk != 0;
         i++, pGmk = pMan->GetNext(pGmk, GMK_TYPE_CONTAINER)) {
        if (pGmk != 0 && i == idx) {
            return static_cast<const CStgGmk_Container *>(pGmk);
        }
    }
    return 0;
}

void msw::CStageHockey09::Disp()
{
    CStageManHockey::Disp();
}

long msw::CStageHockey09::GetConveyorNum() const
{
    return 2;
}

msw::CStage09Base::CConveyor *msw::CStageHockey09::NewConveyor() const
{
    return new CConveyorState;
}

msw::CStgSeSlot *msw::CStageHockey09::GetSeSlot()
{
    return this;
}

long msw::CStageHockey09::GetSeSlotBeg() const
{
    return SE_SLOT_CONVEYOR;
}

long msw::CStageHockey09::SE_GetSlotNum() const
{
    return 5;
}
