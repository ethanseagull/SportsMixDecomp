// The cup select screen (CCupSelect, "me_cup_select"), the difficulty tabs
// over it (CCupSelectTab, "me_cup_select_w") and the message window that
// explains the selected difficulty (CLevelMessage, "me_cup_select_mes").
// The two button screens take their panes, animations and buttons from a
// button file (CButtonEntry).
#include "game/msw/SelectMenu/CCupSelect.hpp"
#include "game/msw/SelectMenu/CCupSelectTab.hpp"
#include "game/msw/SelectMenu/CLevelMessage.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the windows' opening and closing animations. A
    // table, not a scalar, as in game/menu/CCourtTab.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    const char *const s_LayoutName = "me_cup_select.brlyt";
    const char *const s_ButtonFileName = "me_cup_select.btn";

    // CCupSelect's panes and animations (indices into the button file's
    // name tables).
    enum {
        PANE_WIN = 0
    };

    enum {
        ANIM_CUP_DISABLED = 0x0A,   // a disabled cup button
        ANIM_CUP_DISABLED_C = 0x05, // + game kind: its colour
        ANIM_APPEAR = 0x0F,         // the cup buttons appear
        ANIM_WIN_IN = 0x14,
        ANIM_WIN_OUT = 0x16,
        ANIM_WIN_IN_BACK = 0x17,    // window in, coming back
        ANIM_WIN_OUT_BACK = 0x18,   // window out, going back
        ANIM_CUP_APPEAR_PANE = 0x0D // the pane that plays ANIM_APPEAR
    };

    // Per cup: the pane that plays the cup button's animations, the "new"
    // mark and the pane hung into it.
    long s_CupPanes[3] = { 8, 5, 2 };
    long s_NewPanes[3] = { 16, 15, 14 };
    long s_NewPicPanes[4] = { 17, 18, 19, 0 };

    // The difficulty tabs' panes, for two and three difficulties.
    long s_TabPanes2[2] = { 4, 6 };
    long s_TabPanes3[4] = { 10, 13, 16, 0 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TASK = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_CUP_SELECT_TAB = 0x34,
        OBJ_LEVEL_MESSAGE = 0x35
    };

    // Events sent to and handled by the screens.
    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,          // plus the pad number
        EVENT_OUT = 0x25,
        EVENT_CLOSE_OK = 0x26,
        EVENT_CLOSE = 0x2E,
        EVENT_OPEN = 0x2F,
        EVENT_TAB_ANIM_A = 0x3D,
        EVENT_TAB_ANIM_B = 0x3E,
        EVENT_GET_LEVEL = 0x44,
        EVENT_LEVEL_CHANGED = 0x50
    };

    enum {
        BTN_KIND_CUP_0 = 0x8010,
        BTN_KIND_TAB2_0 = 0x8020,   // the tabs of two difficulties
        BTN_KIND_TAB3_0 = 0x8030    // the tabs of three difficulties
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_UP_DOWN = 0x05,
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08
    };

    enum {
        SE_CURSOR = 0xBC0,
        SE_NEXT = 0xBCD,
        SE_PREV = 0xBCE
    };

    // The game kind (CSelectMenu::m_nGameKind) that only has the expert
    // difficulty.
    enum {
        GAME_KIND_EXPERT = 5
    };

    // Frames the opening animation runs before its end is checked.
    const float WIN_IN_WAIT = 20.0f;
}

// ---------------------------------------------------------------------------
// CCupSelect

