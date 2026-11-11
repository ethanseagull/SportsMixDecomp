#include "game/msw/SelectMenu/CFriendList.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CFriendCode.cpp: the original converts
    // the value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_PLUS_MINUS = 2,   // the "+/-" scroll help (SelectMenu::CPlusMinus)
        OBJ_FRIEND_LIST = 0x28
    };

    // What the objects are sent.
    enum {
        EVENT_LINE = 1,               // to OBJ_TOP: a help line of its own
        EVENT_LINE_MESSAGE = 0x11,    // to OBJ_TOP: the help line
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21,            // plus the pad number
        EVENT_PLUS_MINUS_OUT = 0x25,  // to OBJ_PLUS_MINUS: close
        EVENT_PLUS_MINUS_READY = 0x26 // to OBJ_PLUS_MINUS: may the window close
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_SLIDER = 0x8010,   // the slider bar
        BTN_KIND_UP = 0x8011,       // scroll buttons
        BTN_KIND_DOWN = 0x8012,
        BTN_KIND_ROW_0 = 0x8020,    // to 0x8024, one per row
        BTN_KIND_ROW_3 = 0x8023,
        BTN_KIND_ROW_4 = 0x8024,
        BTN_KIND_CODE = 0x80A1      // to the friend code entry
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_UP = 0x01,
        PAD_DOWN = 0x04,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_PLUS = 0x40,
        PAD_HOLD = 0x100   // the pointer's button is held
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SELECT = 0
    };

    // The modes the screen leads to (CSelectMenu::m_nMode).
    enum {
        MODE_NO_CONNECTION = 4,
        MODE_FRIEND_CODE = 0x23,
        MODE_PLAYER_INFO = 0x25,   // the player's own row
        MODE_FRIEND_INFO = 0x26,   // a registered friend
        MODE_NO_FRIEND_INFO = 0x27, // a friend who has not registered back
        MODE_FRIEND_VS = 0x2A
    };

    // The cockpit object the pointer position is asked from, and the
    // request.
    enum {
        COCKPIT_OBJ_POINTER = 0x1016,
        COCKPIT_EVENT_GET_POS = 0x2711
    };
}

void msw::CSelectMenu::FriendList_In()
{
    // `objId` is stored before `padNo0`; the game numbers them the other
    // way round, so only the two symbol names are exchanged: static-order
    // swap (see notes/decisions.md).
    static const long objId = OBJ_FRIEND_LIST;
    static const long padNo0 = 0;

    m_pPlayerData = CSave::GetPlayerData(-1);
    if (!m_pPlayerData->IsValid()) {
        SetNextMode(MODE_NO_CONNECTION, 0.0f);
        return;
    }

    dwc::CGameDwc *pGameDwc = dwc::CGameDwc::GetInstance();
    if (pGameDwc == 0 || !pGameDwc->m_Dwc.IsConnect()) {
        SetNextMode(MODE_NO_CONNECTION, 0.0f);
        return;
    }

    m_pFriendList = pGameDwc->m_Dwc.GetFriendList();
    SelectMenu::CFriendList *pObj = (SelectMenu::CFriendList *)FindObj(objId);
    // Back from one of the screens the rows lead to, the list stays where
    // it was. (The game does not check pObj here.)
    if (m_Unk9D < MODE_PLAYER_INFO || m_Unk9D > MODE_NO_FRIEND_INFO) {
        pObj->m_nTopIndex = 0;
    }

    Build(objId);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->m_nCursorBtnKindId = nBtnKindId;
    }

    m_fInputWait = 0.0f;
    m_nStep = STEP_SELECT;
    memset(m_FriendListLineName, 0, sizeof(m_FriendListLineName));
    FriendList_LineMessage(-1);
}

void msw::CSelectMenu::FriendList_Loop()
{
    switch (m_nStep) {
        case STEP_SELECT:
            FriendList_LoopStep();
            break;
    }
}

