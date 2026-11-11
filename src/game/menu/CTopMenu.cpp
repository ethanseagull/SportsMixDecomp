#include "game/msw/SelectMenu/CTopMenu.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CStageManMenu.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/math/types.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations; [1]
    // is not read here. A table, not a scalar, as in game/menu/CTeam.cpp.
    const long s_WinAnimSpeed[2] = { 2, 0 };

    // The camera animation (CCameraMenuBG) and the character table
    // animation (CBgChr::PlayAnim) that bring the chosen sport to the
    // front: when it is in front already, by place, and else by the place
    // in front and the chosen place. "Mix" tables are used once the fifth
    // sport is open (five places instead of four).
    const long s_CamStay[4] = { 10, 11, 12, 13 };
    const long s_CamStayMix[5] = { 10, 11, 21, 12, 13 };
    const long s_CamMove[4][4] = {
        { 10, 42, 43, 44 },
        { 46, 11, 47, 48 },
        { 50, 51, 12, 52 },
        { 54, 55, 56, 13 }
    };
    const long s_CamMoveMix[5][5] = {
        { 10, 42, 45, 43, 44 },
        { 46, 11, 49, 47, 48 },
        { 58, 59, 21, 60, 61 },
        { 50, 51, 53, 12, 52 },
        { 54, 55, 57, 56, 13 }
    };
    const long s_TableStay[4] = { 15, 16, 18, 19 };
    const long s_TableStayMix[5] = { 15, 16, 2, 18, 19 };
    const long s_TableMove[4][4] = {
        { 15, 25, 26, 27 },
        { 29, 16, 30, 31 },
        { 33, 34, 18, 35 },
        { 37, 38, 39, 19 }
    };
    const long s_TableMoveMix[5][5] = {
        { 15, 25, 28, 26, 27 },
        { 29, 16, 32, 30, 31 },
        { 41, 42, 2, 43, 44 },
        { 33, 34, 36, 18, 35 },
        { 37, 38, 40, 39, 19 }
    };

    // The name tag animation of each character table slot (CBgChr's
    // SetTagNameAnim; +4 for the selected tag), with four and five
    // places.
    const long s_TagAnim[4] = { 0, 1, 2, 1 };
    const long s_TagAnimMix[5] = { 0, 1, 2, 2, 1 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_UNK_01 = 1,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_UNK_3F = 0x3F,
        OBJ_UNK_40 = 0x40,
        OBJ_UNK_42 = 0x42,
        OBJ_TOP_MENU = 0x46
    };

    // The button kind ids of the top menu.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_WIFI = 0x8010,   // closed while messaging is restricted
        BTN_KIND_8011 = 0x8011,
        BTN_KIND_8012 = 0x8012,
        BTN_KIND_TABLE = 0x8020,  // the character table
        BTN_KIND_BACK_TO_TABLE = 0x8021,
        BTN_KIND_STICK_NONE = 0x8000
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // Values of CSelectMenu::m_nStep while the top menu is shown.
    enum {
        STEP_DEMO = 0,
        STEP_IN2 = 2,
        STEP_SELECT = 3,
        STEP_CLOSE = 4,
        STEP_WIFI_ERROR = 5,
        STEP_FADE_8011 = 6,
        STEP_FADE_WIFI = 7,
        STEP_FADE_8012 = 8,
        STEP_TURN = 9,
        STEP_TURN_WAIT = 10,
        STEP_UNLOCK = 11,
        STEP_UNLOCK_FADE = 12,
        STEP_UNLOCK_WAIT = 13,
        STEP_UNLOCK_CREATE = 14,
        STEP_UNLOCK_SHOW = 15,
        STEP_STAGE = 16,
        STEP_UNLOCK_END = 17
    };

    // The modes the top menu leads to.
    enum {
        MODE_PLAYER_SELECT = 2,
        MODE_GAME_SELECT = 4,
        MODE_GAME = 5,
        MODE_07 = 7,
        MODE_09 = 9,
        MODE_12 = 0x12,
        MODE_16 = 0x16,
        MODE_1A = 0x1A
    };

    enum {
        TABLE_POS_NUM = 4,
        TABLE_POS_NUM_MIX = 5
    };
}

