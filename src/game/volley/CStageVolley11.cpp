// msw::CStageVolley11: volley stage 11, the volcano stage. CStage11Base (a
// DOL class) tilts the court and moves its centre line, raises bubbles and
// runs the volcano; this class hands the stage manager's hooks on, moves
// the centre line towards the team that trails on points (message 0x54),
// carries the net on the centre line, keeps the models it needs (the
// background, the chains, the fire pillars) and gives the VOLLEY_PRM_S11_*
// tuning. The fire pillar cluster and its pools (bcs::CFixedPool,
// bcs::CPool, bcs::CList) are all inline, so their vtables and weak
// functions are emitted here too.
//
// The original file was StageVolley11.cpp: the net class below is in the
// anonymous namespace of that name (__vt__Q227@unnamed@StageVolley11_cpp@
// 17CStageNetVolley11).
#include "game/msw/CStageVolley11.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStageNetVolley.hpp"
#include "game/msw/CStgMdl.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/VolleyPrm.hpp"

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
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_COURT_ANGLE, 0x00 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_COURT_ROT_SPEED, 0x04 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_COURT_ROT_ACC, 0x08 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_CHR_FLOW_SPEED, 0x0C },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_CENTER_MOVE_SPEED, 0x10 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_CENTER_MOVE_ACC, 0x14 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_CENTER_RANGE, 0x18 },
        { S11_PRM_TYPE_LONG, &VOLLEY_PRM_S11_CENTER_LEVEL, 0x1C },
        { S11_PRM_TYPE_LONG, &VOLLEY_PRM_S11_CENTER_MOVE_SCORE, 0x20 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_CENTER_FIX_RATE, 0x24 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_SCALE, 0x28 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_PRESTART, 0x2C },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_JUMP_HEIGHT, 0x30 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_JUMP_FRM, 0x34 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_JUMP_DOF_FRM, 0x38 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_DOWN_FRM, 0x3C },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_INIT_INTERVAL, 0x40 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_INTERVAL, 0x44 },
        { S11_PRM_TYPE_FLOAT, &VOLLEY_PRM_S11_BUBBLE_ZONE_Z, 0x48 },
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
    MSG_27 = 0x27,
    MSG_SCORE = 0x54 // a point was scored; arg2 says how
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

// The camera shake while the centre line moves.
enum {
    QUAKE_CENTER_MOVE = 0x35
};

// Models and animations of the net.
enum {
    NET_MDL_BODY = 1,
    NET_MDL_BASE = 2,     // the base, a roller that turns as the net moves
    NET_ANM_BASE = 1,
    NET_ANM_MOVE_END = 0x10
};

namespace {

    // The net of this stage stands on the centre line: its base rolls as
    // the net moves along z.
    class CStageNetVolley11 : public msw::CStageNetVolley {
        public:
            CStageNetVolley11(bcs::CTask *pParent) : msw::CStageNetVolley(pParent) {}
            virtual ~CStageNetVolley11();

            virtual void Main();
            virtual void OnPostCreate();
    };
}

const void *msw::CStageVolley11::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley11::CStageVolley11(bcs::CTask *pParent)
    : CStageManVolley(pParent), CStage11Base(), m_Unk270(0.0f), m_pBgMdl(0)
{
}

msw::CStageVolley11::~CStageVolley11()
{
}

// Team 0's court runs from the centre line to the back, team 1's from the
// front to the centre line.
float msw::CStageVolley11::GetTeamCourtFront(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return center;
    }
    return -GetBaseCourtDepth_();
}

float msw::CStageVolley11::GetTeamCourtBack(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return GetBaseCourtDepth_();
    }
    return center;
}

const bcs::FVEC3 &msw::CStageVolley11::GetNetPos() const
{
    const CStageNet *pNet = GetNet();
    const CStgSrt *pSrt = pNet != 0 ? pNet->GetSrt() : 0;
    if (pSrt != 0) {
        return pSrt->GetPos();
    }
    return m_vZero;
}

void msw::CStageVolley11::Main_(float frame)
{
    Stage11_Main_(frame);

    // The net stands on the centre line.
    CStageNet *pNet = GetNet();
    if (pNet != 0) {
        bcs::FVEC3 pos(0.0f, 0.0f, GetCurCourtCenter_());
        pNet->SetPos(pos);
    }

    CStageMan::Main_(frame);
}

