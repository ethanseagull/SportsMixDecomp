#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/G2D/CMenuMdlScnGroup.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "game/msw/SelectMenu/CCharSel.hpp"
#include "game/msw/SelectMenu/CCharaLevel.hpp"
#include "game/msw/SelectMenu/CCharaLevelStar.hpp"
#include "game/msw/SelectMenu/CCharaMdl.hpp"
#include "game/msw/SelectMenu/CCharaName.hpp"
#include "game/msw/SelectMenu/CCharaOk.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CStateSave.hpp"
#include "game/msw/menu/CVibEff.hpp"

// The character select screen of CSelectMenu (MenuF): the character table
// (SelectMenu::CCharSel, menu object 0x45) with the characters on the
// court, and the "OK?" windows that follow it (SelectMenu::CCharaOk 0x13,
// the computer level SelectMenu::CCharaLevel 0x14 and the star level
// SelectMenu::CCharaLevelStar 0x15). The names in this file are guesses.
namespace {

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 1,
        OBJ_UNK_02 = 2,
        OBJ_CHARA_NAME = 0x11,
        OBJ_CHARA_STATUS = 0x12,
        OBJ_CHARA_OK = 0x13,
        OBJ_CHARA_LEVEL = 0x14,
        OBJ_CHARA_LEVEL_STAR = 0x15,
        OBJ_CHARA_MDL = 0x1C,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_UNK_3D = 0x3D,
        OBJ_FADE = 0x3F,
        OBJ_COURT_CHR_2 = 0x42,
        OBJ_CHAR_SEL = 0x45
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_IN = 0x3E8,
        STEP_MAIN = 0x3E9,
        STEP_FRAME_OUT = 0x3F3,
        STEP_OK_DELAY = 0x3F4,
        STEP_OK_INIT = 0x3F5,
        STEP_OK = 0x3F6,
        STEP_OK_OUT = 0x3F7,
        STEP_LEVEL_DELAY = 0x3F8,
        STEP_LEVEL_INIT = 0x3F9,
        STEP_LEVEL = 0x3FA,
        STEP_LEVEL_STAR_DELAY = 0x3FB,
        STEP_LEVEL_STAR_INIT = 0x3FC,
        STEP_LEVEL_STAR = 0x3FD
    };

    // The button kind ids of the screen and its windows.
    enum {
        BTN_BACK = 1,
        BTN_OK_YES = 0x4E,
        BTN_OK_NO = 0x4F,
        BTN_CHARA = 0x8010,        // the 0x12 character buttons
        BTN_LEVEL_DOWN = 0x8040,
        BTN_LEVEL_UP = 0x8041,
        BTN_LEVEL_OK = 0x8045,
        BTN_LEVEL_CANCEL = 0x8046,
        BTN_MII = 0x8050,
        BTN_RANDOM = 0x80A0,
        BTN_RANDOM_2 = 0x80A4,
        BTN_COLOR = 0x80A5
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_POINTER = 0x100,
        PAD_PLUS = 0x400,
        PAD_MINUS = 0x800
    };

    // The character that stands for the player's Mii.
    enum {
        CHARA_MII = 0x13
    };

    // Values of CSelectMenu::m_Unk9B (the kind of game being set up).
    enum {
        GAME_WIFI = 4
    };
}

void msw::CSelectMenu::_SetColorChange(long nPad, long nBtnKindId)
{
    if (!CCameraMenuBG::s_Instance.IsEndAnim()) {
        return;
    }

    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHAR_SEL);
    long nChara, nColor;
    if (pCharSel->FindChrTypeByBtnKind(nBtnKindId, nChara, nColor) >= 0 &&
        Chara_CheckSelectChara(nPad, nChara) == -1) {
        return;
    }

    long nIndex = Chara_FindNotDecideCharaIndex(nPad);
    if (nIndex < 0) {
        return;
    }

    if (nBtnKindId != 0) {
        CharaSelectParam *pParam = GetCharaSelectParam(nIndex);
        if (pParam->m_nChara == 0xFF) {
            return;
        }

        unsigned char nOldColor = m_nCharaColor[nPad];
        if (nBtnKindId != BTN_MII) {
            pCharSel->FindChrTypeChangeColor(nBtnKindId);
            m_nCharaColor[nPad] = pCharSel->GetNextChrColor(nBtnKindId, m_nCharaColor[nPad]);
            Chara_SetSelectChara(nPad, pParam->m_nChara, pCharSel->GetChrColor(nBtnKindId, m_nCharaColor[nPad]),
                                 0);
        }
        if (nOldColor != m_nCharaColor[nPad]) {
            pCharSel->SetCourtCharaObj(nBtnKindId, 1, 2, nPad);
        }
    }
}

void msw::CSelectMenu::_SetRandom(long nPad)
{
    menu::CVibEff::s_Instance.SetEnable(0);
    m_nRandomDecideTime[nPad] = 15;
    m_fRandomMoveTime[nPad] = 15.0f;
    CMenuPadInfo::s_Instance.m_aPad[nPad].m_nFlags06 |= 3;
    m_aPadSelect[nPad].m_Unk03 = 0x40;
}

void msw::CSelectMenu::_DisablePointStick(long nPad, int bDisable)
{
    CMenuPadInfo::PAD &rPad = CMenuPadInfo::s_Instance.m_aPad[nPad];
    if (bDisable) {
        rPad.m_nFlags06 |= 1;
    } else {
        rPad.m_nFlags06 &= ~1;
    }
    if (bDisable) {
        rPad.m_nFlags06 |= 2;
    } else {
        rPad.m_nFlags06 &= ~2;
    }
    if (bDisable) {
        m_aPadSelect[nPad].m_Unk03 = 0x40;
    } else {
        m_aPadSelect[nPad].m_Unk03 = 0;
    }
}

