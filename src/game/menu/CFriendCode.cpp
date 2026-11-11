#include "game/msw/SelectMenu/CFriendCode.hpp"

#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/dwc/CDwcFriend.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/SelectMenu/CFriendInfo.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "math.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CWiFiFrindVS.cpp: the original converts
    // the value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_FRIEND_CODE = 0x1E,
        OBJ_DWC_WINDOW = 0x3F // what the Wi-Fi message window is sent through
    };

    // What the objects are sent.
    enum {
        EVENT_LINE_MESSAGE = 0x11, // to OBJ_TOP: the help line
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21,         // plus the pad number
        EVENT_WINDOW_CLOSE = 0x32  // to OBJ_DWC_WINDOW
    };

    // The cockpit's fade window (CCockpit::IsEndObj), and the save window.
    enum {
        COCKPIT_OBJ_FADE = 0x1042,
        COCKPIT_OBJ_SAVE = 0x101A
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_DIGIT_0 = 0x8020, // to 0x8029, one per digit
        BTN_KIND_DIGIT_1 = 0x8021,
        BTN_KIND_DIGIT_9 = 0x8029,
        BTN_KIND_DELETE = 0x8030,
        BTN_KIND_OK = 0x8040
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_INPUT = 0,
        STEP_CONFIRM = 1,       // ask whether to register the friend
        STEP_CONFIRM_WAIT = 2,
        STEP_ERROR = 10,        // a message is shown; back to the input
        STEP_FULL = 20,         // the roster is full; leave once it is closed
        STEP_REGISTER = 30,
        STEP_SAVE = 31,
        STEP_SAVE_WAIT = 32,
        STEP_SAVE_CLOSE = 33,
        STEP_DONE = 34,
        STEP_DONE_CODE = 35
    };

    // Results of CCockpit::GetResult for the message windows.
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };

    // The mode the screen leaves to without a connection.
    enum {
        MODE_NO_CONNECTION = 4,
        MODE_FRIEND_VS = 0x2A
    };

    // Frames the delete button waits after each B press.
    const float DELETE_WAIT = 30.0f;
}

void msw::CSelectMenu::FriendCode_In()
{
    static const long padNo0 = 0;

    dwc::CGameDwc *pGameDwc = dwc::CGameDwc::GetInstance();
    if (pGameDwc == 0 || !pGameDwc->m_Dwc.IsConnect()) {
        SetNextMode(MODE_NO_CONNECTION, 0.0f);
        return;
    }

    m_pFriendList = pGameDwc->m_Dwc.GetFriendList();
    if (m_pFriendList->IsNumMax()) {
        WindowA_Button("WiFiErrorMessage", "WiFi_22", 0, 0);
        m_nStep = STEP_FULL;
        return;
    }

    Build(OBJ_FRIEND_CODE);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_FRIEND_CODE);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }

    m_fInputWait = 0.0f;
    m_nStep = STEP_INPUT;
}