void msw::CStageVolley11::SendMessage(long msg, long arg0, long arg1, long arg2)
{
    CStageManVolley::SendMessage(msg, arg0, arg1, arg2);

    switch (arg0) {
        case MSG_3:
        case MSG_13:
            if (!SE_IsAlive(SE_SLOT_MSG)) {
                SE_Play(SE_SLOT_MSG, SE_MSG, 0.0f, 3);
            }
            break;
        case MSG_SCORE:
            switch (arg2) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 6:
                case 7:
                case 8:
                case 9:
                case 11: {
                    // The centre line moves one level towards the leading
                    // team's side for every VOLLEY_PRM_S11_CENTER_MOVE_SCORE
                    // points of the lead in this period.
                    long score0 = CGame::GetTeamPeriodScore(0, CGame::g_GameWork.m_nPeriod);
                    long lead = CGame::GetTeamPeriodScore(1, CGame::g_GameWork.m_nPeriod) - score0;
                    if (CGame::GetTargetGoalIdx(0) != 0) {
                        lead = -lead;
                    }

                    long cur = GetCourtRotLevel();
                    long level = lead / VOLLEY_PRM_S11_CENTER_MOVE_SCORE;
                    long rest = lead % VOLLEY_PRM_S11_CENTER_MOVE_SCORE;
                    long target = level > GetParam_CourtCenterDiv()     ? GetParam_CourtCenterDiv()
                                  : level < -GetParam_CourtCenterDiv() ? -GetParam_CourtCenterDiv()
                                                                       : level;

                    long step = target - cur;
                    if (cur == target) {
                        break;
                    }
                    // Back towards the middle only on a whole level of
                    // lead (or a jump of more than one level).
                    long dir = cur * step;
                    if ((dir < 0 && (rest == 0 || abs(step) > 1)) || dir >= 0) {
                        bool bUp = step >= 0;
                        for (long n = abs(step); n > 0; n--) {
                            if (!bUp) {
                                SetCourtStatus(COURT_STATUS_2);
                            } else {
                                SetCourtStatus(COURT_STATUS_3);
                            }
                        }
                    }
                    break;
                }
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

msw::CStageNet *msw::CStageVolley11::NewNet(bcs::CTask *pParent)
{
    return new CStageNetVolley11(pParent);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley11::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg", MDL_BG, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_00", MDL_FIRE_PILLAR, 0, 0, NewMdl },
        { "^s[0-9]+_hibasira_near_00", MDL_FIRE_PILLAR_NEAR, 0, 0, NewMdl },
        { "^s[0-9]+_kusari_0([0-9])$", MDL_CHAIN, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageVolley11::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId == MDL_BG) {
        m_pBgMdl = *rHandle.Get();
    }
}

void msw::CStageVolley11::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
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
}

void msw::CStageVolley11::OnSceneEntry()
{
}

void msw::CStageVolley11::OnSceneReentry()
{
    m_FirePillar.ResetAll();
}

void msw::CStageVolley11::OnPeriodInit()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
}

void msw::CStageVolley11::OnGameInit()
{
    CStage11Base::Stage11_OnGameInit();
}

void msw::CStageVolley11::OnTurnInit()
{
    CStage11Base::Stage11_OnTurnInit();
}

void msw::CStageVolley11::OnGameEnd()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
}

void msw::CStageVolley11::OnEventStart()
{
    ResetCourtStatus(COURT_STATUS_1);
    ResetCourtRotLevel_();
}

void msw::CStageVolley11::OnCenterMoveBegin(long team)
{
    CGame::StartCameraQuake(QUAKE_CENTER_MOVE, -1, -1);
}

// When the line has moved towards a team, the net plays its end animation
// (the second argument tells which way).
void msw::CStageVolley11::OnCenterMoveEnd(long team)
{
    CGame::StopCameraQuake(QUAKE_CENTER_MOVE);
    if (team != 0) {
        CStageNet *pNet = GetNet();
        CStgMdl *pMdl = pNet != 0 ? pNet->GetMdl(NET_MDL_BODY) : 0;
        // Animation flags; bit 0 plays it the other way (meaning guessed).
        unsigned long flag = 0;
        if (team > 0) {
            flag |= 1;
        }
        if (pMdl != 0) {
            pMdl->SetAnm(NET_ANM_MOVE_END, flag, 1.0f, 0.0f, 0.0f, -1);
        }
    }
}

// A bubble rises at a random point of the bubble court's half of the court
// (less a margin for the bubble's size), outside the strip of
// GetParam_BubbleZoneZ around the centre. The next bubble goes to the
// other court.
bcs::FVEC3 msw::CStageVolley11::Bubble_CalcCreatePos()
{
    bcs::FVEC3 pos;

    long court = Bubble_GetCourt();
    CStageMan *pStage = CStageMan::GetInstance();
    float size = 1.2f * GetParam_BubbleScale();
    float margin = 0.1f + size;

    float left, right, front, back;
    pStage->GetCourtRect(&left, &right, &front, &back);
    left += margin;
    right -= margin;
    front += margin;
    back -= margin;

    if (court == 0) {
        front = 0.0f;
        float zone = GetParam_BubbleZoneZ() - margin;
        back = zone > front ? zone : front;
    } else {
        back = 0.0f;
        float zone = -GetParam_BubbleZoneZ() + margin;
        front = back < zone ? back : zone;
    }

    pos.x = bcs::BCS_RandRange(left, right);
    pos.z = bcs::BCS_RandRange(front, back);
    pos.y = -GetParam_BubbleGroundHeight();

    Bubble_SetNextCourt();
    return pos;
}

