#include "game/msw/SelectMenu/CFriendInfo.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/dwc/CDwcFriend.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CPlayerInfoRender.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/SelectMenu/CConnectionWait.hpp"
#include "game/msw/SelectMenu/CFriendList.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/SelectMenu/CTrofy.hpp"
#include "game/msw/WiFiSkill.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/layout.h"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CWiFiFrindVS.cpp: the original converts
    // the value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // The ratings of the four border lines of the graph, and their colours.
    long s_BorderValue[msw::SelectMenu::CFriendInfo::BORDER_LINE_NUM] = { 1000, 2000, 4000, 8000 };
    unsigned long s_BorderColor[msw::SelectMenu::CFriendInfo::BORDER_LINE_NUM] = {
        0xB45A00FF, 0xD2D2D2FF, 0xFFF050FF, 0xBEFAFAFF
    };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_PLUS_MINUS = 2,     // SelectMenu::CPlusMinus: the delete button
        OBJ_FRIEND_LIST = 0x28,
        OBJ_FRIEND_INFO = 0x29,
        OBJ_NO_FRIEND_INFO = 0x2A,
        OBJ_DWC_WINDOW = 0x3F,  // what the Wi-Fi message window is sent through
        OBJ_TROFY = 0x4D,
        OBJ_CONNECTION_WAIT = 0xFF
    };

    // What the objects are sent.
    enum {
        EVENT_TOP_CLEAR = 1,
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,         // plus the pad number
        EVENT_PLUS_MINUS_OUT = 0x25,
        EVENT_PLUS_MINUS_IS_OUT = 0x26,
        EVENT_CONNECTION_CLOSE = 0x2E,
        EVENT_WINDOW_CLOSE = 0x32  // to OBJ_DWC_WINDOW
    };

    // The cockpit's fade window (CCockpit::IsEndObj), and the window of a
    // failed connection.
    enum {
        COCKPIT_OBJ_FADE = 0x1042,
        COCKPIT_OBJ_ERROR = 0x1019
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_TAB_0 = 0x8020, // to 0x8024, one per tab
        BTN_KIND_TAB_4 = 0x8024,
        BTN_KIND_MATCH = 0x8030, // ask the friend for a match
        // What CButtonEntry::SetUpButton returns for the match button: the
        // kind id without the 0x8000 bit (the original compares with 0x30).
        BTN_ID_MATCH = 0x30,
        BTN_KIND_DELETE = 0x80A0 // CPlusMinus's minus button: delete the friend
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_MINUS = 0x80
    };

    // Values of CSelectMenu::m_nStep while the friend's window is shown.
    enum {
        STEP_INPUT = 0,
        STEP_MATCH = 10,          // start asking the friend for a match
        STEP_MATCH_CREATE = 11,
        STEP_MATCH_WAIT = 12,
        STEP_MATCH_MATCHED = 13,
        STEP_MATCH_ERROR = 20,    // the match failed: wait, then say so
        STEP_MATCH_ERROR_MSG = 21,
        STEP_MATCH_CLOSE = 30,
        STEP_DELETE_CONFIRM = 40, // the delete question is shown
        STEP_DELETE = 41,
        STEP_DELETE_SAVE = 42,
        STEP_DELETE_SAVE_WAIT = 43,
        STEP_DELETE_DONE = 44,
        STEP_MATCH_START = 99
    };

    // Values of CSelectMenu::m_nStep for the window of a friend who has not
    // registered the player back.
    enum {
        NO_FRIEND_STEP_INPUT = 0,
        NO_FRIEND_STEP_CONFIRM = 10,
        NO_FRIEND_STEP_DELETE = 11,
        NO_FRIEND_STEP_SAVE = 12,
        NO_FRIEND_STEP_SAVE_WAIT = 13,
        NO_FRIEND_STEP_DONE = 14
    };

    // Steps of CSelectMenu::FriendMatchChancelCheck (m_nCancelStep).
    enum {
        CANCEL_STEP_WAIT = 0,  // waiting for the player to cancel
        CANCEL_STEP_CLOSE = 1,
        CANCEL_STEP_ASK = 2
    };

    // Results of CCockpit::GetResult for the message windows.
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };

    // Values of bcs::dwc::CDwc::GetStat() tested here (names guessed).
    enum {
        DWC_STAT_READY = 4,
        DWC_STAT_ERROR = 10
    };

    // The modes the screens leave to.
    enum {
        MODE_FRIEND_VS_WAIT = 0x28,
        MODE_FRIEND_VS = 0x2A
    };

    // Frames the error message waits for.
    const float MATCH_ERROR_WAIT = 60.0f;
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
    m_nStep = STEP_INPUT;
}

