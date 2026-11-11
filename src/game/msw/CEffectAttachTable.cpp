#include "game/msw/CEffectTable.hpp"
#include "bcs/New.hpp"
#include "FastCast.hpp"
#include <stdio.h>

namespace msw {

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
