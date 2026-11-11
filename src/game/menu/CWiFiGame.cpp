#include "game/msw/SelectMenu/CWiFiGame.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/SelectMenu/CCharSel.hpp"
#include "game/msw/SelectMenu/CPlusMinus.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/os/OSTime.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the screen's opening and closing animations. A table,
    // not a scalar, as in game/menu/CHowTo.cpp: the original converts the
    // value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_TASK = 1,
        OBJ_PLUS_MINUS = 2,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_UNK_40 = 0x40,
        OBJ_CHARSEL = 0x45
    };

    // The button kind ids of the screen: the four games (one per
    // CSelectMenu::m_nGameKind), the random game, and the back button.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_GAME_0 = 0x8010,
        BTN_KIND_GAME_1 = 0x8011,
        BTN_KIND_GAME_2 = 0x8012,
        BTN_KIND_GAME_3 = 0x8013,
        BTN_KIND_RANDOM_STYLE = 0x8014, // what the random button sets
        BTN_KIND_RANDOM = 0x80A0,
        GAME_NUM = 4
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_RANDOM = 0x80
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SELECT = 0,
        STEP_WAIT = 1,       // a game is chosen: wait, then go on
        STEP_ERROR = 20,     // the friend match was lost
        STEP_ERROR_MSG = 21,
        STEP_CLOSE = 22
    };

    // CSelectMenu::m_bStarMii in a friend match (names guessed).
    enum {
        ROLE_NONE = 0,
        ROLE_HOST = 1,  // chooses the game
        ROLE_GUEST = 2  // follows the host's cursor
    };

    // Values of bcs::dwc::CDwc::GetStat() tested here (names guessed).
    enum {
        DWC_STAT_READY = 4,
        DWC_STAT_ERROR = 10
    };

    // What CCockpit::GetResult() gives once the message window is closed.
    enum {
        WINDOW_CLOSED = 1000
    };

    // The modes the screen goes on to.
    enum {
        MODE_CHARA_SELECT = 0xE,
        MODE_UNK_1A = 0x1A,
        MODE_FRIEND_VS_WAIT = 0x28,
        MODE_FRIEND_VS = 0x2A
    };

    // The events sent here (meanings guessed from the other screens).
    enum {
        EVENT_TITLE = 0x11,
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21, // plus the pad number
        EVENT_OUT_START = 0x25,
        EVENT_IS_OUT_READY = 0x26,
        EVENT_BG_CHR = 0x33,
        EVENT_COURT_CHR = 0x35,
        EVENT_UNK_47 = 0x47
    };

    // The camera animation of the screen.
    enum {
        CAMERA_ANIM = 0x29
    };

    // Sound effect of the guest's buttons.
    enum {
        SE_PUSH = 0xBBD
    };

    // Whether nBtnKindId is one of the four game buttons.
    inline bool IsGameButton(long nBtnKindId)
    {
        return (unsigned long)(nBtnKindId - BTN_KIND_GAME_0) <= GAME_NUM - 1;
    }
}