void msw::CSelectMenu::FriendInfo_Loop()
{
    static const long padNo0 = 0;

    long nStat = m_pGameDwc->m_Dwc.GetStat();

    switch (m_nStep) {
        case STEP_INPUT:
            FriendInfo_LoopStep();
            break;
        case STEP_MATCH:
            if (CGamePadCheck::GetInstance() != 0) {
                CGamePadCheck::GetInstance()->m_UnkAC = 1;
            }
            if (nStat == DWC_STAT_READY) {
                // Take down any connection indicator still up.
                for (;;) {
                    if (FindObj(OBJ_CONNECTION_WAIT) == 0) {
                        break;
                    }
                    m_pGameDwc->Window_Pict1Close();
                    RemoveObj(OBJ_CONNECTION_WAIT);
                }
                m_pGameDwc->Window_Picture1("WifiConnectMessage", "FVS_WAIT_00", 0);
                m_nCancelStep = CANCEL_STEP_WAIT;
                SelectMenu::CConnectionWait *pWait = new SelectMenu::CConnectionWait(OBJ_CONNECTION_WAIT, 0, 0.0f);
                AddObj(pWait);
                pWait->Set();
                m_nStep = STEP_MATCH_CREATE;
            } else {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            }
            break;
        case STEP_MATCH_CREATE:
            if (m_pGameDwc->m_Dwc.CreateFriendMatch(m_Unk708->m_nIndex, m_nPlayerNum)) {
                m_nStep = STEP_MATCH_WAIT;
            } else {
                SendObjEventTrigger(OBJ_CONNECTION_WAIT, EVENT_CONNECTION_CLOSE, 0);
                m_pGameDwc->Window_Pict1Close();
                m_fInputWait = MATCH_ERROR_WAIT;
                m_nStep = STEP_MATCH_ERROR;
                break;
            }
            FriendMatchChancelCheck();
            break;
        case STEP_MATCH_WAIT:
        case STEP_MATCH_MATCHED:
            if (m_nStep == STEP_MATCH_WAIT && m_pGameDwc->m_Dwc.IsMatched()) {
                m_nStep = STEP_MATCH_MATCHED;
            } else if (m_nStep == STEP_MATCH_MATCHED && m_pGameDwc->m_Dwc.IsMatchPlay()) {
                m_pGameDwc->Window_Pict1Close();
                SendObjEventTrigger(OBJ_CONNECTION_WAIT, EVENT_CONNECTION_CLOSE, 0);
                SetNextMode(MODE_FRIEND_VS_WAIT, (CSelectMenuLayoutBase *)FindObj(OBJ_FRIEND_INFO));
                memset(m_FriendVSWaitWork, 0, sizeof(m_FriendVSWaitWork));
                FriendVSWait_SendSetStep(0x10);
                m_bStarMii = 1;
                if (CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 0;
                }
                m_nStep = STEP_MATCH_START;
                break;
            } else if (m_pGameDwc->m_Dwc.GetStat() == DWC_STAT_ERROR) {
                m_pGameDwc->Window_Pict1Close();
                SendObjEventTrigger(OBJ_CONNECTION_WAIT, EVENT_CONNECTION_CLOSE, 0);
                m_fInputWait = MATCH_ERROR_WAIT;
                m_nStep = STEP_MATCH_ERROR;
                break;
            }
            FriendMatchChancelCheck();
            break;
        case STEP_MATCH_ERROR:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                m_pGameDwc->WindowA_Button("WifiConnectMessage", "FENT_02_00", 0, 0, 1, 0);
                m_nStep = STEP_MATCH_ERROR_MSG;
            }
            break;
        case STEP_MATCH_ERROR_MSG:
            if (CCockpit::GetResult() == RESULT_YES) {
                m_pGameDwc->ClearWindowResult();
                m_fInputWait = MATCH_ERROR_WAIT;
                m_nStep = STEP_MATCH_CLOSE;
            }
            break;
        case STEP_MATCH_CLOSE:
            m_fInputWait -= GetFrameRateF();
            m_pGameDwc->m_Dwc.CloseMatching();
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (nStat == DWC_STAT_READY) {
                if (m_fInputWait < 0.0f) {
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_pGameDwc->Window_Close();
                    if (CGamePadCheck::GetInstance() != 0) {
                        CGamePadCheck::GetInstance()->m_UnkAC = 0;
                    }
                    m_nStep = STEP_INPUT;
                }
            }
            break;
        case STEP_DELETE_CONFIRM:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    m_pGameDwc->ClearWindowResult();
                    m_nStep = STEP_DELETE;
                    break;
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_pGameDwc->ClearWindowResult();
                    m_nStep = STEP_INPUT;
                    break;
            }
            break;
        case STEP_DELETE: {
            dwc::CFriend *pFriend = m_Unk708;
            long nIndex = pFriend->m_nIndex;
            // Forget what the save data keeps for the friend.
            for (long i = 0; i < CSave::PlayerData::FRIEND_MAX; i++) {
                if (m_pPlayerData->m_Friend[i].m_nFriendCode == pFriend->m_nFriendCode) {
                    m_pPlayerData->m_Friend[i].m_Unk298 = 0;
                    m_pPlayerData->m_Friend[i].m_Unk29A = 0;
                    m_pPlayerData->m_Friend[i].m_Unk29C = 0;
                    break;
                }
            }
            m_pFriendList->RemoveFriend(nIndex);
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
            }
            m_nStep = STEP_DELETE_SAVE;
            break;
        }
        case STEP_DELETE_SAVE:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nStep = STEP_DELETE_SAVE_WAIT;
            }
            break;
        case STEP_DELETE_SAVE_WAIT:
            if (CSave::IsComplete()) {
                WindowA_Button("CharaEntryMessage", "FMAKE_13", m_FriendNameWork, 0);
                m_nStep = STEP_DELETE_DONE;
            }
            break;
        case STEP_DELETE_DONE:
            if (CCockpit::GetResult() == RESULT_YES) {
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_pGameDwc->ClearWindowResult();
                CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_FRIEND_INFO);
                m_pGameDwc->m_Dwc.SetFriendServersAsyncFlag(1);
                // The roster starts again from its first row.
                ((SelectMenu::CFriendList *)FindObj(OBJ_FRIEND_LIST))->m_nTopIndex = 0;
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
            break;
    }
}

void msw::CSelectMenu::FriendInfo_LoopStep()
{
    // static-order swap, as in FriendPlayerInfo_In() (see
    // notes/decisions.md).
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
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB_0, 1);
                break;
            case BTN_KIND_MATCH:
                // Only while the friend is online.
                if (pObj->m_nDeleteAnim != 0) {
                    pObj->PushButton(padNo0, BTN_KIND_MATCH);
                    m_nStep = STEP_MATCH;
                }
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_DELETE:
                pObj->PushButton(padNo0, BTN_KIND_DELETE);
                memcpy(m_FriendNameWork, m_Unk708->m_Name, sizeof(m_FriendNameWork));
                WindowYesNo_Button("CharaEntryMessage", "FMAKE_11_00", 0, m_FriendNameWork, 0, 1);
                m_nStep = STEP_DELETE_CONFIRM;
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_MINUS) {
        pObj->PushButton(padNo0, BTN_KIND_DELETE);
        memcpy(m_FriendNameWork, m_Unk708->m_Name, sizeof(m_FriendNameWork));
        WindowYesNo_Button("CharaEntryMessage", "FMAKE_11_00", 0, m_FriendNameWork, 0, 1);
        m_nStep = STEP_DELETE_CONFIRM;
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
        pObj->SetTabSelect(pObj->m_nTabSelect + 1, 1);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
        pObj->SetTabSelect(pObj->m_nTabSelect - 1, 1);
    } else {
        if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
            CancelVsFriend();
        }
        if (m_pVsFriend != 0) {
            SetNextMode(MODE_FRIEND_VS, pObj);
        } else if (!m_Unk708->IsBuddyComplete()) {
            // The friend has been taken off the other side's roster.
            GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        } else {
            InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        }
    }
}

void msw::CSelectMenu::FriendInfo_Out()
{
    static const long objId = OBJ_FRIEND_INFO;

    Scratch(objId);
}

