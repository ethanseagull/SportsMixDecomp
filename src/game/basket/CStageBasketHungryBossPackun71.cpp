// msw::CStageBasketHungryBossPackun71: Hungry Boss Packun stage 71 of the
// basket game, whose two floors rise and sink in turn.
#include "game/msw/CStageBasketHungryBossPackun71.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"
#include <math.h>
#include <stdio.h>

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl gives
    // the boss: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;    // 0x00
        void *m_pValue;  // 0x04
        long m_nOffset;  // 0x08
    };

    static const PRM_TBL s_PrmTbl[] = {
        // For each of the three party game settings, ten tables and their
        // indices, then the floors' values.
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL0_IDX, 0x00 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL0, 0x04 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL1_IDX, 0x08 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL1, 0x0C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL2_IDX, 0x10 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL2, 0x14 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL3_IDX, 0x18 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL3, 0x1C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL4_IDX, 0x20 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL4, 0x24 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL5_IDX, 0x28 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL5, 0x2C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL6_IDX, 0x30 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL6, 0x34 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL7_IDX, 0x38 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL7, 0x3C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL8_IDX, 0x40 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL8, 0x44 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL9_IDX, 0x48 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE0_TBL9, 0x4C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR0_START_TIME_SEC, 0x50 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_FLOOR0_START_MOVE_DIR, 0x54 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR0_FLOAT_DEPTH, 0x58 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR0_DEPTH_CYCLE, 0x5C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR0_FLOAT_STOP_TIME, 0x60 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL0_IDX, 0x64 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL0, 0x68 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL1_IDX, 0x6C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL1, 0x70 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL2_IDX, 0x74 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL2, 0x78 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL3_IDX, 0x7C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL3, 0x80 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL4_IDX, 0x84 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL4, 0x88 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL5_IDX, 0x8C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL5, 0x90 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL6_IDX, 0x94 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL6, 0x98 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL7_IDX, 0x9C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL7, 0xA0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL8_IDX, 0xA4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL8, 0xA8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL9_IDX, 0xAC },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE1_TBL9, 0xB0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR1_START_TIME_SEC, 0xB4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_FLOOR1_START_MOVE_DIR, 0xB8 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR1_FLOAT_DEPTH, 0xBC },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR1_DEPTH_CYCLE, 0xC0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR1_FLOAT_STOP_TIME, 0xC4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL0_IDX, 0xC8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL0, 0xCC },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL1_IDX, 0xD0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL1, 0xD4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL2_IDX, 0xD8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL2, 0xDC },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL3_IDX, 0xE0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL3, 0xE4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL4_IDX, 0xE8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL4, 0xEC },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL5_IDX, 0xF0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL5, 0xF4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL6_IDX, 0xF8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL6, 0xFC },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL7_IDX, 0x100 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL7, 0x104 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL8_IDX, 0x108 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL8, 0x10C },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL9_IDX, 0x110 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_STATE2_TBL9, 0x114 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR2_START_TIME_SEC, 0x118 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_FLOOR2_START_MOVE_DIR, 0x11C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR2_FLOAT_DEPTH, 0x120 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR2_DEPTH_CYCLE, 0x124 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S71_FLOOR2_FLOAT_STOP_TIME, 0x128 },
        { PRM_TYPE_LONG, &BASKET_PRM_S71_DELIVERY_PATTERN, 0x12C },
        { PRM_TYPE_END, 0, 0 },
    };
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// (guessed helper) A stage parameter for the current party game setting
// (party game parameter 0): each parameter comes in three sets.
template <typename T>
static inline T SelectPrm(const T &rPrm0, const T &rPrm1, const T &rPrm2)
{
    if (msw::CGame::GetPartyGameParam(0) == 0) {
        return rPrm0;
    }
    if (msw::CGame::GetPartyGameParam(0) == 1) {
        return rPrm1;
    }
    return rPrm2;
}

// (guessed helper) Bit 0 of the game's flags (meaning not known): while it
// is set the floors wait.
static inline bool IsGameFlag0()
{
    return msw::CGame::g_GameWork.m_nFlag18 & 1;
}

