// The Wi-Fi connection and the Wi-Fi top screen (CSelectMenu::WifiConnect_*,
// SelectMenu::CWiFiTopA) and the windows they open: the match level window
// (CConnectionLevel, used by the match-making screens), the "connecting"
// indicator (CConnectionWait), the new user's second message
// (CConnectNewMess02) and the other players' Miis window (CWiFiOtherMii).
// The headers are included in the order of the classes' code: the vtables
// come out in the reverse order in .data.
#include "game/msw/SelectMenu/CWiFiTopA.hpp"
#include "game/msw/SelectMenu/CConnectionLevel.hpp"
#include "game/msw/SelectMenu/CConnectionWait.hpp"
#include "game/msw/SelectMenu/CConnectNewMess02.hpp"
#include "game/msw/SelectMenu/CWiFiOtherMii.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/dwc/CDwcAPI.hpp"
#include "bcs/dwc/CDwcFriendList.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CPlayerMiiRender.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/SelectMenu/CTrofy.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"
#include "revolution/os/OSTime.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the top screen's opening and closing animations. A
    // table, not a scalar, as in game/menu/CWiFiGame.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_TASK = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_WIFI_TOP = 0x23,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_DWC_WINDOW = 0x3F,
        OBJ_UNK_40 = 0x40,
        OBJ_TROFY = 0x4D,
        OBJ_WINDOW = 0xFF // CConnectNewMess02, CConnectionWait, CWiFiOtherMii
    };

    // Events sent to the menu objects.
    enum {
        EVENT_TASK_UNK_0B = 0x0B,
        EVENT_LINE_MESSAGE = 0x11, // to OBJ_TOP: the help line
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21,         // plus the pad number
        EVENT_OUT_START = 0x25,
        EVENT_IS_OUT_READY = 0x26,
        EVENT_CONNECT_START = 0x29, // to OBJ_TASK
        EVENT_CONNECT_END = 0x2A,   // to OBJ_TASK
        EVENT_WINDOW_CLOSE = 0x2E,  // to OBJ_WINDOW
        EVENT_DWC_WINDOW_UNK_31 = 0x31,
        EVENT_DWC_WINDOW_CLOSE = 0x32,
        EVENT_BG_CHR = 0x33,
        EVENT_COURT_CHR = 0x35,
        EVENT_UNK_47 = 0x47
    };

    // The button kind ids of the Wi-Fi top screen and of the windows.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_TOP_0 = 0x8010, // anybody match; the level window's "yes"
        BTN_KIND_TOP_1 = 0x8011, // the level window's "no"
        BTN_KIND_TOP_2 = 0x8012,
        BTN_KIND_TOP_3 = 0x8013, // the player's own information
        BTN_KIND_OTHER_MII_BACK = 0x8020,
        BTN_KIND_OTHER_MII_NEXT = 0x8021,
        BTN_KIND_OTHER_MII = 0x80A1
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_OTHER_MII = 0x40
    };

    // Values of CSelectMenu::m_nStep while the screens are shown.
    enum {
        STEP_NEW_WAIT = 0,       // a new user: the first message
        STEP_NEW_01 = 1,
        STEP_NEW_02 = 2,
        STEP_NEW_02_CLOSE = 3,
        STEP_NEW_03 = 4,
        STEP_CONNECT_WAIT = 10,
        STEP_CONNECT = 11,
        STEP_TOP_IN = 20,
        STEP_TOP = 21,
        STEP_END_ASK = 30,       // the back button: end the connection?
        STEP_END_ANSWER = 31,
        STEP_END = 32,
        STEP_FADE = 33,
        STEP_FADE_WAIT = 34,
        STEP_OTHER_MII = 40,
        STEP_OTHER_MII_PAGE = 41,
        STEP_OTHER_MII_CLOSE = 42,
        STEP_OTHER_MII_ANSWER = 43,
        STEP_OTHER_MII_DONE = 44,
        STEP_OTHER_MII_END = 45,
        STEP_ERROR = 99
    };

    // Values of CSelectMenu::m_nNextMode set here.
    enum {
        MODE_TOP = 4,
        MODE_WIFI_GAME = 0x1E,
        MODE_WIFI_ENTRY_23 = 0x23,
        MODE_WIFI_ENTRY_24 = 0x24,
        MODE_WIFI_ENTRY_25 = 0x25,
        MODE_FRIEND_VS = 0x2A
    };

    // Answers of the message windows (CCockpit::GetResult).
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };

    // The cockpit objects CCockpit::IsEndObj is asked about.
    enum {
        COCKPIT_OBJ_FADE = 0x1042,
        COCKPIT_OBJ_WINDOW = 0x101A
    };

    // What WifiConnectStep returns.
    enum {
        CONNECT_ERROR_START = -2,
        CONNECT_ERROR = -1,
        CONNECT_BUSY = 0,
        CONNECT_DONE = 1
    };

    // Values of bcs::dwc::CDwc::GetStat() tested here (names guessed).
    enum {
        DWC_STAT_LOGIN = 2,
        DWC_STAT_READY = 4,
        DWC_STAT_ERROR = 13
    };

    // The camera animation of the screens.
    enum {
        CAMERA_ANIM = 0x29
    };

    enum {
        SE_DECIDE = 0xBBB,
        TOP_LINE_LEN = 0x100
    };
}

void msw::CSelectMenu::WifiConnect_In()
{
    // Not used.
    static const long padNo0 = 0;

    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
    m_bStarMii = 0;
    bcs::BCS_SRand(OSGetTime());
    SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR, 0);
    SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, (void *)1);
    CCameraMenuBG::s_Instance.Play(CAMERA_ANIM, 1.0f, 0.0f);

    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    m_pPlayerData = CSave::GetPlayerData(-1);
    if (!m_pPlayerData->IsValid()) {
        SetNextMode(MODE_TOP, 0.0f);
        return;
    }

    m_bWifiNewUser = 0;
    m_pGameDwc = dwc::CGameDwc::GetInstance();
    if (m_pGameDwc != 0 && m_pGameDwc->m_Dwc.IsConnect()) {
        // Already connected: straight to the top screen.
        m_nStep = STEP_TOP_IN;
        return;
    }

    if (m_pPlayerData->m_nFriendCode != 0) {
        m_fInputWait = 30.0f;
        m_nStep = STEP_CONNECT_WAIT;
    } else {
        // No friend code yet: the first connection.
        m_bWifiNewUser = 1;
        m_fInputWait = 60.0f;
        m_nStep = STEP_NEW_WAIT;
    }
}