void msw::CSelectMenu::WifiGame_In()
{
    // static-order swap, as in CSelectMenu::FriendVS_In (see
    // notes/decisions.md).
    static const long objId = 0x25;
    static const long padNo0 = 0;

    bcs::BCS_SRand(OSGetTime());
    m_nStage = 0;
    m_nStageSub = 0;
    Build(objId);

    if ((long)m_Unk9B == 4 && m_bStarMii != ROLE_NONE) {
        if (m_bStarMii == ROLE_HOST) {
            SendObjEventTrigger(OBJ_TOP, EVENT_TITLE, (void *)GetMessageLabelString("WifiMenuMessage", "W_GAME_06_00"));
        } else if (m_bStarMii == ROLE_GUEST) {
            SendObjEventTrigger(OBJ_TOP, EVENT_TITLE, (void *)GetMessageLabelString("WifiMenuMessage", "W_GAME_06_01"));
            menu::CVibEff::s_Instance.SetEnable(0);
        }
    } else {
        SendObjEventTrigger(OBJ_TOP, EVENT_TITLE, (void *)GetMessageLabelString("WifiMenuMessage", "W_GAME_06_00"));
    }
    SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, EVENT_COURT_CHR, 0);
    SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, (void *)1);
    CCameraMenuBG::s_Instance.Play(CAMERA_ANIM, 1.0f, 0.0f);

    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        if ((long)m_Unk9B == 4 && m_bStarMii != ROLE_NONE) {
            // The game the host falls back to when the time runs out.
            m_nCupBtnKindId[0] = BTN_KIND_GAME_0;
            dwc::CGameDwc::SetUpTimer(10.0f, 0);
        }
    }

    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHARSEL);
    if (pCharSel != 0) {
        pCharSel->m_Unk64C[0] = 0;
        pCharSel->m_Unk64C[1] = 0;
        pCharSel->m_Unk64C[2] = 0;
        pCharSel->m_Unk64C[3] = 0;
    }
    SetFadeIn(30.0f);
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::WifiGame_Loop()
{
    // static-order swap, as in WifiGame_In().
    static const long objId = 0x25;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    switch (m_nStep) {
        case STEP_SELECT: {
            unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
            long nBtnKindId = rBtnKindId;
            unsigned long bDecide = (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) != 0;

            if ((long)m_Unk9B == 4 && m_bStarMii != ROLE_NONE) {
                // The other side's data.
                const WiFiSingleData *pData = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(0);
                const WiFiSingleData *pData1 = (const WiFiSingleData *)m_pGameDwc->m_Dwc.GetSingleDataA(1);
                if (pData->m_nRole == m_bStarMii) {
                    pData = pData1;
                }
                if (!m_pGameDwc->m_Dwc.IsMatchPlay()) {
                    dwc::CGameDwc::CloseTimer();
                    m_nStep = STEP_ERROR;
                    return;
                }
                if (m_bStarMii == ROLE_HOST) {
                    m_MySingleData.m_nBtnKindId = nBtnKindId;
                    m_MySingleData.m_nInput = bDecide;
                    m_pGameDwc->m_Dwc.SetMySingleDataA(&m_MySingleData, sizeof(m_MySingleData));
                } else if (m_bStarMii == ROLE_GUEST) {
                    // Follow the host's cursor.
                    bDecide = pData->m_nInput;
                    nBtnKindId = pData->m_nBtnKindId;
                    rBtnKindId = nBtnKindId;
                    pObj->m_nPushButtonId[padNo0] = nBtnKindId;
                    pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
                    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 |= 3;
                    m_aPadSelect[0].m_Unk03 = 0x40;
                }
                if (dwc::CGameDwc::CountTimer() && m_bStarMii == ROLE_HOST) {
                    // Time is up: the host's choice is taken as it is.
                    dwc::CGameDwc::CloseTimer();
                    if (!IsGameButton(nBtnKindId)) {
                        nBtnKindId = m_nCupBtnKindId[0];
                        if (!IsGameButton(nBtnKindId)) {
                            nBtnKindId = BTN_KIND_GAME_0;
                        }
                    }
                    m_MySingleData.m_nBtnKindId = nBtnKindId;
                    m_MySingleData.m_nInput = PAD_A;
                    m_pGameDwc->m_Dwc.SetMySingleDataA(&m_MySingleData, sizeof(m_MySingleData));
                    WifiGame_SetPlayStyle(nBtnKindId);
                    pObj->PushButton(padNo0, nBtnKindId);
                    SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, 0);
                    m_fInputWait = 180.0f;
                    m_nStep = STEP_WAIT;
                    return;
                }
            }

            if (bDecide) {
                switch (nBtnKindId) {
                    case BTN_KIND_GAME_0:
                    case BTN_KIND_GAME_1:
                    case BTN_KIND_GAME_2:
                    case BTN_KIND_GAME_3:
                        dwc::CGameDwc::CloseTimer();
                        WifiGame_SetPlayStyle(nBtnKindId);
                        pObj->PushButton(padNo0, nBtnKindId);
                        SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, 0);
                        if (m_bStarMii == ROLE_NONE) {
                            SetNextMode(MODE_CHARA_SELECT, pObj);
                        } else {
                            m_fInputWait = 180.0f;
                            m_nStep = STEP_WAIT;
                        }
                        break;
                    case BTN_KIND_BACK:
                        dwc::CGameDwc::CloseTimer();
                        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                        break;
                    case BTN_KIND_RANDOM:
                        dwc::CGameDwc::CloseTimer();
                        pObj->PushButton(padNo0, BTN_KIND_RANDOM);
                        WifiGame_SetPlayStyle(BTN_KIND_RANDOM_STYLE);
                        SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, 0);
                        SetNextMode(MODE_CHARA_SELECT, pObj);
                        break;
                }
            } else {
                if (m_bStarMii == ROLE_NONE) {
                    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
                        dwc::CGameDwc::CloseTimer();
                        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                        return;
                    }
                    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RANDOM) {
                        dwc::CGameDwc::CloseTimer();
                        pObj->PushButton(padNo0, BTN_KIND_RANDOM);
                        WifiGame_SetPlayStyle(BTN_KIND_RANDOM_STYLE);
                        SendObjEventTrigger(OBJ_UNK_40, EVENT_UNK_47, 0);
                        SetNextMode(MODE_CHARA_SELECT, pObj);
                        return;
                    }
                    // A friend has asked for a match.
                    if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
                        CancelVsFriend();
                    }
                    if (m_pVsFriend != 0) {
                        SetNextMode(MODE_FRIEND_VS, pObj);
                        return;
                    }
                }
                InputDataToMenuObj(pObj, padNo0, nPad, nMode);
            }
            break;
        }
        case STEP_WAIT:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                SetNextMode(MODE_FRIEND_VS_WAIT, pObj);
            }
            break;
        case STEP_ERROR:
            m_pGameDwc->WindowA_Button("WiFiErrorMessage", "WiFi_16_00", 0, 0, 1, 0);
            m_nStep = STEP_ERROR_MSG;
            break;
        case STEP_ERROR_MSG:
            if (CCockpit::GetResult() == WINDOW_CLOSED) {
                m_pGameDwc->ClearWindowResult();
                m_nStep = STEP_CLOSE;
            }
            break;
        case STEP_CLOSE:
            if (m_pGameDwc->m_Dwc.GetStat() == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (m_pGameDwc->m_Dwc.GetStat() == DWC_STAT_READY) {
                SendObjEventTrigger(OBJ_TASK, EVENT_PUSH, (void *)1);
                m_nCurrentBtnKindId[padNo0][m_nMode] = BTN_KIND_BACK;
                SetNextMode(MODE_UNK_1A, _GetModeChangeWaitTime());
            }
            break;
    }
}

