// msw::CStageDodge11: dodge stage 11, the volcano stage. CStage11Base (a
// DOL class) tilts the court and moves its centre line, raises bubbles and
// runs the volcano; this class hands the stage manager's hooks on, tilts
// the court towards the team with more players in its infield, keeps the
// models it needs (the background, the rolling ground strip, the chains,
// the fire pillars) and gives the DODGE_PRM_S11_* tuning. The fire pillar
// cluster and its pools (bcs::CFixedPool, bcs::CPool, bcs::CList) are all
// inline, so their vtables and weak functions are emitted here too.
#include "game/msw/CStageDodge11.hpp"

#include <math.h>
#include <stdio.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/DodgePrm.hpp"

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
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_COURT_ANGLE, 0x00 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_COURT_ROT_SPEED, 0x04 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_COURT_ROT_ACC, 0x08 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_CHR_FLOW_SPEED, 0x0C },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_CENTER_MOVE_SPEED, 0x10 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_CENTER_MOVE_ACC, 0x14 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_CENTER_RANGE, 0x18 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_SCALE, 0x1C },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_PRESTART, 0x20 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_JUMP_HEIGHT, 0x24 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_JUMP_FRM, 0x28 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_JUMP_DOF_FRM, 0x2C },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_DOWN_FRM, 0x30 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_INIT_INTERVAL, 0x34 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_INTERVAL, 0x38 },
        { S11_PRM_TYPE_FLOAT, &DODGE_PRM_S11_BUBBLE_ZONE_Z, 0x3C },
        { S11_PRM_TYPE_END, 0, 0 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_BG = 1,               // "stage<n>_bg": the background
    MDL_CLINE_GROUND = 5,     // "s<n>_dod_cline_ground_01": the centre line on the ground
    MDL_CLINE_P = 6,          // "s<n>_dod_cline_p_01": the rolling strip under the centre line
    MDL_CHAIN = 10,           // "s<n>_kusari_0<i>": chain i
    MDL_FIRE_PILLAR = 100,    // "s<n>_hibasira_00": a fire pillar
    MDL_FIRE_PILLAR_NEAR = 101 // "s<n>_hibasira_near_00": a fire pillar near the court
};

// Messages SendMessage reacts to (names guessed).
enum {
    MSG_3 = 3,
    MSG_13 = 0x13,
    MSG_27 = 0x27,
    MSG_40 = 0x40
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
    CVID_BUBBLE = 0x35
};

const void *msw::CStageDodge11::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge11::CStageDodge11(bcs::CTask *pParent)
    : CStageManDodge(pParent), CStage11Base(), m_Unk2B4(0.0f), m_pBgMdl(0), m_pGroundNode(0), m_bRotReady(0)
{
}

msw::CStageDodge11::~CStageDodge11()
{
}

// Team 0's court runs from the centre line to the back, team 1's from the
// front to the centre line.
float msw::CStageDodge11::GetTeamCourtFront(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return center;
    }
    return -GetBaseCourtDepth_();
}

float msw::CStageDodge11::GetTeamCourtBack(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return GetBaseCourtDepth_();
    }
    return center;
}

// The outer fields are those of the other team (as in CStageManDodge); the
// side zones (n 2 and 3) run along the court, the end behind team 1 (n 1)
// starts at the court.
float msw::CStageDodge11::GetTeamOuterFieldFront(long team, long n) const
{
    long other = team ^ 1;
    if (other == 0) {
        if (n == 1) {
            return GetTeamCourtBack(other);
        }
        return GetBaseOuterFront_();
    }
    if (n == 2 || n == 3) {
        return GetTeamCourtFront(other);
    }
    float depth = GetBaseOuterDepth_();
    return GetTeamCourtFront(other) - depth;
}

float msw::CStageDodge11::GetTeamOuterFieldBack(long team, long n) const
{
    long other = team ^ 1;
    if (other == 0) {
        if (n == 2 || n == 3) {
            return GetTeamCourtBack(other);
        }
        float depth = GetBaseOuterDepth_();
        return GetTeamCourtBack(other) + depth;
    }
    if (n == 1) {
        return GetTeamCourtFront(other);
    }
    return -GetBaseOuterFront_();
}

