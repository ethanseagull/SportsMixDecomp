#include "game/msw/SelectMenu/CPartyInfo.hpp"
#include "game/msw/SelectMenu/CPartyHelpPage.hpp"

#include <wchar.h>

#include "bcs/CMainApp.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "nw4r/lyt/Pane.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar, as in game/menu/CRuleInfo.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_PARTY_INFO = 0x1A,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_40 = 0x40,
        OBJ_HELP_PAGE_0 = 0x47, // one help page per party game, to 0x4A
        OBJ_HELP_PAGE_1 = 0x48,
        OBJ_HELP_PAGE_2 = 0x49,
        OBJ_HELP_PAGE_3 = 0x4A,
        OBJ_UNK_4B = 0x4B
    };

    // The button kind ids of the screen: the back button, the tabs (one
    // set for each number of tabs: three, two, four) and the help window.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_TAB3_0 = 0x8010,
        BTN_KIND_TAB3_2 = 0x8012,
        BTN_KIND_TAB2_0 = 0x8020,
        BTN_KIND_TAB2_1 = 0x8021,
        BTN_KIND_TAB4_0 = 0x8030,
        BTN_KIND_TAB4_3 = 0x8033,
        BTN_KIND_HELP = 0x8050
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SELECT = 0
    };

    // The mode the help window leads to.
    enum {
        MODE_UNK_9 = 9
    };

    // Frames the screen waits after a tab change.
    const float TAB_WAIT = 20.0f;
}

void msw::CSelectMenu::PartyInfo_In()
{
    static const long padNo0 = 0;

    m_Unk9A = 1;
    Build(OBJ_PARTY_INFO);
    SendObjEventTrigger(OBJ_TOP, 0x11, (void *)GetMessageLabelString("PartyMenuMessage", "PAR_01"));
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PARTY_INFO);
    if (m_Unk9D == 9) {
        CCameraMenuBG::s_Instance.Play(0x29, 1.0f, 0.0f);
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
        SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
    }

    m_fInputWait = 0.0f;
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::PartyInfo_Loop()
{
    switch (m_nStep) {
        case STEP_SELECT:
            PartyInfo_Loop0();
            break;
    }
}

void msw::CSelectMenu::PartyInfo_Loop0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    SelectMenu::CPartyInfo *pObj = (SelectMenu::CPartyInfo *)FindObj(OBJ_PARTY_INFO);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait > 0.0f) {
        return;
    }
    m_fInputWait = 0.0f;

    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    unsigned short nBtnKindId = rBtnKindId;
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->PushButton(padNo0, nBtnKindId);
        if (m_Unk23E == 1) {
            SetWhiteFadeOut(60.0f);
            SetNextMode(MODE_UNK_9, pObj);
            SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
            return;
        }
        switch (nBtnKindId) {
            case BTN_KIND_TAB4_0:
            case BTN_KIND_TAB4_0 + 1:
            case BTN_KIND_TAB4_0 + 2:
            case BTN_KIND_TAB4_3:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB4_0);
                m_fInputWait = TAB_WAIT;
                break;
            case BTN_KIND_TAB3_0:
            case BTN_KIND_TAB3_0 + 1:
            case BTN_KIND_TAB3_2:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB3_0);
                m_fInputWait = TAB_WAIT;
                break;
            case BTN_KIND_TAB2_0:
            case BTN_KIND_TAB2_1:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB2_0);
                m_fInputWait = TAB_WAIT;
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_HELP:
                SetNextMode(MODE_UNK_9, pObj);
                SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        long nNewBtnKindId = rBtnKindId;
        if (m_Unk23E == 1) {
            // A pad: the pointer's highlight goes, a moved cursor lands on
            // the help window, and left and right change the tab.
            pObj->SelectAnimAllOff();
            if (nNewBtnKindId != nBtnKindId) {
                rBtnKindId = BTN_KIND_HELP;
                pObj->m_nPushButtonId[padNo0] = BTN_KIND_HELP;
                pObj->SetCursorMarkerPosition(padNo0, BTN_KIND_HELP);
            }
            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
                pObj->SetTabSelect(pObj->m_nTabSelect - 1);
                m_fInputWait = TAB_WAIT;
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
                pObj->SetTabSelect(pObj->m_nTabSelect + 1);
                m_fInputWait = TAB_WAIT;
            }
        }
    }
}