void msw::CSelectMenu::WifiConnect_Loop()
{
    switch (m_nStep) {
        case STEP_NEW_WAIT:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                SendObjEventTrigger(OBJ_TASK, EVENT_CONNECT_START, 0);
                WindowA_Button("WifiConnectMessage", "WiFi_NEW_01", 0, 0);
                m_nStep = STEP_NEW_01;
            }
            break;
        case STEP_NEW_01:
            if (CCockpit::GetResult() == RESULT_YES) {
                SelectMenu::CConnectNewMess02 *pMess = new SelectMenu::CConnectNewMess02(OBJ_WINDOW);
                AddObj(pMess);
                pMess->Set();
                m_nStep = STEP_NEW_02;
            }
            break;
        case STEP_NEW_02: {
            static const long padNo0 = 0;

            if (CMenuPad::s_Instance.GetInput(padNo0) & PAD_A) {
                SendObjEventTrigger(OBJ_WINDOW, EVENT_WINDOW_CLOSE, 0);
                menu::CSndEff::s_Instance.Play(SE_DECIDE, 1);
                m_nStep = STEP_NEW_02_CLOSE;
            }
            break;
        }
        case STEP_NEW_02_CLOSE:
            if (SendObjEventTrigger(OBJ_WINDOW, EVENT_WINDOW_CLOSE, 0) == 0) {
                RemoveObj(OBJ_WINDOW);
                WindowYesNo_Button("WifiConnectMessage", "WiFi_NEW_03_00", 1, 0, 0, 1);
                m_nStep = STEP_NEW_03;
            }
            break;
        case STEP_NEW_03:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                    m_nStep = STEP_CONNECT_WAIT;
                    break;
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                    m_nStep = STEP_FADE;
                    break;
            }
            break;
        case STEP_CONNECT_WAIT:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                SendObjEventTrigger(OBJ_TASK, EVENT_CONNECT_START, 0);
                dwc::CGameDwc::Initialize();
                m_pGameDwc = dwc::CGameDwc::GetInstance();
                if (m_pGameDwc != 0) {
                    m_pGameDwc->Window_Picture0("WifiConnectMessage", "WiFi_B_00");
                    SelectMenu::CConnectionWait *pWait = new SelectMenu::CConnectionWait(OBJ_WINDOW, 0, -16.0f);
                    AddObj(pWait);
                    pWait->Set();
                    m_nWifiConnectStep = 0;
                    m_nStep = STEP_CONNECT;
                }
            }
            break;
        case STEP_CONNECT:
            switch (WifiConnectStep()) {
                case CONNECT_ERROR_START:
                case CONNECT_ERROR:
                    SendObjEventTrigger(OBJ_TASK, EVENT_CONNECT_END, 0);
                    RemoveObj(OBJ_WINDOW);
                    m_pGameDwc->Window_Pict0Close();
                    m_nStep = STEP_ERROR;
                    break;
                case CONNECT_DONE:
                    SendObjEventTrigger(OBJ_TASK, EVENT_CONNECT_END, 0);
                    RemoveObj(OBJ_WINDOW);
                    m_pGameDwc->Window_Pict0Close();
                    m_nStep = STEP_TOP_IN;
                    break;
            }
            break;
        case STEP_TOP_IN: {
            static const long objId = OBJ_WIFI_TOP;
            static const long padNo0 = 0;

            CGamePadCheck::GetInstance()->EnableBatteryCheck(1);
            Build(objId);
            SendObjEventTrigger(OBJ_TASK, EVENT_TASK_UNK_0B, 0);
            m_Unk27B = 0;
            ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

            CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
            if (pObj != 0) {
                unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
                long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
                rBtnKindId = nBtnKindId;
                pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
                if (m_Unk23E == 1) {
                    pObj->m_nPushButtonId[padNo0] = nBtnKindId;
                    WifiConnect_TopLineMess(nBtnKindId);
                } else {
                    WifiConnect_TopLineMess(BTN_KIND_NONE);
                }
                SetFadeIn(30.0f);
                m_nStep = STEP_TOP;
            }
            // Falls through: the top screen starts at once.
        }
        case STEP_TOP: {
            static const long objId = OBJ_WIFI_TOP;
            static const long padNo0 = 0;

            long nMode = m_nMode;
            long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

            // Returns, not breaks: the original's branches for these
            // differ from a break's.
            if (!CCockpit::IsEndObj(COCKPIT_OBJ_FADE)) {
                return;
            }
            CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
            if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                return;
            }

            unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            m_nWifiTopBtnKindId = rBtnKindId;

            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                pObj->PushButton(padNo0, m_nWifiTopBtnKindId);
                float fWait = _GetModeChangeWaitTime();
                switch (m_nWifiTopBtnKindId) {
                    case BTN_KIND_TOP_0:
                        m_bStarMii = 0;
                        SetNextMode(MODE_WIFI_GAME, pObj);
                        break;
                    case BTN_KIND_TOP_1:
                        SetNextMode(MODE_WIFI_ENTRY_24, pObj);
                        break;
                    case BTN_KIND_TOP_2:
                        SetNextMode(MODE_WIFI_ENTRY_23, pObj);
                        break;
                    case BTN_KIND_TOP_3:
                        SetNextMode(MODE_WIFI_ENTRY_25, pObj);
                        break;
                    case BTN_KIND_OTHER_MII:
                        m_nStep = STEP_OTHER_MII;
                        break;
                    default:
                        // The original passes the current mode here.
                        SetNextMode(nMode, fWait);
                        break;
                    case BTN_KIND_BACK:
                        m_nStep = STEP_END_ASK;
                        break;
                    case BTN_KIND_NONE:
                        break;
                }
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
                pObj->PushButton(padNo0, BTN_KIND_BACK);
                m_nStep = STEP_END_ASK;
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_OTHER_MII) {
                pObj->PushButton(padNo0, BTN_KIND_OTHER_MII);
                m_nStep = STEP_OTHER_MII;
            } else {
                // A friend has asked for a match.
                if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
                    CancelVsFriend();
                }
                if (m_pVsFriend != 0) {
                    SetNextMode(MODE_FRIEND_VS, pObj);
                } else {
                    InputDataToMenuObj(pObj, padNo0, nPad, nMode);
                    if (m_nWifiTopBtnKindId != rBtnKindId) {
                        WifiConnect_TopLineMess(rBtnKindId);
                    }
                }
            }
            break;
        }
        case STEP_END_ASK:
            WindowYesNo_Button("WifiConnectMessage", "WiFi_C_00", 0, 0, 0, 1);
            m_nStep = STEP_END_ANSWER;
            break;
        case STEP_END_ANSWER:
            if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
                CancelVsFriend();
            }
            if (m_pVsFriend != 0) {
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                WindowYesNo_Close();
                SetNextMode(MODE_FRIEND_VS, (CSelectMenuLayoutBase *)FindObj(OBJ_WIFI_TOP));
                break;
            }
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                    m_pGameDwc->ClearWindowResult();
                    if (m_pGameDwc->m_Dwc.Finish()) {
                        SelectMenu::CConnectionWait *pWait = new SelectMenu::CConnectionWait(OBJ_WINDOW, 1, -16.0f);
                        AddObj(pWait);
                        pWait->Set();
                        m_pGameDwc->Window_Picture0("WifiConnectMessage", "WVS_WAIT_06");
                        m_fInputWait = 120.0f;
                        m_nStep = STEP_END;
                    } else {
                        m_nStep = STEP_END_ASK;
                    }
                    break;
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                    m_pGameDwc->ClearWindowResult();
                    m_nCurrentBtnKindId[0][m_nMode] = m_nWifiTopBtnKindId;
                    m_nStep = STEP_TOP;
                    break;
            }
            break;
        case STEP_END:
            m_fInputWait -= GetFrameRateF();
            if (m_pGameDwc->m_Dwc.IsFinishComplete() && m_fInputWait < 0.0f) {
                m_pGameDwc->Window_Pict0Close();
                RemoveObj(OBJ_WINDOW);
                dwc::CGameDwc::Finalize();
                CancelVsFriend();
                m_nStep = STEP_FADE;
            }
            break;
        case STEP_FADE:
            SetWhiteFadeOut(10.0f);
            m_nStep = STEP_FADE_WAIT;
            break;
        case STEP_FADE_WAIT:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                static const long padNo0 = 0;

                CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_WIFI_TOP);
                ClearMenuHistory();
                PushMenuHistory(1, 2, 0);
                PushMenuHistory(2, MODE_TOP, 0);
                PlayMenuBGM(1);

                // Only the leader is marked.
                for (long i = 0; i < 4; i++) {
                    CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 &= ~4;
                }
                CMenuPadInfo::s_Instance.m_aPad[CMenuPadInfo::s_Instance.GetPadLeader()].m_nFlags04 |= 4;

                SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, 0);
                if (pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
                    GoBackToMode(padNo0, MODE_TOP, 0, 1);
                } else {
                    GoBackToMode(padNo0, MODE_TOP, pObj, 1);
                }
            }
            break;
        case STEP_OTHER_MII: {
            static const long padNo0 = 0;

            long nMode = m_nMode;
            SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_UNK_31, (void *)0x6E);
            SelectMenu::CWiFiOtherMii *pOtherMii = new SelectMenu::CWiFiOtherMii(OBJ_WINDOW);
            AddObj(pOtherMii);
            pOtherMii->Set();
            long nBtnKindId = pOtherMii->SetDefaultMoveCursor(padNo0, BTN_KIND_OTHER_MII_NEXT);
            m_nCurrentBtnKindId[padNo0][nMode] = nBtnKindId;
            pOtherMii->m_nPushButtonId[padNo0] = nBtnKindId;
            pOtherMii->SetCursorMarkerPosition(padNo0, nBtnKindId);
            m_nCupBtnKindId[0] = BTN_KIND_OTHER_MII_NEXT;
            m_nStep = STEP_OTHER_MII_PAGE;
            break;
        }
        case STEP_OTHER_MII_PAGE: {
            static const long padNo0 = 0;

            long nMode = m_nMode;
            long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
            SelectMenu::CWiFiOtherMii *pOtherMii = (SelectMenu::CWiFiOtherMii *)FindObj(OBJ_WINDOW);
            unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            unsigned short nBtnKindId = rBtnKindId;

            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                pOtherMii->PushButton(padNo0, nBtnKindId);
                switch (nBtnKindId) {
                    case BTN_KIND_OTHER_MII_NEXT:
                        if (pOtherMii->m_nPage == 0) {
                            pOtherMii->SetPage(1);
                        } else {
                            pOtherMii->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
                            m_nStep = STEP_OTHER_MII_CLOSE;
                        }
                        break;
                    case BTN_KIND_OTHER_MII_BACK:
                        if (pOtherMii->m_nPage == 1) {
                            pOtherMii->SetPage(0);
                        } else {
                            SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                            RemoveObj(OBJ_WINDOW);
                            rBtnKindId = m_nWifiTopBtnKindId;
                            m_nStep = STEP_TOP;
                        }
                        break;
                }
            } else {
                InputDataToMenuObj(pOtherMii, padNo0, nPad, nMode);
            }
            break;
        }
        case STEP_OTHER_MII_CLOSE:
            if (FindObj(OBJ_WINDOW) == 0) {
                WindowYesNo_Button("CharaEntryMessage", "OTHER_MII_03_01", 1, 0, 0, 1);
                m_nStep = STEP_OTHER_MII_ANSWER;
            }
            break;
        case STEP_OTHER_MII_ANSWER:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    m_pPlayerData->m_nOtherMii = 0;
                    m_nStep = STEP_OTHER_MII_DONE;
                    break;
                case RESULT_NO:
                    m_pPlayerData->m_nOtherMii = 1;
                    m_nStep = STEP_OTHER_MII_DONE;
                    break;
            }
            break;
        case STEP_OTHER_MII_DONE:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_WINDOW)) {
                if (m_pPlayerData->m_nOtherMii == 0) {
                    WindowA_Button("CharaEntryMessage", "OTHER_MII_04", 0, 0);
                } else {
                    WindowA_Button("CharaEntryMessage", "OTHER_MII_05", 0, 0);
                }
                m_nStep = STEP_OTHER_MII_END;
            }
            break;
        case STEP_OTHER_MII_END:
            if (CCockpit::GetResult() == RESULT_YES) {
                static const long padNo0 = 0;

                long nMode = m_nMode;
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_DWC_WINDOW_CLOSE, 0);
                m_nCurrentBtnKindId[padNo0][nMode] = m_nWifiTopBtnKindId;
                m_nStep = STEP_TOP;
            }
            break;
    }
}

