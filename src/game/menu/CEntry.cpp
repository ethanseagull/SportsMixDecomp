#include "game/msw/SelectMenu/CEntry.hpp"

#include "bcs/CMainApp.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/menu/CSndEff.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CTeam.cpp: the original converts the
    // value to float at run time and keeps the word first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_UNK_01 = 1,
        OBJ_ENTRY = 9,
        OBJ_EXPLAIN = 0x36,      // SelectMenu::CEntryExplain
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_UNK_3F = 0x3F,
        OBJ_UNK_40 = 0x40,
        OBJ_UNK_42 = 0x42,
        // The cockpit's controller error window (CCockpit::IsEndObj).
        COCKPIT_OBJ_CONTROLLER_ERROR = 0x1018
    };

    // The button kind ids of the entry screen.
    enum {
        BTN_KIND_BACK_ICON = 1,
        BTN_KIND_PLAYER_1 = 0x8010, // one to four players
        BTN_KIND_PLAYER_2 = 0x8011,
        BTN_KIND_PLAYER_3 = 0x8012,
        BTN_KIND_PLAYER_4 = 0x8013,
        BTN_KIND_EXPLAIN = 0x8020   // opens the explanation window
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_CAMERA = 0,
        STEP_SELECT = 1,
        STEP_CLOSE = 2,
        STEP_EXPLAIN_OPEN = 10,
        STEP_EXPLAIN = 11,
        STEP_EXPLAIN_CLOSE = 12,
        STEP_ERROR_OPEN = 20,     // too few remotes: open the error window
        STEP_ERROR = 21,
        STEP_ERROR_CLOSE = 22,
        STEP_ERROR_WAIT = 23
    };

    // Values of CSelectMenu::m_Unk9B: where the entry screen leads. The
    // game compares the byte as a signed word (cmpwi), hence the casts to
    // long where it is tested.
    enum {
        ENTRY_EXHIBITION = 0,
        ENTRY_TOURNAMENT = 1,
        ENTRY_PARTY = 2,
        ENTRY_UNK_3 = 3,
        ENTRY_WIFI = 4
    };

    // Modes passed to CSelectMenu::SetNextMode and friends.
    enum {
        MODE_TOP_MENU = 2,
        MODE_GAME_SELECT = 4,
        MODE_CHARA_SELECT_0 = 0xC,
        MODE_CHARA_SELECT_1 = 0xD,
        MODE_TEAM = 0xE,
        MODE_WIFI_CHARA_SELECT = 0x19,
        MODE_WIFI_SINGLE = 0x1A
    };

    // m_nGameKind of the boss games.
    enum {
        GAME_KIND_BOSS = 5
    };

    // The pad bits of CMenuPadInfo's flag bytes.
    enum {
        PADINFO_REMOTE = 0x01,   // at 0x04
        PADINFO_LEADER = 0x04,   // at 0x04
        PADINFO_OFF = 0x04,      // at 0x06
        PADINFO_LOCKED = 0x08    // at 0x06
    };

    // The play mode of a Wi-Fi match.
    enum {
        PLAY_MODE_WIFI = 6
    };

    // A system sound played when the entry is accepted.
    enum {
        SE_ENTRY_DECIDE = 0xBBB
    };

    // The music of the menus (CSelectMenu::PlayMenuBGM).
    enum {
        BGM_MENU = 1
    };

    // Goes back from the entry screen: to the game selection screen in the
    // boss games (whose menu history is rebuilt), else to the previous one.
    // Inline in the game (no symbol; name guessed).
    inline void Entry_GoBack(msw::CSelectMenu *pMenu, msw::CSelectMenuLayoutBase *pObj)
    {
        if (pMenu->m_nGameKind == GAME_KIND_BOSS) {
            pMenu->ClearMenuHistory();
            pMenu->PushMenuHistory(1, MODE_TOP_MENU, 0);
            pMenu->PushMenuHistory(MODE_TOP_MENU, MODE_GAME_SELECT, 0);
            pMenu->PlayMenuBGM(BGM_MENU);
            pMenu->GoBackToMode(0, MODE_GAME_SELECT, pObj, 1);
        } else {
            pMenu->GoBackToMode(0, pMenu->GetPrevMode(), pObj, 1);
        }
        msw::CGamePadCheck *pPadCheck = msw::CGamePadCheck::GetInstance();
        pPadCheck->m_Unk3C = 1;
    }

    // The same from the Wi-Fi entry screen. Inline in the game (no symbol;
    // name guessed).
    inline void WifiEntry_GoBack(msw::CSelectMenu *pMenu, msw::CSelectMenuLayoutBase *pObj)
    {
        pMenu->GoBackToMode(0, pMenu->GetPrevMode(), pObj, 1);
        pMenu->PlayMenuBGM(BGM_MENU);
        pMenu->SetWhiteFadeOut(10.0f);
        pMenu->SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
        msw::CGamePadCheck *pPadCheck = msw::CGamePadCheck::GetInstance();
        pPadCheck->m_Unk3C = 1;
    }
}