void msw::CSelectMenu::FriendCode_Loop()
{
    switch (m_nStep) {
        case STEP_INPUT:
            FriendCode_LoopStep();
            break;
        case STEP_CONFIRM: {
            // The result is not used.
            FindObj(OBJ_FRIEND_CODE);
            WindowYesNo_Button("CharaEntryMessage", "FMAKE_01_01", 1, m_MessageWork, 0, 1);
            m_nStep = STEP_CONFIRM_WAIT;
            break;
        }
        case STEP_CONFIRM_WAIT:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    Window_Message("CharaEntryMessage", "FMAKE_04", 0);
                    m_nStep = STEP_REGISTER;
                    break;
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_nStep = STEP_INPUT;
                    break;
            }
            break;
        case STEP_ERROR:
            if (CCockpit::GetResult() == RESULT_YES) {
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_nStep = STEP_INPUT;
            }
            break;
        case STEP_FULL:
            if (CCockpit::GetResult() == RESULT_YES) {
                static const long padNo0 = 0;
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                GoBackToMode(padNo0, GetPrevMode(), 0, 1);
            }
            break;
        case STEP_REGISTER: {
            SelectMenu::CFriendCode *pObj = (SelectMenu::CFriendCode *)FindObj(OBJ_FRIEND_CODE);
            unsigned long long nCode = pObj->GetInputCode();
            bcs::dwc::CDwcFriend *pFriend = m_pFriendList->AddFriend(&m_pPlayerData->m_AccUserData, nCode);
            if (pFriend != 0) {
                m_pFriendList->NormalizeNo();
                // Copy the whole roster into the save data.
                for (long i = 0; i < CSave::PlayerData::FRIEND_MAX; i++) {
                    bcs::dwc::CDwcFriend *pEntry = m_pFriendList->GetFriend(i);
                    CSave::PlayerData::Friend *pSave = &m_pPlayerData->m_Friend[i];
                    if (!pEntry->IsValid()) {
                        memset(pSave, 0, sizeof(CSave::PlayerData::Friend));
                    } else {
                        pSave->m_Unk18 = pEntry->m_Unk04;
                        pSave->m_Unk1C = pEntry->m_Unk08;
                        pSave->m_Unk20 = pEntry->m_Unk0C;
                        pSave->m_nFriendCode = pEntry->m_nFriendCode;
                        pSave->m_Unk30 = pEntry->m_nIndex;
                        pSave->m_Unk34 = pEntry->m_Unk1C;
                        memcpy(pSave->m_Data, pEntry->m_Data, sizeof(pSave->m_Data));
                    }
                }
                m_pPlayerData->m_nFriendNum = m_pFriendList->GetNum();
                pFriend->LoadStorage();
            }
            m_nStep = STEP_SAVE;
            break;
        }
        case STEP_SAVE:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nStep = STEP_SAVE_WAIT;
            }
            break;
        case STEP_SAVE_WAIT:
            if (CSave::IsComplete()) {
                Window_Close();
                m_nStep = STEP_SAVE_CLOSE;
            }
            break;
        case STEP_SAVE_CLOSE:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_SAVE)) {
                WindowA_Button("CharaEntryMessage", "FMAKE_05_01", m_MessageWork, 0);
                m_nStep = STEP_DONE;
            }
            break;
        case STEP_DONE:
            if (CCockpit::GetResult() == RESULT_YES) {
                WindowA_Button("CharaEntryMessage", "FMAKE_06_00", m_pPlayerData->m_Name,
                               ExchangeFriendCode(m_pPlayerData->m_nFriendCode));
                m_nStep = STEP_DONE_CODE;
            }
            break;
        case STEP_DONE_CODE:
            if (CCockpit::GetResult() == RESULT_YES) {
                static const long padNo0 = 0;
                SelectMenu::CFriendCode *pObj = (SelectMenu::CFriendCode *)FindObj(OBJ_FRIEND_CODE);
                pObj->ClearInputDigit();
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_pGameDwc->m_Dwc.SetFriendServersAsyncFlag(1);
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
            break;
    }
}