void msw::CSelectMenu::FriendList_LoopStep()
{
    // static-order swap, as in FriendList_In().
    static const long objId = OBJ_FRIEND_LIST;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    SelectMenu::CFriendList *pObj = (SelectMenu::CFriendList *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait <= 0.0f) {
        m_fInputWait = 0.0f;
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_HOLD) {
        switch (nBtnKindId) {
            case BTN_KIND_SLIDER:
                pObj->m_bSliderDrag = 1;
                break;
            case BTN_KIND_UP:
                pObj->SetMoveButton(1, 1.0f, 1);
                break;
            case BTN_KIND_DOWN:
                pObj->SetMoveButton(0, 1.0f, 1);
                break;
        }
        if (pObj->m_bSliderDrag) {
            // The bar follows the pointer, within its range.
            bcs::FVEC3 pos;
            CCockpit::SendObjEventTrigger(COCKPIT_OBJ_POINTER, COCKPIT_EVENT_GET_POS, &pos);
            float fY = pos.y;
            if (pObj->m_fSliderTop < fY) {
                fY = pObj->m_fSliderTop;
            }
            if (pObj->m_fSliderBottom > fY) {
                fY = pObj->m_fSliderBottom;
            }
            bcs::G2D::CPaneObj *pBar = pObj->m_pPaneAccessor->GetPaneObj(SelectMenu::CFriendList::PANE_SLIDER_BAR);
            bcs::G2D::CPaneObj *pBarLight =
                pObj->m_pPaneAccessor->GetPaneObj(SelectMenu::CFriendList::PANE_SLIDER_BAR_LIGHT);
            pObj->m_SliderPos.y = fY;
            pBar->GetPane()->SetTranslate(pObj->m_SliderPos);
            pBarLight->GetPane()->SetTranslate(pObj->m_SliderPos);
            return;
        }
    } else if (pObj->m_bSliderDrag) {
        // Released: the list follows the bar.
        pObj->MoveSliderBarPointer(1);
        pObj->m_bSliderDrag = 0;
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        // The result is not used.
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_ROW_0:
            case BTN_KIND_ROW_0 + 1:
            case BTN_KIND_ROW_0 + 2:
            case BTN_KIND_ROW_3:
            case BTN_KIND_ROW_4: {
                pObj->PushButton(padNo0, nBtnKindId);
                long nRow = nBtnKindId - BTN_KIND_ROW_0;
                if (pObj->m_nTopIndex == 0 && nBtnKindId == BTN_KIND_ROW_0) {
                    // The player's own row.
                    SetNextMode(MODE_PLAYER_INFO, pObj);
                    m_Unk708 = 0;
                } else if (pObj->m_nRowKind[nRow] == 1) {
                    SetNextMode(MODE_FRIEND_INFO, pObj);
                    m_Unk708 = pObj->m_pRowFriend[nRow];
                } else {
                    SetNextMode(MODE_NO_FRIEND_INFO, pObj);
                    m_Unk708 = pObj->m_pRowFriend[nRow];
                }
                break;
            }
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_CODE:
                pObj->PushButton(padNo0, BTN_KIND_CODE);
                SetNextMode(MODE_FRIEND_CODE, pObj);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_PLUS) {
        pObj->PushButton(padNo0, BTN_KIND_CODE);
        SetNextMode(MODE_FRIEND_CODE, pObj);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else {
        if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
            CancelVsFriend();
        }
        if (m_pVsFriend != 0) {
            SetNextMode(MODE_FRIEND_VS, pObj);
            return;
        }

        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        FriendList_LineMessage(nBtnKindId);
    }
}

void msw::CSelectMenu::FriendList_Out()
{
    static const long objId = OBJ_FRIEND_LIST;

    Scratch(objId);
}