void msw::CSelectMenu::WifiConnect_Out()
{
    static const long objId = OBJ_WIFI_TOP;

    Scratch(objId);
    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
}

void msw::CSelectMenu::WifiConnect_TopLineMess(long nBtnKindId)
{
    switch (nBtnKindId) {
        case BTN_KIND_TOP_0:
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "WTOP_04_00"));
            break;
        case BTN_KIND_TOP_1:
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "WTOP_04_01"));
            break;
        case BTN_KIND_TOP_2:
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "WTOP_04_04"));
            break;
        case BTN_KIND_TOP_3: {
            // The player's own information, with the player's name.
            wchar_t text[TOP_LINE_LEN];
            swprintf(text, TOP_LINE_LEN - 1, GetMessageLabelString("WifiMenuMessage", "WTOP_04_02"),
                     CSave::GetPlayerData(-1)->m_Name);
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, text);
            break;
        }
        default:
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("WifiMenuMessage", "WTOP_04_03"));
            break;
    }
}

long msw::CSelectMenu::WifiConnectStep()
{
    long nResult = CONNECT_BUSY;

    switch (m_nWifiConnectStep) {
        case 0:
            // The saved friends into the connection's roster.
            m_pFriendList = m_pGameDwc->m_Dwc.GetFriendList();
            for (long i = 0; i < SAVE_DATA_PLAYER::FRIEND_MAX; i++) {
                SAVE_DATA_PLAYER::Friend &rFriend = m_pPlayerData->m_Friend[i];
                if (rFriend.m_nFriendCode != 0) {
                    m_pFriendList->WriteBack(&rFriend.m_FriendData, rFriend.m_nFriendCode, rFriend.m_Unk30,
                                             rFriend.m_Unk34, rFriend.m_Data, sizeof(rFriend.m_Data));
                }
            }
            m_nWifiConnectStep = 1;
            break;
        case 1: {
            const wchar_t *apName[2];
            apName[0] = m_pPlayerData->m_Name;
            if ((long)m_nPlayerNum == 2) {
                WiFi_Get2PMiiName();
                apName[1] = m_Wifi2PName;
            }
            m_pGameDwc->m_Dwc.Login(&m_pPlayerData->m_AccUserData, apName, m_nPlayerNum);
            m_nWifiConnectStep = 2;
            break;
        }
        case 2: {
            long nStat = m_pGameDwc->m_Dwc.GetStat();
            if (nStat == DWC_STAT_LOGIN) {
                m_pGameDwc->m_Dwc.GetUserData(&m_pPlayerData->m_AccUserData);
                bcs::dwc::CDwcAPI::ReportUserData(&m_pPlayerData->m_AccUserData);
                if (m_pGameDwc->m_Dwc.IsDirtyUserData()) {
                    m_pGameDwc->m_Dwc.ResetDirtyUserData();
                }
                m_pPlayerData->m_nFriendCode = m_pGameDwc->m_Dwc.GetFriendCode();
                if (!m_pGameDwc->m_Dwc.Start()) {
                    return CONNECT_ERROR;
                }
                m_nWifiConnectStep = 3;
            } else if (nStat == DWC_STAT_ERROR) {
                return CONNECT_ERROR;
            }
            break;
        }
        case 3: {
            long nStat = m_pGameDwc->m_Dwc.GetStat();
            if (nStat == DWC_STAT_READY) {
                m_nWifiConnectStep = 4;
            } else if (nStat == DWC_STAT_ERROR) {
                return CONNECT_ERROR;
            }
            break;
        }
        case 4:
            m_pGameDwc->m_Dwc.SetOwnStatusAppData(m_OwnStatusAppData, sizeof(m_OwnStatusAppData));
            m_nWifiConnectStep = 5;
            break;
        case 5: {
            if (m_bWifiNewUser) {
                // A new player's public data.
                memset(&m_pPlayerData->m_nPublicTag, 0, SAVE_DATA_PLAYER::PUBLIC_DATA_SIZE);
                SAVE_DATA_PLAYER *pData = m_pPlayerData;
                pData->m_nPublicTag = SAVE_DATA_PLAYER::PUBLIC_DATA_TAG;
                memset(pData->m_PublicName, 0, sizeof(pData->m_PublicName));
                memset(&pData->m_PublicMii, 0, sizeof(pData->m_PublicMii));
                memset(pData->m_Unk00F0, 0, sizeof(pData->m_Unk00F0));
                m_pPlayerData->m_Skill[0].m_nPoint = 1000;
                m_pPlayerData->m_Skill[1].m_nPoint = 1000;
                m_pPlayerData->m_Skill[2].m_nPoint = 1000;
                m_pPlayerData->m_Skill[3].m_nPoint = 1000;
            }
            RFLStoreData mii;
            bcs::CMiiMan::GetStoreData(mii, m_pPlayerData->m_MiiSource, m_pPlayerData->m_MiiCreateId);
            m_pPlayerData->m_PublicMii = mii;
            wcscpy(m_pPlayerData->m_PublicName, m_pPlayerData->m_Name);
            m_pGameDwc->m_Dwc.SaveEntryOwnPublicData(&m_pPlayerData->m_nPublicTag, SAVE_DATA_PLAYER::PUBLIC_DATA_SIZE);
            m_pFriendList->LoadStorageAll();
            m_nWifiConnectStep = 6;
            break;
        }
        case 6:
            if (m_pGameDwc->m_Dwc.IsStorageComplete()) {
                m_nWifiConnectStep = 7;
            }
            break;
        case 7:
            m_pGameDwc->m_Dwc.SetFriendServersAsyncFlag(1);
            m_nWifiConnectStep = 8;
            break;
        case 8:
            if (m_pGameDwc->m_Dwc.GetFriendServersAsyncCount() >= 1) {
                m_nWifiConnectStep = 9;
            }
            break;
        case 9:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nWifiConnectStep = 10;
            }
            break;
        case 10:
            if (CSave::IsComplete()) {
                nResult = CONNECT_DONE;
            }
            break;
    }
    return nResult;
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    const char *const s_TopLayoutName = "me_Wi-Fi_top_A.brlyt";
    const char *const s_TopButtonFileName = "me_Wi-Fi_top_A.btn";

    // The top screen's trophy panes (by sport).
    long s_TrofyPanes[4] = { 9, 11, 10, 12 };

    enum {
        TOP_PANE_WIN = 0,
        TOP_PANE_TEXT_NAME = 7,
        TOP_PANE_MII = 8,
        TOP_PANE_TEXT_0 = 0xF,
        TOP_PANE_TEXT_1 = 0x10,
        TOP_PANE_TEXT_2 = 0x11,
        TOP_PANE_TEXT_3 = 0x12
    };

    enum {
        TOP_ANIM_IN = 0,
        TOP_ANIM_LOOP = 1,
        TOP_ANIM_OUT = 2,
        TOP_ANIM_IN_BACK = 3,
        TOP_ANIM_OUT_BACK = 4
    };
}