void msw::CSelectMenu::_SetTagNameAnim(long nSlot, int bSelect)
{
    static const long MixPosCharaTable[TABLE_POS_NUM_MIX] = { 0, 1, 4, 2, 3 };

    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
    long nTag = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(nSlot);
    const long *pAnim = s_TagAnim;
    if (m_pPlayerData->m_nFlags5C50 & 8) {
        pAnim = s_TagAnimMix;
        nTag = MixPosCharaTable[nTag];
    }

    if (bSelect) {
        pBgChr->SetTagNameAnim(nTag, pAnim[nSlot] + 4);
    } else {
        pBgChr->SetTagNameAnim(nTag, pAnim[nSlot]);
    }
}

void msw::CSelectMenu::TopMenu_In()
{
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    m_pPlayerData = CSave::GetPlayerData(-1);
    m_nStage = 0;
    m_nStageSub = 0;
    m_nPlayerNum = 1;
    CGamePadCheck::GetInstance()->m_Unk3C = 1;
    SendObjEventTrigger(OBJ_UNK_3F, 0x32, 0);
    SendObjEventTrigger(OBJ_UNK_42, 0x3A, (void *)-1);
    m_Unk9B = 0;
    m_bStarMii = 0;

    // Only the leader's pad has the flag.
    CMenuPadInfo::s_Instance.m_aPad[0].m_nFlags04 &= ~4;
    CMenuPadInfo::s_Instance.m_aPad[1].m_nFlags04 &= ~4;
    CMenuPadInfo::s_Instance.m_aPad[2].m_nFlags04 &= ~4;
    CMenuPadInfo::s_Instance.m_aPad[3].m_nFlags04 &= ~4;
    CMenuPadInfo::s_Instance.m_aPad[nPad].m_nFlags04 |= 4;

    if (m_Unk27F != 0 && CStageMan::IsValidInstance()) {
        CStageMan::GetInstance()->SceneLeaveAll();
        bcs::BCS_Color black(0, 0, 0, 0);
        bcs::CWorldEnv::Set3DBackColorAll(black);
    }
    m_nStep = STEP_UNLOCK;
}

