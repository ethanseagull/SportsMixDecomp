#include "bcs/G2D/CLytManager.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/G2D/CLayoutBase.hpp"
#include "bcs/New.hpp"
#include "nw4r/lyt/drawInfo.h"
#include "nw4r/lyt/layout.h"
#include "nw4r/ut/Rect.h"
#include "revolution/gd.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXTransform.h"

// The DOL's heap functions (no symbols). fn_80065C84 returns the allocator
// of heap nHeap; fn_80065F7C allocates nSize bytes from heap nHeap (0 the
// default) with alignment nAlign (negative: from the end of the heap).
extern "C" MEMAllocator *fn_80065C84(long nHeap);
extern "C" void *fn_80065F7C(unsigned long nSize, long nHeap, long nAlign, long nType);

namespace bcs {
    namespace G2D {

        // The allocator nw4r::lyt gets: the default heap's, with its own
        // functions.
        static void *AllocForLyt(MEMAllocator *pAllocator, unsigned long nSize)
        {
            return fn_80065F7C(nSize, 0, -(long)pAllocator->heapParam1, 5);
        }

        static void FreeForLyt(MEMAllocator *pAllocator, void *pMem)
        {
            operator delete[](pMem);
        }

        static const MEMAllocatorFunc sLytAllocatorFunc = {AllocForLyt, FreeForLyt};

        static CLytManager *spInstance;
        static MEMAllocator sLytAllocator;
        static MEMAllocator *spLytAllocator;