msw::SelectMenu::CCupSelect::CCupSelect(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CCupSelect::~CCupSelect()
{
    Remove();
}

void msw::SelectMenu::CCupSelect::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CCourtTab.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_fWait = WIN_IN_WAIT;
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            m_fWait -= bcs::CMainApp::GetAppFrameRateF();
            if (m_fWait < 0.0f) {
                int bEnd = 0;
                if (IsBackFrameIn(0)) {
                    if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                        bEnd = 1;
                    }
                } else {
                    if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                        bEnd = 1;
                    }
                }
                if (bEnd) {
                    m_bSeEnable = 1;
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()
                && GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_CLOSE_OK, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_CUP_SELECT_TAB, EVENT_CLOSE, 0);
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_OUT, 0);
                GetSelectMenu()->SendObjEventTrigger(OBJ_LEVEL_MESSAGE, EVENT_CLOSE, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game tests IsBackFrameIn here, not IsBackFrameOut.
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
        default:
            // Nothing to update before Set().
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CCupSelect::Disp()
{
}

int msw::SelectMenu::CCupSelect::Set()
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
            ArchiveBuildLoader(s_ButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
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
                pPart->m_nFlags40 |= 0x0004;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            // Moves a picture into each "new" mark and hides the marks.
            nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            for (long i = 0; i < 3; i++) {
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_NewPanes[i]);
                nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(s_NewPicPanes[i]);
                bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
                pPane->SetTranslate(zero);
                m_pPaneAccessor->SetPaneDisp(s_NewPanes[i], 0);
            }
            m_pPaneAccessor->SetPaneDisp(PANE_WIN, 1);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            // The +/- buttons, except in the expert-only game kind.
            // m_nPlayerNum is compared as a signed value (cmpwi), as
            // m_Unk9B in game/menu/CCourtTab.cpp.
            m_bPlusMinus = 0;
            if (pSelectMenu->m_nGameKind != GAME_KIND_EXPERT
                && (pSelectMenu->m_nGameKind == 1 || (long)pSelectMenu->m_nPlayerNum == 1)) {
                pSelectMenu->Build(OBJ_PLUS_MINUS);
                CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
                pPlusMinus->StartUp(1, GetMessageLabelString("List_Plus_Minus_Button", "Plus_04"), 0);
                m_bPlusMinus = 1;
            }
        }
    }
    return 0;
}

// Inlined into Main() although defined after it; defined down here because
// its local static follows Set()'s constants in .rodata. Not declared
// inline: an inline definition after its caller loses the static (see
// notes/toolchain.md, CRuleInfo); the unused out-of-line copy is stripped by
// the module link.
void msw::SelectMenu::CCupSelect::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

// Never called; only their local statics are left in the game's .rodata,
// so the original link stripped them (as GetTargetInfo in
// game/boss/CCockpitBossCoin.cpp). The bodies follow
// game/menu/CFriendInfo.cpp; the second writes the number through the
// accessor instead of formatting it, so that no literal is added.
void msw::SelectMenu::CCupSelect::SetTextPaneString(long nPane, const wchar_t *pText)
{
    static const bool recursive = true;

    if (pText != 0) {
        CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
        nw4r::lyt::Pane *pPane =
            (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
        if (pPane != 0) {
            m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
        }
    }
}

void msw::SelectMenu::CCupSelect::SetTextPaneString(long nPane, long nValue)
{
    static const bool recursive = true;

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
    nw4r::lyt::Pane *pPane =
        (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
    if (pPane != 0) {
        m_pPaneAccessor->SetTextBoxString(nPane, nValue, 0);
    }
}

void msw::SelectMenu::CCupSelect::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCupSelect::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_CUP_SELECT_TAB, EVENT_HIT, pPos);
    if (nResult == 0) {
        nResult = m_pMenuPartList->CheckHitButton(pPos);
        if (nResult == 0) {
            nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_HIT, pPos);
        }
    }
    if (m_bPlusMinus != 0 && nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CCupSelect::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelect::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelect::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelect::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCupSelect::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CCupSelect::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_CUP_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCupSelect::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCupSelect::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    long nEvent = nPad + EVENT_PUSH;
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nEvent, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}

void msw::SelectMenu::CCupSelect::SetAppearanceAnim()
{
    m_pPaneAccessor->PlayAnimation(ANIM_CUP_APPEAR_PANE, ANIM_APPEAR, 1.0f, 0.0f, 1, 1);
}