void msw::CSelectMenu::Entry_In()
{
    static const long padNo0 = 0;

    // Looked up before it is built, as in the game.
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY);
    Build(OBJ_ENTRY);
    SendObjEventTrigger(OBJ_UNK_00, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "ENT_03"));
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
    SendObjEventTrigger(OBJ_UNK_42, 0x35, 0);
    if (m_nGameKind == GAME_KIND_BOSS) {
        SendObjEventTrigger(OBJ_UNK_42, 0x3A, (void *)4);
    } else {
        SendObjEventTrigger(OBJ_COURT_CHR, 0x36, 0);
        SendObjEventTrigger(OBJ_UNK_42, 0x36, 0);
        SendObjEventTrigger(OBJ_UNK_42, 0x3A, (void *)m_nGameKind);
    }
    CGamePadCheck::CleanUp();

    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 &= ~PADINFO_LOCKED;
    }
    m_Unk27B = 0;
    long nLeader = CMenuPadInfo::s_Instance.SelectPadLeader(0);
    ChangeLeaderPad(nLeader);

    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }

    // Only the leader is marked.
    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 &= ~PADINFO_LEADER;
    }
    CMenuPadInfo::s_Instance.m_aPad[nLeader].m_nFlags04 |= PADINFO_LEADER;

    if (m_nGameKind == GAME_KIND_BOSS) {
        SetFadeIn(120.0f);
    } else {
        SetFadeIn(10.0f);
    }
    CGamePadCheck::GetInstance()->m_Unk38 = 0;
    CGamePadCheck::GetInstance()->m_Unk3C = 1;

    if ((long)m_Unk9B == ENTRY_PARTY) {
        CCameraMenuBG::s_Instance.Play(0x20, GetCameraSpeed(0x20), 0.0f);
        m_nStep = STEP_SELECT;
    } else if (m_Unk9D != 5 && SendObjEventTrigger(OBJ_UNK_01, 0x2C, 0) != 0) {
        m_bEntryCameraWait = 1;
        m_nStep = STEP_SELECT;
    } else {
        if (m_nGameKind == GAME_KIND_BOSS) {
            SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0x14);
            SendObjEventTrigger(OBJ_UNK_42, 0x36, (void *)0x14);
        }
        m_bEntryCameraWait = 0;
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::Entry_Loop()
{
    switch (m_nStep) {
        case STEP_CAMERA:
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                CCameraMenuBG::s_Instance.PushBack(0x20, GetCameraSpeed(0x20), 0.0f);
                m_nStep = STEP_SELECT;
            }
            break;
        case STEP_SELECT:
            Entry_LoopStep0();
            break;
        case STEP_CLOSE:
            Entry_LoopStep1();
            break;
        case STEP_EXPLAIN_OPEN:
            Entry_LoopStep10();
            break;
        case STEP_EXPLAIN:
            Entry_LoopStep11();
            break;
        case STEP_EXPLAIN_CLOSE:
            Entry_LoopStep12();
            break;
        case STEP_ERROR_OPEN:
            Entry_LoopStep20();
            break;
        case STEP_ERROR:
            Entry_LoopStep21();
            break;
        case STEP_ERROR_CLOSE:
            Entry_LoopStep22();
            break;
        case STEP_ERROR_WAIT:
            Entry_LoopStep23();
            break;
    }
}

