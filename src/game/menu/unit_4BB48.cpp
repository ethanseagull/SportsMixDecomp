#include "game/msw/CSelectMenu.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CMiiMan.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/SelectMenu/CConnectionLevel.hpp"
#include "game/msw/SelectMenu/CConnectionWait.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "string.h"
#include "wchar.h"

namespace unit_4BB48_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float constant of this
// file with its own lis, which this compiler does when the first .rodata object of the file is
// weak (a function-local const). The module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// The Wi-Fi match-making screens: a match with anybody (WifiWait_*) and the
// set-up of a friend match (FriendVSWait_*). The game's file name is
// SelectMenuWiFiWait.cpp (the hash name of WifiWait's data). Both sides
// send their CSelectMenu::WiFiSingleData each step and go on once every
// player has reached the same step (WifiWait_Agreement).
//
// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_TASK = 1,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_COURT = 0x3E,
        OBJ_DWC_WINDOW = 0x3F, // what the Wi-Fi message window is sent through
        OBJ_UNK_42 = 0x42,
        OBJ_CONNECTION = 0xFF  // SelectMenu::CConnectionWait, then CConnectionLevel
    };

    // What the objects are sent (meanings guessed from the other screens).
    enum {
        EVENT_TITLE = 0x11,
        EVENT_PUSH = 0x21,         // plus the pad number
        EVENT_LEVEL_WINDOW = 0x31,
        EVENT_WINDOW_CLOSE = 0x32,
        EVENT_BG_CHR = 0x33,
        EVENT_COURT_CHR = 0x35,
        EVENT_COURT_CHR_ANIM = 0x36,
        EVENT_COURT_CHR_KIND = 0x3A,
        EVENT_COURT_CLEAR = 0x4D
    };

    // Values of bcs::dwc::CDwc::GetStat() tested here (names guessed).
    enum {
        DWC_STAT_READY = 4,
        DWC_STAT_MATCHING = 5,
        DWC_STAT_ERROR = 10
    };

    // Results of CCockpit::GetResult and of CConnectionLevel.
    enum {
        RESULT_OK = 1000,
        RESULT_CANCEL = 1001
    };

    // The cockpit's window of a failed connection (CCockpit::IsEndObj).
    enum {
        COCKPIT_OBJ_ERROR = 0x1019
    };

    // Modes of CSelectMenu used here.
    enum {
        MODE_TITLE = 4,
        MODE_CHARA_SELECT = 0xE,
        MODE_FRIEND_TOP = 0x1A,
        MODE_WIFI_GAME = 0x1E,
        MODE_COURT = 0x20,
        MODE_GAME_START = 0x31
    };

    // Camera animations of the screen.
    enum {
        CAMERA_TEAM = 0x22,
        CAMERA_COURT = 0x24
    };

    // Steps of WifiWait_Loop (CSelectMenu::m_nStep).
    enum {
        STEP_START = 0,         // start looking for an opponent
        STEP_MATCHING = 1,
        STEP_MATCHING_WAIT = 2,
        STEP_RESTART = 3,       // close the matching and start again
        STEP_FOUND = 4,
        STEP_FOUND_WAIT = 5,
        STEP_LEVEL = 6,         // show the level window
        STEP_LEVEL_WAIT = 7,
        STEP_STAGE_CHECK = 8,
        STEP_STAGE_WAIT = 9,
        STEP_SEND_READY = 10,
        STEP_READY_WAIT = 11,
        STEP_GAME_START = 13,
        STEP_ERROR = 14,        // the match was lost
        STEP_ERROR_WAIT = 15,
        STEP_CLOSE = 16,        // close the matching and leave
        STEP_COURT = 19,        // waiting for the camera before starting
        STEP_END = 20
    };

    // Steps both sides send (WiFiSingleData::m_nStep) in WifiWait_Loop.
    enum {
        SEND_MATCHED = 1,
        SEND_GAME = 2,
        SEND_READY = 3
    };

    // Steps of FriendVSWait_Loop, kept in m_MySingleData.m_nStep and sent.
    enum {
        FVS_STEP_INIT = 0x10,
        FVS_STEP_GAME = 0x11,       // the host has chosen the game
        FVS_STEP_GAME_WAIT = 0x12,
        FVS_STEP_ERROR = 0x13,
        FVS_STEP_CHARA = 0x14,      // the characters are chosen
        FVS_STEP_CHARA_WAIT = 0x15,
        FVS_STEP_COURT = 0x16,      // the court is chosen
        FVS_STEP_COURT_WAIT = 0x17,
        FVS_STEP_ERROR2 = 0x18,
        FVS_STEP_STAGE_WAIT = 0x19,
        FVS_STEP_ERROR_WAIT = 0x1A,
        FVS_STEP_CLOSE = 0x1B,
        FVS_STEP_START = 0x1E
    };

    // CSelectMenu::m_bStarMii in a friend match.
    enum {
        ROLE_HOST = 1,
        ROLE_GUEST = 2
    };

    // CSelectMenu::m_nFriendVSMessage.
    enum {
        FVS_MESSAGE_CHARA = 0x01,
        FVS_MESSAGE_COURT = 0x02,
        FVS_MESSAGE_UNK_80 = 0x80
    };

    // Game kinds and play modes set here.
    enum {
        GAME_KIND_NUM = 4,
        GAME_KIND_VOLLEY = 1
    };

    // Character numbers from here on are Miis.
    enum {
        CHARA_MII = 0x13
    };

    // Sound effects.
    enum {
        SE_LEVEL = 0xBC4,
        SE_OK = 0xBBB,
        SE_CANCEL = 0xBBE
    };

    enum {
        CHARA_NUM = 3,
        STAGE_ANY = 0xFF
    };

    // (no symbol; name guessed) Sets a stage number of CSelectMenu. As in
    // CSelectMenu::Title_SetUpRandGame, the old value is kept only when the
    // new one is 0xFF (none).
    inline void SetStageNo(unsigned char &rStage, unsigned char &rPrevStage, long nStage)
    {
        if (nStage == STAGE_ANY) {
            rPrevStage = rStage;
        }
        rStage = nStage;
    }
}