void msw::CSelectMenu::_MoveCursol(long nPad, long nEntry, long nBtnKindId)
{
    if (m_aPadSelect[nPad].m_Unk00 != 1) {
        return;
    }

    long nMode = m_nMode;
    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHAR_SEL);
    long nNext = pCharSel->GetNextButtonKindId(nBtnKindId, 2);
    if (nNext == 0) {
        return;
    }

    pCharSel->m_nPushButtonId[nPad] = nNext;
    m_nCurrentBtnKindId[nPad][nMode] = nNext;
    long nChara, nColor;
    if (pCharSel->FindChrTypeByBtnKind(nNext, nChara, nColor) >= 0) {
        m_nCharaColor[nPad] = 1;
        Chara_SetSelectChara(nEntry, nChara, pCharSel->GetChrColor(nNext, 1), 0);
    }
}

void msw::CSelectMenu::_WiFiMiiSetUp(long nEntry, long nChara)
{
    if (nChara == CHARA_MII && (long)m_Unk9B == GAME_WIFI) {
        if (nEntry == 0) {
            Mii_SelectParam(nEntry, m_pPlayerData->m_nMiiUnk, m_pPlayerData->m_nMiiIndex,
                            m_pPlayerData->m_MiiSource, m_pPlayerData->m_MiiCreateId);
        } else {
            Mii_SelectParam(nEntry, m_nStarMiiUnk, m_nStarMiiIndex, m_StarMiiSource, m_StarMiiCreateId);
        }
    }
}

inline void msw::CSelectMenu::CharSel_SetMiiChara(long nPad)
{
    // The original looks the screen up here and does not use it.
    FindObj(OBJ_CHAR_SEL);
    if ((long)m_Unk9B == GAME_WIFI) {
        if (m_bStarMii) {
            _WiFiMiiSetUp(nPad, CHARA_MII);
        }
    } else {
        Chara_SetSelectChara(nPad, CHARA_MII, 0xFF, 0);
    }
}

void msw::CSelectMenu::CharSel_In()
{
    if (dwc::CGameDwc::GetInstance() == 0) {
        CGamePadCheck::GetInstance()->EnableBatteryCheck(1);
    }

    SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    m_pPlayerData = CSave::GetPlayerData(-1);
    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHAR_SEL);
    if (pCharSel->m_Unk2C != 0) {
        return;
    }

    pCharSel->SetMixChara(0);
    Build(OBJ_CHAR_SEL);
    Build(OBJ_UNK_3D);
    Build(OBJ_CHARA_STATUS);
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    if (m_nGameKind == 5) {
        SendObjEventTrigger(OBJ_COURT_CHR_2, 0x3A, (void *)4);
    } else {
        SendObjEventTrigger(OBJ_COURT_CHR_2, 0x3A, (void *)m_nGameKind);
    }

    long nMode = m_nMode;
    CMenuPadInfo::PAD *pPads = CMenuPadInfo::s_Instance.m_aPad;
    m_Unk27B = 1;
    long aBtnKindId[4];
    long i;
    for (i = 0; i < 4; i++) {
        CMenuPadInfo::PAD &rPad = pPads[i];
        if (rPad.m_nFlags04 & 4) {
            rPad.m_nFlags06 &= ~4;
            rPad.m_nFlags06 &= ~1;
            rPad.m_nFlags06 &= ~2;
        } else {
            rPad.m_nFlags06 |= 4;
        }
        if (m_Unk9D != 0x11) {
            m_nCurrentBtnKindId[i][nMode] = 0;
            pCharSel->m_nPushButtonId[i] = 0;
            aBtnKindId[i] = 0;
            pCharSel->SetCursorMarkerPosition(i, 0);
        }
        if (!(rPad.m_nFlags06 & 4)) {
            aBtnKindId[i] = pCharSel->SetDefaultMoveCursor(i, m_nCurrentBtnKindId[i][nMode]);
        }
        m_nCurrentBtnKindId[i][nMode] = aBtnKindId[i];
        pCharSel->SetCursorMarkerPosition(i, aBtnKindId[i]);
        if (m_aPadSelect[i].m_Unk00 == 1) {
            pCharSel->m_nPushButtonId[i] = aBtnKindId[i];
        }
    }

    for (i = 0; i < 4; i++) {
        m_fRandomMoveTime[i] = -1.0f;
        m_nRandomDecideTime[i] = -1;
    }
    m_Unk688 = BTN_CHARA;

    SelectMenu::CCharaName *pName = (SelectMenu::CCharaName *)FindObj(OBJ_CHARA_NAME);
    SelectMenu::CCharaMdl *pMdl = (SelectMenu::CCharaMdl *)FindObj(OBJ_CHARA_MDL);
    if (pName && pMdl) {
        pName->SetRewriteTextureData(pMdl->GetResTexObj(), GX_TEXMAP0);
    }
    if (pMdl) {
        pMdl->SetPlayerDeploy(m_nPlayerNum);
    }

    // Lights the buttons of the characters already chosen, silently.
    menu::CSndEff::s_Instance.SetEnable(0);
    for (i = 0; i < 6; i++) {
        unsigned long nPlayerKind = m_aCharaSelectParam[i].m_Unk00;
        if (nPlayerKind <= 3 && Chara_IsCharaDecide(i)) {
            pCharSel->PushButtonIn(nPlayerKind, m_aCharaSelectParam[i].m_nChara,
                                   m_aCharaSelectParam[i].m_nCharaSub);
        }
    }
    menu::CSndEff::s_Instance.SetEnable(1);

    for (i = 0; i < 4; i++) {
        m_fCharSelVibWait[i] = 0.0f;
        long nIndex = Chara_FindNotDecideCharaIndex(i);
        if (nIndex < 0 || Chara_IsCharaDecideSelect(nIndex)) {
            continue;
        }

        long nChara, nColor;
        if (pCharSel->FindChrTypeByBtnKind(m_nCurrentBtnKindId[i][m_nMode], nChara, nColor) < 0) {
            continue;
        }
        if (nChara != CHARA_MII) {
            Chara_SetSelectChara(i, nChara, nColor, 0);
            if (nColor == 0xFF) {
                m_nCharaColor[i] = 1;
            } else {
                m_nCharaColor[i] = 1 << nColor;
            }
        } else {
            m_nCharaColor[i] = 1;
            CharSel_SetMiiChara(i);
        }
    }

    if (m_Unk9D == 0x11) {
        for (i = 0; i < 4; i++) {
            if (pPads[i].IsCharSelPad() && Chara_IsAllDecide(i)) {
                pPads[i].m_nFlags06 |= 3;
                m_aPadSelect[i].m_Unk03 = 0x40;
            }
        }
    } else {
        CharSel_CancelLastDecideChara();
    }

    if ((long)m_Unk9B == GAME_WIFI && m_bStarMii) {
        dwc::CGameDwc::SetUpTimer(20.0f, 1);
    }
    m_fCharSelBackWait = 0.0f;
    if (SendObjEventTrigger(OBJ_TOP, 0x2C, 0) || m_Unk9D == 0x11) {
        m_nStep = STEP_MAIN;
    } else {
        m_nStep = STEP_IN;
    }
    SetFadeIn(10.0f);
}

