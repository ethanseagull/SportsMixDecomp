#include "game/msw/SelectMenu/CButtonACode.hpp"

#include <wchar.h>

#include "game/msw/CMenuPartButton.hpp"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the name tables in .data.
// The two layouts ("main_fmake_02", "main_fmake_06") share the panes and the
// button; the second has a second message line where the first names
// "t_mes01" twice.
namespace {

    enum {
        ANIM_ABUTTON_1,
        ANIM_ABUTTON_2,
        ANIM_ABUTTON_3,
        ANIM_TEXWIN_1,
        ANIM_TEXWIN_2,
        ANIM_TEXWIN_3,
        ANIM_WIN,
        ANIM_TEX_CSR,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_T_MES01,
        PANE_T_MES02,
        PANE_A_BOTAN,
        PANE_A_BOTAN_ATARI,
        PANE_T_FCODE,
        PANE_TEX_CSR,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 1
    };

    // The kind id the cursor falls back to (the button's own is 3).
    enum {
        BTN_KIND_DEFAULT = 7
    };

    const char *const s_LayoutName = "main_fmake_02.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_fmake_02_abutton_1.brlan",
        "main_fmake_02_abutton_2.brlan",
        "main_fmake_02_abutton_3.brlan",
        "main_fmake_02_texwin_1.brlan",
        "main_fmake_02_texwin_2.brlan",
        "main_fmake_02_texwin_3.brlan",
        "main_fmake_02_win_anm_00.brlan",
        "main_fmake_02_tex_csr.brlan"
    };

    // This layout has one message line; its slot for the second names the
    // first again.
    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "t_mes01",
        "t_mes01",
        "A_botan",
        "A_botan_atari",
        "t_fcode02",
        "tex_csr"
    };

    // Size 0xC. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned short m_nNextBtnKindId[5]; // 0x02
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { 3, PANE_A_BOTAN_ATARI, { BTN_KIND_DEFAULT, 0, 0, 0, 0 } }
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CButtonACode::CButtonACode(long nId)
    : CButtonA(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CButtonACode::~CButtonACode()
{
    Remove();
}

void msw::SelectMenu::CButtonACode::Main()
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
                // static-order swap: declared in the order they are stored,
                // which swaps the two symbol names (see CButton1::Main and
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

void msw::SelectMenu::CButtonACode::Disp()
{
}

int msw::SelectMenu::CButtonACode::Set()
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

void msw::SelectMenu::CButtonACode::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CButtonACode::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

// Plays the window's opening animations and starts the cursor pane's. Inlined
// into both Set()s; defined after the first so that its local statics follow
// that Set()'s in .rodata.
inline void msw::SelectMenu::CButtonACode::InitPaneAnim()
{
    // static-order swap: the game names these recursive, reset@0, speed@1,
    // frame@2 and stores them in the reverse order; declaring them in stored
    // order gives the original bytes under swapped names (notes/decisions.md).
    static const float frame = 0.0f;
    static const float speed = 1.0f;
    static const int reset = 1;
    static const int recursive = 1;

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(ANIM_TEXWIN_1, speed, frame, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_ABUTTON_1, speed, frame, recursive, reset);
        pPaneCtrl->PlayAnim(ANIM_WIN, speed, frame, recursive, reset);
        m_pPaneAccessor->PlayAnimation(PANE_TEX_CSR, ANIM_TEX_CSR, speed, frame, recursive, reset);
    }
}

// The second layout's tables.
namespace {

    const char *const s_LayoutName2 = "main_fmake_06.brlyt";

    const char *s_AnimNames2[ANIM_NUM] = {
        "main_fmake_06_abutton_1.brlan",
        "main_fmake_06_abutton_2.brlan",
        "main_fmake_06_abutton_3.brlan",
        "main_fmake_06_texwin_1.brlan",
        "main_fmake_06_texwin_2.brlan",
        "main_fmake_06_texwin_3.brlan",
        "main_fmake_06_win_anm_00.brlan",
        "main_fmake_06_tex_csr.brlan"
    };

    const char *s_PaneNames2[PANE_NUM] = {
        "tw",
        "t_mes01",
        "t_mes02",
        "A_botan",
        "A_botan_atari",
        "t_fcode",
        "tex_csr"
    };
}

msw::SelectMenu::CButtonACode2::CButtonACode2(long nId)
    : CButtonACode(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CButtonACode2::~CButtonACode2()
{
    Remove();
}

int msw::SelectMenu::CButtonACode2::Set()
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
            pPaneCtrl->BuildLayout(0, s_LayoutName2);
            pPaneCtrl->BuildAnim(0, s_AnimNames2, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames2, PANE_NUM);
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

long msw::SelectMenu::CButtonACode2::SetMessageText(long nLine, const wchar_t *pText)
{
    if (pText == 0) {
        return -1;
    }

    switch (nLine) {
        case 0:
            m_pPaneAccessor->SetTextBoxString(PANE_T_MES01, pText, wcslen(pText));
            return 1;
        case 1:
            m_pPaneAccessor->SetTextBoxString(PANE_T_MES02, pText, wcslen(pText));
            return 1;
    }
    return 0;
}
