#include "game/msw/CNowLoading.hpp"

#include "bcs/CLoadMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CMenuScnGroup.hpp"
#include "game/msw/ArchiveFont.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/lyt/animation.h"
#include "nw4r/lyt/drawInfo.h"
#include "nw4r/lyt/layout.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/mtx.h"

namespace {
    // Entry of msw::CLoadData's file table holding the layout's archive
    // (name guessed).
    const long LOAD_DATA_NOW_LOADING = 0x49;

    const char *s_pArcRootDir = "arc";
    const char *s_pLytName = "main_load.brlyt";
    const char *s_pAnmName = "main_load_nowloading.brlan";

    // (name guessed) Non-zero when the loader has nothing left to do.
    inline int IsLoadIdle()
    {
        if (bcs::CLoadMan::IsComplete() && bcs::CLoadMan::IsIdle()) {
            return 1;
        }
        return 0;
    }
}

namespace msw {

    CNowLoading *CNowLoading::s_pInstance;

    void CNowLoading::CreateInstance()
    {
        s_pInstance = new CNowLoading;
    }

    int CNowLoading::IsLoaded()
    {
        CNowLoading *pThis = s_pInstance;
        if (pThis != 0) {
            return pThis->m_bLoaded;
        }
        return 0;
    }

    void CNowLoading::SetType(long nType)
    {
        CNowLoading *pThis = s_pInstance;
        if (pThis != 0) {
            pThis->m_nType = nType;
            pThis->m_fTime = 0.0f;
        }
    }

    void CNowLoading::SetDisp(int bDisp)
    {
        CNowLoading *pThis = s_pInstance;
        if (pThis != 0) {
            pThis->m_bDisp = bDisp;
        }
    }

    CNowLoading::~CNowLoading()
    {
    }

    void CNowLoading::Main()
    {
        if (m_nStep == 0) {
            if (bcs::CLoadMan::IsLoadEnd(CLoadData::GetFilePath(LOAD_DATA_NOW_LOADING),
                                         CLoadData::GetFileId(LOAD_DATA_NOW_LOADING))) {
                if (nw4r::lyt::Layout::mspAllocator == 0) {
                    nw4r::lyt::LytInit();
                    nw4r::lyt::Layout::mspAllocator = bcs::G2D::CLytManager::GetAllocator();
                }
                m_bLoaded = 1;
                m_nStep++;
            }
        }

        if ((m_nType == 1 || m_nType == 2) && m_bLoaded) {
            if (!IsLoadIdle() && m_pLayout == 0) {
                // Loading is going on: build the layout.
                nw4r::lyt::ArcResourceAccessor *pResAccessor = new nw4r::lyt::ArcResourceAccessor;
                if (pResAccessor == 0) {
                    return;
                }
                pResAccessor->Attach(CLoadData::GetPtr(LOAD_DATA_NOW_LOADING), s_pArcRootDir);

                nw4r::lyt::Layout *pLayout = new nw4r::lyt::Layout;
                nw4r::lyt::AnimTransform *pAnimTrans = 0;
                if (pLayout == 0) {
                    return;
                }

                nw4r::lyt::FontRefLink *pFontRefLink = new nw4r::lyt::FontRefLink;
                if (pFontRefLink != 0) {
                    const char *pFontName = ArchiveFont::GetFontName(1);
                    pFontRefLink->Set(pFontName, ArchiveFont::GetFont(1));
                    pResAccessor->RegistFont(pFontRefLink);
                }

                void *pLytRes = pResAccessor->GetResource('blyt', s_pLytName, 0);
                if (pLytRes != 0) {
                    pLayout->Build(pLytRes, pResAccessor);
                    void *pAnmRes = pResAccessor->GetResource('anim', s_pAnmName, 0);
                    pAnimTrans = pLayout->CreateAnimTransform(pAnmRes, pResAccessor);
                    pLayout->BindAnimation(pAnimTrans);
                    pLayout->SetAnimationEnable(pAnimTrans, true);
                }

                m_pAnimTrans = pAnimTrans;
                m_pLayout = pLayout;
                m_pResAccessor = pResAccessor;
                m_pFontRefLink = pFontRefLink;
            } else if (IsLoadIdle() && m_pLayout != 0) {
                // Loading is over: tear it down again.
                delete m_pLayout;
                m_pLayout = 0;

                if (m_pFontRefLink != 0) {
                    if (m_pResAccessor != 0) {
                        m_pResAccessor->UnregistFont(m_pFontRefLink);
                    }
                    delete m_pFontRefLink;
                    m_pFontRefLink = 0;
                }

                if (m_pResAccessor != 0) {
                    m_pResAccessor->Detach();
                    delete m_pResAccessor;
                    m_pResAccessor = 0;
                }
            }
        }
    }

    void CNowLoading::Disp()
    {
        if (m_nType == 1 || m_nType == 2) {
            DrawLayout();
        }
    }

    void CNowLoading::DrawLayout()
    {
        if (m_bLoaded != 0 && m_bDisp != 0) {
            nw4r::lyt::Layout *pLayout = m_pLayout;
            if (pLayout != 0) {
                nw4r::lyt::DrawInfo drawInfo;
                nw4r::math::MTX34 viewMtx;
                Mtx44 projMtx;

                const bcs::G2D::CMenuScnGroup::OrthoProjInfo &rProj = bcs::G2D::CMenuScnGroup::s_OrthoProjInfo;
                float left = rProj.fLeft;
                float right = rProj.fRight;
                float bottom = rProj.fBottom;
                float top = rProj.fTop;
                C_MTXOrtho(projMtx, top, bottom, left, right, rProj.fNear, rProj.fFar);

                PSMTXIdentity(viewMtx);
                drawInfo.SetViewMtx(viewMtx);

                nw4r::ut::Rect viewRect;
                viewRect.left = left;
                viewRect.top = top;
                viewRect.right = right;
                viewRect.bottom = bottom;
                drawInfo.SetViewRect(viewRect);

                bcs::G2D::CLytManager *pLytManager = bcs::G2D::CLytManager::GetInstance();
                int bAdjust = bcs::CWorldEnv::m_vatViewAspectType == 1;
                float scaleH = pLytManager->GetLocationAdjustScaleH(bAdjust);
                nw4r::math::VEC2 adjustScale(scaleH, pLytManager->GetLocationAdjustScaleV(bAdjust));
                drawInfo.SetLocationAdjust(bAdjust);
                drawInfo.SetLocationAdjustScale(adjustScale);

                GXSetProjection(projMtx, GX_ORTHOGRAPHIC);

                pLayout->Animate(0);
                pLayout->CalculateMtx(drawInfo);
                pLayout->Draw(drawInfo);

                nw4r::lyt::AnimTransform *pAnimTrans = m_pAnimTrans;
                if (pAnimTrans != 0) {
                    // Advance one frame, wrapping at the end.
                    float frame = pAnimTrans->GetFrame() + 1.0f;
                    if (frame >= pAnimTrans->GetFrameSize()) {
                        frame -= pAnimTrans->GetFrameSize();
                    }
                    pAnimTrans->SetFrame(frame);
                }
            }
        }
    }
}