void msw::CSelectMenu::CharSel_Loop()
{
    m_fCharSelBackWait -= bcs::CMainApp::GetAppFrameRateF();
    if (m_fCharSelBackWait <= 0.0f) {
        m_fCharSelBackWait = 0.0f;
    }

    switch (m_nStep) {
    case STEP_IN: {
        // The original leaves nAnim unset when m_Unk9B is not 0, 1 or 2
        // (4 returns before it is used; 3 would send garbage). The
        // declaration order fixes the register allocation (r29 to r27).
        long nAnim;
        long nCamera;
        CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
        switch (m_Unk9B) {
        default:
            nCamera = 0x23;
            break;
        case 1:
            nCamera = 0x22;
            break;
        case 2:
            nCamera = 0x23;
            break;
        case GAME_WIFI: {
            if (m_nGameKind >= 0) {
                SendObjEventTrigger(OBJ_COURT_CHR, 0x3A, (void *)m_nGameKind);
            }
            rCamera.Play(0x22, GetCameraSpeed(0x22), 0.0f);
            // Declared in this order for the original's register choice.
            SelectMenu::CCourtChr *pCourtChr2;
            SelectMenu::CCourtChr *pCourtChr;
            SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
            pCourtChr = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR);
            pCourtChr2 = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR_2);
            if (pBgChr) {
                pBgChr->SetTableAnimSpeed(1.0f);
            }
            if (pCourtChr) {
                pCourtChr->SetTableAnimSpeed(1.0f);
            }
            if (pCourtChr2) {
                pCourtChr2->SetTableAnimSpeed(1.0f);
            }
            if (GetTeamPlayerNum() == 2) {
                SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)0xB);
                SendObjEventTrigger(OBJ_COURT_CHR_2, 0x36, (void *)0xB);
            } else {
                SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)5);
                SendObjEventTrigger(OBJ_COURT_CHR_2, 0x36, (void *)5);
            }
            m_nStep = STEP_MAIN;
            return;
        }
        }

        if (rCamera.IsEndAnim()) {
            switch (m_Unk9B) {
            case 1:
                nAnim = 5;
                break;
            case 0:
            case 2:
                nAnim = 7;
                if (menu::CStateSave::GetInstance()->GetStartMenuMode() == 3) {
                    nAnim = 8;
                    menu::CStateSave::GetInstance()->SetStartMenuMode(0);
                }
                break;
            }
            rCamera.PushBack(nCamera, GetCameraSpeed(nCamera), 0.0f);
            if (nAnim == 5 && GetTeamPlayerNum() == 2) {
                nAnim = 0xB;
            }
            SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)nAnim);
            SendObjEventTrigger(OBJ_COURT_CHR_2, 0x36, (void *)nAnim);
            m_nStep = STEP_MAIN;
        }
        break;
    }

    case STEP_MAIN:
        CharSel_LoopStep();
        break;

    case STEP_OK_DELAY:
        CharSel_OkButtonDelay();
        break;

    case STEP_OK_INIT:
        CharSel_OkButtonInit();
        break;

    case STEP_OK:
        CharSel_OkButton();
        break;

    case STEP_LEVEL_DELAY:
        CharSel_LevelOkBtnDelay();
        break;

    case STEP_LEVEL_INIT:
        CharSel_LevelOkBtnInit();
        break;

    case STEP_LEVEL:
        CharSel_LevelOkBtn();
        break;

    case STEP_LEVEL_STAR_DELAY:
        CharSel_LevelStarOkBtnDelay();
        break;

    case STEP_LEVEL_STAR_INIT:
        CharSel_LevelStarOkBtnInit();
        break;

    case STEP_LEVEL_STAR:
        CharSel_LevelStarOkBtn();
        break;

    case STEP_FRAME_OUT:
        CharSel_FrameOut();
        break;

    case STEP_OK_OUT:
        CharSel_OkButtonOut();
        break;
    }
}

inline void msw::CSelectMenu::CharSel_Back(long nPad, long nMode, SelectMenu::CCharSel *pCharSel)
{
    SetCancelBackMode(nPad, GetPrevMode(), pCharSel, 1);
    CharSel_ChangeStateInit(m_nNextMode);
    long i;
    for (i = 0; i < 4; i++) {
        pCharSel->m_Unk64C[i] = 0;
    }
    for (i = 1; i < 4; i++) {
        m_nCurrentBtnKindId[i][nMode] = 0;
    }
    CharSel_CameraReverse();
    m_pPlayerData->ClearSelectMii();
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
}

