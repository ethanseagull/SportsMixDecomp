#include "game/msw/SelectMenu/CMode.hpp"
#include "game/msw/SelectMenu/CTournamentLoad.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations; [1]
    // is not read here. A table, not a scalar, as in game/menu/CHowTo.cpp.
    const long s_WinAnimSpeed[2] = { 2, 0 };

    // The tables declared with one entry more than they are given end in a
    // zero word in the game's .rodata, which nothing reads; the sizes are
    // read from there.

    // What the background characters play when the screen is left (by
    // the place they stand in); the second table when bit 0x08 of the
    // player data is set (a fifth place).
    const long s_LeaveChrAnim[4] = { 20, 21, 23, 24 };
    const long s_LeaveChrAnimAlt[6] = { 20, 21, 22, 23, 24 };

    // The camera position for each place a background character can stand
    // in, as in game/menu/CHowTo.cpp.
    const long s_CamPos[4] = { 10, 11, 12, 13 };
    const long s_CamPosAlt[6] = { 10, 11, 21, 12, 13 };

    // The camera animation once a mode has been chosen.
    const long s_DecideCamPos[4] = { 14, 15, 16, 17 };
    const long s_DecideCamPosAlt[5] = { 14, 15, 15, 16, 17 };

    // What GameMode_SetTopChar plays on the top character (by game kind,
    // 5 read as 4), coming in and not, and where it puts it with bit 0x08
    // of the player data set (by game kind).
    const long s_TopChrInAnim[5] = { 15, 16, 18, 19, 17 };
    const long s_TopChrAnim[6] = { 0, 1, 3, 4, 2 };
    const long s_TopChrPos[6] = { 0, 1, 3, 4, -1, 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_MODE = 8,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_3B = 0x3B,
        OBJ_DWC_WINDOW = 0x3F,
        OBJ_UNK_42 = 0x42,
        OBJ_TOURNAMENT_LOAD = 0xFF
    };

    // Events sent to the menu objects.
    enum {
        EVENT_LINE_MESSAGE = 0x11, // to OBJ_TOP: the help line
        EVENT_LOAD_OPEN = 0x29,    // to OBJ_UNK_01
        EVENT_LOAD_CANCEL = 0x2A,  // to OBJ_UNK_01
        EVENT_MODE_CLOSE = 0x2E,   // to OBJ_MODE
        EVENT_MODE_GET_STATE = 0x30,
        EVENT_WINDOW_CLOSE = 0x32, // to OBJ_DWC_WINDOW
        EVENT_MODE_DECIDE = 0x51,  // to OBJ_UNK_01
        EVENT_MODE_BACK = 0x52     // to OBJ_UNK_01
    };

    // The button kind ids of the screens.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_MODE_1 = 0x1C,
        BTN_KIND_MODE_0 = 0x1D,
        BTN_KIND_MODE_2 = 0x1E,
        BTN_KIND_TUTORIAL_0 = 0x1F,
        BTN_KIND_TUTORIAL_1 = 0x20,
        BTN_KIND_TUTORIAL_2 = 0x21,
        BTN_KIND_LOAD_CONTINUE = 0x8010,
        BTN_KIND_LOAD_NEW = 0x8011,
        BTN_KIND_LOAD_BACK = 0x8012
    };

    // Values of CSelectMenu::m_nStep while the screens are shown.
    enum {
        STEP_MAIN = 0,
        STEP_DECIDE = 1,
        STEP_DECIDE_ANIM = 2,
        STEP_LOAD_WAIT = 10,
        STEP_LOAD = 11,
        STEP_LOAD_NEW = 12,
        STEP_NEW = 13,
        STEP_CONTINUE = 14,
        STEP_NO_MII = 15,
        STEP_PAD_ERROR = 20,
        STEP_PAD_ERROR_2 = 21,
        STEP_LOAD_RESET = 22,
        STEP_CONTINUE_CHECK = 23,
        STEP_TUTORIAL = 30,
        STEP_MODE_CLOSE = 40,
        STEP_MODE_REBUILD = 41,
        STEP_FADE = 100,
        STEP_FADE_WAIT = 101,
        STEP_NEXT_MODE = 102
    };

    // Values of CSelectMenu::m_nNextMode set here.
    enum {
        MODE_TOP = 4,
        MODE_SINGLE = 6,
        MODE_TOURNAMENT = 9,
        MODE_TUTORIAL_MENU = 0x2D,
        MODE_TUTORIAL = 0x31
    };

    // CSelectMenu::m_nGameKind of the mix tournament.
    enum {
        GAME_KIND_MIX = 5
    };

    // Answers of the message windows (CCockpit::GetResult).
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };
}

void msw::CSelectMenu::GameMode_SetTopChar(int bIn)
{
    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);

    if ((unsigned long)m_nGameKind > GAME_KIND_MIX) {
        m_nGameKind = 0;
    }

    // The mix tournament uses the place of game kind 4.
    long nKind = m_nGameKind;
    if (nKind == GAME_KIND_MIX) {
        nKind = 4;
    }

    if (bIn) {
        pBgChr->PlayAnim(s_TopChrInAnim[nKind], 1.0f, 5000.0f);
    } else {
        pBgChr->PlayAnim(s_TopChrAnim[nKind], 1.0f, 5000.0f);
    }

    long nPos = nKind;
    if (m_pPlayerData->m_nFlags5C50 & 8) {
        nPos = s_TopChrPos[m_nGameKind];
    }
    pBgChr->SetPosChara(nPos);
    pBgChr->SetSelectGameChara();
}