void msw::CSelectMenu::WifiGame_Out()
{
    // static-order swap, as in WifiGame_In().
    static const long objId = 0x25;
    static const long padNo0 = 0;

    Scratch(objId);
    menu::CVibEff::s_Instance.SetEnable(1);
    CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 &= ~1;
    CMenuPadInfo::s_Instance.m_aPad[padNo0].m_nFlags06 &= ~2;
    m_aPadSelect[0].m_Unk03 = 0;
    if (m_bStarMii == ROLE_GUEST) {
        if (m_UnkA78 == 1) {
            CMenuPad::s_Instance.ChangeInputStickModeForce(0);
        } else {
            CMenuPad::s_Instance.ChangeInputPointerModeForce(0);
        }
    }
    m_pPlayerData->ClearSelectMii();
}

void msw::CSelectMenu::WifiGame_SetPlayStyle(long nBtnKindId)
{
    switch (nBtnKindId) {
        case BTN_KIND_GAME_0:
        case BTN_KIND_GAME_2:
        case BTN_KIND_GAME_3:
            if ((long)m_nPlayerNum == 2) {
                m_nPlayMode = 6;
            } else {
                m_nPlayMode = 0;
            }
            m_nGameKind = nBtnKindId - BTN_KIND_GAME_0;
            break;
        case BTN_KIND_GAME_1:
            if ((long)m_nPlayerNum == 2) {
                m_nPlayMode = 6;
            } else {
                m_nPlayMode = 4;
            }
            m_nGameKind = nBtnKindId - BTN_KIND_GAME_0;
            break;
        case BTN_KIND_RANDOM_STYLE:
            if ((long)m_nPlayerNum == 2) {
                m_nPlayMode = 6;
            } else {
                m_nPlayMode = 0;
            }
            m_nGameKind = -1;
            break;
    }
    InitCharaSelectParam();
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    const char *const s_LayoutName = "me_Wi-Fi_top_B.brlyt";
    const char *const s_ButtonFileName = "me_Wi-Fi_top_B.btn";

    const char *s_MovieName = "movie/Wi-Fi_top.thp";

    // The picture panes the movie is shown in, by name and by index.
    const char *s_MoviePaneNames[msw::SelectMenu::CWiFiGame::MOVIE_PANE_NUM] = {
        "moviePic_01",
        "moviePic_02",
        "moviePic_03",
        "moviePic_04"
    };

    const unsigned char s_MoviePanes[msw::SelectMenu::CWiFiGame::MOVIE_PANE_NUM] = { 0xB, 0xC, 0xD, 0xE };

    // The pane of each game button (by kind id less BTN_KIND_GAME_0).
    const unsigned char s_GamePanes[GAME_NUM] = { 6, 4, 8, 0xA };

    enum {
        ANIM_PUSH = 5,
        ANIM_WIN_IN = 6,
        ANIM_WIN_OUT = 7,
        ANIM_WIN_IN_NO_BACK = 8,
        ANIM_WIN_OUT_NO_BACK = 0xA,
        ANIM_MOVIE_IN = 0xC
    };

    enum {
        PANE_WIN = 0
    };

    // What CreatePicturePane is given for the movie panes (meaning unknown).
    const unsigned char MOVIE_PANE_ARG = 4;
}

