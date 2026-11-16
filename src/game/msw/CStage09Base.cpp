#include "game/msw/CStage09Base.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    CStage09Base::CStage09Base()
        : m_ppConveyor(0),
          m_nTblSize(0),
          m_nRelate(CV_RELATE_1),
          m_bStopEvent(0),
          m_bStartEvent(0)
    {
    }

    CStage09Base::~CStage09Base()
    {
        DeleteConveyor();
    }

    void CStage09Base::CreateConveyor()
    {
        long num = GetConveyorNum();
        m_ppConveyor = new CConveyor *[num];
        m_nTblSize = num;
        for (long i = 0; i < num; i++) {
            CConveyor *pConveyor = NewConveyor();
            m_ppConveyor[i] = pConveyor;
            pConveyor->SetIndex(i);
            pConveyor->SetOwner(this);
        }
    }

    void CStage09Base::DeleteConveyor()
    {
        if (m_ppConveyor != 0) {
            for (long i = 0; i < m_nTblSize; i++) {
                if (m_ppConveyor[i] != 0) {
                    delete m_ppConveyor[i];
                }
                m_ppConveyor[i] = 0;
            }
            if (m_ppConveyor != 0) {
                delete[] m_ppConveyor;
            }
            m_ppConveyor = 0;
        }
        m_nTblSize = 0;
    }

    CStage09Base::CConveyor *CStage09Base::GetConveyor(long no)
    {
        if (m_ppConveyor == 0) {
            return 0;
        }
        return m_ppConveyor[no];
    }

    const CStage09Base::CConveyor *CStage09Base::GetConveyor(long no) const
    {
        if (m_ppConveyor == 0) {
            return 0;
        }
        return m_ppConveyor[no];
    }

    void CStage09Base::Conveyor_UpdateAll(float frame)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            GetConveyor(i)->Main(frame);
        }

        if (m_bStartEvent) {
            PlayStartSe();
            m_bStartEvent = 0;
        }
        if (m_bStopEvent) {
            PlayStopSe();
            m_bStopEvent = 0;
            CGame::StartCameraQuake(19, -1, -1);
        }

        // Every conveyor stands still: end the running sound.
        if (Conveyor_IsStoppedAll()) {
            CStgSeSlot *pSeSlot = GetSeSlot();
            long slot = GetSeSlotBeg();
            if (pSeSlot->SE_IsAlive(slot)) {
                pSeSlot->SE_Stop(slot, 0.0f);
            }
        }
    }

    void CStage09Base::Conveyor_Start(long no, CConveyor::FLOW_DIR dir, int bAcc, float time)
    {
        CConveyor *pConveyor = GetConveyor(no);
        if (!bAcc) {
            pConveyor->Start(dir);
        } else {
            pConveyor->AccStart(dir, time);
        }
        m_bStartEvent = 1;
        Conveyor_OnStart(no, dir, bAcc, time);
    }

    void CStage09Base::Conveyor_Stop(long no)
    {
        CConveyor *pConveyor = GetConveyor(no);
        if (!pConveyor->IsStopped()) {
            pConveyor->Stop();
            m_bStopEvent = 1;
            Conveyor_OnStop(no);
        }
    }

    void CStage09Base::Conveyor_DelayStart(float delay, long no, CConveyor::FLOW_DIR dir, int bAcc, float time)
    {
        CConveyor *pConveyor = GetConveyor(no);
        if (!bAcc) {
            pConveyor->DelayStart(delay, dir);
        } else {
            pConveyor->DelayAccStart(delay, dir, time);
        }
    }

    void CStage09Base::Conveyor_DelayStop(float delay, long no)
    {
        GetConveyor(no)->DelayStop(delay);
    }

    // no: the conveyor that accelerates (AccStart); below zero none does.
    void CStage09Base::Conveyor_StartAll(CConveyor::FLOW_DIR dir, long no, float time)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            CConveyor *pConveyor = GetConveyor(i);
            if (pConveyor != 0) {
                if (dir != CConveyor::FLOW_DIR_NONE) {
                    CConveyor::FLOW_DIR cvDir = dir;
                    if (Conveyor_GetRelate() == CV_RELATE_2) {
                        // Alternate: all but the chosen conveyor run in
                        // reverse, or without one every odd conveyor.
                        cvDir = (no >= 0 && no != i)          ? Conveyor_GetReverseDir(dir)
                                : (no < 0 && (i & 1) != 0) ? Conveyor_GetReverseDir(dir)
                                                             : dir;
                    }
                    if (i != no) {
                        pConveyor->Start(cvDir);
                    } else {
                        pConveyor->AccStart(cvDir, time);
                    }
                } else {
                    pConveyor->Stop();
                }
            }
        }
        m_bStartEvent = 1;
        Conveyor_OnStartAll(dir, no, time);
    }

    void CStage09Base::Conveyor_AccStartAll(CConveyor::FLOW_DIR dir, float time)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            CConveyor *pConveyor = GetConveyor(i);
            if (pConveyor != 0) {
                if (dir != CConveyor::FLOW_DIR_NONE) {
                    CConveyor::FLOW_DIR cvDir = dir;
                    if (Conveyor_GetRelate() == CV_RELATE_2) {
                        cvDir = (i & 1) != 0 ? Conveyor_GetReverseDir(dir) : dir;
                    }
                    pConveyor->AccStart(cvDir, time);
                } else {
                    pConveyor->Stop();
                }
            }
        }
        m_bStartEvent = 1;
        Conveyor_OnStartAll(dir, -1, time);
    }

    void CStage09Base::Conveyor_StopAll()
    {
        if (!Conveyor_IsStoppedAll()) {
            long num = GetTblSize();
            for (long i = 0; i < num; i++) {
                GetConveyor(i)->Stop();
            }
            m_bStopEvent = 1;
            Conveyor_OnStopAll();
        }
    }

    void CStage09Base::Conveyor_DelayStartAll(float delay, CConveyor::FLOW_DIR dir, long no, float time)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            CConveyor *pConveyor = GetConveyor(i);
            if (pConveyor != 0 && dir != CConveyor::FLOW_DIR_NONE) {
                CConveyor::FLOW_DIR cvDir = dir;
                if (Conveyor_GetRelate() == CV_RELATE_2) {
                    cvDir = (no >= 0 && no != i)          ? Conveyor_GetReverseDir(dir)
                            : (no < 0 && (i & 1) != 0) ? Conveyor_GetReverseDir(dir)
                                                         : dir;
                }
                if (i != no) {
                    pConveyor->DelayStart(delay, cvDir);
                } else {
                    pConveyor->DelayAccStart(delay, cvDir, time);
                }
            }
        }
    }

    void CStage09Base::Conveyor_DelayAccStartAll(float delay, CConveyor::FLOW_DIR dir, float time)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            CConveyor *pConveyor = GetConveyor(i);
            if (pConveyor != 0 && dir != CConveyor::FLOW_DIR_NONE) {
                CConveyor::FLOW_DIR cvDir = dir;
                if (Conveyor_GetRelate() == CV_RELATE_2) {
                    cvDir = (i & 1) != 0 ? Conveyor_GetReverseDir(dir) : dir;
                }
                pConveyor->DelayAccStart(delay, cvDir, time);
            }
        }
    }

    void CStage09Base::Conveyor_DelayStopAll(float delay)
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            GetConveyor(i)->DelayStop(delay);
        }
    }

    void CStage09Base::Conveyor_ResetAll()
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            GetConveyor(i)->Reset();
        }
        CStgSeSlot *pSeSlot = GetSeSlot();
        long slot = GetSeSlotBeg();
        pSeSlot->SE_Stop(slot, 0.0f);
        m_bStopEvent = 0;
        m_bStartEvent = 0;
    }

    bool CStage09Base::Conveyor_IsStoppedAll() const
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            if (!GetConveyor(i)->IsStopped()) {
                return false;
            }
        }
        return true;
    }

    void CStage09Base::PlayStartSe()
    {
        CStgSeSlot *pSeSlot = GetSeSlot();
        long slot = GetSeSlotBeg();
        long se = GetParam_SE(0);
        if (se >= 0) {
            pSeSlot->SE_Play(slot, se, 0.0f, 3);
        }
    }

    void CStage09Base::PlayStopSe()
    {
        // The slot is fetched but not used: the stop sound plays on its own.
        CStgSeSlot *pSeSlot = GetSeSlot();
        long slot = GetSeSlotBeg();
        long se = GetParam_SE(1);
        if (se >= 0) {
            CGameSnd::Play(se, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CStage09Base::Stage09_OnPreInitialize()
    {
        CreateConveyor();
    }

    // The sounds: 0 the start, 1 the stop.
    long CStage09Base::GetParam_SE(long no) const
    {
        switch (no) {
            case 0:
                return 3402;
            case 1:
                return 3403;
        }
        return -1;
    }

    CStage09Base::CConveyor::CConveyor()
        : m_nIndex(0),
          m_nFlowDir(FLOW_DIR_NONE),
          m_fUnk0C(0.0f),
          m_fUnk10(0.0f),
          m_fUnk14(0.0f),
          m_fCurSpeed(0.0f),
          m_fTargetSpeed(0.0f),
          m_fLastSpeed(0.0f),
          m_fOddDist(0.0f),
          m_pMdlNode(0),
          m_pMdlNode2(0),
          m_fAccTimer(-1.0f),
          m_nSpeedLevel(0),
          m_bForceStop(0),
          m_fDelayTimer(-1.0f)
    {
    }

    CStage09Base::CConveyor::~CConveyor()
    {
    }

    void CStage09Base::CConveyor::Main(float frame)
    {
        // A pending delayed start or stop.
        if (frame >= 0.0f && m_fDelayTimer >= 0.0f) {
            m_fDelayTimer -= frame;
            if (m_fDelayTimer <= 0.0f) {
                m_fDelayTimer = -1.0f;
                if (m_nDelayDir != FLOW_DIR_NONE) {
                    if (!m_bDelayAcc) {
                        Start(static_cast<FLOW_DIR>(m_nDelayDir));
                    } else {
                        AccStart(static_cast<FLOW_DIR>(m_nDelayDir), m_fDelayAccTime);
                    }
                    GetOwner()->SetStartEvent(1);
                    GetOwner()->Conveyor_OnStart(GetIndex(), static_cast<FLOW_DIR>(m_nDelayDir), m_bDelayAcc,
                                                 m_fDelayAccTime);
                } else {
                    Stop();
                    GetOwner()->SetStopEvent(1);
                    GetOwner()->Conveyor_OnStop(GetIndex());
                }
            }
        }

        // The end of an acceleration.
        if (frame > 0.0f && m_fAccTimer >= 0.0f) {
            m_fAccTimer -= frame;
            if (m_fAccTimer <= 0.0f) {
                m_fAccTimer = -1.0f;
                SetSpeedLevel(0);
            }
        }

        float speed = GetParam_CvSpeed() / 60.0f;
        switch (m_nFlowDir) {
            case FLOW_DIR_2:
            case FLOW_DIR_3:
                m_fTargetSpeed = speed;
                break;
            case FLOW_DIR_1:
            case FLOW_DIR_4:
                m_fTargetSpeed = -speed;
                break;
            default:
                m_fTargetSpeed = 0.0f;
                break;
        }

        // Move the speed towards the target by 1/60 per frame.
        float cur = m_fCurSpeed;
        float target = m_fTargetSpeed;
        float step = 1.0f / 60.0f;
        if (cur < target) {
            cur += step;
            if (target < cur) {
                cur = target;
            }
        } else if (cur > target) {
            cur -= step;
            if (target > cur) {
                cur = target;
            }
        }

        m_fLastSpeed = cur * frame;
        SetSpeed(cur);
        m_fOddDist += m_fLastSpeed;
    }

    void CStage09Base::CConveyor::Start(FLOW_DIR dir)
    {
        if (!IsForceStop()) {
            m_nFlowDir = dir;
            m_fDelayTimer = -1.0f;
        }
    }

    void CStage09Base::CConveyor::AccStart(FLOW_DIR dir, float time)
    {
        if (!IsForceStop()) {
            m_nFlowDir = dir;
            m_fAccTimer = time;
            SetSpeedLevel(1);
            m_fDelayTimer = -1.0f;
        }
    }

    void CStage09Base::CConveyor::Stop()
    {
        SetSpeedLevel(0);
        SetSpeed(0.0f);
        m_nFlowDir = FLOW_DIR_NONE;
        m_fCurSpeed = 0.0f;
        m_fAccTimer = -1.0f;
        m_fDelayTimer = -1.0f;
    }

    void CStage09Base::CConveyor::DelayStart(float delay, FLOW_DIR dir)
    {
        m_fDelayTimer = delay;
        m_nDelayDir = dir;
        m_bDelayAcc = 0;
        m_fDelayAccTime = -1.0f;
    }

    void CStage09Base::CConveyor::DelayAccStart(float delay, FLOW_DIR dir, float time)
    {
        m_fDelayTimer = delay;
        m_nDelayDir = dir;
        m_bDelayAcc = 1;
        m_fDelayAccTime = time;
    }

    void CStage09Base::CConveyor::DelayStop(float delay)
    {
        m_fDelayTimer = delay;
        m_nDelayDir = FLOW_DIR_NONE;
        m_bDelayAcc = 0;
        m_fDelayAccTime = -1.0f;
    }

    void CStage09Base::CConveyor::Reset()
    {
        Stop();
        m_fOddDist = 0.0f;
    }

    void CStage09Base::CConveyor::GetFlow(bcs::FVEC3 &rFlow) const
    {
        // (a scale that is 1 in this game)
        float scale = 1.0f;
        float flow = m_fLastSpeed * scale;
        switch (m_nFlowDir) {
            case FLOW_DIR_1:
            case FLOW_DIR_2:
                rFlow.x = 0.0f;
                rFlow.y = 0.0f;
                rFlow.z = flow;
                return;
            case FLOW_DIR_3:
            case FLOW_DIR_4:
                rFlow.x = flow;
                rFlow.y = 0.0f;
                rFlow.z = 0.0f;
                return;
        }
        rFlow.x = 0.0f;
        rFlow.y = 0.0f;
        rFlow.z = 0.0f;
    }

    // The jump acceleration factor: conveyor 0 running in direction 1 and
    // conveyor 1 running in direction 2 carry a jump along, the others
    // work against it.
    float CStage09Base::CConveyor::GetFlowRate() const
    {
        FLOW_DIR dir = m_nFlowDir;
        long index = GetIndex();
        if ((index == 0 && dir == FLOW_DIR_1) || (index == 1 && dir == FLOW_DIR_2)) {
            return GetParam_JumpAccFollow();
        }
        return GetParam_JumpAccAgainst();
    }

    void CStage09Base::CConveyor::SetSpeed(float speed)
    {
        m_fCurSpeed = speed;
        float rate = speed * GetParam_MdlAnmDir();
        for (int i = 0; i < 2; i++) {
            CStageMdlListNode *pNode = i == 0 ? GetMdlNode() : GetMdlNode2();
            CStageNodeAnm *pAnm = pNode != 0 ? pNode->GetAnm() : 0;
            if (pAnm != 0) {
                pAnm->SetRate(-1, rate / GetParam_AnmFlow());
            }
        }
    }

    void CStage09Base::CConveyor::SetSpeedLevel(long level)
    {
        long old = m_nSpeedLevel;
        m_nSpeedLevel = level;
        GetOwner()->Conveyor_OnSetSpeedLevel(GetIndex(), level, old);
    }
}