void msw::CSelectMenu::CharSel_LoopStep()
{
    static const long objId = OBJ_CHAR_SEL;
    long nMode = m_nMode;
    long nLeader = CMenuPadInfo::s_Instance.GetPadLeader();

    if (CCameraMenuBG::s_Instance.IsEndAnim()) {
        SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
        SelectMenu::CCourtChr *pCourtChr = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR);
        SelectMenu::CCourtChr *pCourtChr2 = (SelectMenu::CCourtChr *)FindObj(OBJ_COURT_CHR_2);
        if (pBgChr) {
            pBgChr->SetTableAnimSpeed(1.0f);
        }
        if (pCourtChr) {
            pCourtChr->SetTableAnimSpeed(1.0f);
        }
        if (pCourtChr2) {
            pCourtChr2->SetTableAnimSpeed(1.0f);
        }
    }

    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(objId);
    if (!pCharSel || pCharSel->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE ||
        bcs::CMainApp::IsPlayMasterFade()) {
        return;
    }
    CSelectMenuLayoutBase *pStatus = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_STATUS);
    if (!pStatus) {
        return;
    }

    // Once every pad taking part has chosen all its characters, the "OK?"
    // window of the kind of game follows.
    CMenuPadInfo::PAD *pPads = CMenuPadInfo::s_Instance.m_aPad;
    long nPad;
    for (nPad = 0; nPad < 4; nPad++) {
        long nEntry = GetPadEntryId(nPad);
        if (pPads[nPad].IsCharSelPad() && !Chara_IsAllDecide(nEntry)) {
            break;
        }
    }
    if (nPad >= 4) {
        if ((long)m_Unk9B == 0) {
            m_nStep = STEP_LEVEL_STAR_DELAY;
        } else if ((long)m_Unk9B == GAME_WIFI || (long)m_Unk9B == 1) {
            m_nStep = STEP_OK_DELAY;
        } else if (m_nPlayMode == 0x16 || m_nPlayMode == 0x17 || m_nPlayMode == 8) {
            m_nStep = STEP_OK_DELAY;
        } else if ((long)m_Unk9B == 2 && m_Unk9C == 3) {
            m_nStep = STEP_OK_DELAY;
        } else {
            m_nStep = STEP_LEVEL_DELAY;
        }
        return;
    }

    // On Wi-Fi the time limit draws the characters still open.
    int bTimeUp = 0;
    if ((long)m_Unk9B == GAME_WIFI && m_bStarMii && dwc::CGameDwc::CountTimer()) {
        dwc::CGameDwc::CloseTimer();
        bTimeUp = 1;
    }

    for (nPad = 0; nPad < 4; nPad++) {
        CMenuPadInfo::PAD &rPad = pPads[nPad];
        if ((rPad.m_nFlags06 & 4) || !(rPad.m_nFlags04 & 4)) {
            continue;
        }
        if (m_nRandomDecideTime[nPad] >= 0) {
            CharSel_RandomSelect(nPad);
            continue;
        }
        if (m_fCharSelVibWait[nPad]) {
            m_fCharSelVibWait[nPad] -= bcs::CMainApp::GetAppFrameRateF();
            if (m_fCharSelVibWait[nPad] <= 0.0f) {
                menu::CVibEff::s_Instance.SetEnable(1);
                m_fCharSelVibWait[nPad] = 0.0f;
            }
        }
        if (bTimeUp) {
            _SetRandom(nPad);
            continue;
        }

        long nBtnKindId = m_nCurrentBtnKindId[nPad][nMode];
        long nEntry = GetPadEntryId(nPad);
        if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_A) && !rPad.IsUsedCursor()) {
            switch (nBtnKindId) {
            default:
                if ((unsigned long)(nBtnKindId - BTN_CHARA) <= 0x11) {
                    long nChara, nColor;
                    if (pCharSel->FindChrTypeByBtnKind(nBtnKindId, nChara, nColor) >= 0 &&
                        Chara_CheckSelectChara(nPad, nChara, nColor) == -1) {
                        return;
                    }
                    if (pCharSel->PushButton(nPad, nBtnKindId) >= 0) {
                        int bDecide = 0;
                        long nIndex = Chara_FindNotDecideCharaIndex(nPad);
                        if (nIndex >= 0) {
                            CharaSelectParam *pParam = GetCharaSelectParam(nIndex);
                            if (pParam && pParam->m_nChara != 0xFF) {
                                Chara_SetDecideCharaWithMotion(nEntry, pParam->m_nChara,
                                                               (pParam->m_nCharaSub << 1) | pParam->m_Unk13[0],
                                                               nBtnKindId);
                                bDecide = 1;
                            }
                        }
                        if (!bDecide) {
                            pCharSel->PushCancelButton(nPad, nBtnKindId);
                        }
                        if (Chara_IsAllDecide(nEntry)) {
                            CMenuPadInfo::s_Instance.m_aPad[nPad].m_nFlags06 |= 3;
                            m_aPadSelect[nPad].m_Unk03 = 0x40;
                        } else {
                            _MoveCursol(nPad, nEntry, nBtnKindId);
                        }
                        return;
                    }
                }
                break;

            case BTN_MII:
                // Not while a random draw runs or the camera moves.
                for (long i = 0; i < 4; i++) {
                    if (m_nRandomDecideTime[i] >= 0) {
                        return;
                    }
                }
                if (!CCameraMenuBG::s_Instance.IsEndAnim() || !pStatus ||
                    pStatus->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
                    return;
                }
                if ((long)m_Unk9B == GAME_WIFI) {
                    if (pCharSel->m_Unk64C[nPad] == 0) {
                        pCharSel->SetPadCharaObjMotion(nPad, 2, 0);
                        long nMiiEntry = GetPadEntryId(nPad);
                        if (nPad == 0) {
                            Mii_SetMiiParam(nMiiEntry, m_pPlayerData->m_nMiiUnk, m_pPlayerData->m_nMiiIndex,
                                            m_pPlayerData->m_MiiSource, m_pPlayerData->m_MiiCreateId);
                            CharSel_CallMiiVoice(m_pPlayerData->m_MiiSource, m_pPlayerData->m_MiiCreateId);
                        } else {
                            Mii_SetMiiParam(nMiiEntry, m_nStarMiiUnk, m_nStarMiiIndex, m_StarMiiSource,
                                            m_StarMiiCreateId);
                            CharSel_CallMiiVoice(m_StarMiiSource, m_StarMiiCreateId);
                        }
                        pCharSel->PushButton(nPad, nBtnKindId);
                        pCharSel->m_Unk64C[nPad] = 1;
                        if (Chara_IsAllDecide(nEntry)) {
                            rPad.m_nFlags06 |= 3;
                            m_aPadSelect[nPad].m_Unk03 = 0x40;
                        }
                    }
                } else if (pCharSel->m_Unk64C[nPad] == 0) {
                    m_Unk27D = nPad;
                    pCharSel->PushButton(nPad, nBtnKindId);
                    SetNextMode(0x11, _GetModeChangeWaitTime());
                }
                break;

            case BTN_BACK:
                if (nPad == nLeader) {
                    CharSel_Back(nPad, nMode, pCharSel);
                    return;
                }
                break;

            case BTN_RANDOM:
            case BTN_RANDOM_2:
                menu::CVibEff::s_Instance.SetEnable(0);
                pCharSel->PushButton(nPad, nBtnKindId);
                _SetRandom(nPad);
                menu::CVibEff::s_Instance.SetEnable(0);
                break;

            case BTN_COLOR:
                menu::CVibEff::s_Instance.SetEnable(0);
                pCharSel->PushButton(nPad, BTN_COLOR);
                _SetColorChange(nPad, nBtnKindId);
                menu::CVibEff::s_Instance.SetEnable(0);
                break;
            }
            continue;
        }

        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
            CharaSelectParam *pLast = Chara_GetLastDecideChara(nEntry);
            if (pLast) {
                // Gives the last character chosen back.
                long nChara = pLast->m_nChara;
                long nCharaBtn = pCharSel->FindBtnKindByChrType(nChara);
                m_nCurrentBtnKindId[nPad][nMode] = nCharaBtn;
                pCharSel->m_nPushButtonId[nPad] = nCharaBtn;
                pCharSel->SetCursorMarkerPosition(nPad, nCharaBtn);
                long nIndex = Chara_CancelDecideCharaWithMotion(nEntry, nChara);
                pCharSel->PushCancelButton(nPad, nCharaBtn);
                rPad.m_nFlags06 &= ~1;
                rPad.m_nFlags06 &= ~2;
                m_aPadSelect[nPad].m_Unk03 = 0;
                if (nChara == CHARA_MII) {
                    pCharSel->m_Unk64C[nPad] = 0;
                    pCharSel->SetPadCharaObjMotion(nPad, 1, 2);
                    _WiFiMiiSetUp(nPad, nChara);
                    m_pPlayerData->SetSelectMii(m_pPlayerData->SearchMiiIndex(pLast->m_MiiCreateId), 0);
                    if ((long)m_Unk9B != GAME_WIFI) {
                        Chara_SetSelectChara(nPad, nChara, 0xFF, 0);
                    }
                    pCharSel->SetCourtCharaObjImplEx(0, 1, 2, nIndex);
                }
                m_fCharSelBackWait = 30.0f;
                nBtnKindId = 0;
            } else {
                if (nPad == nLeader && m_fCharSelBackWait <= 0.0f) {
                    if ((long)m_Unk9B != GAME_WIFI || !m_bStarMii) {
                        CharSel_Back(nPad, nMode, pCharSel);
                        InitCharaSelectParam();
                        pCharSel->SetAllCourtCharaObj();
                    }
                    return;
                }
            }
        } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_PLUS) &&
                   !rPad.IsUsedCursor()) {
            menu::CSndEff::s_Instance.Play(0xBBB, 1);
            _SetColorChange(nPad, nBtnKindId);
            continue;
        } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_MINUS) &&
                   !rPad.IsUsedCursor()) {
            menu::CSndEff::s_Instance.Play(0xBBB, 1);
            _SetRandom(nPad);
            continue;
        }

        // Moves the cursor and shows the character under it.
        InputDataToMenuObj(pCharSel, nPad, nPad, nMode);
        if (m_bCharSelCancel[nPad]) {
            nBtnKindId = 0;
        }
        long nNewBtnKindId = m_nCurrentBtnKindId[nPad][nMode];
        if (nBtnKindId != nNewBtnKindId) {
            m_nCharaColor[nPad] = 1;
            m_bCharSelCancel[nPad] = 0;
            long nChara, nColor;
            if (pCharSel->FindChrTypeByBtnKind(nNewBtnKindId, nChara, nColor) >= 0) {
                if (nChara & 0x80) {
                    Chara_CancelDecideChara(nEntry, 0xFF);
                } else if (Chara_CheckSelectChara(nPad, nChara, nColor) > 0) {
                    if (nChara != CHARA_MII) {
                        Chara_SetSelectChara(nEntry, nChara,
                                             pCharSel->GetChrColor(nNewBtnKindId, m_nCharaColor[nPad]), 0);
                    } else if ((long)m_Unk9B == GAME_WIFI) {
                        if (pCharSel->m_Unk64C[nPad] == 0) {
                            Chara_SetSelectChara(nEntry, nChara, 0, 0);
                            _WiFiMiiSetUp(nPad, nChara);
                        }
                    } else {
                        CharSel_SetMiiChara(nPad);
                    }
                } else if (m_aPadSelect[nPad].m_Unk00 == 0) {
                    // The character was taken: the cursor shows nothing.
                    Chara_CancelDecideChara(nEntry, 0xFF);
                    m_bCharSelCancel[nPad] = 1;
                } else {
                    _MoveCursol(nPad, nEntry, nNewBtnKindId);
                }
            } else {
                Chara_CancelDecideChara(nEntry, 0xFF);
            }
        } else if (nBtnKindId == 0) {
            Chara_CancelDecideChara(nEntry, 0xFF);
        }
    }
}

