// msw::CStageVolley05: volley stage 05, two rafts that sway on the water,
// side to side and back and forth, with a turret each. BasketF and DodgeF
// have a raft stage 05 too, but with a different motion (along Z only,
// with stops); only the turrets and the model handling are shared.
#include "game/msw/CStageVolley05.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/VolleyPrm.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>
#include <stdio.h>

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
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
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_COURT_WIDTH, 0x00 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_COURT_DEPTH, 0x04 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_WALL_WIDTH, 0x08 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_WALL_DEPTH, 0x0C },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_OUTER_WIDTH, 0x10 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_OUTER_DEPTH, 0x14 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_NET_WIDTH, 0x18 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_NET_HEIGHT_TOP, 0x1C },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_NET_HEIGHT_BTM, 0x20 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_FLOAT_WIDTH, 0x24 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_WIDTH_CYCLE, 0x28 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_FLOAT_DEPTH, 0x2C },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_DEPTH_CYCLE, 0x30 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_TIME_SEC, 0x34 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_FLOAT_WIDTH, 0x38 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_WIDTH_CYCLE, 0x3C },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_FLOAT_DEPTH, 0x40 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_DEPTH_CYCLE, 0x44 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_HEAVY_ACC_TIME_SEC, 0x48 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_TURRET_EARLY_HIT, 0x4C },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_ENV_SE_TIME, 0x50 },
        { PRM_TYPE_FLOAT, &VOLLEY_PRM_S05_ENV_SE_TIME_RND, 0x54 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// Effects and sounds of the turrets.
enum {
    EFF_TURRET_HIT_0 = 0x31,   // team 0's turret fires
    EFF_TURRET_HIT_1 = 0x32,
    EFF_TURRET_BREAK_0 = 0x33, // team 0's turret breaks
    EFF_TURRET_BREAK_1 = 0x34,
    SE_TURRET_HIT = 0xD2B,
    SE_TURRET_BREAK = 0xD2C,
    SE_TURRET_HIT2 = 0xD2D,
    SE_TURRET_BREAK2 = 0xD2E
};

// Animations of a turret.
enum {
    TURRET_ANM_IDLE = 1,
    TURRET_ANM_HIT = 2,
    TURRET_ANM_BREAK = 3,
    TURRET_ANM_BROKEN = 4
};

// Sound slots and sounds of the stage (names guessed).
enum {
    SE_SLOT_WATER = 0,
    SE_SLOT_ENV = 1,
    SE_SLOT_WAVE = 2,
    SE_SLOT_CROWD = 3,
    SE_SLOT_ENV2 = 4,
    SE_SLOT_RAFT = 5,
    SE_SLOT_NUM = 6,
    SE_WATER = 0xD23,
    SE_RAFT = 0xD26,
    SE_ENV = 0xD27,
    SE_ENV2 = 0xD28,
    SE_WAVE = 0xD2F,
    SE_CROWD = 0xD30
};

// (name guessed) Bit 0 of the game's flags (meaning not known).
static inline bool IsGameFlag0()
{
    return msw::CGame::g_GameWork.m_nFlag18 & 1;
}

const void *msw::CStageVolley05::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley05::CStageVolley05(bcs::CTask *pParent)
    : CStageManVolley(pParent), m_fFloatTimeH(0.0f), m_fFloatTimeV(0.0f), m_fFloatSpeedH(0.0f),
      m_fFloatSpeedV(0.0f), m_bFloatReady(0), m_bHeavyShakeH(0), m_bHeavyShakeV(0), m_fHeavyTime(0.0f)
{
    for (long i = 0; i < RAFT_NUM; i++) {
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }
    for (long i = 0; i < RAFT_NUM; i++) {
        m_aTurret[i].m_pNode = 0;
        m_aTurret[i].m_vPos.Set(0.0f, 0.0f, 0.0f);
    }
}

msw::CStageVolley05::~CStageVolley05()
{
}

float msw::CStageVolley05::GetBaseCourtWidth_() const
{
    return VOLLEY_PRM_S05_COURT_WIDTH;
}

float msw::CStageVolley05::GetBaseCourtDepth_() const
{
    return VOLLEY_PRM_S05_COURT_DEPTH;
}

float msw::CStageVolley05::GetBaseGroundWidth_() const
{
    return VOLLEY_PRM_S05_WALL_WIDTH;
}

float msw::CStageVolley05::GetBaseGroundDepth_() const
{
    return VOLLEY_PRM_S05_WALL_DEPTH;
}

float msw::CStageVolley05::GetBaseOuterWidth_() const
{
    return VOLLEY_PRM_S05_OUTER_WIDTH;
}

float msw::CStageVolley05::GetBaseOuterDepth_() const
{
    return VOLLEY_PRM_S05_OUTER_DEPTH;
}

float msw::CStageVolley05::GetBaseOuterFront_() const
{
    return VOLLEY_PRM_S05_OUTER_FRONT;
}

// The ground reaches as far as the rafts can swing.
float msw::CStageVolley05::GetGroundLeft() const
{
    return -(0.5f * GetBaseCourtWidth_()) - GetBaseGroundWidth_() - GetParam_FloatWidth();
}

float msw::CStageVolley05::GetGroundRight() const
{
    return 0.5f * GetBaseCourtWidth_() + GetBaseGroundWidth_() + GetParam_FloatWidth();
}

float msw::CStageVolley05::GetGroundFront() const
{
    return -GetBaseCourtDepth_() - GetBaseGroundDepth_() - GetParam_FloatDepth();
}

float msw::CStageVolley05::GetGroundBack() const
{
    return GetBaseCourtDepth_() + GetBaseGroundDepth_() + GetParam_FloatDepth();
}

float msw::CStageVolley05::GetTeamGroundLeft(long team) const
{
    return -(0.5f * GetBaseCourtWidth_()) - GetBaseGroundWidth_() - GetParam_FloatWidth();
}

float msw::CStageVolley05::GetTeamGroundRight(long team) const
{
    return 0.5f * GetBaseCourtWidth_() + GetBaseGroundWidth_() + GetParam_FloatWidth();
}

// Team 0 plays on the back raft, team 1 on the front one.
float msw::CStageVolley05::GetTeamGroundFront(long team) const
{
    if (team == 0) {
        return 0.0f;
    }
    return -GetBaseCourtDepth_() - GetBaseGroundDepth_() - GetParam_FloatDepth();
}

float msw::CStageVolley05::GetTeamGroundBack(long team) const
{
    if (team != 0) {
        return 0.0f;
    }
    return GetBaseCourtDepth_() + GetBaseGroundDepth_() + GetParam_FloatDepth();
}

float msw::CStageVolley05::GetRaftWidth_() const
{
    return 8.8f;
}

float msw::CStageVolley05::GetRaftEdgeLeft_(long team) const
{
    return GetTeamCourtOffsetX(team) - GetRaftWidth_();
}

float msw::CStageVolley05::GetRaftEdgeRight_(long team) const
{
    return GetTeamCourtOffsetX(team) + GetRaftWidth_();
}

void msw::CStageVolley05::Main_(float frame)
{
    Turret_SetPos();

    if (!IsReadyFloorFlow_() && !IsGameFlag0()) {
        m_bFloatReady = 1;
    }

    float prevTimeH = m_fFloatTimeH;
    float prevTimeV = m_fFloatTimeV;
    if (IsReadyFloorFlow_()) {
        // The rafts speed up at the start and again when the heavy phase
        // begins.
        m_fFloatTimeH += frame * m_fFloatSpeedH;
        float accTime;
        if (IsNowHeavyShakeH()) {
            accTime = 60.0f * VOLLEY_PRM_S05_START_TIME_SEC;
        } else {
            accTime = 60.0f * VOLLEY_PRM_S05_HEAVY_ACC_TIME_SEC;
        }
        if (m_fFloatSpeedH < 1.0f && accTime > 0.0f) {
            m_fFloatSpeedH += 1.0f / accTime;
            m_fFloatSpeedH = m_fFloatSpeedH > 1.0f ? 1.0f : m_fFloatSpeedH < 0.0f ? 0.0f : m_fFloatSpeedH;
        }

        m_fFloatTimeV += frame * m_fFloatSpeedV;
        if (IsNowHeavyShakeH()) {
            accTime = 60.0f * VOLLEY_PRM_S05_START_TIME_SEC;
        } else {
            accTime = 60.0f * VOLLEY_PRM_S05_HEAVY_ACC_TIME_SEC;
        }
        if (m_fFloatSpeedV < 1.0f && accTime > 0.0f) {
            m_fFloatSpeedV += 1.0f / accTime;
            m_fFloatSpeedV = m_fFloatSpeedV > 1.0f ? 1.0f : m_fFloatSpeedV < 0.0f ? 0.0f : m_fFloatSpeedV;
        }
    }

    if (IsReadyFloorFlow_() && IsGimmickRunning() && frame > 0.0f && m_fHeavyTime > 0.0f) {
        m_fHeavyTime -= frame;
        float time = m_fHeavyTime;
        if (0.0f > time) {
            time = 0.0f;
        }
        m_fHeavyTime = time;
    }

    for (long i = 0; i < RAFT_NUM; i++) {
        bcs::FVEC3 prev;
        bcs::FVEC3 offset;

        offset = GetTeamCourtOffset(i);
        prev = offset;

        // Side to side: a sine wave. (SinFIdx with the conversion written
        // out, not nw4r::math::SinRad: the original's float pool has the
        // conversion constant before 2 pi.)
        float cycleH = GetParam_WidthCycle();
        float width = GetParam_FloatWidth();
        while (m_fFloatTimeH >= cycleH) {
            m_fFloatTimeH -= cycleH;
        }
        // -1 when the swing started again, 1 at its middle: the two turns
        // of the rafts.
        long turnH = 0;
        if (prevTimeH > m_fFloatTimeH) {
            turnH = -1;
        } else if (prevTimeH < 0.5f * cycleH && m_fFloatTimeH >= 0.5f * cycleH) {
            turnH = 1;
        }
        if (IsReadyFloorFlow_() && IsGimmickRunning() && !IsNowHeavyShakeH() && m_fHeavyTime <= 0.0f) {
            // The heavy phase starts as team 0's raft swings back or team
            // 1's reaches the middle.
            int bStart = 0;
            if (turnH != 0 && ((i == 0 && turnH < 0) || (i != 0 && turnH > 0))) {
                bStart = 1;
            }
            if (bStart) {
                // Keep the rafts' speed and their place in the swing.
                float prevWidth = GetParam_FloatWidth();
                m_bHeavyShakeH = 1;
                float prevCycle = cycleH;
                cycleH = GetParam_WidthCycle();
                float newWidth = GetParam_FloatWidth();
                if (prevCycle > 0.0f && cycleH > 0.0f && newWidth > 0.0f) {
                    m_fFloatSpeedH = (prevWidth / prevCycle) / (newWidth / cycleH);
                }
                if (turnH < 0) {
                    if (prevCycle != 0.0f) {
                        m_fFloatTimeH /= prevCycle;
                    }
                } else {
                    m_fFloatTimeH -= 0.5f * prevCycle;
                    if (prevCycle != 0.0f) {
                        m_fFloatTimeH /= prevCycle;
                    }
                    m_fFloatTimeH = 0.5f * cycleH + m_fFloatTimeH * cycleH;
                }
            }
        }
        offset.x = nw4r::math::SinFIdx(NW4R_MATH_RAD_TO_FIDX(2.0f * 3.1415927f * (fmodf(m_fFloatTimeH, cycleH) / cycleH))) * width;
        if (i != 0) {
            offset.x *= -1.0f;
        }

        // The turrets fire a little before the rafts reach the ends.
        float early = VOLLEY_PRM_S05_TURRET_EARLY_HIT;
        if (i == 0) {
            float hitTime = 0.25f * cycleH - early;
            if (prevTimeH < hitTime && m_fFloatTimeH >= hitTime) {
                Turret_Hit(1);
            }
        }
        if (i == 1) {
            float hitTime = 0.75f * cycleH - early;
            if (prevTimeH < hitTime && m_fFloatTimeH >= hitTime) {
                Turret_Hit(0);
            }
        }

        // Back and forth: a cosine wave, from the rest position out to the
        // full depth and back.
        float cycleV = GetParam_DepthCycle();
        float depth = 0.5f * GetParam_FloatDepth();
        while (m_fFloatTimeV >= cycleV) {
            m_fFloatTimeV -= cycleV;
        }
        long turnV = 0;
        if (prevTimeV > m_fFloatTimeV) {
            turnV = -1;
        } else if (prevTimeV < 0.5f * cycleV && m_fFloatTimeV >= 0.5f * cycleV) {
            // The middle of this swing is not a turn.
        }
        if (IsReadyFloorFlow_() && IsGimmickRunning() && !IsNowHeavyShakeV() && IsNowHeavyShakeH()) {
            // The back and forth swing follows the side to side one into
            // the heavy phase at its next start.
            int bStart = 0;
            if (turnV != 0) {
                bStart = 1;
            }
            if (bStart) {
                float prevDepth = GetParam_FloatDepth();
                m_bHeavyShakeV = 1;
                float prevCycle = cycleV;
                cycleV = GetParam_DepthCycle();
                float newDepth = 0.5f * GetParam_FloatDepth();
                if (prevCycle > 0.0f && cycleV > 0.0f && newDepth > 0.0f) {
                    m_fFloatSpeedV = (prevDepth / prevCycle) / (newDepth / cycleV);
                }
                if (turnV < 0) {
                    if (prevCycle != 0.0f) {
                        m_fFloatTimeV /= prevCycle;
                    }
                } else {
                    m_fFloatTimeV -= 0.5f * prevCycle;
                    if (prevCycle != 0.0f) {
                        m_fFloatTimeV /= prevCycle;
                    }
                    m_fFloatTimeV = 0.5f * cycleV + m_fFloatTimeV * cycleV;
                }
            }
        }
        offset.z = (nw4r::math::CosFIdx(NW4R_MATH_RAD_TO_FIDX(2.0f * 3.1415927f * (fmodf(m_fFloatTimeV, cycleV) / cycleV))) - 1.0f) * depth;
        if (i == 0) {
            offset.z *= -1.0f;
        }

        SetTeamCourtOffset(i, offset);

        bcs::FVEC3 prevFlow = m_aFloorFlow[i];
        bcs::FVEC3Sub(&m_aFloorFlow[i], &offset, &prev);

        // The raft sound starts when team 0's raft turns round and when a
        // raft starts to move.
        float flowX = m_aFloorFlow[i].x;
        float prevX = prevFlow.x;
        if ((i == 0 && flowX * prevX < 0.0f) || (__fabsf(prevX) < 0.001f && __fabsf(flowX) >= 0.001f)) {
            SE_Play(SE_SLOT_RAFT, SE_RAFT, 0.0f, 0);
        }

        // The raft's model follows the court.
        if (m_aRaftMdl[i].IsValid()) {
            bcs::CScnObj *pObj = (*m_aRaftMdl[i].Get())->GetScnObj();
            if (pObj != 0) {
                pObj->m_vTrans.Set(GetTeamCourtOffsetX(i), 0.0f, GetTeamCourtOffsetZ(i));
                pObj->CalcRootMtx();
            }
        }
    }

    Turret_Update(frame);

    CStageManVolley::Main_(frame);
}

int msw::CStageVolley05::IsNowHeavyShakeH() const
{
    return m_bHeavyShakeH;
}

int msw::CStageVolley05::IsNowHeavyShakeV() const
{
    return m_bHeavyShakeV;
}

void msw::CStageVolley05::GetAssumedTeamCourtOffset(float frame, long team, bcs::FVEC3 &rOffset)
{
    static_cast<CStageVolley05 *>(CStageMan::GetInstance())->GetAssumedTeamCourtOffset_(frame, team, rOffset);
}

// The court offsets of Main_, `frame` frames ahead (without the heavy
// phase's changes).
void msw::CStageVolley05::GetAssumedTeamCourtOffset_(float frame, long team, bcs::FVEC3 &rOffset) const
{
    rOffset.Set(0.0f, 0.0f, 0.0f);

    float timeH = m_fFloatTimeH + frame;
    float cycleH = GetParam_WidthCycle();
    float width = GetParam_FloatWidth();
    while (timeH >= cycleH) {
        timeH -= cycleH;
    }
    rOffset.x = nw4r::math::SinFIdx(NW4R_MATH_RAD_TO_FIDX(2.0f * 3.1415927f * (fmodf(timeH, cycleH) / cycleH))) * width;
    if (team != 0) {
        rOffset.x *= -1.0f;
    }

    float timeV = m_fFloatTimeV + frame;
    float cycleV = GetParam_DepthCycle();
    float depth = 0.5f * GetParam_FloatDepth();
    rOffset.z = (nw4r::math::CosFIdx(NW4R_MATH_RAD_TO_FIDX(2.0f * 3.1415927f * (fmodf(timeV, cycleV) / cycleV))) - 1.0f) * depth;
    if (team == 0) {
        rOffset.z *= -1.0f;
    }
}

// In the heavy phase a turret breaks when its raft swings out to the left
// end.
void msw::CStageVolley05::Turret_Update(float)
{
    for (long i = 0; i < RAFT_NUM; i++) {
        TURRET &rTurret = m_aTurret[i];
        CStageNodeAnm *pAnm = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetAnm() : 0;
        if (!rTurret.m_bBroken) {
            if (pAnm != 0 && pAnm->IsEnd(1)) {
                pAnm->Play(TURRET_ANM_IDLE, 2, 1.0f, 0.0f, 0.0f, -1);
            }
            float limit = VOLLEY_PRM_S05_FLOAT_WIDTH + GetRaftWidth_();
            if (IsNowHeavyShakeH()) {
                if ((i == 1 && GetRaftEdgeLeft_(1) <= -limit) || (i == 0 && GetRaftEdgeLeft_(0) <= -limit)) {
                    if (pAnm != 0) {
                        pAnm->Play(TURRET_ANM_BREAK, 0, 1.0f, 0.0f, 0.0f, -1);
                    }
                    rTurret.m_bBroken = 1;
                    long eff = i == 0 ? EFF_TURRET_BREAK_0 : EFF_TURRET_BREAK_1;
                    CEffectCtrl::CreateAttachEffectAuto(eff, static_cast<bcs::CMdlObj *>(rTurret.m_pNode->GetObj()),
                                                        3);
                    CGameSnd::Play(SE_TURRET_BREAK, rTurret.m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
                    CGameSnd::Play(SE_TURRET_BREAK2, rTurret.m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
                }
            }
        } else {
            if (pAnm != 0 && pAnm->IsEnd(1)) {
                pAnm->Play(TURRET_ANM_BROKEN, 2, 1.0f, 0.0f, 0.0f, -1);
            }
        }
    }
}

void msw::CStageVolley05::Turret_SetPos()
{
    for (long i = 0; i < RAFT_NUM; i++) {
        TURRET &rTurret = m_aTurret[i];
        bcs::CScnObj *pObj = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetObj() : 0;
        if (pObj != 0) {
            bcs::FVEC3 pos = pObj->m_vTrans;
            float x = VOLLEY_PRM_S05_FLOAT_WIDTH + GetRaftWidth_() + GetParam_TurretRadius();
            x *= -1.0f;
            pos.x = x;
            rTurret.m_vPos = pos;
            pObj->m_vTrans = pos;
            pObj->CalcRootMtx();
        }
    }
}

void msw::CStageVolley05::Turret_Reset(int bBroken)
{
    for (long i = 0; i < RAFT_NUM; i++) {
        TURRET &rTurret = m_aTurret[i];
        rTurret.m_bBroken = bBroken;
        long anm = bBroken ? TURRET_ANM_BROKEN : TURRET_ANM_IDLE;
        CStageNodeAnm *pAnm = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetAnm() : 0;
        if (pAnm != 0) {
            pAnm->Play(anm, 2, 1.0f, 0.0f, 0.0f, -1);
        }
    }
}

void msw::CStageVolley05::Turret_Hit(long team)
{
    TURRET &rTurret = m_aTurret[team];
    if (rTurret.m_bBroken) {
        return;
    }

    CStageNodeAnm *pAnm = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetAnm() : 0;
    if (pAnm != 0) {
        pAnm->Play(TURRET_ANM_HIT, 0, 1.0f, 0.0f, 0.0f, -1);
    }
    long eff = team == 0 ? EFF_TURRET_HIT_0 : EFF_TURRET_HIT_1;
    CEffectCtrl::CreateAttachEffectAuto(eff, static_cast<bcs::CMdlObj *>(rTurret.m_pNode->GetObj()), 3);
    CGameSnd::Play(SE_TURRET_HIT, rTurret.m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
    CGameSnd::Play(SE_TURRET_HIT2, rTurret.m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
}

// 2 on a court or outer field (area bits 0xF0F), 8 elsewhere (the water;
// meanings of the values not known).
long msw::CStageVolley05::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return GetAreaFlagByPos(rPos, 0xF0F) ? 2 : 8;
}

// What stands on a raft (area bit 12 + i) moves with it.
void msw::CStageVolley05::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    unsigned long area = GetAreaFlagByPos(rPos, 0xF000);
    for (long i = 0; i < RAFT_NUM; i++) {
        if (area & (1 << (i + 12))) {
            rFlow = m_aFloorFlow[i];
            return;
        }
    }
    rFlow.Set(0.0f, 0.0f, 0.0f);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley05::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageMan.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_wavelog_([0-9]+)", 1, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_yagura_([0-9]+)$", 0x64, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageVolley05::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId != 1) {
        return;
    }

    long no;
    if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no >= 0 && no < RAFT_NUM) {
        CMdlPtrPool::Handle &rMdl = m_aRaftMdl[no];
        // The original clears the handle before copying the new one in, as
        // CStageDodge05 does.
        rMdl = CMdlPtrPool::Handle();
        rMdl = rHandle;
    }
}

void msw::CStageVolley05::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    if (pRule->m_nId == 0x64) {
        long no;
        if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no < RAFT_NUM) {
            m_aTurret[no].m_pNode = pNode;
        }
    }
}

