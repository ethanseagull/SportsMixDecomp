#include "game/msw/CStgGmk_Wave.hpp"
#include "game/msw/CStgGmk_Wave_Act.hpp"
#include "game/msw/CStgGmk_Wave_Mdl.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/g3d/ResMdl.hpp"

// The waves of the beach stage (stage 02). In the original one file holds
// the model (CStgGmk_Wave_Mdl, from 0x802115A4), the actor
// (CStgGmk_Wave_Act), the driver (CStgGmk_Wave_Drv), the wave
// (CStgGmk_Wave) and the cluster that times them (CStgGmk_WaveCluster),
// with their weak functions, the static initialiser and the thunks at the
// end of .text (to 0x80215608). Evidence: the wave's NewMdl inlines the
// model's constructor and the cluster's NewGmk the wave's, the vtables are
// in one .data block (0x80487E68 to 0x80488608) and the .sdata2 constants
// are shared by all five.
//
// Each wave is a model with an animation that rolls it across the court;
// the actor follows the model's node "n_atari". The cluster keeps
// GetMaxNum() waves, starts them one after the other GetParam_PartCycle()
// frames apart and, every GetParam_BigWaveCycle() frames (plus a random
// part), turns the oldest one into a big wave. A wave that turns back
// (its velocity's x changes sign) buries coins; the big waves also bring
// items.

namespace msw {

    // Returned while there is no wave.
    bcs::FVEC3 CStgGmk_Wave_Act::vInvalidWavePos(1000.0f, 0.0f, 0.0f);
    bcs::FVEC3 CStgGmk_Wave_Act::vInvalidWaveVel(0.0f, 0.0f, 0.0f);
}

namespace {

    // (name guessed) What CStgGmk_Wave returns without an actor.
    bcs::FVEC3 s_vNoWave(0.0f, 0.0f, 0.0f);

    // Animation suffixes, appended to the model's animation name.
    const char *s_AnmSuffix[] = { "01", "02", 0 };

    // The wave models; CStgGmk_Wave_Mdl's index picks one.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s02_ef_sea01_03", 0, 0, 0x30C, s_AnmSuffix, "s02_ef_sea01_", 0, 0 },
        { "s02_ef_sea01_01", 0, 0, 0x30C, s_AnmSuffix, "s02_ef_sea01_", 0, 0 },
        { "s02_ef_sea01_02", 0, 0, 0x30C, s_AnmSuffix, "s02_ef_sea01_", 0, 0 },
    };

    // (names guessed) The work memory of the actor's states: STANDBY's
    // sleep time (negative: for ever), WAVE's frames until the items come
    // up (negative: none).
    // (names guessed) The larger and the smaller of two values.
    inline float Max(float a, float b)
    {
        float r = b;
        if (a > b) {
            r = a;
        }
        return r;
    }

    inline float Min(float a, float b)
    {
        float r = a;
        if (b < a) {
            r = b;
        }
        return r;
    }

    struct STATE_WORK {
        long m_Unk00;
        float m_fTimer; // 0x04
    };
}

namespace msw {

