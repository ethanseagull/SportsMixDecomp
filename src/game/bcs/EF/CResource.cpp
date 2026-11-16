#include "bcs/EF/CResource.hpp"

#include "bcs/CArcFileAccessor.hpp"
#include "bcs/New.hpp"
#include "nw4r/ut/DvdFileStream.hpp"
#include "revolution/cx.h"
#include "revolution/gx/GXAttr.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/os/OSCache.h"
#include "string.h"

namespace bcs {
    namespace EF {

        CResource *CResource::s_pInstance;

        CResource::CResource()
        {
            nw4r::ut::List_Init(&m_DataList, 0);
            nw4r::ut::List_Init(&m_MdlList, 0);
            nw4r::ut::List_Init(&m_ScnList, 0);
            m_nEffectNum = 0;
            m_nTextureNum = 0;
            m_nMdlNum = 0;
            m_nArcNum = 0;
        }

        CResource::~CResource()
        {
            Clear();
        }

        CResource *CResource::CreateInstance()
        {
            if (s_pInstance == 0) {
                s_pInstance = new CResource;
            }
            return s_pInstance;
        }

        CResource *CResource::GetInstance()
        {
            return s_pInstance;
        }

        void CResource::DestroyInstance()
        {
            if (s_pInstance != 0) {
                delete s_pInstance;
            }
            s_pInstance = 0;
        }

        void CResource::Clear()
        {
            ScnNode *pScn;
            for (pScn = static_cast<ScnNode *>(nw4r::ut::List_GetLast(&m_ScnList)); pScn != 0;
                 pScn = static_cast<ScnNode *>(nw4r::ut::List_GetLast(&m_ScnList))) {
                nw4r::ut::List_Remove(&m_ScnList, pScn);
                pScn->m_AnmScn.Release();
                delete pScn;
            }

            MdlNode *pMdl;
            for (pMdl = static_cast<MdlNode *>(nw4r::ut::List_GetLast(&m_MdlList)); pMdl != 0;
                 pMdl = static_cast<MdlNode *>(nw4r::ut::List_GetLast(&m_MdlList))) {
                nw4r::ut::List_Remove(&m_MdlList, pMdl);
                delete[] pMdl->m_pMdlName;
                delete pMdl;
            }

            DataNode *pData;
            for (pData = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList)); pData != 0;
                 pData = static_cast<DataNode *>(nw4r::ut::List_GetLast(&m_DataList))) {
                nw4r::ut::List_Remove(&m_DataList, pData);
                delete pData;
            }

