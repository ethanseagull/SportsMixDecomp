// game/msw/CEvDemoMdlObj
// msw::CEvDemoMdlObj (.text 0x80220CA0..0x802219F0): an event model built
// from a plain model file, with an optional shadow.
#include "game/msw/CEvDemoMdlObj.hpp"
#include "game/msw/CEventMdlObj.hpp"
#include "game/msw/EvDemo.hpp"
#include <stdlib.h>
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/EF/CResource.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace msw {

    namespace {
        // Sets or clears flags 0xC0 of a scene object (not drawn). (helper
        // name guessed; the same one as in CEvDemoMdlChr.cpp)
        inline void SetHidden(bcs::CScnObj *pObj, int bHide)
        {
            if (bHide) {
                pObj->OnFlag(0xC0);
            } else {
                pObj->OffFlag(0xC0);
            }
        }
    }

    CEvDemoMdlObj::~CEvDemoMdlObj()
    {
        Destroy();
    }

    // Erases the effects and hands the model and its shadow over for
    // deletion.
    void CEvDemoMdlObj::Destroy()
    {
        EraseEffect(0, GetEffMax());
        if (m_pMdl != 0) {
            m_pMdl->AttachToObject(0, 0, static_cast<bcs::CScnObj::AttachMode>(0));
            m_pMdl->DetachFromParent();
        }
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
        if (m_pShadow != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
        }
        m_pShadow = 0;
        m_pMotion = 0;
    }

    long CEvDemoMdlObj::GetEffMax() const
    {
        return 4;
    }

    void CEvDemoMdlObj::SetDrawLayer(long nLayer)
    {
        m_pMdl->DetachFromParent();
        bcs::CRenderMan::PushBack(m_pMdl, nLayer);
    }

    void CEvDemoMdlObj::SetDrawPriority(long nPriority)
    {
        m_pMdl->SetDrawPriority(nPriority + 0x80, nPriority + 0x80);
    }

    // Without a group the model goes back to the default draw layer.
    void CEvDemoMdlObj::AppendChild(bcs::CScnGroup *pGroup)
    {
        m_pMdl->DetachFromParent();
        if (pGroup != 0) {
            pGroup->AppendChild(m_pMdl);
        } else {
            bcs::CRenderMan::PushBack(m_pMdl, 2);
        }
    }

    void CEvDemoMdlObj::_SetPos(const bcs::FVEC3 &rPos)
    {
        m_pMdl->m_vTrans = rPos;
    }

    void CEvDemoMdlObj::_SetRot(const bcs::FVEC3 &rRot)
    {
        m_pMdl->SetRotXYZ(rRot.x, rRot.y, rRot.z);
    }

    void CEvDemoMdlObj::_SetScale(const bcs::FVEC3 &rScale)
    {
        m_pMdl->m_vScale = rScale;
    }

    void CEvDemoMdlObj::SetShadowDisp(int bDisp)
    {
        if (m_pShadow != 0) {
            SetHidden(m_pShadow, !bDisp);
        }
    }

    // The model and its shadow are drawn while bDisp and bit 0x40 of the
    // flags are set.
    void CEvDemoMdlObj::SetDisp(int bDisp)
    {
        int bShow = 0;
        if (bDisp && (m_nFlag & 0x40)) {
            bShow = 1;
        }
        SetHidden(m_pMdl, !bShow);
        if (m_pShadow != 0) {
            SetHidden(m_pShadow, !bShow);
        }
        _SetFlag(0x80, bDisp);
    }

    // Steps the motion sequence (the one by name with bit 0x1000 of the
    // flags), fades the model (bit 0x200) and its shadow (bit 0x400) with
    // the alpha, hiding them at 0 (bit 0x100), and follows the floor and the
    // model attached to.
    void CEvDemoMdlObj::UpdateFrame(float fFrame)
    {
        if (m_pMdl == 0) {
            return;
        }
        if (m_pMotion != 0) {
            if (m_nFlag & 0x1000) {
                bool bSeq = false;
                if (m_pMotNameData != 0 && m_nMotIndex != -1) {
                    bSeq = true;
                }
                if (bSeq && _IsAnimEnd(m_pMdl, m_pMotNameData[m_nMotIndex].nFlag)) {
                    _NextMotionName(1, 0.0f);
                }
            } else {
                bool bSeq = false;
                if (m_pMotData != 0 && m_nMotIndex != -1) {
                    bSeq = true;
                }
                if (bSeq && m_pMotion->IsEnd()) {
                    _NextMotion(1, 0.0f);
                }
            }
        }
        _UpdateAlpha(fFrame);
        int bDisp = IsDisp();
        float fAlpha = m_fAlpha;
        if (fAlpha > 1.0f) {
            fAlpha = 1.0f;
        }
        if (fAlpha <= 0.0f) {
            fAlpha = 0.0f;
            bDisp = 0;
        }
        if (m_nFlag & 0x200) {
            if (fAlpha != m_pMdl->GetAlphaRateF()) {
                m_pMdl->SetAlphaRateF(fAlpha);
            }
            if (m_nFlag & 0x100) {
                SetHidden(m_pMdl, !bDisp);
            }
        }
        if (m_pShadow != 0) {
            if (m_nFlag & 0x400) {
                if (fAlpha != m_pShadow->GetAlpha()) {
                    m_pShadow->SetAlpha(fAlpha);
                }
                if (m_nFlag & 0x100) {
                    SetHidden(m_pShadow, !bDisp);
                }
            }
            _UpdateFloorHeight(m_pShadow);
        }
        _UpdateAttach();
    }

    void CEvDemoMdlObj::SetAnimSpeed(float fSpeed)
    {
        m_fAnimSpeed = fSpeed;
        if (m_pMotion != 0) {
            m_pMotion->SetRate(fSpeed);
        }
    }

    float CEvDemoMdlObj::AnimPlay(nw4r::g3d::ResFile file, const char *pName, unsigned long nFlag, float fBlend,
                                  float fFrame, int bStopSeq)
    {
        nw4r::g3d::ResFile res = file;
        float fLength = _AnimPlayRes(m_pMdl, res, pName, nFlag, fBlend, fFrame);
        if (bStopSeq) {
            m_pMotData = 0;
            m_nMotIndex = -1;
        }
        return fLength;
    }

    float CEvDemoMdlObj::AnimPlay(const char *pName, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq)
    {
        float fLength = _AnimPlayRes(m_pMdl, GetResFile(), pName, nFlag, fBlend, fFrame);
        if (bStopSeq) {
            m_pMotData = 0;
            m_nMotIndex = -1;
        }
        return fLength;
    }

    // As CEvDemoMdlChr::AnimPlay.
    float CEvDemoMdlObj::AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq)
    {
        long nAnim = nMot;
        if (!(m_nFlag & 0x800)) {
            nAnim = m_pMotTable[abs(nMot)];
            if (nMot < 0) {
                nFlag ^= 1;
            }
        }
        if (m_nAnim != nAnim) {
            m_pMotion->Play(nAnim, nFlag, m_fAnimSpeed, fBlend, fFrame);
            m_nAnim = nAnim;
        }
        if (bStopSeq) {
            m_pMotData = 0;
            m_nMotIndex = -1;
        }
        return m_pMotion->VFunc88(_GetResAnimNo(nMot));
    }

    int CEvDemoMdlObj::IsAnimResLoopType(long nMot) const
    {
        return m_pMotion->VFunc8C(_GetResAnimNo(nMot));
    }

    float CEvDemoMdlObj::AnimResFrame(long nMot) const
    {
        return m_pMotion->VFunc88(_GetResAnimNo(nMot));
    }

    float CEvDemoMdlObj::AnimResTrgFrame(long nMot, unsigned long nTrg, float f) const
    {
        return m_pMotion->SearchTrgFrame(_GetResAnimNo(nMot), nTrg, f);
    }

    const bcs::FMTX34 *CEvDemoMdlObj::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        if (nNode < m_pMdl->GetResNodeNum()) {
            return m_pMdl->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    long CEvDemoMdlObj::GetNodeId(const char *pName) const
    {
        return m_pMdl->GetNodeID(pName);
    }

    void CEvDemoMdlObj::SetLightSelector(long nSelector)
    {
        m_pMdl->SetLightSelector(nSelector);
    }

    void CEvDemoMdlObj::ResetLightSelector()
    {
        m_pMdl->ResetLightSelector();
    }

    void CEvDemoMdlObj::SetMatTevColor(const char *pMat, bcs::BCS_Color color, _GXTevRegID reg)
    {
        m_pMdl->SetMatTevColor(pMat, reg, color);
    }

    // The model's colours at 0x1A8 and 0x1AC (which name goes with which
    // is guessed).
    void CEvDemoMdlObj::SetBaseColor(bcs::BCS_Color color)
    {
        m_pMdl->SetSilhouetteColor(color);
    }

    void CEvDemoMdlObj::SetColorScale(bcs::BCS_Color color)
    {
        m_pMdl->SetBaseColor(color);
    }

    float CEvDemoMdlObj::GetNowAlpha() const
    {
        return m_pMdl->GetAlphaRateF();
    }

    void CEvDemoMdlObj::SetCameraView(long nView)
    {
        static_cast<CEventMdlObj *>(m_pMdl)->SetView(nView);
    }

    unsigned long CEvDemoMdlObj::GetEffcetHandle(long n) const
    {
        if (n >= 0 && n < 4) {
            return m_hEffect[n];
        }
        return 0;
    }

    void CEvDemoMdlObj::SetEffectHandle(unsigned long hEffect, long n)
    {
        m_hEffect[n] = hEffect;
    }

    long CEvDemoMdlObj::GetEffcetId(long n) const
    {
        if (n >= 0 && n < 4) {
            return m_nEffectId[n];
        }
        return -1;
    }

    void CEvDemoMdlObj::SetEffectId(long nId, long n)
    {
        m_nEffectId[n] = nId;
    }

    // bFlag sets bit 0x1000 of the flags: the model plays its sequences by
    // name (see UpdateFrame).
    CEvDemoMdlObj *CEvDemoMdlObj::CreateModel(const _SETUP &rSetup, int bFlag)
    {
        CEvDemoMdlObj *pModel = new CEvDemoMdlObj;

        pModel->m_nIndex = rSetup.nIndex;
        pModel->m_nType = rSetup.nType;
        pModel->m_pName = (const char *)rSetup.pInit;
        nw4r::g3d::ResFile motFile = *rSetup.pMotRes;
        pModel->m_mdlFile = *rSetup.pMdlRes;
        pModel->m_resFile = motFile;
        pModel->m_pMdl = pModel->_CreateMdlObj(pModel->m_pName, 0x1F);
        pModel->m_pMotion = pModel->_CreateAnmMotion(pModel->m_pMdl, 0x1F);
        pModel->m_nFloorHandle = pModel->m_pMdl->GetHandle();

        long nShadowSize = rSetup.nShadowSize;
        if (nShadowSize != -1) {
            nw4r::g3d::ResMdl mdl = pModel->GetResMdl();
            if (pModel->m_pMdl != 0 && mdl.IsValid()) {
                pModel->m_pShadow = new bcs::CMdlShadow(pModel->m_pMdl, mdl, nShadowSize, 0, 0);
            }
        }

        if (rSetup.nLightSelector != -1) {
            pModel->SetLightSelector(rSetup.nLightSelector);
        }
        pModel->SetDisp(rSetup.bDisp);
        if (rSetup.nDrawLayer != -1) {
            bcs::CRenderMan::PushBack(pModel->m_pMdl, rSetup.nDrawLayer);
            if (pModel->m_pShadow != 0) {
                bcs::CRenderMan::PushBack(pModel->m_pShadow, 2);
            }
        }
        if (bFlag) {
            pModel->m_nFlag |= 0x1000;
        }
        return pModel;
    }

    CEvDemoMdlObj *CEvDemoMdlObj::CreateModel(_SETUP &rSetup, long nArc, const char *pFile, const char *pName)
    {
        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, nArc, pFile, 0);

        rSetup.pMdlRes = &file;
        rSetup.pMotRes = &file;
        rSetup.pInit = (void *)pName;
        rSetup.nType = nIndex;
        return CreateModel(rSetup, 1);
    }

    // A model from file pFile of the effect resources.
    CEvDemoMdlObj *CEvDemoMdlObj::CreateModelEffRes(_SETUP &rSetup, const char *pFile, const char *pName)
    {
        nw4r::g3d::ResFile file = bcs::EF::CResource::GetInstance()->GetG3DResFile(pFile);

        rSetup.pMdlRes = &file;
        rSetup.pMotRes = &file;
        rSetup.pInit = (void *)pName;
        return CreateModel(rSetup, 1);
    }
}
