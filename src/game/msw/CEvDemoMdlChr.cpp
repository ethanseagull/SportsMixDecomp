// msw::CEventChrCore and msw::CEvDemoMdlChr (.text 0x80116308..0x801177F4).
// A separate file from CEventModel.cpp: its own .sdata2 pool (0x805C2A50) and
// it calls CEventModel members that CEventModel.cpp inlines.

#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CEventMoveAction.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEvDemoMdlChr.hpp"
#include "game/msw/CEventCamera.hpp"
#include "game/msw/CEventNodeKey.hpp"
#include <stdlib.h>
#include <string.h>
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmShp.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEventNodeSimple.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/mtx.h"

// From here on (0x80116308..0x801177F4) the original was most likely a second
// file: its .sdata2 constants (0x805C2A50..0x805C2A5C) are a pool of their
// own, CEventModel's weak inline virtuals end just before it, and it calls
// CEventModel's functions instead of inlining them.

namespace msw {

    CEvDemoMdlChr::CEvDemoMdlChr(const short *pMotTable) : CEventModel(pMotTable), m_pInit(0), m_pMdl(0), m_pMotion(0)
    {
        for (int i = 0; i < 4; i++) {
            m_hEffect[i] = 0;
        }
        for (int i = 0; i < 4; i++) {
            m_hSe[i] = 0;
        }
    }

    CEvDemoMdlChr::~CEvDemoMdlChr()
    {
        Destroy();
    }