void msw::CSelectMenu::FriendMatchChancelCheck()
{
    switch (m_nCancelStep) {
        case CANCEL_STEP_WAIT:
            if (CCockpit::GetResult() == RESULT_YES) {
                m_pGameDwc->Window_Pict1Close();
                SendObjEventTrigger(OBJ_CONNECTION_WAIT, EVENT_CONNECTION_CLOSE, 0);
                m_nCancelStep = CANCEL_STEP_CLOSE;
            }
            break;
        case CANCEL_STEP_CLOSE:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->ClearWindowResult();
                m_pGameDwc->WindowYesNo_Button("WifiConnectMessage", "FENT_01_00", 0, 0, 0, 0);
                m_nCancelStep = CANCEL_STEP_ASK;
            }
            break;
        case CANCEL_STEP_ASK:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    m_pGameDwc->ClearWindowResult();
                    m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                    m_nCancelStep = CANCEL_STEP_WAIT;
                    m_fInputWait = MATCH_ERROR_WAIT;
                    m_nStep = STEP_MATCH_CLOSE;
                    break;
                case RESULT_NO: {
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_pGameDwc->ClearWindowResult();
                    m_pGameDwc->Window_Picture1("WifiConnectMessage", "FVS_WAIT_00", 0);
                    SelectMenu::CConnectionWait *pWait =
                        new SelectMenu::CConnectionWait(OBJ_CONNECTION_WAIT, 0, 0.0f);
                    AddObj(pWait);
                    pWait->Set();
                    m_nCancelStep = CANCEL_STEP_WAIT;
                    break;
                }
            }
            break;
    }
}

void msw::CSelectMenu::NoFriendInfo_In()
{
    // static-order swap, as in FriendPlayerInfo_In().
    static const long objId = OBJ_NO_FRIEND_INFO;
    static const long padNo0 = 0;

    Build(objId);
    SendObjEventTrigger(OBJ_TOP, EVENT_TOP_CLEAR, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }
    m_nStep = NO_FRIEND_STEP_INPUT;
}

void msw::CSelectMenu::NoFriendInfo_Loop()
{
    dwc::CFriend *pFriend;
    bcs::dwc::CDwcFriend *pEntry;
    static const long padNo0 = 0;

    switch (m_nStep) {
        case NO_FRIEND_STEP_INPUT:
            NoFriendInfo_Main();
            break;
        case NO_FRIEND_STEP_CONFIRM:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    m_pGameDwc->ClearWindowResult();
                    m_nStep = NO_FRIEND_STEP_DELETE;
                    break;
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_pGameDwc->ClearWindowResult();
                    m_nStep = NO_FRIEND_STEP_INPUT;
                    break;
            }
            break;
        case NO_FRIEND_STEP_DELETE: {
            pFriend = m_Unk708;
            m_pFriendList->RemoveFriend(pFriend->m_nIndex);
            if (pFriend != 0) {
                m_pFriendList->NormalizeNo();
                // Copy the whole roster into the save data.
                for (long i = 0; i < CSave::PlayerData::FRIEND_MAX; i++) {
                    pEntry = m_pFriendList->GetFriend(i);
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
            }
            m_nStep = NO_FRIEND_STEP_SAVE;
            break;
        }
        case NO_FRIEND_STEP_SAVE:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nStep = NO_FRIEND_STEP_SAVE_WAIT;
            }
            break;
        case NO_FRIEND_STEP_SAVE_WAIT:
            if (CSave::IsComplete()) {
                WindowA_Button("CharaEntryMessage", "FMAKE_14", 0, 0);
                m_nStep = NO_FRIEND_STEP_DONE;
            }
            break;
        case NO_FRIEND_STEP_DONE:
            if (CCockpit::GetResult() == RESULT_YES) {
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_NO_FRIEND_INFO);
                m_pGameDwc->m_Dwc.SetFriendServersAsyncFlag(1);
                ((SelectMenu::CFriendList *)FindObj(OBJ_FRIEND_LIST))->m_nTopIndex = 0;
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
            break;
    }
}

void msw::CSelectMenu::NoFriendInfo_Main()
{
    // static-order swap, as in FriendPlayerInfo_In().
    static const long objId = OBJ_NO_FRIEND_INFO;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
        return;
    }
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        if (nBtnKindId == BTN_KIND_BACK) {
            pObj->PushButton(padNo0, BTN_KIND_BACK);
            GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        } else if (nBtnKindId == BTN_KIND_DELETE) {
            pObj->PushButton(padNo0, BTN_KIND_DELETE);
            WindowYesNo_Button("CharaEntryMessage", "FMAKE_12_00", 0, 0, 0, 1);
            m_nStep = NO_FRIEND_STEP_CONFIRM;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_MINUS) {
        pObj->PushButton(padNo0, BTN_KIND_DELETE);
        WindowYesNo_Button("CharaEntryMessage", "FMAKE_12_00", 0, 0, 0, 1);
        m_nStep = NO_FRIEND_STEP_CONFIRM;
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

void msw::CSelectMenu::NoFriendInfo_Out()
{
    static const long objId = OBJ_NO_FRIEND_INFO;

    Scratch(objId);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 0,
        ANIM_WIN_OUT = 2,
        // The online mark (PANE_ONLINE).
        ANIM_ONLINE_ON = 8,
        ANIM_ONLINE_LOOP = 9,
        ANIM_ONLINE_OFF = 0xA,
        ANIM_ONLINE_HIDE = 0xB,
        ANIM_ONLINE_IN = 0xC,
        ANIM_ONLINE_OUT = 0xD,
        // The tabs.
        ANIM_TAB_HIT_ON = 0x12,
        ANIM_TAB_HIT_OFF = 0x14,
        ANIM_TAB_SELECT = 0x15,
        ANIM_TAB_IDLE = 0x16,
        ANIM_TAB_UNSELECT = 0x17,
        ANIM_TAB_HIT_OFF_SELECTED = 0x18
    };

    enum {
        PANE_WIN = 0,
        PANE_TEXT_WIN = 0xB,         // to 0xD: the three counts of a tab
        PANE_TEXT_LOSE = 0xC,
        PANE_TEXT_DRAW = 0xD,
        PANE_TEXT_CODE = 0xE,        // the player's own friend code
        PANE_UNK_10 = 0x10,
        PANE_TEXT_FRIEND_NAME = 0x11,
        PANE_ONLINE_ON = 0x12,
        PANE_ONLINE_OFF = 0x13,
        PANE_UNK_15 = 0x15,
        PANE_MII = 0x16,
        PANE_RATE = 0x18,
        PANE_SCALE_TOP = 0x19,
        PANE_SCALE_ZERO = 0x1A,
        PANE_SCALE_DIGIT = 0x1D,     // and the two before it: the digits of the scale's top
        PANE_SCALE_TOP_BASE = 0x1E,
        PANE_COPY_PARENT = 0x1F,
        PANE_DIGIT_0 = 0x23,         // to 0x2C: one per digit
        PANE_ONLINE = 0x2D,
        PANE_MATCH = 0x2E,
        PANE_GRAPH = 0x2F,
        PANE_GRAPH_TOP = 0x30,
        PANE_OWN = 0x32,
        PANE_FRIEND = 0x33,
        PANE_TEXT_ONLINE_ON = 0x35,
        PANE_TEXT_ONLINE_OFF = 0x37,
        PANE_TEXT_FRIEND_CODE = 0x39,
        PANE_TEXT_HELP = 0x3F
    };

    const char *const s_LayoutName = "me_p_inf_a.brlyt";
    const char *const s_ButtonFileName = "me_p_inf_a.btn";

    // The tab panes.
    long s_TabPanes[msw::SelectMenu::CFriendInfo::TAB_NUM] = { 2, 4, 6, 8, 10 };

    // The digit panes, and the names of the copies made of them.
    const char *s_DigitPaneNames[10] = { "no_0", "no_1", "no_2", "no_3", "no_4",
                                         "no_5", "no_6", "no_7", "no_8", "no_9" };
    const char *s_CopyPaneNames[10] = { "cp_00", "cp_01", "cp_02", "cp_5", "cp_90",
                                        "cp_91", "cp_92", "cp_93", "", "" };

    // The panes of the four trophies.
    long s_TrofyPanes[4] = { 0x3E, 0x3C, 0x3B, 0x3D };

    enum {
        TROFY_NUM = 4,
        NUM_TEXT_LEN = 0x10,
        NUM_MAX = 9999
    };

    // Frames before the trophies sparkle.
    const float TROFY_WAIT = 60.0f;

    // Makes a line strip pane called pName, hung under pParent's pane with
    // pPaneCtrl; null when it cannot be allocated.
    inline bcs::G2D::CLineStripDL *CreateLineStrip(const char *pName)
    {
        bcs::G2D::CLineStripDL *pLine = nw4r::lyt::Layout::NewObj<bcs::G2D::CLineStripDL>();
        if (pLine != 0) {
            pLine->SetName(pName);
            pLine->SetUserAllocated(false);
        }
        return pLine;
    }
}