void msw::SelectMenu::CCupSelect::SetStageEnable(long nCup, int bEnable, unsigned char bDisp)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_CUP_0 + nCup);
    if (pPart == 0) {
        return;
    }

    // The colour animations are by game kind; the expert-only kind uses
    // the one before it.
    long nGameKind = GetSelectMenu()->m_nGameKind;
    if (nGameKind == GAME_KIND_EXPERT) {
        nGameKind = GAME_KIND_EXPERT - 1;
    }
    if (bEnable == 1) {
        // Flag 0x0001 marks a disabled part.
        pPart->m_nFlags40 &= 0xFFFE;
        m_pPaneAccessor->PlayAnimation(s_CupPanes[nCup], nGameKind, 1.0f, 0.0f, 1, 1);
        m_pPaneAccessor->SetPaneDisp(s_NewPanes[nCup], bDisp);
    } else {
        pPart->m_nFlags40 |= 0x0001;
        m_pPaneAccessor->PlayAnimation(s_CupPanes[nCup], ANIM_CUP_DISABLED, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_CupPanes[nCup], nGameKind + ANIM_CUP_DISABLED_C, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->SetPaneDisp(s_NewPanes[nCup], 0);
    }
}

void msw::SelectMenu::CCupSelect::CallbackButtonAnimIn(long)
{
    GetSelectMenu()->SendObjEventTrigger(OBJ_CUP_SELECT_TAB, EVENT_GET_LEVEL, 0);

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    if (CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
        return;
    }
    if (nInput & PAD_UP_DOWN) {
        SE_Play(SE_CURSOR, 1);
    } else if (nInput & PAD_LEFT) {
        SE_Play(SE_PREV, 1);
    } else if (nInput & PAD_RIGHT) {
        SE_Play(SE_NEXT, 1);
    }
}

// ---------------------------------------------------------------------------
// CCupSelectTab

namespace {

    const char *const s_TabLayoutName = "me_cup_select_w.brlyt";
    const char *const s_TabButtonFileName = "me_cup_select_w.btn";

    // CCupSelectTab's panes and animations (indices into the button
    // file's name tables).
    enum {
        TAB_PANE_WIN = 0,
        TAB_PANE_TAB_2 = 0x02,      // the row of two tabs
        TAB_PANE_TAB_3 = 0x09,      // the row of three tabs
        TAB_PANE_YAJI_L = 0x12,     // the arrow before the first tab
        TAB_PANE_YAJI_R = 0x13,     // the arrow after the last tab
        TAB_PANE_ANIM = 0x14,
        TAB_PANE_ONE = 0x15,        // the single tab of the expert-only kind
        TAB_PANE_ONE_B = 0x1B,      // the single tab otherwise
        TAB_PANE_ONE_POS = 0x1C,
        TAB_PANE_ROW_2 = 0x19,
        TAB_PANE_ROW_3 = 0x1A
    };

    enum {
        TAB_ANIM_ARROW_ON = 0,
        TAB_ANIM_ARROW_OFF = 1,
        TAB_ANIM_A_0 = 2,
        TAB_ANIM_A_1 = 3,
        TAB_ANIM_A_2 = 4,
        TAB_ANIM_A_3 = 5,
        TAB_ANIM_VIS_ON = 6,
        TAB_ANIM_VIS_OFF = 7,
        TAB_ANIM_WIN_IN = 0x09,
        TAB_ANIM_WIN_OUT = 0x0B,
        TAB_ANIM_WIN_IN_BACK = 0x0C,
        TAB_ANIM_WIN_OUT_BACK = 0x0D,
        TAB_ANIM_ROW_OFF = 0x0E,
        TAB_ANIM_ROW_ON = 0x0F,
        TAB_ANIM_TAB_OFF = 0x10,    // tab off
        TAB_ANIM_TAB_POINT = 0x11,  // pointer over a tab
        TAB_ANIM_TAB_SELECT = 0x12, // tab selected
        TAB_ANIM_TAB_LEAVE = 0x13,  // pointer leaves the selected tab
        TAB_ANIM_TAB_DESELECT = 0x14,
        TAB_ANIM_TAB_POINT_SEL = 0x15, // pointer over the selected tab
        TAB_ANIM_WIN_C_00 = 0x16    // + game kind: the window's colour
    };

    // Values of CCupSelectTab::m_nTabAnimState.
    enum {
        TAB_ANIM_STATE_NONE = 0,
        TAB_ANIM_STATE_A = 1,
        TAB_ANIM_STATE_A_WAIT = 2,
        TAB_ANIM_STATE_B = 3,
        TAB_ANIM_STATE_B_WAIT = 4
    };
}