    // Stops the effects and sounds and hands the model over for deletion.
    void CEvDemoMdlChr::Destroy()
    {
        EraseEffect(0, GetEffMax());
        StopSe(0.1f, 0, GetSeMax());
        if (m_pMdl != 0) {
            m_pMdl->DetachFromParent();
            m_pMdl->AttachToObject(0, 0, static_cast<bcs::CScnObj::AttachMode>(0));
        }
        if (m_pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
        m_pMotion = 0;
    }

    void CEvDemoMdlChr::SetDrawLayer(long nLayer)
    {
        m_pMdl->SetDrawLayer(nLayer);
    }

    void CEvDemoMdlChr::SetDrawPriority(long) {}

    void CEvDemoMdlChr::AppendChild(bcs::CScnGroup *pGroup)
    {
        m_pMdl->AppendToGroup(pGroup);
    }

    void CEvDemoMdlChr::_SetPos(const bcs::FVEC3 &rPos)
    {
        m_pMdl->m_vTrans = rPos;
    }

    void CEvDemoMdlChr::_SetRot(const bcs::FVEC3 &rRot)
    {
        m_pMdl->SetRotXYZ(rRot.x, rRot.y, rRot.z);
    }

    void CEvDemoMdlChr::_SetScale(const bcs::FVEC3 &rScale)
    {
        m_pMdl->m_vScale = rScale;
    }

    // The shadow is drawn while the model is shown and bDisp is set.
    void CEvDemoMdlChr::SetShadowDisp(int bDisp)
    {
        int bShow = 0;
        if ((m_nFlag & 0x80) && (m_nFlag & 0x40)) {
            bShow = 1;
        }
        int bShadow = 0;
        if (bDisp && (m_nFlag & 0x40)) {
            bShadow = 1;
        }
        m_pMdl->SetDispEnable(bShow, bShadow);
    }

    // A hidden model with the star effect loses the effect.
    void CEvDemoMdlChr::SetDisp(int bDisp)
    {
        _SetFlag(0x80, bDisp);
        int bShow = 0;
        if (bDisp && (m_nFlag & 0x40)) {
            bShow = 1;
        }
        m_pMdl->SetDispEnable(bShow, bShow);
        if ((m_nFlag & 0x10000) && !bShow) {
            EraseEffect(0, 1);
        }
    }

    namespace {
        // Sets or clears flags 0xC0 of a scene object (not drawn). (helper
        // name guessed)
        inline void SetHidden(bcs::CScnObj *pObj, int bHide)
        {
            if (bHide) {
                pObj->OnFlag(0xC0);
            } else {
                pObj->OffFlag(0xC0);
            }
        }
    }

    // Steps the motion sequence, fades the model (bit 0x200) and its shadow
    // (bit 0x400) with the alpha, hiding them at 0 (bit 0x100), and follows
    // the floor and the model attached to.
    void CEvDemoMdlChr::UpdateFrame(float fFrame)
    {
        if (m_pMdl == 0 || m_pMotion == 0) {
            return;
        }
        _UpdateEffect();
        _UpdateSe();
        bool bSeq = false;
        if (m_pMotData != 0 && m_nMotIndex != -1) {
            bSeq = true;
        }
        if (bSeq && m_pMotion->IsEnd()) {
            _NextMotion(1, 0.0f);
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
        bcs::CMdlShadow *pShadow = m_pMdl->GetShadow();
        if (pShadow != 0) {
            if (m_nFlag & 0x400) {
                if (fAlpha != pShadow->GetAlpha()) {
                    pShadow->SetAlpha(fAlpha);
                }
                if (m_nFlag & 0x100) {
                    SetHidden(pShadow, !bDisp);
                }
            }
            _UpdateFloorHeight(pShadow);
        }
        _UpdateAttach();
    }

    bcs::CScnObj *CEvDemoMdlChr::GetScnObj()
    {
        return m_pMdl;
    }

    const bcs::CScnObj *CEvDemoMdlChr::GetScnObj() const
    {
        return m_pMdl;
    }

    const bcs::FMTX34 *CEvDemoMdlChr::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        if (nNode < m_pMdl->GetResNodeNum()) {
            return m_pMdl->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    long CEvDemoMdlChr::GetNodeId(const char *pName) const
    {
        return m_pMdl->GetNodeID(pName);
    }

    void CEvDemoMdlChr::SetLightSelector(long nSelector)
    {
        m_pMdl->SetLightSelector(nSelector);
    }

    void CEvDemoMdlChr::ResetLightSelector()
    {
        m_pMdl->ResetLightSelector();
    }

    void CEvDemoMdlChr::ChangeAnimFile(nw4r::g3d::ResFile file)
    {
        m_resFile = file;
        m_pMotion->ChangeResFile(file);
        m_nAnim = -1;
        m_pMotData = 0;
        m_nMotIndex = -1;
    }

    void CEvDemoMdlChr::SetAnimFrame(float fFrame, int bStop)
    {
        if (fFrame >= 0.0f) {
            m_pMotion->SetFrame(fFrame);
        }
        if (bStop) {
            m_pMotion->Stop();
        }
    }

    void CEvDemoMdlChr::SetAnimSpeed(float fSpeed)
    {
        m_fAnimSpeed = fSpeed;
        m_pMotion->SetRate(fSpeed);
    }

    int CEvDemoMdlChr::IsAnimResLoopType(long nMot) const
    {
        return m_pMotion->VFunc8C(_GetResAnimNo(nMot));
    }

    float CEvDemoMdlChr::AnimResFrame(long nMot) const
    {
        return m_pMotion->VFunc88(_GetResAnimNo(nMot));
    }

    float CEvDemoMdlChr::AnimResTrgFrame(long nMot, unsigned long nTrg, float f) const
    {
        return m_pMotion->SearchTrgFrame(_GetResAnimNo(nMot), nTrg, f);
    }

    // Through the motion table a negative motion number plays the motion
    // mirrored (bit 0 of nFlag). A motion already playing is not restarted.
    float CEvDemoMdlChr::AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq)
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

    float CEvDemoMdlChr::GetNowAlpha() const
    {
        return m_pMdl->GetAlphaRateF();
    }

    bcs::CSndTask *CEvDemoMdlChr::GetSndTaskSe(long n)
    {
        if (n >= 0 && n < 4) {
            unsigned long hSe = m_hSe[n];
            if (hSe == 0) {
                return 0;
            }
            return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(hSe));
        }
        return 0;
    }

    void CEvDemoMdlChr::SetSndTaskSe(long n, unsigned long hSe)
    {
        m_hSe[n] = hSe;
    }

    unsigned long CEvDemoMdlChr::GetEffcetHandle(long n) const
    {
        if (n >= 0 && n < 4) {
            return m_hEffect[n];
        }
        return 0;
    }

    void CEvDemoMdlChr::SetEffectHandle(unsigned long hEffect, long n)
    {
        m_hEffect[n] = hEffect;
    }

    void CEvDemoMdlChr::CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser)
    {
        if (pUser != 0) {
            static_cast<CEvDemoMdlChr *>(pUser)->_OnMotTrg(pTrg);
        }
    }