#define S71_FLOOR_PRM(name)                                                                                   \
    SelectPrm(msw::BASKET_PRM_S71_FLOOR0_##name, msw::BASKET_PRM_S71_FLOOR1_##name,                           \
              msw::BASKET_PRM_S71_FLOOR2_##name)

const void *msw::CStageBasketHungryBossPackun71::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasketHungryBossPackun71::CStageBasketHungryBossPackun71(bcs::CTask *pParent)
    : CStageBasketHungryBossPackun(pParent), m_fFloorTime(0.0f), m_bFloorDir(0), m_bFloorReady(0)
{
    for (int i = 0; i < FLOOR_NUM; i++) {
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
        m_apFloorNode[i] = 0;
        m_a3PointPos[i].Set(0.0f, 0.0f, 0.0f);
    }
}

msw::CStageBasketHungryBossPackun71::~CStageBasketHungryBossPackun71()
{
}

float msw::CStageBasketHungryBossPackun71::GetTeamCourtFront(long team) const
{
    // The front of team 0's court moves with its floor; team 1's is fixed.
    // Either stays between fFront and the back of the court.
    static const float fFront = -11.5f;
    float offset = GetTeamCourtOffsetZ(team);
    float limit = 2.0f * GetBaseCourtDepth_() - 11.5f;
    float front = team == 0 ? offset : fFront;

    if (fFront > front) {
        front = fFront;
    }
    if (limit < front) {
        front = limit;
    }
    return front;
}

float msw::CStageBasketHungryBossPackun71::GetTeamCourtBack(long team) const
{
    // The back of team 1's court moves with its floor; team 0's is fixed.
    // Either stays between fFront and the back of the court.
    static const float fFront = -11.5f;
    float back = GetTeamCourtOffsetZ(team);
    float limit = 2.0f * GetBaseCourtDepth_() - 11.5f;

    if (team == 0) {
        back = limit;
    }
    if (fFront > back) {
        back = fFront;
    }
    if (limit < back) {
        back = limit;
    }
    return back;
}

void msw::CStageBasketHungryBossPackun71::Main_(float frame)
{
    if (!IsReadyFloorFlow_() && !IsGameFlag0()) {
        m_bFloorReady = 1;
    }

    if (IsReadyFloorFlow_()) {
        // The floors speed up over START_TIME_SEC seconds.
        m_fFloorTime += frame * m_fFloorSpeed;
        if (m_fFloorSpeed < 1.0f && S71_FLOOR_PRM(START_TIME_SEC) > 0.0f) {
            m_fFloorSpeed += 1.0f / (60.0f * S71_FLOOR_PRM(START_TIME_SEC));
            m_fFloorSpeed = m_fFloorSpeed > 1.0f ? 1.0f : m_fFloorSpeed < 0.0f ? 0.0f : m_fFloorSpeed;
        }
    }

    // One period: a quarter of a wave, a stop, half a wave, a stop, the
    // last quarter. Times are in frames.
    float cycle = 60.0f * S71_FLOOR_PRM(DEPTH_CYCLE);
    float stop = 60.0f * S71_FLOOR_PRM(FLOAT_STOP_TIME);
    float period = 2.0f * stop + cycle;

    while (m_fFloorTime >= period) {
        m_fFloorTime -= period;
    }

    for (int i = 0; i < FLOOR_NUM; i++) {
        bcs::FVEC3 prev;
        bcs::FVEC3 offset;
        float rate;

        offset = GetTeamCourtOffset(i);
        prev = offset;

        // Not used. (kept for the call the original makes)
        fmodf(m_fFloorTime, cycle);

        if (m_fFloorTime < 0.25f * cycle) {
            rate = fmodf(m_fFloorTime, cycle) / cycle;
        } else if (m_fFloorTime >= 0.25f * cycle && m_fFloorTime < 0.25f * cycle + stop) {
            rate = 0.25f;
        } else if (m_fFloorTime - stop < 0.75f * cycle) {
            rate = fmodf(m_fFloorTime - stop, cycle) / cycle;
        } else if (m_fFloorTime - stop >= 0.75f * cycle && m_fFloorTime < 0.75f * cycle + 2.0f * stop) {
            rate = 0.75f;
        } else {
            rate = fmodf(m_fFloorTime - 2.0f * stop, cycle) / cycle;
        }

        float wave = nw4r::math::SinRad(2.0f * 3.14159265f * rate);
        float depth = 0.5f * S71_FLOOR_PRM(FLOAT_DEPTH);
        if (m_bFloorDir) {
            offset.z = wave * depth;
        } else {
            offset.z = -wave * depth;
        }
        SetTeamCourtOffset(i, offset);
        bcs::FVEC3Sub(&m_aFloorFlow[i], &offset, &prev);

        // The floor sound starts when the floors turn round and when they
        // leave the middle.
        float flowZ = m_aFloorFlow[i].z;
        float prevZ = prev.z;
        if ((i == 0 && flowZ * prevZ < 0.0f) || (__fabsf(prevZ) < 0.001f && __fabsf(flowZ) >= 0.001f)) {
            SE_Play(4, 0xDAB, 0.0f, 0);
        }

        // The floor's model follows the court, in the frame of the stage's
        // background model.
        if (m_apFloorNode[i] != 0) {
            bcs::CScnObj *pObj = m_apFloorNode[i]->GetObj();
            if (pObj != 0) {
                bcs::FVEC3 pos(GetTeamCourtOffsetX(i), 0.0f, GetTeamCourtOffsetZ(i));

                if (m_pBgNode != 0) {
                    bcs::FMTX34 mtx;
                    m_pBgNode->GetObj()->GetMtx(bcs::CScnObj::SCNOBJMTXTYPE_ROOT, mtx);
                    PSMTXMultVec(mtx.m, reinterpret_cast<Vec *>(&pos), reinterpret_cast<Vec *>(&pos));
                }
                pObj->m_vTrans = pos;
                pObj->CalcRootMtx();
            }
        }

        // The three-point line moves with the court.
        bcs::FVEC3 courtOffset = GetTeamCourtOffset(i);
        bcs::FVEC3Add(&m_a3PointPos[i], &GetGoalPos(i), &courtOffset);
    }

    CStageBasketHungryBossPackun::Main_(frame);
}

void msw::CStageBasketHungryBossPackun71::UpdateCourtSize_()
{
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasketHungryBossPackun71::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageBasketHungryBossPackun.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_wavelog_([0-9]+)", 1, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageBasketHungryBossPackun::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasketHungryBossPackun71::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile,
                                                           CArn &rArn, long n)
{
    CStageBasketHungryBossPackun::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);

    if (pRule->m_nId == 1) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no >= 0 && no < FLOOR_NUM) {
            m_apFloorNode[no] = pNode;
        }
    }
}

