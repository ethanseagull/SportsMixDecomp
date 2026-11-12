// msw::CStage17Base: the gimmicks of a sport's stage 17. Two turntables
// carry the players round; between turns, now and then, the signals warn
// and a trolley crosses the court; trains pass in the background. The
// sport's stage manager derives from this class next to its own stage
// manager, hands its hooks on (Stage17_*) and gives the tuning
// (GetParam_*). Names are guesses unless the DOL exports them.
#include "game/msw/CStage17Base.hpp"

#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgGmk_Trolley.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

namespace msw {

// (name guessed) x limited to [lo, hi]. A macro rather than an inline
// function: the original reads x before hi, which an inline function's
// arguments (read right to left) do not give.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

    // The inline functions of CTurnTable.

    inline void CStage17Base::CTurnTable::GetCenter(bcs::FVEC3 &rPos) const
    {
        rPos.Set(0.0f, 0.0f, 0.0f);
        float offset = m_pOwner->GetParam_TTOffset();
        rPos.z = m_nIdx == 0 ? offset : -offset;
    }

    inline float CStage17Base::CTurnTable::GetRadius() const
    {
        return m_pOwner->GetParam_TTRadius();
    }

    inline float CStage17Base::CTurnTable::GetDist(const bcs::FVEC3 &rPos) const
    {
        bcs::FVEC3 center;
        GetCenter(center);
        return bcs::FVEC3Dist(&rPos, &center);
    }

    inline bool CStage17Base::CTurnTable::IsOn(const bcs::FVEC3 &rPos) const
    {
        return GetDist(rPos) < GetRadius();
    }

    inline float CStage17Base::CTurnTable::GetRot() const
    {
        return bcs::ROUND_PI(m_fRot);
    }

    inline bool CStage17Base::CTurnTable::IsOpen() const
    {
        if (!m_bTurn) {
            // Quarter turns from the base angle; an even count is open.
            long quarter = bcs::ROUND_PI(m_fRot + 0.17453292f) / 1.5707964f;
            if ((quarter & 1) == 0) {
                return true;
            }
        }
        return false;
    }

