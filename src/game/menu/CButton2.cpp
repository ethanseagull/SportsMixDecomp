#include "game/msw/SelectMenu/CButton2.hpp"

#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the two name tables in
// .data, the button table in .rodata.
namespace {

    enum {
        ANIM_BUTTON_1,
        ANIM_BUTTON_2,
        ANIM_BUTTON_3,
        ANIM_BUTTON_4,
        ANIM_BUTTON_5,
        ANIM_BUTTON_6,
        ANIM_BUTTON_7,
        ANIM_TEXWIN_1,
        ANIM_TEXWIN_2,
        ANIM_TEXWIN_3,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_T_MES01,
        PANE_BTN_00,
        PANE_B_BTN_00,
        PANE_T_BTN_00,
        PANE_BTN_01,
        PANE_B_BTN_01,
        PANE_T_BTN_01,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 2
    };

    // The kind ids of the two buttons and the one the cursor falls back to.
    enum {
        BTN_KIND_0 = 2,
        BTN_KIND_1 = 5,
        BTN_KIND_DEFAULT = 8,
        BTN_KIND_OTHER = 9 // not a button of this screen
    };

    const char *const s_LayoutName = "main_pmake_06.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_pmake_06_button_1.brlan",
        "main_pmake_06_button_2.brlan",
        "main_pmake_06_button_3.brlan",
        "main_pmake_06_button_4.brlan",
        "main_pmake_06_button_5.brlan",
        "main_pmake_06_button_6.brlan",
        "main_pmake_06_button_7.brlan",
        "main_pmake_06_texwin_1.brlan",
        "main_pmake_06_texwin_2.brlan",
        "main_pmake_06_texwin_3.brlan",
        "main_pmake_06_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "t_mes01",
        "btn_00",
        "b_btn_00",
        "t_btn_00",
        "btn_01",
        "b_btn_01",
        "t_btn_01"
    };

    // Size 0xC. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned short m_nNextBtnKindId[5]; // 0x02
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { BTN_KIND_0, PANE_B_BTN_00, { BTN_KIND_DEFAULT, 0, BTN_KIND_OTHER, 0, 0 } },
        { BTN_KIND_1, PANE_B_BTN_01, { BTN_KIND_OTHER, BTN_KIND_DEFAULT, 0, 0, 0 } }
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CButton2::CButton2(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CButton2::~CButton2()
{
    Remove();
}

void msw::SelectMenu::CButton2::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TW, ANIM_TEXWIN_1)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // The game has both of these as symbols, `recursive`
                // numbered before `reset` but stored after it. This
                // compiler stores local statics in the order they are
                // declared, so the order here gives the original bytes and
                // swaps the two symbol names: static-order swap (see
                // notes/decisions.md).
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_TEXWIN_3, 1.0f, 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_BTN_00, ANIM_BUTTON_3, 1.0f, 0.0f, recursive, reset);
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

void msw::SelectMenu::CButton2::Disp()
{
}

int msw::SelectMenu::CButton2::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            pPaneCtrl->SetPaneZSort(1);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            static const int btnNum = BUTTON_NUM;

            for (long i = 0; i < btnNum; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const ButtonParam *pParam = GetButtonParam(i);

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nAnim[0] = ANIM_BUTTON_4;
                pPart->m_nAnim[1] = ANIM_BUTTON_5;
                pPart->m_nAnim[2] = ANIM_BUTTON_6;
                pPart->m_nAnim[3] = ANIM_BUTTON_2;
                pPart->m_nAnim[4] = ANIM_BUTTON_7;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = 0xBBB;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                pPart->m_bUnk40_0020 = 1;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            InitPaneAnim();
        }

        m_Unk2C = 0;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CButton2::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CButton2::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CButton2::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CButton2::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CButton2::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CButton2::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CButton2::SetCursorMarkerVisible(long, int)
{
    return 0;
}

unsigned short msw::SelectMenu::CButton2::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    return m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CButton2::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_DEFAULT;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CButton2::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

// Plays the three animations the window starts with. Inlined into Set(); it
// is defined down here because its local statics follow Set()'s in .rodata.
inline void msw::SelectMenu::CButton2::InitPaneAnim()
{
    // The game stores `reset` before `recursive`; both are 1, so the bytes
    // are the same either way and only the two symbol addresses are
    // exchanged: static-order swap (see notes/decisions.md).
    static const int recursive = 1;
    static const int reset = 1;

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(ANIM_TEXWIN_1, 1.0f, 0.0f, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_BUTTON_1, 1.0f, 0.0f, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_WIN, 1.0f, 0.0f, recursive, reset);
    }
}

long msw::SelectMenu::CButton2::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}