void msw::CSelectMenu::TopMenu_Loop()
{
    switch (m_nStep) {
        case STEP_DEMO:
            TopMenu_DemoStep();
            break;
        case STEP_IN2:
            TopMenu_In2();
            break;
        case STEP_SELECT:
            TopMenu_LoopStep0();
            break;
        case STEP_CLOSE:
            TopMenu_LoopStep1();
            break;
        case STEP_WIFI_ERROR:
            if (CCockpit::GetResult() == 1000) {
                SendObjEventTrigger(OBJ_UNK_3F, 0x32, 0);
                m_nStep = STEP_SELECT;
            }
            break;
        case STEP_FADE_8011:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                TopMenu_ChangeStateInit(MODE_12, -1);
            }
            break;
        case STEP_FADE_WIFI:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                TopMenu_ChangeStateInit(MODE_16, -1);
            }
            break;
        case STEP_FADE_8012:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                TopMenu_ChangeStateInit(MODE_07, -1);
            }
            break;
        case STEP_TURN:
            TopMenu_LoopStep10();
            break;
        case STEP_TURN_WAIT:
            TopMenu_LoopStep11();
            break;
        case STEP_UNLOCK:
            if (!Unlock::IsDispMenuTop()) {
                m_nStep = STEP_UNLOCK_END;
                break;
            }
            StopMenuBGM();
            SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
            SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
            SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
            SendObjEventTrigger(OBJ_COURT_CHR, 0x3A, (void *)-1);
            SendObjEventTrigger(OBJ_UNK_00, 4, 0);
            SetWhiteFadeOut(60.0f);
            m_aPadSelect[0].m_Unk03 = 0x80;
            m_fInputWait = 60.0f;
            m_nStep = STEP_UNLOCK_FADE;
            break;
        case STEP_UNLOCK_FADE:
            if (!bcs::CMainApp::IsPlayMasterFade()) {
                m_fInputWait -= GetFrameRateF();
                if (m_fInputWait <= 0.0f) {
                    SendObjEventTrigger(OBJ_UNK_00, 4, 0);
                    SendObjEventTrigger(OBJ_UNK_01, 0x29, 0);
                    m_fInputWait = 60.0f;
                    m_nStep = STEP_UNLOCK_WAIT;
                }
            }
            break;
        case STEP_UNLOCK_WAIT:
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait <= 0.0f) {
                m_nStep = STEP_UNLOCK_CREATE;
            }
            break;
        case STEP_UNLOCK_CREATE:
            Unlock::CreateInstance(this);
            m_nStep = STEP_UNLOCK_SHOW;
            if (m_Unk27F != 0 && CStageMan::IsValidInstance()) {
                CStageMan::GetInstance()->Delete();
            }
            break;
        case STEP_UNLOCK_SHOW:
            if (Unlock::IsDispMenuEnd()) {
                if (!Unlock::IsDispMenuTop()) {
                    if (!CStageMan::IsValidInstance()) {
                        CStageMan *pStage = new CStageManMenu(this);
                        pStage->Initialize_();
                    }
                    SendObjEventTrigger(OBJ_UNK_00, 3, 0);
                    SendObjEventTrigger(OBJ_UNK_01, 0x28, 0);
                    SendObjEventTrigger(OBJ_UNK_00, 3, 0);
                    SetFadeIn(60.0f);
                    m_aPadSelect[0].m_Unk03 = 0;
                    m_nStep = STEP_UNLOCK_END;
                } else {
                    m_nStep = STEP_UNLOCK_CREATE;
                }
            }
            break;
        case STEP_UNLOCK_END:
            if (m_Unk27F != 0 && CStageMan::IsValidInstance()) {
                CStageMan::GetInstance()->Delete();
            }
            m_nStep = STEP_STAGE;
            break;
        case STEP_STAGE:
            if (!CStageMan::IsValidInstance()) {
                CStageMan *pStage = new CStageManMenu(this);
                pStage->Initialize_();
            }
            switch (m_Unk9D) {
                case MODE_09:
                    GameMode_SetTopChar(0);
                case MODE_GAME:
                    m_nStep = STEP_IN2;
                    break;
                case MODE_12:
                case MODE_16:
                case MODE_07:
                    CCameraMenuBG::s_Instance.Play(1, 1.0f, 0.0f);
                    SetFadeIn(60.0f);
                    m_nStep = STEP_IN2;
                    break;
                default: {
                    SendObjEventTrigger(OBJ_BG_CHR, 0x33, (void *)1);
                    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
                    SendObjEventTrigger(OBJ_COURT_CHR, 0x3A, (void *)-1);
                    SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
                    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
                    long nPos = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
                    // Camera animation 1 when basketball (place 3) is in
                    // front, else 0.
                    CCameraMenuBG::s_Instance.Play(pBgChr != 0 && nPos == 3, 1.0f, 0.0f);
                    m_fInputWait = 10.0f;
                    pBgChr->TagNameFadeInOut(0);
                    PlayMenuBGM(1);
                    m_nStep = STEP_DEMO;
                    break;
                }
            }
            break;
    }
}

void msw::CSelectMenu::TopMenu_DemoStep()
{
    if (m_fInputWait) {
        m_fInputWait -= GetFrameRateF();
        if (m_fInputWait <= 0.0f) {
            m_fInputWait = 0.0f;
            SetFadeIn(30.0f);
        }
    }
    if (CCameraMenuBG::s_Instance.IsEndAnim() && m_fInputWait == 0.0f) {
        m_nStep = STEP_IN2;
    }
}