    inline void CStage17Base::CTurnTable::StopSE()
    {
        if (m_nIdx == 0 && m_nSe != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nSe);
            if (pTask != 0) {
                pTask->Delete();
            }
            m_nSe = 0;
        }
    }

    inline void CStage17Base::CTurnTable::Stop()
    {
        if (m_bTurn) {
            m_bTurn = 0;
            m_fSnapRot = m_fTargetRot - m_fTurnRot;
            m_fTurnRot = m_fTargetRot;
            m_fSpeed = 0.0f;
            m_fMaxSpeed = 0.0f;
            m_bTarget = 1;
            StopSE();
        }
    }

    inline void CStage17Base::CTurnTable::UpdateMdl()
    {
        m_fRot = bcs::ROUND_PI(m_fBaseRot + m_fTurnRot);
        bcs::CScnObj *pObj = m_pObj;
        if (pObj != 0) {
            pObj->SetRotY(m_fRot);
            pObj->CalcRootMtx();
            pObj->CalcWorldMtx(0);
        }
    }

    CStage17Base::CStage17Base()
        : m_ppTurnTable(0),
          m_nTurnTableNum(0),
          m_bSignal(0),
          m_nSignalNum(0),
          m_nState(STATE_WAIT),
          m_pTrolley(0),
          m_nTrainNum(0),
          m_nFixTurnTableTeam(-1)
    {
        for (long i = 0; i < SIGNAL_MAX; i++) {
            m_apSignal[i] = 0;
        }
    }

    CStage17Base::~CStage17Base()
    {
        Stage17_Delete();
    }

    void CStage17Base::Stage17_Create()
    {
        long num = GetTurnTableNum();
        CTurnTable **ppTable = new (1) CTurnTable *[num];
        m_ppTurnTable = ppTable;
        m_nTurnTableNum = num;
        for (long i = 0; i < num; i++) {
            CTurnTable *pTable = NewTurnTable(this, 1, 0x20);
            ppTable[i] = pTable;
            pTable->Assign(i, 0);
        }
        for (long i = 0; i < SIGNAL_MAX; i++) {
            m_apSignal[i] = 0;
        }
        m_nSignalNum = 0;
        GetSeSlot()->SE_Init();
    }

    void CStage17Base::Stage17_Delete()
    {
        if (m_ppTurnTable != 0) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                if (m_ppTurnTable[i] != 0) {
                    delete m_ppTurnTable[i];
                }
                m_ppTurnTable[i] = 0;
            }
            // Checked again, as in the original.
            if (m_ppTurnTable != 0) {
                delete[] m_ppTurnTable;
            }
            m_ppTurnTable = 0;
        }
        m_nTurnTableNum = 0;
    }

    void CStage17Base::Stage17_Main(float frame)
    {
        if (Stage17_IsGimmickReady()) {
            switch (m_nState) {
                case STATE_WAIT:
                    m_fTimer += frame;
                    if (m_fTimer >= m_fTime) {
                        m_nState++;

                        // Turn by a half or a quarter turn, so the tables
                        // stand open or closed.
                        float rot;
                        float rate = GetParam_TTOpenRate();
                        if (rate > 0.0f && rate >= bcs::BCS_RandF()) {
                            rot = IsTurnTableOpen() ? 3.1415927f : 1.5707964f;
                        } else {
                            rot = IsTurnTableOpen() ? 1.5707964f : 3.1415927f;
                        }

                        long dirType = GetParam_TTRotDirType();
                        if (dirType == 2) {
                            dirType = bcs::BCS_RandMax(1L);
                        }
                        float dir = dirType == 0 ? -1.0f : 1.0f;

                        // The two tables turn opposite ways.
                        for (long i = 0; i < m_nTurnTableNum; i++) {
                            if (m_nFixTurnTableTeam >= 0 && i == CGame::GetTargetGoalIdx(m_nFixTurnTableTeam)) {
                                continue;
                            }
                            CTurnTable *pTable = m_ppTurnTable[i];
                            if (pTable != 0) {
                                pTable->Start(rot, dir * ((i & 1) ? -1.0f : 1.0f));
                            }
                        }
                    }
                    break;

                case STATE_TURN: {
                    int bTurn = 0;
                    for (long i = 0; i < m_nTurnTableNum; i++) {
                        if (m_ppTurnTable[i] != 0 && m_ppTurnTable[i]->IsTurn()) {
                            bTurn = 1;
                            break;
                        }
                    }
                    if (!bTurn) {
                        // The trolley only comes when the tables stand open.
                        int bTrolley = IsTurnTableOpen();
                        if (bTrolley) {
                            if (GetParam_TrolleyRate() != 0.0f) {
                                float rnd = bcs::BCS_RandF();
                                if (GetParam_TrolleyRate() < rnd) {
                                    bTrolley = 0;
                                }
                            } else {
                                bTrolley = 0;
                            }
                        }
                        if (bTrolley) {
                            m_nState++;
                            m_fTimer = 0.0f;
                            m_fTime = GetParam_SignalTime();
                        } else {
                            m_nState = STATE_TROLLEY;
                        }
                    }
                    break;
                }

                case STATE_SIGNAL:
                    m_fTimer += frame;
                    if (m_fTimer >= m_fTime) {
                        m_nState++;
                        m_fTimer = 0.0f;
                        m_fTime = GetParam_TrolleyPreWait();
                        SignalStart();
                    }
                    break;

                case STATE_TROLLEY_WAIT:
                    m_fTimer += frame;
                    if (m_fTimer >= m_fTime) {
                        m_nState++;
                        TrolleyActivate();
                        GetSeSlot()->SE_DelayStop(SE_SLOT_SIGNAL, GetParam_SignalAfterTime(),
                                                  GetParam_SignalSEStopTime());
                    }
                    break;

                case STATE_TROLLEY:
                    if (!IsTrolleyActive()) {
                        m_nState = STATE_WAIT;
                        SignalStop();
                        m_fTimer = 0.0f;
                        m_fTime = GetParam_TTWait() + GetParam_TTWaitRnd();
                    }
                    break;
            }
        }

        for (long i = 0; i < m_nTurnTableNum; i++) {
            m_ppTurnTable[i]->Main(frame);
        }
        GetSeSlot()->SE_Update(frame);
    }

    void CStage17Base::Stage17_Reset()
    {
        for (long i = 0; i < m_nTurnTableNum; i++) {
            if (m_ppTurnTable[i] != 0) {
                m_ppTurnTable[i]->Reset();
            }
        }
        SignalReset();
        TrolleyReset();
        ResetTrainAll();

        // Now and then the game starts with the trolley.
        float rate = GetParam_TrolleyInitRate();
        if (rate > 0.0f && rate >= bcs::BCS_RandF()) {
            m_nState = STATE_SIGNAL;
            m_fTimer = 0.0f;
            m_fTime = GetParam_SignalTime();
        } else {
            m_nState = STATE_WAIT;
            m_fTimer = 0.0f;
            m_fTime = GetParam_TTInitialWait() + GetParam_TTWaitRnd();
        }
    }

    bool CStage17Base::IsTrolleyActive() const
    {
        const CStgGmk_Trolley *pTrolley = GetTrolley_();
        if (pTrolley != 0 && pTrolley->IsActive()) {
            return true;
        }
        return false;
    }

    void CStage17Base::TrolleyActivate()
    {
        if (IsTrolleyActive()) {
            return;
        }
        CStgGmk_Trolley *pTrolley = GetTrolley_();
        if (pTrolley == 0) {
            return;
        }

        long dirType = GetParam_TrolleyDirType();
        float dirRot = dirType == 0 ? 0.0f : 3.1415927f;
        float rot = GetParam_TTOpenAngle();
        if (CGame::g_GameWork.m_Unk151 == 0) {
            // Turned round when the camera looks at the other goal.
            CGameCamera *pCamera = CGameCamera::GetInstance(0);
            if (pCamera != 0 && pCamera->GetTargetGoalIdx() != 0) {
                rot += 3.1415927f;
            }
        } else {
            dirType = 2;
        }
        if (dirType == 2) {
            // The random direction is drawn but not used.
            dirType = bcs::BCS_RandRange(0L, 1L);
        }

        pTrolley->SetDefaultRotY(bcs::ROUND_PI(rot + dirRot));
        float offset = GetParam_TrolleyLocOffset();
        float dist = 2.0f * fabsf(offset);
        bcs::FVEC3 pos(0.0f, 0.0f, -offset);
        pTrolley->SetDefaultPos(pos);
        pTrolley->SetRunDist(dist);
        pTrolley->Respawn();
    }

    void CStage17Base::TrolleyReset()
    {
        CStgGmk_Trolley *pTrolley = GetTrolley_();
        if (pTrolley != 0) {
            pTrolley->Inactivate();
        }
    }

    CStage17Base::CTurnTable *CStage17Base::GetTurnTable(long idx)
    {
        if (m_ppTurnTable != 0 && idx >= 0 && idx < m_nTurnTableNum) {
            return m_ppTurnTable[idx];
        }
        return 0;
    }

    const CStage17Base::CTurnTable *CStage17Base::GetTurnTable(long idx) const
    {
        if (m_ppTurnTable != 0 && idx >= 0 && idx < m_nTurnTableNum) {
            return m_ppTurnTable[idx];
        }
        return 0;
    }

    bool CStage17Base::IsTurnTableOpen() const
    {
        for (long i = 0; i < m_nTurnTableNum; i++) {
            const CTurnTable *pTable = m_ppTurnTable[i];
            if (pTable != 0 && !pTable->IsOpen()) {
                return false;
            }
        }
        return true;
    }

    // The flow types 0 to 3 and 6 move with the tables (the original
    // compares the type unsigned).
    long CStage17Base::Stage17_GetFloorAttribute(const bcs::FVEC3 &rPos, CStageMan::FLOW_TYPE type) const
    {
        const CTurnTable *pTable;
        if (type != 5) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                pTable = m_ppTurnTable[i];
                if (pTable != 0 && pTable->GetDist(rPos) < pTable->GetRadius()) {
                    return 2;
                }
            }
        }
        return 0;
    }

    void CStage17Base::Stage17_GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                            CStageMan::FLOW_TYPE type) const
    {
        if ((unsigned long)type <= 3 || type == 6) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                if (m_ppTurnTable[i] != 0 && m_ppTurnTable[i]->IsOn(rPos)) {
                    // How far the point moves as the table turns this frame.
                    bcs::FVEC3 center;
                    m_ppTurnTable[i]->GetCenter(center);
                    // The radius is read and not used.
                    m_ppTurnTable[i]->GetRadius();
                    bcs::FVEC3 local;
                    bcs::FVEC3Sub(&local, &rPos, &center);
                    local.y = 0.0f;
                    float len = bcs::FVEC3Len(&local);

                    nw4r::math::MTX34 mtx;
                    nw4r::math::MTX34RotXYZRad(&mtx, 0.0f,
                                               type == 1 ? m_ppTurnTable[i]->GetFrameRot()
                                                         : m_ppTurnTable[i]->GetFlowRot(),
                                               0.0f);
                    bcs::FVEC3 moved;
                    PSMTXMultVec(mtx, reinterpret_cast<const Vec *>(&local), reinterpret_cast<Vec *>(&moved));
                    bcs::BCS_Vec_Normalize(moved, moved);
                    bcs::FVEC3Scale(&moved, &moved, len);
                    bcs::FVEC3 flow;
                    bcs::FVEC3Sub(&flow, &moved, &local);
                    rFlow = flow;
                    return;
                }
            }
            rFlow.Set(0.0f, 0.0f, 0.0f);
        } else {
            rFlow.Set(0.0f, 0.0f, 0.0f);
        }
    }

    float CStage17Base::Stage17_GetFloorFlowRot(const bcs::FVEC3 &rPos, CStageMan::FLOW_TYPE type) const
    {
        if ((unsigned long)type <= 3 || type == 6) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                if (m_ppTurnTable[i] != 0 && m_ppTurnTable[i]->IsOn(rPos)) {
                    if (type == 1) {
                        return m_ppTurnTable[i]->GetFrameRot();
                    }
                    return m_ppTurnTable[i]->GetFlowRot();
                }
            }
        }
        return 0.0f;
    }

    void CStage17Base::Stage17_CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos,
                                                CStageMan::FLOW_TYPE type) const
    {
        if ((unsigned long)type <= 3 || type == 6) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                if (m_ppTurnTable[i] != 0 && m_ppTurnTable[i]->IsOn(rPos)) {
                    // The point turned with the table to its current angle.
                    bcs::FVEC3 center;
                    m_ppTurnTable[i]->GetCenter(center);
                    // The radius is read and not used.
                    m_ppTurnTable[i]->GetRadius();
                    bcs::FVEC3 local;
                    bcs::FVEC3Sub(&local, &rPos, &center);
                    local.y = 0.0f;
                    float len = bcs::FVEC3Len(&local);

                    nw4r::math::MTX34 mtx;
                    nw4r::math::MTX34RotXYZRad(&mtx, 0.0f, m_ppTurnTable[i]->GetRot(), 0.0f);
                    bcs::FVEC3 moved;
                    PSMTXMultVec(mtx, reinterpret_cast<const Vec *>(&local), reinterpret_cast<Vec *>(&moved));
                    bcs::BCS_Vec_Normalize(moved, moved);
                    bcs::FVEC3Scale(&moved, &moved, len);
                    bcs::FVEC3Add(&rOut, &center, &moved);
                    return;
                }
            }
        }
        rOut = rPos;
    }

    float CStage17Base::Stage17_CalcFloorFlowRot(const bcs::FVEC3 &rPos, CStageMan::FLOW_TYPE type) const
    {
        if ((unsigned long)type <= 3 || type == 6) {
            for (long i = 0; i < m_nTurnTableNum; i++) {
                if (m_ppTurnTable[i] != 0 && m_ppTurnTable[i]->IsOn(rPos)) {
                    return m_ppTurnTable[i]->GetRot();
                }
            }
        }
        return 0.0f;
    }

    void CStage17Base::AddSignalMdl(CStageMdlListNode *pNode)
    {
        if (m_nSignalNum < SIGNAL_MAX) {
            m_apSignal[m_nSignalNum++] = pNode;
        }
    }

    void CStage17Base::SignalStart()
    {
        if (IsSignal()) {
            return;
        }
        m_bSignal = 1;
        for (long i = 0; i < SIGNAL_MAX; i++) {
            CStageNodeAnm *pAnm = m_apSignal[i] != 0 ? m_apSignal[i]->GetAnm() : 0;
            if (pAnm != 0) {
                pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, -1);
            }
        }
        GetSeSlot()->SE_Play(SE_SLOT_SIGNAL, GetParam_SE(SE_SIGNAL), 0.0f, 0);
    }

    void CStage17Base::SignalStop()
    {
        if (!IsSignal()) {
            return;
        }
        for (long i = 0; i < SIGNAL_MAX; i++) {
            CStageNodeAnm *pAnm = m_apSignal[i] != 0 ? m_apSignal[i]->GetAnm() : 0;
            if (pAnm != 0) {
                pAnm->Play(1, 0, 1.0f, 0.0f, 0.0f, -1);
            }
        }
        m_bSignal = 0;
    }

    void CStage17Base::SignalReset()
    {
        SignalStop();
        GetSeSlot()->SE_Stop(SE_SLOT_SIGNAL, 0.0f);
    }

    void CStage17Base::AddTrainMdl(CStageMdlListNode *pNode)
    {
        m_aTrainPosFunc[m_nTrainNum++].SetNode(pNode);
    }

    void CStage17Base::ResetTrainAll()
    {
        for (long i = 0; i < m_nTrainNum; i++) {
            long se = GetParam_SE(SE_TRAIN);
            long seLoop = GetParam_SE(SE_TRAIN_LOOP);
            GetSeSlot()->SE_PlayPos(i, se, 0.0f, 3, &m_aTrainPosFunc[i]);
            GetSeSlot()->SE_RepeatPlayPos(i + 2, seLoop, 180.0f, 420.0f, -1.0f, 0.0f, 12, &m_aTrainPosFunc[i]);
        }
    }

    void CStage17Base::Stage17_OnGimmickReset()
    {
        Stage17_Reset();
    }

    void CStage17Base::Stage17_OnGimmickStart()
    {
    }

    void CStage17Base::Stage17_OnGimmickStop()
    {
        Stage17_Reset();
    }

    void CStage17Base::Stage17_OnGimmickCancel()
    {
        SignalReset();
        TrolleyReset();
        m_nState = STATE_WAIT;
        m_fTimer = 0.0f;
        m_fTime = GetParam_TTWait() + GetParam_TTWaitRnd();
    }

    void CStage17Base::Stage17_OnGimmickPause()
    {
        CTurnTable *pTable;
        for (long i = 0; i < m_nTurnTableNum; i++) {
            pTable = m_ppTurnTable[i];
            if (pTable != 0) {
                pTable->Stop();
            }
        }
    }

    void CStage17Base::Stage17_OnGimmickResume()
    {
    }

    void CStage17Base::Stage17_OnSceneEntry()
    {
        CStgGmk_Trolley *pTrolley = NewTrolley();
        if (pTrolley != 0) {
            pTrolley->Create();
            pTrolley->SceneEntry();
            m_pTrolley = pTrolley;
        }
    }

    void CStage17Base::Stage17_OnTurnInit()
    {
        Stage17_Reset();
    }

    long CStage17Base::GetParam_SE(long type) const
    {
        switch (type) {
            case SE_SIGNAL:
                return 0xD96;
            case SE_TRAIN:
                return 0xD91;
            case SE_TRAIN_LOOP:
                return 0xD92;
            case SE_TURN_STOP:
                return 0xD95;
            case SE_TURN:
                return 0xD94;
        }
        return -1;
    }

    CStage17Base::CTurnTable::CTurnTable(CStage17Base *pOwner)
        : m_pOwner(pOwner),
          m_nIdx(0),
          m_fBaseRot(0.0f),
          m_Unk0C(0.0f),
          m_fRot(0.0f),
          m_Unk14(0.0f),
          m_fTurnRot(0.0f),
          m_fTargetRot(0.0f),
          m_fSpeed(0.0f),
          m_fMaxSpeed(0.0f),
          m_fFlowRot(0.0f),
          m_fFrameRot(0.0f),
          m_fSnapRot(0.0f),
          m_bTurn(0),
          m_fTurnTime(0.0f),
          m_nSe(0)
    {
    }

    CStage17Base::CTurnTable::~CTurnTable()
    {
    }

    void CStage17Base::CTurnTable::Assign(long nIdx, bcs::CScnObj *pObj)
    {
        m_nIdx = nIdx;
        m_pObj = pObj;
    }

    void CStage17Base::CTurnTable::Main(float frame)
    {
        if (!m_bTurn) {
            m_fFlowRot = 0.0f;
        } else {
            m_fTurnTime -= frame;
            if (m_fTurnTime <= 0.0f) {
                // Time to head for the target: take the target half a turn
                // on when it lies behind in the direction of turning.
                m_bTarget = 1;
                float diff = bcs::ROUND_PI(m_fTargetRot - m_fTurnRot);
                if (diff > 3.1415927f) {
                    diff -= 6.2831855f;
                }
                if ((m_fDir > 0.0f && diff < 0.0f) || (m_fDir < 0.0f && diff > 0.0f)) {
                    m_fTargetRot = bcs::ROUND_PI(m_fTargetRot - 3.1415927f);
                }
            }

            float accTime = m_pOwner->GetParam_TTRotAccTime();
            if (accTime > 0.0f) {
                m_fSpeed += frame * (m_fMaxSpeed / accTime);
            } else {
                m_fSpeed = m_fMaxSpeed;
            }
            m_fSpeed = CLAMP(m_fSpeed, 0.0f, m_fMaxSpeed);
            float step = frame * m_fSpeed;
            m_fFlowRot = m_fDir * step;
            m_fTurnRot += m_fFlowRot;
            m_fTurnRot = bcs::ROUND_PI(m_fTurnRot);

            if (m_bTarget) {
                float diff = bcs::ROUND_PI(m_fTargetRot - m_fTurnRot);
                if (diff > 3.1415927f) {
                    diff -= 6.2831855f;
                }
                if (m_bSlow && ((m_fDir > 0.0f && diff < 0.0f) || (m_fDir < 0.0f && diff > 0.0f))) {
                    // Past the target: stop on it.
                    Stop();
                    m_fFlowRot += diff;
                    long effect = m_pOwner->GetParam_Effect(EFFECT_TURN_STOP);
                    if (effect >= 0) {
                        CEffectCtrl::SimpleEffect(effect, m_pObj, 0, 0, 3, 0, 0, 0);
                    }
                    long se = m_pOwner->GetParam_SE(SE_TURN_STOP);
                    if (se >= 0) {
                        CGameSnd::Play(se, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    }
                    CGame::StartCameraQuake(0x18, -1, -1);
                } else if (!m_bSlow && ((m_fDir > 0.0f && diff < 0.08726646f && diff > 0.0f) ||
                                        (m_fDir < 0.0f && diff > -0.08726646f && diff < 0.0f))) {
                    // Within 5 degrees: slow down.
                    m_fMaxSpeed = 0.2f * m_pOwner->GetParam_TTRotSpeed();
                    m_bSlow = 1;
                }
            }
        }

        m_fFrameRot = m_fFlowRot + m_fSnapRot;
        m_fSnapRot = 0.0f;
        UpdateMdl();
    }

    void CStage17Base::CTurnTable::Reset()
    {
        m_fRot = m_fBaseRot;
        m_fTargetRot = 0.0f;
        m_fMaxSpeed = 0.0f;
        m_fTurnRot = 0.0f;
        m_fSpeed = 0.0f;
        m_bTurn = 0;
        m_bTarget = 1;
        m_fFlowRot = 0.0f;
        m_fFrameRot = 0.0f;
        if (m_nIdx == 0 && m_nSe != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nSe);
            if (pTask != 0) {
                pTask->Delete();
            }
            m_nSe = 0;
        }
        m_fRot = bcs::ROUND_PI(m_fBaseRot + m_fTurnRot);
        bcs::CScnObj *pObj = m_pObj;
        if (pObj != 0) {
            pObj->SetRotY(m_fRot);
            pObj->CalcRootMtx();
            pObj->CalcWorldMtx(0);
        }
    }

    long CStage17Base::CTurnTable::Start(float rot, float dir)
    {
        if (m_bTurn) {
            return 0;
        }
        m_fTurnTime = m_pOwner->GetParam_TTRotTime();
        float rnd = m_pOwner->GetParam_TTRotTimeRnd();
        if (rnd > 0.0f) {
            m_fTurnTime += bcs::BCS_RandMax(rnd);
        }
        if (m_fTurnTime <= 0.0f) {
            return 2;
        }

        m_fDir = dir < 0.0f ? -1.0f : 1.0f;
        // A no-op after the line above; the original clamps here too (its
        // compares are in the code), probably a leftover of an earlier
        // signed-speed version.
        m_fDir = CLAMP(m_fDir, -1.0f, 1.0f);
        m_fTargetRot = bcs::ROUND_PI(m_fRot + rot * m_fDir);
        m_fMaxSpeed = m_pOwner->GetParam_TTRotSpeed();
        m_bTurn = 1;
        m_bTarget = 0;
        m_bSlow = 0;

        long effect = m_pOwner->GetParam_Effect(EFFECT_TURN_START);
        if (effect >= 0) {
            CEffectCtrl::SimpleEffect(effect, m_pObj, 0, 0, 3, 0, 0, 0);
        }
        if (m_nIdx == 0) {
            long se = m_pOwner->GetParam_SE(SE_TURN);
            if (se >= 0) {
                m_nSe = CGameSnd::Play(se, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
        }
        return 1;
    }
}