msw::SelectMenu::CWiFiTopA::CWiFiTopA(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_pMiiRender = 0;
}

msw::SelectMenu::CWiFiTopA::~CWiFiTopA()
{
    Remove();
}

void msw::SelectMenu::CWiFiTopA::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(TOP_PANE_WIN, TOP_ANIM_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(TOP_PANE_WIN, TOP_ANIM_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TOP_PANE_WIN, TOP_ANIM_IN_BACK)) {
                    bEnd = 1;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(TOP_PANE_WIN, TOP_ANIM_IN)) {
                bEnd = 1;
            }
            if (bEnd) {
                m_pPaneAccessor->PlayAnimation(TOP_PANE_WIN, TOP_ANIM_LOOP, s_WinAnimSpeed[0], 0.0f, 1, 0);

                CTrofy *pTrofy = static_cast<CTrofy *>(GetSelectMenu()->FindObj(OBJ_TROFY));
                for (long i = 0; i < 4; i++) {
                    pTrofy->SetWiFiTrofyEffect(i, m_pPaneAccessor->GetPaneObj(s_TrofyPanes[i]));
                }
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case 6:
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_IS_OUT_READY, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(TOP_PANE_WIN, TOP_ANIM_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive,
                                                   reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(TOP_PANE_WIN, TOP_ANIM_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_OUT_START, 0);
                static_cast<CTrofy *>(GetSelectMenu()->FindObj(OBJ_TROFY))->ClearTrofyEffect();
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (IsBackFrameOut(0)) {
                if (m_pPaneAccessor->IsAnimEnd(TOP_PANE_WIN, TOP_ANIM_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(TOP_PANE_WIN, TOP_ANIM_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
        default:
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CWiFiTopA::Disp()
{
}

int msw::SelectMenu::CWiFiTopA::Set()
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
            ArchiveBuildLoader(s_TopButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_TopLayoutName);
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

            // The player's Mii, rendered into a picture pane of the size of
            // TOP_PANE_MII, and name.
            SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
            RFLStoreData mii = pPlayerData->GetMiiStoreData();
            nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(TOP_PANE_MII)->GetPane()->GetSize();
            m_pMiiRender = new CPlayerMiiRender(0, mii, size.width, size.height);

            GXTexObj texObj;
            GXInitTexObj(&texObj, m_pMiiRender->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                         GX_CLAMP, GX_CLAMP, 0);
            GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
            bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
            menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(TOP_PANE_MII), "RENDER_MII");
            m_pPaneAccessor->SetTextBoxString(TOP_PANE_TEXT_NAME, pPlayerData->m_Name, wcslen(pPlayerData->m_Name));

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            pSelectMenu->Build(OBJ_TROFY);
            CTrofy *pTrofy = static_cast<CTrofy *>(pSelectMenu->FindObj(OBJ_TROFY));
            for (long i = 0; i < 4; i++) {
                pTrofy->SetWiFiTrofy(i, m_pPaneAccessor->GetPaneObj(s_TrofyPanes[i]));
            }

            const wchar_t *pText = GetMessageLabelString("WifiMenuMessage", "WTOP_02");
            m_pPaneAccessor->SetTextBoxString(TOP_PANE_TEXT_0, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiMenuMessage", "WTOP_03");
            m_pPaneAccessor->SetTextBoxString(TOP_PANE_TEXT_1, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiMenuMessage", "WTOP_05");
            m_pPaneAccessor->SetTextBoxString(TOP_PANE_TEXT_2, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiMenuMessage", "WTOP_01");
            m_pPaneAccessor->SetTextBoxString(TOP_PANE_TEXT_3, pText, wcslen(pText));

            // The help line of the plus button.
            CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
            pSelectMenu->Build(OBJ_PLUS_MINUS);
            pPlusMinus->StartUp(1, GetMessageLabelString("List_Plus_Minus_Button", "Plus_06"), 0);
        }
    }
    return 0;
}

// Inlined into Main(); defined after Set() because its local static
// follows Set()'s constants in .rodata. Not declared inline: an inline
// definition would drop the static, which the original has (the
// out-of-line copy is stripped by the link, as CTournamentLoad's).
void msw::SelectMenu::CWiFiTopA::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CWiFiTopA::Remove()
{
    GetSelectMenu()->Scratch(OBJ_TROFY);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
    if (m_pMiiRender != 0) {
        m_pMiiRender->Delete();
        m_pMiiRender = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CWiFiTopA::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_HIT, pPos);
    }
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CWiFiTopA::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiTopA::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiTopA::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiTopA::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CWiFiTopA::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CWiFiTopA::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_TOP_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CWiFiTopA::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CWiFiTopA::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    long nEvent = nPad + EVENT_PUSH;
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nEvent, (void *)nBtnKindId);
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return 0;
}

// The names in this namespace are guesses.
namespace {

    const char *const s_LevelLayoutName = "WIN_G.brlyt";
    const char *const s_LevelButtonFileName = "WIN_G.btn";

    // The level window's stars: lit (s_LevelStarOnPanes) up to the match
    // level, unlit above it.
    long s_LevelStarOnPanes[4] = { 8, 9, 10, 11 };
    long s_LevelStarOffPanes[4] = { 12, 13, 14, 15 };

    enum {
        LEVEL_PANE_WIN = 0,
        LEVEL_PANE_TEXT_4 = 4,
        LEVEL_PANE_TEXT_6 = 6,
        LEVEL_PANE_TEXT_7 = 7,
        LEVEL_PANE_TEXT_MESSAGE = 0x10
    };

    enum {
        LEVEL_ANIM_IN = 0,
        LEVEL_ANIM_OUT = 2
    };
}

msw::SelectMenu::CConnectionLevel::CConnectionLevel(long nId, long nLevel, void *pMyData, void *pEmData)
    : CSelectMenuLayoutBase(nId)
{
    m_pMyData = (const CSelectMenu::WiFiSingleData *)pMyData;
    m_pEmData = (const CSelectMenu::WiFiSingleData *)pEmData;
    m_nResult = -1;
    m_nLevel = nLevel - 1;
}

msw::SelectMenu::CConnectionLevel::~CConnectionLevel()
{
    Remove();
}

void msw::SelectMenu::CConnectionLevel::Main()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    long nMode = pSelectMenu->m_nMode;

    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in CWiFiTopA::Main.
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(LEVEL_PANE_WIN, LEVEL_ANIM_IN, 1.0f, 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(LEVEL_PANE_WIN, LEVEL_ANIM_IN)) {
                static const long padNo0 = 0;

                pSelectMenu->m_nCurrentBtnKindId[padNo0][nMode] = BTN_KIND_TOP_0;
                m_nPushButtonId[padNo0] = BTN_KIND_TOP_0;
                SetCursorMarkerPosition(padNo0, BTN_KIND_TOP_0);
                m_fInputWait = 10.0f;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            m_fInputWait -= bcs::CMainApp::GetAppFrameRateF();
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 0.0f;
                unsigned short nBtnKindId = pSelectMenu->m_nCurrentBtnKindId[0][nMode];
                if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                    _GetModeChangeWaitTime();
                    switch (nBtnKindId) {
                        case BTN_KIND_TOP_0:
                            m_nResult = RESULT_YES;
                            break;
                        case BTN_KIND_TOP_1:
                            m_nResult = RESULT_NO;
                            break;
                    }
                    return;
                }
            }
            pSelectMenu->InputDataToMenuObj(this, 0, nPad, nMode);
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(LEVEL_PANE_WIN, LEVEL_ANIM_OUT, 1.0f, 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(LEVEL_PANE_WIN, LEVEL_ANIM_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
        default:
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CConnectionLevel::Set()
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
            ArchiveBuildLoader(s_LevelButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            // The game pairs font 1 with font 3's name here.
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_LevelLayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(200.0f);

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

            for (long i = 0; i < 4; i++) {
                if (m_nLevel >= i) {
                    m_pPaneAccessor->SetPaneDisp(s_LevelStarOnPanes[i], 1);
                    m_pPaneAccessor->SetPaneDisp(s_LevelStarOffPanes[i], 0);
                } else {
                    m_pPaneAccessor->SetPaneDisp(s_LevelStarOnPanes[i], 0);
                    m_pPaneAccessor->SetPaneDisp(s_LevelStarOffPanes[i], 1);
                }
            }

            const wchar_t *pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_01");
            m_pPaneAccessor->SetTextBoxString(LEVEL_PANE_TEXT_7, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_05");
            m_pPaneAccessor->SetTextBoxString(LEVEL_PANE_TEXT_6, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_04");
            m_pPaneAccessor->SetTextBoxString(LEVEL_PANE_TEXT_4, pText, wcslen(pText));

            // The message depends on whether either side has a second
            // player.
            const CSelectMenu::WiFiSingleData *pEmData = m_pEmData;
            const CSelectMenu::WiFiSingleData *pMyData = m_pMyData;
            if (wcslen(pEmData->m_Name2P) != 0) {
                pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_08");
            } else if (pMyData->m_nGameKind == 1 || pEmData->m_nGameKind == 1 || wcslen(pMyData->m_Name2P) != 0) {
                pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_07");
            } else {
                pText = GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_02_06");
            }
            m_pPaneAccessor->SetTextBoxString(LEVEL_PANE_TEXT_MESSAGE, pText, wcslen(pText));

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();
        }
    }
    return 0;
}

// Inlined into Main(), as CWiFiTopA::CheckButtonAnim.
void msw::SelectMenu::CConnectionLevel::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CConnectionLevel::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CConnectionLevel::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CConnectionLevel::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CConnectionLevel::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_TOP_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CConnectionLevel::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CConnectionLevel::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// The names in this namespace are guesses.
namespace {

    enum {
        WAIT_ANIM_IN,
        WAIT_ANIM_LOOP,
        WAIT_ANIM_OUT,
        WAIT_ANIM_NUM
    };

    const char *s_WaitAnimNames[WAIT_ANIM_NUM] = {
        "WIN_H_all_in.brlan",
        "WIN_H_all_loop.brlan",
        "WIN_H_all_out.brlan"
    };

    enum {
        WAIT_PANE_WIFI_WAIT = 0,
        WAIT_PANE_UN_CONNECT = 0x1D,
        WAIT_PANE_DISCONNECT = 0x20,
        WAIT_PANE_NUM = 0x23
    };

    const char *s_WaitPaneNames[WAIT_PANE_NUM] = {
        "wifi_wait",
        "BG_null",
        "wifi_bg_00",
        "wifi_bg_01",
        "wifi_bg_02",
        "wifi_bg_03",
        "wifi_bg_04",
        "wifi_bg_05",
        "wait_dot_null",
        "dot_00",
        "dot_01",
        "dot_02",
        "dot_03",
        "dot_04",
        "wii_glow",
        "wii_icon",
        "Eirth_null",
        "ling_glow_null",
        "glow_00",
        "glow_01",
        "glow_02",
        "glow_03",
        "Eirth",
        "Eirth_ring_null",
        "flare_00",
        "flare_01",
        "glow_ball_null",
        "gl_ball_00",
        "gl_ball_01",
        "un_connect",
        "batsu_00",
        "batsu_01",
        "disconnect",
        "batsu_02",
        "batsu_03"
    };

    // The panes the constructor's offset moves (null: none).
    const char *s_WaitMovePanes[3] = {
        "wifi_wait",
        0,
        "disconnect"
    };
}

msw::SelectMenu::CConnectionWait::CConnectionWait(long nId, int bDisconnect, float fOffsetY)
    : CSelectMenuLayoutBase(nId)
{
    m_fOffsetY = fOffsetY;
    m_bDisconnect = bDisconnect;
}

msw::SelectMenu::CConnectionWait::~CConnectionWait()
{
    Remove();
}

void msw::SelectMenu::CConnectionWait::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_WIFI_WAIT, WAIT_ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_UN_CONNECT, WAIT_ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_DISCONNECT, WAIT_ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(WAIT_PANE_WIFI_WAIT, WAIT_ANIM_IN)) {
                m_pPaneAccessor->PlayAnimation(WAIT_PANE_WIFI_WAIT, WAIT_ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(WAIT_PANE_UN_CONNECT, WAIT_ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(WAIT_PANE_DISCONNECT, WAIT_ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                m_Unk30 = STATE_ACTIVE;
            }
            // Falls through. (The original keeps STATE_ACTIVE in its compare
            // chain, which an empty break drops; this form gives it.)
        case STATE_ACTIVE:
            return;
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_WIFI_WAIT, WAIT_ANIM_OUT, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_UN_CONNECT, WAIT_ANIM_OUT, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(WAIT_PANE_DISCONNECT, WAIT_ANIM_OUT, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(WAIT_PANE_WIFI_WAIT, WAIT_ANIM_OUT)) {
                m_Unk30 = STATE_NONE;
                // Ask the menu task to delete the window.
                m_Unk08 = 2;
            }
            break;
    }
}

int msw::SelectMenu::CConnectionWait::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->BuildLayout(0, "WIN_H.brlyt");
            pPaneCtrl->BuildAnim(0, s_WaitAnimNames, WAIT_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_WaitPaneNames, WAIT_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(1.0f);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_pPaneAccessor->SetDisp("un_connect", 0);
            if (m_bDisconnect == 0) {
                m_pPaneAccessor->SetDisp("disconnect", 0);
            } else {
                m_pPaneAccessor->SetDisp("disconnect", 1);
            }

            for (long i = 0; i < 3; i++) {
                if (s_WaitMovePanes[i] != 0) {
                    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPaneObj(s_WaitMovePanes[i])->GetPane();
                    const nw4r::math::VEC3 &rTrans = pPane->GetTranslate();
                    // The game leaves z unset: the pane gets whatever is on
                    // the stack. Kept as it is (a port should copy z).
                    nw4r::math::VEC3 pos;
                    pos.x = rTrans.x;
                    pos.y = m_fOffsetY + rTrans.y;
                    pPane->SetTranslate(pos);
                }
            }
        }
    }
    return 0;
}

void msw::SelectMenu::CConnectionWait::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
}

long msw::SelectMenu::CConnectionWait::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (nEvent == EVENT_WINDOW_CLOSE) {
        if (m_Unk30 < STATE_CLOSE) {
            m_Unk30 = STATE_CLOSE;
        }
        nResult = 0;
    }
    return nResult;
}