    inline void CEvDemoMdlChr::_PlayFootSe(const char *pNode, long nVar, long nSlot)
    {
        long nChr = m_nType;
        long nNode = m_pMdl->GetNodeID(pNode);
        if (nNode != -1) {
            bcs::FMTX34 mtx;
            m_pMdl->GetScnMtxPos(&mtx, nNode, bcs::CScnObj::SCNOBJMTXTYPE_1);
            bcs::FVEC3 vPos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
            StopSe(0.0f, nSlot, nSlot + 1);
            SetSndTaskSe(nSlot, CGameSnd::PlayChrSeA(nChr, 0, nVar, vPos, 1, -1, 0.0f, 1.0f, 0.0f));
        }
    }

    // Trigger kinds: 0x30 a voice, 0x40 the face, 0x10 to 0x12 foot sounds
    // (while bit 0x20000 of the flags is set).
    void CEvDemoMdlChr::_OnMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
    {
        switch (pTrg->m_nType) {
            case 0x30: {
                long nVoice = GetVoiceId(pTrg->m_nParam1);
                if (nVoice != -1) {
                    PlaySe(nVoice, 2, 0.0f, 1.0f, 0.0f);
                }
                break;
            }
            case 0x40:
                if (m_pMdl != 0) {
                    m_pMdl->SetFaceAnmType(pTrg->m_nParam0 + 1);
                }
                break;
            case 0x10:
                if ((m_nFlag & 0x20000) && pTrg->m_nParam0 != 2) {
                    if (pTrg->m_nParam0 == 1) {
                        _PlayFootSe("trans", 0, pTrg->m_nParam1);
                    } else {
                        _PlayFootSe("trans", 1, pTrg->m_nParam1);
                    }
                }
                break;
            case 0x11:
                if (m_nFlag & 0x20000) {
                    _PlayFootSe("trans", 3, 0);
                }
                break;
            case 0x12:
                if (m_pFootSeTable != 0 && (m_nFlag & 0x20000)) {
                    _PlayNodeSe("trans", m_pFootSeTable[3], 0);
                }
                break;
        }
    }

