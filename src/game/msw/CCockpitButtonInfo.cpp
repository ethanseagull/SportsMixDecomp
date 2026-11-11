#include "game/msw/CCockpitButtonInfo.hpp"

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CMessage.hpp"

// This unit is a slice of a wider file: CCockpitButtonInfo,
// CCockpitWindowInfo, CCockpitPartsInfo, CCockpitMenuBase and
// CCockpitMenuBaseButtonA follow one another in the DOL (0x801ECE98 to
// 0x801EF868) and their vtables are in the reverse of that order.

namespace msw {

    void CCockpitButtonInfo::Button::Init()
    {
        m_Unk00 = 0;
        m_nMenuResultId = 0;
        m_aText[0] = L'x';
        m_aText[1] = 0;
        m_abDefault[0] = 0;
        m_abDefault[1] = 0;
        m_abDefault[2] = 0;
        m_abDefault[3] = 0;
        m_aSECode[0] = 0xBBB;
        m_aSECode[1] = 0xBC0;
    }

    CCockpitButtonInfo::~CCockpitButtonInfo()
    {
        if (m_pInfo != 0) {
            delete[] m_pInfo;
        }
        m_pInfo = 0;
    }

    long CCockpitButtonInfo::GetMenuResultId(long nButton)
    {
        if (IsValidButton(nButton)) {
            return m_pInfo[nButton].m_nMenuResultId;
        }
        return -1;
    }

    void CCockpitButtonInfo::SetMenuResultId(long nButton, long nId)
    {
        if (IsValidButton(nButton)) {
            m_pInfo[nButton].m_nMenuResultId = nId;
        }
    }

    const wchar_t *CCockpitButtonInfo::GetButtonText(long nButton)
    {
        if (IsValidButton(nButton)) {
            return m_pInfo[nButton].m_aText;
        }
        return 0;
    }

    void CCockpitButtonInfo::SetButtonText(long nButton, const wchar_t *pText)
    {
        if (IsValidButton(nButton) && pText != 0) {
            SetButtonText(nButton, pText, wcslen(pText));
        }
    }

    long CCockpitButtonInfo::GetButtonSECode(long nButton, long nIdx)
    {
        if (IsValidButton(nButton)) {
            return m_pInfo[nButton].m_aSECode[nIdx];
        }
        return -1;
    }

    void CCockpitButtonInfo::SetButtonSECodeArray(long nButton, long nSE0, long nSE1)
    {
        if (IsValidButton(nButton)) {
            m_pInfo[nButton].m_aSECode[0] = nSE0;
            m_pInfo[nButton].m_aSECode[1] = nSE1;
        }
    }

    void CCockpitButtonInfo::SetDefaultCursorFlag(long nButton)
    {
        long nLeader = CMenuPadInfo::s_Instance.GetPadLeader();
        long nNum = m_nNum;
        for (long i = 0; i < nNum; i++) {
            bool bDefault = nButton == i;
            SetDefaultCursor(i, bDefault, nLeader);
        }
    }

    void CCockpitButtonInfo::SetButtonInfo(const ButtonInfo *pInfo)
    {
        long nNum = m_nNum;
        for (long i = 0; i < nNum; i++, pInfo++) {
            SetMenuResultId(i, pInfo->m_nMenuResultId);

            menu::CMessage *pMessage = &menu::CMessage::s_Instance;
            const wchar_t *pText =
                pMessage->GetTextByLabel(pMessage->GetFileIndex(pInfo->m_pFileName), pInfo->m_pLabel);
            if (pText != 0) {
                SetButtonText(i, pText);
            }

            SetDefaultCursor(i, pInfo->m_bDefault, CMenuPadInfo::s_Instance.GetPadLeader());
        }
    }

    void CCockpitButtonInfo::ButtonParameterInitialize(CCockpitLayoutBase *pOwner, const ButtonPaneInfo *pPaneInfo,
                                                     CMenuPaneAccessor *pPaneAccessor, int nArg)
    {
        if (pPaneInfo == 0 || pPaneAccessor == 0) {
            return;
        }

        long nNum = m_nNum;
        for (long i = 0; i < nNum; i++, pPaneInfo++) {
            short nPaneId = pPaneInfo->m_nTextPaneId;
            if (nPaneId >= 0) {
                const wchar_t *pText = GetButtonText(i);
                pPaneAccessor->SetTextBoxString(nPaneId, pText, wcslen(pText));
            }
            for (long nPad = 0; nPad < 4; nPad++) {
                if (IsDefaultCursor(i, nPad)) {
                    pOwner->m_nDefaultBtnKindId[nPad] = pPaneInfo->m_nBtnKindId;
                }
            }
        }
    }
}