// The names in this namespace are guesses.
namespace {

    enum {
        NEW_ANIM_TEXWIN_1,
        NEW_ANIM_TEXWIN_2,
        NEW_ANIM_TEXWIN_3,
        NEW_ANIM_A_BTN,
        NEW_ANIM_WIN,
        NEW_ANIM_NUM
    };

    const char *s_NewAnimNames[NEW_ANIM_NUM] = {
        "me_wifi_new_02_texwin_1.brlan",
        "me_wifi_new_02_texwin_2.brlan",
        "me_wifi_new_02_texwin_3.brlan",
        "me_wifi_new_02_A_btn.brlan",
        "me_wifi_new_02_win_anm_00.brlan"
    };

    enum {
        NEW_PANE_TW,
        NEW_PANE_BG_BLACK,
        NEW_PANE_G_TW,
        NEW_PANE_E_TW,
        NEW_PANE_W_TW,
        NEW_PANE_T_25_B,
        NEW_PANE_T_25_A,
        NEW_PANE_POSITION_NULL,
        NEW_PANE_A_BOTAN,
        NEW_PANE_TWO_BTN,
        NEW_PANE_NUM
    };

    const char *s_NewPaneNames[NEW_PANE_NUM] = {
        "tw",
        "bg_black",
        "g_tw",
        "e_tw",
        "w_tw",
        "t_25_B",
        "t_25_A",
        "position_null",
        "A_botan",
        "two_btn"
    };