void msw::CStageDodge11::Main_(float frame)
{
    // The radius of the rolling ground strip.
    static const float fRadius = 0.9685f;

    Stage11_Main_(frame);

    // The centre line models follow the court's centre.
    float center = GetCurCourtCenter_();
    CMdlGroup *pGroup = GetMdlGroup(0);
    for (bcs::CListNodeBase *pIt = pGroup->GetList().GetFirst(); pIt != pGroup->GetList().GetEnd();
         bcs::CListNodeBase::Step(pIt)) {
        bcs::CListBase &rList = static_cast<CMdlGroup::CNode *>(pIt)->GetMdl()->GetNodeList()->GetList();

        for (bcs::CListNodeBase *pNodeIt = rList.GetFirst(); pNodeIt != rList.GetEnd();
             bcs::CListNodeBase::Step(pNodeIt)) {
            CStageMdlListNode *pNode = static_cast<CStageMdlListNode *>(pNodeIt);

            if ((pNode->GetFlag() & 0x20) == 0x20) {
                bcs::CScnObj *pObj = pNode->GetObj();
                if (pObj != 0) {
                    pObj->m_vTrans = bcs::FVEC3(pObj->m_vTrans.x, pObj->m_vTrans.y, center);
                    pObj->CalcRootMtx();
                }
            }
        }
    }

    // While the game runs, tilt the court towards the team with more
    // players in its infield.
    if (IsRotReady_() && !(CGame::g_GameWork.m_nFlag18 & 0x4306) && !(CGame::g_GameWork.m_nFlag18 & 0x200)) {
        long num = CChrMan::GetChrCoreNum();
        long balance = 0;
        GetCurCourtCenter_();
        for (long i = 0; i < num; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            long team = CGame::GetTargetGoalIdx(0) == 0;
            if (pChr != 0 && pChr->IsEnable() && pChr->IsDodgeInfield()) {
                balance = team == pChr->GetTeamNo() ? balance - 1 : balance + 1;
            }
        }

        long level = GetCourtRotLevel();
        long step = -1;
        if (balance - level > 0) {
            step = 1;
        }
        COURT_STATUS status = step < 0 ? COURT_STATUS_2 : COURT_STATUS_3;
        while (balance != level) {
            SetCourtStatus(status);
            balance -= step;
        }
    }

    // The ground strip rolls with the centre line: its animation frame
    // follows the strip's z as the turn of a roller of radius fRadius.
    CStageMdlListNode *pGround = m_pGroundNode;
    bcs::CScnObj *pObj = pGround != 0 ? pGround->GetObj() : 0;
    CStageNodeAnm *pAnm = pGround != 0 ? pGround->GetAnm() : 0;
    if (pObj != 0 && pAnm != 0) {
        bcs::FVEC3 trans = pObj->m_vTrans;
        float frameMax = pAnm->GetFrameMax(0);
        if (frameMax > 0.0f) {
            float turn = -frameMax * (trans.z / fRadius / (2.0f * 3.1415927f));
            pAnm->SetFrame(-1, fmodf(frameMax + fmodf(turn, frameMax), frameMax));
        }
    }

    CStageManDodge::Main_(frame);
}

void msw::CStageDodge11::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManDodge::SendMessage(msg, arg0, arg1, arg2);

    switch (arg0) {
        case MSG_3:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_MSG)) {
                SE_Play(SE_SLOT_MSG, SE_MSG, 0.0f, 3);
            }
            break;
        case MSG_40:
            m_bRotReady = 0;
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

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge11::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg$", MDL_BG, 0, 0, NewMdl },
        { "^s[0-9]+_dod_cline_ground_01$", MDL_CLINE_GROUND, 0, 0, NewMdl },
        { "^s[0-9]+_dod_cline_p_01$", MDL_CLINE_P, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_00", MDL_FIRE_PILLAR, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_near_00", MDL_FIRE_PILLAR_NEAR, 0, 0, NewMdl },
        { "^s[0-9]+_kusari_0([0-9])$", MDL_CHAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge11::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_BG) {
        m_pBgMdl = *rHandle.Get();
    }
}

void msw::CStageDodge11::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_CLINE_GROUND:
            pNode->OnFlag(0x20);
            pNode->OffFlag(1);
            break;
        case MDL_CLINE_P: {
            pNode->OnFlag(0x20);
            m_pGroundNode = pNode;
            CStageNodeAnm *pAnm = pNode != 0 ? pNode->GetAnm() : 0;
            if (pAnm != 0) {
                pAnm->Play(1, 2, 1.0f, 0.0f, 0.0f, -1);
                pAnm->SetRate(-1, 0.0f);
            }
            pNode->OffFlag(1);
            break;
        }
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
}