void msw::CSelectMenu::Entry_LoopStep0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (m_bEntryCameraWait && CCameraMenuBG::s_Instance.IsEndAnim()) {
        if (m_nGameKind == GAME_KIND_BOSS) {
            SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0x14);
            SendObjEventTrigger(OBJ_UNK_42, 0x36, (void *)0x14);
        }
        m_bEntryCameraWait = 0;
    }

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY);
    if (pObj == 0) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        switch (nBtnKindId) {
            case BTN_KIND_PLAYER_1:
            case BTN_KIND_PLAYER_2:
            case BTN_KIND_PLAYER_3:
            case BTN_KIND_PLAYER_4:
                // A disabled player count.
                if (pObj->m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId)->m_nFlags40 & 0x0001) {
                    break;
                }
                pObj->PushButton(padNo0, nBtnKindId);
                m_nPlayerNum = nBtnKindId - BTN_KIND_PLAYER_1 + 1;
                CMenuPadInfo::s_Instance.m_aPad[nPad].m_nFlags04 |= PADINFO_LEADER;
                m_nPadPlayer[nPad] = 0;
                switch (m_Unk9B) {
                    case ENTRY_TOURNAMENT:
                        Entry_NextStateTournament(nBtnKindId);
                        break;
                    case ENTRY_EXHIBITION:
                        Entry_NextStateExhibition(nBtnKindId);
                        break;
                    case ENTRY_PARTY:
                        Entry_NextStateParty(nBtnKindId);
                        break;
                }
                break;
            case BTN_KIND_EXPLAIN:
                pObj->PushButton(padNo0, nBtnKindId);
                m_nStep = STEP_EXPLAIN_OPEN;
                break;
            case BTN_KIND_BACK_ICON:
                Entry_GoBack(this, pObj);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        Entry_GoBack(this, pObj);
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::Entry_Out()
{
    CGamePadCheck *pPadCheck = CGamePadCheck::GetInstance();
    pPadCheck->m_Unk38 = 1;
    Scratch(OBJ_ENTRY);
}

void msw::CSelectMenu::WifiEntry_In()
{
    static const long padNo0 = 0;

    Build(OBJ_ENTRY);
    SetFadeIn(10.0f);
    SendObjEventTrigger(OBJ_UNK_00, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "ENT_03"));
    SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
    CCameraMenuBG::s_Instance.Play(0x29, 1.0f, 0.0f);
    CGamePadCheck::CleanUp();

    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 &= ~PADINFO_LOCKED;
    }
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    CGamePadCheck *pPadCheck = CGamePadCheck::GetInstance();
    pPadCheck->m_Unk3C = 1;
    pPadCheck = CGamePadCheck::GetInstance();
    pPadCheck->m_Unk38 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }
    _DisablePointStick(0, 0);
    _DisablePointStick(1, 1);
    m_nPlayerNum = 1;
    WifiEntry_PadEntry();
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::WifiEntry_Loop()
{
    switch (m_nStep) {
        case STEP_SELECT:
            WifiEntry_LoopStep0();
            break;
        case STEP_CLOSE:
            Entry_LoopStep1();
            break;
        case STEP_EXPLAIN_OPEN:
            Entry_LoopStep10();
            break;
        case STEP_EXPLAIN:
            Entry_LoopStep11();
            break;
        case STEP_EXPLAIN_CLOSE:
            Entry_LoopStep12();
            break;
        case STEP_ERROR_OPEN:
            Entry_LoopStep20();
            break;
        case STEP_ERROR:
            Entry_LoopStep21();
            break;
        case STEP_ERROR_CLOSE:
            Entry_LoopStep22();
            break;
        case STEP_ERROR_WAIT:
            Entry_LoopStep23();
            break;
    }
}

