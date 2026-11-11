// msw::CStageBasket11: basket stage 11, the volcano stage. CStage11Base (a
// DOL class) tilts the court, raises bubbles and runs the volcano; this
// class hands the stage manager's hooks on, tilts the court one way and
// then the other on a timer, places the bubbles in a fan in front of a
// goal, keeps the models it needs (the background, the chains, the fire
// pillars) and gives the BASKET_PRM_S11_* tuning. The fire pillar cluster
// and its pools (bcs::CFixedPool, bcs::CPool, bcs::CList) are all inline,
// so their vtables and weak functions are emitted here too.
#include "game/msw/CStageBasket11.hpp"

#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "nw4r/math/triangular.h"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S11_PRM_TYPE {
        S11_PRM_TYPE_END,
        S11_PRM_TYPE_LONG,
        S11_PRM_TYPE_FLOAT
    };

    struct S11_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S11_PRM_TBL s_PrmTbl[] = {
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_COURT_INIT_WAIT, 0x00 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_COURT_WAIT, 0x04 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_COURT_ANGLE, 0x08 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_COURT_ROT_SPEED, 0x0C },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_COURT_ROT_ACC, 0x10 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_CHR_FLOW_SPEED, 0x14 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_SCALE, 0x18 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_PRESTART, 0x1C },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_JUMP_HEIGHT, 0x20 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_JUMP_FRM, 0x24 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_JUMP_DOF_FRM, 0x28 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_DOWN_FRM, 0x2C },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_INIT_INTERVAL, 0x30 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_INTERVAL, 0x34 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_ZONE_R, 0x38 },
        { S11_PRM_TYPE_FLOAT, &BASKET_PRM_S11_BUBBLE_ZONE_ANGLE, 0x3C },
        { S11_PRM_TYPE_LONG, &BASKET_PRM_S11_BUBBLE_ZONE_SLOT, 0x40 },
        { S11_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_BG = 1,               // "stage<n>_bg": the background
    MDL_CHAIN = 10,           // "s<n>_kusari_0<i>": chain i
    MDL_FIRE_PILLAR = 100,    // "s<n>_hibasira_00": a fire pillar
    MDL_FIRE_PILLAR_NEAR = 101 // "s<n>_hibasira_near_00": a fire pillar near the court
};

// Messages SendMessage reacts to (names guessed).
enum {
    MSG_3 = 3,
    MSG_13 = 0x13,
    MSG_27 = 0x27
};

// Sound slot and sounds of the stage.
enum {
    SE_SLOT_MSG = 0,
    SE_MSG = 0xD63,
    SE_FIRE_PILLAR = 0xD62,
    SE_BUBBLE = 0xD67
};

// Voices of the fire pillars and of the bubbles.
enum {
    CVID_FIRE_PILLAR = 0x35,
    CVID_FIRE_PILLAR_NEAR = 0x36,
    CVID_BUBBLE = 0x14
};

const void *msw::CStageBasket11::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket11::CStageBasket11(bcs::CTask *pParent)
    : CStageManBasket(pParent), CStage11Base(), m_Unk288(0.0f), m_pBgMdl(0), m_nTiltStatus(COURT_STATUS_3),
      m_fTiltWait(0.0f), m_bRotReady(0)
{
}

msw::CStageBasket11::~CStageBasket11()
{
}

// Team 0's court runs from its offset to the back, team 1's from the front
// to its offset.
float msw::CStageBasket11::GetTeamCourtFront(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    if (team == 0) {
        return offset;
    }
    return -GetBaseCourtDepth_();
}

float msw::CStageBasket11::GetTeamCourtBack(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    if (team == 0) {
        return GetBaseCourtDepth_();
    }
    return offset;
}

void msw::CStageBasket11::Main_(float frame)
{
    Stage11_Main_(frame);

    // Both courts follow the court's centre.
    bcs::FVEC3 offset(0.0f, 0.0f, GetCurCourtCenter_());
    SetTeamCourtOffset(0, offset);
    SetTeamCourtOffset(1, offset);

    CStageManBasket::Main_(frame);
}