void msw::CSelectMenu::TopMenu_In2()
{
    Build(OBJ_TOP_MENU);
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, (void *)1);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x3A, (void *)-1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TOP_MENU);
    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
    // Pad 0 is written as a literal here: the original stores the local
    // static padNo0 after the two arrays below, so it is declared after
    // them (see notes/toolchain.md, CCockpitDodge).
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(0, nBtnKindId);
    }

    pBgChr->TagNameFadeInOut(1);
    long nPos = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
    pBgChr->SetTagNameAnim(nPos, 1);
    m_nTopSelectChara = -1;

    // The camera animation that shows each place of the character table.
    long camAnim[TABLE_POS_NUM] = { 4, 7, 8, 9 };
    long camAnimMix[TABLE_POS_NUM_MIX] = { 4, 7, 18, 8, 9 };
    static const long padNo0 = 0;
    CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
    long nAnim;
    if (m_pPlayerData->m_nFlags5C50 & 8) {
        nAnim = camAnimMix[nPos];
    } else {
        nAnim = camAnim[nPos];
    }

    // Both branches are the same in the game.
    switch (m_Unk9D) {
        case MODE_12:
        case MODE_1A:
        case MODE_07:
        case MODE_16:
            rCamera.Play(nAnim, GetCameraSpeed(nAnim), 0.0f);
            break;
        default:
            rCamera.Play(nAnim, GetCameraSpeed(nAnim), 0.0f);
            break;
    }

    pBgChr->SetSelectGameChara();
    TopMenu_SelectAnim(static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0));
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::TopMenu_LoopStep0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!bcs::CSnd::IsReady()) {
        return;
    }
    SelectMenu::CTopMenu *pObj = static_cast<SelectMenu::CTopMenu *>(FindObj(OBJ_TOP_MENU));
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }
    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
    if (!pBgChr->IsRotAnim()) {
        return;
    }

    long nChara;
    if (m_aPadSelect[nPad].m_Unk00 == 1) {
        nChara = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
    } else {
        nChara = pObj->m_nHitChara;
    }

    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    long nBtnKindId = rBtnKindId;

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_TABLE:
                if ((unsigned long)nChara <= 4) {
                    TopMenu_ChangeStateInit(MODE_GAME, 0);
                    rBtnKindId = nBtnKindId;
                }
                break;
            case BTN_KIND_8011:
                pObj->PushButton(padNo0, nBtnKindId);
                SetWhiteFadeOut(10.0f);
                m_nStep = STEP_FADE_8011;
                break;
            case BTN_KIND_WIFI:
                m_pPlayerData = CSave::GetPlayerData(-1);
                if (bcs::CMainApp::CheckPCMessageRestriction()) {
                    pObj->m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_WIFI)->InitButtonAnim();
                    WindowA_Button("WiFiErrorMessage", "WiFi_21", 0, 0);
                    m_nStep = STEP_WIFI_ERROR;
                } else {
                    pObj->PushButton(padNo0, nBtnKindId);
                    SetWhiteFadeOut(10.0f);
                    m_nStep = STEP_FADE_WIFI;
                }
                break;
            case BTN_KIND_8012:
                pObj->PushButton(padNo0, nBtnKindId);
                SetWhiteFadeOut(10.0f);
                m_nStep = STEP_FADE_8012;
                break;
            case BTN_KIND_BACK:
                TopMenu_BackPlaterSelect();
                break;
            case BTN_KIND_NONE:
                break;
            default:
                SetNextMode(MODE_GAME_SELECT, pObj);
                break;
        }
        return;
    }

    if (nBtnKindId == BTN_KIND_TABLE && (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT)) {
        pBgChr->RotPosChara(1);
        long nPos = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
        Top_SetTurnCamera(nPos, 0, _IsOpenMixSportsFlag());
    } else if (nBtnKindId == BTN_KIND_TABLE && (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT)) {
        pBgChr->RotPosChara(-1);
        long nPos = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
        Top_SetTurnCamera(nPos, 1, _IsOpenMixSportsFlag());
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        TopMenu_BackPlaterSelect();
        return;
    }
    InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    TopMenu_SelectAnim(nChara);
}

void msw::CSelectMenu::TopMenu_SelectAnim(long nChara)
{
    // The game stores `charaIdTable` before `padNo0`: static-order swap
    // (see notes/decisions.md).
    static const long charaIdTable[TABLE_POS_NUM_MIX] = { 0, 1, 4, 2, 3 };
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));

    long nNum = TABLE_POS_NUM;
    long nGame = nChara;
    if (m_pPlayerData->m_nFlags5C50 & 8) {
        nNum = TABLE_POS_NUM_MIX;
        nGame = charaIdTable[nChara];
    }

    long nBtnKindId = m_nCurrentBtnKindId[nPad][nMode];
    if (nBtnKindId != BTN_KIND_TABLE) {
        for (long i = 0; i < nNum; i++) {
            _SetTagNameAnim(i, 0);
        }
        pBgChr->SetSelectGameChara(-1);
        m_nTopSelectChara = -1;
        return;
    }

    if (m_nTopSelectChara != nChara) {
        pBgChr->SetSelectGameChara(-1);
        long nPos = pBgChr->GetCharaPos(nChara);
        for (long i = 0; i < nNum; i++) {
            if (i == nPos) {
                _SetTagNameAnim(i, 1);
                pBgChr->SetSelectGameTag(nGame);
                if (m_Unk23E == 0) {
                    menu::CSndEff::s_Instance.Play(0xBC0, 1);
                    menu::CVibEff::s_Instance.Play(1, 1, 1);
                }
            } else {
                _SetTagNameAnim(i, 0);
            }
        }
        m_nTopSelectChara = nChara;
    }
}