msw::SelectMenu::CCupSelectTab::CCupSelectTab(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_nLevel = -1;
}

msw::SelectMenu::CCupSelectTab::~CCupSelectTab()
{
    Remove();
}

void msw::SelectMenu::CCupSelectTab::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in CCupSelect::Main().
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WIN, TAB_ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WIN, TAB_ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WIN, TAB_ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WIN, TAB_ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            if (m_nLevelNum != 0) {
                m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            }
            switch (m_nTabAnimState) {
                case TAB_ANIM_STATE_A:
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ANIM, TAB_ANIM_A_1, 1.0f, 0.0f, 1, 0);
                    m_nTabAnimState = TAB_ANIM_STATE_A_WAIT;
                    break;
                case TAB_ANIM_STATE_A_WAIT:
                    if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_ANIM, TAB_ANIM_A_1)) {
                        m_pPaneAccessor->PlayAnimation(TAB_PANE_ANIM, TAB_ANIM_A_2, 1.0f, 0.0f, 1, 0);
                        m_nTabAnimState = TAB_ANIM_STATE_NONE;
                    }
                    break;
                case TAB_ANIM_STATE_B:
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ANIM, TAB_ANIM_A_3, 1.0f, 0.0f, 1, 0);
                    m_nTabAnimState = TAB_ANIM_STATE_B_WAIT;
                    break;
                case TAB_ANIM_STATE_B_WAIT:
                    if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_ANIM, TAB_ANIM_A_3)) {
                        m_pPaneAccessor->PlayAnimation(TAB_PANE_ANIM, TAB_ANIM_A_0, 1.0f, 0.0f, 1, 0);
                        m_nTabAnimState = TAB_ANIM_STATE_NONE;
                    }
                    break;
            }
            break;
        case STATE_CLOSE: {
            // static-order swap, as above.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameOut(0)) {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WIN, TAB_ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(TAB_PANE_WIN, TAB_ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_OUT;
            break;
        }
        case STATE_OUT:
            // The game tests IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WIN, TAB_ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(TAB_PANE_WIN, TAB_ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
        default:
            // Nothing to update before Set().
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CCupSelectTab::Disp()
{
}