void msw::CStageVolley05::OnSceneEntry()
{
    Turret_SetPos();
}

void msw::CStageVolley05::OnGameInit()
{
    m_bHeavyShakeH = 0;
    m_bHeavyShakeV = 0;
    m_fHeavyTime = 60.0f * VOLLEY_PRM_S05_HEAVY_TIME_SEC;
    Turret_Reset(0);
}

void msw::CStageVolley05::OnEntranceEventStart()
{
    m_bHeavyShakeH = 0;
    m_bHeavyShakeV = 0;
    m_fHeavyTime = 60.0f * VOLLEY_PRM_S05_HEAVY_TIME_SEC;
    m_fFloatSpeedH = 0.0f;
    m_fFloatSpeedV = 0.0f;

    // The rafts' models go back to their places.
    for (long i = 0; i < RAFT_NUM; i++) {
        if (m_aRaftMdl[i].IsValid()) {
            bcs::CScnObj *pObj = (*m_aRaftMdl[i].Get())->GetScnObj();
            if (pObj != 0) {
                pObj->m_vTrans.Set(0.0f, 0.0f, 0.0f);
                pObj->CalcRootMtx();
            }
        }
    }
    Turret_Reset(0);
}

void msw::CStageVolley05::OnVictoryEventStart()
{
    m_bHeavyShakeH = 0;
    m_bHeavyShakeV = 0;
    m_fHeavyTime = 60.0f * VOLLEY_PRM_S05_HEAVY_TIME_SEC;
    m_fFloatSpeedH = 0.0f;
    m_fFloatSpeedV = 0.0f;

    for (long i = 0; i < RAFT_NUM; i++) {
        if (m_aRaftMdl[i].IsValid()) {
            bcs::CScnObj *pObj = (*m_aRaftMdl[i].Get())->GetScnObj();
            if (pObj != 0) {
                pObj->m_vTrans.Set(0.0f, 0.0f, 0.0f);
                pObj->CalcRootMtx();
            }
        }
    }
}