msw::SelectMenu::CWiFiGame::CWiFiGame(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_nMovieState = MOVIE_NONE;
    m_Unk5A0 = 0;
}

msw::SelectMenu::CWiFiGame::~CWiFiGame()
{
    Remove();
}

void msw::SelectMenu::CWiFiGame::Main()
{
    MovieControl();

    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_NO_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
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
            } else if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_NO_BACK)) {
                bEnd = 1;
            }
            if (bEnd && m_nMovieState == MOVIE_PLAY) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() && m_nMovieState == MOVIE_PLAY &&
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_IS_OUT_READY, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_NO_BACK, s_WinAnimSpeed[0], 0.0f, recursive,
                                                   reset);
                }
                GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_OUT_START, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_NO_BACK)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CWiFiGame::MovieControl()
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
            if (pMovie->IsWait() == 1 && pMovie->Open(s_MovieName, 1, 0) == 1) {
                m_nMovieState = MOVIE_START;
            }
            break;
        case MOVIE_START:
            if (pMovie->IsReady() && pMovie->Start(0)) {
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

                // This and the moviePaneNum of MOVIE_PLAY below are stored
                // in the reverse of the order their symbols are numbered
                // in; both are 4, so only the names are exchanged:
                // static-order swap (see notes/decisions.md).
                static const int moviePaneNum = MOVIE_PANE_NUM;

                for (long i = 0; i < moviePaneNum; i++) {
                    nw4r::lyt::Pane *pPane = bcs::G2D::CreatePicturePane(s_MoviePaneNames[i], MOVIE_PANE_ARG);
                    if (pPane == 0) {
                        continue;
                    }

                    bcs::G2D::CPaneObj *pPaneObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPane);
                    if (pPaneObj == 0) {
                        continue;
                    }

                    bcs::G2D::CPaneObj *pFrame = m_pPaneAccessor->GetPaneObj(s_MoviePanes[i]);

                    pPaneCtrl->AppendPaneObj(pPaneObj, (const char *)0);
                    bcs::G2D::SetPaneParent(pPaneObj->GetPane(), pFrame->GetPane(), 0, 0);
                    pPaneObj->GetPane()->SetSize(pFrame->GetPane()->GetSize());
                    bcs::G2D::SetupMoviePane(pPaneObj, pFrame);
                }
                m_nMovieState = MOVIE_PLAY;
            }
            break;
        case MOVIE_PLAY:
            if (pMovie->IsPlay()) {
                static const int moviePaneNum = MOVIE_PANE_NUM;

                for (long i = 0; i < moviePaneNum; i++) {
                    bcs::G2D::RemapMoviePane(m_pPaneAccessor->GetPane(s_MoviePaneNames[i], 1, 0));
                    // The original tests the address of the array, not
                    // m_bDispWait[i], so this is always true once the movie
                    // can be drawn; kept as it is.
                    if (pMovie->CanbeDraw() && m_bDispWait != 0) {
                        m_fDispWait[i] -= bcs::CMainApp::GetAppFrameRateF();
                        if (m_fDispWait[i] < 0.0f) {
                            m_pPaneAccessor->SetPaneDisp(s_MoviePanes[i], 1);
                            m_pPaneAccessor->PlayAnimation(s_MoviePanes[i], ANIM_MOVIE_IN, 1.0f, 0.0f, 1, 1);
                            m_bDispWait[i] = 0;
                        }
                    }
                }
            } else if (!pMovie->CanbeDraw()) {
                m_nMovieState = MOVIE_NONE;
            }
            break;
    }
}