void msw::CSelectMenu::PartyInfo_ChangeStateInit(long)
{
}

void msw::CSelectMenu::PartyInfo_Loop1()
{
}

void msw::CSelectMenu::PartyInfo_Out()
{
    m_Unk9A = 0;
    CCameraMenuBG::s_Instance.ClearList();
    Scratch(OBJ_PARTY_INFO);
    Scratch(OBJ_UNK_4B);
    Scratch(OBJ_HELP_PAGE_0);
    Scratch(OBJ_HELP_PAGE_1);
    Scratch(OBJ_HELP_PAGE_2);
    Scratch(OBJ_HELP_PAGE_3);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because they follow those steps' constants in .rodata.
namespace {

    const char *const s_LayoutName = "me_ptg_operation_w.brlyt";
    const char *const s_ButtonFileName = "me_ptg_operation_w.btn";

    // The panes of the screen.
    enum {
        PANE_ROOT = 0,
        PANE_CTRL_ICON_0 = 2,    // the three control icons, lit by tab count
        PANE_CTRL_ICON_1 = 0xB,
        PANE_ARROW_LEFT = 0x11,
        PANE_ARROW_RIGHT = 0x12,
        PANE_CTRL_ICON_2 = 0x19,
        PANE_CTRL_FIRST = 0x2C,  // the controls drawn for each pad style
        PANE_CTRL_STYLE1_0 = 0x2C,
        PANE_CTRL_STYLE1_1 = 0x2D,
        PANE_CTRL_STYLE0_0 = 0x2E,
        PANE_CTRL_STYLE0_1 = 0x2F,
        PANE_CTRL_LAST = 0x31
    };

    // The animations of the screen.
    enum {
        ANIM_ARROW_ON = 0,
        ANIM_WIN_IN = 7,
        ANIM_WIN_OUT = 9,
        ANIM_WIN_IN_BACK = 0xA,
        ANIM_WIN_OUT_BACK = 0xB,
        ANIM_TAB_OFF = 0xC,
        ANIM_TAB_HIT_ON = 0xD,
        ANIM_TAB_SELECT = 0xE,
        ANIM_TAB_HIT_OFF_SELECTED = 0xF,
        ANIM_TAB_HIT_ON_SELECTED = 0x10,
        ANIM_TAB_UNSELECT = 0x11,
        ANIM_OFF = 0x12,
        ANIM_ON = 0x14
    };

    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,   // plus the pad number
        EVENT_CLOSE = 0x2D,
        EVENT_TAB = 0x31,    // the help pages: the selected tab
        EVENT_TAB_2 = 0x32
    };

    // The number of tabs of each party game (CSelectMenu::m_Unk9C: 1, 3,
    // 5 or 7).
    const long s_TabNum[10] = { -1, 4, -1, 3, -1, 3, -1, 3, -1, 0 };
    // The tab panes and the panes of their titles, for two, three and four
    // tabs.
    const long s_TabPanes[3][4] = {
        { 0xC, 0xF, -1, -1 },
        { 4, 7, 0x13, -1 },
        { 0x1B, 0x1D, 0x1F, 0x21 }
    };
    const long s_TabTitles[3][4] = {
        { 0x17, 0x18, -1, -1 },
        { 0x14, 0x15, 0x16, -1 },
        { 0x26, 0x27, 0x28, 0x29 }
    };
    // The control panes of each party game, and the panes Set() puts them
    // under (the first set for the first game, the second for the others).
    const long s_CtrlPanes0[6] = { 0x30, 0x31, 0x2F, 0x2E, 0x2D, 0x2C };
    const long s_CtrlPanes1[6] = { 0x30, 0x31, 0x2E, 0x2C, -1, -1 };
    const long s_CtrlPanes2[6] = { 0x30, 0x31, 0x2E, 0x2C, -1, -1 };
    const long s_CtrlPanes3[6] = { 0x30, 0x31, 0x2E, 0x2C, -1, -1 };
    const long s_CtrlParents[2][6] = {
        { 0x22, 0x23, 0x24, 0x25, 0x24, 0x25 },
        { 6, 9, 0xA, 0xA, -1, -1 }
    };
}