void msw::CSelectMenu::GameMode_In()
{
    const long *pCamPos;
    static const long padNo0 = 0;

    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
    if (m_Unk9D != MODE_TOP && pBgChr != 0) {
        GameMode_SetTopChar(1);
    }

    if (m_nGameKind == GAME_KIND_MIX) {
        // The mix tournament has no mode screen: straight to the saved
        // tournament check.
        m_fInputWait = 30.0f;
        m_nStep = STEP_LOAD_WAIT;
    } else {
        m_fInputWait = 0.0f;
        if (m_Unk9D == MODE_TUTORIAL_MENU) {
            SetFadeIn(60.0f);
        }

        SelectMenu::CMode *pObj = (SelectMenu::CMode *)FindObj(OBJ_MODE);
        pObj->m_nType = SelectMenu::CMode::TYPE_MODE;
        Build(OBJ_MODE);
        SendObjEventTrigger(OBJ_BG_CHR, 0x33, (void *)1);
        SendObjEventTrigger(OBJ_UNK_3B, 0x35, 0);
        pBgChr->SetDispTagName(0);
        SendObjEventTrigger(OBJ_UNK_42, 0x3A, (void *)-1);
        m_Unk27B = 0;
        ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        if (m_Unk23E == 1) {
            pObj->m_nPushButtonId[padNo0] = nBtnKindId;
            GameMode_SetLineMessage(nBtnKindId);
        } else {
            GameMode_SetLineMessage(0);
        }

        if (pBgChr != 0) {
            pBgChr->SetSelectGameChara();
            pCamPos = s_CamPos;
            if (m_pPlayerData->m_nFlags5C50 & 8) {
                pCamPos = s_CamPosAlt;
            }
            if (m_Unk9D != MODE_TOP) {
                CCameraMenuBG::s_Instance.PushBackWithoutReady(pCamPos[pBgChr->GetPosChara(padNo0)], 1.0f, 5000.0f);
            }
        }
        pBgChr->SetCharaSpeed(1.0f);
        m_nStep = STEP_MAIN;
    }
    m_nStage = 0;
}