void msw::CSelectMenu::FriendCode_LoopStep()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
        return;
    }
    SelectMenu::CFriendCode *pObj = (SelectMenu::CFriendCode *)FindObj(OBJ_FRIEND_CODE);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    long nBtnKindId = rBtnKindId;

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_DIGIT_0:
            case BTN_KIND_DIGIT_0 + 1:
            case BTN_KIND_DIGIT_0 + 2:
            case BTN_KIND_DIGIT_0 + 3:
            case BTN_KIND_DIGIT_0 + 4:
            case BTN_KIND_DIGIT_0 + 5:
            case BTN_KIND_DIGIT_0 + 6:
            case BTN_KIND_DIGIT_0 + 7:
            case BTN_KIND_DIGIT_0 + 8:
            case BTN_KIND_DIGIT_9: {
                long nNum = pObj->m_nInputNum;
                if (0 <= nNum && nNum < SelectMenu::CFriendCode::INPUT_DIGIT_NUM) {
                    pObj->m_nInputDigit[nNum] = nBtnKindId - BTN_KIND_DIGIT_0;
                }
                long nNext = nNum + 1;
                if (nNext > SelectMenu::CFriendCode::INPUT_DIGIT_NUM) {
                    nNext = SelectMenu::CFriendCode::INPUT_DIGIT_NUM;
                }
                pObj->m_nInputNum = nNext;
                pObj->SetTextPaneFromInputDigit();
                pObj->SetInputCursor(nNext);
                // With the code complete, a pointer moves to the OK button.
                if (pObj->m_nInputNum >= SelectMenu::CFriendCode::INPUT_DIGIT_NUM && m_Unk23E == 1) {
                    rBtnKindId = BTN_KIND_OK;
                    pObj->m_nPushButtonId[padNo0] = BTN_KIND_OK;
                    pObj->SetCursorMarkerPosition(padNo0, BTN_KIND_OK);
                }
                break;
            }
            case BTN_KIND_DELETE: {
                long nNum = pObj->m_nInputNum;
                if (1 <= nNum && nNum <= SelectMenu::CFriendCode::INPUT_DIGIT_NUM) {
                    pObj->m_nInputDigit[nNum - 1] = SelectMenu::CFriendCode::DIGIT_NONE;
                }
                long nPrev = nNum - 1;
                if (nPrev < 0) {
                    nPrev = 0;
                }
                pObj->m_nInputNum = nPrev;
                pObj->SetTextPaneFromInputDigit();
                pObj->SetInputCursor(nPrev);
                break;
            }
            case BTN_KIND_OK:
                if (pObj->m_nInputNum >= SelectMenu::CFriendCode::INPUT_DIGIT_NUM) {
                    unsigned long long nCode = pObj->GetInputCode();
                    ExchangeFriendCode(nCode);
                    if (m_pFriendList->GetFriendFromFriendCode(nCode) != 0) {
                        // Already a friend.
                        WindowA_Button("CharaEntryMessage", "FMAKE_03", 0, 0);
                        m_nStep = STEP_ERROR;
                        return;
                    }
                    if (!bcs::dwc::CDwcFriendList::CheckFriendCode(&m_pPlayerData->m_AccUserData, nCode)) {
                        WindowA_Button("CharaEntryMessage", "FMAKE_02_01", m_MessageWork, 0);
                        m_nStep = STEP_ERROR;
                        return;
                    }
                    if (m_pPlayerData->m_nFriendCode == nCode) {
                        // The player's own code.
                        WindowA_Button("CharaEntryMessage", "FMAKE_02_01", m_MessageWork, 0);
                        m_nStep = STEP_ERROR;
                        return;
                    }
                    m_nStep = STEP_CONFIRM;
                }
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        long nNum = pObj->m_nInputNum;
        if (nNum == 0) {
            if (m_fInputWait <= 0.0f) {
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
        } else {
            // B deletes the last digit, as the delete button does.
            rBtnKindId = BTN_KIND_DELETE;
            pObj->m_nPushButtonId[padNo0] = BTN_KIND_DELETE;
            pObj->SetCursorMarkerPosition(padNo0, BTN_KIND_DELETE);
            pObj->PushButton(padNo0, BTN_KIND_DELETE);
            if (1 <= nNum && nNum <= SelectMenu::CFriendCode::INPUT_DIGIT_NUM) {
                pObj->m_nInputDigit[nNum - 1] = SelectMenu::CFriendCode::DIGIT_NONE;
            }
            long nPrev = nNum - 1;
            if (nPrev < 0) {
                nPrev = 0;
            }
            pObj->m_nInputNum = nPrev;
            pObj->SetTextPaneFromInputDigit();
            pObj->SetInputCursor(nPrev);
            m_fInputWait = DELETE_WAIT;
        }
    } else {
        if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
            CancelVsFriend();
        }
        if (m_pVsFriend != 0) {
            SetNextMode(MODE_FRIEND_VS, pObj);
            return;
        }

        if (m_fInputWait != 0.0f) {
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                m_fInputWait = 0.0f;
            }
        }

        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        // The help line of the selected button.
        switch (rBtnKindId) {
            case BTN_KIND_OK:
                SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "F_COR_01_00"));
                break;
            default:
                SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "F_COR_01_01"));
                break;
        }
    }
}

void msw::CSelectMenu::FriendCode_Out()
{
    Scratch(OBJ_FRIEND_CODE);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 8,
        ANIM_WIN_OUT = 0xA,
        ANIM_WIN_IN_BACK = 6,
        ANIM_WIN_OUT_BACK = 7,
        ANIM_OK_ENABLE = 0xC,
        ANIM_OK_DISABLE = 0xD,
        ANIM_DIGIT = 0xE        // a digit's text box: blinks as the cursor
    };

    enum {
        PANE_WIN = 0,
        PANE_OK = 0x16
    };

    const char *const s_LayoutName = "me_f_cord.brlyt";
    const char *const s_ButtonFileName = "me_f_cord.btn";

    // The text box of each digit.
    const unsigned char s_DigitPanes[msw::SelectMenu::CFriendCode::INPUT_DIGIT_NUM] = {
        0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
    };
}

