#include "game/msw/menu/CMessage.hpp"

#include <string.h>

#include "bcs/CArcFileAccessor.hpp"
#include "bcs/New.hpp"
#include "nw4r/ut/List.hpp"

// The message library of the DOL (MSBT files). C functions without exported
// names; the comments give what each does, judged from this file.
extern "C" {
    // Sets the allocator the library uses.
    void fn_8035EDF0(void *(*pAlloc)(unsigned long), void (*pFree)(void *));
    // Opens the MSBT file pData; returns its handle.
    void *fn_8035EF50(const void *pData);
    // Closes a handle.
    void fn_8035EFD0(void *pMsg);
    // The index of the text labelled pLabel, negative if none.
    int fn_8035F050(void *pMsg, const char *pLabel);
    // Text nIndex.
    const wchar_t *fn_8035F150(void *pMsg, int nIndex);
    // Size in bytes of text nIndex.
    int fn_8035F1A0(void *pMsg, int nIndex);
    // The number of texts.
    int fn_8035F020(void *pMsg);
    // Writes the label of text nIndex to pLabel; zero if there is none.
    int fn_8035F310(void *pMsg, int nIndex, char *pLabel);

    // Writes the file name of pPath without its extension to pName;
    // returns its length.
    int fn_8019B780(const char *pPath, char *pName);
}

namespace {
    // The archive's root directory, and the extension of the message files.
    const char *s_ArcRoot = "arc";
    const char *s_Ext = ".msbt";
    // The extension without its dot, as CArcFileAccessor::GetFileNames
    // wants it.
    const char *s_ExtName = s_Ext + 1;
}

// Defined here, between the statics above and the buffers below: that is its
// place in the original's .bss and static initialiser.
msw::menu::CMessage msw::menu::CMessage::s_Instance;

namespace {
    // AttachResource writes each file's name here before copying it.
    char s_FileName[0x80];
    // GetLabelByIndex returns its label in this buffer.
    char s_Label[0x100];

    // The message library's allocator.
    void *Alloc(unsigned long size)
    {
        return new (1) unsigned char[size];
    }

    void Free(void *p)
    {
        delete[] static_cast<unsigned char *>(p);
    }
}

namespace msw {
    namespace menu {

        namespace {

            // One loaded message file.
            struct Entry {
                Entry(const void *pData) : m_pMsg(fn_8035EF50(pData))
                {
                    // Cleared eight bytes per step: the original's loop
                    // stores eight bytes per index increment.
                    for (int i = 0; i < sizeof(m_Name); i += 8) {
                        for (int j = 0; j < 8; j++) {
                            m_Name[i + j] = 0;
                        }
                    }
                }

                ~Entry()
                {
                    if (m_pMsg != 0) {
                        fn_8035EFD0(m_pMsg);
                        m_pMsg = 0;
                    }
                }

                nw4r::ut::Link m_Link; // 0x00
                void *m_pMsg;          // 0x08
                char m_Name[0x80];     // 0x0C
            };
        }

        // Size in bytes of the text labelled pLabel in file nFile, 0 if
        // there is none. Inlined into GetTextLengthByLabel; no
        // out-of-line copy in this file.
        inline short CMessage::GetTextSizeByLabel(short nFile, const char *pLabel)
        {
            if (nFile < 0 || pLabel == 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                short nIndex = fn_8035F050(pEntry->m_pMsg, pLabel);
                if (nIndex >= 0) {
                    return fn_8035F1A0(pEntry->m_pMsg, nIndex);
                }
            }
            return 0;
        }

        CMessage::~CMessage() {}

        void CMessage::Initialize()
        {
            nw4r::ut::List_Init(&m_FileList, 0);
            fn_8035EDF0(Alloc, Free);
        }

        void CMessage::Finalize()
        {
            Entry *pEntry;

            while ((pEntry = static_cast<Entry *>(m_FileList.pTail)) != 0) {
                nw4r::ut::List_Remove(&m_FileList, pEntry);
                delete pEntry;
            }
        }

