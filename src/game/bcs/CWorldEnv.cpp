#include "bcs/CWorldEnv.hpp"

#include <math.h>

#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/mtx.h"

namespace bcs {

    // The number of split views; one more entry follows for the full
    // screen.
    static const long VIEW_NUM = 4;


    // A view's entry; any number but 0 to 3 (-1 is passed) selects the full
    // screen entry.
    static inline nw4r::g3d::CameraData *ViewCameraData_(long view)
    {
        if ((unsigned long)view <= VIEW_NUM - 1) {
            return &CWorldEnv::m_ViewCameraData[view];
        }
        return &CWorldEnv::m_ViewCameraData[VIEW_NUM];
    }

    static inline CWorldEnv::ViewProjInfo *ViewProjInfo_(long view)
    {
        if ((unsigned long)view <= VIEW_NUM - 1) {
            return &CWorldEnv::m_ViewProjInfo[view];
        }
        return &CWorldEnv::m_ViewProjInfo[VIEW_NUM];
    }

    void CWorldEnv::Set3DBackColorAll(BCS_Color color)
    {
        unsigned char r = color.r;
        unsigned char g = color.g;
        unsigned char b = color.b;
        unsigned char a = color.a;
        for (long i = 0; i < VIEW_NUM; i++) {
            m_3DBackColor[i].r = r;
            m_3DBackColor[i].g = g;
            m_3DBackColor[i].b = b;
            m_3DBackColor[i].a = a;
        }
    }

    void CWorldEnv::SetViewSplitType(ViewSplitType type)
    {
        m_vstViewSplitType = type;
        UpdateViewSplit();
    }