void msw::CSelectMenu::FriendList_LineMessage(long)
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    SelectMenu::CFriendList *pObj = (SelectMenu::CFriendList *)FindObj(OBJ_FRIEND_LIST);
    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    if (nBtnKindId < BTN_KIND_ROW_0 || nBtnKindId > BTN_KIND_ROW_4) {
        return;
    }

    const wchar_t *pName = pObj->m_Name[nBtnKindId - BTN_KIND_ROW_0];
    if (wcslen(pName) == 0) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "F_LIS_08_01"));
        memset(m_FriendListLineName, 0, sizeof(m_FriendListLineName));
    } else if (wcsncmp(m_FriendListLineName, pName, wcslen(pName)) != 0) {
        // A new name: "<name>'s information" (only when it changes).
        wchar_t line[0xFF];
        memset(line, 0, sizeof(line));
        swprintf(line, 0xFF, GetMessageLabelString("WifiMenuMessage", "F_LIS_08_00"), pName);
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE, line);
        memset(m_FriendListLineName, 0, sizeof(m_FriendListLineName));
        wcscpy(m_FriendListLineName, pName);
    }
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 9,
        ANIM_WIN_OUT = 0xB,
        ANIM_WIN_IN_BACK = 7,
        ANIM_WIN_OUT_BACK = 8,
        ANIM_SCROLL = 0xD      // the rows moving by one
    };

    enum {
        PANE_WIN = 0,
        PANE_WIN_1 = 9,
        PANE_WIN_2 = 0x5C,
        PANE_SELF = 0xB,          // the player's row
        PANE_SELF_NAME = 0xC,
        PANE_SELF_CODE = 0xD,
        PANE_SELF_NUM_1 = 0xE,
        PANE_SELF_NUM_0 = 0xF,
        PANE_SELF_NUM_2 = 0x10,
        PANE_SLIDER_BASE = 0x34,  // the slider bar's track
        PANE_UNK_46 = 0x46,
        PANE_UNK_54 = 0x54,
        PANE_SELF_LABEL = 0x71
    };

    const char *const s_LayoutName = "me_f_list.brlyt";
    const char *const s_ButtonFileName = "me_f_list.btn";

    // The names of the Mii picture panes made by Set() (the last one has
    // none).
    const char *s_FaceNames[msw::SelectMenu::CFriendList::FACE_NUM] = {
        "FACE01", "FACE02", "FACE03", "FACE04", "FACE05", 0
    };

    // Panes, one per row unless said otherwise.
    long s_FacePanes[msw::SelectMenu::CFriendList::FACE_NUM] = { 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C };
    long s_RowPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x36, 0x37, 0x38, 0x39, 0x3A };
    long s_FriendPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x55, 0x4D, 0x4F, 0x51, 0x53 };
    long s_NamePanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x11, 0x16, 0x1B, 0x20, 0x25 };
    long s_Unk1BPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x12, 0x17, 0x1C, 0x21, 0x26 };
    long s_StatusPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x5E, 0x62, 0x66, 0x6B, 0x6E };
    long s_OfflinePanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x60, 0x64, 0x68, 0x6C, 0x70 };
    long s_OnlineIconPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x5D, 0x61, 0x65, 0x69, 0x6D };
    long s_OfflineIconPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x5F, 0x63, 0x67, 0x6A, 0x6F };
    long s_Num1Panes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x13, 0x18, 0x1D, 0x22, 0x27 };
    long s_Num0Panes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x14, 0x19, 0x1E, 0x23, 0x28 };
    long s_Num2Panes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x15, 0x1A, 0x1F, 0x24, 0x29 };
    long s_CodeRowPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x2A, 0x2C, 0x2E, 0x30, 0x32 };
    long s_CodePanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x2B, 0x2D, 0x2F, 0x31, 0x33 };
    long s_LabelPanes[msw::SelectMenu::CFriendList::ROW_NUM] = { 0x72, 0x73, 0x74, 0x75, 0x76 };

    // Values of CFriendList::m_nRowKind.
    enum {
        ROW_NONE = 0,
        ROW_FRIEND = 1
    };

    // The sound of the list moving by a row.
    enum {
        SE_SCROLL = 0xBC0
    };

    // Frames between two refreshes of the rows.
    const float REFRESH_WAIT = 120.0f;
}