void msw::CSelectMenu::TopMenu_ChangeStateInit(long nMode, long)
{
    static const long padNo0 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TOP_MENU);
    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
    SelectMenu::CTopMenu *pTopMenu = static_cast<SelectMenu::CTopMenu *>(FindObj(OBJ_TOP_MENU));

    m_nNextMode = nMode;
    SendObjEventTrigger(OBJ_UNK_01, 0x51, 0);
    pBgChr->TagNameFadeInOut(0);
    pBgChr->SetSelectGameTag(-1);

    switch (nMode) {
        case MODE_GAME: {
            m_Unk9B = 0;
            m_nNextMode = MODE_GAME;
            long nFront = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
            long nChara;
            if (m_Unk23E == 1) {
                nChara = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR))->GetPosChara(0);
            } else {
                nChara = pTopMenu->m_nHitChara;
            }
            m_nGameKind = _GetGameModeByBgChrId(nChara);
            CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;

            if (nChara != nFront) {
                if (m_pPlayerData->m_nFlags5C50 & 8) {
                    rCamera.Play(s_CamMoveMix[nFront][nChara], 1.0f, 0.0f);
                    pBgChr->PlayAnim(s_TableMoveMix[nFront][nChara], 1.0f, 0.0f);
                } else {
                    rCamera.Play(s_CamMove[nFront][nChara], 1.0f, 0.0f);
                    pBgChr->PlayAnim(s_TableMove[nFront][nChara], 1.0f, 0.0f);
                }
            } else if (m_pPlayerData->m_nFlags5C50 & 8) {
                rCamera.Play(s_CamStayMix[nChara], 1.0f, 0.0f);
                pBgChr->PlayAnim(s_TableStayMix[nChara], 1.0f, 0.0f);
            } else {
                rCamera.Play(s_CamStay[nChara], 1.0f, 0.0f);
                pBgChr->PlayAnim(s_TableStay[nChara], 1.0f, 0.0f);
            }
            pBgChr->SetPosChara(nChara);
            PlayMenuBGM(1);
            m_nStep = STEP_TURN;
            break;
        }
        case MODE_12:
            m_Unk9B = 2;
            InitCharaSelectParam();
            CCameraMenuBG::s_Instance.Play(0x29, 1.0f, 0.0f);
            SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
            SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
            PlayMenuBGM(2);
            m_nStep = STEP_CLOSE;
            break;
        case MODE_16:
            m_Unk9B = 4;
            CCameraMenuBG::s_Instance.Play(0x29, 1.0f, 0.0f);
            SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
            SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
            PlayMenuBGM(4);
            m_nStep = STEP_CLOSE;
            break;
        case MODE_07:
            CCameraMenuBG::s_Instance.Play(0x29, 1.0f, 0.0f);
            SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
            SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
            PlayMenuBGM(5);
            m_nStep = STEP_CLOSE;
            break;
        default:
            GoBackToMode(padNo0, nMode, pObj, 1);
            m_nStep = STEP_CLOSE;
            break;
    }

    if (pObj != 0) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
}

void msw::CSelectMenu::TopMenu_LoopStep1()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TOP_MENU);
    if (pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
        // Both branches are the same in the game.
        if (m_nNextMode == MODE_12 || m_nNextMode == MODE_16) {
            SetNextMode(m_nNextMode, 0.0f);
        } else {
            SetNextMode(m_nNextMode, 0.0f);
        }
    }
}

void msw::CSelectMenu::TopMenu_LoopStep10()
{
    SelectMenu::CBgChr *pBgChr = static_cast<SelectMenu::CBgChr *>(FindObj(OBJ_BG_CHR));
    if (pBgChr != 0) {
        pBgChr->SetSelectGameChara();
    }
    m_nStep = STEP_TURN_WAIT;
}

void msw::CSelectMenu::TopMenu_LoopStep11()
{
    if (CCameraMenuBG::s_Instance.IsEndAnim()) {
        m_nStep = STEP_CLOSE;
    }
}

void msw::CSelectMenu::TopMenu_Out()
{
    Scratch(OBJ_TOP_MENU);
}

