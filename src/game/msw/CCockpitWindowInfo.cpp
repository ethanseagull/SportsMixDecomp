#include "game/msw/CCockpitWindowInfo.hpp"

#include <string.h>
#include <wchar.h>

#include "bcs/G2D/CTextBoxDL.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/ut/RuntimeTypeInfo.h"

// This unit is a slice of a wider file: CCockpitButtonInfo, this class,
// CCockpitPartsInfo, CCockpitMenuBase and CCockpitMenuBaseButtonA follow one
// another in the DOL (0x801ECE98 to 0x801EF868), their vtables are in the
// reverse of that order, and the static initialiser at the end of it fills
// s_aFontSize, which CCockpitMenuBase's constructor reads too.

namespace msw {

    CCockpitWindowInfo::WindowInfo::WindowInfo() : m_FontSize(0.0f, 0.0f)
    {
    }

    CCockpitWindowInfo::~CCockpitWindowInfo()
    {
        if (m_pInfo != 0) {
            delete[] m_pInfo;
        }
        m_pInfo = 0;
    }

    void CCockpitWindowInfo::SetWindowText(long nWindow, const wchar_t *pText)
    {
        if (nWindow < m_nNum && pText != 0) {
            SetWindowText(nWindow, pText, wcslen(pText));
        }
    }

    void CCockpitWindowInfo::SetWindowText(long nWindow, const wchar_t *pText, unsigned long nLen)
    {
        if (nWindow < m_nNum) {
            wchar_t *pDst = m_pInfo[nWindow].m_aText;
            unsigned long nCopy = nLen;
            if (nLen >= 0x400) {
                nCopy = 0x3FF;
            }
            memcpy(pDst, pText, nCopy * sizeof(wchar_t));
            pDst[nCopy] = 0;
            m_pInfo[nWindow].m_nTextLen = nCopy + 1;
        }
    }

    void CCockpitWindowInfo::SetWindowFontSize(long nWindow, long nSize)
    {
        if (nWindow < m_nNum) {
            nw4r::lyt::Size size = s_aFontSize[nSize];
            m_pInfo[nWindow].m_FontSize = size;
        }
    }

    void CCockpitWindowInfo::InfoParameterInitialize(CCockpitLayoutBase *pOwner, const short *pPaneId,
                                                     CMenuPaneAccessor *pPaneAccessor, int nArg)
    {
        if (pPaneId == 0 || pPaneAccessor == 0) {
            return;
        }

        long nNum = m_nNum;
        for (long i = 0; i < nNum; i++) {
            if (pPaneId[i] >= 0) {
                nw4r::lyt::Pane *pPane = pPaneAccessor->GetPane(pPaneId[i]);
                if (pPane != 0) {
                    bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);
                    if (pTextBox != 0) {
                        unsigned short nLen = m_pInfo[i].m_nTextLen;
                        pTextBox->AllocStringBuffer(nLen);
                        pTextBox->SetString(m_pInfo[i].m_aText, 0, nLen);
                    }
                }
            }
        }
    }

    void CCockpitWindowInfo::SetWindowTextPaneId(const short *pPaneId)
    {
        long nNum = m_nNum;
        for (long i = 0; i < nNum; i++) {
            m_pInfo[i].m_nPaneId = pPaneId[i];
        }
    }

    // In the original this is filled by the static initialiser at the end
    // of CCockpitMenuBase's file.
    nw4r::lyt::Size CCockpitWindowInfo::s_aFontSize[3] = {
        nw4r::lyt::Size(34.0f, 36.0f),
        nw4r::lyt::Size(26.0f, 30.0f),
        nw4r::lyt::Size(40.0f, 43.0f),
    };
}