// The player's name and Mii into m_MySingleData, and the name into the
// game's player 0.
inline void msw::CSelectMenu::WiFi_SetMyName()
{
    wcscpy(m_MySingleData.m_Name, m_pPlayerData->m_Name);
    bcs::CMiiMan::GetStoreData(m_MySingleData.m_StoreData, m_pPlayerData->m_MiiSource,
                               m_pPlayerData->m_MiiCreateId);
    m_pGameDwc->SetPlayerName(0, m_MySingleData.m_Name);
}

// The same for the second player on this console.
inline void msw::CSelectMenu::WiFi_SetMy2P()
{
    if ((long)m_nPlayerNum == 2) {
        m_MySingleData.m_StoreData2P = WiFi_Get2PMiiStoreData();
        wcscpy(m_MySingleData.m_Name2P, WiFi_Get2PMiiName());
        m_pGameDwc->SetPlayerName2P(0, WiFi_Get2PMiiName());
    }
}

// The trophies and the character change options.
inline void msw::CSelectMenu::WiFi_SetMyRecord()
{
    m_MySingleData.m_nTrophy[0] = CheckSkillToTrophy(m_pPlayerData->m_Skill[0].m_nMaxRate);
    m_MySingleData.m_nTrophy[1] = CheckSkillToTrophy(m_pPlayerData->m_Skill[1].m_nMaxRate);
    m_MySingleData.m_nTrophy[2] = CheckSkillToTrophy(m_pPlayerData->m_Skill[2].m_nMaxRate);
    m_MySingleData.m_nTrophy[3] = CheckSkillToTrophy(m_pPlayerData->m_Skill[3].m_nMaxRate);
    m_MySingleData.m_nChrChangeFlags = m_pPlayerData->bBasketChrChange;
    m_MySingleData.m_nChrChangeFlags |= m_pPlayerData->bDodgeChrChange << 2;
    m_MySingleData.m_nChrChangeFlags |= m_pPlayerData->bHockeyChrChange << 3;
    m_MySingleData.m_Unk1EA = m_pPlayerData->m_Unk5D1F;
}

// The three characters.
inline void msw::CSelectMenu::WiFi_SetMyChara()
{
    for (long i = 0; i < CHARA_NUM; i++) {
        CharaSelectParam *pParam = GetCharaSelectParam(i);
        m_MySingleData.m_nChara[i] = pParam->m_nChara;
        m_MySingleData.m_nCharaSub[i] = pParam->m_nCharaSub;
        m_MySingleData.m_nCharaUnk[i] = pParam->m_Unk13[0];
        if (pParam->m_nChara >= CHARA_MII) {
            // The id comes back as the 8 bytes of an RFLCreateID.
            unsigned long long nCreateId = Mii_GetMiiCreateId(i);
            bcs::CMiiMan::GetStoreData(m_MySingleData.m_CharaMii[i], Mii_GetMiiDataSource(i),
                                       *(RFLCreateID *)&nCreateId);
        }
    }
}

// Both players have agreed: copies their data into the game, sets the play
// mode from the number of players and starts the game.
inline void msw::CSelectMenu::WiFi_SetUpMatchPlayers()
{
    // The original asks for both AIDs and does not use them: the data is
    // taken by index.
    m_pGameDwc->m_Dwc.GetMyAID();
    const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
    m_pGameDwc->m_Dwc.GetEmAID();
    const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);

    m_pGameDwc->SetPlayerData(0, pData0->m_StoreData, pData0->m_StoreData2P, pData0->m_nSkill);
    m_pGameDwc->SetPlayerData(1, pData1->m_StoreData, pData1->m_StoreData2P, pData1->m_nSkill);
    m_pGameDwc->SetPlayerName(0, pData0->m_Name);
    m_pGameDwc->SetPlayerName2P(0, pData0->m_Name2P);
    m_pGameDwc->SetPlayerName(1, pData1->m_Name);
    m_pGameDwc->SetPlayerName2P(1, pData1->m_Name2P);
    for (long i = 0; i < GAME_KIND_NUM; i++) {
        m_pGameDwc->m_PlayerInfo[0].m_nTrophy[i] = pData0->m_nTrophy[i];
    }
    for (long i = 0; i < GAME_KIND_NUM; i++) {
        m_pGameDwc->m_PlayerInfo[1].m_nTrophy[i] = pData1->m_nTrophy[i];
    }
    m_pGameDwc->m_nRole = m_bStarMii;

    long nMyAid = m_pGameDwc->m_Dwc.GetMyAID();
    long nTotal = 0;
    for (long i = 0; i < 2; i++) {
        nTotal += ((const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(i))->m_nPlayerNum;
    }
    CGame::SetMyTeamIdx(nMyAid & 1);
    CGame::g_GameWork.m_Unk151 = 1;

    m_Unk26B = m_nPlayMode;
    if (nTotal >= 4) {
        m_nPlayMode = 8;
    } else if (nTotal >= 3 && pData0->m_nPlayerNum == 1 && pData1->m_nPlayerNum >= 2) {
        m_nPlayMode = 0x11;
    } else if (nTotal >= 3) {
        m_nPlayMode = 7;
    } else if (m_nGameKind == GAME_KIND_VOLLEY) {
        m_nPlayMode = 5;
    } else {
        m_nPlayMode = 1;
    }
    m_pGameDwc->Window_Close();
    SetNextMode(MODE_GAME_START, _GetModeChangeWaitTime());
}

