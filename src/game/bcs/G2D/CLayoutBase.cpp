#include "bcs/G2D/CLayoutBase.hpp"

#include <string.h>

#include "bcs/New.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CLayoutObj.hpp"
#include "bcs/G2D/CLytResource.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "nw4r/lyt/animation.h"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/lyt/layout.h"

namespace bcs {
    namespace G2D {

        // CLayoutDL is an nw4r::lyt::Layout at offset 0 (see CLayoutDL.hpp
        // for why its header does not say so).
        static inline nw4r::lyt::Layout *ToLayout(CLayoutDL *pLayout)
        {
            return reinterpret_cast<nw4r::lyt::Layout *>(pLayout);
        }

        inline void CLayoutBase::ReleaseArchiveResource()
        {
            if (m_pAccessor != 0) {
                UnregistFontRefLinkAll();
                if (m_bMultiArc) {
                    DetachArcResourceLinkAll();
                    CLytResource::DeleteMultiArcResourceAccessor(
                        static_cast<nw4r::lyt::MultiArcResourceAccessor *>(m_pAccessor));
                } else {
                    CLytResource::DeleteArcResourceAccessor(static_cast<nw4r::lyt::ArcResourceAccessor *>(m_pAccessor));
                }
                m_pAccessor = 0;
            }
        }

        CLayoutBase::CLayoutBase()
        {
            Init();
        }

        CLayoutBase::~CLayoutBase()
        {
            UnbindAllAnim();

            ReleaseArchiveResource();

            if (m_pAnim != 0) {
                delete[] m_pAnim;
            }
            m_pAnim = 0;

            if (m_pLayout != 0) {
                delete ToLayout(m_pLayout);
            }
            m_pLayout = 0;
        }

        void CLayoutBase::Init()
        {
            m_pLayout = 0;
            m_pAccessor = 0;
            for (int i = 0; i < 4; i++) {
                m_pArcLink[i] = 0;
            }
            for (int i = 0; i < 4; i++) {
                m_pFontRefLink[i] = 0;
            }
            m_pAnim = 0;
            m_nAnimNum = 0;
            m_nFlags = 0;
        }

        long CLayoutBase::CreateLayoutObject(long nUnk)
        {
            if (m_pLayout != 0) {
                return -1;
            }

            m_pLayout = new (0L) CLayoutDL(nUnk);
            if (m_pLayout == 0) {
                return -2;
            }
            return 0;
        }

        long CLayoutBase::AttachArchiveResource(nw4r::lyt::ArcResourceAccessor *pAccessor)
        {
            if (pAccessor == 0) {
                return -1;
            }

            ReleaseArchiveResource();

            m_pAccessor = pAccessor;
            m_bMultiArc = 0;
            return 0;
        }

        void CLayoutBase::DetachArcResourceLinkAll()
        {
            if (m_bMultiArc) {
                for (int i = 0; i < 4; i++) {
                    if (m_pArcLink[i] != 0) {
                        static_cast<nw4r::lyt::MultiArcResourceAccessor *>(m_pAccessor)->Detach(m_pArcLink[i]);
                        if (CLytResource::GetInstance() != 0) {
                            CLytResource::DeleteArcResourceLink(m_pArcLink[i]);
                        }
                        m_pArcLink[i] = 0;
                    }
                }
            }
        }

        long CLayoutBase::AddFontRefLink(nw4r::ut::Font *pFont, const char *pName)
        {
            if (pFont == 0 && pName == 0) {
                return -1;
            }

            CLytResource *pResource = CLytResource::GetInstance();
            if (pResource == 0) {
                return -2;
            }

            nw4r::lyt::FontRefLink *pLink = pResource->NewFontRefLink();
            if (pLink == 0) {
                return -3;
            }

            pLink->Set(pName, pFont);
            return RegistFontRefLink(pLink);
        }

        long CLayoutBase::RegistFontRefLink(nw4r::lyt::FontRefLink *pLink)
        {
            for (long i = 0; i < 4; i++) {
                if (m_pFontRefLink[i] == 0) {
                    m_pFontRefLink[i] = pLink;
                    if (m_bMultiArc) {
                        nw4r::lyt::MultiArcResourceAccessor *pAccessor =
                            static_cast<nw4r::lyt::MultiArcResourceAccessor *>(m_pAccessor);
                        if (pAccessor->GetFont(pLink->GetFontName()) == 0) {
                            pAccessor->RegistFont(pLink);
                        }
                    } else {
                        nw4r::lyt::ArcResourceAccessor *pAccessor =
                            static_cast<nw4r::lyt::ArcResourceAccessor *>(m_pAccessor);
                        if (pAccessor->GetFont(pLink->GetFontName()) == 0) {
                            pAccessor->RegistFont(pLink);
                        }
                    }
                    return i;
                }
            }
            return -2;
        }