void msw::CSelectMenu::WifiEntry_LoopStep0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        float fWait = _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_PLAYER_1:
                // A match of one player on this console.
                m_nPlayerNum = 1;
                if (Entry_CheckConnectPad(m_nPlayerNum) != 0) {
                    break;
                }
                m_nPlayMode = 0;
                CMenuPadInfo::s_Instance.m_aPad[nPad].m_nFlags04 |= PADINFO_LEADER;
                pObj->PushButton(padNo0, nBtnKindId);
                SetNextMode(MODE_WIFI_SINGLE, fWait);
                WifiEntry_PadEntry();
                break;
            case BTN_KIND_PLAYER_2:
                pObj->PushButton(padNo0, nBtnKindId);
                m_nPlayerNum = 2;
                if (Entry_CheckConnectPad(m_nPlayerNum) < 0) {
                    m_nStep = STEP_ERROR_OPEN;
                    break;
                }
                pObj->PushButton(padNo0, nBtnKindId);
                WifiEntry_ChangeStateInit();
                break;
            case BTN_KIND_PLAYER_3:
            case BTN_KIND_PLAYER_4:
                // Not on Wi-Fi.
                break;
            case BTN_KIND_EXPLAIN:
                pObj->PushButton(padNo0, nBtnKindId);
                m_nStep = STEP_EXPLAIN_OPEN;
                break;
            case BTN_KIND_BACK_ICON:
                WifiEntry_GoBack(this, pObj);
                break;
            case 0:
                break;
            default:
                SetNextMode(nMode, fWait);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        WifiEntry_GoBack(this, pObj);
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::WifiEntry_Out()
{
    CGamePadCheck *pPadCheck = CGamePadCheck::GetInstance();
    pPadCheck->m_Unk38 = 1;
    Scratch(OBJ_ENTRY);
}

// Gives the first m_nPlayerNum pads with a remote their player numbers
// and switches the others off.
void msw::CSelectMenu::WifiEntry_PadEntry()
{
    long nPlayerNum = m_nPlayerNum;
    for (long i = 0; i < 4; i++) {
        if ((CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 & PADINFO_REMOTE) && i < nPlayerNum) {
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 &= ~PADINFO_OFF;
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 |= PADINFO_LEADER;
            m_nPadPlayer[i] = i;
        } else {
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 |= PADINFO_OFF;
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 &= ~PADINFO_LEADER;
            m_nPadPlayer[i] = 0xFF;
        }
    }
    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 |= PADINFO_LOCKED;
    }
    m_Unk269 = nPlayerNum;
}

// Goes on to the Wi-Fi character selection with the player's own Mii.
void msw::CSelectMenu::WifiEntry_ChangeStateInit()
{
    if ((long)m_Unk9B == ENTRY_WIFI) {
        CGamePadCheck::GetInstance()->m_Unk3C = m_nPlayerNum;
        m_nPlayMode = PLAY_MODE_WIFI;
        InitCharaSelectParam();
        float fWait = _GetModeChangeWaitTime();
        m_Unk27D = 1;
        SetNextMode(MODE_WIFI_CHARA_SELECT, fWait);

        CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
        pPlayerData->ClearSelectMii();
        pPlayerData->SetSelectMii(pPlayerData->SearchMiiIndex(pPlayerData->m_MiiCreateId), 1);
        WifiEntry_PadEntry();
    }
}

void msw::CSelectMenu::Entry_NextStateExhibition(long nBtnKindId)
{
    InitCharaSelectParam();
    switch (nBtnKindId) {
        case BTN_KIND_PLAYER_1:
        case BTN_KIND_PLAYER_2:
        case BTN_KIND_PLAYER_3:
        case BTN_KIND_PLAYER_4:
            Entry_ChangeStateInit(MODE_CHARA_SELECT_0);
            break;
    }
}

void msw::CSelectMenu::Entry_NextStateTournament(long nBtnKindId)
{
    m_nPlayMode = CTournament::GetPlayerType(m_nGameKind, m_nPlayerNum);
    InitCharaSelectParam();
    if (nBtnKindId == BTN_KIND_PLAYER_1) {
        Entry_ChangeStateInit(MODE_CHARA_SELECT_0);
    } else {
        Entry_ChangeStateInit(MODE_TEAM);
    }
}