msw::SelectMenu::CFriendList::CFriendList(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CFriendList::~CFriendList()
{
    Remove();
}

void msw::SelectMenu::CFriendList::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CFriendCode.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN_1, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN_2, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN_1, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_WIN_2, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
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
            if (m_bSliderDrag) {
                MoveSliderBarPointer(0);
            } else {
                // The bar follows the selected entry.
                unsigned short nRow = m_nCursorBtnKindId - BTN_KIND_ROW_0;
                if (nRow > 3) {
                    nRow = 3;
                }
                long nIndex = m_nTopIndex + nRow;
                if (m_fSliderRange != 0.0f) {
                    float fStep = -1.0f * (m_fSliderRange / m_pFriendList->GetNum());
                    bcs::G2D::CPaneObj *pBar = m_pPaneAccessor->GetPaneObj(PANE_SLIDER_BAR);
                    bcs::G2D::CPaneObj *pBarLight = m_pPaneAccessor->GetPaneObj(PANE_SLIDER_BAR_LIGHT);
                    m_SliderPos.y = fStep * nIndex + m_fSliderTop;
                    pBar->GetPane()->SetTranslate(m_SliderPos);
                    pBarLight->GetPane()->SetTranslate(m_SliderPos);
                }
            }
            MoveList();
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()
                && GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_READY, 0) != 0) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN_1, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN_2, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN_1, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN_2, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_OUT, 0);
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
        default:
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CFriendList::MoveList()
{
    bcs::dwc::CDwcFriendList *pList = m_pFriendList;

    switch (m_nMoveState) {
        case MOVE_NONE:
            // Refresh the rows now and then (the friends' states change).
            m_fRefreshWait -= bcs::CMainApp::GetAppFrameRateF();
            if (m_fRefreshWait < 0.0f) {
                SetFriendData(m_nTopIndex);
                m_fRefreshWait = REFRESH_WAIT;
            }
            break;
        case MOVE_DOWN:
            if (m_nTopIndex + 4 <= pList->GetNum()) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, m_fMoveSpeed, 0.0f, 1, 1);
                m_nMoveState = MOVE_DOWN_WAIT;
            } else {
                // At the end: the cursor goes to the last row.
                MoveCursor(BTN_KIND_ROW_3);
                m_nMoveState = MOVE_NONE;
            }
            break;
        case MOVE_DOWN_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_SCROLL)) {
                MoveCursor(BTN_KIND_ROW_3);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, 0.0f, 0.0f, 1, 1);
                SE_Play(SE_SCROLL, 1);
                SetFriendData(++m_nTopIndex);
                if (m_bMoveNext) {
                    m_nMoveState = MOVE_DOWN;
                    m_bMoveNext = 0;
                } else {
                    m_nMoveState = MOVE_NONE;
                }
            }
            break;
        case MOVE_JUMP:
            if (m_nTopIndex > pList->GetNum() - 3) {
                m_nTopIndex = pList->GetNum() - 3;
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, 0.0f, 0.0f, 1, 1);
            SetFriendData(m_nTopIndex);
            m_nMoveState = MOVE_NONE;
            break;
        case MOVE_UP:
            if (--m_nTopIndex < 0) {
                // At the top: the cursor goes to the first row.
                m_nTopIndex = 0;
                m_nMoveState = MOVE_NONE;
                MoveCursor(BTN_KIND_ROW_0);
            } else {
                // The rows are filled first and slide in from above.
                SetFriendData(m_nTopIndex);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, m_fMoveSpeed, 20.0f, 1, 1);
                m_nMoveState = MOVE_UP_WAIT;
            }
            break;
        case MOVE_UP_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_SCROLL)) {
                MoveCursor(BTN_KIND_ROW_0);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, 0.0f, 0.0f, 1, 1);
                if (m_nTopIndex != 0) {
                    SE_Play(SE_SCROLL, 1);
                }
                if (m_bMoveNext) {
                    m_nMoveState = MOVE_UP;
                    m_bMoveNext = 0;
                } else {
                    m_nMoveState = MOVE_NONE;
                }
            }
            break;
    }
}

void msw::SelectMenu::CFriendList::MoveSliderBarPointer(int bJump)
{
    long nIndex = (m_fSliderTop - m_SliderPos.y) / (m_fSliderRange / m_pFriendList->GetNum());

    if (bJump) {
        m_nTopIndex = nIndex;
        m_nMoveState = MOVE_JUMP;
        m_bMoveNext = 0;
        return;
    }

    long nTarget = m_nTopIndex + 3;
    if (nIndex != nTarget) {
        int bUp;
        float fSpeed;
        if (nIndex >= nTarget) {
            bUp = 0;
            fSpeed = nIndex - nTarget;
        } else {
            bUp = 1;
            fSpeed = nTarget - nIndex;
        }
        if (fSpeed < 1.0f) {
            fSpeed = 1.0f;
        }
        if (fSpeed > 3.0f) {
            fSpeed = 3.0f;
        }
        SetMoveButton(bUp, fSpeed, 0);
    }
}

void msw::SelectMenu::CFriendList::Disp()
{
}