    CStgGmk_Wave_Mdl::CStgGmk_Wave_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent)
    {
        m_nMdlIdx = idx;
        SetLayer(1);
    }

    CStgGmk_Wave_Mdl::~CStgGmk_Wave_Mdl()
    {
    }

    void CStgGmk_Wave_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 2 ? 2 : (idx < 0 ? 0 : idx);
        CGenericGimmickCreator creator(CLoadData::GetRes(0), &s_MdlParam[idx]);
        CStgMdl::Create(creator);
    }

    bcs::CMdlObj *CStgGmk_Wave_Mdl::NewMdl(long heap) const
    {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }

    void CStgGmk_Wave_Mdl::Main(float rate)
    {
        CStgMdl::Main(rate);
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl != 0) {
            float frame = GetAnmFrame(CStageAnm::ANM_IDX(0));
            float total = GetAnmTotalFrame(CStageAnm::ANM_IDX(0));
            if (total > 0.0f) {
                float rest = 1.0f - frame / total;
                rest = rest > 1.0f ? 1.0f : (rest < 0.0f ? 0.0f : rest);
                long priority = (long)(10.0f * rest) + 0x80;
                pMdl->SetDrawPriority(priority, priority);
            }
        }
    }

    CStgGmk_Wave_Act::CStgGmk_Wave_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_fAge(0.0f), m_fSleepTimer(0.0f), m_bAlive(0), m_nWaveType(0),
          m_nNextWaveType(0), m_bWave(0), m_nAtariNode(-1)
    {
        FsmqInit(this, 3, 4, sizeof(STATE_WORK));
        FsmqSetState(STATE_DEFAULT, &CStgGmk_Wave_Act::CtorState_DEFAULT, &CStgGmk_Wave_Act::LoopState_DEFAULT,
                     &CStgGmk_Wave_Act::DtorState_DEFAULT);
        FsmqSetState(STATE_STANDBY, &CStgGmk_Wave_Act::CtorState_STANDBY, &CStgGmk_Wave_Act::LoopState_STANDBY,
                     &CStgGmk_Wave_Act::DtorState_STANDBY);
        FsmqSetState(STATE_WAVE, &CStgGmk_Wave_Act::CtorState_WAVE, &CStgGmk_Wave_Act::LoopState_WAVE,
                     &CStgGmk_Wave_Act::DtorState_WAVE);
        m_WavePos.Set(0.0f, 0.0f, 0.0f);
        m_WaveVel.Set(0.0f, 0.0f, 0.0f);
    }

    CStgGmk_Wave_Act::~CStgGmk_Wave_Act()
    {
    }

    // Finds the model's node the wave is at.
    void CStgGmk_Wave_Act::OnInit()
    {
        CStgMdl *pMdl = GetParent()->GetMdl(0);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
        if (pMdlObj != 0) {
            nw4r::g3d::ResNode node = pMdlObj->GetResMdl().GetResNode("n_atari");
            if (node.IsValid()) {
                m_nAtariNode = node.GetID();
            } else {
                m_nAtariNode = -1;
            }
        }
    }

    void CStgGmk_Wave_Act::OnUpdate(float rate)
    {
        m_fAge += rate;
        UpdateWavePos(rate);
        FsmqExec();
    }

    // The wave is where the model's node is; its velocity is the move since
    // the last frame. A wave that turns back to +x buries coins.
    void CStgGmk_Wave_Act::UpdateWavePos(float rate)
    {
        CStgObj *pObj = GetParent();
        bcs::FVEC3 prevPos = m_WavePos;
        bcs::FVEC3 prevVel = m_WaveVel;
        long bPrevWave = m_bWave;
        CStgMdl *pMdl = pObj->GetMdl(0);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
        if (pMdlObj != 0 && m_nAtariNode >= 0) {
            bcs::FMTX34 mtx;
            pMdlObj->GetNodeWorldMtx(m_nAtariNode, mtx);
            m_WavePos.Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        }
        if (rate > 0.0f && !IsSleep()) {
            if (bPrevWave) {
                bcs::FVEC3 vel;
                bcs::FVEC3Sub(&vel, &m_WavePos, &prevPos);
                m_WaveVel = vel;
            } else {
                m_WaveVel.Set(0.0f, 0.0f, 0.0f);
            }
            m_bWave = 1;
        } else {
            m_WaveVel.Set(0.0f, 0.0f, 0.0f);
            m_bWave = 0;
        }
        if (CStageMan::GetInstance()->IsGimmickRunning() && m_bWave && m_WaveVel.x > 0.0f &&
            m_WaveVel.x * prevVel.x < 0.0f) {
            DoPopCoin();
        }
    }

    void CStgGmk_Wave_Act::UpdateMdlMtx()
    {
        for (long i = 0; i < 3; i++) {
            CStgMdl *pMdl = GetParent()->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetMtx(GetSrt()->GetMtx());
            }
        }
    }

    // Buries GetParam_PopCoinNum coins between the wave and the court's
    // right side line.
    void CStgGmk_Wave_Act::DoPopCoin()
    {
        float right;
        float left;
        float front;
        float back;
        GetParam_CourtRect(left, right, front, back);
        bcs::FVEC3 pos = GetWavePos();
        pos.x += GetParam_CoinPopPosOfs();
        // Between the side lines.
        left = Max(pos.x, left);
        left = Min(left, right);
        bcs::FVEC3 min(left, 0.0f, front);
        bcs::FVEC3 max(right, 0.0f, back);
        CGimmickMan::CreateBuryCoin(min, max, GetParam_PopCoinNum(GetWaveType()));
    }

    // Drops GetParam_PopItemNum items into the strip GetParam_ItemPopPosX
    // wide along the court's right side line.
    void CStgGmk_Wave_Act::DoPopItem()
    {
        long type = GetWaveType();
        float right;
        float left;
        float front;
        float back;
        GetParam_CourtRect(left, right, front, back);
        float x = right - GetParam_ItemPopPosX(type);
        if (right < x) {
            x = right;
        }
        left = x;
        bcs::FVEC3 min(left, 0.0f, front);
        bcs::FVEC3 max(right, 0.0f, back);
        long num = GetParam_PopItemNum(type);
        float comeFrame = GetParam_PopItemComeFrame(type);
        float popDist = GetParam_PopItemPopDist(type);
        for (long i = 0; i < num; i++) {
            CGimmickMan::CreateDriftItem(0, min, max, comeFrame, popDist);
        }
    }

    float CStgGmk_Wave_Act::GetParam_Interval() const
    {
        return -1.0f;
    }

    // The wave's parameters (the cluster sets them each frame).
    float CStgGmk_Wave_Act::GetParam_Cycle() const
    {
        const CStgGmk_Wave *pWave = static_cast<const CStgGmk_Wave *>(GetParent());
        if (pWave != 0) {
            return pWave->GetParam_Cycle();
        }
        return 900.0f;
    }

    float CStgGmk_Wave_Act::GetParam_AnimTime() const
    {
        const CStgGmk_Wave *pWave = static_cast<const CStgGmk_Wave *>(GetParent());
        if (pWave != 0) {
            return pWave->GetParam_AnimTime();
        }
        return 1.0f;
    }

    float CStgGmk_Wave_Act::GetParam_BigAnimTime() const
    {
        const CStgGmk_Wave *pWave = static_cast<const CStgGmk_Wave *>(GetParent());
        if (pWave != 0) {
            return pWave->GetParam_BigAnimTime();
        }
        return 1.0f;
    }

    long CStgGmk_Wave_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    long CStgGmk_Wave_Act::GetParam_SE(long type) const
    {
        if (type == 0) {
            return 0xD0C;
        }
        return -1;
    }

    // Type 1 is the big wave.
    long CStgGmk_Wave_Act::GetParam_PopCoinNum(long type) const
    {
        return type == 1 ? 10 : 0;
    }

    void CStgGmk_Wave_Act::GetParam_CourtRect(float &rLeft, float &rRight, float &rFront, float &rBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(&rLeft, &rRight, &rFront, &rBack);
    }

    float CStgGmk_Wave_Act::GetParam_CoinPopPosOfs() const
    {
        return 3.0f;
    }

    long CStgGmk_Wave_Act::GetParam_PopItemNum(long type) const
    {
        if (type == 1) {
            return 10;
        }
        return 3;
    }

    float CStgGmk_Wave_Act::GetParam_PopItemDelay(long type) const
    {
        if (type == 1) {
            return 0.0f;
        }
        return 130.0f;
    }

    float CStgGmk_Wave_Act::GetParam_PopItemComeFrame(long type) const
    {
        if (type == 1) {
            return 0.5f * GetParam_BigAnimTime();
        }
        return 0.3f * GetParam_AnimTime();
    }

    float CStgGmk_Wave_Act::GetParam_PopItemPopDist(long type) const
    {
        return 20.0f;
    }

    float CStgGmk_Wave_Act::GetParam_ItemPopPosX(long type) const
    {
        if (type == 1) {
            return 9.0f;
        }
        return 3.0f;
    }

    // Hidden for time frames (for ever when negative), then rolls again.
    void CStgGmk_Wave_Act::Sleep(float time)
    {
        static_cast<STATE_WORK *>(FsmqSetState(STATE_STANDBY))->m_fTimer = time;
    }

    bool CStgGmk_Wave_Act::IsSleep() const
    {
        if (FsmqGetState() == STATE_STANDBY) {
            return true;
        }
        return false;
    }

    // Whether the cluster lets a big wave come now.
    bool CStgGmk_Wave_Act::IsBigWaveReady() const
    {
        const CStgGmk_Wave *pWave = static_cast<const CStgGmk_Wave *>(GetParent());
        const CStgGmk_WaveCluster *pOwner = pWave != 0 ? pWave->GetOwner() : 0;
        if (pOwner != 0) {
            return pOwner->IsNowEnableBigWave();
        }
        return true;
    }

    void CStgGmk_Wave_Act::Reset()
    {
        FsmqClear();
        m_fAge = 0.0f;
        m_WavePos = vInvalidWavePos;
        m_WaveVel = vInvalidWaveVel;
        m_bAlive = 0;
        m_bWave = 0;
    }

    float CStgGmk_Wave_Act::GetSleepTimer() const
    {
        if (IsSleep()) {
            return m_fSleepTimer;
        }
        return 0.0f;
    }

    // Hidden, then asleep for GetParam_Interval frames.
    void CStgGmk_Wave_Act::CtorState_DEFAULT()
    {
        CStgObj *pObj = GetParent();
        m_bAlive = 0;
        for (unsigned long i = 0; i < pObj->GetMdlNum(); i++) {
            CStgMdl *pMdl = pObj->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetVisible(0);
            }
        }
        static_cast<STATE_WORK *>(FsmqSetState(STATE_STANDBY))->m_fTimer = GetParam_Interval();
        FsmqRetry();
    }

    void CStgGmk_Wave_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_Wave_Act::DtorState_DEFAULT()
    {
    }

    // Hidden, the animation stopped, for the frames given to Sleep.
    void CStgGmk_Wave_Act::CtorState_STANDBY()
    {
        m_fSleepTimer = FsmqGetWork<STATE_WORK>()->m_fTimer;
        CStgObj *pObj = GetParent();
        m_bAlive = 0;
        for (unsigned long i = 0; i < pObj->GetMdlNum(); i++) {
            CStgMdl *pMdl = pObj->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetVisible(0);
                pMdl->SetAnmSpeed(0.0f);
            }
        }
    }

    void CStgGmk_Wave_Act::LoopState_STANDBY()
    {
        const STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        float rate = GetFrameRateF();
        if (!(pWork->m_fTimer < 0.0f)) {
            m_fSleepTimer -= rate;
            if (m_fSleepTimer <= 0.0f) {
                m_fSleepTimer = 0.0f;
                FsmqChangeState(STATE_WAVE);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_Wave_Act::DtorState_STANDBY()
    {
    }

    // Shown, the animation (1 for a big wave) played so that it lasts the
    // wave's animation time, at most its cycle less a frame.
    void CStgGmk_Wave_Act::CtorState_WAVE()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        CStgGmk_Wave *pWave = static_cast<CStgGmk_Wave *>(GetParent());
        long num = pWave->GetMdlNum();
        CStgGmk_WaveCluster *pOwner = pWave->GetOwner();
        m_fAge = 0.0f;
        m_bAlive = 1;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pWave->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetVisible(1);
            }
        }

        long type = GetNextWaveType();
        if (type == 1 && !IsBigWaveReady()) {
            type = 0;
        }
        m_nWaveType = type;
        SetNextWaveType(0);

        // Animation 1 is the big wave's.
        unsigned long anm = type == 1;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pWave->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetAnm(anm, 0, 1.0f, 0.0f, 0.0f, -1);
                float frames = pMdl->GetAnmTotalFrame(CStageAnm::ANM_IDX(0));
                if (1.0f > frames) {
                    frames = 1.0f;
                }
                float time = GetParam_Cycle() - 1.0f;
                if (1.0f > time) {
                    time = 1.0f;
                }
                float animTime = type == 1 ? GetParam_BigAnimTime() : GetParam_AnimTime();
                if (animTime < time) {
                    time = animTime;
                }
                pMdl->SetAnmSpeed(frames / time);
            }
        }

        if (type == 1) {
            SE_Play(0, GetParam_SE(0), 0.0f, 0);
        }
        if (pOwner->GetParam_PopItemCycle(type) > 0) {
            pWork->m_fTimer = GetParam_PopItemDelay(type);
        } else {
            pOwner->ResetItemPopCnt(type);
            pWork->m_fTimer = -1.0f;
        }
    }

    // Brings the items when their time comes (every GetParam_PopItemCycle
    // waves of the type); sleeps when the animation has ended.
    void CStgGmk_Wave_Act::LoopState_WAVE()
    {
        float rate = GetFrameRateF();
        CStgMdl *pMdl;
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        CStgGmk_Wave *pWave = static_cast<CStgGmk_Wave *>(GetParent());
        CStgGmk_WaveCluster *pOwner = pWave->GetOwner();
        GetWaveVelocity();
        long type = GetWaveType();
        if (pWork->m_fTimer >= 0.0f && rate > 0.0f) {
            pWork->m_fTimer -= rate;
            if (pWork->m_fTimer <= 0.0f) {
                if (IsBigWaveReady() && pOwner->DecItemPopCnt(type) <= 0) {
                    DoPopItem();
                    pOwner->ResetItemPopCnt(type);
                }
                pWork->m_fTimer = -1.0f;
            }
        }
        pMdl = pWave->GetMdl(0);
        if (pMdl != 0 && pMdl->IsAnmEnd(1)) {
            Sleep(GetParam_Interval());
            FsmqRetry();
        }
    }

    void CStgGmk_Wave_Act::DtorState_WAVE()
    {
        SE_Stop(0, 0.0f);
    }

    CStgGmk_Wave_Drv::CStgGmk_Wave_Drv(CStgGmk *pParent) : CStgDrv(pParent)
    {
    }

    CStgGmk_Wave_Drv::~CStgGmk_Wave_Drv()
    {
    }

    void CStgGmk_Wave_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Wave::CStgGmk_Wave(bcs::CTask *pParent)
        : CStgGmk(pParent, 3), m_fCycle(900.0f), m_fAnimTime(1.0f), m_fBigAnimTime(1.0f), m_pOwner(0)
    {
        m_DefaultPos.Set(0.0f, 0.0f, 0.0f);
        SetType(12);
    }

    CStgGmk_Wave::~CStgGmk_Wave()
    {
    }

    CStgMdl *CStgGmk_Wave::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Wave_Mdl(this, idx);
    }

    void CStgGmk_Wave::OnPostCreate()
    {
    }

    void CStgGmk_Wave::OnSceneEntry()
    {
    }

    void CStgGmk_Wave::OnSceneLeave()
    {
    }

    void CStgGmk_Wave::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    long CStgGmk_Wave::IsActive() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Wave::Inactivate()
    {
        CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            pAct->Sleep(-1.0f);
        }
    }

    void CStgGmk_Wave::Sleep(float time)
    {
        CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            pAct->Sleep(time);
        }
    }

    bool CStgGmk_Wave::IsSleep() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->IsSleep();
        }
        return false;
    }

    float CStgGmk_Wave::GetSleepTimer() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->GetSleepTimer();
        }
        return 0.0f;
    }

    bcs::FVEC3 CStgGmk_Wave::GetWavePos() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->GetWavePos();
        }
        return s_vNoWave;
    }

    bcs::FVEC3 CStgGmk_Wave::GetWaveVelocity() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->GetWaveVelocity();
        }
        return s_vNoWave;
    }

    float CStgGmk_Wave::GetAge() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->GetAge();
        }
        return 0.0f;
    }

    long CStgGmk_Wave::GetWaveType() const
    {
        const CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            return pAct->GetWaveType();
        }
        return 0;
    }

    void CStgGmk_Wave::SetNextWaveType(long type)
    {
        CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            pAct->SetNextWaveType(type);
        }
    }

    void CStgGmk_Wave::Reset()
    {
        CStgGmk_Wave_Act *pAct = GetWaveAct();
        if (pAct != 0) {
            pAct->Reset();
        }
    }

    void CStgGmk_Wave::Disp()
    {
    }

    // The age in hundredths of a frame.
    unsigned long CStgGmk_Wave::OnCalcSum() const
    {
        long sum = 0;
        sum += 100.0f * GetAge();
        return sum;
    }

    CStgGmk_WaveCluster::CStgGmk_WaveCluster(bcs::CTask *pParent)
        : m_pParent(pParent), m_ppGmk(0), m_nTblSize(0), m_fBigWaveTimer(-1.0f), m_nItemPopCnt(0),
          m_nBigItemPopCnt(0)
    {
        m_WavePos0.Set(0.0f, 0.0f, 0.0f);
        m_WaveVel0.Set(0.0f, 0.0f, 0.0f);
        m_WavePos1.Set(0.0f, 0.0f, 0.0f);
        m_WaveVel1.Set(0.0f, 0.0f, 0.0f);
    }

    CStgGmk_WaveCluster::~CStgGmk_WaveCluster()
    {
        DeleteGmk();
    }

    // Makes GetMaxNum() waves.
    void CStgGmk_WaveCluster::CreateGmk()
    {
        long num = GetMaxNum();
        if (num > 0) {
            m_ppGmk = new (1) CStgGmk_Wave *[num];
            m_nTblSize = num;
            for (long i = 0; i < num; i++) {
                CStgGmk_Wave *pWave = NewGmk(0);
                m_ppGmk[i] = pWave;
                if (pWave != 0) {
                    pWave->SetOwner(this);
                    pWave->Create();
                    pWave->SceneEntry();
                }
            }
            TimerReset();
            Reset();
        }
    }

    void CStgGmk_WaveCluster::DeleteGmk()
    {
        if (m_ppGmk != 0) {
            long num = GetTblSize();
            for (long i = 0; i < num; i++) {
                if (m_ppGmk[i] != 0) {
                    m_ppGmk[i]->Delete();
                }
                m_ppGmk[i] = 0;
            }
            if (m_ppGmk != 0) {
                delete[] m_ppGmk;
            }
            m_ppGmk = 0;
            m_nTblSize = 0;
        }
    }

    // Updates the waves' parameters, times and the two waves on the court;
    // when the big wave timer runs out the oldest wave becomes a big one.
    void CStgGmk_WaveCluster::Main(float rate)
    {
        UpdateParam();
        UpdateWaveTimer();
        UpdateWavePos();
        if (m_fBigWaveTimer >= 0.0f) {
            if (IsNowEnableBigWave()) {
                m_fBigWaveTimer -= rate;
                if (m_fBigWaveTimer <= 0.0f) {
                    m_fBigWaveTimer = 0.0f;
                    SetNextBigWaveTimer();
                    CStgGmk_Wave *pWave = GetOldWave();
                    if (pWave != 0) {
                        pWave->SetNextWaveType(1);
                    }
                }
            }
        } else {
            SetNextBigWaveTimer();
        }
    }

    unsigned long CStgGmk_WaveCluster::CalcSum() const
    {
        long num = GetTblSize();
        unsigned long sum = num;
        for (long i = 0; i < num; i++) {
            const CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                sum += pWave->CalcSum();
            }
        }
        return sum;
    }

    // The wave moving to -x and the one moving to +x.
    void CStgGmk_WaveCluster::UpdateWavePos()
    {
        FindWave(m_WavePos0, m_WaveVel0, -1.0f);
        FindWave(m_WavePos1, m_WaveVel1, 1.0f);
    }

    bool CStgGmk_WaveCluster::FindWave(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float dir) const
    {
        long num = GetTblSize();
        if (num <= 0) {
            return false;
        }
        bool bFound = false;
        bcs::FVEC3 pos;
        bcs::FVEC3 foundPos;
        bcs::FVEC3 vel;
        bcs::FVEC3 foundVel;
        for (long i = 0; i < num; i++) {
            const CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                pos = pWave->GetWavePos();
                vel = pWave->GetWaveVelocity();
                if (vel.x * dir > 0.0f && (!bFound || pos.x < foundPos.x)) {
                    bFound = true;
                    foundPos = pos;
                    foundVel = vel;
                }
            }
        }
        if (bFound) {
            rPos = foundPos;
            rVel = foundVel;
        } else {
            rPos.Set(1000.0f, 0.0f, 0.0f);
            rVel.Set(0.0f, 0.0f, 0.0f);
        }
        return bFound;
    }

    // Keeps the sleeping waves GetParam_PartCycle frames apart, counted
    // from the youngest one.
    void CStgGmk_WaveCluster::UpdateWaveTimer()
    {
        long num = GetTblSize();
        if (num <= 0.0f) {
            return;
        }
        float partCycle = GetParam_PartCycle();
        if (partCycle <= 0.0f) {
            return;
        }
        float minAge = -1.0f;
        long youngest = -1;
        for (long i = 0; i < num; i++) {
            CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                float age = pWave->GetAge();
                if (minAge < 0.0f || age < minAge) {
                    minAge = age;
                    youngest = i;
                }
            }
        }
        while (minAge > partCycle) {
            minAge -= partCycle;
        }
        for (long i = 1; i < num; i++) {
            CStgGmk_Wave *pWave = GetGmk((youngest + i) % num);
            if (pWave != 0 && pWave->IsSleep() && pWave->GetSleepTimer() < 0.0f) {
                pWave->Sleep(partCycle * i - minAge);
            }
        }
    }

    // Passes the parameters on to the waves and puts them at their
    // starting x.
    void CStgGmk_WaveCluster::UpdateParam()
    {
        long num = GetTblSize();
        float cycle = GetParam_Cycle();
        if (1.0f > cycle) {
            cycle = 1.0f;
        }
        float offsetX = GetParam_OffsetX();
        float bigOffsetX = GetParam_BigWaveOffsetX();
        for (long i = 0; i < num; i++) {
            CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                pWave->SetParam_Cycle(cycle);
                pWave->SetParam_AnimTime(GetParam_AnimTime());
                pWave->SetParam_BigAnimTime(GetParam_BigAnimTime());
                bcs::FVEC3 pos = pWave->GetDefaultPos();
                pos.x = pWave->GetWaveType() == 1 ? bigOffsetX : offsetX;
                pWave->SetDefaultPos(pos);
                pWave->SetPos(pos);
            }
        }
    }

    // Starts the waves anew, each a part of the cycle after the one before.
    void CStgGmk_WaveCluster::Reset()
    {
        long num = GetTblSize();
        if (num > 0) {
            float step = GetParam_Cycle() / num;
            for (long i = 0; i < num; i++) {
                CStgGmk_Wave *pWave = m_ppGmk[i];
                if (pWave != 0) {
                    pWave->Reset();
                    pWave->SetNextWaveType(0);
                    pWave->Sleep(step * i);
                }
            }
            UpdateParam();
            UpdateWavePos();
        }
    }

    // As Reset, and the big wave timer and the item counts start anew.
    void CStgGmk_WaveCluster::TimerReset()
    {
        long num = GetTblSize();
        if (num > 0) {
            float step = GetParam_Cycle() / num;
            for (long i = 0; i < num; i++) {
                CStgGmk_Wave *pWave = m_ppGmk[i];
                if (pWave != 0) {
                    pWave->Reset();
                    pWave->SetNextWaveType(0);
                    pWave->Sleep(step * i);
                }
            }
            UpdateParam();
            SetNextBigWaveTimer();
            UpdateWavePos();
            ResetItemPopCnt(0);
            ResetItemPopCnt(1);
        }
    }

    // (The original calls GetTblSize and drops the result: an index check
    // that compiled to nothing but the call, probably.)
    CStgGmk_Wave *CStgGmk_WaveCluster::GetGmk(long idx)
    {
        GetTblSize();
        return m_ppGmk[idx];
    }

    const CStgGmk_Wave *CStgGmk_WaveCluster::GetGmk(long idx) const
    {
        GetTblSize();
        return m_ppGmk[idx];
    }

    // The velocity of the wave rPos is behind (past to +x), zero if none.
    bcs::FVEC3 CStgGmk_WaveCluster::GetWaveVelocityByPos(const bcs::FVEC3 &rPos) const
    {
        long num = GetTblSize();
        bcs::FVEC3 vel(0.0f, 0.0f, 0.0f);
        if (num <= 0) {
            return vel;
        }
        if (m_WavePos0.x <= rPos.x) {
            vel = m_WaveVel0;
        } else if (m_WavePos1.x <= rPos.x) {
            vel = m_WaveVel1;
        }
        return vel;
    }

    // The youngest wave.
    CStgGmk_Wave *CStgGmk_WaveCluster::GetNewWave()
    {
        long num = GetTblSize();
        CStgGmk_Wave *pNew = 0;
        float minAge;
        for (long i = 0; i < num; i++) {
            CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                float age = pWave->GetAge();
                if (pNew == 0 || age < minAge) {
                    pNew = pWave;
                    minAge = age;
                }
            }
        }
        return pNew;
    }

    const CStgGmk_Wave *CStgGmk_WaveCluster::GetNewWave() const
    {
        long num = GetTblSize();
        const CStgGmk_Wave *pNew = 0;
        float minAge;
        for (long i = 0; i < num; i++) {
            const CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                float age = pWave->GetAge();
                if (pNew == 0 || age < minAge) {
                    pNew = pWave;
                    minAge = age;
                }
            }
        }
        return pNew;
    }

    // The oldest wave.
    CStgGmk_Wave *CStgGmk_WaveCluster::GetOldWave()
    {
        long num = GetTblSize();
        CStgGmk_Wave *pOld = 0;
        float maxAge;
        for (long i = 0; i < num; i++) {
            CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                float age = pWave->GetAge();
                if (pOld == 0 || age > maxAge) {
                    pOld = pWave;
                    maxAge = age;
                }
            }
        }
        return pOld;
    }

    const CStgGmk_Wave *CStgGmk_WaveCluster::GetOldWave() const
    {
        long num = GetTblSize();
        const CStgGmk_Wave *pOld = 0;
        float maxAge;
        for (long i = 0; i < num; i++) {
            const CStgGmk_Wave *pWave = GetGmk(i);
            if (pWave != 0) {
                float age = pWave->GetAge();
                if (pOld == 0 || age > maxAge) {
                    pOld = pWave;
                    maxAge = age;
                }
            }
        }
        return pOld;
    }

    // The youngest wave other than pWave.
    CStgGmk_Wave *CStgGmk_WaveCluster::GetNextWave(const CStgGmk_Wave *pWave)
    {
        long num = GetTblSize();
        CStgGmk_Wave *pNext = 0;
        float minAge;
        for (long i = 0; i < num; i++) {
            CStgGmk_Wave *pOther = GetGmk(i);
            if (pOther != 0 && pOther != pWave) {
                float age = pOther->GetAge();
                if (pNext == 0 || age < minAge) {
                    pNext = pOther;
                    minAge = age;
                }
            }
        }
        return pNext;
    }

    const CStgGmk_Wave *CStgGmk_WaveCluster::GetNextWave(const CStgGmk_Wave *pWave) const
    {
        long num = GetTblSize();
        const CStgGmk_Wave *pNext = 0;
        float minAge;
        for (long i = 0; i < num; i++) {
            const CStgGmk_Wave *pOther = GetGmk(i);
            if (pOther != 0 && pOther != pWave) {
                float age = pOther->GetAge();
                if (pNext == 0 || age < minAge) {
                    pNext = pOther;
                    minAge = age;
                }
            }
        }
        return pNext;
    }

    // GetParam_BigWaveCycle frames plus up to GetParam_BigWaveCycleRnd; no
    // big wave (-1) when the cycle is not positive.
    void CStgGmk_WaveCluster::SetNextBigWaveTimer()
    {
        m_fBigWaveTimer = GetParam_BigWaveCycle();
        if (m_fBigWaveTimer > 0.0f) {
            float rnd = GetParam_BigWaveCycleRnd();
            if (rnd > 0.0f) {
                m_fBigWaveTimer += bcs::BCS_RandMax(rnd);
            }
        } else {
            m_fBigWaveTimer = -1.0f;
        }
    }

    bool CStgGmk_WaveCluster::IsNowEnableBigWave() const
    {
        return CStageMan::GetInstance()->IsGimmickRunning();
    }

    long CStgGmk_WaveCluster::GetItemPopCnt(long type) const
    {
        if (type == 1) {
            return m_nBigItemPopCnt;
        }
        return m_nItemPopCnt;
    }

    long CStgGmk_WaveCluster::DecItemPopCnt(long type)
    {
        long &rCnt = type == 1 ? m_nBigItemPopCnt : m_nItemPopCnt;
        return --rCnt;
    }

    void CStgGmk_WaveCluster::ResetItemPopCnt(long type)
    {
        long &rCnt = type == 1 ? m_nBigItemPopCnt : m_nItemPopCnt;
        rCnt = GetParam_PopItemCycle(type);
    }

    // One cycle covers all the waves.
    float CStgGmk_WaveCluster::GetParam_Cycle() const
    {
        return GetParam_PartCycle() * GetMaxNum();
    }

    float CStgGmk_WaveCluster::GetParam_PartCycle() const
    {
        return 900.0f / GetMaxNum();
    }

    float CStgGmk_WaveCluster::GetParam_BigWaveCycle() const
    {
        return 1200.0f;
    }

    float CStgGmk_WaveCluster::GetParam_BigWaveCycleRnd() const
    {
        return 600.0f;
    }

    float CStgGmk_WaveCluster::GetParam_OffsetX() const
    {
        return 0.0f;
    }

    float CStgGmk_WaveCluster::GetParam_BigWaveOffsetX() const
    {
        return 0.0f;
    }

    float CStgGmk_WaveCluster::GetParam_AnimTime() const
    {
        return GetParam_PartAnimTime() * GetMaxNum();
    }

    float CStgGmk_WaveCluster::GetParam_BigAnimTime() const
    {
        return GetParam_BigPartAnimTime() * GetMaxNum();
    }

    float CStgGmk_WaveCluster::GetParam_PartAnimTime() const
    {
        return 60.0f;
    }

    float CStgGmk_WaveCluster::GetParam_BigPartAnimTime() const
    {
        return 60.0f;
    }

    long CStgGmk_WaveCluster::GetParam_PopItemCycle(long type) const
    {
        if (type == 1) {
            return 1;
        }
        return 3;
    }
}
