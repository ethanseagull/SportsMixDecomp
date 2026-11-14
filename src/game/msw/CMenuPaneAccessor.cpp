#include "game/msw/CMenuPaneAccessor.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/ut/RuntimeTypeInfo.h"
#include "wchar.h"

namespace msw {

    CMenuPaneAccessor::CMenuPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum)
    {
        m_pPaneCtrl = pPaneCtrl;
        m_ppPaneNames = ppPaneNames;
        m_nNum = nNum;
    }

    CMenuPaneAccessor::~CMenuPaneAccessor() {}

    const char *CMenuPaneAccessor::GetPaneName(long nPane)
    {
        return m_ppPaneNames[nPane];
    }

    void CMenuPaneAccessor::SetDisp(const char *pName, int bDisp)
    {
        if (pName == 0) {
            return;
        }
        nw4r::lyt::Pane *pPane = GetPane(pName, 1, 0);
        if (pPane != 0) {
            pPane->SetVisible(bDisp);
        }
    }

    long CMenuPaneAccessor::PlayAnimation(long nPane, long nAnim, float fSpeed, float fFrame, int bRecursive, int bReset)
    {
        bcs::G2D::CPaneObj *pPaneObj = GetPaneObj(nPane);
        if (pPaneObj == 0) {
            return -1;
        }
        pPaneObj->PlayAnimation(nAnim, fSpeed, fFrame, bRecursive, bReset, 1);
        return 0;
    }

    long CMenuPaneAccessor::StopAnimation(long nPane, long nAnim, int bRecursive)
    {
        bcs::G2D::CPaneObj *pPaneObj = GetPaneObj(nPane);
        if (pPaneObj == 0) {
            return -1;
        }
        pPaneObj->StopAnimation(nAnim, bRecursive);
        return 0;
    }

    int CMenuPaneAccessor::IsAnimEnd(long nPane, long nAnim)
    {
        bcs::G2D::CPaneObj *pPaneObj = GetPaneObj(nPane);
        if (pPaneObj == 0) {
            return 1;
        }
        return bcs::G2D::IsAnimEndRecursive(pPaneObj, nAnim) != 0;
    }

    long CMenuPaneAccessor::SetTextBoxString(long nPane, const wchar_t *pText, unsigned short nLen)
    {
        if (pText == 0) {
            return -1;
        }

        nw4r::lyt::Pane *pPane = GetPane(nPane);
        if (pPane == 0) {
            return -2;
        }

        bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);
        if (pTextBox == 0) {
            return -3;
        }

        // Only when the text changes.
        if (pTextBox->GetStringLength() != nLen || wcsncmp(pText, pTextBox->GetStringBuffer(), nLen) != 0) {
            int nBufLen = nLen + 2;
            if (nBufLen > pTextBox->GetStringBufferLength()) {
                pTextBox->AllocStringBuffer(nBufLen);
            }
            pTextBox->SetString(pText, 0, nLen);
            return 1;
        }
        return 0;
    }

    long CMenuPaneAccessor::SetTextBoxString(long nPane, unsigned long nValue, unsigned long nArg)
    {
        const wchar_t *pText = menu::GetNumString(nValue, nArg);
        return SetTextBoxString(nPane, pText, wcslen(pText));
    }

    bcs::G2D::CPaneObj *CMenuPaneAccessor::GetPaneObj(const char *pName)
    {
        if (pName == 0) {
            return 0;
        }
        if (m_pPaneCtrl == 0) {
            return 0;
        }
        return m_pPaneCtrl->GetChildPaneObj(pName);
    }

    nw4r::lyt::Pane *CMenuPaneAccessor::GetPane(const char *pName, int bRecursive, nw4r::lyt::Pane *pRoot)
    {
        if (pName == 0) {
            return 0;
        }
        if (m_pPaneCtrl == 0) {
            return 0;
        }
        return m_pPaneCtrl->FindPaneByName(pName, bRecursive, pRoot);
    }

    bcs::G2D::CPaneObj *CMenuPaneAccessor::GetPaneObj(nw4r::lyt::Pane *pPane)
    {
        if (pPane == 0) {
            return 0;
        }
        if (m_pPaneCtrl == 0) {
            return 0;
        }
        return m_pPaneCtrl->GetChildPaneObj(pPane);
    }
}