void msw::CSelectMenu::GameMode_Loop()
{
    switch (m_nStep) {
        default:
            GameMode_LoopStep0();
            break;
        case STEP_DECIDE:
            GameMode_LoopStep1();
            break;
        case STEP_DECIDE_ANIM:
            GameMode_LoopStep2();
            break;
        case STEP_LOAD_WAIT: {
            CGamePadCheck *pPadCheck = CGamePadCheck::GetInstance();
            pPadCheck->m_Unk38 = 0;
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f && ((SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR))->IsAnimEnd() &&
                CCameraMenuBG::s_Instance.IsEndAnim()) {
                m_pPlayerData = CSave::GetPlayerData(-1);
                if (!m_pPlayerData->IsValid()) {
                    m_nStep = STEP_NEW;
                } else if (CTournament::IsContinue(m_nGameKind, -1)) {
                    SendObjEventTrigger(OBJ_UNK_01, EVENT_LOAD_OPEN, 0);
                    SelectMenu::CTournamentLoad *pLoad = new SelectMenu::CTournamentLoad(OBJ_TOURNAMENT_LOAD);
                    AddObj(pLoad);
                    pLoad->Set();
                    m_nCupBtnKindId[0] = BTN_KIND_LOAD_CONTINUE;
                    m_nCurrentBtnKindId[0][m_nMode] = BTN_KIND_LOAD_CONTINUE;
                    pLoad->m_nPushButtonId[0] = BTN_KIND_LOAD_CONTINUE;
                    pLoad->SetCursorMarkerPosition(0, BTN_KIND_LOAD_CONTINUE);
                    m_fInputWait = 30.0f;
                    m_nStep = STEP_LOAD;
                } else {
                    CTournament::Initialize(1);
                    m_nStep = STEP_NEW;
                }
            }
            break;
        }
        case STEP_LOAD: {
            // The game stores this static after the one of
            // STEP_MODE_REBUILD (both 0, so only the two symbol addresses
            // are exchanged): static-order swap (see notes/decisions.md).
            static const long padNo0 = 0;

            long nMode = m_nMode;
            long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

            CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TOURNAMENT_LOAD);
            if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                return;
            }
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait > 0.0f) {
                return;
            }
            m_fInputWait = 0.0f;

            CGamePadCheck *pPadCheck;
            long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
                pObj->PushButton(padNo0, nBtnKindId);
                switch (nBtnKindId) {
                    case BTN_KIND_LOAD_CONTINUE:
                        if (CTournament::IsContinueNotMiiData(m_nGameKind, -1)) {
                            WindowA_Button("TournamentMenuMessage", "TOU_LOAD_08", 0, 0);
                            m_nStep = STEP_NO_MII;
                        } else {
                            GameMode_TournamentCheckConnectPad();
                        }
                        break;
                    case BTN_KIND_LOAD_NEW:
                        WindowYesNo_Button("TournamentMenuMessage", "TOU_LOAD_07", 1, 0, 0, 1);
                        m_nStep = STEP_LOAD_NEW;
                        break;
                    case BTN_KIND_LOAD_BACK:
                        GameMode_TournamentBackCheck();
                        pPadCheck = CGamePadCheck::GetInstance();
                        pPadCheck->m_Unk38 = 1;
                        break;
                }
            } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
                GameMode_TournamentBackCheck();
                pPadCheck = CGamePadCheck::GetInstance();
                pPadCheck->m_Unk38 = 1;
            } else {
                InputDataToMenuObj(pObj, padNo0, nPad, nMode);
            }
            break;
        }
        case STEP_LOAD_NEW:
            switch (CCockpit::GetResult()) {
                case RESULT_YES:
                    RemoveObj(OBJ_TOURNAMENT_LOAD);
                    CTournament::Initialize(1);
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    m_nStep = STEP_NEW;
                    break;
                case RESULT_NO: {
                    // Back to the window, its buttons reset.
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    CSelectMenuLayoutBase *pLoad = (CSelectMenuLayoutBase *)FindObj(OBJ_TOURNAMENT_LOAD);
                    for (long i = BTN_KIND_LOAD_CONTINUE; i <= BTN_KIND_LOAD_BACK; i++) {
                        CMenuPartBase *pPart = pLoad->m_pMenuPartList->FindObjectByCurrentBtnKindId(i);
                        if (pPart != 0) {
                            pPart->InitButtonAnim();
                        }
                    }
                    m_nStep = STEP_LOAD;
                    break;
                }
            }
            break;
        case STEP_NEW:
            m_Unk9B = 1;
            m_UnkA00 = 0;
            GameMode_ChangeStateInit(MODE_TOURNAMENT);
            CGamePadCheck::GetInstance()->m_Unk38 = 1;
            break;
        case STEP_CONTINUE: {
            long nLevel = m_pPlayerData->IsDifficult(m_nGameKind);
            CMenuOption::s_Instance.GetPlayerSaveData(CSave::GetUsePlayerID(), nLevel);
            SetPadDefault();
            CMenuOption::s_Instance.SetGameParameter(RuleSel_GetOptionKindId());
            if ((long)m_nPlayerNum != 1) {
                CGame::g_GameWork.m_nUnk1C = 0;
                CGame::SetTeamChrChangeCtrlType(0, 0);
                CGame::SetTeamChrChangeCtrlType(1, 0);
            }
            m_Unk9B = 1;
            m_UnkA00 = 1;
            GameMode_ChangeStateInit(MODE_TUTORIAL);
            CGamePadCheck::GetInstance()->m_Unk38 = 1;
            break;
        }
        case STEP_NO_MII:
            if (CCockpit::GetResult() == RESULT_YES) {
                RemoveObj(OBJ_TOURNAMENT_LOAD);
                CTournament::Initialize(1);
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_nStep = STEP_NEW;
            }
            break;
        case STEP_PAD_ERROR:
            Entry_LoopStep20();
            break;
        case STEP_PAD_ERROR_2:
            Entry_LoopStep21();
            break;
        case STEP_LOAD_RESET: {
            CSelectMenuLayoutBase *pLoad = (CSelectMenuLayoutBase *)FindObj(OBJ_TOURNAMENT_LOAD);
            for (long i = BTN_KIND_LOAD_CONTINUE; i <= BTN_KIND_LOAD_BACK; i++) {
                CMenuPartBase *pPart = pLoad->m_pMenuPartList->FindObjectByCurrentBtnKindId(i);
                if (pPart != 0) {
                    pPart->InitButtonAnim();
                }
            }
            m_nStep = STEP_LOAD;
            break;
        }
        case STEP_CONTINUE_CHECK:
            GameMode_TournamentCheckConnectPad();
            break;
        case STEP_TUTORIAL:
            GameMode_LoopStep30();
            break;
        case STEP_MODE_CLOSE:
            if (SendObjEventTrigger(OBJ_MODE, EVENT_MODE_GET_STATE, 0) == CSelectMenuLayoutBase::STATE_NONE) {
                Scratch(OBJ_MODE);
                m_nStep = STEP_MODE_REBUILD;
            }
            break;
        case STEP_MODE_REBUILD: {
            static const long padNo0 = 0;

            SelectMenu::CMode *pObj = (SelectMenu::CMode *)FindObj(OBJ_MODE);
            Build(OBJ_MODE);
            unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
            long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
            rBtnKindId = nBtnKindId;
            pObj->m_nPushButtonId[padNo0] = nBtnKindId;
            pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
            m_nStep = pObj->m_nReturnStep;
            break;
        }
        case STEP_FADE:
            GameMode_LoopStep100();
            break;
        case STEP_FADE_WAIT:
            GameMode_LoopStep101();
            break;
        case STEP_NEXT_MODE:
            SetNextMode(m_nNextMode, 0.0f);
            break;
    }
}

