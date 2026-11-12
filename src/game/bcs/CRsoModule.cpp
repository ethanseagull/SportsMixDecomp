#include "bcs/CRsoModule.hpp"
#include "bcs/New.hpp"
#include <stdio.h>

// bcs::CRsoModule: loading, linking and unlinking the game's RSO modules,
// and the static module they all link against.

namespace bcs {

    CLoadMan::FILE_INPUT CRsoModule::s_StaticFile = { "Module/Final/MSW.sel", 0, 0, 4, 0, -32, 2 };
    void *CRsoModule::s_pStaticRso;

    void CRsoModule::LoadStaticAsync()
    {
        CLoadMan::Load(&s_StaticFile);
    }

    int CRsoModule::MountStatic()
    {
        if (s_pStaticRso) {
            return 1;
        }
        s_pStaticRso = CLoadMan::GetPtr(&s_StaticFile);
        if (s_pStaticRso == 0) {
            return 0;
        }
        RSOListInit(s_pStaticRso);
        return 1;
    }

    // What an import points at while its module is not mounted.
    static void UnresolvedFunc() {}

    inline void CRsoModule::ResetImports()
    {
        for (long i = 0; i < m_nImport; i++) {
            *m_pImport[i].m_ppFunc = UnresolvedFunc;
        }
    }

    CRsoModule::CRsoModule(const char *pName, const char *pFileName, IMPORT *pImport, long nImport,
                           long nUnk130)
        : m_pszName(pName)
        , m_pImport(pImport)
        , m_nImport(nImport)
        , m_pRso(0)
        , m_pBss(0)
        , m_nSize(0)
        , m_nUnk130(nUnk130)
    {
        sprintf(m_File.m_szPath, "Module/Final/%sF.cmp", pFileName);
        m_File.m_nFlags = 6;
        m_File.m_nId = 0;
        m_File.m_Unk108 = 0;
        m_File.m_nAlign = -32;
        m_File.m_nUnk10C = 2;
        ResetImports();
    }

    CRsoModule::~CRsoModule()
    {
        UnMount();
    }

    void CRsoModule::LoadAsync()
    {
        CLoadMan::Load(&m_File);
    }

    // Links the loaded module in (with a fresh .bss), resolves the imports
    // and runs its prolog.
    int CRsoModule::Mount()
    {
        if (m_bMounted) {
            return 1;
        }
        RSOObjectHeader *pRso = static_cast<RSOObjectHeader *>(CLoadMan::GetPtr(&m_File));
        if (pRso == 0) {
            return 0;
        }
        unsigned long bssSize = (pRso->bssSize + 31) & ~31;
        m_nSize = CLoadMan::GetSize(&m_File);
        m_pRso = pRso;
        m_pBss = 0;
        if (bssSize != 0) {
            m_pBss = new (0) unsigned char[bssSize];
        }
        RSOLinkList(m_pRso, m_pBss);
        for (long i = 0; i < m_nImport; i++) {
            *m_pImport[i].m_ppFunc =
                reinterpret_cast<void (*)()>(RSOFindExportSymbolAddr(m_pRso, m_pImport[i].m_pszName));
        }
        m_pRso->prolog();
        m_bMounted = 1;
        return 1;
    }

    // Runs the module's epilog, unlinks it and frees its file and .bss.
    void CRsoModule::UnMount()
    {
        if (m_bMounted) {
            m_pRso->epilog();
            ResetImports();
            RSOUnLinkList(m_pRso);
            CLoadMan::UnLoad(&m_File);
            if (m_pBss) {
                delete[] m_pBss;
            }
            m_pBss = 0;
            m_pRso = 0;
            m_bMounted = 0;
        }
    }
}
