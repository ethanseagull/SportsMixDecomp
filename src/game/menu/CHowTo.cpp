#include "game/msw/SelectMenu/CHowTo.hpp"

#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations; [1]
    // is not read here. A table, not a scalar: the original converts the
    // value to float at run time and keeps both words, first in .rodata
    // (a scalar const is folded to 2.0f and not emitted).
    const long s_WinAnimSpeed[2] = { 2, 0 };

    // The camera position for each place a background character can stand
    // in; the second table when bit 0x08 of the player data is set.
    const long s_CamPos[4] = { 10, 11, 12, 13 };
    const long s_CamPosAlt[5] = { 10, 11, 21, 12, 13 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_HOWTO = 10,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_3B = 0x3B,
        OBJ_UNK_42 = 0x42
    };

    // The button kind ids of the screen: the three tutorials, and the
    // back button.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_TUTORIAL_0 = 0x1F,
        BTN_KIND_TUTORIAL_1 = 0x20,
        BTN_KIND_TUTORIAL_2 = 0x21
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_MAIN = 0,
        STEP_1 = 1,
        STEP_2 = 2,
        STEP_100 = 100,
        STEP_101 = 101,
        STEP_NEXT_MODE = 102
    };
}

void msw::CSelectMenu::HowTo_In()
{
    const long *pCamPos;
    static const long padNo0 = 0;

    m_pPlayerData = CSave::GetPlayerData(-1);

    SelectMenu::CBgChr *pBgChr = (SelectMenu::CBgChr *)FindObj(OBJ_BG_CHR);
    if (m_Unk9D != 4 && pBgChr != 0) {
        GameMode_SetTopChar(1);
        SetFadeIn(60.0f);
    }

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_HOWTO);
    Build(OBJ_HOWTO);
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
        if (m_Unk9D != 4) {
            CCameraMenuBG::s_Instance.PushBackWithoutReady(pCamPos[pBgChr->GetPosChara(padNo0)], 1.0f, 5000.0f);
        }
    }
    pBgChr->SetCharaSpeed(1.0f);
    m_nStep = STEP_MAIN;
}

void msw::CSelectMenu::HowTo_Loop()
{
    switch (m_nStep) {
        default:
            HowTo_LoopStep0();
            break;
        case STEP_1:
            GameMode_LoopStep1();
            break;
        case STEP_2:
            GameMode_LoopStep2();
            break;
        case STEP_100:
            GameMode_LoopStep100();
            break;
        case STEP_101:
            GameMode_LoopStep101();
            break;
        case STEP_NEXT_MODE:
            SetNextMode(m_nNextMode, 0.0f);
            break;
    }
}

void msw::CSelectMenu::HowTo_LoopStep0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    // The result is not used.
    FindObj(OBJ_BG_CHR);

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_HOWTO);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_TUTORIAL_0:
                pObj->PushButton(padNo0, nBtnKindId);
                SendObjEventTrigger(OBJ_HOWTO, 0x2E, 0);
                GameMode_ChangeStateInit(0x2D);
                break;
            case BTN_KIND_TUTORIAL_1:
                if (CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 1;
                }
                CTournament::Tutorial(m_nGameKind, CMenuPad::s_Instance.GetPadStyle(0), 0);
                SetPadDefault();
                m_Unk9B = 1;
                m_UnkA00 = 1;
                SendObjEventTrigger(OBJ_HOWTO, 0x2E, 0);
                GameMode_ChangeStateInit(0x31);
                break;
            case BTN_KIND_TUTORIAL_2:
                if (CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 1;
                }
                CTournament::Tutorial(m_nGameKind, CMenuPad::s_Instance.GetPadStyle(0), 1);
                SetPadDefault();
                m_Unk9B = 1;
                m_UnkA00 = 1;
                SendObjEventTrigger(OBJ_HOWTO, 0x2E, 0);
                GameMode_ChangeStateInit(0x31);
                break;
            case BTN_KIND_BACK:
                SendObjEventTrigger(OBJ_HOWTO, 0x2E, 0);
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        pObj->PushButton(padNo0, BTN_KIND_BACK);
        SendObjEventTrigger(OBJ_HOWTO, 0x2E, 0);
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        GameMode_SetLineMessage(m_nCurrentBtnKindId[padNo0][nMode]);
    }
}