msw::SelectMenu::CFriendInfo::CFriendInfo(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_pMiiRender = 0;
}

msw::SelectMenu::CFriendInfo::~CFriendInfo()
{
    Remove();
}

void msw::SelectMenu::CFriendInfo::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // The game numbers this unit's statics the other way round:
            // static-order swap (see notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            if (m_nDeleteAnim != 0) {
                m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_IN, 1.0f, 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    bEnd = 1;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    bEnd = 1;
                }
            }
            if (bEnd) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (m_nDeleteAnim != 0) {
                    m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_LOOP, 1.0f, 0.0f, recursive, reset);
                }
                m_fTrofyWait = TROFY_WAIT;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
        case STATE_ACTIVE:
            CheckFrindStat();
            CheckButtonAnim();
            if (m_nPrevTab != -1 && m_pPaneAccessor->IsAnimEnd(s_TabPanes[m_nPrevTab], ANIM_TAB_UNSELECT)) {
                m_pPaneAccessor->PlayAnimation(s_TabPanes[m_nPrevTab], ANIM_TAB_IDLE, 1.0f, 0.0f, 1, 1);
                m_nPrevTab = -1;
            }
            if (m_fTrofyWait != 0.0f) {
                m_fTrofyWait -= bcs::CMainApp::GetAppFrameRateF();
                if (m_fTrofyWait <= 0.0f) {
                    CTrofy *pTrofy = static_cast<CTrofy *>(GetSelectMenu()->FindObj(OBJ_TROFY));
                    for (long i = 0; i < TROFY_NUM; i++) {
                        pTrofy->SetWiFiTrofyEffect(i, m_pPaneAccessor->GetPaneObj(s_TrofyPanes[i]));
                    }
                    m_fTrofyWait = 0.0f;
                }
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_IS_OUT, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                if (m_nDeleteAnim != 0) {
                    m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_OUT, 1.0f, 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_OUT, 0);
                m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_OFF, 0);
                m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_ON, 0);
                static_cast<CTrofy *>(GetSelectMenu()->FindObj(OBJ_TROFY))->ClearTrofyEffect();
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
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

void msw::SelectMenu::CFriendInfo::Disp()
{
}

int msw::SelectMenu::CFriendInfo::Set()
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
            const char *pLayoutName = s_LayoutName;
            pPaneCtrl->BuildLayout(0, pLayoutName);
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
                if ((unsigned short)SetUpButton(pPart, i, 0) != BTN_ID_MATCH) {
                    pPart->m_bUnk40_0020 = 0;
                }
                pPart->m_bUnk40_0004 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            m_pFriend = pSelectMenu->m_Unk708;

            for (long nPane = 0x19; nPane <= 0x2C; nPane++) {
                m_pPaneAccessor->SetPaneDisp(nPane, 0);
            }

            // The copies of the digit panes the scale of the graph uses.
            nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            {
                nw4r::lyt::Pane *pPane = DuplicateDigitPane(0, "cpy_0", pLayoutName);
                bcs::G2D::SetPaneParent(pPane, m_pPaneAccessor->GetPane(PANE_COPY_PARENT), 1, 0);
                m_pPaneAccessor->SetPaneDisp(PANE_COPY_PARENT, 1);
                pPane->SetTranslate(zero);
            }
            m_pZeroPane[0] = DuplicateDigitPane(0, s_CopyPaneNames[0], pLayoutName);
            m_pZeroPane[1] = DuplicateDigitPane(0, s_CopyPaneNames[1], pLayoutName);
            m_pZeroPane[2] = DuplicateDigitPane(0, s_CopyPaneNames[2], pLayoutName);
            m_pFivePane = DuplicateDigitPane(5, s_CopyPaneNames[3], pLayoutName);
            m_pNinePane[0] = DuplicateDigitPane(9, s_CopyPaneNames[4], pLayoutName);
            m_pNinePane[1] = DuplicateDigitPane(9, s_CopyPaneNames[5], pLayoutName);
            m_pNinePane[2] = DuplicateDigitPane(9, s_CopyPaneNames[6], pLayoutName);
            m_pNinePane[3] = DuplicateDigitPane(9, s_CopyPaneNames[7], pLayoutName);
            for (long i = 0; i < 8; i++) {
                m_pPaneAccessor->SetDisp(s_CopyPaneNames[i], 0);
            }

            // The line strips of the graph.
            m_pPrimLine = AppendLineStrip("PrimLine");
            m_pPrimShadow = AppendLineStrip("PrimShadow");
            m_pNumLine = AppendLineStrip("NumLine");
            m_pShadowLine = AppendLineStrip("ShadowLine");
            for (long i = 0; i < BORDER_LINE_NUM; i++) {
                m_pBorderLine[i] = AppendLineStrip("BorderLine");
                m_pSBorderLine[i] = AppendLineStrip("SBorderLine");
            }

            m_nOnlineStat = 0xFF;
            m_nDeleteAnim = 0;
            m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_HIDE, 1.0f, 0.0f, 1, 0);
            m_nTabSelect = -1;
            m_nPrevTab = -1;

            // From the friend roster, the delete button.
            if (pSelectMenu->m_nMode == 0x26) {
                pSelectMenu->Build(OBJ_PLUS_MINUS);
                CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
                pPlusMinus->StartUp(0, 0, GetMessageLabelString("List_Plus_Minus_Button", "Minus_02"));
            }

            pSelectMenu->Build(OBJ_TROFY);
            CTrofy *pTrofy = static_cast<CTrofy *>(pSelectMenu->FindObj(OBJ_TROFY));
            pTrofy->m_pFriend = m_pFriend;
            for (long i = 0; i < TROFY_NUM; i++) {
                pTrofy->SetWiFiTrofy(i, m_pPaneAccessor->GetPaneObj(s_TrofyPanes[i]));
            }

            SetTextPaneString(PANE_TEXT_HELP, GetMessageLabelString("WifiMenuMessage", "F_INF_14"));

            m_bVibEnable = 1;
            m_bSeEnable = 1;
        }
    }
    return 0;
}