            nw4r::ef::Resource *pRes = nw4r::ef::Resource::GetInstance();
            pRes->RemoveEffectProjectAll();
            pRes->RemoveTextureProjectAll();
        }

        long CResource::AddEffect(const char *pPath)
        {
            long nIndex = Load(pPath);
            if (nIndex >= 0) {
                nw4r::ef::Resource *pRes = nw4r::ef::Resource::GetInstance();
                DataNode *pNode = static_cast<DataNode *>(nw4r::ut::List_GetNth(&m_DataList, nIndex));
                if (pNode != 0) {
                    nw4r::ef::EffectProject *pProject = pRes->Add(pNode->m_pData);
                    if (pProject != 0) {
                        pNode->m_nType = TYPE_EFFECT;
                        pNode->m_pRes = pProject;
                        m_nEffectNum = pRes->GetNumEffectProject();
                    }
                }
            }
            return nIndex;
        }

        long CResource::AddTexture(const char *pPath)
        {
            long nIndex = Load(pPath);
            if (nIndex >= 0) {
                nw4r::ef::Resource *pRes = nw4r::ef::Resource::GetInstance();
                DataNode *pNode = static_cast<DataNode *>(nw4r::ut::List_GetNth(&m_DataList, nIndex));
                if (pNode != 0) {
                    pNode->m_nType = TYPE_TEXTURE;
                    pNode->m_pRes = pRes->AddTexture(pNode->m_pData);
                    DCFlushRange(pNode->m_pData, pNode->m_nSize);
                    m_nTextureNum = pRes->GetNumTextureProject();
                }
            }
            return nIndex;
        }

        long CResource::AddG3DResFile(const char *pPath)
        {
            long nIndex = Load(pPath);
            if (nIndex >= 0) {
                DataNode *pNode = static_cast<DataNode *>(nw4r::ut::List_GetNth(&m_DataList, nIndex));
                if (pNode == 0) {
                    return -1;
                }
                pNode->m_nType = TYPE_G3D;

                unsigned char *pData = pNode->m_pData;
                nw4r::g3d::ResFile file = nw4r::g3d::ResFile::FromAligned(pData);
                if (file.IsValid()) {
                    file.Init();
                    file.Bind(file);
                    if (file.HasResMdl()) {
                        unsigned long nNum = file.GetResMdlNumEntries();
                        for (unsigned long i = 0; i < nNum; i++) {
                            MdlNode *pMdl = new MdlNode;
                            if (pMdl == 0) {
                                continue;
                            }
                            const char *pMdlName = file.GetResMdl(i).GetName();
                            if (pMdlName == 0) {
                                continue;
                            }

                            unsigned long nMdlLen = strlen(pMdlName) + 1;
                            unsigned long nPathLen = strlen(pPath) + 1;
                            char *pNames = new (0) char[nMdlLen + nPathLen];
                            strncpy(pNames, pMdlName, nMdlLen);
                            char *pFileName = pNames + nMdlLen;
                            strncpy(pFileName, pPath, nPathLen);

                            pMdl->m_pData = pData;
                            pMdl->m_Unk0C = 0;
                            pMdl->m_pMdlName = pNames;
                            pMdl->m_pFileName = pFileName;
                            nw4r::ut::List_Append(&m_MdlList, pMdl);
                            m_nMdlNum++;
                        }
                    }
                }
            }
            return nIndex;
        }

        long CResource::AddArchive(const char *pPath)
        {
            long nIndex = Load(pPath);
            if (nIndex >= 0) {
                DataNode *pNode = static_cast<DataNode *>(nw4r::ut::List_GetNth(&m_DataList, nIndex));
                if (pNode != 0) {
                    pNode->m_nType = TYPE_ARC;
                    pNode->m_pRes = 0;
                    m_nArcNum++;
                }
            }
            return nIndex;
        }

        int CResource::GetDataNum()
        {
            return nw4r::ut::List_GetSize(&m_DataList);
        }

        const char *CResource::GetDataName(long nIndex, long nType)
        {
            if (nIndex < 0) {
                return 0;
            }
            DataNode *pNode = static_cast<DataNode *>(nw4r::ut::List_GetNth(&m_DataList, nIndex));
            if (pNode == 0) {
                return 0;
            }
            if (nType != TYPE_ANY && nType != pNode->m_nType) {
                return 0;
            }
            return pNode->m_szName;
        }

        void CResource::RelocateCommand()
        {
            nw4r::ef::Resource::GetInstance()->RelocateCommand();
        }

        void CResource::InvalidateCache()
        {
            GXInvalidateVtxCache();
            GXInvalidateTexAll();
        }

        inline CResource::DataNode *CResource::FindEffectNode(const char *pName)
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (pNode->m_pData != 0 && pNode->m_nType == TYPE_EFFECT && pNode->m_pRes != 0 &&
                    strcmp(pName, static_cast<nw4r::ef::EffectProject *>(pNode->m_pRes)->projectname) == 0) {
                    return pNode;
                }
            }
            return 0;
        }

        nw4r::ef::EffectProject *CResource::FindEffectProject(const char *pName)
        {
            DataNode *pNode = FindEffectNode(pName);
            if (pNode == 0 || pNode->m_nType != TYPE_EFFECT) {
                return 0;
            }
            return static_cast<nw4r::ef::EffectProject *>(pNode->m_pRes);
        }

        nw4r::g3d::ResFile CResource::GetG3DResFile(const char *pName)
        {
            MdlNode *pNode;
            for (pNode = static_cast<MdlNode *>(nw4r::ut::List_GetFirst(&m_MdlList)); pNode != 0;
                 pNode = static_cast<MdlNode *>(nw4r::ut::List_GetNext(&m_MdlList, pNode))) {
                if (strcmp(pNode->m_pFileName, pName) == 0) {
                    return nw4r::g3d::ResFile::FromAligned(pNode->m_pData);
                }
            }
            return nw4r::g3d::ResFile(0);
        }

        nw4r::g3d::ResFile CResource::GetG3DResFileByMdlName(const char *pName)
        {
            MdlNode *pNode;
            for (pNode = static_cast<MdlNode *>(nw4r::ut::List_GetFirst(&m_MdlList)); pNode != 0;
                 pNode = static_cast<MdlNode *>(nw4r::ut::List_GetNext(&m_MdlList, pNode))) {
                if (strcmp(pNode->m_pMdlName, pName) == 0) {
                    return nw4r::g3d::ResFile::FromAligned(pNode->m_pData);
                }
            }
            return nw4r::g3d::ResFile(0);
        }

        inline CResource::DataNode *CResource::FindDataNode(const char *pPath)
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (strcmp(pPath, pNode->m_szName) == 0) {
                    return pNode;
                }
            }
            return 0;
        }

        CArcFileAccessor *CResource::GetArcAccesor(const char *pPath, const char *pRoot)
        {
            DataNode *pNode = FindDataNode(pPath);
            if (pNode == 0 || pNode->m_nType != TYPE_ARC) {
                return 0;
            }
            CArcFileAccessor *pAccessor = new CArcFileAccessor;
            if (pAccessor != 0) {
                pAccessor->Attach(pNode->m_pData, pRoot);
            }
            return pAccessor;
        }

        void CResource::ReleaseArcAccesor(CArcFileAccessor *pAccessor)
        {
            if (pAccessor != 0) {
                pAccessor->Detach();
                delete pAccessor;
            }
        }

        // x rounded up to a multiple of base (a power of two).
        static inline unsigned long RoundUp(unsigned long x, unsigned int base)
        {
            return (x + (base - 1)) & ~(base - 1);
        }

        // The whole of pStream in a 32-byte aligned buffer (null if it is
        // empty or cannot be read); *pSize receives its size.
        static inline unsigned char *ReadAll(nw4r::ut::DvdFileStream *pStream, unsigned long *pSize)
        {
            unsigned long nLen = pStream->GetSize();
            if (nLen == 0) {
                return 0;
            }
            unsigned long nReadSize = RoundUp(nLen, pStream->GetSizeAlign());
            unsigned char *pData = new (1, 32) unsigned char[nReadSize];
            if (pData == 0) {
                return 0;
            }
            if (pStream->Read(pData, nReadSize) != nReadSize) {
                delete[] pData;
                return 0;
            }
            *pSize = nLen;
            return pData;
        }

        long CResource::Load(const char *pPath)
        {
            if (pPath == 0 || strlen(pPath) == 0) {
                return -1;
            }

            long nIndex = 0;
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (strcmp(pPath, pNode->m_szName) == 0) {
                    return nIndex;
                }
                nIndex++;
            }

            nw4r::ut::DvdFileStream stream(pPath);
            if (!stream.IsAvailable()) {
                return -4;
            }
            stream.Seek(0, 0);

            unsigned long nSize = 0;
            unsigned char *pData = ReadAll(&stream, &nSize);
            if (pData == 0) {
                return -5;
            }

            // A .bin file is LH-compressed.
            if (strstr(pPath, ".bin") != 0) {
                DCInvalidateRange(pData, nSize);
                unsigned long nUnpacked = CXGetUncompressedSize(pData);
                unsigned char *pUnpacked = new (1, 32) unsigned char[nUnpacked + 4];
                CXUncompressLH(pData, pUnpacked, __alloca(0x880));
                DCFlushRange(pUnpacked, nUnpacked);
                delete[] pData;
                pData = pUnpacked;
            }

            pNode = new DataNode;
            pNode->m_pData = pData;
            pNode->m_nSize = nSize;
            pNode->m_pRes = 0;
            pNode->m_nType = TYPE_EFFECT;
            pNode->m_nFlag = 0;
            unsigned long nNameLen = strlen(pPath) + 1;
            strncpy(pNode->m_szName, pPath, nNameLen);
            nw4r::ut::List_Append(&m_DataList, pNode);
            return nIndex;
        }

        int CResource::GetResourceAfterLoad(const char *pPath, void *pData, unsigned long size)
        {
            DataNode *pNode;
            for (pNode = static_cast<DataNode *>(nw4r::ut::List_GetFirst(&m_DataList)); pNode != 0;
                 pNode = static_cast<DataNode *>(nw4r::ut::List_GetNext(&m_DataList, pNode))) {
                if (strcmp(pPath, pNode->m_szName) == 0) {
                    return 1;
                }
            }
            if (pData == 0 || size == 0) {
                return -1;
            }

            pNode = new DataNode;
            pNode->m_pData = static_cast<unsigned char *>(pData);
            pNode->m_nSize = size;
            pNode->m_pRes = 0;
            pNode->m_nType = TYPE_EFFECT;
            pNode->m_nFlag = 1;
            unsigned long nNameLen = strlen(pPath) + 1;
            strncpy(pNode->m_szName, pPath, nNameLen);
            nw4r::ut::List_Append(&m_DataList, pNode);
            return 0;
        }
    }
}
