// msw::CStage02Base: the waves of a sport's stage 02. It owns the wave
// cluster the sport's stage makes (NewWaveCluster), tells the stage where
// the floor flows (a ball or player beyond the wave is pushed or pulled
// along x), and keeps a list of splashes: one per splash model of the stage
// ("_sibu00" to "_sibu05"), each playing the splash effect of the side the
// wave flows to while the wave is fast enough.
#include "game/msw/CStage02Base.hpp"

#include <math.h>

#include "bcs/CListNodeBase.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk_Wave.hpp"

namespace msw {

    namespace {

        // (names guessed) The splash of a splash model: model id, kind and
        // effect. The table ends with a negative id.
        struct SPLASH_MDL {
            long m_nMdlId;
            long m_nType;
            long m_nEffId;
        };

        const SPLASH_MDL s_SplashMdlTbl[] = {
            { 0xA, 0, 0x42D },
            { 0xB, 0, 0x42E },
            { 0xC, 0, 0x42F },
            { 0xD, 1, 0x430 },
            { 0xE, 1, 0x431 },
            { 0xF, 1, 0x432 },
            { -1, 0, 0 },
        };
    }

    // The splash's effects are attached effects of the effect manager.
    inline void CStage02Base::CSplashEff::CEffHandle::Stop()
    {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }

    inline void CStage02Base::CSplashEff::CEffHandle::Erase()
    {
        CEffectCtrl::EraseAttachEffect(m_hEffect);
        m_hEffect = 0;
    }

    CStage02Base::CStage02Base(bcs::CTask *pParent)
        : m_pParent(pParent),
          m_pWaveCluster(0)
    {
    }

    CStage02Base::CSplashList::~CSplashList()
    {
        while (GetCount() != 0) {
            bcs::CListNodeBase *pNode = CListBase::GetFirst();
            if (pNode == GetEnd()) {
                pNode = 0;
            }
            delete static_cast<CSplashEff *>(pNode);
        }
    }

    CStage02Base::~CStage02Base()
    {
        if (m_pWaveCluster != 0) {
            delete m_pWaveCluster;
        }
        m_pWaveCluster = 0;
    }

    void CStage02Base::Stage02_Main(float frame)
    {
        if (m_pWaveCluster != 0) {
            m_pWaveCluster->Main(frame);
        }
        m_SplashList.Main(frame, m_pWaveCluster);
    }

    long CStage02Base::Stage02_GetFloorAttribute(const bcs::FVEC3 &rPos) const
    {
        long attr = 0;
        if (m_pWaveCluster != 0 && Stage02_IsOnWave(rPos)) {
            attr |= 2;
        }
        return attr;
    }