    // CMenuPad::GetPadStyle of the classic controller (guessed).
    enum {
        PAD_STYLE_TWO_BTN = 1
    };
}

msw::SelectMenu::CConnectNewMess02::~CConnectNewMess02()
{
    Remove();
}

void msw::SelectMenu::CConnectNewMess02::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(NEW_PANE_TW, NEW_ANIM_TEXWIN_1, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(NEW_PANE_TW, NEW_ANIM_TEXWIN_1)) {
                m_pPaneAccessor->PlayAnimation(NEW_PANE_TW, NEW_ANIM_TEXWIN_2, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(NEW_PANE_TW, NEW_ANIM_A_BTN, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(NEW_PANE_TW, NEW_ANIM_WIN, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckPadStyle();
            break;
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(NEW_PANE_TW, NEW_ANIM_TEXWIN_3, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(NEW_PANE_TW, NEW_ANIM_TEXWIN_3)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }
}

int msw::SelectMenu::CConnectNewMess02::Set()
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
            pPaneCtrl->BuildLayout(0, "me_wifi_new_02.brlyt");
            pPaneCtrl->BuildAnim(0, s_NewAnimNames, NEW_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_NewPaneNames, NEW_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(120.0f);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            const wchar_t *pText = GetMessageLabelString("WifiConnectMessage", "WiFi_NEW_02_00");
            m_pPaneAccessor->SetTextBoxString(NEW_PANE_T_25_A, pText, wcslen(pText));
            pText = GetMessageLabelString("WifiConnectMessage", "WiFi_NEW_02_01");
            m_pPaneAccessor->SetTextBoxString(NEW_PANE_T_25_B, pText, wcslen(pText));

            CheckPadStyle();
            m_pPaneAccessor->SetPaneDisp(NEW_PANE_BG_BLACK, 0);
            m_bClose = 0;
        }
    }
    return 0;
}

// Inlined into Main() and Set().
inline void msw::SelectMenu::CConnectNewMess02::CheckPadStyle()
{
    CMenuPad &rPad = CMenuPad::s_Instance;

    if (m_nPadStyle != rPad.GetPadStyle(0)) {
        if (rPad.GetPadStyle(0) == PAD_STYLE_TWO_BTN) {
            m_pPaneAccessor->SetPaneDisp(NEW_PANE_A_BOTAN, 0);
            m_pPaneAccessor->SetPaneDisp(NEW_PANE_TWO_BTN, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(NEW_PANE_A_BOTAN, 1);
            m_pPaneAccessor->SetPaneDisp(NEW_PANE_TWO_BTN, 0);
        }
        m_nPadStyle = rPad.GetPadStyle(0);
    }
}

void msw::SelectMenu::CConnectNewMess02::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
}

long msw::SelectMenu::CConnectNewMess02::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (m_pPaneAccessor == 0) {
        return -1;
    }
    if (nEvent == EVENT_WINDOW_CLOSE) {
        if (m_bClose) {
            return m_Unk30;
        }
        m_Unk30 = STATE_CLOSE;
        m_bClose = 1;
    } else {
        nResult = 0;
    }
    return nResult;
}