void msw::CSelectMenu::GameMode_LoopStep0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    // The result is not used.
    FindObj(OBJ_BG_CHR);

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_MODE);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_MODE_0:
                SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_DECIDE, 0);
                m_Unk9B = 0;
                pObj->PushButton(padNo0, nBtnKindId);
                GameMode_ChangeStateInit(MODE_TOURNAMENT);
                break;
            case BTN_KIND_MODE_1:
                SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_DECIDE, 0);
                pObj->PushButton(padNo0, nBtnKindId);
                m_nStep = STEP_LOAD_WAIT;
                break;
            case BTN_KIND_MODE_2:
                SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_DECIDE, 0);
                m_Unk9B = 0;
                pObj->PushButton(padNo0, nBtnKindId);
                GameMode_ChangeStateInit(MODE_SINGLE);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, MODE_TOP, pObj, 1);
                GameMode_ChangeStateInit(m_nNextMode);
                SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_BACK, 0);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_BACK, 0);
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        GameMode_ChangeStateInit(m_nNextMode);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        GameMode_SetLineMessage(m_nCurrentBtnKindId[padNo0][nMode]);
    }
}

void msw::CSelectMenu::GameMode_ChangeStateInit(long nState)
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_MODE);
    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);

    m_nNextMode = nState;
    switch (nState) {
        case MODE_TUTORIAL_MENU:
        case MODE_TOURNAMENT:
        case MODE_TUTORIAL:
            if (m_nGameKind == GAME_KIND_MIX) {
                m_nStep = STEP_FADE;
            } else {
                if (pObj != 0) {
                    pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
                }
                if (pBgChr != 0) {
                    pBgChr->PrepareDecideAnimChara();
                }
                m_nStep = STEP_DECIDE;
            }
            break;
        case MODE_SINGLE:
            pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
            m_nStep = STEP_NEXT_MODE;
            break;
        default:
            if (pObj != 0) {
                pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
            }
            if (pBgChr != 0) {
                long nPos = pBgChr->GetPosChara(0);
                const long *pChrAnim;
                const long *pCamPos;
                if (m_pPlayerData->m_nFlags5C50 & 8) {
                    pChrAnim = s_LeaveChrAnimAlt;
                    pCamPos = s_CamPosAlt;
                } else {
                    pChrAnim = s_LeaveChrAnim;
                    pCamPos = s_CamPos;
                }
                pBgChr->PlayAnim(pChrAnim[nPos], 1.0f, 0.0f);
                CCameraMenuBG::s_Instance.Play(pCamPos[nPos], -1.0f, 0.0f);
            }
            m_nStep = STEP_DECIDE;
            break;
    }
    PlayMenuBGM(1);
}

void msw::CSelectMenu::GameMode_LoopStep1()
{
    switch ((unsigned long)m_nNextMode) {
        case MODE_TOP:
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                SetNextMode(m_nNextMode, 0.0f);
            }
            break;
        case MODE_TUTORIAL_MENU:
        case MODE_TOURNAMENT:
        case MODE_TUTORIAL: {
            SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
            if (pBgChr->IsEndPrepareDecideAnimChara()) {
                pBgChr->PlayDecideAnimChara();
                long nPos = pBgChr->GetPosChara(0);
                const long *pCamPos = s_DecideCamPos;
                CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
                if (m_pPlayerData->m_nFlags5C50 & 8) {
                    pCamPos = s_DecideCamPosAlt;
                }
                rCamera.Play(pCamPos[nPos], 1.0f, 0.0f);
                m_nStep = STEP_DECIDE_ANIM;
            }
            break;
        }
    }
}

void msw::CSelectMenu::GameMode_LoopStep2()
{
    static const long padNo0 = 0;

    if (CMenuPad::s_Instance.GetInput(padNo0) & 0x30) {
        m_nStep = STEP_FADE;
        return;
    }

    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
    if ((m_nNextMode == MODE_TUTORIAL_MENU || m_nNextMode == MODE_TOURNAMENT || m_nNextMode == MODE_TUTORIAL) &&
        pBgChr->IsEndDecideAnimChara()) {
        m_nStep = STEP_FADE;
        pBgChr->ResetChara();
    }
}

void msw::CSelectMenu::GameMode_LoopStep30()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    // The result is not used.
    FindObj(OBJ_BG_CHR);

    SelectMenu::CMode *pObj = (SelectMenu::CMode *)FindObj(OBJ_MODE);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_TUTORIAL_0:
                pObj->PushButton(padNo0, nBtnKindId);
                GameMode_ChangeStateInit(MODE_TUTORIAL_MENU);
                break;
            case BTN_KIND_TUTORIAL_1:
                CTournament::Tutorial(m_nGameKind, 0, 0);
                m_Unk9B = 1;
                m_UnkA00 = 1;
                GameMode_ChangeStateInit(MODE_TUTORIAL);
                break;
            case BTN_KIND_TUTORIAL_2:
                // The game starts the first tutorial here too.
                CTournament::Tutorial(m_nGameKind, 0, 0);
                m_Unk9B = 1;
                m_UnkA00 = 1;
                GameMode_ChangeStateInit(MODE_TUTORIAL);
                break;
            case BTN_KIND_BACK:
                SendObjEventTrigger(OBJ_MODE, EVENT_MODE_CLOSE, 0);
                pObj->m_nType = SelectMenu::CMode::TYPE_MODE;
                pObj->m_nReturnStep = STEP_MAIN;
                m_nStep = STEP_MODE_CLOSE;
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        pObj->PushButton(padNo0, BTN_KIND_BACK);
        SendObjEventTrigger(OBJ_MODE, EVENT_MODE_CLOSE, 0);
        pObj->m_nType = SelectMenu::CMode::TYPE_MODE;
        pObj->m_nReturnStep = STEP_MAIN;
        m_nStep = STEP_MODE_CLOSE;
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        GameMode_SetLineMessage(m_nCurrentBtnKindId[padNo0][nMode]);
    }
}

