#include "bcs/EF/CResource.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CWorldEnv.hpp"
#include "nw4r/ef/EffectSystem.hpp"
#include "revolution/gx/GXTransform.h"
#include "revolution/gx/GXStruct.h"
#include "revolution/mtx.h"

namespace {

    // The object after pObj in pList. The DOL reads the link inline,
    // without List_GetNext's null check.
    inline void *GetNext(const nw4r::ut::List *pList, void *pObj)
    {
        return reinterpret_cast<nw4r::ut::Link *>(reinterpret_cast<char *>(pObj) + pList->offset)->pNext;
    }

    // (name and meaning guessed) A 0xA0-byte block CManager::CalcView fills
    // on the stack: the view matrix, the texture projection matrix, and
    // defaults. Nothing reads it afterwards in this build.
    struct ViewInfo {
        nw4r::math::MTX34 m_ViewMtx;  // 0x00
        nw4r::math::MTX34 m_TexMtx;   // 0x30
        unsigned char m_Unk60;        // 0x60
        unsigned long m_Unk64;        // 0x64
        unsigned long m_Unk68;        // 0x68
        unsigned char m_Unk6C;        // 0x6C
        unsigned long m_Unk70;        // 0x70
        float m_Unk74[4];             // 0x74
        float m_Unk84;                // 0x84
        float m_Unk88[4];             // 0x88
        _GXColor m_Color0;            // 0x98
        _GXColor m_Color1;            // 0x9C

        ViewInfo()
        {
            PSMTXIdentity(m_ViewMtx);
            PSMTXIdentity(m_TexMtx);
            m_Unk60 = 0;
            m_Unk64 = 0;
            m_Unk68 = 0;
            m_Unk6C = 1;
            m_Unk70 = 0;
            m_Unk74[0] = 0.0f;
            m_Unk74[1] = 1.0f;
            m_Unk74[2] = 0.0f;
            m_Unk74[3] = 1.0f;
            m_Unk88[0] = 0.0f;
            m_Unk88[1] = 0.0f;
            m_Unk88[2] = 0.0f;
            m_Unk88[3] = 0.0f;
            m_Color0.b = 255;
            m_Color0.g = 255;
            m_Color0.r = 255;
            m_Color0.a = 255;
            m_Color1.b = 0;
            m_Color1.g = 0;
            m_Color1.r = 0;
            m_Color1.a = 255;
        }
    };
}

namespace bcs {
    namespace EF {

        CManager *CManager::GetInstance()
        {
            return s_pInstance;
        }

        void CManager::Destroy()
        {
            if (s_pInstance) {
                s_pInstance->Delete();
                s_pInstance = 0;
            }
        }

        void CManager::Main()
        {
            if (CMainApp::IsPause(0xC000)) {
                return;
            }

            CalcAll();
            nw4r::ef::EffectSystem::GetInstance()->mMemoryManager->GarbageCollection();

            long nViews = CWorldEnv::GetViewSplitNum();
            for (long nGroup = 0; nGroup < 6; nGroup++) {
                if (nGroup == 4) {
                    CalcView(4, -1);
                } else {
                    for (long nView = 0; nView < nViews; nView++) {
                        CalcView(nGroup, nView);
                    }
                }
            }
        }

        void CManager::Disp()
        {
            for (long i = 0; i < 6; i++) {
                CEffectBase *pObj = static_cast<CEffectBase *>(m_aList[i].pHead);
                while (pObj != 0) {
                    if (CEffectBase *pBody = pObj->GetBody()) {
                        pBody->Disp();
                    }
                    pObj = static_cast<CEffectBase *>(GetNext(&m_aList[i], pObj));
                }
            }
        }

        void CManager::CalcAll()
        {
            CEffectBase *pObj;
            for (long i = 0; i < 6; i++) {
                pObj = static_cast<CEffectBase *>(m_aList[i].pHead);
                while (pObj != 0) {
                    CEffectBase *pBody = pObj->GetBody();
                    pObj = static_cast<CEffectBase *>(GetNext(&m_aList[i], pObj));
                    if (pBody == 0) {
                        continue;
                    }

                    long nFlag = pBody->GetFlag();
                    if (!(nFlag & CEffectBase::FLAG_STOP_CALC)) {
                        pBody->Calc();
                    }
                    // Read again: Calc may have set the delete bit.
                    nFlag = pBody->GetFlag();
                    if (((nFlag >> 2) & 1) == 1) {
                        delete pBody;
                    }
                }
            }
        }

        void CEffectBase::Calc()
        {
        }

        void CManager::CalcView(long nGroup, long nView)
        {
            ViewInfo info;
            float f1;
            float f2;

            CWorldEnv::CameraData *pCam = CWorldEnv::GetViewCameraData(nView);
            if (pCam == 0) {
                return;
            }

            info.m_ViewMtx = pCam->m_ViewMtx;
            if (nView >= 0) {
                nw4r::math::MTX34 texMtx;
                C_MTXLightPerspective(texMtx.m, pCam->m_fFovy, pCam->m_fAspect, 0.5f, -0.5f, 0.5f, 0.5f);
                info.m_TexMtx = texMtx;
                if (GetInstance()->m_bUnk95 == true) {
                    if (CWorldEnv::GetViewSplitNum() == 1) {
                        f1 = 10000.0f;
                        f2 = 20000.0f;
                    } else {
                        f1 = -20000.0f;
                        f2 = -10000.0f;
                    }
                } else {
                    f1 = 100.0f;
                    f2 = 300.0f;
                }
                GXSetProjection(pCam->m_ProjMtx, GX_PERSPECTIVE);
            } else {
                // f1 and f2 stay unset on this path in the original too.
                nw4r::math::MTX34 texMtx;
                C_MTXLightPerspective(texMtx.m, pCam->m_fFovy, pCam->m_fAspect, 0.5f, -0.5f, 0.5f, 0.5f);
                info.m_TexMtx = texMtx;
                GXSetProjection(pCam->m_ProjMtx, GX_PERSPECTIVE);
            }

            nw4r::ef::EffectSystem::GetInstance()->SetProcessCamera(
                reinterpret_cast<const nw4r::math::VEC3 &>(pCam->m_Eye), pCam->m_ViewMtx, f1, f2);

            for (CEffectBase *pObj = static_cast<CEffectBase *>(m_aList[nGroup].pHead); pObj != 0;
                 pObj = static_cast<CEffectBase *>(GetNext(&m_aList[nGroup], pObj))) {
                CEffectBase *pBody = pObj->GetBody();
                if (pBody && !(pBody->GetFlag() & CEffectBase::FLAG_STOP_CALC_VIEW)) {
                    pBody->CalcView(nView);
                }
            }
        }

        void CEffectBase::CalcView(long)
        {
        }

        void CEffectBase::Unk14()
        {
        }

        void CEffectBase::Disp()
        {
        }
    }
}