        inline CLytManager::ObjNode *CLytManager::FindNode(unsigned long nId)
        {
            nw4r::ut::List *pList = &m_ObjList;
            for (ObjNode *pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, 0)); pNode != 0;
                 pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, pNode))) {
                if (nId == pNode->m_nId) {
                    return pNode;
                }
            }
            return 0;
        }

        inline void CLytManager::DeleteNodeAll()
        {
            nw4r::ut::List *pList = &m_ObjList;
            for (ObjNode *pNode = static_cast<ObjNode *>(nw4r::ut::List_GetLast(pList)); pNode != 0;
                 pNode = static_cast<ObjNode *>(nw4r::ut::List_GetLast(pList))) {
                nw4r::ut::List_Remove(pList, pNode);
                delete pNode->m_pObj;
                delete pNode;
            }
        }

        CLytManager::~CLytManager()
        {
            DeleteNodeAll();
            if (m_pLinkPool != 0) {
                delete[] m_pLinkPool;
            }
            m_pLinkPool = 0;
        }

        inline CLytManager::CLytManager() : CTask(0, 0x37)
        {
            m_nFlag = 0;
            CBase::s_nNextId = 1;
            SetUnk08(0xC000);
            if (nw4r::lyt::Layout::mspAllocator == 0) {
                nw4r::lyt::LytInit();
                nw4r::lyt::Layout::mspAllocator = GetAllocator();
            }
            nw4r::ut::List_Init(&m_ObjList, 0);
            m_nFlag = 0;
            if (CWorldEnv::m_vatViewAspectType == 1) {
                m_nFlag |= 1;
            } else {
                m_nFlag &= ~1;
            }
            m_Unk40 = 6;
            m_pLinkPool = new (0) LinkNode[0x400];
            m_nLinkNum = 0;
        }

        CLytManager *CLytManager::CreateInstance()
        {
            if (spInstance == 0) {
                spInstance = new CLytManager();
            }
            return spInstance;
        }

        CLytManager *CLytManager::GetInstance()
        {
            return spInstance;
        }

        void CLytManager::DestroyInstance()
        {
            if (spInstance != 0) {
                spInstance->Delete();
                spInstance = 0;
            }
        }

        MEMAllocator *CLytManager::GetAllocator()
        {
            if (spLytAllocator == 0) {
                sLytAllocator = *fn_80065C84(0);
                sLytAllocator.pFunc = &sLytAllocatorFunc;
                spLytAllocator = &sLytAllocator;
            }
            return spLytAllocator;
        }

        void CLytManager::DeleteObjectAll()
        {
            DeleteNodeAll();
        }

        void CLytManager::Main()
        {
            nw4r::ut::List *pList = &m_ObjList;
            ObjNode *pNext = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, 0));
            while (pNext != 0) {
                ObjNode *pNode = pNext;
                pNext = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, pNode));
                CBase *pObj = pNode->m_pObj;
                if (pObj != 0 && (pObj->m_nFlag & 8) == 0 && pObj->m_nWait == 0) {
                    pObj->Unk0C();
                }
            }
        }

        void CLytManager::Disp()
        {
            m_nLinkNum = 0;
        }

        int CLytManager::AddObject(CBase *pObj, long nBit)
        {
            int bAdded = 0;
            if (FindNode(pObj->m_nId) == 0) {
                ObjNode *pNode = new ObjNode;
                pNode->m_nId = pObj->m_nId;
                pNode->m_pObj = pObj;
                pNode->m_nMask = 1 << nBit;
                if (pNode != 0) {
                    nw4r::ut::List_Append(&m_ObjList, pNode);
                }
                bAdded = 1;
            }
            return bAdded;
        }

        int CLytManager::DeleteObject(CBase *pObj)
        {
            if (pObj == 0) {
                return 0;
            }
            int bDeleted = 0;
            ObjNode *pNode = FindNode(pObj->m_nId);
            if (pNode != 0) {
                if (pNode != 0) {
                    nw4r::ut::List_Remove(&m_ObjList, pNode);
                }
                delete pNode;
                delete pObj;
                bDeleted = 1;
            }
            return bDeleted;
        }

        CBase *CLytManager::GetObject(unsigned long nId)
        {
            for (int i = 0; i < 8; i++) {
                ObjNode *pNode = FindNode(nId);
                if (pNode != 0) {
                    return pNode->m_pObj;
                }
            }
            return 0;
        }

        long CLytManager::SetupDraw(nw4r::lyt::DrawInfo *pDrawInfo, const nw4r::ut::Rect *pRect, long nView, unsigned long nGxType)
        {
            nw4r::ut::Rect rect;
            rect.left = pRect->left;
            rect.top = pRect->top;
            rect.right = pRect->right;
            rect.bottom = pRect->bottom;
            if (nView >= 0) {
                switch (CWorldEnv::m_vstViewSplitType) {
                    case 1:
                        rect.left *= 0.5f;
                        rect.right *= 0.5f;
                        break;
                    case 2:
                        rect.top *= 0.5f;
                        rect.bottom *= 0.5f;
                        break;
                    case 3:
                        rect.left *= 0.5f;
                        rect.right *= 0.5f;
                        rect.top *= 0.5f;
                        rect.bottom *= 0.5f;
                        break;
                }
            }

            Mtx44 projMtx;
            long nResult = CalcDrawInfo(pDrawInfo, &rect, nView, projMtx);
            if (nResult < 0) {
                return nResult;
            }
            if (nGxType == 'GX' || nGxType == 'GD') {
                SetProjection(GX_ORTHOGRAPHIC, projMtx, nGxType);
            }
            return 0;
        }

        long CLytManager::CalcDrawInfo(nw4r::lyt::DrawInfo *pDrawInfo, const nw4r::ut::Rect *pRect, long nView, Mtx44 projMtx)
        {
            pDrawInfo->SetLocationAdjustScale(nw4r::math::VEC2(GetLocationAdjustScaleH(GetAdjustMode()), GetLocationAdjustScaleV(GetAdjustMode())));
            pDrawInfo->SetLocationAdjust(m_nFlag & 1);
            pDrawInfo->SetDebugDrawMode((m_nFlag >> 1) & 1);

            nw4r::math::MTX34 viewMtx;
            nw4r::ut::Rect viewRect;
            float fLeft;
            float fTop;
            float fRight;
            float fBottom;
            bool bMultipleView;
            if (nView >= 0) {
                fLeft = 0.0f;
                fTop = 0.0f;
                fRight = fLeft + (pRect->right - pRect->left);
                fBottom = fTop + (pRect->bottom - pRect->top);
                C_MTXOrtho(projMtx, fTop, fBottom, fLeft, fRight, -1000.0f, 1000.0f);
                PSMTXIdentity(viewMtx);
                bMultipleView = false;
            } else {
                fLeft = pRect->left;
                fBottom = pRect->bottom;
                fRight = pRect->right;
                fTop = pRect->top;
                C_MTXOrtho(projMtx, fTop, fBottom, fLeft, fRight, -1000.0f, 1000.0f);
                PSMTXIdentity(viewMtx);

                // Undo the location adjustment's vertical stretch.
                FVEC3 scale;
                GetLocationAdjustScale(&scale, pDrawInfo->IsLocationAdjust());
                nw4r::math::MTX34 scaleMtx;
                PSMTXIdentity(scaleMtx);
                nw4r::math::MTX34Scale(&scaleMtx, &scaleMtx, reinterpret_cast<const nw4r::math::VEC3 *>(&scale));
                PSMTXConcat(viewMtx, scaleMtx, viewMtx);
                bMultipleView = false;
            }

            pDrawInfo->SetViewMtx(viewMtx);
            pDrawInfo->SetMultipleViewMtxOnDraw(bMultipleView);
            viewRect.left = fLeft;
            viewRect.top = fTop;
            viewRect.right = fRight;
            viewRect.bottom = fBottom;
            pDrawInfo->SetViewRect(viewRect);
            return 0;
        }

        void CLytManager::SetProjection(GXProjectionType type, Mtx44 projMtx, unsigned long nGxType)
        {
            switch (nGxType) {
                case 'GX': {
                    GXSetProjection(projMtx, type);
                    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
                    GXColor color = {0, 0, 0, 0};
                    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
                    GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
                    GXSetCoPlanar(GX_FALSE);
                    GXSetCullMode(GX_CULL_NONE);
                    break;
                }
                case 'GD': {
                    GDSetProjection(projMtx, type);
                    GDSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
                    GXColor color = {0, 0, 0, 0};
                    GDSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
                    GDSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
                    GDSetCoPlanar(GX_FALSE);
                    GDSetCullMode(GX_CULL_NONE);
                    break;
                }
            }
        }

        float CLytManager::GetLocationAdjustScaleH(int bAdjust)
        {
            float fAspect = bAdjust ? 1.2f : 0.9f;
            return (float)(long)CMainApp::g_Rmode.fbWidth / (float)(long)CMainApp::g_Rmode.viWidth / fAspect;
        }

        float CLytManager::GetLocationAdjustScaleV(int bAdjust)
        {
            return 1.0f;
        }

        static inline void GetSafeAreaRate(FVEC3 *pRate, int bAdjust)
        {
            pRate->z = 0.0f;
            if (bAdjust) {
                pRate->x = 0.87f;
                pRate->y = 0.87f;
            } else {
                pRate->x = 0.844f;
                pRate->y = 0.808f;
            }
        }

        // The safe-area rate and the converted VI width are computed but not
        // used: the original keeps their dead stack stores.
        void CLytManager::GetLocationAdjustScale(FVEC3 *pScale, int bAdjust)
        {
            FVEC3 rate;
            GetSafeAreaRate(&rate, bAdjust);
            float fViWidth = (float)(long)CMainApp::g_Rmode.viWidth;
            pScale->x = 480.0f / (float)(long)CMainApp::g_Rmode.efbHeight;
            pScale->y = 1.0f;
            pScale->z = 1.0f;
        }

        CLytManager::LinkNode *CLytManager::NewLink(void *pData)
        {
            long nIndex = m_nLinkNum++;
            m_pLinkPool[nIndex].m_pData = pData;
            return &m_pLinkPool[nIndex];
        }

        void CLytManager::SetHideAll(long nLayer, int bHide)
        {
            nw4r::ut::List *pList = &m_ObjList;
            for (ObjNode *pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, 0)); pNode != 0;
                 pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, pNode))) {
                CBase *pObj = pNode->m_pObj;
                if (pObj == 0) {
                    continue;
                }
                unsigned char nCount = pObj->m_nHideCount;
                if (bHide == 1) {
                    if (nCount < 255) {
                        nCount++;
                    }
                } else if (nCount != 0) {
                    nCount--;
                }
                pObj->m_nHideCount = nCount;

                int bHidden = nCount != 0;
                if (bHidden == 1) {
                    pObj->m_nFlag |= 8;
                } else {
                    pObj->m_nFlag &= ~8;
                }
                if (bHidden == 1) {
                    pObj->m_nFlag |= 0x10;
                } else {
                    pObj->m_nFlag &= ~0x10;
                }
            }
        }

        void CLytManager::AddWaitAll(long nLayer, long nAdd)
        {
            nw4r::ut::List *pList = &m_ObjList;
            for (ObjNode *pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, 0)); pNode != 0;
                 pNode = static_cast<ObjNode *>(nw4r::ut::List_GetNext(pList, pNode))) {
                CBase *pObj = pNode->m_pObj;
                if (pObj != 0) {
                    long nWait = nAdd + pObj->m_nWait;
                    if (nWait < 0) {
                        nWait = 0;
                    }
                    pObj->m_nWait = nWait;
                }
            }
        }
    }
}