msw::SelectMenu::CPartyInfo::CPartyInfo(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartyInfo::~CPartyInfo()
{
    Remove();
}

void msw::SelectMenu::CPartyInfo::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap (notes/decisions.md): the game numbers
            // these recursive, reset@0, recursive@1, reset@2 and stores
            // them in the reverse order; declared this way the bytes are
            // the original's and the names swapped.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            DispControlType();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_4B, EVENT_CLOSE, 0);
                GetSelectMenu()->SendObjEventTrigger(m_nHelpPageObj, EVENT_CLOSE, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CPartyInfo::Disp()
{
}

int msw::SelectMenu::CPartyInfo::Set()
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            // Light the control icon of the number of tabs.
            long nTabNum = s_TabNum[pSelectMenu->m_Unk9C];
            if (nTabNum == 4) {
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_2, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_0, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_1, ANIM_OFF, 1.0f, 0.0f, 1, 0);
            } else if (nTabNum == 3) {
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_2, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_0, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_1, ANIM_OFF, 1.0f, 0.0f, 1, 0);
            } else if (nTabNum == 2) {
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_2, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_0, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_CTRL_ICON_1, ANIM_ON, 1.0f, 0.0f, 1, 0);
            }

            for (long i = PANE_CTRL_FIRST; i <= PANE_CTRL_LAST; i++) {
                m_pPaneAccessor->SetPaneDisp(i, 0);
            }

            // The help page of the party game.
            pSelectMenu->Build(OBJ_UNK_4B);
            const long *pCtrlPanes = 0;
            long nParentSet;
            switch (pSelectMenu->m_Unk9C) {
                case 1:
                    m_nHelpPageObj = OBJ_HELP_PAGE_0;
                    pCtrlPanes = s_CtrlPanes0;
                    nParentSet = 0;
                    break;
                case 3:
                    m_nHelpPageObj = OBJ_HELP_PAGE_1;
                    pCtrlPanes = s_CtrlPanes1;
                    nParentSet = 1;
                    break;
                case 5:
                    m_nHelpPageObj = OBJ_HELP_PAGE_2;
                    pCtrlPanes = s_CtrlPanes2;
                    nParentSet = 1;
                    break;
                case 7:
                    m_nHelpPageObj = OBJ_HELP_PAGE_3;
                    pCtrlPanes = s_CtrlPanes3;
                    nParentSet = 1;
                    break;
            }
            pSelectMenu->Build(m_nHelpPageObj);

            // Move the game's control panes under their places and show
            // them.
            for (long i = 0; i < 6; i++) {
                long nParent = s_CtrlParents[nParentSet][i];
                long nPane = pCtrlPanes[i];
                if (nParent != -1) {
                    nw4r::math::VEC3 trans(0.0f, 0.0f, 0.0f);
                    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
                    bcs::G2D::SetPaneParent(pPane, m_pPaneAccessor->GetPane(nParent), 1, 0);
                    pPane->SetTranslate(trans);
                    m_pPaneAccessor->SetPaneDisp(nPane, 1);
                }
            }

            m_nPadStyle = -1;
            DispControlType();
            pSelectMenu->m_nCurrentBtnKindId[0][pSelectMenu->m_nMode] = BTN_KIND_HELP;
            m_nPushButtonId[0] = BTN_KIND_HELP;
            SetCursorMarkerPosition(0, BTN_KIND_HELP);
            m_nTabSelect = -1;
            SetTabSelect(0);

            m_bVibEnable = 1;
            m_bSeEnable = 1;
        }
    }
    return 0;
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it (as in game/menu/CRuleInfo.cpp). Defined
// down here because its local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CPartyInfo::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CPartyInfo::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartyInfo::SendEvent(long, void *)
{
    return -1;
}