int msw::SelectMenu::CFriendList::Set()
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

            // The button file tells which buttons belong to which rows.
            long nFriendButton = 0;
            long nCodeButton = 0;
            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                unsigned short nType = SetUpButton(pPart, i, 0);
                switch (nType) {
                    case 1:
                        m_nSelfButton = i;
                        break;
                    case 2:
                        m_nFriendButton[nFriendButton++] = i;
                        break;
                    case 3:
                        m_nCodeButton[nCodeButton++] = i;
                        break;
                }
                if (nType >= 0x10 && nType <= 0x12) {
                    pPart->m_bUnk40_0020 = 0;
                }
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            // Every row starts hidden.
            m_pPaneAccessor->SetPaneDisp(PANE_UNK_54, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_SELF, 0);
            for (long i = 0; i < ROW_NUM; i++) {
                CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ROW_0 + i);
                if (pPart != 0) {
                    pPart->m_nFlags40 |= 1;
                    m_pPaneAccessor->SetPaneDisp(s_RowPanes[i], 0);
                }
                m_pPaneAccessor->SetPaneDisp(s_FriendPanes[i], 0);
                m_pPaneAccessor->SetPaneDisp(s_CodeRowPanes[i], 0);
                m_pPaneAccessor->SetPaneDisp(s_Unk1BPanes[i], 0);
            }

            // The Mii pictures, rendered into textures of the size of the
            // first picture pane (RGB5A3: two bytes per texel).
            nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(s_FacePanes[0])->GetPane()->GetSize();
            long nTexSize = 2.0f * (size.width * size.height);
            for (long i = 0; i < FACE_NUM; i++) {
                m_pFaceTex[i] = new (1) unsigned char[nTexSize];

                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pFaceTex[i], (long)size.width, (long)size.height, GX_TF_RGB5A3, GX_CLAMP,
                             GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
                m_pFacePane[i] =
                    menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(s_FacePanes[i]), s_FaceNames[i]);
                m_pPaneAccessor->SetPaneDisp(s_FacePanes[i], 0);
            }

            m_pPlayerData = CSave::GetPlayerData(-1);
            dwc::CGameDwc *pGameDwc = dwc::CGameDwc::GetInstance();
            if (pGameDwc == 0 || !pGameDwc->m_Dwc.IsConnect()) {
                return -1;
            }
            m_pFriendList = pGameDwc->m_Dwc.GetFriendList();

            m_fSliderRange = 0.0f;
            m_fSliderTop = 0.0f;
            m_fSliderBottom = 0.0f;
            m_nMoveState = MOVE_NONE;
            m_fRefreshWait = REFRESH_WAIT;
            m_nCursorBtnKindId = BTN_KIND_ROW_0;
            SetFriendData(m_nTopIndex);
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_SCROLL, 0.0f, 0.0f, 1, 1);
            SetUpSliderBar();

            pSelectMenu->Build(OBJ_PLUS_MINUS);
            CPlusMinus *pPlusMinus = (CPlusMinus *)pSelectMenu->FindObj(OBJ_PLUS_MINUS);
            pPlusMinus->StartUp(1, GetMessageLabelString("List_Plus_Minus_Button", "Plus_05"), 0);

            m_pPaneAccessor->SetPaneDisp(PANE_UNK_54, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_UNK_46, 0);
            m_bVibEnable = 1;
            m_bSeEnable = 1;
        }
    }
    return 0;
}

void msw::SelectMenu::CFriendList::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    for (long i = 0; i < FACE_NUM; i++) {
        // Allocated with new[]; the game frees them with delete.
        if (m_pFaceTex[i] != 0) {
            delete m_pFaceTex[i];
        }
        m_pFaceTex[i] = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CFriendList::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    // The fifth row is only half shown: it counts as the row of the cursor.
    if (nResult == BTN_KIND_ROW_4) {
        nResult = m_nCursorBtnKindId;
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CFriendList::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendList::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendList::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendList::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CFriendList::StickMoveCheck(long nPad, long nBtnKindId)
{
    long nCurrent = nBtnKindId;
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_ROW_0;
    }

    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nCurrent, nInput);
    // Past the first or the last full row, the list scrolls.
    if ((nInput & PAD_UP) && nBtnKindId == BTN_KIND_ROW_0) {
        SetMoveButton(1, 1.0f, 0);
    } else if ((nInput & PAD_DOWN) && nBtnKindId == BTN_KIND_ROW_3) {
        SetMoveButton(0, 1.0f, 0);
    }
    if (nNext == BTN_KIND_ROW_4) {
        nNext = BTN_KIND_ROW_3;
    }
    return nNext;
}

