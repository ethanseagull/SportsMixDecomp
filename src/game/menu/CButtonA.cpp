#include "game/msw/SelectMenu/CButtonA.hpp"

#include <wchar.h>

#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the two name tables in
// .data, the button table in .rodata.
namespace {

    enum {
        ANIM_ABUTTON_1,
        ANIM_ABUTTON_2,
        ANIM_ABUTTON_3,
        ANIM_TEXWIN_1,
        ANIM_TEXWIN_2,
        ANIM_TEXWIN_3,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_T_MES01,
        PANE_A_BOTAN,
        PANE_A_BOTAN_ATARI,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 1
    };

    // The kind id the cursor falls back to (the button's own is 2).
    enum {
        BTN_KIND_DEFAULT = 10
    };

    const char *const s_LayoutName = "main_pmake_03.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_pmake_03_abutton_1.brlan",
        "main_pmake_03_abutton_2.brlan",
        "main_pmake_03_abutton_3.brlan",
        "main_pmake_03_texwin_1.brlan",
        "main_pmake_03_texwin_2.brlan",
        "main_pmake_03_texwin_3.brlan",
        "main_pmake_03_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "t_mes01",
        "A_botan",
        "A_botan_atari"
    };

    // Size 0xC. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned short m_nNextBtnKindId[5]; // 0x02
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { 2, PANE_A_BOTAN_ATARI, { BTN_KIND_DEFAULT, 0, 0, 0, 0 } }
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CButtonA::CButtonA(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CButtonA::~CButtonA()
{
    Remove();
}

void msw::SelectMenu::CButtonA::Main()
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
            CheckButtonAnim();
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

void msw::SelectMenu::CButtonA::Disp()
{
}

int msw::SelectMenu::CButtonA::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
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
                pPart->m_nAnim[0] = ANIM_ABUTTON_2;
                pPart->m_nAnim[1] = ANIM_ABUTTON_2;
                pPart->m_nAnim[2] = ANIM_ABUTTON_2;
                pPart->m_nAnim[3] = ANIM_ABUTTON_2;
                pPart->m_nAnim[4] = ANIM_ABUTTON_3;
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

void msw::SelectMenu::CButtonA::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CButtonA::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CButtonA::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CButtonA::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CButtonA::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CButtonA::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CButtonA::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CButtonA::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CButtonA::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_DEFAULT;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CButtonA::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CButtonA::CheckButtonAnim()
{
    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    return 0;
}

// Plays the three animations the window starts with. Inlined into Set(); it
// is defined down here because its local statics follow Set()'s in .rodata.
inline void msw::SelectMenu::CButtonA::InitPaneAnim()
{
    // The game stores `reset` before `recursive`; both are 1, so the bytes
    // are the same either way and only the two symbol addresses are
    // exchanged: static-order swap (see notes/decisions.md).
    static const int recursive = 1;
    static const int reset = 1;

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(ANIM_TEXWIN_1, 1.0f, 0.0f, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_ABUTTON_1, 1.0f, 0.0f, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_WIN, 1.0f, 0.0f, recursive, reset);
    }
}

long msw::SelectMenu::CButtonA::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}

long msw::SelectMenu::CButtonA::SetMessageText(long nLine, const wchar_t *pText)
{
    if (pText == 0) {
        return -1;
    }

    switch (nLine) {
        case 0:
            m_pPaneAccessor->SetTextBoxString(PANE_T_MES01, pText, wcslen(pText));
            return 1;
    }
    return 0;
}