void msw::CSelectMenu::GameMode_LoopStep100()
{
    static const float endWaitTime = 10.0f;

    SetWhiteFadeOut(endWaitTime);
    m_nStep = STEP_FADE_WAIT;
}

void msw::CSelectMenu::GameMode_LoopStep101()
{
    if (bcs::CMainApp::IsPlayMasterFade()) {
        return;
    }

    SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, 0);
    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
    CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
    if (rCamera.IsEndAnim()) {
        pBgChr->SetCharaSpeed(1.0f);
        pBgChr->ResetChara();
        m_fInputWait = 10.0f;
        m_nStep = STEP_NEXT_MODE;
    } else {
        pBgChr->SetCharaSpeed(500.0f);
        rCamera.SetSpeed(500.0f);
    }
}

void msw::CSelectMenu::GameMode_Out()
{
    Scratch(OBJ_MODE);
}

void msw::CSelectMenu::GameMode_TournamentCheckConnectPad()
{
    long nPlayerNum = CTournament::GetContinuePlayer(m_nGameKind, -1);
    m_nPlayerNum = nPlayerNum;
    if (Entry_CheckConnectPad(nPlayerNum) < 0) {
        m_nStep = STEP_PAD_ERROR;
        return;
    }

    RemoveObj(OBJ_TOURNAMENT_LOAD);
    CTournament::Continue(m_nGameKind, -1);
    CGamePadCheck::GetInstance()->m_Unk3C = m_nPlayerNum;
    CGamePadCheck *pPadCheck = CGamePadCheck::GetInstance();
    pPadCheck->m_Unk38 = 1;
    m_nStep = STEP_CONTINUE;
}

void msw::CSelectMenu::GameMode_SetLineMessage(long nBtnKindId)
{
    if (nBtnKindId == BTN_KIND_MODE_0) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_02_01"));
    } else if (nBtnKindId == BTN_KIND_MODE_1) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_02_00"));
    } else if (nBtnKindId == BTN_KIND_MODE_2) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_02_02"));
    } else if (nBtnKindId == BTN_KIND_TUTORIAL_0) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("Text_Rule_Common", "Rule_Common_Menu_01_00"));
    } else if (nBtnKindId == BTN_KIND_TUTORIAL_1) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("Text_Rule_Common", "Rule_Common_Menu_01_01"));
    } else if (nBtnKindId == BTN_KIND_TUTORIAL_2) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("Text_Rule_Common", "Rule_Common_Menu_01_01"));
    } else if (m_nStep != STEP_TUTORIAL) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_02_03"));
    }
}

void msw::CSelectMenu::GameMode_TournamentBackCheck()
{
    if (m_nGameKind == GAME_KIND_MIX) {
        RemoveObj(OBJ_TOURNAMENT_LOAD);
        SendObjEventTrigger(OBJ_UNK_01, EVENT_MODE_BACK, 0);
        GoBackToMode(0, GetPrevMode(), 0, 1);
        GameMode_ChangeStateInit(m_nNextMode);
    } else {
        SendObjEventTrigger(OBJ_UNK_01, EVENT_LOAD_CANCEL, 0);
        RemoveObj(OBJ_TOURNAMENT_LOAD);
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_02_03"));
        m_nStep = STEP_MAIN;
    }
}