// The side to side swing starts at a random end; the rafts stand still
// until Main_ starts them again.
void msw::CStageVolley05::OnGimmickReset()
{
    float rnd = bcs::BCS_RandF();
    float cycleH = GetParam_WidthCycle();
    m_fFloatTimeH = rnd < 0.5f ? 0.0f : 0.5f * cycleH;
    // (the depth cycle is read and not used)
    GetParam_DepthCycle();
    m_fFloatTimeV = 0.0f;
    m_fFloatSpeedH = 0.0f;
    m_fFloatSpeedV = 0.0f;
    m_bFloatReady = 0;

    for (long i = 0; i < RAFT_NUM; i++) {
        SetTeamCourtOffset(i, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }
    Turret_Reset(IsNowHeavyShakeH());
}

void msw::CStageVolley05::OnGimmickCancel()
{
    float rnd = bcs::BCS_RandF();
    float cycleH = GetParam_WidthCycle();
    m_fFloatTimeH = rnd < 0.5f ? 0.0f : 0.5f * cycleH;
    GetParam_DepthCycle();
    m_fFloatTimeV = 0.0f;
    m_fFloatSpeedH = 0.0f;
    m_fFloatSpeedV = 0.0f;
    m_bFloatReady = 0;

    for (long i = 0; i < RAFT_NUM; i++) {
        SetTeamCourtOffset(i, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }
    Turret_Reset(IsNowHeavyShakeH());
}

void msw::CStageVolley05::OnPeriodInit()
{
    m_bHeavyShakeH = 0;
    m_bHeavyShakeV = 0;
    m_fHeavyTime = 60.0f * VOLLEY_PRM_S05_HEAVY_TIME_SEC;
}

void msw::CStageVolley05::OnGameEnd()
{
    m_bFloatReady = 0;
}

int msw::CStageVolley05::IsReadyFloorFlow_() const
{
    if (IsGimmickRunning()) {
        return m_bFloatReady;
    }
    return 0;
}

// Event 0 starts the stage's sounds; 1 to 5 make the crowd cheer.
void msw::CStageVolley05::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_WATER)) {
                SE_PlayEx(SE_SLOT_WATER, SE_WATER, 3, 1, -1, 0.0f, 0.5f, 0.0f);
            }
            if (!SE_IsAlive(SE_SLOT_ENV)) {
                SE_RepeatPlay(SE_SLOT_ENV, SE_ENV, 60.0f * VOLLEY_PRM_S05_ENV_SE_TIME,
                              60.0f * VOLLEY_PRM_S05_ENV_SE_TIME_RND, -1.0f, 0.0f, 12);
            }
            if (!SE_IsAlive(SE_SLOT_WAVE)) {
                SE_Play(SE_SLOT_WAVE, SE_WAVE, 0.0f, 3);
            }
            if (!SE_IsAlive(SE_SLOT_ENV2)) {
                SE_RepeatPlay(SE_SLOT_ENV2, SE_ENV2, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            SE_Play(SE_SLOT_CROWD, SE_CROWD, 0.0f, 0);
            break;
    }
}

