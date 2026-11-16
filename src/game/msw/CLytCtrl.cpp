#include "game/msw/CLytCtrl.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/G2D/CFontMan.hpp"
#include "bcs/G2D/CLayoutBase.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CLytResource.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/ArchiveFont.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "nw4r/ut/Font.hpp"

extern "C" {
    // (game/msw/menu.cpp; name unknown) Writes the file name pPath without
    // its extension to pName; returns its size with the terminator, or 0
    // when it does not fit.
    int fn_8019B780(const char *pPath, char *pName);
}

namespace msw {

    enum {
        ARC_NUM = 10,       // layout archives CLoadData holds
        LAYOUT_ARC_NUM = 3, // of them, the ones GetLayoutResourceAccesor searches
        FILE_FONT = 0x58    // CLoadData entry of the font archive
    };

    static const long s_ArcId[ARC_NUM] = {
        0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x58,
    };

    static const long s_LayoutArcId[LAYOUT_ARC_NUM] = { 0x4B, 0x4C, 0x4D };

    static const char *const s_pRootDir = "arc";

    unsigned long CLytCtrl::s_InstanceHandle;

    // Fills every text box under pPane whose name starts with "t_" with the
    // text of that label in message file nFile.
    static void SetTextBoxFromMessage(short nFile, nw4r::lyt::Pane *pPane)
    {
        menu::CMessage *pMessage = &menu::CMessage::s_Instance;
        const char *pName = pPane->GetName();
        // The first two characters read as one big-endian halfword (one
        // lhz in the original); a little-endian port must compare them
        // one by one.
        if (*reinterpret_cast<const unsigned short *>(pName) == 't_') {
            bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);
            if (pTextBox != 0) {
                short nLength = pMessage->GetTextLengthByLabel(nFile, pName);
                if (nLength > 0) {
                    pTextBox->SetString(pMessage->GetTextByLabel(nFile, pName), 0, nLength);
                }
            }
        }