long msw::SelectMenu::CPartyInfo::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);

    // The pointer's highlight on the tabs.
    long nTabNum = s_TabNum[GetSelectMenu()->m_Unk9C];
    long nSet;
    long nFirstBtnKindId;
    if (nTabNum == 4) {
        nSet = 2;
        nFirstBtnKindId = BTN_KIND_TAB4_0;
    } else if (nTabNum == 3) {
        nSet = 1;
        nFirstBtnKindId = BTN_KIND_TAB3_0;
    } else if (nTabNum == 2) {
        nSet = 0;
        nFirstBtnKindId = BTN_KIND_TAB2_0;
    }
    const long *pTabPanes = s_TabPanes[nSet];
    for (long i = 0; i < nTabNum; i++) {
        long nBtnKindId = nFirstBtnKindId + i;
        if (m_bTabHit[i] == 0 && nBtnKindId == nResult) {
            if (i == m_nTabSelect) {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], ANIM_TAB_HIT_ON_SELECTED, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], ANIM_TAB_HIT_ON, 1.0f, 0.0f, 1, 0);
            }
            m_bTabHit[i] = 1;
        }
        if (m_bTabHit[i] == 1 && nBtnKindId != nResult) {
            m_pPaneAccessor->StopAnimation(pTabPanes[i], ANIM_TAB_HIT_ON, 1);
            m_pPaneAccessor->StopAnimation(pTabPanes[i], ANIM_TAB_HIT_ON_SELECTED, 1);
            if (i == m_nTabSelect) {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], ANIM_TAB_HIT_OFF_SELECTED, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(pTabPanes[i], ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
            }
            m_bTabHit[i] = 0;
        }
    }

    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(1, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CPartyInfo::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartyInfo::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartyInfo::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartyInfo::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPartyInfo::StickMoveCheck(long nPad, long nBtnKindId)
{
    // From a button the stick does not move, start at the help window.
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_HELP;
    }
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CPartyInfo::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_HELP;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CPartyInfo::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CPartyInfo::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

void msw::SelectMenu::CPartyInfo::SetTabSelect(long nTab)
{
    long nPrev = m_nTabSelect;
    if (nTab == nPrev) {
        return;
    }
    long nTabNum = s_TabNum[GetSelectMenu()->m_Unk9C];
    if (nTab >= nTabNum || nTab < 0) {
        return;
    }

    if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
        if (nTab > nPrev) {
            SE_Play(0xBCD, 1);
        } else {
            SE_Play(0xBCE, 1);
        }
    } else {
        SE_Play(0xBCD, 1);
    }

    const long *pTabPanes = s_TabPanes[nTabNum - 2];
    const long *pTitles = s_TabTitles[nTabNum - 2];
    m_nTabSelect = nTab;
    for (long i = 0; i < nTabNum; i++) {
        m_pPaneAccessor->StopAnimation(pTabPanes[i], ANIM_TAB_HIT_ON, 1);
        if (nTab == i) {
            m_pPaneAccessor->PlayAnimation(pTabPanes[nTab], ANIM_TAB_SELECT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[nTab], 1);
        } else if (nPrev == i) {
            m_pPaneAccessor->PlayAnimation(pTabPanes[nPrev], ANIM_TAB_UNSELECT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[nPrev], 0);
        } else {
            m_pPaneAccessor->PlayAnimation(pTabPanes[i], ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[i], 0);
        }
    }

    // The arrows: off at either end.
    if (nTab + 1 >= nTabNum) {
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_LEFT, ANIM_ARROW_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_LEFT, ANIM_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->StopAnimation(PANE_ARROW_RIGHT, ANIM_ARROW_ON, 1);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_RIGHT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
    } else if (nTab - 1 < 0) {
        m_pPaneAccessor->StopAnimation(PANE_ARROW_LEFT, ANIM_ARROW_ON, 1);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_LEFT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_RIGHT, ANIM_ARROW_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_RIGHT, ANIM_ON, 1.0f, 0.0f, 1, 0);
    } else {
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_LEFT, ANIM_ARROW_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_RIGHT, ANIM_ARROW_ON, 1.0f, 0.0f, 1, 0);
    }

    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_4B, EVENT_TAB, &m_nTabSelect);
    GetSelectMenu()->SendObjEventTrigger(m_nHelpPageObj, EVENT_TAB, &m_nTabSelect);
    GetSelectMenu()->SendObjEventTrigger(m_nHelpPageObj, EVENT_TAB_2, &m_nTabSelect);
}