void msw::CStageBasket11::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManBasket::SendMessage(msg, arg0, arg1, arg2);

    switch (arg0) {
        case MSG_3:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_MSG)) {
                SE_Play(SE_SLOT_MSG, SE_MSG, 0.0f, 3);
            }
            break;
        case MSG_27:
            if (arg1 == 8) {
                SetCenterFixTeam(arg2);
            } else if (arg1 == 9) {
                SetRotFixTeam(arg2);
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket11::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg$", MDL_BG, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_00", MDL_FIRE_PILLAR, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_near_00", MDL_FIRE_PILLAR_NEAR, 0, 0, NewMdl },
        { "^s[0-9]+_kusari_0([0-9])$", MDL_CHAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageBasket11::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_BG) {
        m_pBgMdl = *rHandle.Get();
    }
}

void msw::CStageBasket11::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_FIRE_PILLAR:
            m_FirePillar.PushBack(new (1) STAGE11::CStgRndAnmObj(this, pNode, CVID_FIRE_PILLAR, SE_FIRE_PILLAR,
                                                                 60.0f, 600.0f));
            pNode->OffFlag(1);
            break;
        case MDL_FIRE_PILLAR_NEAR:
            m_FirePillar.PushBack(new (1) STAGE11::CStgRndAnmObj(this, pNode, CVID_FIRE_PILLAR_NEAR,
                                                                 SE_FIRE_PILLAR, 60.0f, 600.0f));
            pNode->OffFlag(1);
            break;
        case MDL_CHAIN: {
            long no;
            if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0) {
                SetChainMdl(no, pNode);
            }
            pNode->OffFlag(1);
            break;
        }
        case MDL_BG:
            SetRotCenterMdl(pNode);
            pNode->OffFlag(1);
            break;
    }

    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

// The first tilt waits BASKET_PRM_S11_COURT_INIT_WAIT seconds.
void msw::CStageBasket11::OnSceneEntry()
{
    m_fTiltWait = 60.0f * BASKET_PRM_S11_COURT_INIT_WAIT;
}

void msw::CStageBasket11::OnSceneReentry()
{
    m_FirePillar.ResetAll();
}

void msw::CStageBasket11::OnGameInit()
{
    CStage11Base::Stage11_OnGameInit();
    m_bRotReady = 1;
}

void msw::CStageBasket11::OnTurnInit()
{
    CStage11Base::Stage11_OnTurnInit();
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
    m_fTiltWait = 60.0f * BASKET_PRM_S11_COURT_INIT_WAIT;
    Bubble_InitCourt();
}

void msw::CStageBasket11::OnGameEnd()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
    m_bRotReady = 0;
}

void msw::CStageBasket11::OnEntranceEventStart()
{
    m_bRotReady = 0;
}

// The court tilts one way (status 2), then the other (status 3), with a
// wait of BASKET_PRM_S11_COURT_WAIT seconds at rest (status 0) between.
void msw::CStageBasket11::OnCtorCourtStatusChange(COURT_STATUS status)
{
    switch (status) {
        case COURT_STATUS_2:
        case COURT_STATUS_3:
            m_nTiltStatus = status;
            break;
    }
}

void msw::CStageBasket11::OnLoopCourtStatusChange(COURT_STATUS status)
{
    if (status == COURT_STATUS_0 && GetStepCount_() >= m_fTiltWait) {
        if (m_nTiltStatus == COURT_STATUS_2) {
            SetCourtStatus(COURT_STATUS_3);
        } else {
            SetCourtStatus(COURT_STATUS_2);
        }
    }
}

void msw::CStageBasket11::OnDtorCourtStatusChange(COURT_STATUS status)
{
    if (status == COURT_STATUS_0) {
        m_fTiltWait = 60.0f * BASKET_PRM_S11_COURT_WAIT;
    }
}