void msw::CSelectMenu::WifiWait_In()
{
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    m_pPlayerData = CSave::GetPlayerData(-1);
    if (!m_pPlayerData->IsValid()) {
        SetNextMode(MODE_TITLE, 0.0f);
        return;
    }

    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
    m_pGameDwc = dwc::CGameDwc::GetInstance();
    if (m_pGameDwc != 0 && m_pGameDwc->m_Dwc.IsConnect()) {
        m_nStep = STEP_START;
    } else {
        SetNextMode(MODE_TITLE, 0.0f);
    }

    if (m_bWifiWaitCourt == 1) {
        if (m_nWifiGameKind >= 0) {
            SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR_KIND, (void *)m_nWifiGameKind);
            SendObjEventTrigger(OBJ_COURT, EVENT_COURT_CLEAR, 0);
            Build(OBJ_COURT);
            SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR, 0);
            SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR, (void *)1);
            CCameraMenuBG::s_Instance.Play(CAMERA_COURT, GetCameraSpeed(CAMERA_COURT), 0.0f);
            SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR_ANIM, (void *)9);
            SendObjEventTrigger(OBJ_UNK_42, EVENT_COURT_CHR_ANIM, (void *)9);
            WifiCourt_SetUpDisplay();
        } else {
            SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR, 0);
            SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR, 0);
            CCameraMenuBG::s_Instance.Play(CAMERA_TEAM, GetCameraSpeed(CAMERA_TEAM), 0.0f);
            if (GetTeamPlayerNum() == 2) {
                SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR_ANIM, (void *)0xB);
                SendObjEventTrigger(OBJ_UNK_42, EVENT_COURT_CHR_ANIM, (void *)0xB);
            } else {
                SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR_ANIM, (void *)5);
                SendObjEventTrigger(OBJ_UNK_42, EVENT_COURT_CHR_ANIM, (void *)5);
            }
            m_nGameKind = -1;
            m_nStage = STAGE_ANY;
            m_nStageSub = STAGE_ANY;
        }
        m_nStep = STEP_COURT;
        m_bWifiWaitCourt = 0;
    }

    m_nWifiGameKind = m_nGameKind;
    m_pGameDwc->m_UnkDCC = 0;
    m_bWifiRetry = 0;
    m_bWifiMatchLimit = 1;
    m_nWifiSavedGameKind = m_nGameKind;
    m_nWifiSavedStageSub = m_nStageSub;
    m_nWifiSavedStage = m_nStage;
    memset(m_pGameDwc->m_PlayerInfo, 0, sizeof(m_pGameDwc->m_PlayerInfo));
}

