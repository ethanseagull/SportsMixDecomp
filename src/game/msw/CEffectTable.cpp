#include "game/msw/CEffectTable.hpp"
#include "bcs/New.hpp"
#include "FastCast.hpp"
#include <stdio.h>

// One file holds CEffectTableList, CEffectTable and CEffectAttachTable: the
// code is in that order and the three vtables follow in .data in the reverse
// order (AttachTable, Table, TableList).

// (guess) The original CEffectTableList destructor stores the vtable
// pointer, which the compiler leaves out of an empty body and keeps when the
// body calls an inline function, even one that compiles to nothing. What the
// original called there is not known (see notes/decisions.md).
static inline void Nothing() {}

namespace msw {
    CEffectTableList::CEffectTableList()
    {
        m_apTable[0] = 0;
        m_apTable[1] = 0;
        m_apTable[2] = 0;
        m_apTable[3] = 0;
        m_nNum = 0;
    }

    CEffectTableList::~CEffectTableList()
    {
        Nothing();
    }

    long CEffectTableList::Add(CTableBase *pTable)
    {
        CTableBase *pPrev; // declared first: the original's register order
        if (pTable == 0) {
            return -1;
        }
        long nNum = m_nNum;
        if (nNum >= 4) {
            return -2;
        }

        unsigned short nTop;
        if (nNum == 0) {
            nTop = 0;
        } else {
            pPrev = m_apTable[nNum - 1];
            if (pPrev == 0) {
                nTop = 0;
            } else {
                nTop = pPrev->m_Unk00 + pPrev->m_Unk02;
            }
        }
        pTable->m_Unk00 = nTop;
        m_apTable[nNum] = pTable;
        m_nNum = nNum + 1;
        return 0;
    }

    CTableBase *CEffectTableList::Find(long idx) const
    {
        for (long i = 0; i < 4; i++) {
            CTableBase *pTable = m_apTable[i];
            if (pTable != 0) {
                long nTop = pTable->m_Unk00;
                long nEnd = nTop + pTable->m_Unk02;
                if (nTop <= idx && idx < nEnd) {
                    return pTable;
                }
            }
        }
        return 0;
    }

    CEffectTable::CEffectTable() {
        m_pHeader = 0;
        m_pEntry = 0;
        m_ppName = 0;
        m_nNameTop = 0;
        m_nGroup1Top = 0;
        m_nChildNameTop = 0;
        m_nGroup3Top = 0;
    }

    CEffectTable::~CEffectTable() {
        if (m_ppName != 0) {
            delete[] m_ppName;
        }
        m_ppName = 0;
    }

    const char *CEffectTable::GetName(long idx) const {
        long i = idx - m_Unk00;
        if (m_ppName == 0 || m_pEntry == 0) {
            return 0;
        }
        return m_ppName[m_nNameTop + m_pEntry[i].nName];
    }

    unsigned short CEffectTable::GetChildNum(long idx) const {
        long i = idx - m_Unk00;
        if (m_pEntry == 0) {
            return 0;
        }
        return m_pEntry[i].nChildNum;
    }

    const char *CEffectTable::GetChildName(long idx, long nChild) const {
        long i = idx - m_Unk00;
        if (m_ppName == 0 || m_pEntry == 0) {
            return 0;
        }
        return m_ppName[nChild + m_nChildNameTop + m_pEntry[i].nChildTop];
    }

    float CEffectTable::GetParam0(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[0]);
    }

    float CEffectTable::GetParam1(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[1]);
    }

    float CEffectTable::GetParam2(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[2]);
    }

    float CEffectTable::GetParam3(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[3]);
    }

    long CEffectTable::Parse(unsigned char *pData) {
        if (pData == 0) {
            return -1;
        }

        const HEADER *pHeader = reinterpret_cast<const HEADER *>(pData);
        m_pHeader = pHeader;
        m_pEntry = reinterpret_cast<const ENTRY *>(pData + sizeof(HEADER));
        m_nNameTop = 0;
        m_nGroup1Top = pHeader->nNameNum;
        m_nChildNameTop = m_nGroup1Top + pHeader->nEntryNum;
        m_nGroup3Top = m_nChildNameTop + pHeader->nChildNameNum;
        long nNum = m_nGroup3Top + pHeader->nGroup3Num;

        m_ppName = new (1) const char *[nNum];
        if (m_ppName == 0) {
            return -2;
        }

        const char *pName = reinterpret_cast<const char *>(pData + sizeof(HEADER) + m_pHeader->nEntryNum * sizeof(ENTRY));
        for (long i = 0; i < nNum; i++) {
            m_ppName[i] = pName;
            while (*pName != '\0') {
                pName++;
            }
            pName++;
        }

        m_Unk00 = 0;
        m_Unk02 = GetEntryNum();
        return 0;
    }

    CEffectAttachTable::CEffectAttachTable() {
        m_pHeader = 0;
        m_pEntry = 0;
        m_ppName = 0;
        m_nNameTop = 0;
        m_nSubNameTop = 0;
    }

    CEffectAttachTable::~CEffectAttachTable() {
        if (m_ppName != 0) {
            delete[] m_ppName;
        }
        m_ppName = 0;
    }

    const char *CEffectAttachTable::GetName(long idx) const {
        long i = idx - m_Unk00;
        if (m_ppName == 0 || m_pEntry == 0) {
            return 0;
        }
        return m_ppName[m_nNameTop + m_pEntry[i].nName];
    }

    const char *CEffectAttachTable::GetAttachName(long idx) const {
        static char sBuf[0x100];

        if (m_ppName == 0 || m_pHeader == 0) {
            return 0;
        }
        const char *pName = GetName(idx);
        if (pName == 0) {
            return 0;
        }
        const char *pSubName = m_ppName[m_nSubNameTop + (idx - m_Unk00)];
        // The original tests the name a second time here (the same cr0
        // result is branched on twice).
        if (pName == 0) {
            return 0;
        }
        sprintf(sBuf, "%s_%s", pName, pSubName);
        return sBuf;
    }

    float CEffectAttachTable::GetParam0(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[0]);
    }

    float CEffectAttachTable::GetParam1(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[1]);
    }

    float CEffectAttachTable::GetParam2(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[2]);
    }

    float CEffectAttachTable::GetParam3(long idx) const {
        return S16ToF32(&GetEntry(idx).aParam[3]);
    }

    long CEffectAttachTable::Parse(unsigned char *pData) {
        if (pData == 0) {
            return -1;
        }

        const CEffectTable::HEADER *pHeader = reinterpret_cast<const CEffectTable::HEADER *>(pData);
        m_pHeader = pHeader;
        m_pEntry = reinterpret_cast<const ENTRY *>(pData + sizeof(CEffectTable::HEADER));
        m_nNameTop = 0;
        m_nSubNameTop = pHeader->nNameNum;
        long nNum = m_nSubNameTop + pHeader->nEntryNum;

        m_ppName = new (1) const char *[nNum];
        if (m_ppName == 0) {
            return -2;
        }

        const char *pName = reinterpret_cast<const char *>(pData + sizeof(CEffectTable::HEADER) + m_pHeader->nEntryNum * sizeof(ENTRY));
        for (long i = 0; i < nNum; i++) {
            m_ppName[i] = pName;
            while (*pName != '\0') {
                pName++;
            }
            pName++;
        }

        m_Unk00 = 0;
        m_Unk02 = GetEntryNum();
        return 0;
    }
}
