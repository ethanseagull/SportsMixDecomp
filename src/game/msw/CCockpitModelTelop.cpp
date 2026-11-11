// msw::CCockpitModelTelop: a telop drawn as an effect, base of each sport's
// model telop (CCockpitHockeyModelTelop and the others), and
// msw::CCockpitModelTelopHold, one that stays until it is told to go.
#include "game/msw/CCockpitModelTelop.hpp"
#include "game/msw/CCockpitModelTelopHold.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/EF/CEffect.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (names guessed) What each telop type shows; the base class knows one.
    struct TelopData {
        long nEffectId; // effect for slot 0
        long nSe;       // sound played with it, -1 for none
    };

    const TelopData s_TelopData[1] = {
        { 0x4D4, -1 },
    };
}

msw::CCockpitModelTelop::CCockpitModelTelop(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , m_nType(nType)
{
    m_hEffect[0] = 0;
    m_hEffect[1] = 0;
}

msw::CCockpitModelTelop::~CCockpitModelTelop()
{
    Remove();
}

void msw::CCockpitModelTelop::Main()
{
    unsigned long hEffect;
    switch (m_nStep) {
        case 1:
            m_nStep = 2;
            break;
        case 2: {
            // Ends once no effect of the telop plays any more.
            int bEnd = 1;
            for (long i = 0; i < 2; i++) {
                hEffect = m_hEffect[i];
                if (hEffect != 0) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                    if (pCluster != 0 && pCluster->IsValid()) {
                        bEnd = 0;
                        break;
                    }
                }
            }
            if (bEnd == 1) {
                m_Unk08 = 2;
                m_nStep = 0;
            }
            break;
        }
    }
}

void msw::CCockpitModelTelop::Disp()
{
}

