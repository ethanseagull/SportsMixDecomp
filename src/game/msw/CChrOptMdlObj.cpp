#include "game/msw/CChrOptMdlObj.hpp"
#include "game/msw/CChrOptTask.hpp"

#include <stdlib.h>
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // Shows or hides a model of the task (both hide bits of the scene
    // object).
    inline void SetHide(bcs::CScnObj *pObj, int bHide)
    {
        if (bHide) {
            pObj->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        } else {
            pObj->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        }
    }
}

namespace msw {

    // ---------------------------------------------------------------------
    // CChrOptMdlObj

    CChrOptMdlObj::CChrOptMdlObj()
        : m_pMdl(0), m_pShadow(0), m_pRef(0), m_pMotion(0), m_fFadeSpeed(0.0f), m_fAlpha(0.0f)
    {
    }

    CChrOptMdlObj::~CChrOptMdlObj()
    {
        if (m_pRef != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pRef);
        }
        m_pRef = 0;
        if (m_pShadow != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
        }
        m_pShadow = 0;
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
        m_pMotion = 0;
    }

    void CChrOptMdlObj::CreateObj(nw4r::g3d::ResMdl mdl)
    {
        if (mdl.IsValid()) {
            m_pMdl = new (0x100) bcs::CMdlObj(mdl, 0, 0);
            m_pMdl->SetExBlendMode(1);
            m_pMdl->OnFlag(0x2000);
            bcs::CRenderMan::PushBack(m_pMdl, 2);
        }
    }

    void CChrOptMdlObj::CreateShadow(nw4r::g3d::ResMdl mdl, long nArg)
    {
        if (mdl.IsValid()) {
            m_pShadow = new (0x100) bcs::CMdlShadow(m_pMdl, mdl, nArg, 0, 0);
            bcs::CRenderMan::PushBack(m_pShadow, 2);
        }
    }

    void CChrOptMdlObj::CreateRef(nw4r::g3d::ResMdl mdl)
    {
        if (mdl.IsValid()) {
            m_pRef = new (0x100) bcs::CMdlRef(m_pMdl, mdl, 0, 0, 0);
            bcs::CRenderMan::PushBack(m_pRef, 4);
        }
    }

    void CChrOptMdlObj::CreateAnm(nw4r::g3d::ResFile file, const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo)
    {
        if (file.IsValid()) {
            m_pMotion = new (0x100) bcs::CAnmMotion();
            m_pMotion->Create(m_pMdl, file, pInfo, 3, 0);
            m_pMdl->AttachAnmObj(m_pMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);
        }
    }

    void CChrOptMdlObj::RecalcMtx()
    {
        if (m_pMdl != 0) {
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
    }

    void CChrOptMdlObj::FadeIn_(float fFrames)
    {
        if (m_fFadeSpeed <= 0.0f) {
            if (fFrames <= 1.0f) {
                m_fFadeSpeed = 1.0f;
            } else {
                m_fFadeSpeed = 1.0f / fFrames;
            }
        }
    }

    void CChrOptMdlObj::FadeOut_(float fFrames)
    {
        if (m_fFadeSpeed >= 0.0f) {
            if (fFrames <= 1.0f) {
                m_fFadeSpeed = -1.0f;
            } else {
                m_fFadeSpeed = -1.0f / fFrames;
            }
        }
    }

    void CChrOptMdlObj::UpdateFade_(float fStep)
    {
        if (m_fFadeSpeed != 0.0f) {
            m_fAlpha += m_fFadeSpeed * fStep;
            m_fAlpha = Clamp(m_fAlpha, 0.0f, 1.0f);
            if (m_pMdl != 0) {
                m_pMdl->SetAlphaRateF(m_fAlpha);
            }
        }
    }

    void CChrOptMdlObj::AnimPlay(long nMotId, int bLoop, float fInterFrame, float fFrame)
    {
        if (m_pMotion != 0) {
            unsigned long nFlag = bLoop ? 2 : 0;
            if (nMotId < 0) {
                nFlag |= 1;
                nMotId = abs(nMotId);
            }
            m_pMotion->Play(nMotId, nFlag, 1.0f, fInterFrame, fFrame);
        }
    }

    void CChrOptMdlObj::AnimStop()
    {
        if (m_pMotion != 0) {
            m_pMotion->Stop();
        }
    }

    void CChrOptMdlObj::AnimSetSpeed(float fSpeed)
    {
        if (m_pMotion != 0) {
            m_pMotion->SetRate(fSpeed);
        }
    }

    float CChrOptMdlObj::AnimGetSpeed() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->GetRate();
        }
        return 1.0f;
    }

    float CChrOptMdlObj::AnimGetFrame() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->GetFrame();
        }
        return 0.0f;
    }

    float CChrOptMdlObj::AnimGetFrameNum() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc28();
        }
        return 0.0f;
    }

    float CChrOptMdlObj::AnimGetResFrameNum(long nMotId) const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc88(nMotId);
        }
        return 0.0f;
    }

    long CChrOptMdlObj::AnimGetMotID() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc64(3, 0);
        }
        return -1;
    }

    int CChrOptMdlObj::IsAnimResLoopType(long nMotId) const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc8C(nMotId);
        }
        return 0;
    }

    void CChrOptMdlObj::AnimSetFrame(float fFrame)
    {
        if (m_pMotion != 0) {
            m_pMotion->SetFrame(fFrame);
        }
    }

    int CChrOptMdlObj::IsAnimFlip() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc98(3, 0);
        }
        return 0;
    }

    int CChrOptMdlObj::IsAnimLoop() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFunc3C();
        }
        return 0;
    }

    int CChrOptMdlObj::IsAnimLooped() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->IsLooped();
        }
        return 0;
    }

    void CChrOptMdlObj::AnimResetLoopedFlag()
    {
        if (m_pMotion != 0) {
            m_pMotion->ResetLoopedFlag(3);
        }
    }

    int CChrOptMdlObj::IsAnimEnd() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->IsEnd();
        }
        return 1;
    }

    int CChrOptMdlObj::IsAnimInter() const
    {
        if (m_pMotion != 0) {
            return m_pMotion->VFuncA0(3);
        }
        return 0;
    }

    long CChrOptMdlObj::GetNodeIdx(const char *pName, long nDefault) const
    {
        if (m_pMdl != 0) {
            nw4r::g3d::ResNode node = m_pMdl->GetResMdl().GetResNode(pName);
            if (node.IsValid()) {
                return node.GetID();
            }
        }
        return nDefault;
    }

    // ---------------------------------------------------------------------
    // CChrOptTask

    CChrOptTask::CChrOptTask(bcs::CTask *pParent, int bFollow)
        : bcs::CUnivTaskEx(pParent, 0x15),
          m_nFlag(FLAG_START_FRAME0),
          m_hParent(0),
          m_nAppearEff(-1),
          m_nDisappearEff(-1),
          m_fDelay(0.0f),
          m_nMot(0),
          m_nAppearSe(-1),
          m_nDisappearSe(-1),
          m_nLoopSe(-1),
          m_hLoopSe(0)
    {
        if (bFollow) {
            m_nFlag |= FLAG_FOLLOW;
        }
        for (long i = 0; i < EFF_SLOT_NUM; i++) {
            m_aEffHandle[i] = 0;
            m_aEffId[i] = -1;
        }
    }

    CChrOptTask::~CChrOptTask()
    {
        Eff_StopAll();
        SE_StopLoop(0.0f);
    }

    inline void CChrOptTask::AttachTo_(const bcs::CScnObj *pParent, long nNode)
    {
        if (m_pMdl != 0) {
            m_pMdl->AttachToObject(pParent, nNode, bcs::CScnObj::ATTACHMODE_0);
        }
        if (pParent != 0) {
            m_hParent = pParent->GetHandle();
        }
    }

    void CChrOptTask::Create(const bcs::CScnObj *pParent, long nNode, nw4r::g3d::ResFile res, long nMdl,
                             long nShadow, long nRef, nw4r::g3d::ResFile mot,
                             const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo, long nShadowArg, long nAppearEff,
                             long nDisappearEff)
    {
        if (res.IsValid() && nMdl >= 0) {
            CreateObj(res.GetResMdl(static_cast<unsigned long>(nMdl)));
            CreateAnm(mot, pNodeExInfo);
            if (m_pMdl != 0 && nShadow >= 0) {
                CreateShadow(res.GetResMdl(static_cast<unsigned long>(nShadow)), nShadowArg);
            }
            if (m_pMdl != 0 && nRef >= 0) {
                CreateRef(res.GetResMdl(static_cast<unsigned long>(nRef)));
            }
            if (m_pMdl != 0 && pParent != 0 && nNode >= 0) {
                AttachTo_(pParent, nNode);
            }
            if (m_pMdl != 0) {
                m_nAppearEff = nAppearEff;
                m_nDisappearEff = nDisappearEff;
            }
            RecalcMtx();
        }
    }

    void CChrOptTask::Create(const bcs::CScnObj *pParent, long nNode, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot,
                             const char *pMdlName, const char *pShadowName, const char *pRefName,
                             const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo, long nShadowArg, long nAppearEff,
                             long nDisappearEff)
    {
        if (!res.IsValid()) {
            return;
        }
        nw4r::g3d::ResMdl mdl = res.GetResMdl(pMdlName);
        CreateObj(mdl);
        CreateAnm(mot, pNodeExInfo);
        if (m_pMdl != 0 && pShadowName != 0) {
            CreateShadow(res.GetResMdl(pShadowName), nShadowArg);
        }
        if (m_pMdl != 0 && pRefName != 0) {
            // The original looks pRefName up and then passes the main model.
            nw4r::g3d::ResMdl ref = res.GetResMdl(pRefName);
            CreateRef(mdl);
        }
        if (m_pMdl != 0 && pParent != 0 && nNode >= 0) {
            AttachTo_(pParent, nNode);
        }
        if (m_pMdl != 0) {
            m_nAppearEff = nAppearEff;
            m_nDisappearEff = nDisappearEff;
        }
        RecalcMtx();
    }

    // A fade in (out) may start unless one is already running; one that
    // is still waiting for its delay may be restarted. (guess: written with
    // a result variable, which the original's li 0 / li 1 shows)
    inline bool CChrOptTask::CanFadeIn_() const
    {
        bool bCan = false;
        if (m_fFadeSpeed <= 0.0f) {
            bCan = true;
        } else if (m_fDelay > 0.0f) {
            bCan = true;
        }
        return bCan;
    }

    inline bool CChrOptTask::CanFadeOut_() const
    {
        bool bCan = false;
        if (m_fFadeSpeed >= 0.0f) {
            bCan = true;
        } else if (m_fDelay > 0.0f) {
            bCan = true;
        }
        return bCan;
    }

    // The effect of a fade, in slot 8, which is then freed for the next one
    // (no effect while the models are hidden).
    inline void CChrOptTask::StartAppearEffect_()
    {
        if (!(m_nFlag & FLAG_HIDE)) {
            Eff_CreateAttachAuto(EFF_SLOT_FADE, m_nAppearEff, 3);
            m_aEffId[EFF_SLOT_FADE] = -1;
        }
    }

    inline void CChrOptTask::StartDisappearEffect_()
    {
        if (!(m_nFlag & FLAG_HIDE)) {
            Eff_CreateAttachAuto(EFF_SLOT_FADE, m_nDisappearEff, 3);
            m_aEffId[EFF_SLOT_FADE] = -1;
        }
    }

    // The sound of a fade, at the model's position.
    inline void CChrOptTask::StartFadeSe_(long nSe)
    {
        if (nSe >= 0 && m_pMdl != 0) {
            bcs::FMTX34 mtx;
            m_pMdl->GetMtx(bcs::CScnObj::SCNOBJMTXTYPE_1, mtx);
            bcs::FVEC3 pos;
            pos.x = mtx.m[0][3];
            pos.y = mtx.m[1][3];
            pos.z = mtx.m[2][3];
            CGameSnd::Play(nSe, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CChrOptTask::AnimPlay(long nMot, float fFrames, float fDelay)
    {
        if (CanFadeIn_()) {
            if (fFrames <= 1.0f) {
                m_fFadeSpeed = 1.0f;
            } else {
                m_fFadeSpeed = 1.0f / fFrames;
            }
            m_fDelay = 0.0f < fDelay ? fDelay : 0.0f;
            m_nMot = nMot;
            if (m_fDelay <= 0.0f) {
                StartAppearEffect_();
            }
            if (m_fDelay <= 0.0f) {
                StartFadeSe_(m_nAppearSe);
                SE_PlayLoop();
            }
        }
    }

    void CChrOptTask::AnimPlayOut(long nMot, float fFrames, float fDelay)
    {
        if (CanFadeOut_()) {
            if (fFrames <= 1.0f) {
                m_fFadeSpeed = -1.0f;
            } else {
                m_fFadeSpeed = -1.0f / fFrames;
            }
            m_fDelay = 0.0f < fDelay ? fDelay : 0.0f;
            m_nMot = nMot;
            if (m_fDelay <= 0.0f) {
                StartDisappearEffect_();
            }
            if (m_fDelay <= 0.0f) {
                StartFadeSe_(m_nDisappearSe);
                SE_StopLoop(5.0f);
            }
        }
    }

    void CChrOptTask::SetAppearSe(long nSe)
    {
        m_nAppearSe = nSe;
    }

    void CChrOptTask::SetDisappearSe(long nSe)
    {
        m_nDisappearSe = nSe;
    }

    void CChrOptTask::SetVisible(int bVisible)
    {
        if (bVisible) {
            m_nFlag &= ~FLAG_HIDE;
        } else {
            m_nFlag |= FLAG_HIDE;
        }
    }

    void CChrOptTask::SetLoopSe(long nSe)
    {
        m_nLoopSe = nSe;
    }

    void CChrOptTask::Main()
    {
        float fStep = GetFrameRateF();
        OnMain_();
        UpdateMotion_(fStep);
        UpdateFade(fStep);
        RecalcMtx();
    }

    void CChrOptTask::OnMain_()
    {
    }

    void CChrOptTask::UpdateFade(float fStep)
    {
        if (m_fFadeSpeed == 0.0f) {
            return;
        }
        if (m_fDelay > 0.0f) {
            if (m_nMot != 0) {
                float fFrame = AnimGetFrame();
                float fLast = AnimGetFrameNum() - 1.0f;
                if (fFrame < (fLast > m_fDelay ? m_fDelay : fLast)) {
                    return;
                }
            } else {
                m_fDelay -= fStep;
                if (m_fDelay > 0.0f) {
                    return;
                }
            }
            m_fDelay = 0.0f;
            if (m_fFadeSpeed > 0.0f) {
                StartAppearEffect_();
                StartFadeSe_(m_nAppearSe);
                SE_PlayLoop();
            } else {
                StartDisappearEffect_();
                StartFadeSe_(m_nDisappearSe);
                SE_StopLoop(5.0f);
            }
        }
        m_fAlpha += m_fFadeSpeed * fStep;
        m_fAlpha = Clamp(m_fAlpha, 0.0f, 1.0f);
        if (m_pMdl != 0) {
            m_pMdl->SetAlphaRateF(m_fAlpha);
            int bHide = (m_nFlag >> 2) & 1;
            SetHide(m_pMdl, bHide);
            if (m_pShadow != 0) {
                SetHide(m_pShadow, bHide);
            }
            if (m_pRef != 0) {
                SetHide(m_pRef, bHide);
            }
        }
        if (m_fAlpha <= 0.0f && m_fFadeSpeed < 0.0f) {
            Delete();
        }
    }

    void CChrOptTask::UpdateMotion_(float fStep)
    {
        if (m_pMdl == 0) {
            return;
        }
        if ((m_nFlag & FLAG_FOLLOW) && m_hParent != 0) {
            bcs::CMdlObj *pParent = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_hParent));
            if (pParent == 0) {
                Delete();
                return;
            }
            bcs::CAnmMotion *pMotion = static_cast<bcs::CAnmMotion *>(pParent->GetAnmObjChr());
            if (pMotion != 0) {
                long nMot = pMotion->VFunc64(3, 0);
                float fRate = pMotion->GetRate();
                int bFlip = pMotion->VFunc98(3, 0);
                if (nMot != AnimGetMotID() || bFlip != IsAnimFlip()) {
                    float fInterFrame = pMotion->VFuncA4(3);
                    if (bFlip) {
                        nMot = -nMot;
                    }
                    AnimPlay(nMot, 0, fInterFrame, 0.0f);
                }
                AnimSetSpeed(fRate);
            }
            m_pMdl->UpdateFrame(fStep);
            if (pMotion != 0) {
                AnimSetFrame(pMotion->GetFrame());
            }
            m_pMdl->UpdateFrame(0.0f);
            m_pMdl->SetSilhouetteColor(pParent->GetSilhouetteColor());
            m_pMdl->SetBaseColor(pParent->GetBaseColor());
        } else {
            m_pMdl->UpdateFrame(fStep);
        }
    }

    void CChrOptTask::CreateObj(nw4r::g3d::ResMdl mdl)
    {
        CChrOptMdlObj::CreateObj(mdl);
        if (m_pMdl != 0) {
            m_pMdl->OnFlag(0x100);
            m_pMdl->OnFlag(0x2);
            m_pMdl->SetExBlendMode(1);
            m_pMdl->OnFlag(0x2000);
            m_pMdl->SetAlphaRate(0);
        }
    }

    void CChrOptTask::AnimPlay(long nMotId, int bLoop, float fInterFrame, float fFrame)
    {
        if (m_pMotion != 0) {
            unsigned long nFlag = bLoop ? 2 : 0;
            if (nMotId < 0) {
                nFlag |= 1;
                nMotId = abs(nMotId);
            }
            if (m_nFlag & FLAG_START_FRAME0) {
                fInterFrame = 0.0f;
            }
            m_pMotion->Play(nMotId, nFlag, 1.0f, fInterFrame, fFrame);
            m_nFlag &= ~FLAG_START_FRAME0;
        }
    }

    void CChrOptTask::SE_PlayLoop()
    {
        SE_StopLoop(0.0f);
        if (m_nLoopSe >= 0 && m_pMdl != 0) {
            m_hLoopSe = CGameSnd::Play(m_nLoopSe, &m_pMdl->m_vTrans, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CChrOptTask::SE_StopLoop(float fFrames)
    {
        if (m_hLoopSe != 0) {
            bcs::CSndTask *pSnd = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hLoopSe));
            if (pSnd != 0 && !pSnd->IsDelete()) {
                pSnd->Stop(fFrames);
            }
            m_hLoopSe = 0;
        }
    }

    unsigned long CChrOptTask::Eff_CreateAttach(long nSlot, long nEff, unsigned long nNode, const bcs::FVEC3 *pOfs,
                                                unsigned long nFlag, unsigned long nArg)
    {
        if (nEff != m_aEffId[nSlot]) {
            Eff_Stop(nSlot);
            if (nEff >= 0) {
                m_aEffHandle[nSlot] = CEffectCtrl::CreateAttachEffect(nEff, m_pMdl, pOfs, nNode, nFlag & 1,
                                                                      (nFlag >> 1) & 1, 0, nArg, 0, 0, 0);
                m_aEffId[nSlot] = nEff;
            }
        }
        return m_aEffHandle[nSlot];
    }

    unsigned long CChrOptTask::Eff_CreateAttach(long nSlot, long nEff, const char *pNode, const bcs::FVEC3 *pOfs,
                                                unsigned long nFlag, unsigned long nArg)
    {
        long nNode = GetNodeIdx(pNode, -1);
        if (nNode >= 0) {
            return Eff_CreateAttach(nSlot, nEff, static_cast<unsigned long>(nNode), pOfs, nFlag, nArg);
        }
        Eff_Stop(nSlot);
        return 0;
    }

    unsigned long CChrOptTask::Eff_CreateAttachAuto(long nSlot, long nEff, unsigned long nArg)
    {
        if (nEff != m_aEffId[nSlot]) {
            Eff_Stop(nSlot);
            if (nEff >= 0) {
                m_aEffHandle[nSlot] = CEffectCtrl::CreateAttachEffectAuto(nEff, m_pMdl, nArg);
                m_aEffId[nSlot] = nEff;
            }
        }
        return m_aEffHandle[nSlot];
    }

    unsigned long CChrOptTask::Eff_GetHandle(long nSlot)
    {
        return m_aEffHandle[nSlot];
    }

    long CChrOptTask::Eff_GetId(long nSlot)
    {
        return m_aEffId[nSlot];
    }

    void CChrOptTask::Eff_SetSpeed(long nSlot, float fSpeed)
    {
        unsigned long hEffect = Eff_GetHandle(nSlot);
        if (hEffect != 0) {
            CEffectCluster *pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
            if (pCluster != 0) {
                pCluster->SetSpeedOverride(1, fSpeed);
            }
        }
    }

    int CChrOptTask::Eff_IsAlive(long nSlot)
    {
        unsigned long hEffect = Eff_GetHandle(nSlot);
        if (hEffect == 0) {
            return 0;
        }
        return CEffectCtrl::GetInstance()->FindList(hEffect) != 0;
    }

    void CChrOptTask::Eff_Stop(long nSlot)
    {
        if (m_aEffHandle[nSlot] != 0) {
            CEffectCtrl::StopAttachEffect(m_aEffHandle[nSlot]);
        }
        m_aEffHandle[nSlot] = 0;
        m_aEffId[nSlot] = -1;
    }

    void CChrOptTask::Eff_Erase(long nSlot)
    {
        if (m_aEffHandle[nSlot] != 0) {
            CEffectCtrl::EraseAttachEffect(m_aEffHandle[nSlot]);
        }
        m_aEffHandle[nSlot] = 0;
        m_aEffId[nSlot] = -1;
    }

    void CChrOptTask::Eff_StopAll()
    {
        for (long i = 0; i < EFF_SLOT_NUM; i++) {
            Eff_Stop(i);
        }
    }

    void CChrOptTask::Eff_EraseAll()
    {
        for (long i = 0; i < EFF_SLOT_NUM; i++) {
            Eff_Erase(i);
        }
    }
}