void msw::SelectMenu::CWiFiGame::Disp()
{
}

int msw::SelectMenu::CWiFiGame::Set()
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

            if (pSelectMenu->m_bStarMii == ROLE_NONE) {
                // The help line of the minus button.
                m_bPlusMinus = 1;
                pSelectMenu->Build(OBJ_PLUS_MINUS);
                CPlusMinus *pPlusMinus = static_cast<CPlusMinus *>(pSelectMenu->FindObj(OBJ_PLUS_MINUS));
                pPlusMinus->StartUp(0, 0, GetMessageLabelString("List_Plus_Minus_Button", "Minus_03"));
            } else {
                m_bPlusMinus = 0;
            }

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();
            InitPaneAnim();

            m_nMovieState = MOVIE_STOP;

            static const int moviePaneNum = MOVIE_PANE_NUM;

            for (long i = 0; i < moviePaneNum; i++) {
                m_pPaneAccessor->SetPaneDisp(s_MoviePanes[i], 0);
                m_bDispWait[i] = 1;
                m_fDispWait[i] = 30.0f;
            }
            m_bVibEnable = 1;
            m_bSeEnable = 1;
        }
    }
    return 0;
}

// Inlined into Main(); defined after Set() because its local static
// follows Set()'s in .rodata.
inline void msw::SelectMenu::CWiFiGame::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    if (m_bPlusMinus) {
        GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, backEvent, m_nPushButtonId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, backEvent, m_nPushButtonId);
}

// Shows the movie's frames. Inlined into Set(), as above.
inline void msw::SelectMenu::CWiFiGame::InitPaneAnim()
{
    static const int moviePaneNum = MOVIE_PANE_NUM;

    for (long i = 0; i < moviePaneNum; i++) {
        bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(s_MoviePanes[i]);
        if (pPaneObj != 0) {
            pPaneObj->GetPane()->SetVisible(true);
        }
    }
}

void msw::SelectMenu::CWiFiGame::Remove()
{
    GetSelectMenu()->Scratch(OBJ_PLUS_MINUS);
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

long msw::SelectMenu::CWiFiGame::HitCheck(bcs::FVEC3 *pPos)
{
    // The guest's cursor is where the host's is.
    CSelectMenu *pSelectMenu = GetSelectMenu();
    if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii == ROLE_GUEST) {
        return m_nMarkerBtnKindId;
    }
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0 && m_bPlusMinus) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, EVENT_HIT, pPos);
        if (nResult == 0) {
            nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, EVENT_HIT, pPos);
        }
    }
    return nResult;
}

long msw::SelectMenu::CWiFiGame::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiGame::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiGame::SetCursorMarkerPosition(long, long nBtnKindId)
{
    m_nMarkerBtnKindId = nBtnKindId;
    return 0;
}

long msw::SelectMenu::CWiFiGame::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CWiFiGame::StickMoveCheck(long nPad, long nBtnKindId)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii == ROLE_GUEST) {
        return nBtnKindId;
    }
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CWiFiGame::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList == 0) {
        return BTN_KIND_GAME_0;
    }
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_GAME_0;
}

int msw::SelectMenu::CWiFiGame::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CWiFiGame::PushButton(long nPad, long nBtnKindId)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii != ROLE_NONE) {
        // The guest's screen only shows the host's choice.
        SE_Play(SE_PUSH, 1);
        m_pPaneAccessor->PlayAnimation(s_GamePanes[nBtnKindId - BTN_KIND_GAME_0], ANIM_PUSH, 1.0f, 0.0f, 1, 1);
        return nBtnKindId;
    }
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }

    long nResult = 0;
    long nEvent = nPad + EVENT_PUSH;
    if (m_bPlusMinus) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_TASK, nEvent, (void *)nBtnKindId);
    }
    GetSelectMenu()->SendObjEventTrigger(OBJ_PLUS_MINUS, nEvent, (void *)nBtnKindId);
    return nResult;
}
