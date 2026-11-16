#include "bcs/G2D/CLytResource.hpp"

#include "bcs/New.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include "string.h"

namespace bcs {
    namespace G2D {

        CLytResource *CLytResource::s_pInstance;

        CLytResource *CLytResource::GetInstance()
        {
            return s_pInstance;
        }

        void CLytResource::DestroyInstance()
        {
            if (s_pInstance != 0) {
                delete s_pInstance;
            }
            s_pInstance = 0;
        }

        void CLytResource::Clear()
        {
            ObjNode<nw4r::lyt::MultiArcResourceAccessor> *pMulti;
            for (pMulti = static_cast<ObjNode<nw4r::lyt::MultiArcResourceAccessor> *>(nw4r::ut::List_GetLast(&m_MultiAccessorList));
                 pMulti != 0;
                 pMulti = static_cast<ObjNode<nw4r::lyt::MultiArcResourceAccessor> *>(nw4r::ut::List_GetLast(&m_MultiAccessorList))) {
                nw4r::ut::List_Remove(&m_MultiAccessorList, pMulti);
                pMulti->m_pObj->DetachAll();
                delete pMulti->m_pObj;
                delete pMulti;
            }

            ObjNode<nw4r::lyt::ArcResourceLink> *pLink;
            for (pLink = static_cast<ObjNode<nw4r::lyt::ArcResourceLink> *>(nw4r::ut::List_GetLast(&m_ArcLinkList));
                 pLink != 0;
                 pLink = static_cast<ObjNode<nw4r::lyt::ArcResourceLink> *>(nw4r::ut::List_GetLast(&m_ArcLinkList))) {
                nw4r::ut::List_Remove(&m_ArcLinkList, pLink);
                delete pLink->m_pObj;
                delete pLink;
            }

            ObjNode<nw4r::lyt::ArcResourceAccessor> *pArc;
            for (pArc = static_cast<ObjNode<nw4r::lyt::ArcResourceAccessor> *>(nw4r::ut::List_GetLast(&m_ArcAccessorList));
                 pArc != 0;
                 pArc = static_cast<ObjNode<nw4r::lyt::ArcResourceAccessor> *>(nw4r::ut::List_GetLast(&m_ArcAccessorList))) {
                nw4r::ut::List_Remove(&m_ArcAccessorList, pArc);
                pArc->m_pObj->Detach();
                delete pArc->m_pObj;
                delete pArc;
            }

            ObjNode<nw4r::lyt::ResourceAccessor> *pAcc;
            for (pAcc = static_cast<ObjNode<nw4r::lyt::ResourceAccessor> *>(nw4r::ut::List_GetLast(&m_AccessorList));
                 pAcc != 0;
                 pAcc = static_cast<ObjNode<nw4r::lyt::ResourceAccessor> *>(nw4r::ut::List_GetLast(&m_AccessorList))) {
                nw4r::ut::List_Remove(&m_AccessorList, pAcc);
                delete pAcc->m_pObj;
                delete pAcc;
            }

            ObjNode<nw4r::lyt::FontRefLink> *pFont;
            for (pFont = static_cast<ObjNode<nw4r::lyt::FontRefLink> *>(nw4r::ut::List_GetLast(&m_FontRefLinkList));
                 pFont != 0;
                 pFont = static_cast<ObjNode<nw4r::lyt::FontRefLink> *>(nw4r::ut::List_GetLast(&m_FontRefLinkList))) {
                nw4r::ut::List_Remove(&m_FontRefLinkList, pFont);
                delete pFont->m_pObj;
                delete pFont;
            }

            DataNode *pData;
            for (pData = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList));
                 pData != 0;
                 pData = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList))) {
                nw4r::ut::List_Remove(&m_DataList, pData);
                if (!(pData->m_nFlags & 1)) {
                    if (pData->m_pData != 0) {
                        delete[] static_cast<unsigned char *>(pData->m_pData);
                    }
                    pData->m_pData = 0;
                }
                delete pData;
            }
        }

        inline void *CLytResource::FindData(const char *pName)
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (strcmp(pName, pNode->m_szName) == 0) {
                    return pNode->m_pData;
                }
            }
            return 0;
        }

        nw4r::lyt::ArcResourceAccessor *CLytResource::NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName)
        {
            if (pFileName == 0 || strlen(pFileName) == 0 || pRootFolderName == 0 || strlen(pRootFolderName) == 0) {
                return 0;
            }

            void *pArchive = FindData(pFileName);
            if (pArchive == 0) {
                return 0;
            }

            nw4r::lyt::ArcResourceAccessor *pAccessor = new (0) nw4r::lyt::ArcResourceAccessor();
            if (pAccessor != 0) {
                if (!pAccessor->Attach(pArchive, pRootFolderName)) {
                    delete pAccessor;
                    pAccessor = 0;
                } else {
                    ObjNode<nw4r::lyt::ArcResourceAccessor> *pNode = new (0) ObjNode<nw4r::lyt::ArcResourceAccessor>;
                    pNode->m_pObj = pAccessor;
                    nw4r::ut::List_Append(&m_ArcAccessorList, pNode);
                }
            }
            return pAccessor;
        }

        nw4r::lyt::FontRefLink *CLytResource::NewFontRefLink()
        {
            nw4r::lyt::FontRefLink *pLink = new (0) nw4r::lyt::FontRefLink();
            if (pLink != 0) {
                ObjNode<nw4r::lyt::FontRefLink> *pNode = new (0) ObjNode<nw4r::lyt::FontRefLink>;
                pNode->m_pObj = pLink;
                nw4r::ut::List_Append(&m_FontRefLinkList, pNode);
            }
            return pLink;
        }

        void CLytResource::DeleteArcResourceAccessor(nw4r::lyt::ArcResourceAccessor *pAccessor)
        {
            CLytResource *pRes = s_pInstance;
            if (pRes == 0) {
                return;
            }
            nw4r::ut::List *pList = &pRes->m_ArcAccessorList;
            ObjNode<nw4r::lyt::ArcResourceAccessor> *pNode;
            for (pNode = static_cast<ObjNode<nw4r::lyt::ArcResourceAccessor> *>(nw4r::ut::List_GetFirst(pList)); pNode != 0;
                 pNode = static_cast<ObjNode<nw4r::lyt::ArcResourceAccessor> *>(nw4r::ut::List_GetNext(pList, pNode))) {
                if (pNode->m_pObj == pAccessor) {
                    pAccessor->Detach();
                    delete pAccessor;
                    nw4r::ut::List_Remove(pList, pNode);
                    delete pNode;
                    return;
                }
            }
        }

        void CLytResource::DeleteArcResourceLink(nw4r::lyt::ArcResourceLink *pLink)
        {
            CLytResource *pRes = s_pInstance;
            if (pRes == 0) {
                return;
            }
            nw4r::ut::List *pList = &pRes->m_ArcLinkList;
            ObjNode<nw4r::lyt::ArcResourceLink> *pNode;
            for (pNode = static_cast<ObjNode<nw4r::lyt::ArcResourceLink> *>(nw4r::ut::List_GetFirst(pList)); pNode != 0;
                 pNode = static_cast<ObjNode<nw4r::lyt::ArcResourceLink> *>(nw4r::ut::List_GetNext(pList, pNode))) {
                if (pNode->m_pObj == pLink) {
                    delete pLink;
                    nw4r::ut::List_Remove(pList, pNode);
                    delete pNode;
                    return;
                }
            }
        }

        void CLytResource::DeleteMultiArcResourceAccessor(nw4r::lyt::MultiArcResourceAccessor *pAccessor)
        {
            CLytResource *pRes = s_pInstance;
            if (pRes == 0) {
                return;
            }
            nw4r::ut::List *pList = &pRes->m_MultiAccessorList;
            ObjNode<nw4r::lyt::MultiArcResourceAccessor> *pNode;
            for (pNode = static_cast<ObjNode<nw4r::lyt::MultiArcResourceAccessor> *>(nw4r::ut::List_GetFirst(pList)); pNode != 0;
                 pNode = static_cast<ObjNode<nw4r::lyt::MultiArcResourceAccessor> *>(nw4r::ut::List_GetNext(pList, pNode))) {
                if (pNode->m_pObj == pAccessor) {
                    delete pAccessor;
                    nw4r::ut::List_Remove(pList, pNode);
                    delete pNode;
                    return;
                }
            }
        }

        void CLytResource::DeleteFontRefLink(nw4r::lyt::FontRefLink *pLink)
        {
            CLytResource *pRes = s_pInstance;
            if (pRes == 0) {
                return;
            }
            nw4r::ut::List *pList = &pRes->m_FontRefLinkList;
            ObjNode<nw4r::lyt::FontRefLink> *pNode;
            for (pNode = static_cast<ObjNode<nw4r::lyt::FontRefLink> *>(nw4r::ut::List_GetFirst(pList)); pNode != 0;
                 pNode = static_cast<ObjNode<nw4r::lyt::FontRefLink> *>(nw4r::ut::List_GetNext(pList, pNode))) {
                if (pNode->m_pObj == pLink) {
                    delete pLink;
                    nw4r::ut::List_Remove(pList, pNode);
                    delete pNode;
                    return;
                }
            }
        }

        inline CLytResource::DataNode *CLytResource::FindDataNode(const char *pName)
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (strcmp(pName, pNode->m_szName) == 0) {
                    return pNode;
                }
            }
            return 0;
        }

        inline void CLytResource::DeleteDataNode(DataNode *pNode)
        {
            if (pNode != 0) {
                if (!(pNode->m_nFlags & 1)) {
                    if (pNode->m_pData != 0) {
                        delete[] static_cast<unsigned char *>(pNode->m_pData);
                    }
                    pNode->m_pData = 0;
                }
                nw4r::ut::List_Remove(&m_DataList, pNode);
                delete pNode;
            }
        }

        void CLytResource::Cleanup(const char *pName)
        {
            DeleteDataNode(FindDataNode(pName));
        }

        void CLytResource::CleanupAll()
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList))) {
                DeleteDataNode(pNode);
            }
        }

        long CLytResource::LoadDataASync(const CLoadMan::FILE_INPUT *pFile)
        {
            if (pFile == 0) {
                return -1;
            }
            return CLoadMan::Load(pFile) == 0 ? -2 : 0;
        }

        long CLytResource::IsCompleteDataASync(const CLoadMan::FILE_INPUT *pFile)
        {
            if (CLoadMan::IsLoadEnd(pFile) == 0) {
                return -1;
            }
            return GetResourceFromDvdMan(pFile->m_szPath, pFile->m_nId);
        }
    }
}