void msw::CSelectMenu::CharSel_Out()
{
    bcs::G2D::CMenuMdlScnGroup::SetDefaultPersProjInfo();
    Scratch(OBJ_CHAR_SEL);
    Scratch(OBJ_CHARA_OK);
    Scratch(OBJ_CHARA_LEVEL);
    Scratch(OBJ_CHARA_LEVEL_STAR);
    Scratch(OBJ_CHARA_NAME);
    Scratch(OBJ_CHARA_MDL);
    Scratch(OBJ_CHARA_STATUS);
    Scratch(OBJ_UNK_3D);
    for (long i = 0; i < 4; i++) {
        CMenuPadInfo::PAD &rPad = CMenuPadInfo::s_Instance.m_aPad[i];
        if (i == CMenuPadInfo::s_Instance.SelectPadLeader(0)) {
            rPad.m_nFlags06 &= ~1;
            rPad.m_nFlags06 &= ~2;
            m_aPadSelect[i].m_Unk03 = 0;
        } else {
            rPad.m_nFlags06 |= 3;
            m_aPadSelect[i].m_Unk03 = 0x40;
        }
    }
    if (m_nNextMode != 0x11) {
        SetCharaSelectSaveParam();
    }
    CGamePadCheck::GetInstance()->EnableBatteryCheck(0);
}