// A copy, called pName, of the digit pane pSrcName; inlined into Set().
inline nw4r::lyt::Pane *msw::SelectMenu::CFriendInfo::DuplicateDigitPane(long nDigit, const char *pName,
                                                                         const char *pLayoutName)
{
    bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
    bcs::G2D::CPaneObj *pCopy = bcs::G2D::DuplicatePaneObj(pLytObj, s_DigitPaneNames[nDigit], pName, pLayoutName, 0);
    pLytObj->AppendPaneObj(pCopy, 0);
    return pCopy->GetPane();
}

// A new line strip pane called pName, hung under the graph pane; inlined
// into Set().
inline bcs::G2D::CLineStripDL *msw::SelectMenu::CFriendInfo::AppendLineStrip(const char *pName)
{
    bcs::G2D::CLineStripDL *pLine = CreateLineStrip(pName);
    if (pLine != 0) {
        bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
        bcs::G2D::CPaneObj *pGraph = m_pPaneAccessor->GetPaneObj(PANE_GRAPH);
        bcs::G2D::CPaneObj *pLineObj = bcs::G2D::CreatePaneObj(pLytObj, pLine);
        if (pLineObj != 0) {
            pLytObj->AppendPaneObj(pLineObj, 0);
            bcs::G2D::SetPaneParent(pLineObj->GetPane(), pGraph->GetPane(), 1, 0);
            pLineObj->GetPane()->SetVisible(true);
            pLineObj->GetPane()->SetScale(nw4r::math::VEC2(1.0f, 1.0f));
        }
    }
    return pLine;
}

// Inlined into Main() although defined after it; defined down here because
// its local static follows Set()'s constants in .rodata.
inline void msw::SelectMenu::CFriendInfo::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

// Puts pText into text box nPane when the pane exists.
inline void msw::SelectMenu::CFriendInfo::SetTextPaneString(long nPane, const wchar_t *pText)
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

// Writes nValue (at most 9999) into text box nPane when the pane exists.
inline void msw::SelectMenu::CFriendInfo::SetTextPaneString(long nPane, long nValue)
{
    static const bool recursive = true;

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
    nw4r::lyt::Pane *pPane =
        (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
    if (pPane != 0) {
        if (nValue >= NUM_MAX) {
            nValue = NUM_MAX;
        }
        wchar_t num[NUM_TEXT_LEN];
        swprintf(num, NUM_TEXT_LEN - 1, L"%d", nValue);
        m_pPaneAccessor->SetTextBoxString(nPane, num, wcslen(num));
    }
}

void msw::SelectMenu::CFriendInfo::Remove()
{
    GetSelectMenu()->Scratch(OBJ_TROFY);
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    if (m_pMiiRender != 0) {
        m_pMiiRender->Delete();
        m_pMiiRender = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CFriendInfo::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }

    // The pointer's highlight on the tabs.
    for (long i = 0; i < TAB_NUM; i++) {
        long nBtnKindId = BTN_KIND_TAB_0 + i;
        if (m_bTabHit[i] == 0 && nBtnKindId == nResult) {
            m_pPaneAccessor->PlayAnimation(s_TabPanes[i], ANIM_TAB_HIT_ON, 1.0f, 0.0f, 1, 0);
            m_bTabHit[i] = 1;
        }
        if (m_bTabHit[i] == 1 && nBtnKindId != nResult) {
            m_pPaneAccessor->StopAnimation(s_TabPanes[i], ANIM_TAB_HIT_ON, 1);
            if (i == m_nTabSelect) {
                m_pPaneAccessor->PlayAnimation(s_TabPanes[i], ANIM_TAB_HIT_OFF_SELECTED, 1.0f, 0.0f, 1, 1);
            } else {
                m_pPaneAccessor->PlayAnimation(s_TabPanes[i], ANIM_TAB_HIT_OFF, 1.0f, 0.0f, 1, 0);
            }
            m_bTabHit[i] = 0;
        }
    }
    return nResult;
}

long msw::SelectMenu::CFriendInfo::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendInfo::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendInfo::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CFriendInfo::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CFriendInfo::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
    // While the friend is online the stick always lands on the match button.
    if (m_nDeleteAnim != 0) {
        nNext = BTN_KIND_MATCH;
    }
    return nNext;
}

long msw::SelectMenu::CFriendInfo::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_MATCH;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CFriendInfo::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CFriendInfo::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    long nEvent = nPad + EVENT_PUSH;
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nEvent, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}