void msw::CSelectMenu::Top_SetTurnCamera(long nPos, int bReverse, int bMix)
{
    CCameraMenuBG &rCamera = CCameraMenuBG::s_Instance;
    long nAnim0;
    long nAnim1;
    float fRate0;
    float fRate1;

    if (bReverse) {
        if (bMix) {
            long anim0[TABLE_POS_NUM_MIX] = { 5, 2, 19, 20, 4 };
            long anim1[TABLE_POS_NUM_MIX] = { 9, 6, 7, 18, 8 };
            nAnim0 = anim0[nPos];
            nAnim1 = anim1[nPos];
        } else {
            long anim0[TABLE_POS_NUM] = { 5, 2, 3, 4 };
            long anim1[TABLE_POS_NUM] = { 9, 6, 7, 8 };
            nAnim0 = anim0[nPos];
            nAnim1 = anim1[nPos];
        }
        fRate0 = -1.0f;
        fRate1 = -1.0f;
    } else {
        if (bMix) {
            long anim0[TABLE_POS_NUM_MIX] = { 2, 19, 20, 4, 5 };
            long anim1[TABLE_POS_NUM_MIX] = { 7, 18, 8, 9, 6 };
            nAnim0 = anim0[nPos];
            nAnim1 = anim1[nPos];
        } else {
            long anim0[TABLE_POS_NUM] = { 2, 3, 4, 5 };
            long anim1[TABLE_POS_NUM] = { 7, 8, 9, 6 };
            nAnim0 = anim0[nPos];
            nAnim1 = anim1[nPos];
        }
        fRate0 = 1.0f;
        fRate1 = 1.0f;
    }

    float fSpeed0 = fRate0 * GetCameraSpeed(nAnim0);
    float fSpeed1 = fRate1 * GetCameraSpeed(nAnim1);
    rCamera.PushBack(nAnim0, fSpeed0, 0.0f);
    rCamera.PushBack(nAnim1, fSpeed1, 0.0f);
}

void msw::CSelectMenu::TopMenu_BackPlaterSelect()
{
    static const long padNo0 = 0;

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TOP_MENU);
    ClearMenuHistory();
    PushMenuHistory(1, 2, 0);
    GoBackToMode(padNo0, MODE_PLAYER_SELECT, pObj, 1);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    enum {
        PANE_ROOT = 0,
        PANE_TEXT_0 = 8,
        PANE_TEXT_1 = 9,
        PANE_TEXT_2 = 10
    };

    enum {
        ANIM_IN = 0,
        ANIM_WIN_IN = 6,
        ANIM_WIN_LOOP = 7,
        ANIM_WIN_OUT = 8
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21, // plus the pad number
        EVENT_CLOSE = 0x26
    };

    const char *const s_LayoutName = "me_top_new.brlyt";
    const char *const s_ButtonFileName = "me_top_new.btn";
}

msw::SelectMenu::CTopMenu::CTopMenu(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CTopMenu::~CTopMenu()
{
    Remove();
}

void msw::SelectMenu::CTopMenu::Disp()
{
}

void msw::SelectMenu::CTopMenu::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_IN)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_LOOP, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            // The three buttons come forward and play their highlight
            // while the cursor is on them.
            for (long i = 0; i < BTN_ANIM_NUM; i++) {
                long nBtnKindId = BTN_KIND_WIFI + i;
                long nPane;
                long nAnim;
                switch (nBtnKindId) {
                    case BTN_KIND_WIFI:
                        nPane = 2;
                        nAnim = 12;
                        break;
                    case BTN_KIND_8011:
                        nPane = 4;
                        nAnim = 10;
                        break;
                    case BTN_KIND_8012:
                        nPane = 6;
                        nAnim = 11;
                        break;
                    default:
                        nPane = -1;
                        nAnim = -1;
                        break;
                }
                if (m_bBtnAnim[i] == 0 && nBtnKindId == m_nCursorBtnKindId) {
                    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPaneObj(nPane)->GetPane();
                    nw4r::math::VEC3 pos = pPane->GetTranslate();
                    pos.z = 5.0f;
                    pPane->SetTranslate(pos);
                    m_pPaneAccessor->PlayAnimation(nPane, nAnim, 1.0f, 0.0f, 1, 0);
                    m_bBtnAnim[i] = 1;
                } else if (m_bBtnAnim[i] == 1 && nBtnKindId != m_nCursorBtnKindId) {
                    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPaneObj(nPane)->GetPane();
                    nw4r::math::VEC3 pos = pPane->GetTranslate();
                    pos.z = 0.0f;
                    pPane->SetTranslate(pos);
                    m_pPaneAccessor->StopAnimation(nPane, nAnim, 1);
                    m_bBtnAnim[i] = 0;
                }
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd() && GetSelectMenu()->SendObjEventTrigger(1, EVENT_CLOSE, 0)) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_WIN_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }
}