float msw::CStageVolley05::GetParam_FloatDepth() const
{
    if (IsNowHeavyShakeV()) {
        return VOLLEY_PRM_S05_HEAVY_FLOAT_DEPTH;
    }
    return VOLLEY_PRM_S05_FLOAT_DEPTH;
}

float msw::CStageVolley05::GetParam_DepthCycle() const
{
    if (IsNowHeavyShakeV()) {
        return 60.0f * VOLLEY_PRM_S05_HEAVY_DEPTH_CYCLE;
    }
    return 60.0f * VOLLEY_PRM_S05_DEPTH_CYCLE;
}

float msw::CStageVolley05::GetParam_FloatWidth() const
{
    if (IsNowHeavyShakeH()) {
        return VOLLEY_PRM_S05_HEAVY_FLOAT_WIDTH;
    }
    return VOLLEY_PRM_S05_FLOAT_WIDTH;
}

float msw::CStageVolley05::GetParam_WidthCycle() const
{
    if (IsNowHeavyShakeH()) {
        return 60.0f * VOLLEY_PRM_S05_HEAVY_WIDTH_CYCLE;
    }
    return 60.0f * VOLLEY_PRM_S05_WIDTH_CYCLE;
}

float msw::CStageVolley05::GetParam_TurretRadius() const
{
    return 1.85f;
}

void msw::CStageVolley05::Disp3D(long)
{
}

long msw::CStageVolley05::SE_GetSlotNum() const
{
    return SE_SLOT_NUM;
}