    void CEvDemoMdlChr::_PlayNodeSe(const char *pNode, long nSe, long nSlot)
    {
        if (nSe == -1) {
            return;
        }
        long nNode = m_pMdl->GetNodeID(pNode);
        if (nNode == -1) {
            return;
        }
        bcs::FMTX34 mtx;
        m_pMdl->GetScnMtxPos(&mtx, nNode, bcs::CScnObj::SCNOBJMTXTYPE_1);
        bcs::FVEC3 vPos;
        vPos.x = mtx.m[0][3];
        vPos.y = mtx.m[1][3];
        vPos.z = mtx.m[2][3];
        StopSe(0.0f, nSlot, nSlot + 1);
        unsigned long hSe = CGameSnd::Play(nSe, vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        SetSndTaskSe(nSlot, hSe);
    }

    CEvDemoMdlChr *CEvDemoMdlChr::CreateModel(const _SETUP &rSetup, long nKind)
    {
        CEvDemoMdlChr *pModel = new CEvDemoMdlChr(rSetup.pMotTable);
        pModel->m_nKind = nKind;
        pModel->m_nIndex = rSetup.nIndex;
        pModel->m_nType = rSetup.nType;
        pModel->m_pInit = rSetup.pInit;
        pModel->m_mdlFile = *rSetup.pMdlRes;
        pModel->m_resFile = *rSetup.pMotRes;
        pModel->m_pMdl = new (0) CChrMdlObj;
        if (pModel->m_nKind == 2) {
            pModel->m_pMdl->CreateEventMiiObj(pModel->m_mdlFile, pModel->m_nType, pModel->m_pInit, 0);
        } else {
            pModel->m_pMdl->CreateEventObj(pModel->m_mdlFile, pModel->m_nType, pModel->m_pInit, 0);
        }
        pModel->m_pMdl->OnFlag(0x4000);
        const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo = CC_DATA::GetMdl_NodeExInfo(pModel->m_nType, 0);
        pModel->m_pMotion = new (0) bcs::CAnmMotion;
        pModel->m_pMotion->Create(pModel->m_pMdl, pModel->m_resFile, pInfo, 3, 0);
        pModel->m_pMdl->AttachAnmObj(pModel->m_pMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);
        pModel->m_pMotion->SetTrgCallback(CallbackMotTrg, pModel);
        pModel->m_nFloorHandle = pModel->m_pMdl->GetHandle();
        if (rSetup.nLightSelector != -1) {
            pModel->SetLightSelector(rSetup.nLightSelector);
        }
        pModel->SetDisp(rSetup.bDisp);
        pModel->m_pMdl->SetDrawLayer(rSetup.nDrawLayer);
        return pModel;
    }
}

namespace msw {

    namespace {
        // The animation slot of the character the event motions play in.
        const unsigned long EVENT_ANIM_SLOT = 3;
    }

    CEventChrCore::CEventChrCore(long nType, long nPlayer, const short *pMotTable)
        : CEventModel(pMotTable)
    {
        m_pChr = CChrMan::GetChrCore(nPlayer);
        m_nType = nType;
        m_nIndex = nPlayer;
    }

    CEventChrCore::~CEventChrCore()
    {
        Destroy();
    }

    // Gives the character back its normal sound volumes.
    void CEventChrCore::Destroy()
    {
        if (CChrMan::IsActive() && m_nIndex != -1) {
            CChrCore *pChr = CChrMan::GetChrCore(m_nIndex);
            if (pChr != 0) {
                pChr->SetSeVolume(1.0f);
                pChr->SetVoiceVolume(1.0f, 0.0f);
            }
        }
        m_pChr = 0;
    }

    // Starts from where the character stands.
    void CEventChrCore::Create()
    {
        m_vPos = m_pChr->GetPos();
        m_vRot = m_pChr->GetRot();
        m_vScale.x = 1.0f;
        m_vScale.y = 1.0f;
        m_vScale.z = 1.0f;
    }

    // The character is drawn only while both the event shows it and bit
    // 0x40 of the flags is set.
    void CEventChrCore::SetDisp(int bDisp)
    {
        bool bShow = false;
        if (bDisp && (m_nFlag & 0x40)) {
            bShow = true;
        }
        CChrCore *pChr = m_pChr;
        if (!bShow) {
            pChr->OnDispFlag(4);
        } else {
            pChr->OffDispFlag(4);
        }
        if (bDisp) {
            m_nFlag |= 0x80;
        } else {
            m_nFlag &= ~0x80;
        }
    }

    void CEventChrCore::_UpdateFlags()
    {
        m_pChr->SetEnableFlagEC23((m_nFlag >> 18) & 1);
        m_pChr->SetEnableFlagEC24((m_nFlag >> 19) & 1);
    }

    // Steps the motion sequence on when the character's motion has ended.
    void CEventChrCore::UpdateFrame(float)
    {
        bool bSeq = false;
        if (m_pMotData != 0 && m_nMotIndex != -1) {
            bSeq = true;
        }
        if (bSeq && m_pChr->IsAnimEnd(EVENT_ANIM_SLOT)) {
            _NextMotion(1, 0.0f);
        }
    }

    void CEventChrCore::ChangeAnimFile(nw4r::g3d::ResFile) {}