void msw::CSelectMenu::HowTo_Out()
{
    Scratch(OBJ_HOWTO);
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
        PANE_T_RUBI_00,
        PANE_T_RUBI_01,
        PANE_T_RUBI_02,
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

    // What this screen answers in SendEvent.
    enum {
        EVENT_CLOSE = 0x2E,
        EVENT_GET_STATE = 0x30
    };

    const char *s_LayoutName = "me_multiplay_02.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "me_multiplay_02_btn_00.brlan",
        "me_multiplay_02_btn_01.brlan",
        "me_multiplay_02_btn_02.brlan",
        "me_multiplay_02_btn_03.brlan",
        "me_multiplay_02_btn_04.brlan",
        "me_multiplay_02_btn_color_00.brlan",
        "me_multiplay_02_btn_color_01.brlan",
        "me_multiplay_02_btn_color_02.brlan",
        "me_multiplay_02_btn_color_03.brlan",
        "me_multiplay_02_btn_vis_00.brlan",
        "me_multiplay_02_btn_vis_01.brlan",
        "me_multiplay_02_btn_vis_02.brlan",
        "me_multiplay_02_btn_vis_03.brlan",
        "me_multiplay_02_btn_vis_04.brlan",
        "me_multiplay_02_multi_w00.brlan",
        "me_multiplay_02_multi_w01.brlan",
        "me_multiplay_02_multi_w02.brlan",
        "me_multiplay_02_multi_w03.brlan",
        "me_multiplay_02_multi_w04.brlan",
        "me_multiplay_02_multi_w05.brlan",
        "me_multiplay_02_multi_w06.brlan"
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
        "b_mp_btn_11",
        "t_rubi_00",
        "t_rubi_01",
        "t_rubi_02"
    };

    // Size 0x1C. A button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nTextPane;          // 0x02
        unsigned short m_nNextBtnKindId[5]; // 0x04
        const char *m_pLabel;               // 0x10, in message group "Text_Rule_Common"
        unsigned short m_nRubyPane;         // 0x14, 0 for none
        const char *m_pRubyLabel;           // 0x18
    };

    ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { 3, PANE_MP_BTN_09, PANE_T_MP_BTN_09, { 0x20, 0x20, 0x21, 0, 0 },
          "Rule_Common_Menu_00_00", PANE_T_RUBI_00, "Rule_Common_Menu_00_00_Ruby" },
        { 4, PANE_MP_BTN_10, PANE_T_MP_BTN_10, { 0x21, 0x20, 0x1F, 0, 0 },
          "Rule_Common_Menu_00_01", PANE_T_RUBI_01, "Rule_Common_Menu_00_01_Ruby" },
        { 5, PANE_MP_BTN_11, PANE_T_MP_BTN_11, { 0x1F, 0x21, 0x1F, 0, 0 },
          "Rule_Common_Menu_00_02", 0, 0 }
    };

    // The kind id the cursor falls back to.
    long s_DefaultBtnKindId = BTN_KIND_TUTORIAL_1;
}

msw::SelectMenu::CHowTo::CHowTo(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CHowTo::~CHowTo()
{
    Remove();
}

void msw::SelectMenu::CHowTo::Main()
{
    StateControl();

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CHowTo::Disp()
{
}

void msw::SelectMenu::CHowTo::StateControl()
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

int msw::SelectMenu::CHowTo::Set()
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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
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

                const ButtonParam *pParam = &s_ButtonParam[i];

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

                const wchar_t *pText = GetMessageLabelString("Text_Rule_Common", pParam->m_pLabel);
                m_pPaneAccessor->SetTextBoxString(pParam->m_nTextPane, pText, wcslen(pText));
                if (pParam->m_nRubyPane != 0) {
                    pText = GetMessageLabelString("Text_Rule_Common", pParam->m_pRubyLabel);
                    m_pPaneAccessor->SetTextBoxString(pParam->m_nRubyPane, pText, wcslen(pText));
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

void msw::SelectMenu::CHowTo::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CHowTo::SendEvent(long nEvent, void *pParam)
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

long msw::SelectMenu::CHowTo::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CHowTo::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CHowTo::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CHowTo::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CHowTo::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CHowTo::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CHowTo::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = s_DefaultBtnKindId;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CHowTo::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CHowTo::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into StateControl() although defined after it; the out-of-line
// copy is not called and the link drops it. Defined down here because its
// local static follows Set()'s constants in .rodata (declared `inline`, the
// static is not emitted at all).
void msw::SelectMenu::CHowTo::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Plays the opening animation of the buttons and the colour of the game
// kind. Inlined into Set().
inline void msw::SelectMenu::CHowTo::PaneColorInit()
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