void msw::CSelectMenu::WifiWait_Loop()
{
    long nStat = m_pGameDwc->m_Dwc.GetStat();
    if (nStat == DWC_STAT_ERROR) {
        m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
    }

    switch (m_nStep) {
        case STEP_COURT:
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                if (m_nWifiGameKind >= 0) {
                    Scratch(OBJ_COURT);
                }
                m_nStep = STEP_START;
            }
            break;
        case STEP_START:
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            }
            if (nStat == DWC_STAT_READY) {
                m_pPlayerData = CSave::GetPlayerData(-1);
                m_nWifiPlayerNum = 2;
                m_fWifiMatchTime = 0.0f;
                memset(&m_MySingleData, 0, sizeof(m_MySingleData));
                WifiWait_SetGameSkill();
                WiFi_SetMyRecord();
                m_pGameDwc->m_Dwc.StartAnybodyMatching(m_nWifiPlayerNum, m_nPlayerNum, m_nGameKind,
                                                       m_MySingleData.m_nSkill, m_bWifiMatchLimit);
                if (m_bWifiRetry == 0) {
                    m_pGameDwc->Window_Picture1("WifiConnectMessage", "FVS_WAIT_00", 0);
                    m_pGameDwc->Window_Pict1Lock(1);
                    SelectMenu::CConnectionWait *pWait = new SelectMenu::CConnectionWait(OBJ_CONNECTION, 0, 0.0f);
                    AddObj(pWait);
                    pWait->Set();
                }
                SendObjEventTrigger(OBJ_TOP, EVENT_TITLE,
                                    (void *)GetMessageLabelString("WifiConnectMessage", "WVS_WAIT_00_01"));
                m_bWifiRetry = 0;
                m_nStep = STEP_MATCHING;
            }
            break;
        case STEP_MATCHING:
            if (nStat == DWC_STAT_ERROR) {
                m_bWifiRetry = 1;
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (m_bWifiRetry == 1 && nStat == DWC_STAT_READY) {
                m_nStep = STEP_START;
            } else if (nStat == DWC_STAT_MATCHING) {
                m_pGameDwc->Window_Pict1Lock(0);
                m_nStep = STEP_MATCHING_WAIT;
            }
            break;
        case STEP_MATCHING_WAIT:
            if (nStat == DWC_STAT_READY) {
                m_nStep = STEP_START;
                m_bWifiRetry = 1;
            } else if (m_pGameDwc->m_Dwc.IsMatchPlay()) {
                // An opponent is found: send this player's data.
                m_fWifiMatchTime = 0.0f;
                m_bWifiMatchLimit = 1;
                m_MySingleData.m_nHash = m_pGameDwc->Makehash("SelectMenuWiFiWait.cpp");
                m_MySingleData.m_nStep = SEND_MATCHED;
                m_MySingleData.m_nSeed = bcs::BCS_Rand();
                WiFi_SetMyName();
                WiFi_SetMy2P();
                m_MySingleData.m_nPlayerNum = m_nPlayerNum;
                m_MySingleData.m_nGameKind = m_nGameKind;
                m_MySingleData.m_nStage = m_nStage;
                m_MySingleData.m_nStageSub = m_nStageSub;
                WiFi_SetMyChara();
                WifiWait_SendSetStep();
                RemoveObj(OBJ_CONNECTION);
                m_pGameDwc->Window_Pict1Close();
                m_nStep = STEP_FOUND;
            } else if (CCockpit::GetResult() == RESULT_OK) {
                // Cancelled.
                m_bWifiRetry = 0;
                m_pGameDwc->Window_Pict1Close();
                m_pGameDwc->ClearWindowResult();
                RemoveObj(OBJ_CONNECTION);
                m_fInputWait = 120.0f;
                m_nStep = STEP_CLOSE;
            } else {
                m_fWifiMatchTime += GetFrameRateF();
                if (m_pGameDwc->m_Dwc.IsShouldMatchAustraliaOnly() && m_bWifiMatchLimit &&
                    m_fWifiMatchTime >= 3600.0f) {
                    // A minute without an opponent: look further.
                    m_bWifiRetry = 1;
                    m_bWifiMatchLimit = 0;
                    m_fInputWait = 120.0f;
                    m_nStep = STEP_RESTART;
                }
            }
            break;
        case STEP_RESTART:
            m_pGameDwc->m_Dwc.CloseMatching();
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (nStat == DWC_STAT_READY) {
                m_nStep = STEP_START;
            }
            break;
        case STEP_FOUND:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                m_fInputWait = 180.0f;
                m_nStep = STEP_FOUND_WAIT;
            }
            break;
        case STEP_FOUND_WAIT:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 0.0f;
                long nResult = WifiWait_Agreement(SEND_MATCHED);
                if (nResult == 1) {
                    m_pGameDwc->Window_Close();
                    m_nStep = STEP_LEVEL;
                } else if (nResult == -1) {
                    m_pGameDwc->Window_Close();
                    m_nStep = STEP_ERROR;
                }
            }
            break;
        case STEP_LEVEL: {
            const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
            const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);
            if (pData0->m_nGameKind != pData1->m_nGameKind && pData0->m_nGameKind != -1 &&
                pData1->m_nGameKind != -1) {
                // The two want different games.
                m_fInputWait = 120.0f;
                m_nStep = STEP_CLOSE;
            } else {
                bcs::BCS_SRand(pData0->m_nSeed);
                dwc::CGameDwc::SetUpTimer(30.0f, 0);
                long nMyAid = m_pGameDwc->m_Dwc.GetMyAID();
                void *pMyData = (void *)m_pGameDwc->m_Dwc.GetSingleDataA(nMyAid);
                long nEmAid = m_pGameDwc->m_Dwc.GetEmAID();
                void *pEmData = (void *)m_pGameDwc->m_Dwc.GetSingleDataA(nEmAid);
                SelectMenu::CConnectionLevel *pLevel = new SelectMenu::CConnectionLevel(
                    OBJ_CONNECTION, m_pGameDwc->m_Dwc.GetMatchingLevel(), pMyData, pEmData);
                AddObj(pLevel);
                pLevel->Set();
                menu::CSndEff::s_Instance.Play(SE_LEVEL, 1);
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_LEVEL_WINDOW, (void *)0x78);
                m_nStep = STEP_LEVEL_WAIT;
            }
            break;
        }
        case STEP_LEVEL_WAIT: {
            SelectMenu::CConnectionLevel *pLevel = (SelectMenu::CConnectionLevel *)FindObj(OBJ_CONNECTION);
            if (pLevel != 0 && pLevel->m_Unk30 == CSelectMenuLayoutBase::STATE_ACTIVE) {
                switch (pLevel->m_nResult) {
                    case RESULT_OK:
                        RemoveObj(OBJ_CONNECTION);
                        SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                        dwc::CGameDwc::CloseTimer();
                        WifiWait_GameModeStage();
                        m_MySingleData.m_nStep = SEND_GAME;
                        WifiWait_SendSetStep();
                        m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                        menu::CSndEff::s_Instance.Play(SE_OK, 1);
                        m_nStep = STEP_STAGE_WAIT;
                        break;
                    case RESULT_CANCEL:
                        RemoveObj(OBJ_CONNECTION);
                        SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                        dwc::CGameDwc::CloseTimer();
                        m_pGameDwc->WindowA_Button("WifiConnectMessage", "WVS_WAIT_04", 0, 0, 1, 0);
                        m_pGameDwc->m_Dwc.CloseMatching();
                        menu::CSndEff::s_Instance.Play(SE_CANCEL, 1);
                        m_nStep = STEP_ERROR_WAIT;
                        break;
                    default:
                        if (dwc::CGameDwc::CountTimer()) {
                            // Time is up: taken as accepted.
                            dwc::CGameDwc::CloseTimer();
                            RemoveObj(OBJ_CONNECTION);
                            SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                            WifiWait_GameModeStage();
                            m_MySingleData.m_nStep = SEND_GAME;
                            WifiWait_SendSetStep();
                            m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                            m_nStep = STEP_STAGE_WAIT;
                        } else if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
                            RemoveObj(OBJ_CONNECTION);
                            SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                            dwc::CGameDwc::CloseTimer();
                            m_nStep = STEP_ERROR;
                        }
                        break;
                }
            }
            break;
        }
        case STEP_STAGE_WAIT: {
            long nResult = WifiWait_Agreement(SEND_GAME);
            if (nResult == 1) {
                m_nStep = STEP_STAGE_CHECK;
            } else if (nResult == -1) {
                m_pGameDwc->Window_Close();
                m_nStep = STEP_ERROR;
            }
            break;
        }
        case STEP_STAGE_CHECK:
            if (WifiWait_StageCheck() == 1) {
                m_nStep = STEP_SEND_READY;
            } else if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
                m_pGameDwc->Window_Close();
                m_nStep = STEP_ERROR;
            }
            break;
        case STEP_ERROR:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->WindowA_Button("WifiConnectMessage", "WVS_WAIT_05", 0, 0, 1, 0);
                m_nStep = STEP_ERROR_WAIT;
            }
            break;
        case STEP_SEND_READY:
            m_MySingleData.m_nStep = SEND_READY;
            WifiWait_SendSetStep();
            m_nStep = STEP_READY_WAIT;
            break;
        case STEP_READY_WAIT: {
            long nResult = WifiWait_Agreement(SEND_READY);
            if (nResult == 1) {
                if (m_nWifiGameKind >= 0) {
                    Court_EndCamera();
                }
                m_nStep = STEP_GAME_START;
            } else if (nResult == -1) {
                m_pGameDwc->Window_Close();
                m_nStep = STEP_ERROR;
            }
            break;
        }
        case STEP_GAME_START:
            if (CGamePadCheck::GetInstance() != 0) {
                CGamePadCheck::GetInstance()->m_UnkAC = 1;
            }
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                WiFi_SetUpMatchPlayers();
                m_nStep = STEP_END;
            }
            break;
        case STEP_ERROR_WAIT:
            if (CCockpit::GetResult() == RESULT_OK) {
                m_pGameDwc->ClearWindowResult();
                m_bWifiRetry = 1;
                m_fInputWait = 120.0f;
                m_nStep = STEP_CLOSE;
            }
            break;
        case STEP_CLOSE:
            if (m_fInputWait != 0.0f) {
                m_fInputWait -= GetFrameRateF();
                if (m_fInputWait <= 0.0f) {
                    m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                    m_fInputWait = 0.0f;
                }
            }
            m_pGameDwc->m_Dwc.CloseMatching();
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (nStat == DWC_STAT_READY) {
                m_nGameKind = m_nWifiSavedGameKind;
                m_nStageSub = m_nWifiSavedStageSub;
                m_nStage = m_nWifiSavedStage;
                m_pGameDwc->Window_Close();
                if (m_bWifiRetry) {
                    // Look for another opponent.
                    m_bWifiRetry = 0;
                    m_nStep = STEP_START;
                } else {
                    if (m_nWifiGameKind < 0) {
                        m_nGameKind = -1;
                        m_nStage = STAGE_ANY;
                        m_nStageSub = STAGE_ANY;
                    }
                    SendObjEventTrigger(OBJ_TASK, EVENT_PUSH, (void *)1);
                    m_nCurrentBtnKindId[0][m_nMode] = 1;
                    SetNextMode(GetPrevMode(), _GetModeChangeWaitTime());
                }
            }
            break;
    }
}

