#include "game/msw/CEffectTable.hpp"
#include "bcs/New.hpp"
#include "FastCast.hpp"

// The original file is probably one with CEffectTableList.cpp and
// CEffectAttachTable.cpp: the three vtables sit in .data in the reverse
// order of the code (AttachTable, Table, TableList). Parse follows the
// getters directly (0x801247E0), outside this unit's slice.

namespace msw {

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
}
