// msw::CStageBasket05: basket stage 05, two floating rafts with a turret
// each. DodgeF and VolleyF have the same stage (CStageDodge05,
// CStageVolley05) on their own stage manager; the basket-specific parts are
// the base class, the three-point centres and the BASKET_PRM_S05_* values.
#include "game/msw/CStageBasket05.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
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
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_START_TIME_SEC, 0x00 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_FLOAT_DEPTH, 0x04 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_DEPTH_CYCLE, 0x08 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_FLOAT_STOP_TIME, 0x0C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_HEAVY_TIME_SEC, 0x10 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_HEAVY_FLOAT_DEPTH, 0x14 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_HEAVY_DEPTH_CYCLE, 0x18 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_HEAVY_FLOAT_STOP_TIME, 0x1C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_HEAVY_ACC_TIME_SEC, 0x20 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S05_TURRET_EARLY_HIT, 0x24 },
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

// (name guessed) Bit 0 of the game's flags (meaning not known).
static inline bool IsGameFlag0()
{
    return msw::CGame::g_GameWork.m_nFlag18 & 1;
}

// rOut = r1 + r2.
static inline void AddVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
{
    bcs::FVEC3Add(&rOut, &r1, &r2);
}

const void *msw::CStageBasket05::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket05::CStageBasket05(bcs::CTask *pParent)
    : CStageManBasket(pParent), m_fFloatTime(0.0f), m_bFloatReady(0), m_bHeavyShake(0), m_nHeavyWait(0),
      m_fHeavyTime(0.0f), m_nFixTeam(-1)
{
    for (long i = 0; i < RAFT_NUM; i++) {
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
        m_a3PointPos[i].Set(0.0f, 0.0f, 0.0f);
    }
    for (long i = 0; i < RAFT_NUM; i++) {
        m_aTurret[i].m_pNode = 0;
        m_aTurret[i].m_vPos.Set(0.0f, 0.0f, 0.0f);
    }
}

msw::CStageBasket05::~CStageBasket05()
{
}

float msw::CStageBasket05::GetBaseOuterWidth_() const
{
    return GetBaseGroundWidth_();
}

float msw::CStageBasket05::GetBaseOuterDepth_() const
{
    return GetBaseGroundDepth_();
}

float msw::CStageBasket05::GetRaftDepth_() const
{
    return 15.0f;
}

float msw::CStageBasket05::GetRaftEdgeFront_() const
{
    return GetTeamCourtOffsetZ(1) - GetRaftDepth_();
}

float msw::CStageBasket05::GetRaftEdgeBack_() const
{
    return GetTeamCourtOffsetZ(0) + GetRaftDepth_();
}

// The court ends at the outer edges of the rafts, but not beyond the
// court's own length.
float msw::CStageBasket05::GetCourtFront() const
{
    float offset = GetTeamCourtOffsetZ(1);
    float depth = GetBaseCourtDepth_();
    float front = offset - GetRaftDepth_();

    if (-depth > front) {
        front = -depth;
    }
    return front;
}

float msw::CStageBasket05::GetCourtBack() const
{
    float offset = GetTeamCourtOffsetZ(0);
    float depth = GetBaseCourtDepth_();
    float back = offset + GetRaftDepth_();

    if (depth < back) {
        back = depth;
    }
    return back;
}

// Team 0 plays on the back raft, team 1 on the front one; the centre line
// moves with the rafts.
float msw::CStageBasket05::GetTeamCourtFront(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    if (team == 0) {
        return offset;
    }

    float depth = GetBaseCourtDepth_();
    float front = offset - GetRaftDepth_();
    if (-depth > front) {
        front = -depth;
    }
    return front;
}

float msw::CStageBasket05::GetTeamCourtBack(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    if (team == 0) {
        float depth = GetBaseCourtDepth_();
        float back = offset + GetRaftDepth_();
        if (depth < back) {
            back = depth;
        }
        return back;
    }
    return offset;
}

float msw::CStageBasket05::GetCourtInnerAreaFront() const
{
    return -GetBaseCourtDepth_();
}

float msw::CStageBasket05::GetCourtInnerAreaBack() const
{
    return GetBaseCourtDepth_();
}

float msw::CStageBasket05::GetTeamCourtInnerAreaFront(long team) const
{
    if (team == 0) {
        return GetTeamCourtFront(team);
    }
    return -GetBaseCourtDepth_();
}