long msw::SelectMenu::CFriendList::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_ROW_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CFriendList::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CFriendList::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nPad + EVENT_PUSH, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nPad + EVENT_PUSH, (void *)nBtnKindId);
    return 0;
}

// The helpers below are inlined in the game and defined here because their
// local statics follow Set()'s constants in .rodata.
inline void msw::SelectMenu::CFriendList::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

inline void msw::SelectMenu::CFriendList::SetTextPaneString(long nPane, const wchar_t *pText)
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

inline void msw::SelectMenu::CFriendList::SetTextPaneString(long nPane, long nValue)
{
    static const bool recursive = true;

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
    nw4r::lyt::Pane *pPane =
        (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
    if (pPane != 0) {
        wchar_t num[0x10];
        swprintf(num, 0xF, L"%d", nValue);
        m_pPaneAccessor->SetTextBoxString(nPane, num, wcslen(num));
    }
}

inline void msw::SelectMenu::CFriendList::MoveCursor(long nBtnKindId)
{
    if (m_nCursorBtnKindId != nBtnKindId) {
        CSelectMenu *pSelectMenu = GetSelectMenu();
        pSelectMenu->m_nCurrentBtnKindId[0][pSelectMenu->m_nMode] = nBtnKindId;
        if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
            m_nPushButtonId[0] = nBtnKindId;
            SetCursorMarkerPosition(0, nBtnKindId);
        }
        m_nCursorBtnKindId = nBtnKindId;
    }
}

inline void msw::SelectMenu::CFriendList::SetFaceIcon(long nFace, RFLStoreData &rData)
{
    nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(s_FacePanes[nFace])->GetPane()->GetSize();
    GXColor bgColor = { 0, 0, 0, 0 };
    RFLIconSetting setting;
    setting.width = (long)size.width;
    setting.height = (long)size.height;
    setting.bgType = 1;
    setting.bgColor = bgColor;
    setting.drawXluOnly = 0;
    bcs::CMiiMan::MakeFaceIcon(m_pFaceTex[nFace], rData, 0, setting, 1);
    m_pPaneAccessor->SetPaneDisp(s_FacePanes[nFace], 1);
}

