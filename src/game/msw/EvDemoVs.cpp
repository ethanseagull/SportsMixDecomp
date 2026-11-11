#include "game/msw/EvDemoVs.hpp"

namespace msw {
namespace EvDemoVs {

void Initialize()
{
    s_nCopyResMode = 0;
    for (int i = 0; i < RES_DATA_NUM; i++) {
        RES_DATA &rRes = s_ResData[i];
        rRes.m_nChrPos = -1;
        rRes.m_bResFile = 0;
        rRes.m_nFileID = -1;
        rRes.m_pszPath = 0;
        rRes.m_pData = 0;
        rRes.m_nSize = 0;
    }
}

void Finalize()
{
    int i;
    RES_DATA *pRes = s_ResData;
    for (i = 0; i < RES_DATA_NUM; i++, pRes++) {
        if (pRes->m_pData != 0) {
            delete[] pRes->m_pData;
        }
        pRes->m_pData = 0;
        pRes->m_nSize = 0;
    }
    Initialize();
}

void CopyResMode(long nMode)
{
    s_nCopyResMode = nMode;
}

void SetChrData(long nChr, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir)
{
    RES_DATA *pRes = &s_ResData[nChr + RES_DATA_CHR];
    if (pRes->m_nChrPos == -1) {
        int nNext = 0;
        for (int i = 0; i < RES_DATA_NUM; i++) {
            if (s_ResData[i].m_nChrPos != -1) {
                nNext = s_ResData[i].m_nChrPos + 2;
            }
        }
        pRes->m_nChrPos = nNext;
    }
    s_ChrPos[pRes->m_nChrPos] = rPos;
    s_ChrPos[pRes->m_nChrPos + 1] = rDir;
}

}
}