        long CMessage::AttachResource(const void *pData)
        {
            const char *names[0x200];

            if (pData == 0) {
                return -1;
            }

            bcs::CArcFileAccessor *pAccessor = new bcs::CArcFileAccessor();
            if (pAccessor == 0) {
                return -2;
            }

            pAccessor->Attach(pData, s_ArcRoot);
            long nFiles = pAccessor->GetFileNames(s_ExtName, names, 0x200);

            for (long i = 0; i < nFiles; i++) {
                const void *pFile = pAccessor->GetResource(0, names[i], 0);
                if (pFile == 0) {
                    continue;
                }

                Entry *pEntry = new Entry(pFile);
                if (pEntry == 0) {
                    continue;
                }

                if (fn_8019B780(names[i], s_FileName) > 0) {
                    unsigned long size = strlen(s_FileName) + 1;
                    if (size != 0 && size < sizeof(pEntry->m_Name)) {
                        strncpy(pEntry->m_Name, s_FileName, size);
                    }
                }
                nw4r::ut::List_Append(&m_FileList, pEntry);
            }

            pAccessor->Detach();
            delete pAccessor;
            return 0;
        }

        const wchar_t *CMessage::GetTextByLabel(short nFile, const char *pLabel)
        {
            if (nFile < 0 || pLabel == 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                short nIndex = fn_8035F050(pEntry->m_pMsg, pLabel);
                if (nIndex >= 0) {
                    return fn_8035F150(pEntry->m_pMsg, nIndex);
                }
            }
            return 0;
        }

        const wchar_t *CMessage::GetText(short nFile, short nIndex)
        {
            if (nFile < 0 || nIndex < 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                return fn_8035F150(pEntry->m_pMsg, nIndex);
            }
            return 0;
        }

        short CMessage::GetTextSize(short nFile, short nIndex)
        {
            if (nFile < 0 || nIndex < 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                return fn_8035F1A0(pEntry->m_pMsg, nIndex);
            }
            return 0;
        }

        short CMessage::GetTextLengthByLabel(short nFile, const char *pLabel)
        {
            return GetTextSizeByLabel(nFile, pLabel) / sizeof(wchar_t);
        }

        short CMessage::GetTextLength(short nFile, short nIndex)
        {
            return GetTextSize(nFile, nIndex) / sizeof(wchar_t);
        }

        short CMessage::GetFileIndex(const char *pName)
        {
            if (pName == 0) {
                return -1;
            }

            short nIndex = 0;
            for (Entry *pEntry = static_cast<Entry *>(m_FileList.pHead); pEntry != 0; nIndex++) {
                if (strcmp(pName, pEntry->m_Name) == 0) {
                    return nIndex;
                }
                pEntry = static_cast<Entry *>(
                    reinterpret_cast<nw4r::ut::Link *>(reinterpret_cast<char *>(pEntry) + m_FileList.offset)->pNext);
            }
            return -1;
        }

        short CMessage::GetLabelIndex(short nFile, const char *pLabel)
        {
            if (nFile < 0 || pLabel == 0) {
                return -1;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                return fn_8035F050(pEntry->m_pMsg, pLabel);
            }
            return -1;
        }

        short CMessage::GetTextNum(short nFile)
        {
            if (nFile < 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                return fn_8035F020(pEntry->m_pMsg);
            }
            return -1;
        }

        const char *CMessage::GetLabelByIndex(short nFile, short nIndex)
        {
            if (nFile < 0 || nIndex < 0) {
                return 0;
            }

            Entry *pEntry = static_cast<Entry *>(nw4r::ut::List_GetNth(&m_FileList, nFile));
            if (pEntry != 0) {
                if (static_cast<short>(fn_8035F310(pEntry->m_pMsg, nIndex, s_Label)) != 0) {
                    return s_Label;
                }
            }
            return 0;
        }
    }
}
