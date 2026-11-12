#include "game/msw/CStgMdl.hpp"
#include "game/msw/CStgGenericGmkMdl.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CStageAnm.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/ut/Color.hpp"
#include "bcs/CCapture.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CStgMdlObj.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"
#include "string.h"

// msw::CStgMdl, a model of a stage object, msw::CStgGenericGmkMdl, and the
// creators that make a model's parts.

namespace bcs {
    // (DOL lbl_804CFE50, name guessed; in the bcs part of .bss, next to
    // bcs::CRenderMan's) The texture the stage models' "reflect" textures
    // are bound to.
    extern GXTexObj g_ReflectTexObj;
}

namespace msw {

    // (DOL lbl_805C0CE4, name guessed) The colour of the background now,
    // which every stage model takes as its base colour (game/msw/CGame.cpp
    // keeps it up to date).
    extern bcs::BCS_Color g_BgCurColor;

    CStgMdl::CCreator CStgMdl::NullCreator;

    CStgGenericGmkMdl::CStgGenericGmkMdl(CStgObj *pParent, nw4r::g3d::ResFile res, const MDL_PARAM *pParam,
                                         const char *pName)
        : CStgMdl(pParent), m_Creator(res, pParam, pName), m_bUseCreator(1)
    {
    }

    CStgGenericGmkMdl::~CStgGenericGmkMdl()
    {
    }

    void CStgGenericGmkMdl::Create()
    {
        if (m_bUseCreator) {
            CStgMdl::Create(m_Creator);
        } else {
            CStgMdl::Create();
        }
    }

    CStgMdl::CStgMdl(CStgObj *pParent)
        : m_pParent(pParent)
        , m_pMdlObj(0)
        , m_pShadow(0)
        , m_pRef(0)
        , m_pAnm(0)
        , m_nAnmArgHead(0)
        , m_nAnmArgTail(0)
        , m_nLayer(2)
        , m_UnkE4(0)
        , m_fFadeFrame(0.0f)
        , m_fFadeLength(0.0f)
        , m_nFade(0)
        , m_bSceneEntry(0)
    {
        PSMTXIdentity(m_Mtx.m);
    }