        nw4r::ut::LinkList<nw4r::lyt::Pane, 4> &rList = pPane->GetChildList();
        for (nw4r::ut::LinkList<nw4r::lyt::Pane, 4>::Iterator it = rList.GetBeginIter(); it != rList.GetEndIter(); ++it) {
            SetTextBoxFromMessage(nFile, &*it);
        }
    }

    CLytCtrl::~CLytCtrl()
    {
        if (!IsDelete()) {
            Dispose();
            DetachResource();
            Close();
        }
    }

    void CLytCtrl::Initialize()
    {
        SetUnk08(0xC000);
        m_nPauseFlag = 0;

        bcs::G2D::CFontMan *pFontMan = bcs::G2D::CFontMan::GetInstance();
        if (pFontMan == 0) {
            pFontMan = bcs::G2D::CFontMan::CreateInstance();
        }

        long nFontNum = ArchiveFont::GetFontNum();
        nw4r::lyt::ArcResourceAccessor *pAccessor =
            bcs::G2D::CLytResource::GetInstance()->NewArcResourceAccessor(CLoadData::GetFilePath(FILE_FONT), s_pRootDir);
        if (pAccessor != 0 && pFontMan != 0) {
            for (long i = 0; i < nFontNum; i++) {
                void *pRes = pAccessor->GetResource('fnta', ArchiveFont::GetFontName(i), 0);
                if (pRes != 0) {
                    pFontMan->SetFont(ArchiveFont::GetFontSlot(i),
                                      bcs::G2D::CreateArchiveFont(pRes, ArchiveFont::GetGlyphList(i)));
                }
            }
            bcs::CLoadMan::UnLoad(CLoadData::GetFilePath(FILE_FONT), CLoadData::GetFileId(FILE_FONT));
        }

        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager != 0) {
            if (bcs::CWorldEnv::m_vatViewAspectType == 1) {
                pManager->OnAdjustMode();
            } else {
                pManager->OffAdjustMode();
            }
        }
    }

    inline CLytCtrl::CLytCtrl(bcs::CTask *pParent)
        : bcs::CTask(pParent, -1)
    {
        bcs::G2D::CLytManager::CreateInstance();
        bcs::G2D::CLytResource::CreateInstance();

        bcs::G2D::CLytResource *pResource = bcs::G2D::CLytResource::GetInstance();
        if (pResource != 0) {
            for (long i = 0; i < ARC_NUM; i++) {
                long nId = s_ArcId[i];
                pResource->AttachResource(CLoadData::GetFilePath(nId), CLoadData::GetPtr(nId), CLoadData::GetSize(nId));
            }
        }

        Initialize();
    }

    inline CLytCtrl::CLytCtrl(bcs::CTask *pParent, const ArcInfo *pArc, long nNum)
        : bcs::CTask(pParent, -1)
    {
        bcs::G2D::CLytManager::CreateInstance();
        bcs::G2D::CLytResource::CreateInstance();
        AttachResource(pArc, nNum);
        Initialize();
    }

    CLytCtrl *CLytCtrl::CreateInstance(bcs::CTask *pParent)
    {
        CLytCtrl *pLytCtrl = GetInstance();
        if (pLytCtrl == 0) {
            pLytCtrl = new CLytCtrl(pParent);
            if (pLytCtrl != 0) {
                s_InstanceHandle = pLytCtrl->GetId();
            }
        }
        return pLytCtrl;
    }

    CLytCtrl *CLytCtrl::CreateInstance(bcs::CTask *pParent, const ArcInfo *pArc, long nNum)
    {
        CLytCtrl *pLytCtrl = GetInstance();
        if (pLytCtrl == 0) {
            pLytCtrl = new CLytCtrl(pParent, pArc, nNum);
            if (pLytCtrl != 0) {
                s_InstanceHandle = pLytCtrl->GetId();
            }
        }
        return pLytCtrl;
    }

    CLytCtrl *CLytCtrl::GetInstance()
    {
        return static_cast<CLytCtrl *>(bcs::CTaskMan::GetTask(s_InstanceHandle));
    }

    void CLytCtrl::AttachResource(const ArcInfo *pArc, long nNum)
    {
        bcs::G2D::CLytResource *pResource = bcs::G2D::CLytResource::GetInstance();
        if (pResource == 0) {
            return;
        }

        for (long i = 0; i < ARC_NUM; i++) {
            long nId = s_ArcId[i];
            const ArcInfo *pInfo = pArc;
            for (long j = 0; j < nNum; j++, pInfo++) {
                if (nId == pInfo->m_nId) {
                    pResource->AttachResource(pInfo->m_pPath, pInfo->m_pData, pInfo->m_nSize);
                }
            }
        }
    }

    void CLytCtrl::DetachResource()
    {
        bcs::G2D::CLytResource *pResource = bcs::G2D::CLytResource::GetInstance();
        if (pResource != 0) {
            pResource->CleanupAll();
            bcs::G2D::CLytResource::DestroyInstance();
        }
    }

    void CLytCtrl::Dispose()
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager != 0) {
            pManager->DeleteObjectAll();
        }
    }

    void CLytCtrl::Close()
    {
        CLytCtrl *pLytCtrl = GetInstance();
        if (pLytCtrl != 0) {
            bcs::G2D::CFontMan *pFontMan = bcs::G2D::CFontMan::GetInstance();
            if (pFontMan != 0) {
                pFontMan->DeleteFont(pFontMan->GetFont(3));
            }
            pLytCtrl->Delete();
            s_InstanceHandle = 0;
        }
    }

    void CLytCtrl::DeleteObject(unsigned long nHandle)
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager != 0) {
            bcs::G2D::CBase *pObj = pManager->GetObject(nHandle);
            if (pObj != 0) {
                pManager->DeleteObject(pObj);
            }
        }
    }

    void CLytCtrl::UnregistFont(nw4r::lyt::ArcResourceAccessor *pAccessor, nw4r::lyt::FontRefLink *pLink)
    {
        if (pLink != 0 && bcs::G2D::CLytResource::GetInstance() != 0) {
            if (pAccessor != 0) {
                pAccessor->UnregistFont(pLink);
            }
            bcs::G2D::CLytResource::DeleteFontRefLink(pLink);
        }
    }

    nw4r::lyt::ArcResourceAccessor *CLytCtrl::NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName)
    {
        return bcs::G2D::CLytResource::GetInstance()->NewArcResourceAccessor(pFileName, pRootFolderName);
    }

    void CLytCtrl::DeleteArcResourceAccessor(nw4r::lyt::ArcResourceAccessor *pAccessor)
    {
        if (bcs::G2D::CLytResource::GetInstance() != 0) {
            bcs::G2D::CLytResource::DeleteArcResourceAccessor(pAccessor);
        }
    }

    nw4r::ut::Font *CLytCtrl::GetArchiveFont(long nFont)
    {
        bcs::G2D::CFontMan *pFontMan = bcs::G2D::CFontMan::GetInstance();
        if (pFontMan == 0) {
            return 0;
        }
        return pFontMan->GetFont(nFont);
    }

    const char *CLytCtrl::GetFontResourceName(long nFont)
    {
        return ArchiveFont::GetFontName(nFont);
    }

    unsigned long CLytCtrl::IsLocationAdjust()
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager == 0) {
            return 0;
        }
        return pManager->GetAdjustMode();
    }

    nw4r::lyt::ResourceAccessor *CLytCtrl::GetLayoutResourceAccesor(long nType, const char *pLayoutName)
    {
        for (long i = 0; i < LAYOUT_ARC_NUM; i++) {
            nw4r::lyt::ArcResourceAccessor *pAccessor =
                NewArcResourceAccessor(CLoadData::GetFilePath(s_LayoutArcId[i]), s_pRootDir);
            if (pAccessor != 0) {
                if (pAccessor->GetResource(nType, pLayoutName, 0) != 0) {
                    return pAccessor;
                }
                DeleteArcResourceAccessor(pAccessor);
            }
        }
        return 0;
    }

    void CLytCtrl::SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout, const char *pName)
    {
        char szFileName[128];
        fn_8019B780(pName, szFileName);

        short nFile = menu::CMessage::s_Instance.GetFileIndex(szFileName);
        if (nFile < 0) {
            return;
        }
        msw::SetTextBoxFromMessage(nFile, pLayout->GetLayout()->GetRootPane());
    }

    void CLytCtrl::Main()
    {
        UpdatePause();
    }

    // Hides the layers 0 to 5 of the layout manager once more (bHide 1) or
    // once less.
    static inline void SetHideAll(int bHide)
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager != 0) {
            pManager->SetHideAll(0, bHide);
            pManager->SetHideAll(1, bHide);
            pManager->SetHideAll(2, bHide);
            pManager->SetHideAll(3, bHide);
            pManager->SetHideAll(4, bHide);
            pManager->SetHideAll(5, bHide);
        }
    }

    void CLytCtrl::UpdatePause()
    {
        int bHidden = m_nPauseFlag & 1;
        int bPause = CGame::g_uPauseBit & 1;
        if (!bHidden && bPause) {
            m_nPauseFlag |= 1;
            SetHideAll(1);
        } else if (bHidden && !bPause) {
            m_nPauseFlag &= ~1;
            SetHideAll(0);
        }

        if (!(m_nPauseFlag & 8) && bcs::CMainApp::IsPause(1)) {
            m_nPauseFlag |= 8;
            SetHideAll(1);
        } else if ((m_nPauseFlag & 8) && !bcs::CMainApp::IsPause(1)) {
            m_nPauseFlag &= ~8;
            SetHideAll(0);
        }
    }

    long CLytCtrl::SetPauseStop()
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager == 0) {
            return -1;
        }
        pManager->AddWaitAll(0, 1);
        pManager->AddWaitAll(5, 1);
        pManager->AddWaitAll(7, 1);
        return 0;
    }

    long CLytCtrl::ClearPauseStop()
    {
        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        if (pManager == 0) {
            return -1;
        }
        pManager->AddWaitAll(0, -1);
        pManager->AddWaitAll(5, -1);
        pManager->AddWaitAll(7, -1);
        return 0;
    }
}