int msw::SelectMenu::CCupSelectTab::Set()
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
            ArchiveBuildLoader(s_TabButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_TabLayoutName);
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

            switch (m_nLevelNum) {
                case 0:
                    // One difficulty: a single tab, placed where the
                    // game kind wants it.
                    if (pSelectMenu->m_nGameKind == GAME_KIND_EXPERT) {
                        nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
                        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(TAB_PANE_ONE_POS);
                        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(TAB_PANE_ONE);
                        bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
                        pPane->SetTranslate(zero);
                    } else {
                        nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
                        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(TAB_PANE_ONE_POS);
                        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(TAB_PANE_ONE_B);
                        bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
                        pPane->SetTranslate(zero);
                    }
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_2, TAB_ANIM_VIS_OFF, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_3, TAB_ANIM_VIS_ON, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_2, TAB_ANIM_ROW_OFF, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_3, TAB_ANIM_ROW_OFF, 1.0f, 0.0f, 1, 1);
                    break;
                case 1:
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_2, TAB_ANIM_VIS_ON, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_3, TAB_ANIM_VIS_OFF, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_2, TAB_ANIM_ROW_ON, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_3, TAB_ANIM_ROW_OFF, 1.0f, 0.0f, 1, 1);
                    break;
                case 2:
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_2, TAB_ANIM_VIS_ON, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_ROW_3, TAB_ANIM_VIS_OFF, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_2, TAB_ANIM_ROW_OFF, 1.0f, 0.0f, 1, 1);
                    m_pPaneAccessor->PlayAnimation(TAB_PANE_TAB_3, TAB_ANIM_ROW_ON, 1.0f, 0.0f, 1, 1);
                    break;
            }

            m_pPaneAccessor->PlayAnimation(TAB_PANE_WIN, GetSelectMenu()->m_nGameKind + TAB_ANIM_WIN_C_00, 1.0f, 0.0f, 1, 1);
            m_nLevel = -1;
            m_pPaneAccessor->PlayAnimation(TAB_PANE_ANIM, TAB_ANIM_A_0, 1.0f, 0.0f, 1, 0);
            m_nTabAnimState = TAB_ANIM_STATE_NONE;
            m_bVibEnable = 1;
            m_bSeEnable = 1;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CCupSelectTab::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCupSelectTab::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    if (GetSelectMenu()->m_aPadSelect[nPad].m_Unk03 & 0x40) {
        return nResult;
    }
    if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
        return nResult;
    }

    const long *pTabPanes;
    long nTabNum;
    long nBtnKindId0;
    if (m_nLevelNum == 1) {
        pTabPanes = s_TabPanes2;
        nTabNum = 2;
        nBtnKindId0 = BTN_KIND_TAB2_0;
    } else if (m_nLevelNum == 2) {
        pTabPanes = s_TabPanes3;
        nTabNum = 3;
        nBtnKindId0 = BTN_KIND_TAB3_0;
    } else {
        return nResult;
    }

    for (long i = 0; i < nTabNum; i++) {
        long nBtnKindId = nBtnKindId0 + i;
        if (m_bTabPointed[i] == 0 && nBtnKindId == nResult) {
            if (i == m_nLevel) {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], TAB_ANIM_TAB_POINT_SEL, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], TAB_ANIM_TAB_POINT, 1.0f, 0.0f, 1, 0);
            }
            Vib_Start(1, 1, 1);
            SE_Play(SE_CURSOR, 1);
            m_bTabPointed[i] = 1;
        }
        if (m_bTabPointed[i] == 1 && nBtnKindId != nResult) {
            m_pPaneAccessor->StopAnimation(pTabPanes[i], TAB_ANIM_TAB_POINT_SEL, 1);
            m_pPaneAccessor->StopAnimation(pTabPanes[i], TAB_ANIM_TAB_POINT, 1);
            if (i == m_nLevel) {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], TAB_ANIM_TAB_LEAVE, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], TAB_ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
            }
            m_bTabPointed[i] = 0;
        }
    }
    return nResult;
}

long msw::SelectMenu::CCupSelectTab::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelectTab::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelectTab::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCupSelectTab::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCupSelectTab::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CCupSelectTab::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        if (m_nLevelNum == 1) {
            nBtnKindId = BTN_KIND_TAB2_0;
        } else if (m_nLevelNum == 2) {
            nBtnKindId = BTN_KIND_TAB3_0;
        }
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCupSelectTab::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCupSelectTab::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;
    switch (nEvent) {
        case EVENT_GET_LEVEL:
            nResult = m_nLevel;
            break;
        case EVENT_HIT:
            nResult = HitCheck(static_cast<bcs::FVEC3 *>(pParam));
            break;
        case EVENT_CLOSE:
            if (m_Unk30 < STATE_CLOSE) {
                m_Unk30 = STATE_CLOSE;
            }
            break;
        case EVENT_TAB_ANIM_A:
            if (m_nTabAnimState == TAB_ANIM_STATE_NONE) {
                m_nTabAnimState = TAB_ANIM_STATE_A;
                nResult = 0;
            }
            break;
        case EVENT_TAB_ANIM_B:
            if (m_nTabAnimState == TAB_ANIM_STATE_NONE) {
                m_nTabAnimState = TAB_ANIM_STATE_B;
                nResult = 0;
            }
            break;
        default:
            nResult = 0;
            break;
    }
    return nResult;
}

long msw::SelectMenu::CCupSelectTab::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}