    CStgMdl::~CStgMdl()
    {
        if (m_pShadow) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
        }
        m_pShadow = 0;
        if (m_pRef) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pRef);
        }
        m_pRef = 0;
        if (m_pMdlObj) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlObj);
        }
        m_pMdlObj = 0;
        if (m_pAnm) {
            delete m_pAnm;
        }
        m_pAnm = 0;
    }

    // Makes the model, its animations, its shadow and its reference model
    // with rCreator. A part the creator cannot make is dropped.
    void CStgMdl::Create(const CCreator &rCreator)
    {
        DeleteMdl();
        if (OnPreCreate()) {
            bcs::CMdlObj *pMdl = NewMdl(0);
            if (pMdl) {
                if (!rCreator.CreateMdlObj(pMdl)) {
                    if (pMdl) {
                        bcs::CScnObj::AppendDelayDeleteObject(pMdl);
                    }
                } else {
                    m_pMdlObj = pMdl;
                    SetupReflectMat_(pMdl);
                    SetupMat_(pMdl);
                    m_pAnm = new CStageAnm;
                    rCreator.CreateAnm(m_pAnm, pMdl);

                    bcs::CMdlShadow *pShadow = NewShadowMdl(0);
                    if (pShadow) {
                        if (rCreator.CreateShadowMdl(pShadow)) {
                            m_pShadow = pShadow;
                            pShadow->SetLink(pMdl);
                        } else if (pShadow) {
                            bcs::CScnObj::AppendDelayDeleteObject(pShadow);
                        }
                    }

                    bcs::CMdlRef *pRef = NewRefMdl(0);
                    if (pRef) {
                        if (rCreator.CreateRefMdl(pRef)) {
                            m_pRef = pRef;
                            pRef->SetLink(pMdl, 0);
                        } else if (pRef) {
                            bcs::CScnObj::AppendDelayDeleteObject(pRef);
                        }
                    }
                }
            }
            OnPostCreate();
        }
    }

    void CStgMdl::DeleteMdl()
    {
        if (m_pShadow) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pShadow);
        }
        m_pShadow = 0;
        if (m_pRef) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pRef);
        }
        m_pRef = 0;
        if (m_pMdlObj) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlObj);
        }
        m_pMdlObj = 0;
        if (m_pAnm) {
            delete m_pAnm;
        }
        m_pAnm = 0;
        m_bSceneEntry = 0;
    }

    // Puts the model, its shadow and its reference model in the scene
    // (each only when not attached to another object).
    void CStgMdl::SceneEntry()
    {
        if (!m_bSceneEntry) {
            Commit();
            bcs::CScnObj *pScnObj = GetScnObj();
            if (pScnObj && !pScnObj->GetParent()) {
                bcs::CRenderMan::PushBack(pScnObj, GetLayer());
            }
            bcs::CMdlShadow *pShadow = GetMdlShadow();
            if (pShadow && !pShadow->GetParent()) {
                bcs::CRenderMan::PushBack(pShadow, 2);
            }
            bcs::CMdlRef *pRef = GetMdlRef();
            if (pRef && !pRef->GetParent()) {
                bcs::CRenderMan::PushBack(pRef, 4);
            }
            OnSceneEntry();
            m_bSceneEntry = 1;
        }
    }

    void CStgMdl::SceneLeave()
    {
        if (m_bSceneEntry) {
            m_bSceneEntry = 0;
            OnSceneLeave();
            bcs::CScnObj *pScnObj = GetScnObj();
            if (pScnObj) {
                pScnObj->DetachFromParent();
            }
            bcs::CMdlShadow *pShadow = GetMdlShadow();
            if (pShadow) {
                pShadow->DetachFromParent();
            }
            bcs::CMdlRef *pRef = GetMdlRef();
            if (pRef) {
                pRef->DetachFromParent();
            }
        }
    }

    void CStgMdl::SetDrawPriority(long opa, long xlu)
    {
        bcs::CScnObj *pScnObj = GetScnObj();
        if (pScnObj) {
            pScnObj->SetDrawPriority(opa, xlu);
        }
    }

    namespace {

        // Bit 0 of the scene object's flags hides it.
        inline void SetScnObjVisible(bcs::CScnObj *pObj, int bVisible)
        {
            if (pObj) {
                if (!bVisible) {
                    pObj->OnFlag(1);
                } else {
                    pObj->OffFlag(1);
                }
            }
        }
    }

    // Shows or hides the model, its shadow and its reference model, and
    // puts them in or takes them out of the scene.
    void CStgMdl::SetVisible(int bVisible)
    {
        SetScnObjVisible(GetScnObj(), bVisible);
        SetScnObjVisible(GetMdlShadow(), bVisible);
        SetScnObjVisible(GetMdlRef(), bVisible);
        if (bVisible) {
            SceneEntry();
        } else {
            SceneLeave();
        }
    }

    void CStgMdl::AttachToMdl(CStgMdl *pMdl, long node, bcs::CScnObj::AttachMode mode)
    {
        if (pMdl) {
            bcs::CScnObj *pScnObj = GetScnObj();
            bcs::CScnObj *pParent = pMdl->GetScnObj();
            if (pScnObj && pParent) {
                pScnObj->AttachToObject(pParent, node, mode);
            }
        }
    }

    void CStgMdl::DetachFromMdl()
    {
        bcs::CScnObj *pScnObj = GetScnObj();
        if (pScnObj) {
            pScnObj->AttachToObject(0, 0, static_cast<bcs::CScnObj::AttachMode>(0));
        }
    }

    void CStgMdl::FadeIn(float time, float delay)
    {
        m_nFade = 1;
        m_fFadeFrame = -delay;
        m_fFadeLength = time;
    }

    void CStgMdl::FadeOut(float time, float delay)
    {
        m_nFade = -1;
        m_fFadeFrame = -delay;
        m_fFadeLength = time;
    }

    void CStgMdl::FadeUpdate(float rate)
    {
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (m_nFade != 0 && pMdl) {
            m_fFadeFrame += rate;
            float t = m_fFadeLength > 0.0f ? m_fFadeFrame / m_fFadeLength : 1.0f;
            t = t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
            if (m_nFade > 0) {
                pMdl->SetAlphaRateF(t);
            } else if (m_nFade < 0) {
                pMdl->SetAlphaRateF(1.0f - t);
            }
            if (m_fFadeFrame >= m_fFadeLength) {
                m_nFade = 0;
            }
        }
    }

    void CStgMdl::Main(float rate)
    {
        AnmQueueUpdate_();
        OnUpdate(rate);
        FadeUpdate(rate);
        Commit();
    }

    // Gives the scene object the model's matrix and the model the colour
    // of the background.
    void CStgMdl::Commit()
    {
        bcs::CScnObj *pScnObj = GetScnObj();
        if (pScnObj) {
            bcs::FMTX34 mtx;
            bcs::FMTX34Copy(&mtx, &GetMtx());
            pScnObj->SetMtx(bcs::CScnObj::SCNOBJMTXTYPE_ROOT, mtx);
        }
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl) {
            pMdl->SetBaseColor(g_BgCurColor);
        }
    }

    bool CStgMdl::GetNodeMtx(bcs::FMTX34 &rMtx, long node) const
    {
        const bcs::CMdlObj *pMdl = GetMdlObj();
        if (!pMdl) {
            return false;
        }
        nw4r::g3d::ChrAnmResult *pResult = pMdl->GetChrAnmResult();
        if (!pResult) {
            return false;
        }
        pResult[node].GetMtx(reinterpret_cast<nw4r::math::MTX34 *>(&rMtx));
        return true;
    }

    bool CStgMdl::GetNodeMtx(bcs::FMTX34 &rMtx, const char *pNode) const
    {
        const bcs::CMdlObj *pMdl = GetMdlObj();
        if (!pMdl) {
            return false;
        }
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!mdl.IsValid()) {
            return false;
        }
        nw4r::g3d::ResNode node = mdl.GetResNode(pNode);
        if (!node.IsValid()) {
            return false;
        }
        nw4r::g3d::ChrAnmResult *pResult = pMdl->GetChrAnmResult();
        if (!pResult) {
            return false;
        }
        pResult[node.GetID()].GetMtx(reinterpret_cast<nw4r::math::MTX34 *>(&rMtx));
        return true;
    }

    bool CStgMdl::GetNodeWorldMtx(bcs::FMTX34 &rMtx, long node) const
    {
        const bcs::CMdlObj *pMdl = GetMdlObj();
        if (!pMdl) {
            return false;
        }
        pMdl->GetNodeWorldMtx(node, rMtx);
        return true;
    }

    bool CStgMdl::GetNodeWorldMtx(bcs::FMTX34 &rMtx, const char *pNode) const
    {
        const bcs::CMdlObj *pMdl = GetMdlObj();
        if (!pMdl) {
            return false;
        }
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!mdl.IsValid()) {
            return false;
        }
        nw4r::g3d::ResNode node = mdl.GetResNode(pNode);
        if (!node.IsValid()) {
            return false;
        }
        pMdl->GetNodeWorldMtx(node.GetID(), rMtx);
        return true;
    }

    // Starts an animation now, dropping the queued ones.
    void CStgMdl::SetAnm(unsigned long anm, unsigned long arg, float speed, float frame, float blend,
                         unsigned long flag)
    {
        AnmArgClear_();
        if (m_pAnm) {
            m_pAnm->SetAnm(anm, arg, speed, frame, blend, flag);
        }
    }

    // Queues an animation to start when the ones before it have ended.
    void CStgMdl::PushAnm(unsigned long anm, unsigned long arg, float speed, float frame, float blend,
                          unsigned long flag)
    {
        ANM_ARG *pArg = AnmArgPush_();
        if (pArg) {
            pArg->anm = anm;
            pArg->arg = arg;
            pArg->speed = speed;
            pArg->frame = frame;
            pArg->blend = blend;
            pArg->flag = flag;
        }
    }

    void CStgMdl::PopAnmAll()
    {
        AnmArgClear_();
    }

    void CStgMdl::SetAnmEx(unsigned long anm, unsigned long arg)
    {
        AnmArgClear_();
        if (m_pAnm) {
            m_pAnm->SetAnmEx(anm, arg);
        }
    }

    void CStgMdl::CancelAnm()
    {
        AnmArgClear_();
        if (m_pAnm) {
            m_pAnm->Cancel(-1);
        }
    }

    void CStgMdl::ResetAnm()
    {
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pMdl) {
            pMdl->ResetAnm();
        }
    }

    void CStgMdl::SetAnmSpeed(float speed)
    {
        if (m_pAnm) {
            m_pAnm->SetSpeed(-1, speed);
        }
    }

    // The animation tests are true without animations.
    bool CStgMdl::IsAnmEnd(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsEnd(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    bool CStgMdl::IsAnmEndAll(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsLooped(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    bool CStgMdl::IsAnmLooped(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsAnmLooped(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    bool CStgMdl::IsAnmLoopedAll(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsAnmLoopedAll(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    bool CStgMdl::IsAnmLoop(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsAnmLoop(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    bool CStgMdl::IsAnmLoopAll(unsigned long anm)
    {
        bool result = false;
        CStageAnm *pAnm = GetAnmObj();
        if (pAnm) {
            if (pAnm->IsAnmLoopAll(anm)) {
                result = true;
            }
        } else {
            result = true;
        }
        return result;
    }

    // The queue of PushAnm, a ring of ANM_ARG_NUM entries.
    CStgMdl::ANM_ARG *CStgMdl::AnmArgPush_()
    {
        long tail = m_nAnmArgTail;
        m_nAnmArgTail = (tail + 1) % ANM_ARG_NUM;
        return &m_AnmArg[tail];
    }

    CStgMdl::ANM_ARG *CStgMdl::AnmArgPop_()
    {
        if (m_nAnmArgTail == m_nAnmArgHead) {
            return 0;
        }
        ANM_ARG *pArg = &m_AnmArg[m_nAnmArgHead];
        m_nAnmArgHead = (m_nAnmArgHead + 1) % ANM_ARG_NUM;
        return pArg;
    }

    const CStgMdl::ANM_ARG *CStgMdl::AnmArgFront_() const
    {
        if (m_nAnmArgTail == m_nAnmArgHead) {
            return 0;
        }
        return &m_AnmArg[m_nAnmArgHead];
    }

    void CStgMdl::AnmArgClear_()
    {
        m_nAnmArgHead = m_nAnmArgTail;
    }

    bool CStgMdl::AnmArgIsEmpty_() const
    {
        return m_nAnmArgTail == m_nAnmArgHead;
    }

    // Starts the next queued animation once the current one has ended.
    void CStgMdl::AnmQueueUpdate_()
    {
        if (!AnmArgIsEmpty_()) {
            const ANM_ARG *pArg = AnmArgFront_();
            if (IsAnmEndAll(pArg->flag)) {
                if (m_pAnm) {
                    m_pAnm->SetAnm(pArg->anm, pArg->arg, pArg->speed, pArg->frame, pArg->blend, pArg->flag);
                }
                AnmArgPop_();
            }
        }
    }

    // The names of the textures the material set-up binds.
    static const char *s_pReflectTexName = "reflect";
    static const char *s_pScreenTexName = "screen";
    static const char *s_pScreenProjTexName = "screen_proj";

    // Whether name is pStr, whose length is len.
    static inline bool IsTexName(nw4r::g3d::ResName name, const char *pStr, unsigned long len)
    {
        bool bSame = false;
        if (len == name.GetLength() && strcmp(name.GetName(), pStr) == 0) {
            bSame = true;
        }
        return bSame;
    }

    // Binds the reflection texture to every texture named "reflect".
    bool CStgMdl::SetupReflectMat_(bcs::CMdlObj *pMdl)
    {
        bool bFound = false;
        for (unsigned long i = 0; i < pMdl->GetResMdl().GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat mat = pMdl->GetResMdl().GetResMat(i);
            if (mat.IsValid()) {
                nw4r::g3d::ResTexObj texObj = mat.GetResTexObj();
                unsigned long num = mat.GetNumResTexPlttInfo();
                for (unsigned long j = 0; j < num; j++) {
                    static const unsigned long s_nLen = strlen(s_pReflectTexName);
                    if (IsTexName(mat.GetResTexPlttInfo(j).GetTexResName(), s_pReflectTexName, s_nLen)) {
                        *texObj.GetTexObj(static_cast<_GXTexMapID>(j)) = bcs::g_ReflectTexObj;
                        texObj.Validate(static_cast<_GXTexMapID>(j));
                        bFound = true;
                    }
                }
                pMdl->ResetAnm();
            }
        }
        return bFound;
    }

    // Binds the screen capture to every texture named "screen" or
    // "screen_proj"; a "screen" texture also gets a matrix that fits the
    // capture to the screen's aspect ratio.
    bool CStgMdl::SetupMat_(bcs::CMdlObj *pMdl)
    {
        bool bFound = false;
        for (unsigned long i = 0; i < pMdl->GetResMdl().GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat mat = pMdl->GetResMdl().GetResMat(i);
            if (mat.IsValid()) {
                nw4r::g3d::ResTexObj texObj = mat.GetResTexObj();
                unsigned long num = mat.GetNumResTexPlttInfo();
                for (unsigned long j = 0; j < num; j++) {
                    static const unsigned long s_nScreenLen = strlen(s_pScreenTexName);
                    static const unsigned long s_nScreenProjLen = strlen(s_pScreenProjTexName);
                    int kind = 0;
                    nw4r::g3d::ResTexPlttInfo info = mat.GetResTexPlttInfo(j);
                    if (IsTexName(info.GetTexResName(), s_pScreenTexName, s_nScreenLen)) {
                        kind = 1;
                    } else if (IsTexName(info.GetTexResName(), s_pScreenProjTexName, s_nScreenProjLen)) {
                        kind = 2;
                    }
                    if (kind != 0) {
                        bcs::CCapture *pCapture = bcs::CCapture::GetInstance(0);
                        if (pCapture) {
                            *texObj.GetTexObj(static_cast<_GXTexMapID>(j)) = *pCapture->GetTexObj();
                            texObj.Validate(static_cast<_GXTexMapID>(j));
                            bFound = true;
                            nw4r::g3d::ResTexSrt srt = mat.GetResTexSrt();
                            if (srt.IsValid() && kind == 1) {
                                float scale = bcs::CWorldEnv::m_vatViewAspectType == 1 ? 1.0f : 4.0f / 3.0f;
                                float offset = 0.5f * (1.0f - scale);
                                nw4r::math::MTX34 mtx;
                                PSMTXIdentity(mtx);
                                mtx._00 = scale;
                                mtx._03 = offset;
                                srt.SetEffectMtx(0, &mtx);
                            }
                        }
                    }
                }
                pMdl->ResetAnm();
            }
        }
        return bFound;
    }

    // Makes the model of the archive entry GetIndex() returns, with every
    // root node at the rest pose.
    bool CStgMdl::CGenericCreator::CreateMdlObj(bcs::CMdlObj *pMdl) const
    {
        const nw4r::g3d::ResFile *pRes = m_pRes;
        const CArn *pArn = m_pArn;
        long idx = GetIndex();
        if (idx < 0 || idx >= pArn->GetNum()) {
            return false;
        }
        nw4r::g3d::ResMdl mdl = pRes->GetResMdl(pArn->GetName(idx));
        if (!mdl.IsValid()) {
            return false;
        }
        unsigned short kind = CArn::s_MdlKindTbl[pArn->GetEntry(idx).nKind];
        unsigned long flag = CStageAnm::GetMdlCreateFlag(*pRes, pArn, idx);
        for (nw4r::g3d::ResNode node = mdl.GetResNode(0); node.IsValid(); node = node.GetNextSibling()) {
            node.SetScale(1.0f, 1.0f, 1.0f);
            node.SetRotate(0.0f, 0.0f, 0.0f);
            node.SetTranslate(0.0f, 0.0f, 0.0f);
        }
        if (!pMdl->Create(mdl, flag, 0)) {
            return false;
        }
        unsigned short prio = pArn->GetEntry(idx).nDrawPriority;
        if (prio != 0) {
            pMdl->SetDrawPriority(prio, prio);
        } else {
            pMdl->SetDrawPriority(idx >> 3, idx >> 3);
        }
        if (kind == 4) {
            unsigned long num = pMdl->GetResMdl().GetResMatNumEntries();
            for (unsigned long i = 0; i < num; i++) {
                bcs::CMdlObj::CopiedMatAccess access(pMdl, i);
                nw4r::g3d::ResMatMisc misc = access.GetResMatMisc(false);
                if (misc.IsValid()) {
                    misc.SetFogIdx(-1);
                }
            }
        }
        pMdl->OnFlag(0x10000);
        return true;
    }

    bool CStgMdl::CGenericCreator::CreateAnm(CStageAnm *pAnm, bcs::CMdlObj *pMdl) const
    {
        if (!pAnm || !pMdl) {
            return false;
        }
        return pAnm->Create(pMdl, *m_pRes, m_pArn, m_nIdx);
    }

    bool CStgMdl::CGenericGimmickCreator::CreateMdlObj(bcs::CMdlObj *pMdl) const
    {
        if (!pMdl || !m_pParam || !m_Res.IsValid() || !m_pParam->pMdlName) {
            return false;
        }
        nw4r::g3d::ResFile res = m_Res;
        nw4r::g3d::ResMdl mdl = res.GetResMdl(m_pParam->pMdlName);
        if (!mdl.IsValid()) {
            return false;
        }
        static_cast<CStgMdlObj *>(pMdl)->Create(mdl, m_pParam->unk0C, 0);
        return true;
    }

    // Build a shadow model (with a 64 x 64 texture) and a reference model
    // from mdl. (guess) The original passes the model through inline
    // functions like these, as the gimmick creator's CStgMdlObj::Create
    // does: the copy of mdl is made at the call, after the flag is read.
    static inline void CreateShadowMdl_(bcs::CMdlShadow *pMdl, nw4r::g3d::ResMdl mdl, unsigned long flag)
    {
        pMdl->Create(mdl, 0x40, flag, 0);
    }

    static inline void CreateRefMdl_(bcs::CMdlRef *pMdl, nw4r::g3d::ResMdl mdl, unsigned long flag)
    {
        pMdl->Create(mdl, flag, 0);
    }

    bool CStgMdl::CGenericGimmickCreator::CreateShadowMdl(bcs::CMdlShadow *pMdl) const
    {
        if (!pMdl || !m_pParam || !m_Res.IsValid() || !m_pParam->pShadowName) {
            return false;
        }
        nw4r::g3d::ResFile res = m_Res;
        nw4r::g3d::ResMdl mdl = res.GetResMdl(m_pParam->pShadowName);
        if (!mdl.IsValid()) {
            return false;
        }
        CreateShadowMdl_(pMdl, mdl, m_pParam->unk0C);
        return true;
    }

    bool CStgMdl::CGenericGimmickCreator::CreateRefMdl(bcs::CMdlRef *pMdl) const
    {
        if (!pMdl || !m_pParam || !m_Res.IsValid() || !m_pParam->pRefName) {
            return false;
        }
        nw4r::g3d::ResFile res = m_Res;
        nw4r::g3d::ResMdl mdl = res.GetResMdl(m_pParam->pRefName);
        if (!mdl.IsValid()) {
            return false;
        }
        CreateRefMdl_(pMdl, mdl, m_pParam->unk0C);
        return true;
    }

    bool CStgMdl::CGenericGimmickCreator::CreateAnm(CStageAnm *pAnm, bcs::CMdlObj *pMdl) const
    {
        if (!pAnm || !pMdl || !m_pParam || !m_Res.IsValid() || !m_pParam->ppAnmSuffix) {
            return false;
        }
        nw4r::g3d::ResFile res = m_Res;
        pAnm->SetAnmParam(m_pParam->pAnmParam);
        return pAnm->Create(pMdl, res, m_pParam->ppAnmSuffix, m_pName ? m_pName : m_pParam->pAnmName,
                            m_pParam->unk18);
    }

    bool CStgMdl::CGenericMVAGimmickCreator::CreateMdlObj(bcs::CMdlObj *pMdl) const
    {
        if (!pMdl || !m_pParam || !m_Res.IsValid() || !m_pParam->pMdlName) {
            return false;
        }
        nw4r::g3d::ResFile res = m_Res;
        nw4r::g3d::ResMdl mdl = res.GetResMdl(m_pParam->pMdlName);
        if (!mdl.IsValid()) {
            return false;
        }
        static_cast<CFgMdl *>(pMdl)->Create(mdl, m_pParam->unk0C, 0);
        return true;
    }

}
