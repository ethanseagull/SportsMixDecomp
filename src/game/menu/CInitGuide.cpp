#include "game/msw/SelectMenu/CInitGuide.hpp"

#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuPad.hpp"
#include "wchar.h"

namespace {
    // The mode InitGuide_Loop moves on to (name guessed).
    const long MODE_AFTER_INIT_GUIDE = 1;
}

void msw::CSelectMenu::InitGuide_In()
{
    CGamePadCheck::CleanUp();
    SetBlockFadeOut(1.0f);
}

void msw::CSelectMenu::InitGuide_Loop()
{
    SetNextMode(MODE_AFTER_INIT_GUIDE, 0.0f);
}

void msw::CSelectMenu::InitGuide_Out()
{
}

// The names in this namespace are guesses; the objects have no symbols.
// They are defined here, after the CSelectMenu steps, because their strings
// follow those steps' float constants in .rodata.
namespace {

    enum {
        ANIM_A_BTN,
        ANIM_TEXWIN_1,
        ANIM_TEXWIN_2,
        ANIM_TEXWIN_3,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_T_ATTENTION,
        PANE_A_BOTAN,
        PANE_TWO_BTN,
        PANE_BG_BLACK,
        PANE_NUM
    };

    const char *const s_LayoutName = "me_attention.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "me_attention_A_btn.brlan",
        "me_attention_texwin_1.brlan",
        "me_attention_texwin_2.brlan",
        "me_attention_texwin_3.brlan",
        "me_attention_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "t_attention",
        "A_botan",
        "two_btn",
        "bg_black"
    };

    // The pad style that shows the two-button prompt.
    const int PAD_STYLE_TWO_BUTTONS = 1;
}

msw::SelectMenu::CInitGuide::CInitGuide(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CInitGuide::~CInitGuide()
{
    Remove();
}

void msw::SelectMenu::CInitGuide::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_TEXWIN_1, 1.0f, 10.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_WIN, 1.0f, 10.0f, 1, 0);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            // Animation 6 is past the end of s_AnimNames; IsAnimEnd then
            // reports the end at once. Kept as in the game.
            if (m_pPaneAccessor->IsAnimEnd(PANE_TW, 6)) {
                m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_A_BTN, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_TEXWIN_2, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            SetButtonDispByPadStyle();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_TEXWIN_3, 1.0f, 0.0f, 1, 1);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TW, ANIM_TEXWIN_3)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CInitGuide::Disp()
{
}

int msw::SelectMenu::CInitGuide::Set()
{
    // Declared up here: declared where it is set, it lands in another
    // saved register than the original's.
    const wchar_t *pText;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(1.0f);

            pText = GetMessageLabelString("TopMenuMessage", "CNT_INF_00");
            m_pPaneAccessor->SetTextBoxString(PANE_T_ATTENTION, pText, wcslen(pText));
            m_pPaneAccessor->SetPaneDisp(PANE_BG_BLACK, 0);
            SetButtonDispByPadStyle();

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CInitGuide::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

// Shows the two-button prompt or the A button prompt, whichever matches the
// style of pad 0.
inline void msw::SelectMenu::CInitGuide::SetButtonDispByPadStyle()
{
    if (CMenuPad::s_Instance.GetPadStyle(0) == PAD_STYLE_TWO_BUTTONS) {
        m_pPaneAccessor->SetPaneDisp(PANE_A_BOTAN, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_TWO_BTN, 1);
    } else {
        m_pPaneAccessor->SetPaneDisp(PANE_A_BOTAN, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_TWO_BTN, 0);
    }
}