void msw::CSelectMenu::Entry_NextStateParty(long nBtnKindId)
{
    m_nPlayMode = _GetPartyPlayType(m_nPlayerNum);
    InitCharaSelectParam();
    switch (nBtnKindId) {
        case BTN_KIND_PLAYER_1:
        case BTN_KIND_PLAYER_2:
        case BTN_KIND_PLAYER_3:
        case BTN_KIND_PLAYER_4:
            Entry_ChangeStateInit(MODE_TEAM);
            break;
    }
}

long msw::CSelectMenu::Entry_CheckConnectPad(long nNum)
{
    for (long i = 0; i < nNum; i++) {
        if (!(CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 & PADINFO_REMOTE)) {
            return -1;
        }
    }
    return 0;
}

void msw::CSelectMenu::Entry_ChangeStateInit(long nMode)
{
    switch (nMode) {
        case MODE_CHARA_SELECT_0:
        case MODE_CHARA_SELECT_1:
        case MODE_TEAM:
            if (Entry_CheckConnectPad(m_nPlayerNum) < 0) {
                m_nNextMode = nMode;
                m_nStep = STEP_ERROR_OPEN;
                return;
            }
            break;
    }

    menu::CSndEff::s_Instance.Play(SE_ENTRY_DECIDE, 1);
    long nPlayerNum = m_nPlayerNum;
    for (long i = 0; i < 4; i++) {
        if ((CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 & PADINFO_REMOTE) && i < nPlayerNum) {
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 |= PADINFO_LEADER;
            m_nPadPlayer[i] = i;
        } else {
            CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags04 &= ~PADINFO_LEADER;
            m_nPadPlayer[i] = 0xFF;
        }
    }
    m_Unk269 = nPlayerNum;
    ((CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY))->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::s_Instance.m_aPad[i].m_nFlags06 |= PADINFO_LOCKED;
    }
    m_nNextMode = nMode;
    m_nStep = STEP_CLOSE;
}

// Once the window has closed, moves on to the mode Entry_ChangeStateInit
// chose.
void msw::CSelectMenu::Entry_LoopStep1()
{
    if (((CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY))->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }

    SendObjEventTrigger(OBJ_UNK_01, 0x51, 0);
    CGamePadCheck::GetInstance()->m_Unk3C = m_nPlayerNum;
    unsigned long nNextMode = m_nNextMode;
    switch (nNextMode) {
        case MODE_CHARA_SELECT_0:
        case MODE_CHARA_SELECT_1:
            SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
            SendObjEventTrigger(OBJ_UNK_42, 0x36, (void *)2);
            CCameraMenuBG::s_Instance.PushBack(0x21, GetCameraSpeed(0x21), 0.0f);
            SetNextMode(nNextMode, 10.0f);
            break;
        case MODE_TEAM:
            m_pPlayerData->ClearSelectMii();
            // fall through
        default:
            SetNextMode(nNextMode, 0.0f);
            break;
    }
}

void msw::CSelectMenu::Entry_LoopStep10()
{
    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
    Build(OBJ_EXPLAIN);
    SendObjEventTrigger(OBJ_UNK_3F, 0x31, (void *)0x78);
    m_nStep = STEP_EXPLAIN;
}

void msw::CSelectMenu::Entry_LoopStep11()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_EXPLAIN);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pObj->PushButton(padNo0, nBtnKindId);
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_UNK_3F, 0x32, (void *)0x6E);
        m_nStep = STEP_EXPLAIN_CLOSE;
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::Entry_LoopStep12()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_EXPLAIN);
    if (pObj == 0 || pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
        Scratch(OBJ_EXPLAIN);
        m_nStep = STEP_SELECT;
    }
}