msw::SelectMenu::CFriendCode::CFriendCode(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_pMenuPartList = 0;
    ClearInputDigit();
    m_Unk2C = 0;
}

msw::SelectMenu::CFriendCode::~CFriendCode()
{
    Remove();
}

void msw::SelectMenu::CFriendCode::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CScoreTop.cpp (see
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
            CheckButtonAnim();
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
        default:
            // Unlike CTeam, no part list update in an unknown state.
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CFriendCode::Disp()
{
}

int msw::SelectMenu::CFriendCode::Set()
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
                SetUpButton(pPart, i, 1);
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            ClearInputDigit();
            SetTextPaneFromInputDigit();
            SetInputCursor(m_nInputNum);
        }
    }
    return 0;
}

void msw::SelectMenu::CFriendCode::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CFriendCode::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CFriendCode::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendCode::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendCode::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendCode::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CFriendCode::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CFriendCode::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_DIGIT_1;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CFriendCode::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CFriendCode::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it.
void msw::SelectMenu::CFriendCode::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
}

long msw::SelectMenu::CFriendCode::SetTextPaneFromInputDigit()
{
    // Each text box shows figureNum digits.
    static const int figureNum = 1;
    static const int tableLen = INPUT_DIGIT_NUM;

    for (long i = 0; i < tableLen; i++) {
        unsigned long nValue = 0;
        long nFigure = 0;
        for (long j = 0; j < figureNum; j++) {
            unsigned char nDigit = m_nInputDigit[i + j];
            if (nDigit <= 9) {
                nValue *= 10;
                nValue += nDigit;
                nFigure++;
            }
        }

        unsigned char nPane = s_DigitPanes[i];
        if (nFigure > 0) {
            m_pPaneAccessor->SetPaneDisp(nPane, 1);
            m_pPaneAccessor->SetTextBoxString(nPane, nValue, nFigure);
            m_pPaneAccessor->GetPaneObj(nPane)->PlayAnimation(ANIM_DIGIT, 0.0f, 0.0f, 1, 1, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(nPane, 0);
        }
    }
    return 0;
}

// The two below are inline in the game (no symbols) and are used by Set()
// and by CSelectMenu's steps.
inline unsigned long long msw::SelectMenu::CFriendCode::GetInputCode() const
{
    unsigned long long nCode = 0;
    for (long i = INPUT_DIGIT_NUM - 1; i >= 0; i--) {
        unsigned char nDigit = m_nInputDigit[i];
        double fScale = pow(10.0, INPUT_DIGIT_NUM - 1 - i);
        nCode += (unsigned long long)nDigit * fScale;
    }
    return nCode;
}

void msw::SelectMenu::CFriendCode::SetTextPaneString(long nPane, const wchar_t *pText)
{
    m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
}

void msw::SelectMenu::CFriendCode::SetInputCursor(long nIndex)
{
    if (m_nInputNum < INPUT_DIGIT_NUM) {
        unsigned char nPane = s_DigitPanes[nIndex];
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        SetTextPaneString(nPane, L"_");
        m_pPaneAccessor->GetPaneObj(nPane)->PlayAnimation(ANIM_DIGIT, 0.5f, 0.0f, 1, 1, 1);
    }

    // The OK button works only once the code is complete.
    bool bComplete = m_nInputNum >= INPUT_DIGIT_NUM;
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_OK);
    if (pPart != 0) {
        if (bComplete) {
            m_pPaneAccessor->GetPaneObj(PANE_OK)->PlayAnimation(ANIM_OK_ENABLE, 1.0f, 0.0f, 1, 1, 1);
            pPart->m_nFlags40 &= 0xFFFE;
        } else {
            m_pPaneAccessor->GetPaneObj(PANE_OK)->PlayAnimation(ANIM_OK_DISABLE, 1.0f, 0.0f, 1, 1, 1);
            pPart->m_nFlags40 |= 0x0001;
        }
    }
}