void msw::SelectMenu::CFriendList::SetFriendData(long nTop)
{
    CSave::PlayerData *pPlayerData = m_pPlayerData;
    bcs::dwc::CDwcFriendList *pList = m_pFriendList;
    long nRow = 0;

    memset(m_Name, 0, sizeof(m_Name));
    m_pPaneAccessor->SetPaneDisp(PANE_SELF, 0);
    for (long i = 0; i < ROW_NUM; i++) {
        m_pPaneAccessor->SetPaneDisp(s_FriendPanes[i], 0);
        m_pPaneAccessor->SetPaneDisp(s_CodeRowPanes[i], 0);
        m_nRowKind[i] = ROW_NONE;
    }
    ResetButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nSelfButton)));
    for (long i = 0; i < ROW_NUM; i++) {
        ResetButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nFriendButton[i])));
        ResetButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nCodeButton[i])));
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    pList->Sort(0);

    for (long i = nTop; i < nTop + ROW_NUM; i++) {
        if (i == 0) {
            // The player's own row.
            SetTextPaneString(PANE_SELF_NAME, pPlayerData->m_Name);
            wcscpy(m_Name[0], pPlayerData->m_Name);
            SetTextPaneString(PANE_SELF_CODE, pSelectMenu->ExchangeFriendCode(pPlayerData->m_nFriendCode));
            SetTextPaneString(PANE_SELF_LABEL, GetMessageLabelString("WifiMenuMessage", "F_LIS_02_00"));
            SetTextPaneString(PANE_SELF_NUM_0, pPlayerData->m_Unk2EA);
            SetTextPaneString(PANE_SELF_NUM_1, pPlayerData->m_Unk2EC);
            SetTextPaneString(PANE_SELF_NUM_2, pPlayerData->m_Unk2EE);

            RFLStoreData storeData;
            bcs::CMiiMan::GetStoreData(storeData, pPlayerData->m_MiiSource, pPlayerData->m_MiiCreateId);
            SetFaceIcon(0, storeData);

            nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(PANE_SELF);
            nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
            bcs::G2D::SetPaneParent(pPane, m_pPaneAccessor->GetPane(s_RowPanes[0]), 1, 0);
            pPane->SetTranslate(pos);
            m_pPaneAccessor->SetPaneDisp(PANE_SELF, 1);

            CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ROW_0 + nRow);
            if (pPart != 0) {
                pPart->m_nFlags40 &= 0xFFFE;
                m_pPaneAccessor->SetPaneDisp(s_RowPanes[nRow], 1);
            }
            ReFineButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nSelfButton)), BTN_KIND_ROW_0, 0, BTN_KIND_ROW_0 + 1, 0,
                           0);
            nRow++;
            continue;
        }

        if (i - 1 >= pList->GetNum()) {
            break;
        }

        nw4r::lyt::Pane *pPane = 0;
        dwc::CFriend *pFriend = static_cast<dwc::CFriend *>(pList->GetSortFriend(i - 1));
        m_pRowFriend[nRow] = pFriend;
        if (pFriend->IsBuddyComplete() == 1) {
            long nIndex = pFriend->m_nIndex;

            // Online, offline or another state.
            if (pFriend->GetOnlineStat() == 0) {
                SetTextPaneString(s_OfflinePanes[nRow], GetMessageLabelString("WifiMenuMessage", "F_LIS_01_01"));
                m_pPaneAccessor->SetPaneDisp(s_OnlineIconPanes[nRow], 0);
                m_pPaneAccessor->SetPaneDisp(s_OfflineIconPanes[nRow], 1);
            } else if (pFriend->GetOnlineStat() == 1) {
                SetTextPaneString(s_StatusPanes[nRow], GetMessageLabelString("WifiMenuMessage", "F_LIS_01_00"));
                m_pPaneAccessor->SetPaneDisp(s_OnlineIconPanes[nRow], 1);
                m_pPaneAccessor->SetPaneDisp(s_OfflineIconPanes[nRow], 0);
            } else {
                SetTextPaneString(s_StatusPanes[nRow], GetMessageLabelString("WifiMenuMessage", "F_LIS_01_02"));
                m_pPaneAccessor->SetPaneDisp(s_OnlineIconPanes[nRow], 1);
                m_pPaneAccessor->SetPaneDisp(s_OfflineIconPanes[nRow], 0);
            }

            const wchar_t *pName = pFriend->m_Name;
            SetTextPaneString(s_NamePanes[nRow], pName);
            wcscpy(m_Name[nRow], pFriend->m_Name);
            SetTextPaneString(s_LabelPanes[nRow], GetMessageLabelString("WifiMenuMessage", "F_LIS_02_01"));

            CSave::PlayerData::Friend *pSave = &pPlayerData->m_Friend[nIndex];
            SetTextPaneString(s_Num0Panes[nRow], pSave->m_Unk298);
            SetTextPaneString(s_Num1Panes[nRow], pSave->m_Unk29A);
            SetTextPaneString(s_Num2Panes[nRow], pSave->m_Unk29C);

            SetFaceIcon(nRow + 1, pFriend->GetOwnStorageData()->m_StoreData);

            pPane = m_pPaneAccessor->GetPane(s_FriendPanes[nRow]);
            m_pPaneAccessor->SetPaneDisp(s_FriendPanes[nRow], 1);

            CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ROW_0 + nRow);
            if (pPart != 0) {
                pPart->m_nFlags40 &= 0xFFFE;
                m_pPaneAccessor->SetPaneDisp(s_RowPanes[nRow], 1);
            }
            unsigned short nBtnKindId = BTN_KIND_ROW_0 + nRow;
            ReFineButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nFriendButton[nRow])), nBtnKindId, nBtnKindId - 1,
                           nBtnKindId + 1, 0, 0);
            m_nRowKind[nRow] = ROW_FRIEND;
        } else if (pFriend->IsBuddyComplete() == 0) {
            // Not registered back yet: only the code is shown.
            SetTextPaneString(s_CodePanes[nRow], pSelectMenu->ExchangeFriendCode(pFriend->m_nFriendCode));

            pPane = m_pPaneAccessor->GetPane(s_CodeRowPanes[nRow]);
            m_pPaneAccessor->SetPaneDisp(s_CodeRowPanes[nRow], 1);

            CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ROW_0 + nRow);
            if (pPart != 0) {
                pPart->m_nFlags40 &= 0xFFFE;
                m_pPaneAccessor->SetPaneDisp(s_RowPanes[nRow], 1);
            }
            unsigned short nBtnKindId = BTN_KIND_ROW_0 + nRow;
            ReFineButtonID(static_cast<CMenuPartButton *>(m_pMenuPartList->FindObject(m_nCodeButton[nRow])), nBtnKindId, nBtnKindId - 1,
                           nBtnKindId + 1, 0, 0);
        }

        // The row's panes move under the row's button.
        nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
        bcs::G2D::SetPaneParent(pPane, m_pPaneAccessor->GetPane(s_RowPanes[nRow]), 1, 0);
        pPane->SetTranslate(pos);
        nRow++;
    }
}