void msw::CSelectMenu::WifiWait_Out()
{
}

void msw::CSelectMenu::WifiWait_SetGameSkill()
{
    switch (m_nGameKind) {
        case 0:
            m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[0].m_nRate;
            break;
        case 1:
            m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[1].m_nRate;
            break;
        case 2:
            m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[2].m_nRate;
            break;
        case 3:
            m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[3].m_nRate;
            break;
        default:
            // Any game: the mean of the four.
            m_MySingleData.m_nSkill = 0;
            for (long i = 0; i < GAME_KIND_NUM; i++) {
                m_MySingleData.m_nSkill += m_pPlayerData->m_Skill[i].m_nRate;
            }
            m_MySingleData.m_nSkill /= GAME_KIND_NUM;
            break;
    }
}

void msw::CSelectMenu::WifiWait_GameModeStage()
{
    const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
    const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);
    long nKind0 = pData0->m_nGameKind;
    long nKind = nKind0;
    long nKind1 = pData1->m_nGameKind;

    if (nKind0 >= 0 && nKind1 >= 0) {
        // Both chose one (the same, see STEP_LEVEL).
        nKind = nKind0;
    }
    if (nKind0 < 0 && nKind1 < 0) {
        // Neither chose: the same draw on both sides.
        bcs::BCS_SRand(pData0->m_nSeed);
        nKind = bcs::BCS_RandRange(0L, 3L);
    } else if (nKind0 < 0) {
        nKind = nKind1;
    } else if (nKind1 < 0) {
        nKind = nKind0;
    }
    m_nGameKind = nKind;
    WifiWait_SetGameSkill();
    WifiWait_SelectStage();
}