int msw::SelectMenu::CCupSelectTab::SetDifficultySelect(long nLevel)
{
    long nPrevLevel = m_nLevel;
    if (nLevel == nPrevLevel) {
        return 0;
    }
    if (m_nLevelNum == 0) {
        m_nLevel = 0;
        return 0;
    }

    // The arrows are hidden at the first and the last difficulty.
    if (nLevel == 0 && m_bLeftArrow == 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_OFF, 1.0f, 0.0f, 1, 1);
        m_bLeftArrow = 1;
    } else if (m_bLeftArrow != 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_L, TAB_ANIM_ARROW_ON, 1.0f, 0.0f, 1, 1);
        m_bLeftArrow = 0;
    }
    if (nLevel == m_nLevelNum && m_bRightArrow == 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_OFF, 1.0f, 0.0f, 1, 1);
        m_bRightArrow = 1;
    } else if (m_bRightArrow != 0) {
        m_pPaneAccessor->PlayAnimation(TAB_PANE_YAJI_R, TAB_ANIM_ARROW_ON, 1.0f, 0.0f, 1, 1);
        m_bRightArrow = 0;
    }

    m_nLevel = nLevel;

    // Left unset (as in the game) when m_nLevelNum is above 2, which
    // Cup_In never sets.
    const long *pTabPane;
    if (m_nLevelNum == 1) {
        pTabPane = s_TabPanes2;
    } else if (m_nLevelNum == 2) {
        pTabPane = s_TabPanes3;
    }
    for (long i = 0; i <= m_nLevelNum; pTabPane++, i++) {
        m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TAB_POINT, 1);
        if (nLevel == i) {
            m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TAB_SELECT, 1.0f, 0.0f, 1, 0);
        } else if (nPrevLevel == i) {
            m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TAB_DESELECT, 1.0f, 0.0f, 1, 0);
        } else {
            m_pPaneAccessor->PlayAnimation(*pTabPane, TAB_ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
        }
    }

    if (CMenuPad::s_Instance.IsInputPointerMode(0)) {
        SE_Play(SE_NEXT, 1);
    }
    return 1;
}

long msw::SelectMenu::CCupSelectTab::GetCupBtnKindId(long nLevel)
{
    // The game returns 0 whenever there are tabs.
    long nBtnKindId = 0;
    if (m_nLevelNum == 0) {
        nBtnKindId = nLevel;
    }
    return nBtnKindId;
}

void msw::SelectMenu::CCupSelectTab::AllStopTabAnim()
{
    const long *pTabPane;
    if (m_nLevelNum == 1) {
        pTabPane = s_TabPanes2;
    } else if (m_nLevelNum == 2) {
        pTabPane = s_TabPanes3;
    } else {
        return;
    }

    for (long i = 0; i <= m_nLevelNum; pTabPane++, i++) {
        m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TAB_POINT_SEL, 1);
        m_pPaneAccessor->StopAnimation(*pTabPane, TAB_ANIM_TAB_POINT, 1);
        m_bTabPointed[i] = 0;
    }
}

// ---------------------------------------------------------------------------
// CLevelMessage

namespace {

    enum {
        MES_ANIM_IN,
        MES_ANIM_WAIT,
        MES_ANIM_OUT,
        MES_ANIM_HIDE,
        MES_ANIM_NUM
    };

    const char *s_MesAnimNames[MES_ANIM_NUM] = {
        "me_cup_select_mes_inout_00.brlan",
        "me_cup_select_mes_inout_01.brlan",
        "me_cup_select_mes_inout_02.brlan",
        "me_cup_select_mes_inout_03.brlan"
    };

    enum {
        MES_PANE_INOUT,
        MES_PANE_WIN,
        MES_PANE_WIN_00,
        MES_PANE_BAR_00,
        MES_PANE_BAR_01,
        MES_PANE_BAR_02,
        MES_PANE_RANK_POS,
        MES_PANE_NORMAL,
        MES_PANE_HARD,
        MES_PANE_EXPERT,
        MES_PANE_T_MES,
        MES_PANE_NUM
    };

    const char *s_MesPaneNames[MES_PANE_NUM] = {
        "inout", "win", "win_00", "bar_00", "bar_01", "bar_02",
        "rank_pos", "normal", "hard", "expert", "t_mes"
    };

    // Values of CLevelMessage's state (m_Unk30) beyond the base's.
    enum {
        STATE_CHANGE = 6           // the old text is moving out
    };
}