float msw::CStageBasket05::GetTeamCourtInnerAreaBack(long team) const
{
    if (team == 0) {
        return GetBaseCourtDepth_();
    }
    return GetTeamCourtBack(team);
}

float msw::CStageBasket05::GetGroundFront() const
{
    return -(GetBaseCourtDepth_() + GetBaseGroundDepth_());
}

float msw::CStageBasket05::GetGroundBack() const
{
    return GetBaseCourtDepth_() + GetBaseGroundDepth_();
}

float msw::CStageBasket05::GetTeamGroundFront(long team) const
{
    if (team == 0) {
        return 0.0f;
    }
    return -(GetBaseCourtDepth_() + GetBaseGroundDepth_());
}

float msw::CStageBasket05::GetTeamGroundBack(long team) const
{
    if (team == 0) {
        return GetBaseCourtDepth_() + GetBaseGroundDepth_();
    }
    return 0.0f;
}

float msw::CStageBasket05::GetOuterFieldFront() const
{
    return -GetBaseOuterDepth_();
}

float msw::CStageBasket05::GetOuterFieldBack() const
{
    return GetBaseOuterDepth_();
}

float msw::CStageBasket05::GetTeamOuterFieldFront(long team, long) const
{
    if (team == 0) {
        return -GetBaseOuterDepth_();
    }
    return 0.0f;
}

float msw::CStageBasket05::GetTeamOuterFieldBack(long team, long) const
{
    if (team == 0) {
        return 0.0f;
    }
    return GetBaseOuterDepth_();
}

// The hole runs from the outer edge of team's raft to the end of the
// ground, across the width of team's court.
bool msw::CStageBasket05::GetHoleRect(long team, float *pLeft, float *pRight, float *pFront, float *pBack) const
{
    float left;
    float right;
    float front;
    float back;

    if (team == 0) {
        GetTeamCourtRect(team, &left, &right, 0, &front);
        back = GetGroundBack();
        if (front >= back) {
            return false;
        }
    } else {
        GetTeamCourtRect(team, &left, &right, &back, 0);
        front = GetGroundFront();
        if (front >= back) {
            return false;
        }
    }

    if (pLeft != 0) {
        *pLeft = left;
    }
    if (pRight != 0) {
        *pRight = right;
    }
    if (pFront != 0) {
        *pFront = front;
    }
    if (pBack != 0) {
        *pBack = back;
    }
    return true;
}