// The names in this namespace are guesses; the objects have no symbols.
// They are defined here, after the CSelectMenu steps, because their strings
// follow those steps' constants in .rodata. All of them are in .data.
namespace {

    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_BTN_COLOR_00, // one colour per game kind
        ANIM_BTN_COLOR_01,
        ANIM_BTN_COLOR_02,
        ANIM_BTN_COLOR_03,
        ANIM_BTN_VIS_00,
        ANIM_BTN_VIS_01,
        ANIM_BTN_VIS_02,
        ANIM_BTN_VIS_03,
        ANIM_BTN_VIS_04,
        ANIM_MULTI_W00,
        ANIM_MULTI_W01,
        ANIM_MULTI_W02,
        ANIM_MULTI_W03,
        ANIM_MULTI_W04,
        ANIM_MULTI_W05,
        ANIM_MULTI_W06,
        ANIM_NUM
    };

    enum {
        PANE_MULTI_W00_POS,
        PANE_MP_BTN_00,
        PANE_MP_BTN_01,
        PANE_MP_BTN_09,
        PANE_MP_BTN_10,
        PANE_MP_BTN_11,
        PANE_T_MP_BTN_00,
        PANE_T_MP_BTN_01,
        PANE_T_MP_BTN_09,
        PANE_T_MP_BTN_10,
        PANE_T_MP_BTN_11,
        PANE_B_MP_BTN_00,
        PANE_B_MP_BTN_01,
        PANE_B_MP_BTN_09,
        PANE_B_MP_BTN_10,
        PANE_B_MP_BTN_11,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 3
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_CLOSE_CHECK = 0x26,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // What the mode screen answers in SendEvent.
    enum {
        EVENT_CLOSE = 0x2E,
        EVENT_GET_STATE = 0x30
    };

    const char *s_LayoutName = "me_multiplay_00.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "me_multiplay_00_btn_00.brlan",
        "me_multiplay_00_btn_01.brlan",
        "me_multiplay_00_btn_02.brlan",
        "me_multiplay_00_btn_03.brlan",
        "me_multiplay_00_btn_04.brlan",
        "me_multiplay_00_btn_color_00.brlan",
        "me_multiplay_00_btn_color_01.brlan",
        "me_multiplay_00_btn_color_02.brlan",
        "me_multiplay_00_btn_color_03.brlan",
        "me_multiplay_00_btn_vis_00.brlan",
        "me_multiplay_00_btn_vis_01.brlan",
        "me_multiplay_00_btn_vis_02.brlan",
        "me_multiplay_00_btn_vis_03.brlan",
        "me_multiplay_00_btn_vis_04.brlan",
        "me_multiplay_00_multi_w00.brlan",
        "me_multiplay_00_multi_w01.brlan",
        "me_multiplay_00_multi_w02.brlan",
        "me_multiplay_00_multi_w03.brlan",
        "me_multiplay_00_multi_w04.brlan",
        "me_multiplay_00_multi_w05.brlan",
        "me_multiplay_00_multi_w06.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "multi_w00_pos",
        "mp_btn_00",
        "mp_btn_01",
        "mp_btn_09",
        "mp_btn_10",
        "mp_btn_11",
        "t_mp_btn_00",
        "t_mp_btn_01",
        "t_mp_btn_09",
        "t_mp_btn_10",
        "t_mp_btn_11",
        "b_mp_btn_00",
        "b_mp_btn_01",
        "b_mp_btn_09",
        "b_mp_btn_10",
        "b_mp_btn_11"
    };

    // Size 0x14. A button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nTextPane;          // 0x02
        unsigned short m_nNextBtnKindId[5]; // 0x04
        const char *m_pLabel;               // 0x10
    };

    // The buttons of CMode::TYPE_TUTORIAL (labels in "Text_Rule_Common").
    ButtonParam s_TutorialButtonParam[BUTTON_NUM] = {
        { 3, PANE_MP_BTN_09, PANE_T_MP_BTN_09, { 0x20, 0x20, 0x21, 0, 0 }, "Rule_Common_Menu_00_00" },
        { 4, PANE_MP_BTN_10, PANE_T_MP_BTN_10, { 0x21, 0x20, 0x1F, 0, 0 }, "Rule_Common_Menu_00_01" },
        { 5, PANE_MP_BTN_11, PANE_T_MP_BTN_11, { 0x1F, 0x21, 0x1F, 0, 0 }, "Rule_Common_Menu_00_02" }
    };

    // The buttons of CMode::TYPE_MODE (labels in "ExhibitionMenuMessage").
    ButtonParam s_ModeButtonParam[BUTTON_NUM] = {
        { 3, PANE_MP_BTN_09, PANE_T_MP_BTN_09, { 0x1D, 0x1D, 0x1C, 0, 0 }, "MODE_01_01" },
        { 4, PANE_MP_BTN_10, PANE_T_MP_BTN_10, { 0x1C, 0x1D, 0x1E, 0, 0 }, "MODE_01_00" },
        { 5, PANE_MP_BTN_11, PANE_T_MP_BTN_11, { 0x1E, 0x1C, 0x1E, 0, 0 }, "MODE_01_02" }
    };

    // The kind id the cursor falls back to, by CMode::m_nType.
    long s_DefaultBtnKindId[4] = { 0, 0, BTN_KIND_TUTORIAL_1, BTN_KIND_MODE_0 };
}