void msw::CSelectMenu::Entry_LoopStep20()
{
    CGamePadCheck::GetInstance()->RemoveControllerTypeErrorMessage();
    if (CCockpit::IsEndObj(COCKPIT_OBJ_CONTROLLER_ERROR)) {
        long nPlayerNum = m_nPlayerNum;
        CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
        CGamePadCheck::GetInstance()->WindowControllerError("ExhibitionMenuMessage", "ENT_B_00", "ENT_B_01", nPlayerNum, 0);
        CGamePadCheck::GetInstance()->SetDispCursorFrameCount(nPlayerNum);
        m_nStep = STEP_ERROR;
    }
}

// The error window is open: closes it once enough remotes are there.
void msw::CSelectMenu::Entry_LoopStep21()
{
    long nPlayerNum = m_nPlayerNum;
    long nConnect = 0;
    for (long i = 0; i < nPlayerNum; i++) {
        if (CMenuPad::s_Instance.IsConnect(i)) {
            nConnect++;
        }
        // The result is not used.
        CMenuPad::s_Instance.IsNunchakuStyle(i);
    }

    if (nConnect < nPlayerNum) {
        if (CMenuPad::s_Instance.GetInput(CMenuPadInfo::s_Instance.GetPadLeader()) & PAD_A) {
            CGamePadCheck::GetInstance()->WindowControllerErrorClose();
            SendObjEventTrigger(OBJ_UNK_3F, 0x32, (void *)0x6E);
            m_fEntryErrorWait = 0.0f;
            m_nStep = STEP_ERROR_CLOSE;
        }
    } else if (Entry_CheckConnectPad(m_nPlayerNum) == 0) {
        CGamePadCheck::GetInstance()->WindowControllerErrorClose();
        m_fEntryErrorWait = 120.0f;
        m_nStep = STEP_ERROR_WAIT;
    }
}

void msw::CSelectMenu::Entry_LoopStep22()
{
    if (!CCockpit::IsEndObj(COCKPIT_OBJ_CONTROLLER_ERROR)) {
        return;
    }

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_ENTRY);
    for (long nBtnKindId = BTN_KIND_PLAYER_1; nBtnKindId <= BTN_KIND_PLAYER_4; nBtnKindId++) {
        CMenuPartBase *pPart = pObj->m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
        if (pPart != 0) {
            pPart->InitButtonAnim();
        }
    }

    if (Entry_CheckConnectPad(m_nPlayerNum) == 0) {
        if ((long)m_Unk9B == ENTRY_WIFI) {
            WifiEntry_ChangeStateInit();
        } else {
            Entry_ChangeStateInit(m_nNextMode);
        }
    } else {
        m_nStep = STEP_SELECT;
    }
}

void msw::CSelectMenu::Entry_LoopStep23()
{
    static const long padNo0 = 0;

    if (!CCockpit::IsEndObj(COCKPIT_OBJ_CONTROLLER_ERROR)) {
        return;
    }

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        SendObjEventTrigger(OBJ_UNK_3F, 0x32, (void *)0x6E);
        m_fEntryErrorWait = 0.0f;
        m_nStep = STEP_ERROR_CLOSE;
    } else {
        if (m_fEntryErrorWait > 0.0f) {
            return;
        }
        if (Entry_CheckConnectPad(m_nPlayerNum) < 0) {
            m_nStep = STEP_ERROR;
        } else {
            SendObjEventTrigger(OBJ_UNK_3F, 0x32, (void *)0x6E);
            m_nStep = STEP_ERROR_CLOSE;
        }
    }
}

// ---------------------------------------------------------------------------
// msw::SelectMenu::CEntry
// ---------------------------------------------------------------------------