void msw::CSelectMenu::WifiWait_SelectStage()
{
    const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
    const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);
    long nStage = -1;
    long nStageSub = -1;

    long nStage0 = pData0->m_nStage;
    if (nStage0 == STAGE_ANY) {
        nStage0 = -1;
    }
    long nStage1 = pData1->m_nStage;
    if (nStage1 == STAGE_ANY) {
        nStage1 = -1;
    }
    long nStageSub0 = pData0->m_nStageSub;
    if (nStageSub0 == STAGE_ANY) {
        nStageSub0 = -1;
    }
    long nStageSub1 = pData1->m_nStageSub;
    if (nStageSub1 == STAGE_ANY) {
        nStageSub1 = -1;
    }

    // The result is not used.
    CSave::GetPlayerSetUpData(-1);
    if (nStage0 == -1 && nStage1 == -1) {
        Court_RandomSelect(nStage, nStageSub);
    } else if (nStage1 == -1) {
        nStage = nStage0;
        nStageSub = nStageSub0;
    } else if (nStage0 == -1) {
        nStage = nStage1;
        nStageSub = nStageSub1;
    }
    if (nStage == -1 && nStageSub == -1) {
        // Both chose one: the same draw on both sides.
        bcs::BCS_SRand(pData0->m_nSeed);
        if (bcs::BCS_Rand() & 1) {
            nStage = nStage0;
            nStageSub = nStageSub0;
        } else {
            nStage = nStage1;
            nStageSub = nStageSub1;
        }
    }

    m_MySingleData.m_nStage = nStage;
    m_MySingleData.m_nStageSub = nStageSub;
    SetStageNo(m_nStage, m_nPrevStage, nStage);
    SetStageNo(m_nStageSub, m_nPrevStageSub, nStageSub);
    WifiWait_SendSetStep();
}

long msw::CSelectMenu::WifiWait_StageCheck()
{
    const WiFiSingleData *pData0 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
    const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);

    if (pData0->m_nStage != STAGE_ANY && pData1->m_nStage != STAGE_ANY) {
        if (pData0->m_nStage == pData1->m_nStage && pData0->m_nStageSub == pData1->m_nStageSub) {
            return 1;
        }
        bcs::BCS_SRand(pData0->m_nSeed);
        if (bcs::BCS_Rand() & 1) {
            m_MySingleData.m_nStage = pData0->m_nStage;
            m_MySingleData.m_nStageSub = pData0->m_nStageSub;
        } else {
            m_MySingleData.m_nStage = pData1->m_nStage;
            m_MySingleData.m_nStageSub = pData1->m_nStageSub;
        }
        SetStageNo(m_nStage, m_nPrevStage, m_MySingleData.m_nStage);
        SetStageNo(m_nStageSub, m_nPrevStageSub, m_MySingleData.m_nStageSub);
    }
    WifiWait_SendSetStep();
    return 0;
}

long msw::CSelectMenu::WifiWait_Agreement(long nStep)
{
    if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
        return -1;
    }

    long nNum = 0;
    for (long i = 0; i < m_nWifiPlayerNum; i++) {
        const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(i);
        if (m_MySingleData.m_nHash == pData->m_nHash && pData->m_nStep >= nStep) {
            nNum++;
        }
    }
    return nNum == m_nWifiPlayerNum;
}

void msw::CSelectMenu::WifiWait_SendSetStep()
{
    m_MySingleData.m_nPadStyle[0] = CMenuPad::s_Instance.GetPadStyle(0);
    m_MySingleData.m_nPadStyle[1] = CMenuPad::s_Instance.GetPadStyle(1);
    m_pGameDwc->m_Dwc.SetMySingleDataA(&m_MySingleData, sizeof(m_MySingleData));
}