void msw::SelectMenu::CFriendInfo::SetUpDisp(int bFriend)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    RFLStoreData storeData;

    SetTabSelect(0, bFriend);
    if (bFriend) {
        dwc::CFriend *pFriend = m_pFriend;
        dwc::CFriend::StorageData *pData = pFriend->GetOwnStorageData();
        const wchar_t *pName = pFriend->m_Name;
        SetTextPaneString(PANE_TEXT_FRIEND_NAME, pName);
        SetTextPaneString(PANE_TEXT_FRIEND_CODE, pSelectMenu->ExchangeFriendCode(pFriend->m_nFriendCode));
        SetTextPaneString(PANE_TEXT_WIN, pData->m_TotalSkill.m_nWin);
        SetTextPaneString(PANE_TEXT_LOSE, pData->m_TotalSkill.m_nLose);
        SetTextPaneString(PANE_TEXT_DRAW, pData->m_TotalSkill.m_nDraw);
        m_pPaneAccessor->SetPaneDisp(PANE_OWN, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_FRIEND, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_UNK_10, 0);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 |= 1;
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        memcpy(&storeData, &pData->m_StoreData, sizeof(RFLStoreData));
        m_bFriend = 1;
    } else {
        SetTextPaneString(PANE_UNK_10, pPlayerData->m_Name);
        SetTextPaneString(PANE_TEXT_CODE, pSelectMenu->ExchangeFriendCode(pPlayerData->m_nFriendCode));
        m_pPaneAccessor->SetPaneDisp(PANE_OWN, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_FRIEND, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_ON, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_OFF, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_TEXT_FRIEND_NAME, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_UNK_15, 0);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 |= 1;
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        bcs::CMiiMan::GetStoreData(storeData, pPlayerData->m_MiiSource, pPlayerData->m_MiiCreateId);
        m_bFriend = 0;
    }

    // The Mii, rendered into a picture pane of the size of PANE_MII.
    nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(PANE_MII)->GetPane()->GetSize();
    m_pMiiRender = new CPlayerInfoRender(0, storeData, size.width, size.height);

    GXTexObj texObj;
    GXInitTexObj(&texObj, m_pMiiRender->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3, GX_CLAMP,
                 GX_CLAMP, 0);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
    menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(PANE_MII), "RENDER_MII");
    m_pMiiRender->SetDispState(1);

    if (bFriend) {
        CheckFrindStat();
    }
}

void msw::SelectMenu::CFriendInfo::CheckFrindStat()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    dwc::CFriend *pFriend = m_pFriend;

    if (m_bFriend == 0) {
        return;
    }

    if (m_nDeleteAnim == 1 && m_pPaneAccessor->IsAnimEnd(PANE_ONLINE, ANIM_ONLINE_ON)) {
        m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_LOOP, 1.0f, 0.0f, 1, 0);
        m_nDeleteAnim++;
    }

    if (m_nOnlineStat == pFriend->GetOnlineStat()) {
        if (pFriend->GetOnlineStat() != 1) {
            m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 |= 1;
            m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        }
        return;
    }

    if (pFriend->GetOnlineStat() == 0) {
        // Offline.
        SetTextPaneString(PANE_TEXT_ONLINE_OFF, GetMessageLabelString("WifiMenuMessage", "F_INF_13_01"));
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_OFF, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_ON, 0);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 |= 1;
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        if (m_pMiiRender != 0) {
            m_pMiiRender->SetDispState(0);
        }
        if (m_nDeleteAnim != 0) {
            m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_OFF, 1.0f, 0.0f, 1, 1);
            m_nDeleteAnim = 0;
        }
    } else if (pFriend->GetOnlineStat() == 1) {
        // Online and free: a match can be asked for.
        SetTextPaneString(PANE_TEXT_ONLINE_ON, GetMessageLabelString("WifiMenuMessage", "F_INF_13_00"));
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_OFF, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_ON, 1);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 &= ~1;
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 1);
        pSelectMenu->m_nCurrentBtnKindId[0][pSelectMenu->m_nMode] = BTN_KIND_MATCH;
        m_nPushButtonId[0] = BTN_KIND_MATCH;
        SetCursorMarkerPosition(0, BTN_KIND_MATCH);
        if (m_pMiiRender != 0) {
            m_pMiiRender->SetDispState(1);
        }
        if (m_nDeleteAnim == 0) {
            m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_ON, 1.0f, 0.0f, 1, 1);
            m_nDeleteAnim = 1;
        }
    } else {
        // Online but busy.
        SetTextPaneString(PANE_TEXT_ONLINE_ON, GetMessageLabelString("WifiMenuMessage", "F_INF_13_02"));
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_OFF, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_ONLINE_ON, 1);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_MATCH)->m_nFlags40 |= 1;
        m_pPaneAccessor->SetPaneDisp(PANE_MATCH, 0);
        if (m_pMiiRender != 0) {
            m_pMiiRender->SetDispState(2);
        }
        if (m_nDeleteAnim != 0) {
            m_pPaneAccessor->PlayAnimation(PANE_ONLINE, ANIM_ONLINE_OFF, 1.0f, 0.0f, 1, 1);
            m_nDeleteAnim = 0;
        }
    }
    m_nOnlineStat = pFriend->GetOnlineStat();
}