    void CWorldEnv::UpdateViewSplit()
    {
        // The aspect ratio of a view, by m_vatViewAspectType (0 4:3, 1 16:9)
        // and m_vstViewSplitType: halved for the side-by-side split (1),
        // doubled for the split into a top and a bottom view (2).
        static const float s_fViewAspect[2][4] = {
            { 1.3223685026168823f, 0.6611842513084412f, 2.6447370052337646f, 1.3223685026168823f },
            { 1.8052631616592407f, 0.9026315808296204f, 3.6105263233184814f, 1.8052631616592407f },
        };

        float fAspect = s_fViewAspect[m_vatViewAspectType][m_vstViewSplitType];
        for (long i = 0; i < VIEW_NUM; i++) {
            m_ViewCameraData[i].projAspect = fAspect;
            m_ViewProjInfo[i].projAspect = fAspect;
            m_ViewCameraData[i].flags &= ~0x88;
        }

        const GXRenderModeObj &rRmode = nw4r::g3d::G3DState::GetRenderModeObj();
        ViewProjInfo *pInfo = m_ViewProjInfo;
        switch (m_vstViewSplitType) {
            case 1:
                // Side by side: views 0 and 2 on the left, 1 and 3 on the
                // right.
                pInfo[0].viewportX = 0.0f;
                pInfo[0].viewportY = 0.0f;
                pInfo[1].viewportX = rRmode.fbWidth / 2 + 1;
                pInfo[1].viewportY = 0.0f;
                pInfo[2].viewportX = 0.0f;
                pInfo[2].viewportY = 0.0f;
                pInfo[3].viewportX = rRmode.fbWidth / 2 + 1;
                pInfo[3].viewportY = 0.0f;
                pInfo[0].scissorX = 0;
                pInfo[0].scissorY = 0;
                pInfo[1].scissorX = rRmode.fbWidth / 2 + 1;
                pInfo[1].scissorY = 0;
                pInfo[2].scissorX = 0;
                pInfo[2].scissorY = 0;
                pInfo[3].scissorX = rRmode.fbWidth / 2 + 1;
                pInfo[3].scissorY = 0;
                pInfo[0].viewportWidth = pInfo[1].viewportWidth = pInfo[2].viewportWidth =
                    pInfo[3].viewportWidth = rRmode.fbWidth / 2 - 1;
                pInfo[0].scissorWidth = pInfo[1].scissorWidth = pInfo[2].scissorWidth =
                    pInfo[3].scissorWidth = rRmode.fbWidth / 2 - 1;
                pInfo[0].viewportHeight = pInfo[1].viewportHeight = pInfo[2].viewportHeight =
                    pInfo[3].viewportHeight = rRmode.efbHeight;
                pInfo[0].scissorHeight = pInfo[1].scissorHeight = pInfo[2].scissorHeight =
                    pInfo[3].scissorHeight = rRmode.efbHeight;
                break;
            case 2:
                // One above the other: views 0 and 2 at the top, 1 and 3 at
                // the bottom.
                pInfo[0].viewportX = 0.0f;
                pInfo[0].viewportY = 0.0f;
                pInfo[1].viewportX = 0.0f;
                pInfo[1].viewportY = rRmode.efbHeight / 2 + 1;
                pInfo[2].viewportX = 0.0f;
                pInfo[2].viewportY = 0.0f;
                pInfo[3].viewportX = 0.0f;
                pInfo[3].viewportY = rRmode.efbHeight / 2 + 1;
                pInfo[0].scissorX = 0;
                pInfo[0].scissorY = 0;
                pInfo[1].scissorX = 0;
                pInfo[1].scissorY = rRmode.efbHeight / 2 + 1;
                pInfo[2].scissorX = 0;
                pInfo[2].scissorY = 0;
                pInfo[3].scissorX = 0;
                pInfo[3].scissorY = rRmode.efbHeight / 2 + 1;
                pInfo[0].viewportWidth = pInfo[1].viewportWidth = pInfo[2].viewportWidth =
                    pInfo[3].viewportWidth = rRmode.fbWidth;
                pInfo[0].scissorWidth = pInfo[1].scissorWidth = pInfo[2].scissorWidth =
                    pInfo[3].scissorWidth = rRmode.fbWidth;
                pInfo[0].viewportHeight = pInfo[1].viewportHeight = pInfo[2].viewportHeight =
                    pInfo[3].viewportHeight = rRmode.efbHeight / 2 - 1;
                pInfo[0].scissorHeight = pInfo[1].scissorHeight = pInfo[2].scissorHeight =
                    pInfo[3].scissorHeight = rRmode.efbHeight / 2 - 1;
                break;
            case 3:
                // Four quarters: 0 top left, 1 top right, 2 bottom left,
                // 3 bottom right.
                pInfo[0].viewportX = 0.0f;
                pInfo[0].viewportY = 0.0f;
                pInfo[1].viewportX = rRmode.fbWidth / 2 + 1;
                pInfo[1].viewportY = 0.0f;
                pInfo[2].viewportX = 0.0f;
                pInfo[2].viewportY = rRmode.efbHeight / 2 + 1;
                pInfo[3].viewportX = rRmode.fbWidth / 2 + 1;
                pInfo[3].viewportY = rRmode.efbHeight / 2 + 1;
                pInfo[0].scissorX = 0;
                pInfo[0].scissorY = 0;
                pInfo[1].scissorX = rRmode.fbWidth / 2 + 1;
                pInfo[1].scissorY = 0;
                pInfo[2].scissorX = 0;
                pInfo[2].scissorY = rRmode.efbHeight / 2 + 1;
                pInfo[3].scissorX = rRmode.fbWidth / 2 + 1;
                pInfo[3].scissorY = rRmode.efbHeight / 2 + 1;
                pInfo[0].viewportWidth = pInfo[1].viewportWidth = pInfo[2].viewportWidth =
                    pInfo[3].viewportWidth = rRmode.fbWidth / 2 - 1;
                pInfo[0].scissorWidth = pInfo[1].scissorWidth = pInfo[2].scissorWidth =
                    pInfo[3].scissorWidth = rRmode.fbWidth / 2 - 1;
                pInfo[0].viewportHeight = pInfo[1].viewportHeight = pInfo[2].viewportHeight =
                    pInfo[3].viewportHeight = rRmode.efbHeight / 2 - 1;
                pInfo[0].scissorHeight = pInfo[1].scissorHeight = pInfo[2].scissorHeight =
                    pInfo[3].scissorHeight = rRmode.efbHeight / 2 - 1;
                break;
            default:
                // One view: every view is the full screen.
                pInfo[0].viewportX = 0.0f;
                pInfo[0].viewportY = 0.0f;
                pInfo[1].viewportX = 0.0f;
                pInfo[1].viewportY = 0.0f;
                pInfo[2].viewportX = 0.0f;
                pInfo[2].viewportY = 0.0f;
                pInfo[3].viewportX = 0.0f;
                pInfo[3].viewportY = 0.0f;
                pInfo[0].scissorX = pInfo[0].scissorY = pInfo[1].scissorX = pInfo[1].scissorY =
                    pInfo[2].scissorX = pInfo[2].scissorY = pInfo[3].scissorX = pInfo[3].scissorY = 0;
                pInfo[0].viewportWidth = pInfo[1].viewportWidth = pInfo[2].viewportWidth =
                    pInfo[3].viewportWidth = rRmode.fbWidth;
                pInfo[0].scissorWidth = pInfo[1].scissorWidth = pInfo[2].scissorWidth =
                    pInfo[3].scissorWidth = rRmode.fbWidth;
                pInfo[0].viewportHeight = pInfo[1].viewportHeight = pInfo[2].viewportHeight =
                    pInfo[3].viewportHeight = rRmode.efbHeight;
                pInfo[0].scissorHeight = pInfo[1].scissorHeight = pInfo[2].scissorHeight =
                    pInfo[3].scissorHeight = rRmode.efbHeight;
                break;
        }

        // The full screen view.
        float fFullAspect = s_fViewAspect[m_vatViewAspectType][0];
        m_ViewCameraData[VIEW_NUM].projAspect = fFullAspect;
        m_ViewProjInfo[VIEW_NUM].projAspect = fFullAspect;
        m_ViewCameraData[VIEW_NUM].flags &= ~0x88;
        ViewProjInfo &rFull = m_ViewProjInfo[VIEW_NUM];
        rFull.viewportX = 0.0f;
        rFull.viewportY = 0.0f;
        rFull.scissorX = rFull.scissorY = 0;
        rFull.viewportWidth = rRmode.fbWidth;
        rFull.scissorWidth = rRmode.fbWidth;
        rFull.viewportHeight = rRmode.efbHeight;
        rFull.scissorHeight = rRmode.efbHeight;

        for (long i = 0; i < VIEW_NUM + 1; i++) {
            nw4r::g3d::CameraData &rData = m_ViewCameraData[i];
            const ViewProjInfo &rInfo = m_ViewProjInfo[i];
            rData.viewportOrigin.x = rInfo.viewportX;
            rData.viewportOrigin.y = rInfo.viewportY;
            rData.viewportSize.x = rInfo.viewportWidth;
            rData.viewportSize.y = rInfo.viewportHeight;
            rData.scissorX = rInfo.scissorX;
            rData.scissorY = rInfo.scissorY;
            rData.scissorWidth = rInfo.scissorWidth;
            rData.scissorHeight = rInfo.scissorHeight;
        }
    }