long msw::CSelectMenu::CheckSkillToTrophy(short nSkill)
{
    long nTrophy = 0;
    if (nSkill >= 8000) {
        nTrophy = 4;
    } else if (nSkill >= 4000) {
        nTrophy = 3;
    } else if (nSkill >= 2000) {
        nTrophy = 2;
    } else if (nSkill >= 1000) {
        nTrophy = 1;
    }
    return nTrophy;
}

void msw::CSelectMenu::FriendVSWait_In()
{
    m_nWifiPlayerNum = 2;
    m_pGameDwc->m_UnkDCC = 0;
    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    m_pPlayerData = CSave::GetPlayerData(-1);
    if (!m_pPlayerData->IsValid()) {
        SetNextMode(MODE_TITLE, 0.0f);
        return;
    }

    m_pGameDwc = dwc::CGameDwc::GetInstance();
    if (m_pGameDwc != 0 && m_pGameDwc->m_Dwc.IsConnect()) {
        if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
            m_pGameDwc->WindowA_Button("WiFiErrorMessage", "WiFi_16_00", 0, 0, 0, 0);
            m_MySingleData.m_nStep = FVS_STEP_ERROR_WAIT;
        }
    } else {
        SetNextMode(MODE_TITLE, 0.0f);
    }

    if (m_bStarMii == ROLE_HOST) {
        m_pGameDwc->m_pMatchFriend = m_Unk708;
    } else {
        m_pGameDwc->m_pMatchFriend = m_pVsFriendSaved;
    }
}

void msw::CSelectMenu::FriendVSWait_Loop()
{
    long nStat = m_pGameDwc->m_Dwc.GetStat();

    switch (m_MySingleData.m_nStep) {
        case FVS_STEP_INIT:
            memset(&m_MySingleData, 0, sizeof(m_MySingleData));
            m_MySingleData.m_nSeed = bcs::BCS_Rand();
            WiFi_SetMyName();
            WiFi_SetMyRecord();
            WiFi_SetMy2P();
            m_MySingleData.m_nPlayerNum = m_nPlayerNum;
            m_MySingleData.m_nGameKind = 0;
            m_MySingleData.m_nSkill = 0;
            m_MySingleData.m_nStage = 0;
            m_MySingleData.m_nStageSub = 0;
            SetNextMode(MODE_WIFI_GAME, _GetModeChangeWaitTime());
            m_MySingleData.m_nStep = FVS_STEP_GAME;
            break;
        case FVS_STEP_GAME:
            // Back from the game selection.
            if (m_bStarMii == ROLE_HOST) {
                if (m_nGameKind == -1) {
                    m_nGameKind = bcs::BCS_RandRange(0L, 4L);
                }
                m_MySingleData.m_nGameKind = m_nGameKind;
            }
            m_MySingleData.m_Unk05 = 0;
            FriendVSWait_SendSetStep(FVS_STEP_GAME_WAIT);
            break;
        case FVS_STEP_GAME_WAIT: {
            long nResult = FriendVSWait_Agreement(FVS_STEP_GAME_WAIT);
            if (nResult == 0) {
                // The other side has not got there yet.
                break;
            }
            if (nResult == 1) {
                m_pGameDwc->Window_Close();
                if (m_bStarMii == ROLE_GUEST) {
                    m_MySingleData.m_nGameKind = m_nGameKind =
                        ((const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0))->m_nGameKind;
                }
                switch (m_nGameKind) {
                    case 0:
                        m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[0].m_nRate;
                        break;
                    case 1:
                        m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[1].m_nRate;
                        break;
                    case 2:
                        m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[2].m_nRate;
                        break;
                    case 3:
                        m_MySingleData.m_nSkill = m_pPlayerData->m_Skill[3].m_nRate;
                        break;
                }
                m_Unk26B = m_nPlayMode;
                switch (m_nGameKind) {
                    case 0:
                    case 2:
                    case 3:
                        if ((long)m_nPlayerNum == 2) {
                            m_nPlayMode = 6;
                        } else {
                            m_nPlayMode = 0;
                        }
                        break;
                    case 1:
                        if ((long)m_nPlayerNum == 2) {
                            m_nPlayMode = 6;
                        } else {
                            m_nPlayMode = 4;
                        }
                        break;
                }
                SetNextMode(MODE_CHARA_SELECT, _GetModeChangeWaitTime());
                FriendVSWait_SendSetStep(FVS_STEP_CHARA);
            } else if (nResult == -1) {
                m_pGameDwc->Window_Close();
                m_MySingleData.m_nStep = FVS_STEP_ERROR;
            }
            break;
        }
        case FVS_STEP_ERROR:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->WindowA_Button("WiFiErrorMessage", "WiFi_16_00", 0, 0, 1, 0);
                m_MySingleData.m_nStep = FVS_STEP_ERROR_WAIT;
            }
            break;
        case FVS_STEP_CHARA:
            // Back from the character selection.
            WiFi_SetMyChara();
            m_MySingleData.m_Unk05 = 0;
            FriendVSWait_SendSetStep(FVS_STEP_CHARA_WAIT);
            break;
        case FVS_STEP_CHARA_WAIT:
            m_pGameDwc->Window_Close();
            if (FriendVSWait_Agreement(FVS_STEP_CHARA_WAIT) == -1) {
                m_pGameDwc->WindowA_Button("WiFiErrorMessage", "WiFi_16_00", 0, 0, 1, 0);
                m_MySingleData.m_nStep = FVS_STEP_ERROR_WAIT;
            } else {
                SetNextMode(MODE_COURT, _GetModeChangeWaitTime());
                FriendVSWait_SendSetStep(FVS_STEP_COURT);
            }
            break;
        case FVS_STEP_COURT:
            // Back from the court selection.
            m_MySingleData.m_nStage = m_nStage;
            m_MySingleData.m_nStageSub = m_nStageSub;
            FriendVSWait_SendSetStep(FVS_STEP_COURT_WAIT);
            m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
            m_nFriendVSMessage = 0;
            break;
        case FVS_STEP_COURT_WAIT: {
            FriendVSWait_Message();
            long nResult = FriendVSWait_Agreement(FVS_STEP_COURT_WAIT);
            if (nResult == 0) {
                // The other side has not got there yet.
                break;
            }
            if (nResult == 1) {
                bcs::BCS_SRand(((const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0))->m_nSeed);
                WifiWait_SelectStage();
                Court_EndCamera();
                m_MySingleData.m_nStep = FVS_STEP_STAGE_WAIT;
            } else if (nResult == -1) {
                m_pGameDwc->Window_Close();
                m_MySingleData.m_nStep = FVS_STEP_ERROR2;
            }
            break;
        }
        case FVS_STEP_ERROR2:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->WindowA_Button("WiFiErrorMessage", "WiFi_16_00", 0, 0, 1, 0);
                m_MySingleData.m_nStep = FVS_STEP_ERROR_WAIT;
            }
            break;
        case FVS_STEP_STAGE_WAIT: {
            FriendVSWait_Message();
            long nResult = FriendVSWait_Agreement(FVS_STEP_COURT_WAIT);
            if (nResult == -1) {
                m_pGameDwc->Window_Close();
                m_MySingleData.m_nStep = FVS_STEP_ERROR2;
            } else if (nResult == 1 && WifiWait_StageCheck() == 1) {
                if (CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 1;
                }
                if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                    WiFi_SetUpMatchPlayers();
                    FriendVSWait_SendSetStep(FVS_STEP_START);
                }
            }
            break;
        }
        case FVS_STEP_ERROR_WAIT:
            if (CCockpit::GetResult() == RESULT_OK) {
                m_pGameDwc->WindowA_Close();
                m_pGameDwc->ClearWindowResult();
                m_bWifiRetry = 1;
                m_fInputWait = 120.0f;
                FriendVSWait_SendSetStep(FVS_STEP_CLOSE);
            }
            break;
        case FVS_STEP_CLOSE:
            if (m_fInputWait != 0.0f) {
                m_fInputWait -= GetFrameRateF();
                if (m_fInputWait < 0.0f) {
                    m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                    m_fInputWait = 0.0f;
                }
            }
            m_pGameDwc->m_Dwc.CloseMatching();
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (nStat == DWC_STAT_READY) {
                m_pGameDwc->Window_Close();
                SendObjEventTrigger(OBJ_TASK, EVENT_PUSH, (void *)1);
                m_nCurrentBtnKindId[0][m_nMode] = 1;
                SetNextMode(MODE_FRIEND_TOP, _GetModeChangeWaitTime());
            }
            break;
    }
}