void msw::SelectMenu::CFriendInfo::SetTabSelect(long nTab, int bFriend)
{
    long nPrev = m_nTabSelect;
    if (nTab == nPrev) {
        return;
    }
    if ((unsigned long)nTab > TAB_NUM - 1) {
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

    m_nTabSelect = nTab;
    for (long i = 0; i < TAB_NUM; i++) {
        m_bTabHit[i] = 0;
        if (nTab == i) {
            m_pPaneAccessor->PlayAnimation(s_TabPanes[nTab], ANIM_TAB_SELECT, 1.0f, 0.0f, 1, 1);
        } else if (nPrev == i) {
            m_pPaneAccessor->PlayAnimation(s_TabPanes[nPrev], ANIM_TAB_UNSELECT, 1.0f, 0.0f, 1, 1);
        } else {
            m_pPaneAccessor->PlayAnimation(s_TabPanes[i], ANIM_TAB_IDLE, 1.0f, 0.0f, 1, 1);
        }
    }
    m_nPrevTab = nPrev;

    if (bFriend) {
        dwc::CFriend::StorageData *pData = m_pFriend->GetOwnStorageData();
        WiFiSkill *pSkill = 0;
        switch (nTab) {
            case 0:
                pSkill = &pData->m_TotalSkill;
                m_GraphColor = 0xA000DCFF;
                break;
            case 1:
                pSkill = &pData->m_Skill[0];
                m_GraphColor = 0xFF0000FF;
                break;
            case 2:
                pSkill = &pData->m_Skill[1];
                m_GraphColor = 0x00FF00FF;
                break;
            case 3:
                pSkill = &pData->m_Skill[2];
                m_GraphColor = 0xFFFF00FF;
                break;
            case 4:
                pSkill = &pData->m_Skill[3];
                m_GraphColor = 0x0000FFFF;
                break;
        }
        SetScorePos(pSkill);
        SetTextPaneString(PANE_TEXT_WIN, pSkill->m_nWin);
        SetTextPaneString(PANE_TEXT_LOSE, pSkill->m_nLose);
        SetTextPaneString(PANE_TEXT_DRAW, pSkill->m_nDraw);
    } else {
        CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
        WiFiSkill *pSkill = 0;
        switch (nTab) {
            case 0:
                pSkill = &pPlayerData->m_TotalSkill;
                m_GraphColor = 0xA000DCFF;
                break;
            case 1:
                pSkill = &pPlayerData->m_Skill[0];
                m_GraphColor = 0xFF0000FF;
                break;
            case 2:
                pSkill = &pPlayerData->m_Skill[1];
                m_GraphColor = 0x00FF00FF;
                break;
            case 3:
                pSkill = &pPlayerData->m_Skill[2];
                m_GraphColor = 0xFFFF00FF;
                break;
            case 4:
                pSkill = &pPlayerData->m_Skill[3];
                m_GraphColor = 0x0000FFFF;
                break;
        }
        SetScorePos(pSkill);
        SetTextPaneString(PANE_TEXT_WIN, pSkill->m_nWin);
        SetTextPaneString(PANE_TEXT_LOSE, pSkill->m_nLose);
        SetTextPaneString(PANE_TEXT_DRAW, pSkill->m_nDraw);
    }
}

void msw::SelectMenu::CFriendInfo::SetScorePos(WiFiSkill *pSkill)
{
    bcs::G2D::CPaneObj *pGraph = m_pPaneAccessor->GetPaneObj(PANE_GRAPH);
    bcs::G2D::CPaneObj *pRate = m_pPaneAccessor->GetPaneObj(PANE_RATE);
    unsigned short nMax = 1000;
    nw4r::lyt::Size size = pGraph->GetPane()->GetSize();
    nw4r::math::VEC3 ratePos = pRate->GetPane()->GetTranslate();
    nw4r::math::VEC2 bottom;
    bottom.y = -1.0f * (size.height * 0.5f);

    // The top of the scale: 300 over the best rating, rounded up to 500.
    long nTop = pSkill->m_nMaxRate + 300;
    if (nTop > 1000) {
        nMax = nTop + (500 - nTop % 500);
        if (nMax > NUM_MAX) {
            nMax = NUM_MAX;
        }
    }

    SetTextPaneString(PANE_RATE, pSkill->m_nRate);
    DispNumberPane(nMax);

    nw4r::math::VEC2 origin(-(size.width * 0.5f), -(size.height * 0.5f));
    SetSkillLine(pSkill, origin, size.width / 30.0f, nMax, size.height);

    for (long i = 0; i < BORDER_LINE_NUM; i++) {
        memset(m_BorderLine[i], 0, sizeof(m_BorderLine[i]));
        SetBorderSkillLine(i, s_BorderValue[i], nMax);
    }
}

void msw::SelectMenu::CFriendInfo::DispNumberPane(unsigned short nMax)
{
    for (long i = 0; i < 10; i++) {
        m_pPaneAccessor->SetDisp(s_CopyPaneNames[i], 0);
        m_pPaneAccessor->SetPaneDisp(PANE_DIGIT_0 + i, 0);
    }

    // The three lower digits of the top of the scale. Only 0, 5 and 9 can
    // occur (the top is a multiple of 500, or 9999).
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    for (long i = 0; i < 3; i++) {
        long nDigit = nMax % 10;
        long nPane = PANE_SCALE_DIGIT - i;
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(nPane);
        if (nDigit == 0) {
            bcs::G2D::SetPaneParent(m_pZeroPane[i], pParent, 1, 0);
            m_pZeroPane[i]->SetTranslate(zero);
            m_pPaneAccessor->SetDisp(s_CopyPaneNames[i], 1);
        } else if (nDigit == 5) {
            bcs::G2D::SetPaneParent(m_pFivePane, pParent, 1, 0);
            m_pFivePane->SetTranslate(zero);
            m_pPaneAccessor->SetDisp(s_CopyPaneNames[3], 1);
        } else if (nDigit == 9) {
            bcs::G2D::SetPaneParent(m_pNinePane[i], pParent, 1, 0);
            m_pNinePane[i]->SetTranslate(zero);
            m_pPaneAccessor->SetDisp(s_CopyPaneNames[4 + i], 1);
        }
        m_pPaneAccessor->SetPaneDisp(nPane, 1);
        nMax /= 10;
    }

    // The thousands digit is the original pane.
    nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_SCALE_ZERO);
    long nPane = PANE_DIGIT_0 + nMax;
    nw4r::lyt::Pane *pDigit = m_pPaneAccessor->GetPane(nPane);
    bcs::G2D::SetPaneParent(pDigit, pParent, 1, 0);
    pDigit->SetTranslate(zero);
    m_pPaneAccessor->SetPaneDisp(PANE_SCALE_ZERO, 1);
    m_pPaneAccessor->SetPaneDisp(nPane, 1);
    m_pPaneAccessor->SetPaneDisp(PANE_SCALE_TOP, 1);
    m_pPaneAccessor->SetPaneDisp(PANE_SCALE_TOP_BASE, 1);
}

namespace {

    // A pane's translation as an engine vector (same three floats), for the
    // vector routines of bcs/BCS_Vec.hpp.
    inline const bcs::FVEC3 &GetPaneTranslate(bcs::G2D::CPaneObj *pObj)
    {
        return reinterpret_cast<const bcs::FVEC3 &>(pObj->GetPane()->GetTranslate());
    }
}

