#include "game/msw/CCockpitResult.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"
#include <wchar.h>

// (DOL, .sdata, unnamed) The layout name, "result_a.brlyt". Defined in the
// part of this file before 0x8019AECC, which is not written yet.
extern "C" const char *lbl_805BFD78;

namespace msw {

    long CCockpitResult::SetTitleText(const wchar_t *pText)
    {
        m_pPaneAccessor->SetTextBoxString(26, pText, wcslen(pText));
        return 0;
    }

    long CCockpitResult::CancelMenuImpl()
    {
        m_nState = 4;
        return 0;
    }

    void CCockpitResult::ChangeTextFont(int bColor)
    {
        // Looked up and not used.
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
        if (pLytCtrl == 0) {
            return;
        }

        nw4r::lyt::ResourceAccessor *pResAccessor = pLytCtrl->GetLayoutResourceAccesor(0, lbl_805BFD78);
        if (pResAccessor == 0) {
            return;
        }

        void *pFont = pResAccessor->GetResource('font', bColor ? "FOT-RS_NO_01_B.brfnt" : "FOT-RS_NO_02_B.brfnt", 0);
        if (pFont == 0) {
            return;
        }

        unsigned char aPane[4] = { 0x15, 0x14, 0x17, 0x16 };
        for (long i = 0; i < 4; i++) {
            nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(aPane[i]);
            if (pPane != 0) {
                bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);
                if (pTextBox != 0) {
                    // Setting the font resets the size: keep it.
                    nw4r::lyt::Size size = pTextBox->GetFontSize();
                    pTextBox->SetFontByResource(pFont);
                    pTextBox->SetFontSize(size);
                }
            }
        }
    }

    void CCockpitResult::SetResultButton(long nScore0, long nScore1)
    {
        long nButton;
        if (nScore0 > nScore1) {
            nButton = 0x20;
        } else if (nScore0 < nScore1) {
            nButton = 0x21;
        } else {
            nButton = 0x22;
        }

        // Both arms are the same in the game.
        float fDelay;
        if (m_nId == 0x101B) {
            fDelay = GetDispTime(5);
        } else {
            fDelay = GetDispTime(5);
        }

        cockpit::CResultButtonTask *pTask = new cockpit::CResultButtonTask;
        pTask->SetKind(1000);
        pTask->m_fDelay = fDelay;
        pTask->m_nButton = nButton;
        m_DispTaskList.PushBack(pTask);
    }

    void CCockpitResult::SetDelayResultButton()
    {
        // Both arms are the same in the game.
        float fDelay;
        if (m_nId == 0x101B) {
            fDelay = GetDispTime(5);
        } else {
            fDelay = GetDispTime(5);
        }

        cockpit::CDelayResultButtonTask *pTask = new cockpit::CDelayResultButtonTask;
        pTask->SetKind(2000);
        pTask->m_fDelay = fDelay;
        m_DispTaskList.PushBack(pTask);
        CCockpit::ClearResult(-1);
    }
}