    void CEventChrCore::SetAnimFrame(float, int) {}

    void CEventChrCore::SetAnimSpeed(float fSpeed)
    {
        m_fAnimSpeed = fSpeed;
        m_pChr->AnimSetSpeed(EVENT_ANIM_SLOT, m_fAnimSpeed);
    }

    int CEventChrCore::IsAnimResLoopType(long nMot) const
    {
        return m_pChr->IsAnimResLoopType(_GetResAnimNo(nMot));
    }

    float CEventChrCore::AnimResFrame(long nMot) const
    {
        return m_pChr->AnimGetResFrameNum(_GetResAnimNo(nMot));
    }

    float CEventChrCore::AnimResTrgFrame(long, unsigned long, float) const
    {
        return 0.0f;
    }

    // Bit 0 of nFlag plays the mirrored motion; bit 1 goes on to
    // CChrCore::AnimPlay. A motion already playing is not restarted.
    float CEventChrCore::AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq)
    {
        int bFlag2 = (nFlag >> 1) & 1;
        long nAnim = nMot;
        if (!(m_nFlag & 0x800)) {
            nAnim = m_pMotTable[abs(nMot)];
        }
        if (nFlag & 1) {
            nAnim = -nAnim;
        }
        if (m_nAnim != nAnim) {
            m_pChr->AnimPlay(EVENT_ANIM_SLOT, nAnim, bFlag2, fBlend, fFrame);
            m_pChr->AnimSetSpeed(EVENT_ANIM_SLOT, m_fAnimSpeed);
            m_nAnim = nAnim;
        }
        if (bStopSeq) {
            m_pMotData = 0;
            m_nMotIndex = -1;
        }
        return m_pChr->AnimGetResFrameNum(_GetResAnimNo(nMot));
    }

    void CEventChrCore::_SetPos(const bcs::FVEC3 &rPos)
    {
        m_pChr->SetPos(rPos);
    }

    void CEventChrCore::_SetRot(const bcs::FVEC3 &rRot)
    {
        m_pChr->SetRot(rRot);
    }

    void CEventChrCore::_SetScale(const bcs::FVEC3 &) {}

    bcs::CScnObj *CEventChrCore::GetScnObj()
    {
        return m_pChr->GetMdl();
    }

    const bcs::CScnObj *CEventChrCore::GetScnObj() const
    {
        return m_pChr->GetMdl();
    }

    long CEventChrCore::GetNodeId(const char *pName) const
    {
        bcs::CMdlObj *pMdl = m_pChr->GetMdl();
        if (pMdl != 0) {
            return pMdl->GetNodeID(pName);
        }
        return -1;
    }

    const bcs::FMTX34 *CEventChrCore::GetNodeWorldMtxPtr(unsigned long nNode) const
    {
        bcs::CMdlObj *pMdl = m_pChr->GetMdl();
        if (pMdl != 0 && nNode < pMdl->GetResNodeNum()) {
            return pMdl->GetNodeWorldMtxPtr(nNode);
        }
        return 0;
    }

    void CEventChrCore::SetLightSelector(long) {}

    void CEventChrCore::ResetLightSelector() {}

    void CEventChrCore::PlayVoice(long nVoice)
    {
        if (nVoice == -1) {
            return;
        }
        m_pChr->PlayVoice(nVoice, 0);
    }

    void CEventChrCore::PlaySe(long nSe)
    {
        if (nSe == -1) {
            return;
        }
        m_pChr->PlaySe(nSe, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    void CEventChrCore::SetSeVolume(float fRate)
    {
        m_pChr->SetSeVolume(fRate);
    }

    void CEventChrCore::SetVoiceVolume(float fRate, float f)
    {
        m_pChr->SetVoiceVolume(fRate, f);
    }

    long CEventChrCore::GetTeamNo() const
    {
        return m_pChr->GetTeamNo();
    }

    int CEventChrCore::IsObj() const
    {
        if (m_pChr != 0) {
            return m_pChr->IsEnable();
        }
        return 0;
    }
}