long msw::CStageBasketHungryBossPackun71::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    if (GetAreaFlagByPos(rPos, 0xF0F) != 0) {
        return 2;
    }
    return CStageMan::GetFloorAttribute(rPos, type);
}

void msw::CStageBasketHungryBossPackun71::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                                       FLOW_TYPE type) const
{
    // Floor i carries the area of bit i and the one of bit 8 + (i ^ 1) of
    // the area flags (meaning of the bits not known).
    unsigned long flag = GetAreaFlagByPos(rPos, 0xF);

    for (long i = 0; i < FLOOR_NUM; i++) {
        if (flag & ((1 << i) | (1 << ((i ^ 1) + 8)))) {
            rFlow = m_aFloorFlow[i];
            return;
        }
    }
    rFlow.Set(0.0f, 0.0f, 0.0f);
}

void msw::CStageBasketHungryBossPackun71::OnGimmickReset()
{
    m_fFloorTime = 0.0f;
    m_fFloorSpeed = 0.0f;
    m_bFloorReady = 0;

    for (long i = 0; i < FLOOR_NUM; i++) {
        SetTeamCourtOffset(i, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }

    // 0 and 1 fix the floors' first direction; anything else draws it.
    switch (S71_FLOOR_PRM(START_MOVE_DIR)) {
        case 0:
            m_bFloorDir = 1;
            break;
        case 1:
            m_bFloorDir = 0;
            break;
        default:
            m_bFloorDir = bcs::BCS_RandF() < 0.5f;
            break;
    }
}

int msw::CStageBasketHungryBossPackun71::IsReadyFloorFlow_() const
{
    if (GetGimmickStep() == GIMMICK_STOP) {
        return 0;
    }
    return m_bFloorReady;
}

void msw::CStageBasketHungryBossPackun71::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(0)) {
            SE_Play(0, 0xDA8, 0.0f, 3);
        }
        if (!SE_IsAlive(1)) {
            SE_RepeatPlay(1, 0xDAC, 180.0f, 420.0f, -1.0f, 0.0f, 12);
        }
        if (!SE_IsAlive(2)) {
            SE_Play(0, 0xDAD, 0.0f, 3);
        }
    }
}

long msw::CStageBasketHungryBossPackun71::SE_GetSlotNum() const
{
    return 5;
}

const bcs::FVEC3 &msw::CStageBasketHungryBossPackun71::Get3PointAreaPos(long goal) const
{
    return m_a3PointPos[goal];
}
