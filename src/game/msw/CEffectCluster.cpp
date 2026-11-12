#include "game/msw/CEffectCluster.hpp"

#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include <string.h>

namespace msw {

    int CEffectCluster::IsValid()
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        for (long i = 0; i < m_nEffectNum; i++) {
            if (pManager->IsExist(m_Effect[i].m_hEffect)) {
                return 1;
            }
        }
        return 0;
    }

    long CEffectCluster::GetMdlObjNum()
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        long nNum = 0;
        long nEffectNum = m_nEffectNum;
        for (long i = 0; i < nEffectNum; i++) {
            bcs::EF::CEffect *pEffect = pManager->GetEffect(m_Effect[i].m_hEffect);
            if (pEffect != 0 && pEffect->GetMdlObj() != 0) {
                nNum++;
            }
        }
        return nNum;
    }

    bcs::CMdlObj *CEffectCluster::GetMdlObj(long nIndex)
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        long nNum = 0;
        long nEffectNum = m_nEffectNum;
        for (long i = 0; i < nEffectNum; i++) {
            bcs::EF::CEffect *pEffect = pManager->GetEffect(m_Effect[i].m_hEffect);
            if (pEffect != 0) {
                bcs::CMdlObj *pMdl = pEffect->GetMdlObj();
                if (pMdl != 0) {
                    if (nIndex == nNum) {
                        return pMdl;
                    }
                    nNum++;
                }
            }
        }
        return 0;
    }

    long CEffectCluster::GetEffectNum()
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        long nNum = 0;
        long nEffectNum = m_nEffectNum;
        for (long i = 0; i < nEffectNum; i++) {
            if (pManager->GetEffect(m_Effect[i].m_hEffect) != 0) {
                nNum++;
            }
        }
        return nNum;
    }

    bcs::EF::CEffect *CEffectCluster::GetEffect(long nIndex)
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        return pManager->GetEffect(m_Effect[nIndex].m_hEffect);
    }

    bcs::EF::CEffect *CEffectCluster::FindEffect(const char *pName)
    {
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        long nEffectNum = m_nEffectNum;
        for (long i = 0; i < nEffectNum; i++) {
            bcs::EF::CEffect *pEffect = pManager->GetEffect(m_Effect[i].m_hEffect);
            if (pEffect == 0) {
                continue;
            }
            if (pEffect->GetType() == 1) {
                // A model effect: compare the model's name.
                bcs::CMdlObj *pMdl = pEffect->GetMdlObj();
                if (pMdl == 0) {
                    continue;
                }
                nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
                if (!mdl.IsValid()) {
                    continue;
                }
                if (strcmp(mdl.GetName(), pName) == 0) {
                    return pEffect;
                }
            } else if (pEffect->HasEmitter(pName)) {
                return pEffect;
            }
        }
        return 0;
    }

    void CEffectCluster::SetSpeedOverride(int bOn, float fSpeed)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetSpeedOverride, bOn, fSpeed);
    }

    void CEffectCluster::SetSpeed(float fSpeed)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetAnmRate, fSpeed);
    }

    void CEffectCluster::SetZSortOffset(float fOffset)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetZSortOffset, fOffset);
    }

    void CEffectCluster::SetScale(const bcs::FVEC3 *pScale)
    {
        if (pScale != 0) {
            CallEffectFunc(&bcs::EF::CEffect::SetScale, *pScale);
            CallEffectFunc(&bcs::EF::CEffect::SetParticleScale, *pScale);
        }
    }

    void CEffectCluster::SetEmitterScale(const bcs::FVEC3 *pScale)
    {
        if (pScale != 0) {
            CallEffectFunc(&bcs::EF::CEffect::SetScale, *pScale);
        }
    }

    void CEffectCluster::SetTranslate(const bcs::FVEC3 *pPos)
    {
        if (pPos != 0) {
            CallEffectFunc(&bcs::EF::CEffect::SetLocalTranslate, *pPos);
        }
    }

    void CEffectCluster::SetRotateRad(const bcs::FVEC3 *pRot)
    {
        if (pRot != 0) {
            CallEffectFunc(&bcs::EF::CEffect::SetRotateRad, *pRot);
        }
    }

    void CEffectCluster::SetFadeoutTime(float fFrame)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetFadeoutTime, fFrame);
    }

    void CEffectCluster::SetFadeinTime(float fFrame)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetFadeinTime, fFrame);
    }

    void CEffectCluster::SetPrimaryColor(_GXColor color)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetPrimaryColor, color);
    }

    void CEffectCluster::SetPointLightColorPreset(long nPreset)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetPointLightColorPreset, nPreset);
    }

    void CEffectCluster::SetSecondaryColor(_GXColor color)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetSecondaryColor, color);
    }

    void CEffectCluster::SetPointLightColor(_GXColor color0, _GXColor color1)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetPointLightColor, color0, color1);
    }

    void CEffectCluster::Set2D(int b2D)
    {
        CallEffectFunc(&bcs::EF::CEffect::Set2DFlag, b2D);
    }

    void CEffectCluster::SetAlpha(float fAlpha)
    {
        CallEffectFunc(&bcs::EF::CEffect::SetAlpha, fAlpha);
    }
}
