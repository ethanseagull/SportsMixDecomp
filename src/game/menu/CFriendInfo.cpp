#include "game/msw/SelectMenu/CFriendInfo.hpp"

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

// The steps of the friend's information window (their local statics open
// this unit's .rodata). The rest of CFriendInfo is not written yet.
namespace {
    enum {
        OBJ_FRIEND_INFO = 0x29,
        BTN_KIND_TAB_0 = 0x8020,
        BTN_KIND_TAB_4 = 0x8024,
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        EVENT_TOP_CLEAR = 1
    };
}

void msw::CSelectMenu::FriendPlayerInfo_In()
{
    static const long objId = OBJ_FRIEND_INFO;
    static const long padNo0 = 0;

    m_Unk708 = 0;
    Build(objId);
    SendObjEventTrigger(OBJ_TOP, EVENT_TOP_CLEAR, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    SelectMenu::CFriendInfo *pObj = (SelectMenu::CFriendInfo *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        pObj->SetUpDisp(0);
    }
}

void msw::CSelectMenu::FriendPlayerInfo_Loop()
{
    static const long objId = OBJ_FRIEND_INFO;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
        return;
    }
    SelectMenu::CFriendInfo *pObj = (SelectMenu::CFriendInfo *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        switch (nBtnKindId) {
            case BTN_KIND_TAB_0:
            case BTN_KIND_TAB_0 + 1:
            case BTN_KIND_TAB_0 + 2:
            case BTN_KIND_TAB_0 + 3:
            case BTN_KIND_TAB_4:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB_0, 0);
                break;
            case BTN_KIND_BACK:
                CMenuPad::s_Instance.ChangeInputPointerModeForce(nPad);
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        CMenuPad::s_Instance.ChangeInputStickModeForce(nPad);
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
        pObj->SetTabSelect(pObj->m_nTabSelect + 1, 0);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
        pObj->SetTabSelect(pObj->m_nTabSelect - 1, 0);
    } else {
        if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
            CancelVsFriend();
        }
        if (m_pVsFriend != 0) {
            SetNextMode(MODE_FRIEND_VS, pObj);
            return;
        }
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::FriendPlayerInfo_Out()
{
    static const long objId = OBJ_FRIEND_INFO;

    Scratch(objId);
}

void msw::CSelectMenu::FriendInfo_In()
{
    static const long objId = OBJ_FRIEND_INFO;
    static const long padNo0 = 0;

    Build(objId);
    SendObjEventTrigger(OBJ_TOP, EVENT_TOP_CLEAR, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    SelectMenu::CFriendInfo *pObj = (SelectMenu::CFriendInfo *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        pObj->SetUpDisp(1);
    }
    m_nStep = 0;
}