void msw::SelectMenu::CFriendInfo::SetSkillLine(WiFiSkill *pSkill, nw4r::math::VEC2 &rOrigin, float fStepX,
                                                float fMax, float fHeight)
{
    m_nGraphPointNum = GRAPH_POINT_NUM;

    // The last GRAPH_POINT_NUM entries of the history, right-aligned.
    long nSkip = 0;
    if (pSkill->m_nHistoryNum >= GRAPH_POINT_NUM) {
        nSkip = pSkill->m_nHistoryNum - (GRAPH_POINT_NUM - 1);
    }
    long nEmpty = 0;
    if (pSkill->m_nHistoryNum < GRAPH_POINT_NUM) {
        nEmpty = (GRAPH_POINT_NUM - 1) - pSkill->m_nHistoryNum;
    }

    nw4r::math::VEC2 origin = rOrigin;
    float fBottom = -1.0f * (fHeight * 0.5f);
    long nHistory = nSkip;
    m_PrimLine[0].pos.x = origin.x;
    m_PrimLine[0].pos.y = origin.y;
    for (long i = 0; i < GRAPH_POINT_NUM; i++) {
        float fRate = 0.0f;
        if (i >= nEmpty) {
            if (fRate != pSkill->m_History[nHistory]) {
                fRate = (pSkill->m_History[nHistory] * 100) / fMax;
            }
            nHistory++;
        }
        float fLine = (fHeight * fRate) / 100.0f;
        m_PrimLine[i].pos.y = fBottom + fLine;
        m_PrimLine[i].color = m_GraphColor;
        if (i < GRAPH_POINT_NUM - 1) {
            m_PrimLine[i + 1].pos.x = fStepX + m_PrimLine[i].pos.x;
        }
    }

    nw4r::math::VEC3 linePos(0.0f, 1.0f, 0.0f);
    m_pPrimLine->SetVertexInfo(m_PrimLine, m_nGraphPointNum, 2);
    m_pPrimLine->m_Unk17C = 0;
    m_pPrimLine->SetTranslate(linePos);

    // The shadow: the same line in grey, one unit lower.
    memcpy(m_PrimShadow, m_PrimLine, sizeof(m_PrimShadow));
    for (long i = 0; i < GRAPH_POINT_NUM; i++) {
        m_PrimShadow[i].color = 0x32;
    }
    linePos.y -= 1.0f;
    m_pPrimShadow->SetVertexInfo(m_PrimShadow, m_nGraphPointNum, 2);
    m_pPrimShadow->m_Unk17C = 0;
    m_pPrimShadow->SetTranslate(linePos);

    // The line from the graph's top to the rating.
    nw4r::math::VEC3 numPos(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 ratePos(GetPaneTranslate(m_pPaneAccessor->GetPaneObj(PANE_RATE)));
    bcs::FVEC3Add(&ratePos, &ratePos, &GetPaneTranslate(m_pPaneAccessor->GetPaneObj(PANE_GRAPH_TOP)));
    bcs::FVEC3Sub(&ratePos, &ratePos, &GetPaneTranslate(m_pPaneAccessor->GetPaneObj(PANE_GRAPH)));
    m_NumLine[0].pos = m_PrimLine[GRAPH_POINT_NUM - 1].pos;
    m_NumLine[1].pos.x = ratePos.x;
    m_NumLine[1].pos.y = ratePos.y;
    m_NumLine[0].color = 0xFFFFFFFF;
    m_NumLine[1].color = 0xFFFFFFFF;
    m_pNumLine->SetVertexInfo(m_NumLine, 2, 3);
    m_pNumLine->m_Unk17C = 0;
    m_pNumLine->SetTranslate(numPos);

    memcpy(m_ShadowLine, m_NumLine, sizeof(m_ShadowLine));
    m_ShadowLine[0].color = 0x32;
    m_ShadowLine[1].color = 0x32;
    numPos.x += 1.0f;
    m_pShadowLine->SetVertexInfo(m_ShadowLine, 2, 2);
    m_pShadowLine->m_Unk17C = 0;
    m_pShadowLine->SetTranslate(numPos);
}

void msw::SelectMenu::CFriendInfo::SetBorderSkillLine(long nIndex, long nValue, float fMax)
{
    if (m_nTabSelect == 0) {
        return;
    }

    nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(PANE_GRAPH)->GetPane()->GetSize();
    float fRate = (nValue * 100) / fMax;
    if (fRate < 100.0f) {
        nw4r::math::VEC3 linePos(0.0f, 0.0f, 0.0f);
        float fHalfWidth = size.width * 0.5f;
        float fHeight = (size.height * fRate) / 100.0f;
        float fBottom = -1.0f * (size.height * 0.5f);
        float fY = fBottom + fHeight;
        m_BorderLine[nIndex][0].pos.y = fY;
        m_BorderLine[nIndex][1].pos.y = fY;
        m_BorderLine[nIndex][0].pos.x = -1.0f * fHalfWidth;
        m_BorderLine[nIndex][1].pos.x = fHalfWidth;
        m_BorderLine[nIndex][0].color = s_BorderColor[nIndex];
        m_BorderLine[nIndex][1].color = s_BorderColor[nIndex];
        m_pBorderLine[nIndex]->SetVertexInfo(m_BorderLine[nIndex], 2, 3);
        m_pBorderLine[nIndex]->m_Unk17C = 0;
        m_pBorderLine[nIndex]->SetTranslate(linePos);
    }
}

// The names in this namespace are guesses.
namespace {

    enum {
        NO_FRIEND_PANE_WIN = 0,
        NO_FRIEND_PANE_TEXT_CODE = 1,
        NO_FRIEND_ANIM_WIN_IN = 1,
        NO_FRIEND_ANIM_WIN_OUT = 0
    };

    const char *const s_NoFriendLayoutName = "me_p_inf_b.brlyt";
    const char *const s_NoFriendButtonFileName = "me_p_inf_b.btn";
}

msw::SelectMenu::CNoFriendInfo::CNoFriendInfo(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CNoFriendInfo::~CNoFriendInfo()
{
    Remove();
}

void msw::SelectMenu::CNoFriendInfo::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in CFriendInfo::Main().
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f,
                                               recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f,
                                               recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_IS_OUT, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_OUT, s_WinAnimSpeed[0],
                                                   0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_OUT, s_WinAnimSpeed[0],
                                                   0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_PLUS_MINUS_OUT, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(NO_FRIEND_PANE_WIN, NO_FRIEND_ANIM_WIN_OUT)) {
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

void msw::SelectMenu::CNoFriendInfo::Disp()
{
}

int msw::SelectMenu::CNoFriendInfo::Set()
{
    const wchar_t *pCode;
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
            ArchiveBuildLoader(s_NoFriendButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_NoFriendLayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            pCode = pSelectMenu->ExchangeFriendCode(pSelectMenu->m_Unk708->m_nFriendCode);
            m_pPaneAccessor->SetTextBoxString(NO_FRIEND_PANE_TEXT_CODE, pCode, wcslen(pCode));

            pSelectMenu->Build(OBJ_PLUS_MINUS);
            CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
            pPlusMinus->StartUp(0, 0, GetMessageLabelString("List_Plus_Minus_Button", "Minus_02"));
        }
    }
    return 0;
}

// Inlined into Main(); defined after Set() because its local static follows
// Main()'s in .rodata. Unlike CFriendInfo's, it does not ask the part list
// (the window has no buttons to animate).
inline void msw::SelectMenu::CNoFriendInfo::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CNoFriendInfo::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CNoFriendInfo::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CNoFriendInfo::PushButton(long nPad, long nBtnKindId)
{
    long nEvent = nPad + EVENT_PUSH;
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nEvent, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}