void msw::CSelectMenu::CharSel_CameraReverse()
{
    if ((long)m_Unk9B == 2) {
        SetWhiteFadeOut(60.0f);
    }
    SendObjEventTrigger(OBJ_TOP, 0x51, 0);

    CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
    long nCamera;
    switch (m_Unk9B) {
    default:
        nCamera = 0x23;
        break;
    case 1:
        nCamera = 0x22;
        break;
    case 2:
        nCamera = 0x23;
        break;
    case GAME_WIFI:
        nCamera = 0x22;
        break;
    }
    rCamera.PushBack(nCamera, -1.0f * GetCameraSpeed(nCamera), 0.0f);

    long nAnim;
    switch (m_Unk9B) {
    case 0:
        nAnim = 0x1C;
        break;
    case 1:
        nAnim = 0x1A;
        break;
    case 2:
        nAnim = 0x1C;
        break;
    case GAME_WIFI:
        nAnim = 0x1A;
        break;
    default:
        nAnim = -1;
        break;
    }
    if (nAnim >= 0) {
        if (nAnim == 0x1A && GetTeamPlayerNum() == 2) {
            nAnim = 0x20;
        }
        SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)nAnim);
        SendObjEventTrigger(OBJ_COURT_CHR_2, 0x36, (void *)nAnim);
    }
}

void msw::CSelectMenu::CharSel_RandomSelect(long nPad)
{
    SelectMenu::CCharSel *pCharSel;
    static const long objId = OBJ_CHAR_SEL;
    long nMode = m_nMode;
    pCharSel = (SelectMenu::CCharSel *)FindObj(objId);
    if (!pCharSel || pCharSel->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    int bMove = 0;
    int bDecide = 0;
    float fRate = bcs::CMainApp::GetAppFrameRateF();
    float fDecideTime = m_nRandomDecideTime[nPad] - fRate;
    if (fDecideTime <= 0.0f) {
        bDecide = 1;
    }
    float fMoveTime = m_fRandomMoveTime[nPad] - fRate;
    if (fMoveTime <= 0.0f) {
        bMove = 1;
    }

    long nEntry = GetPlayerEntryId(nPad);
    if (bMove == 1 || bDecide == 1) {
        long nBtnKindId = pCharSel->RandomMoveCheck();
        long nChara, nColor;
        if (pCharSel->FindChrTypeByBtnKind(nBtnKindId, nChara, nColor) >= 0) {
            nColor = pCharSel->GetChrColor(nBtnKindId, m_nCharaColor[nEntry]);
            if (bDecide) {
                pCharSel->PushButton(nEntry, nBtnKindId);
                Chara_SetDecideCharaWithMotion(nPad, nChara, nColor, nBtnKindId);
            }
            m_nCurrentBtnKindId[nEntry][nMode] = nBtnKindId;
            pCharSel->m_nPushButtonId[nEntry] = nBtnKindId;
            pCharSel->SetCursorMarkerPosition(nEntry, nBtnKindId);
        }
    }

    if (Chara_FindNotDecideCharaIndex(nPad) < 0) {
        m_nRandomDecideTime[nPad] = -1;
        m_fRandomMoveTime[nPad] = -1.0f;
        m_fCharSelVibWait[nEntry] = 60.0f;
        return;
    }

    if (fDecideTime <= 0.0f) {
        fDecideTime = 15.0f;
    }
    m_nRandomDecideTime[nPad] = fDecideTime;
    if (fMoveTime <= 0.0f) {
        fMoveTime = 15.0f;
    }
    m_fRandomMoveTime[nPad] = fMoveTime;
}

void msw::CSelectMenu::CharSel_OkButtonDelay()
{
    menu::CVibEff::s_Instance.SetEnable(1);
    SendObjEventTrigger(OBJ_FADE, 0x31, (void *)0x78);
    m_fEntryErrorWait = 15.0f;
    m_nStep = STEP_OK_INIT;
}

void msw::CSelectMenu::CharSel_OkButtonInit()
{
    static const long padNo0 = 0;
    long nMode = m_nMode;
    if (m_fEntryErrorWait > 0.0f) {
        return;
    }

    static const long objId = OBJ_CHARA_OK;
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(objId);
    pOk->m_bCharaMode = 1;
    Build(OBJ_CHARA_OK);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x01;
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x02;
    SendObjEventTrigger(OBJ_UNK_02, 0x48, 0);
    if (pOk) {
        pOk->SetDefaultMoveCursor(padNo0, m_nCurrentBtnKindId[padNo0][nMode]);
        m_nCurrentBtnKindId[padNo0][nMode] = BTN_OK_YES;
        pOk->m_nPushButtonId[padNo0] = BTN_OK_YES;
        m_nStep = STEP_OK;
    }
    for (long i = 0; i < 4; i++) {
        m_fRandomMoveTime[i] = -1.0f;
        m_nRandomDecideTime[i] = -1;
    }
}

void msw::CSelectMenu::CharSel_OkButton()
{
    static const long objId = OBJ_CHARA_OK;
    long nMode = m_nMode;
    SelectMenu::CCharaOk *pOk = (SelectMenu::CCharaOk *)FindObj(objId);
    if (!pOk || pOk->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    static const long padNo0 = 0;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    if ((long)m_Unk9B == GAME_WIFI && m_bStarMii && dwc::CGameDwc::CountTimer()) {
        dwc::CGameDwc::CloseTimer();
        pOk->PushNext();
        pOk->PushButton(padNo0, BTN_OK_YES);
        CharSel_ChangeStateInit(0x28);
        return;
    }

    int bCancel = 0;
    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
        case BTN_OK_YES:
            dwc::CGameDwc::CloseTimer();
            pOk->PushNext();
            pOk->PushButton(padNo0, nBtnKindId);
            switch (m_Unk9B) {
            case GAME_WIFI:
                if (m_bStarMii == 0) {
                    if (m_nGameKind == -1) {
                        // Any game kind on Wi-Fi: the stage is drawn later.
                        m_nPrevStage = m_nStage;
                        m_nStage = 0xFF;
                        m_nPrevStageSub = m_nStageSub;
                        m_nStageSub = 0xFF;
                        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
                        SetNextMode(0x21, pOk);
                    } else {
                        CharSel_ChangeStateInit(0x20);
                    }
                } else {
                    CharSel_ChangeStateInit(0x28);
                }
                break;
            case 1:
                CharSel_ChangeStateInit(0x15);
                break;
            case 0:
                CharSel_ChangeStateInit(0x10);
                break;
            case 2:
                CharSel_ChangeStateInit(0x2C);
                break;
            }
            return;
        case BTN_OK_NO:
            bCancel = 1;
            break;
        default:
            return;
        }
    }

    if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) || bCancel) {
        pOk->PushCancel();
        pOk->PushButton(padNo0, BTN_OK_NO);
        pOk->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
        static const long objId = OBJ_CHAR_SEL;
        SendObjEventTrigger(OBJ_UNK_02, 0x49,
                            (void *)((SelectMenu::CCharSel *)FindObj(objId))->m_Unk7A4);
        m_nStep = STEP_OK_OUT;
        return;
    }

    InputDataToMenuObj(pOk, padNo0, nPad, nMode);
}