void msw::SelectMenu::CPartyInfo::DispControlType()
{
    CMenuPad &rPad = CMenuPad::s_Instance;
    if (m_nPadStyle == rPad.GetPadStyle(0)) {
        return;
    }

    // The game hides and shows two of the panes twice.
    if (rPad.GetPadStyle(0) == 1) {
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_0, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_0, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_1, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_0, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_0, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_1, 1);
    } else {
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_0, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_0, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE0_1, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_0, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_0, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_CTRL_STYLE1_1, 0);
    }
    m_nPadStyle = rPad.GetPadStyle(0);
}

void msw::SelectMenu::CPartyInfo::SelectAnimAllOff()
{
    long nTabNum = s_TabNum[GetSelectMenu()->m_Unk9C];
    long nSet;
    if (nTabNum == 4) {
        nSet = 2;
    } else if (nTabNum == 3) {
        nSet = 1;
    } else if (nTabNum == 2) {
        nSet = 0;
    }
    const long *pTabPanes = s_TabPanes[nSet];
    for (long i = 0; i < nTabNum; i++) {
        m_pPaneAccessor->StopAnimation(pTabPanes[i], ANIM_TAB_HIT_ON, 1);
        m_pPaneAccessor->StopAnimation(pTabPanes[i], ANIM_TAB_HIT_ON_SELECTED, 1);
        m_bTabHit[i] = 0;
    }
}

// The names in this namespace are guesses. Defined here because they follow
// CPartyInfo's constants in .rodata.
namespace {

    const char *const s_HelpLayoutName = "me_ptg_operation_1P.brlyt";
    const char *const s_HelpButtonFileName = "me_ptg_operation_1P.btn";

    // The movie of each party game, by CSelectMenu::m_Unk9C.
    const char *s_HelpMovieNames[8] = {
        "movie/per_exp_70_BA.thp",
        "movie/per_exp_70_BA.thp",
        "movie/per_exp_40_VO.thp",
        "movie/per_exp_40_VO.thp",
        "movie/per_exp_55_DO.thp",
        "movie/per_exp_55_DO.thp",
        "movie/per_exp_85_A.thp",
        "movie/per_exp_85_A.thp"
    };

    // The panes of the help page.
    enum {
        HELP_PANE_ROOT = 0,
        HELP_PANE_MOVIE = 1,     // the movie is shown in it
        HELP_PANE_TEXT_0 = 2,    // the controls text
        HELP_PANE_PAGE_1 = 3,
        HELP_PANE_TEXT_1 = 5
    };