        void CLayoutBase::UnregistFontRefLinkAll()
        {
            if (CLytResource::GetInstance() == 0) {
                for (int i = 0; i < 4; i++) {
                    m_pFontRefLink[i] = 0;
                }
                return;
            }

            if (m_bMultiArc) {
                nw4r::lyt::MultiArcResourceAccessor *pAccessor =
                    static_cast<nw4r::lyt::MultiArcResourceAccessor *>(m_pAccessor);
                for (int i = 0; i < 4; i++) {
                    if (m_pFontRefLink[i] != 0) {
                        if (pAccessor != 0) {
                            pAccessor->UnregistFont(m_pFontRefLink[i]);
                        }
                        CLytResource::DeleteFontRefLink(m_pFontRefLink[i]);
                        m_pFontRefLink[i] = 0;
                    }
                }
            } else {
                nw4r::lyt::ArcResourceAccessor *pAccessor = static_cast<nw4r::lyt::ArcResourceAccessor *>(m_pAccessor);
                for (int i = 0; i < 4; i++) {
                    if (m_pFontRefLink[i] != 0) {
                        if (pAccessor != 0) {
                            pAccessor->UnregistFont(m_pFontRefLink[i]);
                        }
                        CLytResource::DeleteFontRefLink(m_pFontRefLink[i]);
                        m_pFontRefLink[i] = 0;
                    }
                }
            }
        }

        long CLayoutBase::BuildLayout(long nUnk, const char *pLayoutName)
        {
            if (pLayoutName == 0 || strlen(pLayoutName) == 0) {
                return -1;
            }

            void *pRes = m_pAccessor->GetResource(nUnk, pLayoutName, 0);
            if (pRes == 0) {
                return -2;
            }

            if (!ToLayout(m_pLayout)->Build(pRes, m_pAccessor)) {
                return -3;
            }
            m_pLayout->m_pMessageName = pLayoutName;
            return 0;
        }

        long CLayoutBase::BuildAnim(long nUnk, const char **ppAnimNames, long nNum)
        {
            if (ppAnimNames == 0 || nNum == 0) {
                return -1;
            }

            if (m_pAnim != 0) {
                delete[] m_pAnim;
            }
            m_pAnim = 0;

            m_pAnim = new (0L) nw4r::lyt::AnimResource[nNum];
            if (m_pAnim == 0) {
                return -3;
            }

            unsigned long i;
            for (i = 0; i < nNum; i++) {
                m_pAnim[i].Set(m_pAccessor->GetResource(nUnk, ppAnimNames[i], 0));
            }

            for (i = 0; i < nNum; i++) {
                if (!ToLayout(m_pLayout)->BindAnimationAuto(m_pAnim[i], m_pAccessor)) {
                    UnbindAllAnim();
                    if (m_pAnim != 0) {
                        delete[] m_pAnim;
                    }
                    m_pAnim = 0;
                    return -2;
                }
            }

            m_nAnimNum = nNum;
            return 0;
        }

        long CLayoutBase::UnbindAnim(long nAnim)
        {
            if (nAnim >= m_nAnimNum || nAnim < 0) {
                return -1;
            }

            for (nw4r::lyt::AnimTransform *pAnimTrans = FindAnimTransform(nAnim, 0); pAnimTrans != 0;
                 pAnimTrans = FindAnimTransform(nAnim, pAnimTrans)) {
                ToLayout(m_pLayout)->UnbindAnimation(pAnimTrans);
            }
            return 0;
        }

        nw4r::lyt::AnimTransform *CLayoutBase::FindAnimTransform(long nAnim, nw4r::lyt::AnimTransform *pAfter)
        {
            nw4r::lyt::AnimResource *pAnim = GetAnimResource(nAnim);
            if (pAnim == 0) {
                return 0;
            }

            nw4r::lyt::Layout::AnimTransformList &rList = ToLayout(m_pLayout)->GetAnimTransformList();
            nw4r::lyt::Layout::AnimTransformList::Iterator it = rList.GetBeginIter();
            if (nAnim < 0 || nAnim >= rList.GetSize()) {
                return 0;
            }

            if (pAfter != 0) {
                for (; it != rList.GetEndIter(); ++it) {
                    if (pAfter == &*it) {
                        ++it;
                        break;
                    }
                }
            }

            for (; it != rList.GetEndIter(); ++it) {
                nw4r::lyt::AnimTransform &rAnimTrans = *it;
                if (rAnimTrans.GetAnimResource() == pAnim->GetResourceBlock()) {
                    return &rAnimTrans;
                }
            }
            return 0;
        }

