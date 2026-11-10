#include "game/msw/SelectMenu/CEntryExplain.hpp"

#include <wchar.h>

#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/menu/CMessage.hpp"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the two name tables and
// the two message names in .data, the button table in .rodata.
namespace {

    enum {
        ANIM_A_BTN,
        ANIM_INOUT_00,
        ANIM_INOUT_01,
        ANIM_INOUT_02,
        ANIM_INOUT_03,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_FRAME_INOUT,
        PANE_A_BOTAN_00,
        PANE_TWO_BTN,
        PANE_E_TW,
        PANE_BG_BLACK,
        PANE_T_MES00,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 2
    };

    // Sent to the owner's object 1 when the button has been pushed; the
    // window closes when that returns nonzero. The name is a guess.
    const long EVENT_EXPLAIN_END = 0x26;

    const char *const s_LayoutName = "me_ent_a.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "me_ent_a_A_btn.brlan",
        "me_ent_a_inout_00.brlan",
        "me_ent_a_inout_01.brlan",
        "me_ent_a_inout_02.brlan",
        "me_ent_a_inout_03.brlan",
        "me_ent_a_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "frame_inout",
        "A_botan_00",
        "two_btn",
        "e_tw",
        "bg_black",
        "t_mes00"
    };

    // Size 4. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;      // 0x00
        unsigned short m_nNextBtnKindId; // 0x02
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { 1, 0x26 },
        { 2, 0x26 }
    };

    // The message shown in the window.
    const char *s_MessageFile = "ExhibitionMenuMessage";
    const char *s_MessageLabel = "ENT_A_00";

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CEntryExplain::CEntryExplain(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CEntryExplain::~CEntryExplain()
{
    Remove();
}

void msw::SelectMenu::CEntryExplain::Main()
{
    StateControl();

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CEntryExplain::Disp()
{
}

// Shows the A button guide or the two-button guide, by the pad style of
// pad 0.
inline void msw::SelectMenu::CEntryExplain::SetDispButtonGuide()
{
    if (CMenuPad::s_Instance.GetPadStyle(0) != 0) {
        m_pPaneAccessor->SetPaneDisp(PANE_A_BOTAN_00, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_TWO_BTN, 1);
    } else {
        m_pPaneAccessor->SetPaneDisp(PANE_A_BOTAN_00, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_TWO_BTN, 0);
    }
}

void msw::SelectMenu::CEntryExplain::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // The game numbers `recursive` before `reset` in each block but
            // stores all four in reverse. This compiler stores local statics
            // in the order they are declared, so the order here gives the
            // original bytes and swaps the symbol names: static-order swap
            // (see notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_FRAME_INOUT, ANIM_INOUT_01, 1.0f, 0.0f, recursive, reset);
            m_pPaneAccessor->PlayAnimation(PANE_E_TW, ANIM_WIN, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_FRAME_INOUT, ANIM_INOUT_01)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            SetDispButtonGuide();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                if (GetSelectMenu()->SendObjEventTrigger(1, EVENT_EXPLAIN_END, 0)) {
                    // static-order swap, as above.
                    static const int reset = 1;
                    static const int recursive = 0;
                    m_pPaneAccessor->PlayAnimation(PANE_FRAME_INOUT, ANIM_INOUT_03, 1.0f, 0.0f, recursive, reset);
                    m_Unk30 = STATE_OUT;
                }
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_FRAME_INOUT, ANIM_INOUT_03)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }
}

int msw::SelectMenu::CEntryExplain::Set()
{
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
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);

            long nFile = menu::CMessage::s_Instance.GetFileIndex(s_MessageFile);
            const wchar_t *pText = menu::CMessage::s_Instance.GetTextByLabel(nFile, s_MessageLabel);
            m_pPaneAccessor->SetTextBoxString(PANE_T_MES00, pText, wcslen(pText));

            pPaneCtrl->SetSortZ(120.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            // A function-local static, as in the game: the loop bound is read
            // from memory.
            static const int btnNum = BUTTON_NUM;

            for (long i = 0; i < btnNum; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const ButtonParam *pParam = GetButtonParam(i);

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                // The game clears them in this order.
                pPart->m_nAnim[0] = 0;
                pPart->m_nAnim[2] = 0;
                pPart->m_nAnim[1] = 0;
                pPart->m_nAnim[3] = 0;
                pPart->m_nAnim[4] = 0;
                pPart->m_nAnim[5] = 0;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId;
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = -1;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                pPart->m_bUnk40_0020 = 1;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            m_pPaneAccessor->SetPaneDisp(PANE_BG_BLACK, 0);
            SetDispButtonGuide();
        }
    }
    return 0;
}

void msw::SelectMenu::CEntryExplain::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CEntryExplain::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CEntryExplain::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CEntryExplain::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntryExplain::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntryExplain::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntryExplain::SetCursorMarkerVisible(long, int)
{
    return 0;
}

// The virtual returns `long`: this override returns nBtnKindId whole (no
// clrlwi) and CGamePadCheck compares the result as a word.
long msw::SelectMenu::CEntryExplain::StickMoveCheck(long nPad, long nBtnKindId)
{
    return nBtnKindId;
}

long msw::SelectMenu::CEntryExplain::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return nBtnKindId;
}

int msw::SelectMenu::CEntryExplain::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return 1;
    }

    int bStickMode = 0;
    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CEntryExplain::PushButton(long nPad, long nBtnKindId)
{
    return m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
}