    // The animations of the help page.
    enum {
        HELP_ANIM_PAGE_IN = 0,
        HELP_ANIM_WIN_IN = 1,
        HELP_ANIM_PAGE_ON = 2,   // a page that is already shown
        HELP_ANIM_WIN_OUT = 3,
        HELP_ANIM_WIN_IN_BACK = 4,
        HELP_ANIM_WIN_OUT_BACK = 5,
        HELP_ANIM_PAGE_SHOW = 6,
        HELP_ANIM_PAGE_HIDE = 7,
        HELP_ANIM_MOVIE_IN = 8
    };

    const unsigned char MOVIE_PANE_ARG = 3;

    // Frames before the movie is shown.
    const float MOVIE_WAIT = 30.0f;
}

msw::SelectMenu::CPartyHelpPage::CPartyHelpPage(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_nMovieState = MOVIE_NONE;
}

msw::SelectMenu::CPartyHelpPage::~CPartyHelpPage()
{
    Remove();
}

void msw::SelectMenu::CPartyHelpPage::Main()
{
    MovieControl();

    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in CPartyInfo::Main.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(HELP_PANE_ROOT, HELP_ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(HELP_PANE_ROOT, HELP_ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            DispControlType();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(m_nOutPane, HELP_ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(m_nOutPane, HELP_ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_nMovieState = MOVIE_NONE;
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(m_nOutPane, HELP_ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(m_nOutPane, HELP_ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CPartyHelpPage::Set()
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
            ArchiveBuildLoader(s_HelpButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_HelpLayoutName);
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
            m_nMovieState = MOVIE_STOP;
            m_nPadStyle = -1;
            DispControlType();

            // The first page is shown; the movie waits.
            m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_PAGE_SHOW, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(HELP_PANE_PAGE_1, HELP_ANIM_PAGE_IN, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->SetPaneDisp(HELP_PANE_MOVIE, 0);
            m_bMovieWait = 1;
            m_fMovieWait = MOVIE_WAIT;
            m_bPageOn[0] = 1;
            m_bPageOn[1] = 0;
            m_nOutPane = HELP_PANE_ROOT;
        }
    }
    return 0;
}

void msw::SelectMenu::CPartyHelpPage::Remove()
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

long msw::SelectMenu::CPartyHelpPage::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_TAB:
            // The game tests the address of the array, not its first
            // element.
            if (m_bPageOn != 0) {
                switch (*(long *)pParam) {
                    case 0:
                        if (m_bPageOn[0] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_PAGE_ON, 1.0f, 0.0f, 1, 0);
                        } else {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_PAGE_SHOW, 1.0f, 0.0f, 1, 0);
                        }
                        if (m_bPageOn[1] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_PAGE_1, HELP_ANIM_PAGE_HIDE, 1.0f, 0.0f, 1, 0);
                        }
                        m_bPageOn[0] = 1;
                        m_bPageOn[1] = 0;
                        m_nOutPane = HELP_PANE_ROOT;
                        break;
                    case 1:
                        if (m_bPageOn[0] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_PAGE_HIDE, 1.0f, 0.0f, 1, 0);
                        }
                        if (m_bPageOn[1] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_PAGE_1, HELP_ANIM_PAGE_ON, 1.0f, 0.0f, 1, 0);
                        } else {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_PAGE_1, HELP_ANIM_PAGE_SHOW, 1.0f, 0.0f, 1, 0);
                        }
                        m_bPageOn[0] = 0;
                        m_bPageOn[1] = 1;
                        m_nOutPane = HELP_PANE_PAGE_1;
                        break;
                    default:
                        if (m_bPageOn[0] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_ROOT, HELP_ANIM_PAGE_HIDE, 1.0f, 0.0f, 1, 0);
                        }
                        if (m_bPageOn[1] == 1) {
                            m_pPaneAccessor->PlayAnimation(HELP_PANE_PAGE_1, HELP_ANIM_PAGE_HIDE, 1.0f, 0.0f, 1, 0);
                        }
                        m_bPageOn[0] = 0;
                        m_bPageOn[1] = 0;
                        m_nOutPane = HELP_PANE_ROOT;
                        break;
                }
            }
            break;
        case EVENT_CLOSE:
            m_Unk30 = STATE_CLOSE;
            break;
    }
    return -1;
}