// The names in this namespace are guesses.
namespace {

    const char *const s_OtherMiiLayoutName = "WIN_J.brlyt";
    const char *const s_OtherMiiButtonFileName = "WIN_J.btn";

    // The first message label of each page; the page shows it and the two
    // that follow.
    const char *s_OtherMiiPageLabels[2] = {
        "OTHER_MII_01_00",
        "OTHER_MII_02_00"
    };

    enum {
        OTHER_MII_PANE_WIN = 0,
        OTHER_MII_PANE_TEXT_0 = 5,
        OTHER_MII_PANE_TEXT_2 = 6,
        OTHER_MII_PANE_TEXT_1 = 7,
        OTHER_MII_PANE_8 = 8
    };

    enum {
        OTHER_MII_ANIM_IN = 5,
        OTHER_MII_ANIM_OUT = 7,
        OTHER_MII_ANIM_LOOP = 9
    };
}

msw::SelectMenu::CWiFiOtherMii::~CWiFiOtherMii()
{
    Remove();
}

void msw::SelectMenu::CWiFiOtherMii::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(OTHER_MII_PANE_WIN, OTHER_MII_ANIM_IN, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(OTHER_MII_PANE_WIN, OTHER_MII_ANIM_IN)) {
                m_pPaneAccessor->PlayAnimation(OTHER_MII_PANE_WIN, OTHER_MII_ANIM_LOOP, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            break;
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(OTHER_MII_PANE_WIN, OTHER_MII_ANIM_OUT, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(OTHER_MII_PANE_WIN, OTHER_MII_ANIM_OUT)) {
                m_Unk30 = STATE_NONE;
                // Ask the menu task to delete the window.
                m_Unk08 = 2;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CWiFiOtherMii::Disp()
{
}

int msw::SelectMenu::CWiFiOtherMii::Set()
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
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_OtherMiiButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_OtherMiiLayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(120.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                SetUpButton(pPart, i, 0);
                pPart->m_bUnk40_0004 = 1;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            m_nPage = -1;
            SetPage(0);
            m_pPaneAccessor->SetPaneDisp(OTHER_MII_PANE_8, 0);
        }
    }
    return 0;
}

// Inlined into Set() and CSelectMenu::WifiConnect_Loop. Not declared
// inline: the original has its strings in the unit's string pool, where an
// inline function would give them their own objects.
void msw::SelectMenu::CWiFiOtherMii::SetPage(long nPage)
{
    short nIndex = GetMessageLabelIndex("CharaEntryMessage", s_OtherMiiPageLabels[nPage]);
    SetTextPaneString(OTHER_MII_PANE_TEXT_0, GetMessageString("CharaEntryMessage", nIndex));
    SetTextPaneString(OTHER_MII_PANE_TEXT_1, GetMessageString("CharaEntryMessage", nIndex + 1));
    SetTextPaneString(OTHER_MII_PANE_TEXT_2, GetMessageString("CharaEntryMessage", nIndex + 2));
    m_nPage = nPage;
}

// Inlined into SetPage(), as CTournamentLoad::SetTextPaneString.
inline void msw::SelectMenu::CWiFiOtherMii::SetTextPaneString(long nPane, const wchar_t *pText)
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

void msw::SelectMenu::CWiFiOtherMii::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CWiFiOtherMii::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CWiFiOtherMii::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiOtherMii::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiOtherMii::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiOtherMii::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CWiFiOtherMii::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CWiFiOtherMii::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_OTHER_MII_NEXT;
}

int msw::SelectMenu::CWiFiOtherMii::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CWiFiOtherMii::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}