void msw::CSelectMenu::FriendVSWait_Out()
{
}

void msw::CSelectMenu::FriendVSWait_SendSetStep(long nStep)
{
    m_MySingleData.m_nHash = m_pGameDwc->Makehash("FriendVSWait");
    m_MySingleData.m_nRole = m_bStarMii;
    m_MySingleData.m_nStep = nStep;
    WifiWait_SendSetStep();
}

long msw::CSelectMenu::FriendVSWait_Agreement(long nStep)
{
    if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
        return -1;
    }

    long nNum = 0;
    for (long i = 0; i < m_nWifiPlayerNum; i++) {
        const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(i);
        if (m_MySingleData.m_nHash == pData->m_nHash && pData->m_nStep >= nStep) {
            nNum++;
        }
    }
    return nNum == m_nWifiPlayerNum;
}

void msw::CSelectMenu::FriendVSWait_Message()
{
    long nEmAid = m_pGameDwc->m_Dwc.GetEmAID();
    long nStep = ((const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(nEmAid))->m_nStep;
    if (nStep == FVS_STEP_CHARA && !(m_nFriendVSMessage & FVS_MESSAGE_CHARA)) {
        m_pGameDwc->Window_Message("WifiMenuMessage", "WAIT_01_00_00", 0, 0);
        m_nFriendVSMessage = FVS_MESSAGE_UNK_80 | FVS_MESSAGE_CHARA;
    } else if (nStep == FVS_STEP_COURT && !(m_nFriendVSMessage & FVS_MESSAGE_COURT)) {
        m_pGameDwc->Window_Message("WifiMenuMessage", "WAIT_01_00_01", 0, 0);
        m_nFriendVSMessage = FVS_MESSAGE_COURT;
    }
}