// The names in this namespace are guesses.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 0x10,
        ANIM_WIN_OUT = 0x11,
        ANIM_WIN_IN_BACK = 0x12,
        ANIM_WIN_OUT_BACK = 0x14,
        // The opening animation Set() picks by the kind of entry.
        ANIM_ENTRY_TEAM = 5,
        ANIM_ENTRY_ONE = 6,
        ANIM_ENTRY_ALL = 7,
        // A pad's icons.
        ANIM_ICON_NONE = 8,
        ANIM_ICON_FLASH = 9,
        ANIM_ICON_OFF = 0xA,
        ANIM_ICON_SWAP = 0xC,
        ANIM_ICON_ON = 0xD,
        // A pad's player panel.
        ANIM_PLAYER_OFF = 0xE,
        ANIM_PLAYER_ON = 0xF,
        ANIM_PLAYER_TEXT_OFF = 0x16,
        ANIM_PLAYER_TEXT_ON = 0x17
    };

    enum {
        PANE_WIN = 0
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // The button kind ids Set() rewires.
    enum {
        BTN_KIND_OK = 0x8010,
        BTN_KIND_0 = 0x8011,
        BTN_KIND_1 = 0x8012,
        BTN_KIND_2 = 0x8013,
        BTN_KIND_BACK = 0x8020
    };

    // What CButtonEntry::SetUpButton returns for the buttons Set() needs.
    enum {
        BTN_ID_0 = 0x11,
        BTN_ID_1 = 0x12,
        BTN_ID_BACK = 0x20
    };

    // The extension bit of CMenuPadInfo's flag byte at 0x04.
    enum {
        PADINFO_EXTENSION = 0x02
    };

    const char *const s_LayoutName = "me_entry.brlyt";
    const char *const s_ButtonFileName = "me_entry.btn";

    // The panes of each pad (by pad number).
    unsigned char s_PanePlayer[msw::SelectMenu::CEntry::PAD_NUM] = { 0x02, 0x04, 0x06, 0x08 };
    unsigned char s_PaneIcon0[msw::SelectMenu::CEntry::PAD_NUM] = { 0x0A, 0x0D, 0x10, 0x13 };
    unsigned char s_PaneIcon1[msw::SelectMenu::CEntry::PAD_NUM] = { 0x0B, 0x0E, 0x11, 0x14 };
    unsigned char s_PaneIcon2[msw::SelectMenu::CEntry::PAD_NUM] = { 0x0C, 0x0F, 0x12, 0x15 };
}

