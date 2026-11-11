// The help screens shown before a party game: one class per game, all with
// the same shape (a window with the controls, drawn for the controller
// style of pad 0). Only the layout, the panes switched by the controller
// style and what SendEvent does with event 0x32 differ.
#include "game/msw/SelectMenu/CPartyMEHelpBossPackun.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpBaseDodge.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpCrushSkate.hpp"
#include "game/msw/SelectMenu/CPartyMEHelpRhythmReceive.hpp"

#include "bcs/CMovie.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar, as in game/menu/CPartySelect.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module). The same in every screen.
    enum {
        ANIM_WIN_IN = 1,
        ANIM_WIN_OUT = 3,
        ANIM_WIN_IN_BACK = 4,
        ANIM_WIN_OUT_BACK = 5,
        ANIM_PLAYERS = 6, // played after event 0x32 has set the panes
        ANIM_SHOW = 7     // played on event 0x31
    };

    enum {
        PANE_WIN = 0
    };

    // What the screens are sent (SendEvent).
    enum {
        EVENT_CLOSE = 0x2D,
        EVENT_SHOW = 0x31,
        EVENT_PLAYERS = 0x32 // the parameter points to a number (of players?)
    };

    // CMenuPad::GetPadStyle values told apart here.
    enum {
        PAD_STYLE_NONE = -1,
        PAD_STYLE_1 = 1
    };
}

// ---------------------------------------------------------------------------
// CPartyMEHelpBossPackun

namespace {
    const char *const s_BossPackunLayoutName = "me_ptg_operation_BA.brlyt";
    const char *const s_BossPackunButtonFileName = "me_ptg_operation_BA.btn";
}

msw::SelectMenu::CPartyMEHelpBossPackun::CPartyMEHelpBossPackun(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartyMEHelpBossPackun::~CPartyMEHelpBossPackun()
{
    Remove();
}

inline void msw::SelectMenu::CPartyMEHelpBossPackun::UpdatePadStyle()
{
    CMenuPad *pPad = &CMenuPad::s_Instance;
    if (m_nPadStyle != pPad->GetPadStyle(0)) {
        if (pPad->GetPadStyle(0) == PAD_STYLE_1) {
            m_pPaneAccessor->SetPaneDisp(3, 0);
            m_pPaneAccessor->SetPaneDisp(5, 0);
            m_pPaneAccessor->SetPaneDisp(4, 1);
            m_pPaneAccessor->SetPaneDisp(6, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(3, 1);
            m_pPaneAccessor->SetPaneDisp(5, 1);
            m_pPaneAccessor->SetPaneDisp(4, 0);
            m_pPaneAccessor->SetPaneDisp(6, 0);
        }
        m_nPadStyle = pPad->GetPadStyle(0);
    }
}

void msw::SelectMenu::CPartyMEHelpBossPackun::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CPartySelect.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            UpdatePadStyle();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CPartyMEHelpBossPackun::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_BossPackunButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_BossPackunLayoutName);
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_nPadStyle = PAD_STYLE_NONE;
            UpdatePadStyle();
        }
    }
    return 0;
}

void msw::SelectMenu::CPartyMEHelpBossPackun::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartyMEHelpBossPackun::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_SHOW:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SHOW, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_PLAYERS:
            switch (*(long *)pParam) {
                case 0:
                case 1:
                    m_pPaneAccessor->SetPaneDisp(1, 0);
                    m_pPaneAccessor->SetPaneDisp(2, 0);
                    break;
                case 2:
                    m_pPaneAccessor->SetPaneDisp(1, 1);
                    m_pPaneAccessor->SetPaneDisp(2, 0);
                    break;
                case 3:
                    m_pPaneAccessor->SetPaneDisp(1, 0);
                    m_pPaneAccessor->SetPaneDisp(2, 1);
                    break;
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_PLAYERS, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// CPartyMEHelpBaseDodge

namespace {
    const char *const s_BaseDodgeLayoutName = "me_ptg_operation_DO.brlyt";
    const char *const s_BaseDodgeButtonFileName = "me_ptg_operation_DO.btn";
}

msw::SelectMenu::CPartyMEHelpBaseDodge::CPartyMEHelpBaseDodge(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartyMEHelpBaseDodge::~CPartyMEHelpBaseDodge()
{
    Remove();
}

inline void msw::SelectMenu::CPartyMEHelpBaseDodge::UpdatePadStyle()
{
    CMenuPad *pPad = &CMenuPad::s_Instance;
    if (m_nPadStyle != pPad->GetPadStyle(0)) {
        if (pPad->GetPadStyle(0) == PAD_STYLE_1) {
            m_pPaneAccessor->SetPaneDisp(2, 0);
            m_pPaneAccessor->SetPaneDisp(3, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(2, 1);
            m_pPaneAccessor->SetPaneDisp(3, 0);
        }
        m_nPadStyle = pPad->GetPadStyle(0);
    }
}

void msw::SelectMenu::CPartyMEHelpBaseDodge::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as above.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            UpdatePadStyle();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CPartyMEHelpBaseDodge::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_BaseDodgeButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_BaseDodgeLayoutName);
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_nPadStyle = PAD_STYLE_NONE;
            UpdatePadStyle();
        }
    }
    return 0;
}