float msw::CStageVolley11::GetParam_BubbleScale() const
{
    return VOLLEY_PRM_S11_BUBBLE_SCALE;
}

float msw::CStageVolley11::GetParam_BubbleZoneZ() const
{
    return VOLLEY_PRM_S11_BUBBLE_ZONE_Z;
}

// The angle is given in degrees.
float msw::CStageVolley11::GetParam_CourtAngle() const
{
    return bcs::DEG_TO_RAD(VOLLEY_PRM_S11_COURT_ANGLE);
}

// The speeds are given in degrees per second.
float msw::CStageVolley11::GetParam_CourtRotSpeed() const
{
    return bcs::DEG_TO_RAD(VOLLEY_PRM_S11_COURT_ROT_SPEED) / 60.0f;
}

float msw::CStageVolley11::GetParam_CourtRotAcc() const
{
    return bcs::DEG_TO_RAD(VOLLEY_PRM_S11_COURT_ROT_ACC) / 60.0f;
}

float msw::CStageVolley11::GetParam_ChrFlowSpeed() const
{
    return VOLLEY_PRM_S11_CHR_FLOW_SPEED;
}

// The speeds are given per second.
float msw::CStageVolley11::GetParam_CenterMoveSpeed() const
{
    return VOLLEY_PRM_S11_CENTER_MOVE_SPEED / 60.0f;
}

float msw::CStageVolley11::GetParam_CenterMoveAcc() const
{
    return VOLLEY_PRM_S11_CENTER_MOVE_ACC / 60.0f;
}

float msw::CStageVolley11::GetParam_CourtCenterRange() const
{
    return VOLLEY_PRM_S11_CENTER_RANGE;
}

long msw::CStageVolley11::GetParam_CourtCenterDiv() const
{
    return VOLLEY_PRM_S11_CENTER_LEVEL;
}

long msw::CStageVolley11::GetParam_SE(long type) const
{
    if (type == 2) {
        return SE_BUBBLE;
    }
    return CStage11Base::GetParam_SE(type);
}

long msw::CStageVolley11::GetParam_CVID(long type) const
{
    if (type == 2) {
        return CVID_BUBBLE;
    }
    return -1;
}

float msw::CStageVolley11::GetParam_CourtCenterFixRate() const
{
    return VOLLEY_PRM_S11_CENTER_FIX_RATE;
}

// The net's functions come here in the original file.
void CStageNetVolley11::OnPostCreate()
{
    msw::CStgMdl *pMdl = GetMdl(NET_MDL_BASE);
    if (pMdl != 0) {
        pMdl->SetAnm(NET_ANM_BASE, 2, 1.0f, 0.0f, 0.0f, -1);
        pMdl->SetAnmSpeed(0.0f);
    }
}

void CStageNetVolley11::Main()
{
    msw::CStageNetVolley::Main();

    msw::CStgMdl *pMdl = GetMdl(NET_MDL_BASE);
    if (pMdl != 0) {
        bcs::FVEC3 pos = GetSrt()->GetPos();
        float frameMax = pMdl->GetAnmTotalFrame(msw::CStageAnm::ANM_IDX(0));
        if (frameMax > 0.0f) {
            // The base is a roller of radius 1.3166.
            float turn = -frameMax * (pos.z / 1.3166f / (2.0f * 3.1415927f));
            pMdl->SetAnmFrame(fmodf(frameMax + fmodf(turn, frameMax), frameMax));
        }
    }
}

CStageNetVolley11::~CStageNetVolley11()
{
}

void msw::CStageVolley11::AddCourtRotLevel_(long n)
{
    SetCourtRotLevel_(n + GetCourtRotLevel());
}

// The tilt has as many levels as the centre line.
long msw::CStageVolley11::GetParam_CourtRotDiv() const
{
    return GetParam_CourtCenterDiv();
}

// The intervals are given in seconds.
float msw::CStageVolley11::GetParam_BubbleInitInterval() const
{
    return 60.0f * VOLLEY_PRM_S11_BUBBLE_INIT_INTERVAL;
}

float msw::CStageVolley11::GetParam_BubbleInterval() const
{
    return 60.0f * VOLLEY_PRM_S11_BUBBLE_INTERVAL;
}

float msw::CStageVolley11::GetParam_BubbleDownFrm() const
{
    return VOLLEY_PRM_S11_BUBBLE_DOWN_FRM;
}

float msw::CStageVolley11::GetParam_BubblePreStartFrm() const
{
    return VOLLEY_PRM_S11_BUBBLE_PRESTART;
}

float msw::CStageVolley11::GetParam_BubbleJumpHeight()
{
    return VOLLEY_PRM_S11_BUBBLE_JUMP_HEIGHT;
}

float msw::CStageVolley11::GetParam_BubbleJumpFrm()
{
    return VOLLEY_PRM_S11_BUBBLE_JUMP_FRM;
}

float msw::CStageVolley11::GetParam_BubbleJumpDOF()
{
    return VOLLEY_PRM_S11_BUBBLE_JUMP_DOF_FRM;
}