void msw::CSelectMenu::CharSel_LevelOkBtnDelay()
{
    CMenuOption::s_Instance.GetPlayerSaveData(CSave::GetUsePlayerID(), 0);
    menu::CVibEff::s_Instance.SetEnable(1);
    SendObjEventTrigger(OBJ_FADE, 0x31, (void *)0x78);
    m_fEntryErrorWait = 15.0f;
    m_nStep = STEP_LEVEL_INIT;
}

void msw::CSelectMenu::CharSel_LevelOkBtnInit()
{
    static const long padNo0 = 0;
    long nMode = m_nMode;
    Build(OBJ_CHARA_LEVEL);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x01;
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x02;
    SendObjEventTrigger(OBJ_UNK_02, 0x48, 0);
    SelectMenu::CCharaLevel *pLevel = (SelectMenu::CCharaLevel *)FindObj(OBJ_CHARA_LEVEL);
    if (pLevel) {
        pLevel->SetDefaultMoveCursor(padNo0, m_nCurrentBtnKindId[padNo0][nMode]);
        m_nCurrentBtnKindId[padNo0][nMode] = BTN_LEVEL_OK;
        pLevel->m_nPushButtonId[padNo0] = BTN_LEVEL_OK;
        m_nStep = STEP_LEVEL;
    }
    m_fInputWait = 0.0f;
    for (long i = 0; i < 4; i++) {
        m_fRandomMoveTime[i] = -1.0f;
        m_nRandomDecideTime[i] = -1;
    }
}

void msw::CSelectMenu::CharSel_LevelOkBtn()
{
    static const long padNo0 = 0;
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCharaLevel *pLevel = (SelectMenu::CCharaLevel *)FindObj(OBJ_CHARA_LEVEL);
    if (!pLevel || pLevel->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    int bCancel = 0;
    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait <= 0.0f) {
        m_fInputWait = 0.0f;
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_POINTER) {
        switch (nBtnKindId) {
        case BTN_LEVEL_DOWN:
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 20.0f;
                pLevel->SetRuleText(-1);
                pLevel->PushButton(padNo0, nBtnKindId);
            }
            break;
        case BTN_LEVEL_UP:
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 20.0f;
                pLevel->SetRuleText(1);
                pLevel->PushButton(padNo0, nBtnKindId);
            }
            break;
        }
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        switch (nBtnKindId) {
        case BTN_LEVEL_OK:
            pLevel->PushButton(padNo0, nBtnKindId);
            CMenuOption::s_Instance.SetPlayerSaveData(CSave::GetUsePlayerID(), 0, 0);
            switch (m_Unk9B) {
            case 0:
                CharSel_ChangeStateInit(0x10);
                break;
            case 2:
                CharSel_ChangeStateInit(0x2C);
                break;
            }
            return;
        case BTN_LEVEL_CANCEL:
            pLevel->PushButton(padNo0, nBtnKindId);
            bCancel = 1;
            break;
        default:
            return;
        }
    }

    if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) || bCancel) {
        pLevel->PushButton(padNo0, BTN_LEVEL_CANCEL);
        pLevel->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
        static const long objId = OBJ_CHAR_SEL;
        SendObjEventTrigger(OBJ_UNK_02, 0x49,
                            (void *)((SelectMenu::CCharSel *)FindObj(objId))->m_Unk7A4);
        m_nStep = STEP_OK_OUT;
        return;
    }

    InputDataToMenuObj(pLevel, padNo0, nPad, nMode);
}

void msw::CSelectMenu::CharSel_LevelStarOkBtnDelay()
{
    menu::CVibEff::s_Instance.SetEnable(1);
    SendObjEventTrigger(OBJ_FADE, 0x31, (void *)0x78);
    m_fEntryErrorWait = 15.0f;
    m_nStep = STEP_LEVEL_STAR_INIT;
}

void msw::CSelectMenu::CharSel_LevelStarOkBtnInit()
{
    static const long padNo0 = 0;
    long nMode = m_nMode;
    Build(OBJ_CHARA_LEVEL_STAR);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x01;
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags06 &= ~0x02;
    SendObjEventTrigger(OBJ_UNK_02, 0x48, 0);
    SelectMenu::CCharaLevelStar *pLevel = (SelectMenu::CCharaLevelStar *)FindObj(OBJ_CHARA_LEVEL_STAR);
    if (pLevel) {
        pLevel->SetDefaultMoveCursor(padNo0, m_nCurrentBtnKindId[padNo0][nMode]);
        m_nCurrentBtnKindId[padNo0][nMode] = BTN_LEVEL_OK;
        pLevel->m_nPushButtonId[padNo0] = BTN_LEVEL_OK;
        m_nStep = STEP_LEVEL_STAR;
    }
    m_fInputWait = 0.0f;
    for (long i = 0; i < 4; i++) {
        m_fRandomMoveTime[i] = -1.0f;
        m_nRandomDecideTime[i] = -1;
    }
}