// A bubble rises at a random point of a fan in front of the goal of the
// current bubble court: one of BASKET_PRM_S11_BUBBLE_ZONE_SLOT (at least
// two) directions spread evenly over the zone angle either side of the
// goal's facing, at a random distance up to the zone radius from the
// three-point area's centre. The next bubble goes to the other court.
bcs::FVEC3 msw::CStageBasket11::Bubble_CalcCreatePos()
{
    bcs::FVEC3 pos;

    long court = Bubble_GetCourt();
    long slotNum = BASKET_PRM_S11_BUBBLE_ZONE_SLOT;
    if (slotNum < 2) {
        slotNum = 2;
    }
    long slot = bcs::BCS_RandMax(slotNum - 1);

    float zoneAngle = GetParam_BubbleZoneAngle();
    float angle = 1.5707964f + (-zoneAngle + (float)slot / (float)(slotNum - 1) * (zoneAngle - -zoneAngle));
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    while (angle < 0.0f) {
        angle += 6.2831855f;
    }
    angle += GetGoalRot(court).y;

    float dist = bcs::BCS_RandF();
    pos.x = dist * nw4r::math::CosRad(angle);
    pos.z = dist * nw4r::math::SinRad(angle);
    pos.x *= GetParam_BubbleZoneR();
    pos.z *= GetParam_BubbleZoneR();

    bcs::FVEC3 center = Get3PointAreaPos(court);
    bcs::FVEC3Add(&pos, &pos, &center);
    pos.y = -GetParam_BubbleGroundHeight();

    Bubble_SetNextCourt();
    return pos;
}

// The angles are given in degrees.
float msw::CStageBasket11::GetParam_BubbleZoneAngle() const
{
    return bcs::DEG_TO_RAD(BASKET_PRM_S11_BUBBLE_ZONE_ANGLE);
}

float msw::CStageBasket11::GetParam_BubbleZoneR() const
{
    return BASKET_PRM_S11_BUBBLE_ZONE_R;
}

float msw::CStageBasket11::GetParam_CourtAngle() const
{
    return bcs::DEG_TO_RAD(BASKET_PRM_S11_COURT_ANGLE);
}

// The speeds are given in degrees per second.
float msw::CStageBasket11::GetParam_CourtRotSpeed() const
{
    return bcs::DEG_TO_RAD(BASKET_PRM_S11_COURT_ROT_SPEED) / 60.0f;
}

float msw::CStageBasket11::GetParam_CourtRotAcc() const
{
    return bcs::DEG_TO_RAD(BASKET_PRM_S11_COURT_ROT_ACC) / 60.0f;
}

float msw::CStageBasket11::GetParam_ChrFlowSpeed() const
{
    return BASKET_PRM_S11_CHR_FLOW_SPEED;
}

long msw::CStageBasket11::GetParam_SE(long type) const
{
    if (type == 1) {
        return SE_BUBBLE;
    }
    return CStage11Base::GetParam_SE(type);
}

long msw::CStageBasket11::GetParam_CVID(long type) const
{
    switch (type) {
        case 0:
        case 1:
            return CVID_BUBBLE;
    }
    return -1;
}

// The centre line does not move in this mode.
float msw::CStageBasket11::GetParam_CourtCenterRange() const
{
    return 0.0f;
}

// The intervals are given in seconds.
float msw::CStageBasket11::GetParam_BubbleInitInterval() const
{
    return 60.0f * BASKET_PRM_S11_BUBBLE_INIT_INTERVAL;
}

float msw::CStageBasket11::GetParam_BubbleInterval() const
{
    return 60.0f * BASKET_PRM_S11_BUBBLE_INTERVAL;
}

float msw::CStageBasket11::GetParam_BubbleScale() const
{
    return BASKET_PRM_S11_BUBBLE_SCALE;
}

float msw::CStageBasket11::GetParam_BubbleDownFrm() const
{
    return BASKET_PRM_S11_BUBBLE_DOWN_FRM;
}

float msw::CStageBasket11::GetParam_BubblePreStartFrm() const
{
    return BASKET_PRM_S11_BUBBLE_PRESTART;
}

float msw::CStageBasket11::GetParam_BubbleJumpHeight()
{
    return BASKET_PRM_S11_BUBBLE_JUMP_HEIGHT;
}

float msw::CStageBasket11::GetParam_BubbleJumpFrm()
{
    return BASKET_PRM_S11_BUBBLE_JUMP_FRM;
}

float msw::CStageBasket11::GetParam_BubbleJumpDOF()
{
    return BASKET_PRM_S11_BUBBLE_JUMP_DOF_FRM;
}