msw::SelectMenu::CLevelMessage::CLevelMessage(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CLevelMessage::~CLevelMessage()
{
    Remove();
}

void msw::SelectMenu::CLevelMessage::Main()
{
    if (m_pPaneAccessor == 0) {
        return;
    }

    switch (m_Unk30) {
        case STATE_SET: {
            m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_EXPERT, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_WIN, 1);
            CSelectMenu *pSelectMenu = GetSelectMenu();
            if (pSelectMenu->m_nGameKind == GAME_KIND_EXPERT) {
                m_pPaneAccessor->SetPaneDisp(MES_PANE_EXPERT, 1);
                const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_EXPART");  // the game's spelling
                m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
            } else {
                long nLevel = pSelectMenu->SendObjEventTrigger(OBJ_CUP_SELECT_TAB, EVENT_GET_LEVEL, 0);
                if (nLevel == 0) {
                    m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 1);
                    const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_NORMAL");
                    m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
                } else {
                    m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 1);
                    const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_HARD");
                    m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
                }
                m_nLevel = nLevel;
            }
            m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(MES_PANE_INOUT, MES_ANIM_IN)) {
                m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_WAIT, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            break;
        case STATE_CHANGE:
            if (m_pPaneAccessor->IsAnimEnd(MES_PANE_INOUT, MES_ANIM_OUT)) {
                m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 0);
                m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 0);
                m_pPaneAccessor->SetPaneDisp(MES_PANE_EXPERT, 0);
                if (m_nLevel == 0) {
                    m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 1);
                    const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_NORMAL");
                    m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
                } else {
                    m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 1);
                    const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_HARD");
                    m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
                }
                m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_IN, 1.0f, 0.0f, 1, 1);
                m_Unk30 = STATE_IN;
            }
            break;
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_OUT, s_WinAnimSpeed[0], 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(MES_PANE_INOUT, MES_ANIM_OUT)) {
                m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 0);
                m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 0);
                m_pPaneAccessor->SetPaneDisp(MES_PANE_EXPERT, 0);
                m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_HIDE, s_WinAnimSpeed[0], 0.0f, 1, 1);
                m_Unk30 = STATE_NONE;
            }
            break;
    }
}

int msw::SelectMenu::CLevelMessage::Set()
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->BuildLayout(0, "me_cup_select_mes.brlyt");
            pPaneCtrl->BuildAnim(0, s_MesAnimNames, MES_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_MesPaneNames, MES_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            m_pPaneAccessor->SetPaneDisp(MES_PANE_NORMAL, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_HARD, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_EXPERT, 0);
            m_pPaneAccessor->SetPaneDisp(MES_PANE_WIN, 0);
            const wchar_t *pText = GetMessageLabelString("List_Intro_Text", "INTRO_LV_NORMAL");
            m_pPaneAccessor->SetTextBoxString(MES_PANE_T_MES, pText, wcslen(pText));
            m_Unk30 = STATE_NONE;
        }
    }
    return 0;
}

void msw::SelectMenu::CLevelMessage::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
}

long msw::SelectMenu::CLevelMessage::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;
    if (m_pPaneAccessor == 0) {
        return nResult;
    }

    switch (nEvent) {
        case EVENT_OPEN:
            if (m_Unk30 == STATE_CLOSE || m_Unk30 == STATE_OUT || m_Unk30 == STATE_NONE) {
                m_pPaneAccessor->StopAnimation(MES_PANE_INOUT, MES_ANIM_OUT, 1);
                m_Unk30 = STATE_SET;
            }
            break;
        case EVENT_CLOSE:
            if (m_Unk30 == STATE_SET || m_Unk30 == STATE_IN || m_Unk30 == STATE_ACTIVE) {
                m_pPaneAccessor->StopAnimation(MES_PANE_INOUT, MES_ANIM_IN, 1);
                m_Unk30 = STATE_CLOSE;
            }
            break;
        case EVENT_LEVEL_CHANGED:
            if (m_Unk30 == STATE_NONE) {
                m_Unk30 = STATE_SET;
            } else {
                long nLevel = GetSelectMenu()->SendObjEventTrigger(OBJ_CUP_SELECT_TAB, EVENT_GET_LEVEL, 0);
                if (nLevel != m_nLevel) {
                    m_nLevel = nLevel;
                    m_pPaneAccessor->PlayAnimation(MES_PANE_INOUT, MES_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                    m_Unk30 = STATE_CHANGE;
                }
            }
            break;
        default:
            nResult = 0;
            break;
    }
    return nResult;
}

void msw::SelectMenu::CLevelMessage::Disp()
{
}