void msw::CSelectMenu::CharSel_LevelStarOkBtn()
{
    static const long padNo0 = 0;
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CCharaLevelStar *pLevel = (SelectMenu::CCharaLevelStar *)FindObj(OBJ_CHARA_LEVEL_STAR);
    if (!pLevel || pLevel->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    int bCancel = 0;
    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait <= 0.0f) {
        m_fInputWait = 0.0f;
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_POINTER) {
        switch (nBtnKindId) {
        case BTN_LEVEL_DOWN:
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 20.0f;
                pLevel->SetLevelText(-1);
                pLevel->PushButton(padNo0, nBtnKindId);
            }
            break;
        case BTN_LEVEL_UP:
            if (m_fInputWait <= 0.0f) {
                m_fInputWait = 20.0f;
                pLevel->SetLevelText(1);
                pLevel->PushButton(padNo0, nBtnKindId);
            }
            break;
        }
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        switch (nBtnKindId) {
        case BTN_LEVEL_OK:
            pLevel->PushButton(padNo0, nBtnKindId);
            switch (m_Unk9B) {
            case 0:
                CSave::GetPlayerData(-1)->SetDifficult(m_nGameKind, pLevel->GetLevel());
                CharSel_ChangeStateInit(0x10);
                break;
            case 2:
                CSave::GetPlayerData(-1)->SetDifficult(m_nGameKind, pLevel->GetLevel());
                CharSel_ChangeStateInit(0x2C);
                break;
            }
            return;
        case BTN_LEVEL_CANCEL:
            pLevel->PushButton(padNo0, nBtnKindId);
            bCancel = 1;
            break;
        default:
            return;
        }
    }

    if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_B) || bCancel) {
        pLevel->PushButton(padNo0, BTN_LEVEL_CANCEL);
        pLevel->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
        static const long objId = OBJ_CHAR_SEL;
        SendObjEventTrigger(OBJ_UNK_02, 0x49,
                            (void *)((SelectMenu::CCharSel *)FindObj(objId))->m_Unk7A4);
        m_nStep = STEP_OK_OUT;
        return;
    }

    InputDataToMenuObj(pLevel, padNo0, nPad, nMode);
}

void msw::CSelectMenu::CharSel_ChangeStateInit(long nNextMode)
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_OK);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL_STAR);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
        SendObjEventTrigger(OBJ_FADE, 0x32, 0);
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHAR_SEL);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_STATUS);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
    m_nStage = 0;
    m_nStageSub = 0;
    m_nNextMode = nNextMode;
    m_nStep = STEP_FRAME_OUT;
}

void msw::CSelectMenu::CharSel_FrameOut()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_OK);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL_STAR);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHAR_SEL);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    SetNextMode(m_nNextMode, 0.0f);
}

void msw::CSelectMenu::CharSel_OkButtonOut()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_OK);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }
    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_CHARA_LEVEL_STAR);
    if (pObj && pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_NONE) {
        return;
    }

    Scratch(OBJ_CHARA_OK);
    Scratch(OBJ_CHARA_LEVEL);
    Scratch(OBJ_CHARA_LEVEL_STAR);
    CMenuPadInfo::PAD *pPads = CMenuPadInfo::s_Instance.m_aPad;
    for (long i = 0; i < 4; i++) {
        if (pPads[i].m_nFlags04 & 4) {
            pPads[i].m_nFlags06 &= ~4;
            pPads[i].m_nFlags06 &= ~1;
            pPads[i].m_nFlags06 &= ~2;
            m_aPadSelect[i].m_Unk03 = 0;
        }
    }
    CharSel_CancelLastDecideChara();
    m_nStep = STEP_MAIN;
}

void msw::CSelectMenu::CharSel_CancelLastDecideChara()
{
    long nMode = m_nMode;
    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHAR_SEL);
    for (long i = 0; i < 4; i++) {
        if (!Chara_IsAllDecide(i)) {
            continue;
        }
        CharaSelectParam *pLast = Chara_GetLastDecideChara(i);
        if (!pLast) {
            continue;
        }

        long nChara = pLast->m_nChara;
        long nBtnKindId = pCharSel->FindBtnKindByChrType(nChara);
        m_nCurrentBtnKindId[i][nMode] = nBtnKindId;
        if (pCharSel) {
            pCharSel->m_nPushButtonId[i] = nBtnKindId;
            pCharSel->SetCursorMarkerPosition(i, nBtnKindId);
        }
        long nIndex = Chara_CancelDecideChara(i, nChara);
        if (pCharSel) {
            pCharSel->PushCancelButton(i, nBtnKindId);
        }
        if (nChara == CHARA_MII) {
            pCharSel->m_Unk64C[i] = 0;
            pCharSel->SetPadCharaObjMotion(i, 1, 2);
            _WiFiMiiSetUp(i, nChara);
            m_pPlayerData->SetSelectMii(m_pPlayerData->SearchMiiIndex(pLast->m_MiiCreateId), 0);
            if ((long)m_Unk9B != GAME_WIFI) {
                Chara_SetSelectChara(i, nChara, 0xFF, 0);
            }
            pCharSel->SetCourtCharaObjImplEx(0, 1, 2, nIndex);
        }
    }
}

void msw::CSelectMenu::CharSel_CallMiiVoice(RFLDataSource source, RFLCreateID createId)
{
    SelectMenu::CCharSel *pCharSel = (SelectMenu::CCharSel *)FindObj(OBJ_CHAR_SEL);
    RFLStoreData data;
    RFLAdditionalInfo info;
    bcs::CMiiMan::GetStoreData(data, source, createId);
    bcs::CMiiMan::GetAdditionalInfo(info, data);
    if (info.sex) {
        pCharSel->CallCharVoice(0x14);
    } else {
        pCharSel->CallCharVoice(0x13);
    }
}

// The game kind of character table place nChara; with the fifth (mix)
// sport open (bit 0x08 of the save flags) the table has five places. For
// any other place the original falls off the end and returns whatever is
// in r3 (the low byte of `this`); a port should return a defined value.
unsigned char msw::CSelectMenu::_GetGameModeByBgChrId(unsigned char nChara)
{
    if (m_pPlayerData->m_nFlags5C50 & 8) {
        switch (nChara) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 5;
        case 3:
            return 2;
        case 4:
            return 3;
        }
    } else {
        switch (nChara) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 3;
        }
    }
}

long msw::CSelectMenu::_GetCharaPosId(long nSlot)
{
    return ((SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR))->GetPosChara(nSlot);
}