void msw::CStageDodge11::OnSceneEntry()
{
}

void msw::CStageDodge11::OnSceneReentry()
{
    m_FirePillar.ResetAll();
}

void msw::CStageDodge11::OnGameInit()
{
    CStage11Base::Stage11_OnGameInit();
}

void msw::CStageDodge11::OnTurnInit()
{
    CStage11Base::Stage11_OnTurnInit();
    ResetCourtStatus(COURT_STATUS_0);
    m_bRotReady = 1;
}

void msw::CStageDodge11::OnPeriodInit()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
}

void msw::CStageDodge11::OnGameStart()
{
    m_bRotReady = 1;
}

void msw::CStageDodge11::OnGameEnd()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
    m_bRotReady = 0;
}

void msw::CStageDodge11::OnCenterMoveBegin(long team)
{
}

void msw::CStageDodge11::OnCenterMoveEnd(long team)
{
}

// The angle is given in degrees.
float msw::CStageDodge11::GetParam_CourtAngle() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S11_COURT_ANGLE);
}

// The speeds are given in degrees per second.
float msw::CStageDodge11::GetParam_CourtRotSpeed() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S11_COURT_ROT_SPEED) / 60.0f;
}

float msw::CStageDodge11::GetParam_CourtRotAcc() const
{
    return bcs::DEG_TO_RAD(DODGE_PRM_S11_COURT_ROT_ACC) / 60.0f;
}

long msw::CStageDodge11::GetParam_CourtRotDiv() const
{
    return DODGE_PRM_S11_CENTER_LEVEL;
}

float msw::CStageDodge11::GetParam_ChrFlowSpeed() const
{
    return DODGE_PRM_S11_CHR_FLOW_SPEED;
}

// The speeds are given per second.
float msw::CStageDodge11::GetParam_CenterMoveSpeed() const
{
    return DODGE_PRM_S11_CENTER_MOVE_SPEED / 60.0f;
}

float msw::CStageDodge11::GetParam_CenterMoveAcc() const
{
    return DODGE_PRM_S11_CENTER_MOVE_ACC / 60.0f;
}

float msw::CStageDodge11::GetParam_CourtCenterRange() const
{
    return DODGE_PRM_S11_CENTER_RANGE;
}

long msw::CStageDodge11::GetParam_CourtCenterDiv() const
{
    return DODGE_PRM_S11_CENTER_LEVEL;
}

long msw::CStageDodge11::GetParam_SE(long type) const
{
    if (type == 2) {
        return SE_BUBBLE;
    }
    return CStage11Base::GetParam_SE(type);
}

long msw::CStageDodge11::GetParam_CVID(long type) const
{
    if (type == 2) {
        return CVID_BUBBLE;
    }
    return -1;
}

void msw::CStageDodge11::AddCourtRotLevel_(long n)
{
    SetCourtRotLevel_(n + GetCourtRotLevel());
}

float msw::CStageDodge11::GetParam_BubbleZoneZ() const
{
    return DODGE_PRM_S11_BUBBLE_ZONE_Z;
}

// The intervals are given in seconds.
float msw::CStageDodge11::GetParam_BubbleInitInterval() const
{
    return 60.0f * DODGE_PRM_S11_BUBBLE_INIT_INTERVAL;
}

float msw::CStageDodge11::GetParam_BubbleInterval() const
{
    return 60.0f * DODGE_PRM_S11_BUBBLE_INTERVAL;
}

float msw::CStageDodge11::GetParam_BubbleScale() const
{
    return DODGE_PRM_S11_BUBBLE_SCALE;
}

float msw::CStageDodge11::GetParam_BubbleDownFrm() const
{
    return DODGE_PRM_S11_BUBBLE_DOWN_FRM;
}

float msw::CStageDodge11::GetParam_BubblePreStartFrm() const
{
    return DODGE_PRM_S11_BUBBLE_PRESTART;
}

float msw::CStageDodge11::GetParam_BubbleJumpHeight()
{
    return DODGE_PRM_S11_BUBBLE_JUMP_HEIGHT;
}

float msw::CStageDodge11::GetParam_BubbleJumpFrm()
{
    return DODGE_PRM_S11_BUBBLE_JUMP_FRM;
}

float msw::CStageDodge11::GetParam_BubbleJumpDOF()
{
    return DODGE_PRM_S11_BUBBLE_JUMP_DOF_FRM;
}