void msw::SelectMenu::CFriendList::SetMoveButton(int bUp, float fSpeed, int bIdleOnly)
{
    if (bIdleOnly == 1 && m_nMoveState != MOVE_NONE) {
        return;
    }

    if (bUp == 1) {
        if (m_nMoveState == MOVE_UP || m_nMoveState == MOVE_UP_WAIT) {
            m_bMoveNext = 1;
        } else if (m_nMoveState == MOVE_NONE) {
            m_nMoveState = MOVE_UP;
            m_bMoveNext = 0;
        }
        m_fMoveSpeed = -fSpeed;
    } else {
        if (m_nMoveState == MOVE_DOWN || m_nMoveState == MOVE_DOWN_WAIT) {
            m_bMoveNext = 1;
        } else if (m_nMoveState == MOVE_NONE) {
            m_nMoveState = MOVE_DOWN;
            m_bMoveNext = 0;
        }
        m_fMoveSpeed = fSpeed;
    }
}

void msw::SelectMenu::CFriendList::SetUpSliderBar()
{
    bcs::dwc::CDwcFriendList *pList = m_pFriendList;

    nw4r::lyt::Pane *pBase = m_pPaneAccessor->GetPaneObj(PANE_SLIDER_BASE)->GetPane();
    nw4r::lyt::Size baseSize = pBase->GetSize();
    nw4r::math::VEC3 basePos = pBase->GetTranslate();
    bcs::G2D::CPaneObj *pBar = m_pPaneAccessor->GetPaneObj(PANE_SLIDER_BAR);
    bcs::G2D::CPaneObj *pBarLight = m_pPaneAccessor->GetPaneObj(PANE_SLIDER_BAR_LIGHT);
    nw4r::lyt::Size barSize = pBar->GetPane()->GetSize();

    if (pList->GetNum() <= 3) {
        // Everything fits: the bar fills the track and is not shown.
        nw4r::lyt::Size size(barSize.width, baseSize.height);
        pBar->GetPane()->SetSize(size);
        pBarLight->GetPane()->SetSize(size);
        pBar->GetPane()->SetTranslate(basePos);
        pBarLight->GetPane()->SetTranslate(basePos);
        m_pPaneAccessor->SetPaneDisp(PANE_SLIDER_BAR_LIGHT, 0);
        return;
    }

    // The more friends, the shorter the bar.
    float fHeight = baseSize.height * (101.0f - (pList->GetNum() * 100) / 32) / 100.0f;
    if (fHeight < barSize.height) {
        fHeight = barSize.height;
    }
    nw4r::lyt::Size size(barSize.width, fHeight);
    pBar->GetPane()->SetSize(size);
    pBarLight->GetPane()->SetSize(size);
    m_pPaneAccessor->SetPaneDisp(PANE_SLIDER_BAR_LIGHT, 1);

    m_fSliderRange = baseSize.height - fHeight;
    m_fSliderTop = basePos.y + m_fSliderRange * 0.5f;
    m_fSliderBottom = basePos.y - m_fSliderRange * 0.5f;
    m_SliderPos.x = pBar->GetPane()->GetTranslate().x;
    m_SliderPos.y = m_fSliderTop;
    pBar->GetPane()->SetTranslate(m_SliderPos);
    pBarLight->GetPane()->SetTranslate(m_SliderPos);
}