void msw::SelectMenu::CPartyHelpPage::MovieControl()
{
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie == 0) {
        return;
    }

    switch (m_nMovieState) {
        case MOVIE_STOP:
            m_nMovieState = MOVIE_OPEN;
            pMovie->Stop();
            break;
        case MOVIE_OPEN:
            if (pMovie->IsWait() == 1 && pMovie->Open(s_HelpMovieNames[GetSelectMenu()->m_Unk9C], 1, 0) == 1) {
                m_nMovieState = MOVIE_START;
            }
            break;
        case MOVIE_START:
            if (pMovie->IsReady() && pMovie->Start(0)) {
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
                nw4r::lyt::Pane *pPane = bcs::G2D::CreatePicturePane("moviePic_01", MOVIE_PANE_ARG);
                if (pPane == 0) {
                    break;
                }
                bcs::G2D::CPaneObj *pPaneObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPane);
                if (pPaneObj == 0) {
                    break;
                }

                bcs::G2D::CPaneObj *pFrame = m_pPaneAccessor->GetPaneObj(HELP_PANE_MOVIE);

                pPaneCtrl->AppendPaneObj(pPaneObj, (const char *)0);
                bcs::G2D::SetPaneParent(pPaneObj->GetPane(), pFrame->GetPane(), 0, 0);
                pPaneObj->GetPane()->SetSize(pFrame->GetPane()->GetSize());
                bcs::G2D::SetupMoviePane(pPaneObj, pFrame);
                m_nMovieState = MOVIE_PLAY;
            }
            break;
        case MOVIE_PLAY:
            bcs::G2D::RemapMoviePane(m_pPaneAccessor->GetPane("moviePic_01", 1, 0));
            if (pMovie->CanbeDraw() && m_bMovieWait != 0) {
                m_fMovieWait -= bcs::CMainApp::GetAppFrameRateF();
                if (m_fMovieWait < 0.0f) {
                    m_pPaneAccessor->SetPaneDisp(HELP_PANE_MOVIE, 1);
                    m_pPaneAccessor->PlayAnimation(HELP_PANE_MOVIE, HELP_ANIM_MOVIE_IN, 1.0f, 0.0f, 1, 1);
                    m_bMovieWait = 0;
                }
            }
            break;
    }
}

void msw::SelectMenu::CPartyHelpPage::DispControlType()
{
    CMenuPad &rPad = CMenuPad::s_Instance;
    CSelectMenu *pSelectMenu = GetSelectMenu();
    if (m_nPadStyle == rPad.GetPadStyle(0)) {
        return;
    }

    const wchar_t *pText0;
    const wchar_t *pText1;
    if (rPad.GetPadStyle(0) == 1) {
        switch (pSelectMenu->m_Unk9C) {
            case 1:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_02");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_02_01");
                break;
            case 3:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_00");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_00_01");
                break;
            case 5:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_03");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_03_01");
                break;
            case 7:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_01");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_01_01_01");
                break;
        }
    } else {
        switch (pSelectMenu->m_Unk9C) {
            case 1:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_02");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_02_01");
                break;
            case 3:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_00");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_00_01");
                break;
            case 5:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_03");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_03_01");
                break;
            case 7:
                pText0 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_01");
                pText1 = GetMessageLabelString("PartyMenuMessage", "PAR_TUT_06_01_01");
                break;
        }
    }
    m_pPaneAccessor->SetTextBoxString(HELP_PANE_TEXT_0, pText0, wcslen(pText0));
    m_pPaneAccessor->SetTextBoxString(HELP_PANE_TEXT_1, pText1, wcslen(pText1));
    m_nPadStyle = rPad.GetPadStyle(0);
}

