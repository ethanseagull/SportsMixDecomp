#include "game/msw/SelectMenu/CPlusMinus.hpp"

#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the pane table in .data.
namespace {

    const char *const s_LayoutName = "me_minus_plus.brlyt";
    const char *const s_ButtonFileName = "me_minus_plus.btn";

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 6,
        ANIM_WIN_OUT = 8
    };

    enum {
        PANE_WIN = 0,
        PANE_TEXT = 0x12 // the text box of button 0; the others follow
    };

    enum {
        BUTTON_NUM = 6
    };

    // The kind id of button 0; the others follow.
    enum {
        BTN_KIND_BUTTON = 0x80A0
    };

    // What the screen is sent, and what its object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21, // plus the pad number, 0 to 3
        EVENT_CLOSE = 0x25,
        EVENT_IS_PUSH_END = 0x26,
        EVENT_HIDE = 0x48,
        EVENT_SHOW = 0x49 // the parameter is the layout, as in StartUp
    };

    // The pane of each button.
    long s_ButtonPanes[BUTTON_NUM] = { 2, 5, 10, 12, 14, 17 };
}

msw::SelectMenu::CPlusMinus::CPlusMinus(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

msw::SelectMenu::CPlusMinus::~CPlusMinus()
{
    Remove();
}

void msw::SelectMenu::CPlusMinus::Main()
{
    if (m_pPaneAccessor == 0) {
        return;
    }

    switch (m_Unk30) {
        case STATE_SET: {
            // The game numbers these `recursive`, `reset@0`, `recursive@1`,
            // `reset@2` and stores them in the reverse order. All four are
            // 1, so the bytes are the same; only the symbol addresses are
            // exchanged: static-order swap (see notes/decisions.md).
            static const int recursive = 1;
            static const int reset = 1;
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE: {
            static const int recursive = 1;
            static const int reset = 1;
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_OUT;
            break;
        }
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CPlusMinus::Disp()
{
}

int msw::SelectMenu::CPlusMinus::Set()
{
    const char *pFileName;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    pFileName = pSelectMenu->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_ButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                SetUpButton(pPart, i, 0);
                pPart->m_bUnk40_0004 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            HideAllButtons();
        }
    }
    return 0;
}

void msw::SelectMenu::CPlusMinus::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
}

void msw::SelectMenu::CPlusMinus::StartUp(long nType, const wchar_t *pText1, const wchar_t *pText0)
{
    switch (nType) {
        case 0:
            if (pText0 != 0) {
                ShowButton(0, pText0);
            }
            break;
        case 1:
            if (pText1 != 0) {
                ShowButton(1, pText1);
            }
            break;
        case 2:
            if (pText0 != 0) {
                ShowButton(2, pText0);
            }
            if (pText1 != 0) {
                ShowButton(3, pText1);
            }
            break;
        case 3:
            if (pText0 != 0) {
                ShowButton(4, pText0);
            }
            if (pText1 != 0) {
                ShowButton(5, pText1);
            }
            break;
    }
    m_Unk2C = 1;
}

long msw::SelectMenu::CPlusMinus::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (m_pPaneAccessor == 0) {
        return -1;
    }

    switch (nEvent) {
        case EVENT_HIT:
            nResult = HitCheck((bcs::FVEC3 *)pParam);
            break;
        case EVENT_BACK:
            m_pMenuPartList->CheckButtonAnim((const unsigned short *)pParam, 0);
            nResult = 0;
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
        case EVENT_IS_PUSH_END:
            nResult = m_pMenuPartList->IsPushButtonAnimEnd();
            break;
        case EVENT_PUSH + 0:
        case EVENT_PUSH + 1:
        case EVENT_PUSH + 2:
        case EVENT_PUSH + 3:
            nResult = PushButton(nEvent - EVENT_PUSH, (unsigned short)(long)pParam);
            break;
        case EVENT_HIDE:
            HideAllButtons();
            break;
        case EVENT_SHOW:
            switch ((long)pParam) {
                case 0:
                    ShowButton(0);
                    break;
                case 1:
                    ShowButton(1);
                    break;
                case 2:
                    ShowButton(2);
                    ShowButton(3);
                    break;
                case 3:
                    ShowButton(4);
                    ShowButton(5);
                    break;
            }
            break;
        default:
            nResult = 0;
            break;
    }
    return nResult;
}

long msw::SelectMenu::CPlusMinus::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CPlusMinus::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlusMinus::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlusMinus::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}

long msw::SelectMenu::CPlusMinus::SetDefaultMoveCursor(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlusMinus::StickMoveCheck(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlusMinus::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPlusMinus::SetCursorMarkerPosition(long, long)
{
    return 0;
}

inline void msw::SelectMenu::CPlusMinus::HideAllButtons()
{
    for (long i = 0; i < BUTTON_NUM; i++) {
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_BUTTON + i)->m_nFlags40 |= 0x0001;
        m_pPaneAccessor->SetPaneDisp(s_ButtonPanes[i], 0);
    }
}

inline void msw::SelectMenu::CPlusMinus::ShowButton(long nButton)
{
    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_BUTTON + nButton)->m_nFlags40 &= ~0x0001;
    m_pPaneAccessor->SetPaneDisp(s_ButtonPanes[nButton], 1);
}

inline void msw::SelectMenu::CPlusMinus::ShowButton(long nButton, const wchar_t *pText)
{
    m_pPaneAccessor->SetTextBoxString(PANE_TEXT + nButton, pText, wcslen(pText));
    ShowButton(nButton);
}

// The two below belong to CSelectMenu but are defined in this file (the
// game places them after CPlusMinus's members).

int msw::CSelectMenu::_GetUsedCursol(long nPad)
{
    return CMenuPadInfo::s_Instance.IsUsedCursor(nPad);
}

void msw::CSelectMenu::_SetStarMii(long nPad)
{
    // Looked up and not used.
    FindObj(0x45);

    long nMode = m_Unk9B;
    if (nMode == 4) {
        // The game tests the mode a second time here.
        if (m_bStarMii != 0 && nMode == 4) {
            if (nPad == 0) {
                CSave::PlayerData *pPlayerData = m_pPlayerData;
                Mii_SelectParam(nPad, pPlayerData->m_nMiiUnk, pPlayerData->m_nMiiIndex,
                                pPlayerData->m_MiiSource, pPlayerData->m_MiiCreateId);
            } else {
                Mii_SelectParam(nPad, m_nStarMiiUnk, m_nStarMiiIndex, m_StarMiiSource, m_StarMiiCreateId);
            }
        }
    } else {
        Chara_SetSelectChara(nPad, 0x13, 0xFF, 0);
    }
}