msw::SelectMenu::CMode::CMode(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CMode::~CMode()
{
    Remove();
}

void msw::SelectMenu::CMode::Main()
{
    StateControl();

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CMode::Disp()
{
}

void msw::SelectMenu::CMode::StateControl()
{
    if (m_pPaneAccessor == 0) {
        return;
    }

    switch (m_Unk30) {
        case STATE_SET: {
            // The game has both of these as symbols, `recursive` numbered
            // before `reset` but stored after it: static-order swap (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, ANIM_MULTI_W01, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_MULTI_W00_POS, ANIM_MULTI_W01)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() &&
                GetSelectMenu()->SendObjEventTrigger(1, EVENT_CLOSE_CHECK, 0) != 0) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                long nAnim = IsBackFrameOut(0) ? ANIM_MULTI_W06 : ANIM_MULTI_W03;
                m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, nAnim, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT: {
            long nAnim = IsBackFrameOut(0) ? ANIM_MULTI_W06 : ANIM_MULTI_W03;
            if (m_pPaneAccessor->IsAnimEnd(PANE_MULTI_W00_POS, nAnim)) {
                m_Unk30 = STATE_NONE;
            }
            break;
        }
    }
}

int msw::SelectMenu::CMode::Set()
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < BUTTON_NUM; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const ButtonParam *pParam = 0;
                if (m_nType == TYPE_TUTORIAL) {
                    pParam = &s_TutorialButtonParam[i];
                } else if (m_nType == TYPE_MODE) {
                    pParam = &s_ModeButtonParam[i];
                }

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nAnim[0] = ANIM_BTN_01;
                pPart->m_nAnim[1] = ANIM_BTN_02;
                pPart->m_nAnim[2] = ANIM_BTN_03;
                pPart->m_nAnim[3] = ANIM_BTN_00;
                pPart->m_nAnim[4] = ANIM_BTN_04;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = 0xBBC;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 1;
                pPart->m_bUnk40_0020 = 1;
                pPart->InitButtonAnim();

                if (m_nType == TYPE_TUTORIAL) {
                    const wchar_t *pText = GetMessageLabelString("Text_Rule_Common", pParam->m_pLabel);
                    m_pPaneAccessor->SetTextBoxString(pParam->m_nTextPane, pText, wcslen(pText));
                } else if (m_nType == TYPE_MODE) {
                    const wchar_t *pText = GetMessageLabelString("ExhibitionMenuMessage", pParam->m_pLabel);
                    m_pPaneAccessor->SetTextBoxString(pParam->m_nTextPane, pText, wcslen(pText));
                }
            }
            m_pMenuPartList = pMenuPartList;

            PaneColorInit();
        }

        m_Unk2C = 0;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CMode::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CMode::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (nEvent == EVENT_GET_STATE) {
        nResult = m_Unk30;
    } else if (nEvent == EVENT_CLOSE) {
        m_Unk30 = STATE_CLOSE;
        nResult = 0;
    }
    return nResult;
}

long msw::SelectMenu::CMode::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CMode::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CMode::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CMode::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CMode::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CMode::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CMode::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = s_DefaultBtnKindId[m_nType];
    }
    return nBtnKindId;
}

int msw::SelectMenu::CMode::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CMode::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into StateControl() although defined after it; the out-of-line
// copy is not called and the link drops it. Defined down here because its
// local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CMode::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Plays the opening animation of the buttons and the colour of the game
// kind. Inlined into Set().
inline void msw::SelectMenu::CMode::PaneColorInit()
{
    // static-order swap: the game stores `reset` first.
    static const int recursive = 1;
    static const int reset = 1;

    m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, ANIM_BTN_VIS_04, 1.0f, 0.0f, recursive, reset);

    long nAnim;
    switch (GetSelectMenu()->m_nGameKind) {
        case 0:
            nAnim = ANIM_BTN_COLOR_00;
            break;
        case 1:
            nAnim = ANIM_BTN_COLOR_01;
            break;
        case 2:
            nAnim = ANIM_BTN_COLOR_02;
            break;
        case 3:
            nAnim = ANIM_BTN_COLOR_03;
            break;
        default:
            nAnim = ANIM_BTN_COLOR_00;
            break;
    }
    m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, nAnim, 1.0f, 0.0f, recursive, reset);
}

// The names in this namespace are guesses. Defined here because the
// strings follow CMode's constants in .rodata.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_LOAD_IN = 5,
        ANIM_LOAD_OUT_END = 6,
        ANIM_LOAD_OUT = 7,
        ANIM_LOAD_IN_2 = 8
    };

    enum {
        PANE_LOAD_WIN = 0,
        PANE_LOAD_CUP_POS = 1,
        PANE_LOAD_RANK_POS = 2,
        PANE_LOAD_TEXT_PLAYER = 3,
        PANE_LOAD_TEXT_CUP = 4,
        PANE_LOAD_CUP_0 = 0xB, // one per cup
        PANE_LOAD_RANK_0 = 0xE,
        PANE_LOAD_RANK_1 = 0xF,
        PANE_LOAD_RANK_3 = 0x10
    };

    enum {
        LOAD_TEXT_BUF_LEN = 0x20
    };

    const char *const s_LoadLayoutName = "me_to_load.brlyt";
    const char *const s_LoadButtonFileName = "me_to_load.btn";
}

msw::SelectMenu::CTournamentLoad::~CTournamentLoad()
{
    Remove();
}

