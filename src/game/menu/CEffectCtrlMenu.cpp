#include "game/msw/CEffectCtrlMenu.hpp"

#include "bcs/CArcFileAccessor.hpp"
#include "bcs/EF/CResource.hpp"
#include "game/msw/CEffectTable.hpp"
#include "game/msw/CLoadData.hpp"

msw::CEffectCtrl *msw::CEffectCtrlMenu::CreateInstance(bcs::CTask *pParent)
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance == 0) {
        pInstance = new CEffectCtrlMenu(pParent);
        SetInstance(pInstance);
    }
    return pInstance;
}

msw::CEffectCtrlMenu::~CEffectCtrlMenu()
{
    CEffectCtrl *pInstance = GetInstance();

    if (pInstance != 0) {
        pInstance->Delete();
        // The original stores the handle again here; it is not cleared.
        SetInstance(pInstance);
    }
}

void msw::CEffectCtrlMenu::InitializeDataTable()
{
    bcs::EF::CResource *pResource = bcs::EF::CResource::GetInstance();

    if (pResource != 0) {
        long fileIds[3] = { 0x33, 0x29, 0x45 };
        CEffectTableList *pTableList = new CEffectTableList;
        CEffectTableList *pAttachTableList = new CEffectTableList;

        for (int i = 0; i < 3; i++) {
            bcs::CArcFileAccessor *pAccessor =
                pResource->GetArcAccesor(CLoadData::GetFilePath(fileIds[i]), g_EffectCommonDataArchiveRoot);
            unsigned long size;

            CEffectTable *pTable = new CEffectTable;
            if (pTable != 0) {
                unsigned char *pData = pAccessor->GetResource('tabl', g_EffectTableFileName, &size);
                if (pData != 0) {
                    pTable->Parse(pData);
                    pTableList->Add(pTable);
                } else {
                    delete pTable;
                }
            }

            CEffectAttachTable *pAttachTable = new CEffectAttachTable;
            if (pAttachTable != 0) {
                unsigned char *pData = pAccessor->GetResource('tabl', g_EffectAttachTableFileName, &size);
                if (pData != 0) {
                    pAttachTable->Parse(pData);
                    pAttachTableList->Add(pAttachTable);
                } else {
                    delete pAttachTable;
                }
            }

            pResource->ReleaseArcAccesor(pAccessor);
        }

        m_pTableList = pTableList;
        m_pAttachTableList = pAttachTableList;
    }
}

void msw::CEffectCtrlMenu::AttachResource()
{
    if (bcs::EF::CManager::GetInstance() != 0) {
        bcs::EF::CResource *pResource = bcs::EF::CResource::GetInstance();

        if (pResource != 0) {
            long fileIds[4] = { 0x33, 0x29, 0x41, 0x45 };
            long fileId;
            int i;

            for (i = 0; i < 4; i++) {
                fileId = fileIds[i];

                if (CLoadData::IsComplete(fileId)) {
                    if (!pResource->GetResourceAfterLoad(CLoadData::GetFilePath(fileId), CLoadData::GetPtr(fileId),
                                                         CLoadData::GetSize(fileId))) {
                        const char *pPath = CLoadData::GetFilePath(fileId);

                        pResource->AddArchive(pPath);
                        AttachArcResource(fileId, pPath);
                    }
                }
            }

            pResource->RelocateCommand();
            pResource->InvalidateCache();
        }
    }
}

void msw::CEffectCtrlMenu::Main()
{
    CEffectCtrl::Main();
}

void msw::CEffectCtrlMenu::Disp()
{
}

int msw::CEffectCtrlMenu::InitPatch(long, bcs::EF::CEffect *)
{
    return 0;
}