int msw::CCockpitModelTelop::Set()
{
    if (m_nType >= 0 && m_nType < 1) {
        const TelopData &rData = s_TelopData[m_nType];

        SetEffect(0, rData.nEffectId);
        if (rData.nSe >= 0) {
            CGameSnd::Play(rData.nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_nStep = 1;
    }
    return 0;
}

void msw::CCockpitModelTelop::Remove()
{
    m_nStep = 0;
    for (long i = 0; i < 2; i++) {
        CEffectCtrl::EraseAttachEffect(m_hEffect[i]);
        m_hEffect[i] = 0;
    }
}

long msw::CCockpitModelTelop::SendEvent(long nEvent, void *pParam)
{
    long nResult = 0;
    unsigned long hEffect = m_hEffect[0];

    switch (nEvent) {
        case 0x2715: {
            // (meaning guessed) The value of the first model's animation.
            CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
            if (pCluster != 0 && pCluster->GetMdlObjNum() > 0) {
                bcs::CMdlObj *pMdlObj = pCluster->GetMdlObj(0);
                if (pMdlObj != 0) {
                    bcs::CAnmObj *pAnmObj = pMdlObj->GetAnmObjChr();
                    if (pAnmObj != 0) {
                        nResult = (long)pAnmObj->VFunc28();
                        break;
                    }
                }
            }
            nResult = 0;
            break;
        }
        case 0x2716:
            // pParam points at the new animation rate.
            if (pParam != 0) {
                SetAnmRate(*(float *)pParam);
            }
            break;
    }
    return nResult;
}

void msw::CCockpitModelTelop::SetEffect(long nSlot, long nEffectId)
{
    if (nSlot >= 0 && nSlot < 2) {
        CEffectCtrl::EraseAttachEffect(m_hEffect[nSlot]);
        m_hEffect[nSlot] = 0;

        unsigned long hEffect = CEffectCtrl::CreateEffect(nEffectId, 4, 0, 0, 0);
        CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
        if (pCluster != 0) {
            pCluster->CallEffectFunc(&bcs::EF::CEffect::Set2DFlag, 1);
        }
        m_hEffect[nSlot] = hEffect;
    }
}

void msw::CCockpitModelTelop::SetAnmRate(float fRate)
{
    CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect[0]);
    if (pCluster != 0) {
        pCluster->CallEffectFunc(&bcs::EF::CEffect::SetAnmRate, fRate);
    }
}

namespace {
    // (name guessed) The effect each CCockpitModelTelopHold type shows.
    const long s_HoldEffectId[10] = {
        0x4CC, 0x4C9, 0x4CA, 0x4CB, 0x4CD, 0x4CE, 0x4CF, 0x4D3, 0x4D5, 0x4D6,
    };
}

msw::CCockpitModelTelopHold::CCockpitModelTelopHold(long nId, long nType, float fSortZ, long nUnk)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
    , m_nType(nType)
    , m_nUnk7C(nUnk)
    , m_nEffectId(-1)
{
    m_Unk78 = 0;
    m_hEffect[0] = 0;
    m_hEffect[1] = 0;
}

msw::CCockpitModelTelopHold::~CCockpitModelTelopHold()
{
    Remove();
}

void msw::CCockpitModelTelopHold::Main()
{
    switch (m_nState) {
        case 1:
            m_nState = 3;
            break;
        case 2:
            break;
        case 3:
            // Once the telop has played in and may leave, starts the
            // effect that takes it out.
            if (IsAnmEnd() && IsWaitFrameOut() == 1) {
                SetEffect(1, m_nEffectId, m_nUnk7C + 1, 1, 1.0f);
                m_nState = 4;
            }
            break;
        case 4:
            CEffectCtrl::EraseAttachEffect(m_hEffect[0]);
            m_hEffect[0] = 0;
            m_nState = 5;
            break;
        case 5:
            if (IsAnmEnd()) {
                m_nState = 0;
                m_Unk08 = 2;
            }
            break;
        default:
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitModelTelopHold::Disp()
{
}

int msw::CCockpitModelTelopHold::Set()
{
    m_nEffectId = s_HoldEffectId[m_nType];
    SetEffect(0, m_nEffectId, m_nUnk7C, 1, 1.0f);
    SetWaitFrameOut(1);
    m_nStep = 1;
    m_nState = 1;
    return 0;
}

void msw::CCockpitModelTelopHold::Remove()
{
    m_nStep = 0;
    m_nState = 0;
    for (long i = 0; i < 2; i++) {
        CEffectCtrl::EraseAttachEffect(m_hEffect[i]);
        m_hEffect[i] = 0;
    }
}

long msw::CCockpitModelTelopHold::SendEvent(long nEvent, void *pParam)
{
    long nResult = 0;

    switch (nEvent) {
        case 0x2712:
            // (meaning guessed) Lets a telop that is on screen leave.
            if ((m_nState < 4 || m_nState > 5) && m_nState != 0) {
                SetWaitFrameOut(1);
                m_nState = 3;
            }
            break;
        case 0x2713:
            SetWaitFrameOut(pParam != 0);
            break;
        case 0x2715: {
            // (meaning guessed) The value of the first model's animation.
            CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect[0]);
            if (pCluster != 0 && pCluster->GetMdlObjNum() > 0) {
                bcs::CMdlObj *pMdlObj = pCluster->GetMdlObj(0);
                if (pMdlObj != 0) {
                    bcs::CAnmObj *pAnmObj = pMdlObj->GetAnmObjChr();
                    if (pAnmObj != 0) {
                        nResult = (long)pAnmObj->VFunc28();
                        break;
                    }
                }
            }
            nResult = 0;
            break;
        }
        case 0x2716:
            // pParam points at the new animation rate.
            if (pParam != 0) {
                SetAnmRate(*(float *)pParam);
            }
            break;
    }
    return nResult;
}

void msw::CCockpitModelTelopHold::SetEffect(long nSlot, long nEffectId, long nUnk, int bDisp, float fRate)
{
    if (nSlot >= 0 && nSlot < 2) {
        CEffectCtrl::EraseAttachEffect(m_hEffect[nSlot]);
        m_hEffect[nSlot] = 0;

        unsigned long hEffect = CEffectCtrl::CreateEffect(nEffectId, 4, 0, nUnk, 0);
        CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
        if (pCluster != 0) {
            pCluster->CallEffectFunc(&bcs::EF::CEffect::Set2DFlag, 0);
            pCluster->CallEffectFunc(&bcs::EF::CEffect::SetDispFlag, bDisp);
            pCluster->CallEffectFunc(&bcs::EF::CEffect::SetAnmRate, fRate);
        }
        m_hEffect[nSlot] = hEffect;
    }
}

void msw::CCockpitModelTelopHold::SetAnmRate(float fRate)
{
    CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect[0]);
    if (pCluster != 0) {
        pCluster->CallEffectFunc(&bcs::EF::CEffect::SetAnmRate, fRate);
    }
}