    void CStage02Base::Stage02_GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                            CStageMan::FLOW_TYPE type) const
    {
        float margin = GetParam_WavePosMargin();
        bcs::FVEC3 pos = rPos;
        pos.x -= margin;
        bcs::FVEC3 flow(0.0f, 0.0f, 0.0f);

        if (m_pWaveCluster != 0 && Stage02_IsGimmickReady()) {
            if ((type >= 5 && type <= 6) || type == CStageMan::FLOW_TYPE_0) {
                if (Stage02_IsOnWave(rPos)) {
                    flow = m_pWaveCluster->GetWaveVelocityByPos(pos);
                    if (flow.x < 0.0f) {
                        flow.x *= GetParam_PushPower();
                    } else {
                        flow.x *= GetParam_PullPower();
                    }
                }
            }
        }
        rFlow = flow;
    }

    int CStage02Base::Stage02_IsOnWave(const bcs::FVEC3 &rPos) const
    {
        if (m_pWaveCluster != 0) {
            const bcs::FVEC3 &rWavePos = m_pWaveCluster->GetWavePos();
            if (rPos.x > rWavePos.x + GetParam_WavePosMargin()) {
                return 1;
            }
        }
        return 0;
    }

    bool CStage02Base::Stage02_OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                              nw4r::g3d::ResFile file, CArn &rArn, long n)
    {
        if (pRule->m_Unk08 == 1) {
            // A splash model: no model, a splash at its position.
            long nType = 0;
            long nEffId = -1;
            bcs::FVEC3 pos;
            rArn.GetPos(n, &pos);
            for (long i = 0; s_SplashMdlTbl[i].m_nMdlId >= 0; i++) {
                if (pRule->m_nId == s_SplashMdlTbl[i].m_nMdlId) {
                    nType = s_SplashMdlTbl[i].m_nType;
                    nEffId = s_SplashMdlTbl[i].m_nEffId;
                    break;
                }
            }
            if (nEffId >= 0) {
                NewSplashNode(nType, nEffId, pos);
            }
            return false;
        }
        return true;
    }

    void CStage02Base::Stage02_OnPostInitialize()
    {
    }

    void CStage02Base::Stage02_OnSceneEntry()
    {
        if (m_pWaveCluster != 0) {
            delete m_pWaveCluster;
        }
        m_pWaveCluster = 0;
        m_pWaveCluster = NewWaveCluster(m_pParent);
        if (m_pWaveCluster != 0) {
            m_pWaveCluster->CreateGmk();
        }
    }

    CStgGmk_WaveCluster *CStage02Base::NewWaveCluster(bcs::CTask *pParent) const
    {
        return new (1) CStgGmk_WaveCluster(pParent);
    }

    void CStage02Base::Stage02_OnPeriodInit()
    {
        if (m_pWaveCluster != 0) {
            m_pWaveCluster->Reset();
            m_pWaveCluster->TimerReset();
        }
    }

    void CStage02Base::Stage02_OnGimmickReset()
    {
        if (m_pWaveCluster != 0) {
            m_pWaveCluster->Reset();
        }
        m_SplashList.EraseAll();
    }

    void CStage02Base::Stage02_OnGimmickPause()
    {
        if (m_pWaveCluster != 0) {
            m_pWaveCluster->Reset();
        }
        m_SplashList.EraseAll();
    }

    void CStage02Base::Stage02_OnGimmickResume()
    {
    }

    unsigned long CStage02Base::Stage02_OnCalcSum() const
    {
        unsigned long sum = 0;
        if (m_pWaveCluster != 0) {
            sum = m_pWaveCluster->CalcSum();
        }
        return sum;
    }

    CStage02Base::CSplashEff::CEffHandle::~CEffHandle()
    {
        Stop();
    }

    CStage02Base::CSplashEff::CEffHandle::CEffHandle()
        : m_hEffect(0)
    {
    }

    CStage02Base::CSplashEff::CSplashEff(long nKind, const bcs::FVEC3 &rPos)
        : m_nKind(nKind)
    {
        m_vPos = rPos;
    }

    CStage02Base::CSplashEff::~CSplashEff()
    {
    }

    // Plays the splash of the side the wave at the splash flows to, while
    // the wave moves faster than 1/120 a frame.
    void CStage02Base::CSplashEff::Main(float frame, const CStgGmk_WaveCluster *pCluster)
    {
        if (frame <= 0.0f) {
            return;
        }

        bcs::FVEC3 pos = GetPos();
        pos.x -= 2.0f;
        bcs::FVEC3 vel = pCluster->GetWaveVelocityByPos(pos);
        float speed = fabsf(vel.x / frame);

        long nType;
        long nOther;
        if (vel.x < 0.0f) {
            nType = 0;
            nOther = 1;
        } else {
            nType = 1;
            nOther = 0;
        }

        if (speed > 1.0f / 120.0f) {
            Kick(nType);
        } else {
            Stop(nType);
        }
        Stop(nOther);
    }

    void CStage02Base::CSplashEff::Kick(long type)
    {
        long nEffId = GetEffId(type);
        if (nEffId < 0) {
            return;
        }

        CEffHandle &rEffect = m_aEffect[type];
        if (rEffect.m_hEffect != 0) {
            return;
        }

        rEffect.Stop();
        unsigned long hEffect = CEffectCtrl::CreateEffect(nEffId, 3, 0, 0, 0);
        CEffectCluster *pCluster = hEffect != 0 ? CEffectCtrl::GetObject(hEffect) : 0;
        if (pCluster != 0) {
            pCluster->SetTranslate(&m_vPos);
        }
        rEffect.Erase();
        rEffect.m_hEffect = hEffect;
    }

    void CStage02Base::CSplashEff::Stop(long type)
    {
        m_aEffect[type].Stop();
    }

    void CStage02Base::CSplashEff::Erase(long type)
    {
        m_aEffect[type].Erase();
    }

    void CStage02Base::CSplashEff::StopAll()
    {
        for (long i = 0; i < 2; i++) {
            m_aEffect[i].Stop();
        }
    }

    void CStage02Base::CSplashEff::EraseAll()
    {
        for (long i = 0; i < 2; i++) {
            m_aEffect[i].Erase();
        }
    }

    long CStage02Base::CSplashEff::GetEffId(long type) const
    {
        switch (m_nKind) {
            case 0:
                switch (type) {
                    case 0:
                        return 0x42D;
                    case 1:
                        return 0x430;
                }
                break;
            case 1:
                switch (type) {
                    case 0:
                        return 0x42E;
                    case 1:
                        return 0x431;
                }
                break;
            case 2:
                switch (type) {
                    case 0:
                        return 0x42F;
                    case 1:
                        return 0x432;
                }
                break;
        }
        return -1;
    }

    void CStage02Base::CSplashList::Main(float frame, const CStgGmk_WaveCluster *pCluster)
    {
        for (bcs::CListNodeBase *pNode = CListBase::GetFirst(); pNode != GetEnd(); bcs::CListNodeBase::Step(pNode)) {
            static_cast<CSplashEff *>(pNode)->Main(frame, pCluster);
        }
    }

    void CStage02Base::CSplashList::StopAll()
    {
        for (bcs::CListNodeBase *pNode = CListBase::GetFirst(); pNode != GetEnd(); bcs::CListNodeBase::Step(pNode)) {
            static_cast<CSplashEff *>(pNode)->StopAll();
        }
    }

    void CStage02Base::CSplashList::EraseAll()
    {
        for (bcs::CListNodeBase *pNode = CListBase::GetFirst(); pNode != GetEnd(); bcs::CListNodeBase::Step(pNode)) {
            static_cast<CSplashEff *>(pNode)->EraseAll();
        }
    }
}