void msw::SelectMenu::CTournamentLoad::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            // Both branches are the same in the game.
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_LOAD_WIN, ANIM_LOAD_IN, 1.0f, 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_LOAD_WIN, ANIM_LOAD_IN_2, 1.0f, 0.0f, reset, recursive);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_LOAD_WIN, ANIM_LOAD_IN, 1.0f, 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_LOAD_WIN, ANIM_LOAD_IN_2, 1.0f, 0.0f, reset, recursive);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_LOAD_WIN, ANIM_LOAD_IN)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case 6:
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_LOAD_WIN, ANIM_LOAD_OUT, 1.0f, 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_LOAD_WIN, ANIM_LOAD_OUT_END)) {
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

void msw::SelectMenu::CTournamentLoad::Disp()
{
}

int msw::SelectMenu::CTournamentLoad::Set()
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
            ArchiveBuildLoader(s_LoadButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LoadLayoutName);
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
            ClearPushButtonId();

            nw4r::lyt::Pane *pCupPane = 0;
            nw4r::lyt::Pane *pRankPane = 0;
            nw4r::math::VEC3 trans(0.0f, 0.0f, 0.0f);

            // The cup of the saved tournament.
            switch (CTournament::GetContinueCup(pSelectMenu->m_nGameKind, -1)) {
                case 1:
                    SetTextPaneString(PANE_LOAD_TEXT_CUP, GetMessageLabelString("TournamentMenuMessage", "CUP_05"));
                    pCupPane = m_pPaneAccessor->GetPane(PANE_LOAD_CUP_0);
                    m_pPaneAccessor->SetPaneDisp(PANE_LOAD_CUP_0, 1);
                    break;
                case 2:
                    SetTextPaneString(PANE_LOAD_TEXT_CUP, GetMessageLabelString("TournamentMenuMessage", "CUP_06"));
                    pCupPane = m_pPaneAccessor->GetPane(PANE_LOAD_CUP_0 + 1);
                    m_pPaneAccessor->SetPaneDisp(PANE_LOAD_CUP_0 + 1, 1);
                    break;
                case 3:
                    SetTextPaneString(PANE_LOAD_TEXT_CUP, GetMessageLabelString("TournamentMenuMessage", "CUP_07"));
                    pCupPane = m_pPaneAccessor->GetPane(PANE_LOAD_CUP_0 + 2);
                    m_pPaneAccessor->SetPaneDisp(PANE_LOAD_CUP_0 + 2, 1);
                    break;
            }
            bcs::G2D::SetPaneParent(pCupPane, m_pPaneAccessor->GetPane(PANE_LOAD_CUP_POS), 1, 0);
            pCupPane->SetTranslate(trans);
            m_pPaneAccessor->SetPaneDisp(PANE_LOAD_CUP_POS, 1);

            // How far it had got.
            switch (CTournament::GetContinueRank(pSelectMenu->m_nGameKind, -1)) {
                case 0:
                    if (pSelectMenu->m_nGameKind == GAME_KIND_MIX) {
                        pRankPane = m_pPaneAccessor->GetPane(PANE_LOAD_RANK_3);
                        m_pPaneAccessor->SetPaneDisp(PANE_LOAD_RANK_3, 1);
                    } else {
                        pRankPane = m_pPaneAccessor->GetPane(PANE_LOAD_RANK_0);
                        m_pPaneAccessor->SetPaneDisp(PANE_LOAD_RANK_0, 1);
                    }
                    break;
                case 1:
                    pRankPane = m_pPaneAccessor->GetPane(PANE_LOAD_RANK_1);
                    m_pPaneAccessor->SetPaneDisp(PANE_LOAD_RANK_1, 1);
                    break;
                case 3:
                    pRankPane = m_pPaneAccessor->GetPane(PANE_LOAD_RANK_3);
                    m_pPaneAccessor->SetPaneDisp(PANE_LOAD_RANK_3, 1);
                    break;
            }
            if (pRankPane != 0) {
                bcs::G2D::SetPaneParent(pRankPane, m_pPaneAccessor->GetPane(PANE_LOAD_RANK_POS), 1, 0);
                // The game puts the cup pane back in place again here, not
                // the rank pane.
                pCupPane->SetTranslate(trans);
                m_pPaneAccessor->SetPaneDisp(PANE_LOAD_RANK_POS, 1);
            }

            // The number of players.
            long nPlayerNum = CTournament::GetContinuePlayer(pSelectMenu->m_nGameKind, -1);
            wchar_t text[LOAD_TEXT_BUF_LEN];
            swprintf(text, LOAD_TEXT_BUF_LEN, GetMessageLabelString("TournamentMenuMessage", "TOU_LOAD_01"), nPlayerNum);
            SetTextPaneString(PANE_LOAD_TEXT_PLAYER, text);

            m_nPushButtonId[0] = BTN_KIND_LOAD_CONTINUE;
            SetCursorMarkerPosition(0, BTN_KIND_LOAD_CONTINUE);
        }
    }
    return 0;
}

void msw::SelectMenu::CTournamentLoad::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CTournamentLoad::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CTournamentLoad::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTournamentLoad::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTournamentLoad::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CTournamentLoad::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CTournamentLoad::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CTournamentLoad::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_LOAD_CONTINUE;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CTournamentLoad::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CTournamentLoad::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined down here because its local
// static follows Main()'s constants in .rodata.
void msw::SelectMenu::CTournamentLoad::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Inlined into Set(), as CheckButtonAnim() into Main().
void msw::SelectMenu::CTournamentLoad::SetTextPaneString(long nPane, const wchar_t *pText)
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
