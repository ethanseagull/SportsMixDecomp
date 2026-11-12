#include "game/msw/CGameVib.hpp"
#include "bcs/CPadWI.hpp"
#include "bcs/CWifiPadBase.hpp"

namespace msw {

    bcs::CLoadMan::FILE_INPUT CGameVib::g_sFileInput[5] = {
        { "vib/wi_sys.vib", 0, 0, 4, 1, -32, 0 },
        { "vib/wi_ba.vib", 0, 0, 4, 1, -32, 0 },
        { "vib/wi_vo.vib", 0, 0, 4, 1, -32, 0 },
        { "vib/wi_do.vib", 0, 0, 4, 1, -32, 0 },
        { "vib/wi_ho.vib", 0, 0, 4, 1, -32, 0 },
    };

    CGameVib *CGameVib::m_pInstance;

    void CGameVib::Create()
    {
        if (m_pInstance == 0) {
            m_pInstance = new CGameVib();
        }
    }

    int CGameVib::IsLoaded()
    {
        CGameVib *p = m_pInstance;
        if (p == 0) {
            return 0;
        }
        return p->m_bLoaded;
    }

    int CGameVib::Start(long nTable, long nPattern, long nFrame, long nPad)
    {
        if (!IsLoaded()) {
            return 0;
        }

        PATTERN *pPattern = m_pInstance->m_pFile[nTable]->m_Pattern;
        float data[8][2];
        for (long i = 0; i < pPattern[nPattern].m_nNum; i++) {
            data[i][0] = pPattern[nPattern].m_Data[i][0];
            data[i][1] = pPattern[nPattern].m_Data[i][1];
        }
        long nTime = nFrame;
        if (nFrame == -2) {
            nTime = pPattern[nPattern].m_nFrame;
        }

        long nLocalPad = bcs::CWifiPadBase::GetMyLocalPadID(nPad);
        if (nLocalPad >= 0) {
            return bcs::CPadWI::StartMotor(&data[0][0], pPattern[nPattern].m_nNum, nTime, nLocalPad);
        }
        return 0;
    }

    int CGameVib::Start(float fFrame, long nPad)
    {
        long nLocalPad = bcs::CWifiPadBase::GetMyLocalPadID(nPad);
        if (nLocalPad >= 0) {
            return bcs::CPadWI::StartMotor(fFrame, nLocalPad);
        }
        return 0;
    }

    void CGameVib::Stop(long nPad)
    {
        long nLocalPad = bcs::CWifiPadBase::GetMyLocalPadID(nPad);
        if (nLocalPad >= 0) {
            bcs::CPadWI::StopMotor(nLocalPad);
        }
    }

    CGameVib::~CGameVib()
    {
        bcs::CLoadMan::UnLoad(g_sFileInput, 5);
    }

    void CGameVib::Main()
    {
        switch (m_nStep) {
            case 0:
                bcs::CLoadMan::LoadAsync(g_sFileInput, 5);
                m_nStep++;
                break;
            case 1:
                if (bcs::CLoadMan::IsLoadEnd(g_sFileInput, 5)) {
                    for (long i = 0; i < 5; i++) {
                        m_pFile[i] = static_cast<FILE *>(bcs::CLoadMan::GetPtr(&g_sFileInput[i]));
                    }
                    m_bLoaded = 1;
                    m_nStep++;
                }
                break;
        }
    }
}