int msw::SelectMenu::CTopMenu::Set()
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);
            pPaneCtrl->SetPaneZSort(1);

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
        }
    }

    m_Unk2C = 1;
    m_Unk30 = STATE_SET;
    m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_IN, 1.0f, 0.0f, 1, 1);
    m_nStickBtnKindId = BTN_KIND_WIFI;
    m_Unk5A4 = -1;
    for (long i = 0; i < BTN_ANIM_NUM; i++) {
        m_bBtnAnim[i] = 0;
    }

    const wchar_t *pText = GetMessageLabelString("TopMenuMessage", "TOP_01");
    m_pPaneAccessor->SetTextBoxString(PANE_TEXT_0, pText, wcslen(pText));
    pText = GetMessageLabelString("TopMenuMessage", "TOP_02");
    m_pPaneAccessor->SetTextBoxString(PANE_TEXT_1, pText, wcslen(pText));
    pText = GetMessageLabelString("TopMenuMessage", "TOP_04");
    m_pPaneAccessor->SetTextBoxString(PANE_TEXT_2, pText, wcslen(pText));
    return 0;
}

void msw::SelectMenu::CTopMenu::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CTopMenu::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    SetCursolHit(-1);

    // The sports on the character table: the character and its name tag.
    CSelectMenu *pSelectMenu = GetSelectMenu();
    for (long i = 0; i < TABLE_POS_NUM_MIX; i++) {
        nw4r::math::AABB chara;
        nw4r::math::AABB tag;
        static_cast<CBgChr *>(pSelectMenu->FindObj(OBJ_BG_CHR))->GetBBoxInfo(i, chara, tag);
        if (pPos->x >= chara.min.x && pPos->x <= chara.max.x && pPos->y >= chara.max.y && pPos->y <= chara.min.y) {
            nResult = BTN_KIND_TABLE;
            SetCursolHit(i);
            break;
        }
        if (pPos->x >= tag.min.x && pPos->x <= tag.max.x && pPos->y >= tag.max.y && pPos->y <= tag.min.y) {
            nResult = BTN_KIND_TABLE;
            SetCursolHit(i);
            break;
        }
    }

    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(1, EVENT_HIT, pPos);
    }
    if (CMenuPad::s_Instance.IsInputPointerMode(CMenuPadInfo::s_Instance.GetPadLeader())) {
        m_nCursorBtnKindId = nResult;
    }
    return nResult;
}

long msw::SelectMenu::CTopMenu::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTopMenu::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTopMenu::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CTopMenu::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CTopMenu::StickMoveCheck(long nPad, long nBtnKindId)
{
    // nBtnKindId becomes the button to come back to from the table.
    long nCurBtnKindId = nBtnKindId;
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_TABLE;
    }

    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nCurBtnKindId, nInput);
    if (nNext == BTN_KIND_TABLE) {
        if (nBtnKindId != BTN_KIND_TABLE) {
            // Leaving a button for the table: remember where to come back.
            m_nStickBtnKindId = nBtnKindId;
            m_Unk5A4 = -1;
        } else {
            nNext = (unsigned short)m_pMenuPartList->StickMovementJudgmentCursor(nPad, nNext, nInput);
            if (nNext == BTN_KIND_STICK_NONE) {
                nNext = BTN_KIND_TABLE;
            }
        }
        if (nNext == BTN_KIND_BACK_TO_TABLE) {
            nNext = m_nStickBtnKindId;
        }
    }
    m_nCursorBtnKindId = nNext;
    return nNext;
}

long msw::SelectMenu::CTopMenu::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_TABLE;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CTopMenu::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CTopMenu::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into HitCheck() although defined after it. Defined down here
// because its local static follows Main()'s constants in .rodata.
void msw::SelectMenu::CTopMenu::SetCursolHit(long nChara)
{
    static const long charaIdTable[TABLE_POS_NUM_MIX] = { 0, 1, 3, 4, 2 };

    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    m_nHitChara = nChara;
    if (nChara >= 0 && (pPlayerData->m_nFlags5C50 & 8)) {
        m_nHitChara = charaIdTable[nChara];
    }
}

// Inlined into Main(), as SetCursolHit().
void msw::SelectMenu::CTopMenu::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}