msw::SelectMenu::CEntry::CEntry(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CEntry::~CEntry()
{
    Remove();
}

void msw::SelectMenu::CEntry::Main()
{
    StateControl();
}

void msw::SelectMenu::CEntry::Disp()
{
}

void msw::SelectMenu::CEntry::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            CheckControllerState();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
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
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CEntry::Set()
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

            unsigned short nButton0;
            unsigned short nButton1;
            unsigned short nButtonBack;
            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                // The kind id of the button's first neighbour, 16 bits.
                switch ((unsigned short)SetUpButton(pPart, i, 0)) {
                    case BTN_ID_0:
                        nButton0 = i;
                        break;
                    case BTN_ID_1:
                        nButton1 = i;
                        break;
                    case BTN_ID_BACK:
                        nButtonBack = i;
                        pPart->m_nFlags40 |= 0x0004;
                        break;
                }
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            CMenuPartButton *pButton;
            switch (pSelectMenu->m_Unk9B) {
                case ENTRY_UNK_3:
                case ENTRY_WIFI:
                    // Only one and two players.
                    pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_1)->m_nFlags40 |= 0x0001;
                    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_2)->m_nFlags40 |= 0x0001;
                    pButton = (CMenuPartButton *)m_pMenuPartList->FindObject(nButtonBack);
                    ResetButtonID(pButton);
                    ReFineButtonID(pButton, BTN_KIND_BACK, 0, 0, 0, BTN_KIND_0);
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_ENTRY_TEAM, 1.0f, 0.0f, 1, 1);
                    break;
                case ENTRY_TOURNAMENT:
                    // No four players.
                    pButton = (CMenuPartButton *)pMenuPartList->FindObject(nButton0);
                    ResetButtonID(pButton);
                    ReFineButtonID(pButton, BTN_KIND_0, 0, BTN_KIND_1, BTN_KIND_BACK, BTN_KIND_OK);
                    pButton = (CMenuPartButton *)m_pMenuPartList->FindObject(nButton1);
                    ResetButtonID(pButton);
                    ReFineButtonID(pButton, BTN_KIND_1, BTN_KIND_OK, 0, BTN_KIND_BACK, 0);
                    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_2)->m_nFlags40 |= 0x0001;
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_ENTRY_ONE, 1.0f, 0.0f, 1, 1);
                    break;
                default:
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_ENTRY_ALL, 1.0f, 0.0f, 1, 1);
                    break;
            }

            for (long i = 0; i < PAD_NUM; i++) {
                m_fCtrlWait[i] = 0.0f;
                m_bPadOn[i] = 0;
                m_nCtrlState[i] = CTRL_UNSET;
            }
            CheckControllerState();

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CEntry::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CEntry::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(1, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CEntry::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntry::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntry::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CEntry::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CEntry::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CEntry::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_OK;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CEntry::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CEntry::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into StateControl() although defined after it. Defined down here
// because its local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CEntry::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CEntry::CheckControllerState()
{
    int bChanged = 0;

    for (long i = 0; i < PAD_NUM; i++) {
        unsigned char nFlags = CMenuPadInfo::s_Instance.GetPadFlags(i);
        long bExtension = 0;
        if ((nFlags & PADINFO_REMOTE) && (nFlags & PADINFO_EXTENSION)) {
            bExtension = 1;
        }
        unsigned char nState = (nFlags & PADINFO_REMOTE) | (bExtension ? 2 : 0);

        if (nState == m_nCtrlState[i]) {
            continue;
        }
        // A change is shown only once it has lasted.
        m_fCtrlWait[i] -= bcs::CMainApp::GetAppFrameRateF();
        if (m_fCtrlWait[i] > 0.0f) {
            continue;
        }

        switch (nState) {
            case CTRL_NONE:
                m_pPaneAccessor->PlayAnimation(s_PaneIcon2[i], ANIM_ICON_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PaneIcon1[i], ANIM_ICON_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_NONE, 1.0f, 0.0f, 1, 0);
                m_fCtrlWait[i] = 60.0f;
                break;
            case CTRL_REMOTE:
                if (m_nCtrlState[i] & PADINFO_REMOTE) {
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_SWAP, 1.0f, 0.0f, 1, 0);
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon1[i], ANIM_ICON_SWAP, 1.0f, 0.0f, 1, 0);
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_FLASH, 1.0f, 0.0f, 1, 0);
                } else {
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_OFF, 1.0f, 0.0f, 1, 0);
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon1[i], ANIM_ICON_OFF, 1.0f, 0.0f, 1, 0);
                }
                m_pPaneAccessor->PlayAnimation(s_PaneIcon2[i], ANIM_ICON_ON, 1.0f, 0.0f, 1, 0);
                m_fCtrlWait[i] = 0.0f;
                break;
            case CTRL_EXTENSION:
                if (m_nCtrlState[i] & PADINFO_REMOTE) {
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon2[i], ANIM_ICON_SWAP, 1.0f, 0.0f, 1, 0);
                } else {
                    m_pPaneAccessor->PlayAnimation(s_PaneIcon2[i], ANIM_ICON_OFF, 1.0f, 0.0f, 1, 0);
                }
                m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PaneIcon1[i], ANIM_ICON_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PaneIcon0[i], ANIM_ICON_FLASH, 1.0f, 0.0f, 1, 0);
                m_fCtrlWait[i] = 0.0f;
                break;
        }
        bChanged = 1;
        m_nCtrlState[i] = nState;
    }

    if (bChanged) {
        // Pads take part from pad 0 up to the first one without a remote.
        int bOn = 1;
        for (long i = 0; i < PAD_NUM; i++) {
            if ((CMenuPadInfo::s_Instance.GetPadFlags(i) & PADINFO_REMOTE) && bOn) {
                m_pPaneAccessor->PlayAnimation(s_PanePlayer[i], ANIM_PLAYER_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PanePlayer[i], ANIM_PLAYER_TEXT_ON, 1.0f, 0.0f, 1, 0);
                m_bPadOn[i] = 1;
            } else {
                m_pPaneAccessor->PlayAnimation(s_PanePlayer[i], ANIM_PLAYER_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_PanePlayer[i], ANIM_PLAYER_TEXT_OFF, 1.0f, 0.0f, 1, 0);
                m_bPadOn[i] = 0;
                bOn = 0;
            }
        }
    }
}
