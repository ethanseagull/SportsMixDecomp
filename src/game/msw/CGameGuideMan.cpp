#include "game/msw/CGameGuideMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"

// (DOL, unnamed; its unit is not written yet) The text of line message
// label nLabel, in the pad (bPad 0) or the nunchaku style (bPad 1); null when
// there is none.
extern "C" const wchar_t *fn_80217A28(long nLabel, int bPad);

namespace msw {

    // The message label of each line message type.
    static const long sLineMsgLabel[4] = {0, 1, 2, 3};

    inline void CGameGuideTelop::Remove()
    {
        if (m_nType >= 0 && m_nTelopId >= 0) {
            if (m_bGameTelop) {
                CGame::TelopRemove(m_nTelopId);
            } else {
                CCockpit::CloseGuideTelop(m_nTelopId, 0);
            }
            m_nTelopId = -1;
        }
        m_nType = -1;
        m_bEnd = 0;
        m_fTime = 0.0f;
        if (m_nTaskId != 0) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nTaskId);
            if (pTask != 0 && !pTask->IsDelete()) {
                pTask->Delete();
            }
            m_nTaskId = 0;
        }
    }

    int CGameGuideMan::IsActive()
    {
        return s_pInstance != 0;
    }

    CGameGuideMan *CGameGuideMan::Create(bcs::CTask *pParent)
    {
        CGame::GameWork &rWork = CGame::g_GameWork;
        if (rWork.m_nGameType == 0 && (rWork.m_nUnk40 & 0x200) && rWork.m_nUnk180 == 0 && rWork.m_Unk151 == 0) {
            s_pInstance = new (0x100) CGameGuideMan(pParent);
        }
        return s_pInstance;
    }

    void CGameGuideMan::Reset()
    {
        if (s_pInstance != 0) {
            s_pInstance->Reset_();
        }
    }

    void CGameGuideMan::Reset_()
    {
        if (m_pTelop != 0) {
            m_pTelop->Remove();
        }
        EndAllPlayerGuide_(1);
        EndLineMsg_(0);
        if (m_pChildRoot != 0) {
            m_pChildRoot->DeleteAllChild();
        }
    }

    int CGameGuideMan::SendMessage(long nMsg, long nArg1, long nArg2, long nArg3)
    {
        if (s_pInstance != 0) {
            return s_pInstance->SendMessage_(nMsg, nArg1, nArg2, nArg3);
        }
        return 0;
    }

    int CGameGuideMan::SendMessage_(long nMsg, long, long, long)
    {
        switch (nMsg) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 11:
            case 13:
            case 15:
            case 18:
            case 19:
                Reset_();
                break;
            case 24:
            case 54:
            case 64:
            case 85:
            case 97:
                Reset_();
                break;
        }
        return 0;
    }

    long CGameGuideMan::GetGuideCount_(long nType, long nPlayer)
    {
        return m_GuideCount[nPlayer][nType];
    }

    void CGameGuideMan::AddGuideCount_(long nType, long nPlayer)
    {
        m_GuideCount[nPlayer][nType]++;
    }

    long CGameGuideMan::GetGuideSuccessCount(long nType, long nPlayer)
    {
        if (s_pInstance != 0) {
            return s_pInstance->GetGuideSuccessCount_(nType, nPlayer);
        }
        return 0;
    }

    long CGameGuideMan::GetGuideSuccessCount_(long nType, long nPlayer)
    {
        return m_GuideSuccessCount[nPlayer][nType];
    }

    long CGameGuideMan::AddGuideSuccessCount(long nType, long nPlayer)
    {
        if (s_pInstance != 0) {
            return s_pInstance->AddGuideSuccessCount_(nType, nPlayer);
        }
        return 0;
    }

    long CGameGuideMan::AddGuideSuccessCount_(long nType, long nPlayer)
    {
        return ++m_GuideSuccessCount[nPlayer][nType];
    }

    void CGameGuideMan::ClearGuideCount_(long nType, long nPlayer)
    {
        if (nPlayer < 0) {
            for (int i = 0; i < 4; i++) {
                m_GuideCount[i][nType] = 0;
            }
        } else {
            m_GuideCount[nPlayer][nType] = 0;
        }
    }

    void CGameGuideMan::ClearGuideSuccessCount_(long nType, long nPlayer)
    {
        if (nPlayer < 0) {
            for (int i = 0; i < 4; i++) {
                m_GuideSuccessCount[i][nType] = 0;
            }
        } else {
            m_GuideSuccessCount[nPlayer][nType] = 0;
        }
    }

    int CGameGuideMan::IsGuideDisp(long nType, long nPlayer)
    {
        if (s_pInstance != 0) {
            return s_pInstance->IsGuideDisp_(nType, nPlayer);
        }
        return 0;
    }

    int CGameGuideMan::IsGuideDisp_(long nType, long nPlayer)
    {
        CGameGuideTelop *pTelop = m_pTelop;
        if (pTelop != 0) {
            if (nType >= 0 && pTelop->m_nType == nType && pTelop->m_nPlayer == nPlayer) {
                return 1;
            }
            return 0;
        }
        return 0;
    }

    int CGameGuideMan::IsDispEnable()
    {
        CGame::GameWork &rWork = CGame::g_GameWork;
        if (!(rWork.m_nUnk40 & 0x200)) {
            return 0;
        }
        if (rWork.m_nUnk180 != 0) {
            return 0;
        }
        if (rWork.m_Unk151 != 0) {
            return 0;
        }
        return CGame::IsLineMsgDispEnable() != 0;
    }

    int CGameGuideMan::StartGuideTelop(long nType, CChrCore *pChr, float fTime, const bcs::FVEC3 *pOfs, float f0,
                                       float f1, float f2)
    {
        if (s_pInstance != 0) {
            if (IsDispEnable()) {
                return s_pInstance->StartGuideTelop_(nType, pChr, fTime, pOfs, f0, f1, f2);
            }
            return 0;
        }
        return 0;
    }

    int CGameGuideMan::StartGuideTelop_(long nType, CChrCore *pChr, float fTime, const bcs::FVEC3 *pOfs, float f0,
                                        float f1, float f2)
    {
        int bPad = pChr->DriverGetPadType() == 1;
        if (m_pTelop != 0) {
            if (m_pTelop->Start(nType, pChr->GetPlayerNo(), pChr->GetTeamNo(), pChr->GetCtrlPlayerNo(), pOfs, bPad,
                                fTime, f0, f1, f2)) {
                AddGuideCount_(nType, pChr->GetCtrlPlayerNo());
            }
            return 1;
        }
        return 0;
    }

    void CGameGuideMan::EndGuideTelop(long nPlayer)
    {
        if (s_pInstance != 0) {
            s_pInstance->EndGuideTelop_(nPlayer);
        }
    }

    void CGameGuideMan::EndGuideTelop_(long nPlayer)
    {
        CGameGuideTelop *pTelop = m_pTelop;
        if (pTelop != 0 && pTelop->m_nType >= 0) {
            if (nPlayer < 0) {
                pTelop->m_bEnd = 1;
            } else if (pTelop->m_nPlayer == nPlayer) {
                pTelop->m_bEnd = 1;
            }
        }
    }

    void CGameGuideMan::RemoveGuideTelop()
    {
        if (s_pInstance != 0) {
            s_pInstance->RemoveGuideTelop_();
        }
    }

    void CGameGuideMan::RemoveGuideTelop_()
    {
        if (m_pTelop != 0) {
            m_pTelop->Remove();
        }
    }

    int CGameGuideMan::IsGuideEnable_()
    {
        return 1;
    }

    int CGameGuideMan::IsLineMsgDisp(long nType)
    {
        if (s_pInstance != 0) {
            if (IsDispEnable()) {
                return s_pInstance->IsLineMsgDisp_(nType);
            }
            return 0;
        }
        return 0;
    }

    int CGameGuideMan::IsLineMsgDisp_(long nType)
    {
        if (!m_bLineMsg) {
            return 0;
        }
        if (nType >= 0 && nType != m_nLineMsgType) {
            return 0;
        }
        return 1;
    }

    int CGameGuideMan::StartLineMsg(long nType, CChrCore *pChr, int nArg, float fArg)
    {
        if (s_pInstance != 0) {
            if (IsDispEnable()) {
                return s_pInstance->StartLineMsg_(nType, pChr, nArg, fArg);
            }
            return 0;
        }
        return 0;
    }

    int CGameGuideMan::StartLineMsg_(long nType, CChrCore *pChr, int nArg, float)
    {
        if ((unsigned long)nType > 3) {
            EndLineMsg_(1);
            return 0;
        }
        if (!m_bLineMsg) {
            if (nArg) {
                CCockpit::SetLineMsgMode(1);
            } else {
                CCockpit::SetLineMsgMode(0);
            }
        }
        if (nType != m_nLineMsgType) {
            const long *pLabel = &sLineMsgLabel[nType];
            int bPad = 0;
            if (pChr != 0) {
                bPad = pChr->DriverGetPadType() != 1;
            }
            long nLabel = *pLabel;
            if (nLabel < 0) {
                EndLineMsg_(1);
                return 0;
            }
            const wchar_t *pText = fn_80217A28(nLabel, bPad);
            if (pText != 0) {
                CCockpit::SetLineMsgText(pText);
            }
        }
        m_bLineMsg = 1;
        m_nLineMsgType = nType;
        return 1;
    }
}
