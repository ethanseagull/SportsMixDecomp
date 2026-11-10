#include "game/msw/SelectMenu/CButton0.hpp"

// The names of these three are guesses; the objects have no symbols. A const
// pointer lands in .rodata behind its string, the two tables in .data.
namespace {
    const char *const s_LayoutName = "main_fmake_04.brlyt";

    const char *s_AnimNames[] = {
        "main_fmake_04_texwin_1.brlan",
        "main_fmake_04_texwin_2.brlan",
        "main_fmake_04_texwin_3.brlan",
        "main_fmake_04_win_anm_00.brlan"
    };

    const char *s_PaneNames[] = {
        "tw",
        "t_mes01"
    };
}

msw::SelectMenu::CButton0::CButton0(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CButton0::~CButton0()
{
    Remove();
}

void msw::SelectMenu::CButton0::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(0, 0)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE: {
            // The game has both of these as symbols, `recursive` numbered
            // before `reset` but stored after it. static-order swap
            // (notes/decisions.md): this compiler stores local statics in the order they are declared, so the order
            // here gives the original bytes and swaps the two symbol names
            // (see notes/toolchain.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(0, 2, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_OUT;
            break;
        }
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(0, 2)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CButton0::Disp()
{
}

int msw::SelectMenu::CButton0::Set()
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
            pPaneCtrl->BuildAnim(0, s_AnimNames, 4);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, 2);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);
            InitPaneAnim();
        }

        m_Unk2C = 0;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CButton0::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CButton0::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

// Plays the two animations the window starts with. Inlined into Set(); it is
// defined down here because its local statics follow Set()'s float literal
// in .rodata.
inline void msw::SelectMenu::CButton0::InitPaneAnim()
{
    static const int recursive = 1;
    static const int reset = 1;

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(0, 1.0f, 0.0f, recursive, reset);
        pPaneCtrl->PlayAnim(3, 1.0f, 0.0f, recursive, reset);
    }
}