void msw::CStageBasket05::Main_(float frame)
{
    Turret_SetPos();

    if (!IsReadyFloorFlow_() && !IsGameFlag0()) {
        m_bFloatReady = 1;
    }

    float prevTime = m_fFloatTime;

    if (IsReadyFloorFlow_() && m_nFixTeam < 0) {
        // The rafts speed up at the start and again when the heavy phase
        // begins.
        m_fFloatTime += frame * m_fFloatSpeed;
        float accTime;
        if (IsNowHeavyShake()) {
            accTime = 60.0f * BASKET_PRM_S05_START_TIME_SEC;
        } else {
            accTime = 60.0f * BASKET_PRM_S05_HEAVY_ACC_TIME_SEC;
        }
        if (m_fFloatSpeed < 1.0f && accTime > 0.0f) {
            m_fFloatSpeed += 1.0f / accTime;
            m_fFloatSpeed = m_fFloatSpeed > 1.0f ? 1.0f : m_fFloatSpeed < 0.0f ? 0.0f : m_fFloatSpeed;
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

    // One period: a quarter of a wave, a stop, half a wave, a stop, the
    // last quarter. Times are in frames.
    float cycle = GetParam_DepthCycle();
    float stop = GetParam_FloatStopTime();
    float period = 2.0f * stop + cycle;

    while (m_fFloatTime >= period) {
        m_fFloatTime -= period;
    }

    // -1 when the period started again, 1 at its middle: the two turns of
    // the rafts.
    long turn = 0;
    if (prevTime > m_fFloatTime) {
        turn = -1;
    } else if (prevTime < 0.5f * period && m_fFloatTime >= 0.5f * period) {
        turn = 1;
    }

    if (IsReadyFloorFlow_() && IsGimmickRunning() && !IsNowHeavyShake() && m_fHeavyTime <= 0.0f) {
        // The heavy phase starts on a turn towards the goal the camera
        // looks at, or on the second turn.
        int bStart = 0;
        if (turn != 0) {
            if (CGame::g_GameWork.m_Unk151 == 0) {
                CGameCamera *pCamera = CGameCamera::GetInstance(0);
                if (pCamera != 0 && pCamera->GetTargetGoalIdx() == 0 && turn < 0) {
                    bStart = 1;
                } else if (pCamera != 0 && pCamera->GetTargetGoalIdx() == 1 && turn > 0) {
                    bStart = 1;
                } else if (m_nHeavyWait > 0) {
                    bStart = 1;
                } else {
                    m_nHeavyWait++;
                }
            } else {
                bStart = 1;
            }
        }

        if (bStart) {
            // Keep the rafts' speed and their place in the period.
            float prevDepth = GetParam_FloatDepth();
            m_bHeavyShake = 1;
            float prevCycle = cycle;
            cycle = GetParam_DepthCycle();
            stop = GetParam_FloatStopTime();
            float newPeriod = 2.0f * stop + cycle;
            float depth = GetParam_FloatDepth();

            if (prevCycle > 0.0f && cycle > 0.0f && depth > 0.0f) {
                m_fFloatSpeed = (prevDepth / prevCycle) / (depth / cycle);
            }
            if (turn < 0) {
                if (period != 0.0f) {
                    m_fFloatTime /= period;
                }
            } else {
                m_fFloatTime -= 0.5f * period;
                if (period != 0.0f) {
                    m_fFloatTime /= period;
                }
                m_fFloatTime = 0.5f * newPeriod + m_fFloatTime * newPeriod;
            }
        }
    }

    for (long i = 0; i < RAFT_NUM; i++) {
        bcs::FVEC3 prev;
        bcs::FVEC3 offset;
        float rate;

        offset = GetTeamCourtOffset(i);
        prev = offset;

        // Not used. (kept for the call the original makes)
        fmodf(m_fFloatTime, cycle);

        if (m_fFloatTime < 0.25f * cycle) {
            rate = fmodf(m_fFloatTime, cycle) / cycle;
        } else if (m_fFloatTime >= 0.25f * cycle && m_fFloatTime < 0.25f * cycle + stop) {
            rate = 0.25f;
        } else if (m_fFloatTime - stop < 0.75f * cycle) {
            rate = fmodf(m_fFloatTime - stop, cycle) / cycle;
        } else if (m_fFloatTime - stop >= 0.75f * cycle && m_fFloatTime < 0.75f * cycle + 2.0f * stop) {
            rate = 0.75f;
        } else {
            rate = fmodf(m_fFloatTime - 2.0f * stop, cycle) / cycle;
        }

        float wave = nw4r::math::SinRad(2.0f * 3.14159265f * rate);
        offset.z = -wave * GetParam_FloatDepth();

        // Fixed: both rafts as far as they go towards that team's goal.
        if (m_nFixTeam >= 0) {
            long goal = CGame::GetTargetGoalIdx(m_nFixTeam);
            float depth = GetParam_FloatDepth();
            offset.z = goal == 0 ? depth : -depth;
        }

        // The turrets fire a little before the rafts reach the ends.
        if (m_nFixTeam < 0) {
            float early = BASKET_PRM_S05_TURRET_EARLY_HIT;
            if (i == 0) {
                float hitTime = 0.25f * cycle - early;
                if (prevTime < hitTime && m_fFloatTime >= hitTime) {
                    Turret_Hit(0);
                }
            }
            if (i == 1) {
                float hitTime = 0.75f * cycle - early;
                if (prevTime - stop < hitTime && m_fFloatTime - stop >= hitTime) {
                    Turret_Hit(1);
                }
            }
        }

        SetTeamCourtOffset(i, offset);
        bcs::FVEC3 prevFlow = m_aFloorFlow[i];
        bcs::FVEC3Sub(&m_aFloorFlow[i], &offset, &prev);

        // The raft sound starts when the rafts turn round and when they
        // leave a stop.
        float flowZ = m_aFloorFlow[i].z;
        float prevZ = prevFlow.z;
        if ((i == 0 && flowZ * prevZ < 0.0f) || (__fabsf(prevZ) < 0.001f && __fabsf(flowZ) >= 0.001f)) {
            SE_Play(5, 0xD26, 0.0f, 0);
        }

        // The raft's model follows the court; it is scaled to the raft's
        // length (the model is 15 units long).
        if (m_aRaftMdl[i].IsValid()) {
            bcs::CScnObj *pObj = (*m_aRaftMdl[i].Get())->GetScnObj();
            if (pObj != 0) {
                pObj->m_vTrans.Set(GetTeamCourtOffsetX(i), 0.0f, GetTeamCourtOffsetZ(i));
                pObj->m_vScale.Set(1.0f, 1.0f, GetRaftDepth_() / 15.0f);
                pObj->CalcRootMtx();
            }
        }

        // The three-point line moves with the court.
        AddVec(m_a3PointPos[i], GetGoalPos(i), GetTeamCourtOffset(i));
    }

    Turret_Update(frame);
    CStageManBasket::Main_(frame);
}

int msw::CStageBasket05::IsNowHeavyShake() const
{
    return m_bHeavyShake;
}

// In the heavy phase a turret breaks when its raft swings out to the end.
void msw::CStageBasket05::Turret_Update(float)
{
    for (long i = 0; i < RAFT_NUM; i++) {
        TURRET &rTurret = m_aTurret[i];
        CStageNodeAnm *pAnm = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetAnm() : 0;

        if (!rTurret.m_bBroken) {
            if (pAnm != 0 && pAnm->IsEnd(1)) {
                pAnm->Play(TURRET_ANM_IDLE, 2, 1.0f, 0.0f, 0.0f, -1);
            }

            float limit = BASKET_PRM_S05_FLOAT_DEPTH + GetRaftDepth_();
            if (IsNowHeavyShake()) {
                if ((i == 0 && GetRaftEdgeFront_() <= -limit) || (i == 1 && GetRaftEdgeBack_() >= limit)) {
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

void msw::CStageBasket05::Turret_SetPos()
{
    for (long i = 0; i < RAFT_NUM; i++) {
        TURRET &rTurret = m_aTurret[i];
        bcs::CScnObj *pObj = rTurret.m_pNode != 0 ? rTurret.m_pNode->GetObj() : 0;

        if (pObj != 0) {
            bcs::FVEC3 pos = pObj->m_vTrans;
            float z = BASKET_PRM_S05_FLOAT_DEPTH + GetRaftDepth_() + GetParam_TurretRadius();
            if (i == 0) {
                z *= -1.0f;
            }
            pos.z = z;
            rTurret.m_vPos = pos;
            pObj->m_vTrans = pos;
            pObj->CalcRootMtx();
        }
    }
}

void msw::CStageBasket05::Turret_Reset(int bBroken)
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

void msw::CStageBasket05::Turret_Hit(long team)
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

// 8 over the holes, 2 elsewhere (meanings of the values not known).
long msw::CStageBasket05::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE) const
{
    for (long i = 0; i < RAFT_NUM; i++) {
        float left;
        float right;
        float front;
        float back;

        if (GetHoleRect(i, &left, &right, &front, &back)) {
            if (front < 0.0f) {
                if (rPos.z < back) {
                    return 8;
                }
            } else if (front < rPos.z) {
                return 8;
            }
        }
    }
    return 2;
}

// What stands on a raft moves with it (both rafts move alike).
void msw::CStageBasket05::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE) const
{
    if (m_nFixTeam >= 0) {
        rFlow.Set(0.0f, 0.0f, 0.0f);
        return;
    }

    float front = GetRaftEdgeFront_();
    float back = GetRaftEdgeBack_();
    if (front <= rPos.z && rPos.z <= back) {
        rFlow = m_aFloorFlow[0];
    } else {
        rFlow.Set(0.0f, 0.0f, 0.0f);
    }
}

// Message 0x27 with 3 fixes the rafts towards the goal of team arg2 (-1
// lets them go).
void msw::CStageBasket05::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManBasket::SendMessage(msg, arg1, arg2, arg3);
    if (msg == 0x27 && arg1 == 3) {
        m_nFixTeam = arg2;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket05::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^null_wavelog_([0-9]+)$", 1, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_bridge_([0-9]+)$", 0x14, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_yagura_([0-9]+)$", 0x64, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasket05::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    if (pRule->m_nId != 1) {
        return;
    }

    long no;
    if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no >= 0 && no < RAFT_NUM) {
        CMdlPtrPool::Handle &rMdl = m_aRaftMdl[no];
        // The original clears the handle before copying the new one in, as
        // CStageBasket13 does.
        rMdl = CMdlPtrPool::Handle();
        rMdl = rHandle;
    }
}

void msw::CStageBasket05::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    switch (pRule->m_nId) {
        case 1:
            // The rafts turn with the attacked goal.
            RegistRotNode(pNode, 3.1415927f);
            break;
        case 0x64: {
            long no;
            if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no < RAFT_NUM) {
                m_aTurret[no].m_pNode = pNode;
            }
            break;
        }
        case 0x14:
            if (pNode->GetObj() != 0) {
                pNode->GetObj()->SetDrawPriority(0xC9, 0xC9);
            }
            break;
    }
    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasket05::OnSceneEntry()
{
    Turret_SetPos();
}

inline void msw::CStageBasket05::ResetHeavyShake_()
{
    m_bHeavyShake = 0;
    m_nHeavyWait = 0;
    m_fHeavyTime = 60.0f * BASKET_PRM_S05_HEAVY_TIME_SEC;
}

void msw::CStageBasket05::OnGameInit()
{
    ResetHeavyShake_();
    Turret_Reset(0);
}

void msw::CStageBasket05::OnEntranceEventStart()
{
    ResetHeavyShake_();

    // The rafts' models start in place.
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

// The rafts start at one end of the period or the other.
void msw::CStageBasket05::OnGimmickReset()
{
    float period = GetParam_DepthCycle() + 2.0f * GetParam_FloatStopTime();
    m_fFloatTime = bcs::BCS_RandF() < 0.5f ? 0.0f : 0.5f * period;
    m_fFloatSpeed = 0.0f;
    m_bFloatReady = 0;

    for (long i = 0; i < RAFT_NUM; i++) {
        SetTeamCourtOffset(i, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }
    Turret_Reset(IsNowHeavyShake());
}

void msw::CStageBasket05::OnGimmickCancel()
{
    float period = GetParam_DepthCycle() + 2.0f * GetParam_FloatStopTime();
    m_fFloatTime = bcs::BCS_RandF() < 0.5f ? 0.0f : 0.5f * period;
    m_fFloatSpeed = 0.0f;
    m_bFloatReady = 0;

    for (long i = 0; i < RAFT_NUM; i++) {
        SetTeamCourtOffset(i, bcs::FVEC3(0.0f, 0.0f, 0.0f));
        m_aFloorFlow[i].Set(0.0f, 0.0f, 0.0f);
    }
    Turret_Reset(IsNowHeavyShake());
}

void msw::CStageBasket05::OnPeriodInit()
{
    ResetHeavyShake_();
}

int msw::CStageBasket05::IsReadyFloorFlow_() const
{
    if (GetGimmickStep() == GIMMICK_STOP) {
        return 0;
    }
    if (!IsGimmickRunning()) {
        return 0;
    }
    if (m_nFixTeam >= 0) {
        return 0;
    }
    return m_bFloatReady;
}

void msw::CStageBasket05::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(0)) {
                SE_Play(0, 0xD23, 0.0f, 3);
            }
            if (!SE_IsAlive(1)) {
                SE_RepeatPlay(1, 0xD27, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            if (!SE_IsAlive(2)) {
                SE_Play(2, 0xD2F, 0.0f, 3);
            }
            if (!SE_IsAlive(4)) {
                SE_RepeatPlay(4, 0xD28, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            break;
        case 1:
        case 2:
        case 3:
            SE_Play(3, 0xD30, 0.0f, 0);
            break;
    }
}

float msw::CStageBasket05::GetParam_FloatDepth() const
{
    if (IsNowHeavyShake()) {
        return BASKET_PRM_S05_HEAVY_FLOAT_DEPTH;
    }
    return BASKET_PRM_S05_FLOAT_DEPTH;
}

float msw::CStageBasket05::GetParam_DepthCycle() const
{
    if (IsNowHeavyShake()) {
        return 60.0f * BASKET_PRM_S05_HEAVY_DEPTH_CYCLE;
    }
    return 60.0f * BASKET_PRM_S05_DEPTH_CYCLE;
}

float msw::CStageBasket05::GetParam_FloatStopTime() const
{
    if (IsNowHeavyShake()) {
        return 60.0f * BASKET_PRM_S05_HEAVY_FLOAT_STOP_TIME;
    }
    return 60.0f * BASKET_PRM_S05_FLOAT_STOP_TIME;
}

float msw::CStageBasket05::GetParam_TurretRadius() const
{
    return 1.85f;
}

long msw::CStageBasket05::SE_GetSlotNum() const
{
    return 6;
}

const bcs::FVEC3 &msw::CStageBasket05::Get3PointAreaPos(long goal) const
{
    return m_a3PointPos[goal];
}