        nw4r::lyt::AnimResource *CLayoutBase::GetAnimResource(long nAnim)
        {
            if (nAnim >= m_nAnimNum || nAnim < 0) {
                return 0;
            }
            return &m_pAnim[nAnim];
        }

        nw4r::lyt::AnimResource *CLayoutBase::FindAnimResource(nw4r::lyt::AnimTransform *pAnimTrans)
        {
            if (pAnimTrans == 0) {
                return 0;
            }

            for (unsigned long i = 0; i < m_nAnimNum; i++) {
                if (m_pAnim[i].GetResourceBlock() == pAnimTrans->GetAnimResource()) {
                    return &m_pAnim[i];
                }
            }
            return 0;
        }

        CLayoutDL *CLayoutBase::GetLayout()
        {
            return m_pLayout;
        }

        int CLayoutBase::IsAnimEnd(long nAnim)
        {
            nw4r::lyt::AnimTransform *pAnimTrans = FindAnimTransform(nAnim, 0);
            if (pAnimTrans == 0) {
                return 1;
            }
            float fLastFrame = pAnimTrans->GetFrameSize() - 1.0f;
            return pAnimTrans->GetFrame() >= fLastFrame;
        }

        float CLayoutBase::GetAnimFrameMax(long nAnim)
        {
            nw4r::lyt::AnimResource *pAnim = GetAnimResource(nAnim);
            if (pAnim == 0) {
                return -1.0f;
            }
            return pAnim->GetFrameSize() - 1.0f;
        }

        float CLayoutBase::GetAnimFrameSize(long nAnim)
        {
            nw4r::lyt::AnimResource *pAnim = GetAnimResource(nAnim);
            if (pAnim == 0) {
                return -1.0f;
            }
            return pAnim->GetFrameSize();
        }

        float CLayoutBase::GetSortZ()
        {
            nw4r::lyt::Pane *pRoot = m_pLayout->GetRootPane();
            if (pRoot == 0) {
                return 0.0f;
            }
            return pRoot->GetGlobalMtx()._23;
        }

        void CLayoutBase::SetSortZ(float fZ)
        {
            SetLayoutSortZ(m_pLayout, fZ);
        }

        int CLayoutBase::IsAnimLoop(long nAnim)
        {
            nw4r::lyt::AnimResource *pAnim = GetAnimResource(nAnim);
            if (pAnim == 0) {
                return 0;
            }
            return pAnim->IsLoopData();
        }

        void CLayoutObj::PlayAnim(long nAnim, float fSpeed, float fFrame)
        {
            if (SetAnimEnable(nAnim, 1) <= 0) {
                return;
            }

            nw4r::lyt::AnimTransform *pAnimTrans = FindAnimTransform(nAnim, 0);
            if (pAnimTrans != 0) {
                if (fFrame < 0.0f) {
                    fFrame = 0.0f;
                } else {
                    float fLastFrame = pAnimTrans->GetFrameSize() - 1.0f;
                    if (fFrame > fLastFrame) {
                        fFrame = fLastFrame;
                    }
                }
                pAnimTrans->SetFrame(fFrame);

                for (; pAnimTrans != 0; pAnimTrans = FindAnimTransform(nAnim, pAnimTrans)) {
                    pAnimTrans->SetFrame(fFrame);
                }
            }

            if (IsAnimLoop(nAnim) == 1) {
                m_bAnimLoop = 1;
            } else {
                m_bAnimLoop = 0;
            }
            m_fSpeed = fSpeed;
            m_bAnimStop = 0;
            m_nAnim = nAnim;
        }

        long CLayoutObj::SetAnimEnable(long nAnim, int bExclusive)
        {
            long nNum = 0;
            nw4r::lyt::Layout *pLayout = ToLayout(m_pLayout);
            nw4r::lyt::AnimTransform *pAnimTrans;

            if (bExclusive == 1) {
                nw4r::lyt::Layout::AnimTransformList &rList = pLayout->GetAnimTransformList();
                for (nw4r::lyt::Layout::AnimTransformList::Iterator it = rList.GetBeginIter(); it != rList.GetEndIter();) {
                    pAnimTrans = &*it++;
                    pLayout->SetAnimationEnable(pAnimTrans, false);
                }
            }

            if (nAnim >= 0) {
                pAnimTrans = FindAnimTransform(nAnim, 0);
                while (pAnimTrans != 0) {
                    pLayout->SetAnimationEnable(pAnimTrans, true);
                    pAnimTrans = FindAnimTransform(nAnim, pAnimTrans);
                    nNum++;
                }
            }
            return nNum;
        }
    }
}