void msw::SelectMenu::CPartyMEHelpBaseDodge::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();

    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0 && pMovie->IsPlay()) {
        pMovie->Stop();
    }

    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartyMEHelpBaseDodge::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_SHOW:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SHOW, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_PLAYERS:
            if (*(long *)pParam <= 1) {
                m_pPaneAccessor->SetPaneDisp(1, 0);
            } else {
                m_pPaneAccessor->SetPaneDisp(1, 1);
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_PLAYERS, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// CPartyMEHelpCrushSkate

namespace {
    const char *const s_CrushSkateLayoutName = "me_ptg_operation_HO.brlyt";
    const char *const s_CrushSkateButtonFileName = "me_ptg_operation_HO.btn";
}

msw::SelectMenu::CPartyMEHelpCrushSkate::CPartyMEHelpCrushSkate(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartyMEHelpCrushSkate::~CPartyMEHelpCrushSkate()
{
    Remove();
}

inline void msw::SelectMenu::CPartyMEHelpCrushSkate::UpdatePadStyle()
{
    CMenuPad *pPad = &CMenuPad::s_Instance;
    if (m_nPadStyle != pPad->GetPadStyle(0)) {
        if (pPad->GetPadStyle(0) == PAD_STYLE_1) {
            m_pPaneAccessor->SetPaneDisp(2, 0);
            m_pPaneAccessor->SetPaneDisp(3, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(2, 1);
            m_pPaneAccessor->SetPaneDisp(3, 0);
        }
        m_nPadStyle = pPad->GetPadStyle(0);
    }
}

void msw::SelectMenu::CPartyMEHelpCrushSkate::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as above.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            UpdatePadStyle();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CPartyMEHelpCrushSkate::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_CrushSkateButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_CrushSkateLayoutName);
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_nPadStyle = PAD_STYLE_NONE;
            UpdatePadStyle();
        }
    }
    return 0;
}

void msw::SelectMenu::CPartyMEHelpCrushSkate::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();

    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0 && pMovie->IsPlay()) {
        pMovie->Stop();
    }

    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartyMEHelpCrushSkate::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_SHOW:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SHOW, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_PLAYERS:
            if (*(long *)pParam <= 1) {
                m_pPaneAccessor->SetPaneDisp(1, 0);
            } else {
                m_pPaneAccessor->SetPaneDisp(1, 1);
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_PLAYERS, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// CPartyMEHelpRhythmReceive

namespace {
    const char *const s_RhythmReceiveLayoutName = "me_ptg_operation_VO.brlyt";
    const char *const s_RhythmReceiveButtonFileName = "me_ptg_operation_VO.btn";
}

msw::SelectMenu::CPartyMEHelpRhythmReceive::CPartyMEHelpRhythmReceive(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartyMEHelpRhythmReceive::~CPartyMEHelpRhythmReceive()
{
    Remove();
}

inline void msw::SelectMenu::CPartyMEHelpRhythmReceive::UpdatePadStyle()
{
    CMenuPad *pPad = &CMenuPad::s_Instance;
    if (m_nPadStyle != pPad->GetPadStyle(0)) {
        if (pPad->GetPadStyle(0) == PAD_STYLE_1) {
            m_pPaneAccessor->SetPaneDisp(2, 0);
            m_pPaneAccessor->SetPaneDisp(3, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(2, 1);
            m_pPaneAccessor->SetPaneDisp(3, 0);
        }
        m_nPadStyle = pPad->GetPadStyle(0);
    }
}

void msw::SelectMenu::CPartyMEHelpRhythmReceive::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as above.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            UpdatePadStyle();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CPartyMEHelpRhythmReceive::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_RhythmReceiveButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_RhythmReceiveLayoutName);
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_nPadStyle = PAD_STYLE_NONE;
            UpdatePadStyle();
        }
    }
    return 0;
}

void msw::SelectMenu::CPartyMEHelpRhythmReceive::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();

    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0 && pMovie->IsPlay()) {
        pMovie->Stop();
    }

    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartyMEHelpRhythmReceive::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_SHOW:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SHOW, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_PLAYERS:
            if (*(long *)pParam <= 1) {
                m_pPaneAccessor->SetPaneDisp(1, 0);
            } else {
                m_pPaneAccessor->SetPaneDisp(1, 1);
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_PLAYERS, 1.0f, 0.0f, 1, 0);
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
    }
    return -1;
}