    long CWorldEnv::GetViewSplitNum()
    {
        switch (m_vstViewSplitType) {
            case 1:
            case 2:
                return 2;
            case 3:
                return 4;
            default:
                return 1;
        }
    }

    // The header's CameraData names the members its callers read; it has the
    // layout of nw4r::g3d::CameraData.
    CWorldEnv::CameraData *CWorldEnv::GetViewCameraData(long view)
    {
        return reinterpret_cast<CameraData *>(ViewCameraData_(view));
    }

    void CWorldEnv::GetViewCameraMtx(long view, nw4r::math::MTX34 *pMtx)
    {
        nw4r::g3d::CameraData *pData = ViewCameraData_(view);
        if (pData->flags & 8) {
            PSMTXCopy(pData->cameraMtx, *pMtx);
        } else {
            nw4r::g3d::Camera camera(pData);
            camera.GetCameraMtx(pMtx);
        }
    }

    CWorldEnv::ViewProjInfo *CWorldEnv::GetViewProjInfo(long view)
    {
        return ViewProjInfo_(view);
    }

    void CWorldEnv::SetViewFovy(long view, float fovy)
    {
        nw4r::g3d::CameraData *pData = ViewCameraData_(view);
        ViewProjInfo *pInfo = ViewProjInfo_(view);
        float f = fabsf(fovy);
        pData->projFovy = f;
        pInfo->projFovy = f;
    }

    float CWorldEnv::GetViewFovy(long view)
    {
        return ViewProjInfo_(view)->projFovy;
    }

    void CWorldEnv::SetViewNearFar(long view, float fNear, float fFar)
    {
        nw4r::g3d::CameraData *pData = ViewCameraData_(view);
        ViewProjInfo *pInfo = ViewProjInfo_(view);
        float f = fabsf(fNear);
        pData->projNear = f;
        pInfo->projNear = f;

        pData = ViewCameraData_(view);
        pInfo = ViewProjInfo_(view);
        f = fabsf(fFar);
        pData->projFar = f;
        pInfo->projFar = f;
    }

    void CWorldEnv::SetViewCameraData(long view, const nw4r::g3d::CameraData &rData)
    {
        nw4r::g3d::CameraData *pData = ViewCameraData_(view);
        ViewProjInfo *pInfo = ViewProjInfo_(view);
        *pData = rData;
        pData->projFovy = pInfo->projFovy;
        pData->projAspect = pInfo->projAspect;
        pData->projNear = pInfo->projNear;
        pData->projFar = pInfo->projFar;
        pData->viewportOrigin.x = pInfo->viewportX;
        pData->viewportOrigin.y = pInfo->viewportY;
        pData->viewportSize.x = pInfo->viewportWidth;
        pData->viewportSize.y = pInfo->viewportHeight;
        pData->scissorX = pInfo->scissorX;
        pData->scissorY = pInfo->scissorY;
        pData->scissorWidth = pInfo->scissorWidth;
        pData->scissorHeight = pInfo->scissorHeight;
        pData->flags &= ~0x88;
    }
}
