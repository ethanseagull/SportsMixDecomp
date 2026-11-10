#include "game/msw/SelectMenu/CPlayMode.hpp"

#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CChrCaption.hpp"
#include "game/msw/SelectMenu/CCourtCharaObj.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar, as in game/menu/CHowTo.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_TASK = 1,
        OBJ_PLAYMODE = 0xD,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_CHR_CAPTION = 0x3C
    };

    // The button kind ids of the screen: the back button and up to four
    // play modes.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_MODE_0 = 0x32,
        BTN_KIND_MODE_1 = 0x33,
        BTN_KIND_MODE_2 = 0x34,
        BTN_KIND_MODE_3 = 0x35
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_CAMERA = 0,
        STEP_MAIN = 1,
        STEP_LEAVE = 2
    };

    // The modes the screen leads to.
    enum {
        MODE_NEXT_D = 0xD,
        MODE_NEXT_E = 0xE
    };

    // Camera animation of the screen.
    enum {
        CAM_PLAYMODE = 0x21
    };

    // The index (0 to 3) of a number of players (1 to 4).
    inline long GetPlayerIdx(long nPlayerNum)
    {
        return nPlayerNum == 1 ? 0 : nPlayerNum == 2 ? 1 : nPlayerNum == 3 ? 2 : 3;
    }

    // The mode play mode nPlayMode leads to: 0xD or 0xE.
    inline long GetNextMode(unsigned char nPlayMode)
    {
        switch (nPlayMode) {
            case 7:
            case 8:
            case 14:
            case 15:
                return MODE_NEXT_D;
        }
        return MODE_NEXT_E;
    }
}

void msw::CSelectMenu::PlayMode_In()
{
    static const long padNo0 = 0;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    Build(OBJ_CHR_CAPTION);
    Build(OBJ_PLAYMODE);
    SendObjEventTrigger(OBJ_TOP, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "VS_03"));
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYMODE);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        rBtnKindId = nBtnKindId;
        pObj->SetCursorMarkerVisible(nPad, 1);
    }

    if (SendObjEventTrigger(OBJ_TASK, 0x2C, 0) != 0) {
        m_nStep = STEP_MAIN;
    } else {
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::PlayMode_Loop()
{
    switch (m_nStep) {
        case STEP_CAMERA:
            CCameraMenuBG::s_Instance.PushBackWithoutReady(CAM_PLAYMODE, GetCameraSpeed(CAM_PLAYMODE), 0.0f);
            m_nStep = STEP_MAIN;
            break;
        case STEP_MAIN:
            PlayMode_Loop0();
            break;
        case STEP_LEAVE:
            PlayMode_Loop1();
            break;
    }
}

void msw::CSelectMenu::PlayMode_Loop0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYMODE);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        switch (nBtnKindId) {
            case BTN_KIND_MODE_0:
            case BTN_KIND_MODE_1:
            case BTN_KIND_MODE_2:
            case BTN_KIND_MODE_3: {
                pObj->PushButton(padNo0, nBtnKindId);
                long nPlayMode = _GetPlayModeFromButtonId(nBtnKindId, m_nPlayerNum);
                if (nPlayMode < 0) {
                    break;
                }
                m_nPlayMode = nPlayMode;
                PlayMode_ChangeStateInit(GetNextMode(m_nPlayMode));
                break;
            }
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                CCameraMenuBG::s_Instance.PushBack(CAM_PLAYMODE, -1.0f * GetCameraSpeed(CAM_PLAYMODE), 0.0f);
                PlayMode_ChangeStateInit(m_nNextMode);
                SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        CCameraMenuBG::s_Instance.PushBack(CAM_PLAYMODE, -1.0f * GetCameraSpeed(CAM_PLAYMODE), 0.0f);
        PlayMode_ChangeStateInit(m_nNextMode);
        SendObjEventTrigger(OBJ_COURT_CHR, 0x35, 0);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::PlayMode_ChangeStateInit(long nMode)
{
    switch (nMode) {
        case MODE_NEXT_D:
            m_Unk26C = 0;
            break;
        case MODE_NEXT_E:
            m_pPlayerData->ClearSelectMii();
            break;
    }

    SendObjEventTrigger(OBJ_TASK, 0x51, 0);

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYMODE);
    if (pObj != 0) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }

    InitCharaSelectParam();
    m_nNextMode = nMode;
    m_nStep = STEP_LEAVE;
}

void msw::CSelectMenu::PlayMode_Loop1()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYMODE);
    if (pObj == 0 || pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
        SetNextMode(m_nNextMode, 0.0f);
    }
}

void msw::CSelectMenu::PlayMode_Out()
{
    Scratch(OBJ_PLAYMODE);
    Scratch(OBJ_CHR_CAPTION);
}

// The names in this namespace are guesses; the objects have no symbols.
// They are defined here, after the CSelectMenu steps, because their strings
// follow those steps' constants in .rodata.
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
        ANIM_BTN_COLOR_04,
        ANIM_BTN_COLOR_05,
        ANIM_BTN_VIS_00,   // one per number of buttons
        ANIM_BTN_VIS_01,
        ANIM_BTN_VIS_02,
        ANIM_BTN_VIS_03,
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
        PANE_MP_BTN_02,
        PANE_MP_BTN_03,
        PANE_MP_BTN_04,
        PANE_MP_BTN_05,
        PANE_MP_BTN_06,
        PANE_MP_BTN_07,
        PANE_MP_BTN_08,
        PANE_T_MP_BTN_00,
        PANE_T_MP_BTN_01,
        PANE_T_MP_BTN_02,
        PANE_T_MP_BTN_03,
        PANE_T_MP_BTN_04,
        PANE_T_MP_BTN_05,
        PANE_T_MP_BTN_06,
        PANE_T_MP_BTN_07,
        PANE_T_MP_BTN_08,
        PANE_B_MP_BTN_00,
        PANE_B_MP_BTN_01,
        PANE_B_MP_BTN_02,
        PANE_B_MP_BTN_03,
        PANE_B_MP_BTN_04,
        PANE_B_MP_BTN_05,
        PANE_B_MP_BTN_06,
        PANE_B_MP_BTN_07,
        PANE_B_MP_BTN_08,
        PANE_T_3,
        PANE_T_4,
        PANE_T_5,
        PANE_T_6,
        PANE_NUM
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21, // plus the pad number
        EVENT_CLOSE_CHECK = 0x26
    };

    enum {
        PLAYER_IDX_NUM = 4,
        COURT_SET_NUM = 4
    };

    // The layout, and a null entry after it.
    const char *s_LayoutName[2] = { "me_multiplay_00.brlyt", 0 };

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
        "me_multiplay_00_btn_color_04.brlan",
        "me_multiplay_00_btn_color_05.brlan",
        "me_multiplay_00_btn_vis_00.brlan",
        "me_multiplay_00_btn_vis_01.brlan",
        "me_multiplay_00_btn_vis_02.brlan",
        "me_multiplay_00_btn_vis_03.brlan",
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
        "mp_btn_02",
        "mp_btn_03",
        "mp_btn_04",
        "mp_btn_05",
        "mp_btn_06",
        "mp_btn_07",
        "mp_btn_08",
        "t_mp_btn_00",
        "t_mp_btn_01",
        "t_mp_btn_02",
        "t_mp_btn_03",
        "t_mp_btn_04",
        "t_mp_btn_05",
        "t_mp_btn_06",
        "t_mp_btn_07",
        "t_mp_btn_08",
        "b_mp_btn_00",
        "b_mp_btn_01",
        "b_mp_btn_02",
        "b_mp_btn_03",
        "b_mp_btn_04",
        "b_mp_btn_05",
        "b_mp_btn_06",
        "b_mp_btn_07",
        "b_mp_btn_08",
        "t_3",
        "t_4",
        "t_5",
        "t_6"
    };

    // Size 0x10. A button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nTextPane;          // 0x02
        unsigned short m_nNextBtnKindId[3]; // 0x04: its own kind, up, down
        const char *m_pLabel;               // 0x0C, in message group "ExhibitionMenuMessage"
    };

    // The buttons for one to four players.
    const ButtonParam s_ButtonParam1P[] = {
        { 1, PANE_B_MP_BTN_00, PANE_T_MP_BTN_00, { BTN_KIND_MODE_0, 0, BTN_KIND_MODE_1 }, "VS_02_00" },
        { 2, PANE_B_MP_BTN_01, PANE_T_MP_BTN_01, { BTN_KIND_MODE_1, BTN_KIND_MODE_0, 0 }, "VS_02_01" }
    };

    const ButtonParam s_ButtonParam2P[] = {
        { 6, PANE_B_MP_BTN_05, PANE_T_MP_BTN_05, { BTN_KIND_MODE_0, 0, BTN_KIND_MODE_1 }, "VS_02_00" },
        { 7, PANE_B_MP_BTN_06, PANE_T_MP_BTN_06, { BTN_KIND_MODE_1, BTN_KIND_MODE_0, BTN_KIND_MODE_2 }, "VS_02_01" },
        { 8, PANE_B_MP_BTN_07, PANE_T_MP_BTN_07, { BTN_KIND_MODE_2, BTN_KIND_MODE_1, BTN_KIND_MODE_3 }, "VS_02_00" },
        { 9, PANE_B_MP_BTN_08, PANE_T_MP_BTN_08, { BTN_KIND_MODE_3, BTN_KIND_MODE_2, 0 }, "VS_02_01" }
    };

    const ButtonParam s_ButtonParam3P[] = {
        { 3, PANE_B_MP_BTN_02, PANE_T_MP_BTN_02, { BTN_KIND_MODE_1, 0, BTN_KIND_MODE_2 }, "VS_02_01" },
        { 4, PANE_B_MP_BTN_03, PANE_T_MP_BTN_03, { BTN_KIND_MODE_2, BTN_KIND_MODE_1, BTN_KIND_MODE_3 }, "VS_02_00" },
        { 5, PANE_B_MP_BTN_04, PANE_T_MP_BTN_04, { BTN_KIND_MODE_3, BTN_KIND_MODE_2, 0 }, "VS_02_01" }
    };

    const ButtonParam s_ButtonParam4P[] = {
        { 1, PANE_B_MP_BTN_00, PANE_T_MP_BTN_00, { BTN_KIND_MODE_2, 0, BTN_KIND_MODE_3 }, "VS_02_00" },
        { 2, PANE_B_MP_BTN_01, PANE_T_MP_BTN_01, { BTN_KIND_MODE_3, BTN_KIND_MODE_2, 0 }, "VS_02_01" }
    };

    // The number of buttons and the buttons, by number of players.
    const long s_ButtonNum[PLAYER_IDX_NUM] = { 2, 4, 3, 2 };

    const ButtonParam *s_ButtonParam[PLAYER_IDX_NUM] = {
        s_ButtonParam1P,
        s_ButtonParam2P,
        s_ButtonParam3P,
        s_ButtonParam4P
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nPlayerIdx, long nButton)
    {
        return nButton < s_ButtonNum[nPlayerIdx] ? &s_ButtonParam[nPlayerIdx][nButton] : 0;
    }

    // Size 5. One court character.
    struct CourtChara {
        unsigned char m_nPos;     // 0x00, its place (the table's index)
        unsigned char m_nChara;   // 0x01, CHARA_NONE for an empty place
        unsigned char m_nUnk2;    // 0x02
        unsigned char m_nUnk3;    // 0x03
        unsigned char m_nCaption; // 0x04
    };

    enum {
        COURT_CHARA_NUM = 6,
        CHARA_NONE = -1 // stored as 0xFF
    };

    // The characters shown for each play mode button (see s_CourtCharaSet).
    CourtChara s_CourtChara1P_0[COURT_CHARA_NUM] = {
        { 0, 0, 0, 1, 0 },
        { 1, 0, 0xFE, 2, 4 },
        { 2, 0, 0xFD, 4, 4 },
        { 3, 0, 0xFD, 5, 4 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };
    CourtChara s_CourtChara1P_1[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 0, 0xFE, 1, 4 },
        { 2, 0, 0xFE, 2, 4 },
        { 3, 0, 0xFD, 3, 4 },
        { 4, 0, 0xFD, 4, 4 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtChara2P_0[COURT_CHARA_NUM] = {
        { 0, 0, 0, 1, 0 },
        { 1, 1, 0, 2, 1 },
        { 2, 0, 0xFD, 4, 4 },
        { 3, 0, 0xFD, 5, 4 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };
    CourtChara s_CourtChara2P_1[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 1, 0, 1, 1 },
        { 2, 0, 0xFE, 2, 4 },
        { 3, 0, 0xFD, 3, 4 },
        { 4, 0, 0xFD, 4, 4 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtChara2P_2[COURT_CHARA_NUM] = {
        { 0, 0, 0, 1, 0 },
        { 1, 0, 0xFE, 2, 4 },
        { 2, 1, 0, 4, 1 },
        { 3, 0, 0xFD, 5, 4 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };
    CourtChara s_CourtChara2P_3[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 0, 0xFE, 1, 4 },
        { 2, 0, 0xFE, 2, 4 },
        { 3, 1, 0, 3, 1 },
        { 4, 0, 0xFD, 4, 4 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtChara3P_1[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 1, 0, 1, 1 },
        { 2, 2, 0, 2, 2 },
        { 3, 0, 0xFD, 3, 4 },
        { 4, 0, 0xFD, 4, 4 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtChara3P_2[COURT_CHARA_NUM] = {
        { 0, 0, 0, 1, 0 },
        { 1, 1, 0, 2, 1 },
        { 2, 2, 0, 4, 2 },
        { 3, 0, 0xFD, 5, 4 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };
    CourtChara s_CourtChara3P_3[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 1, 0, 1, 1 },
        { 2, 0, 0xFE, 2, 4 },
        { 3, 2, 0, 3, 2 },
        { 4, 0, 0xFD, 4, 4 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtChara4P_2[COURT_CHARA_NUM] = {
        { 0, 0, 0, 1, 0 },
        { 1, 1, 0, 2, 1 },
        { 2, 2, 0, 4, 2 },
        { 3, 0x0A, 0, 5, 3 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };
    CourtChara s_CourtChara4P_3[COURT_CHARA_NUM] = {
        { 0, 0, 0, 0, 0 },
        { 1, 1, 0, 1, 1 },
        { 2, 0, 0xFE, 2, 4 },
        { 3, 2, 0, 3, 2 },
        { 4, 0x0A, 0, 4, 3 },
        { 5, 0, 0xFD, 5, 4 }
    };
    CourtChara s_CourtCharaDefault[COURT_CHARA_NUM] = {
        { 0, 0, 0xFD, 1, 0 },
        { 1, 1, 0xFD, 2, 1 },
        { 2, 2, 0xFD, 4, 2 },
        { 3, 0x0A, 0xFD, 5, 3 },
        { 4, CHARA_NONE, 0xFF, 0, 4 },
        { 5, CHARA_NONE, 0xFF, 3, 4 }
    };

    // The button the cursor starts on, by number of players.
    const long s_DefaultBtnKindId[PLAYER_IDX_NUM] = {
        BTN_KIND_MODE_0, BTN_KIND_MODE_0, BTN_KIND_MODE_1, BTN_KIND_MODE_2
    };

    // Size 8. The court characters of one play mode button.
    struct CourtCharaSet {
        long m_nNum;
        CourtChara *m_pChara;
    };

    const CourtCharaSet s_CourtCharaSet[PLAYER_IDX_NUM][COURT_SET_NUM] = {
        { { COURT_CHARA_NUM, s_CourtChara1P_0 }, { COURT_CHARA_NUM, s_CourtChara1P_1 },
          { COURT_CHARA_NUM, s_CourtCharaDefault }, { COURT_CHARA_NUM, s_CourtCharaDefault } },
        { { COURT_CHARA_NUM, s_CourtChara2P_0 }, { COURT_CHARA_NUM, s_CourtChara2P_1 },
          { COURT_CHARA_NUM, s_CourtChara2P_2 }, { COURT_CHARA_NUM, s_CourtChara2P_3 } },
        { { COURT_CHARA_NUM, s_CourtCharaDefault }, { COURT_CHARA_NUM, s_CourtChara3P_1 },
          { COURT_CHARA_NUM, s_CourtChara3P_2 }, { COURT_CHARA_NUM, s_CourtChara3P_3 } },
        { { COURT_CHARA_NUM, s_CourtCharaDefault }, { COURT_CHARA_NUM, s_CourtCharaDefault },
          { COURT_CHARA_NUM, s_CourtChara4P_2 }, { COURT_CHARA_NUM, s_CourtChara4P_3 } }
    };
}

msw::SelectMenu::CPlayMode::CPlayMode(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

msw::SelectMenu::CPlayMode::~CPlayMode()
{
    Remove();
}

void msw::SelectMenu::CPlayMode::Main()
{
    StateControl();

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CPlayMode::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // The game numbers the statics of this file in declaration
            // order but stores each function's in the reverse one; the bytes
            // only come out right declared in reverse here: static-order
            // swap (see notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            static const long padNo = 0;
            m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, ANIM_MULTI_W01, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            SetCursorMarkerPosition(padNo, GetCurrentButtonKindId(padNo));
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_MULTI_W00_POS, ANIM_MULTI_W01)) {
                long nPlayerIdx = GetPlayerIdx(GetSelectMenu()->m_nPlayerNum);
                SetCourtCharaObj(s_DefaultBtnKindId[nPlayerIdx]);
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

void msw::SelectMenu::CPlayMode::Disp()
{
}

int msw::SelectMenu::CPlayMode::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    long nPlayerIdx = GetPlayerIdx(pSelectMenu->m_nPlayerNum);
    m_nPlayerIdx = nPlayerIdx;

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_LayoutName[0]);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            long nButtonNum = s_ButtonNum[nPlayerIdx];
            for (long i = 0; i < nButtonNum; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const ButtonParam *pParam = GetButtonParam(nPlayerIdx, i);

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nAnim[0] = ANIM_BTN_01;
                pPart->m_nAnim[1] = ANIM_BTN_02;
                pPart->m_nAnim[2] = ANIM_BTN_03;
                pPart->m_nAnim[3] = ANIM_BTN_00;
                pPart->m_nAnim[4] = ANIM_BTN_04;
                pPart->m_nAnim[5] = ANIM_BTN_02;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = 0xBBB;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                pPart->m_bUnk40_0020 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            for (long i = 0; i < nButtonNum; i++) {
                const ButtonParam *pParam = GetButtonParam(nPlayerIdx, i);
                const wchar_t *pText = GetMessageLabelString("ExhibitionMenuMessage", pParam->m_pLabel);
                m_pPaneAccessor->SetTextBoxString(pParam->m_nTextPane, pText, wcslen(pText));
            }

            const wchar_t *pText = GetMessageLabelString("me_multiplay_00", "t_3");
            m_pPaneAccessor->SetTextBoxString(PANE_T_3, pText, wcslen(pText));
            pText = GetMessageLabelString("me_multiplay_00", "t_4");
            m_pPaneAccessor->SetTextBoxString(PANE_T_4, pText, wcslen(pText));
            pText = GetMessageLabelString("me_multiplay_00", "t_5");
            m_pPaneAccessor->SetTextBoxString(PANE_T_5, pText, wcslen(pText));
            pText = GetMessageLabelString("me_multiplay_00", "t_6");
            m_pPaneAccessor->SetTextBoxString(PANE_T_6, pText, wcslen(pText));

            ButtonDispInit(nButtonNum);
            PaneColorInit();
        }

        m_Unk2C = 1;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CPlayMode::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPlayMode::SendEvent(long, void *)
{
    return -1;
}

long msw::SelectMenu::CPlayMode::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CPlayMode::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayMode::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayMode::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayMode::ClearCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayMode::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPlayMode::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CPlayMode::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = s_DefaultBtnKindId[m_nPlayerIdx];
    }
    return nBtnKindId;
}

int msw::SelectMenu::CPlayMode::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CPlayMode::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into StateControl() although defined after it; the out-of-line
// copy is not called and the link drops it. Defined down here because its
// local static follows the constants of Set() in .rodata.
void msw::SelectMenu::CPlayMode::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// The colour of the game kind. Inlined into Set().
inline void msw::SelectMenu::CPlayMode::PaneColorInit()
{
    // static-order swap, as in StateControl().
    static const int reset = 1;
    static const int recursive = 1;

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
        case 5:
            nAnim = ANIM_BTN_COLOR_05;
            break;
        default:
            nAnim = ANIM_BTN_COLOR_00;
            break;
    }
    m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, nAnim, 1.0f, 0.0f, recursive, reset);
}

// Shows as many buttons as there are. Inlined into Set().
inline void msw::SelectMenu::CPlayMode::ButtonDispInit(long nButtonNum)
{
    // static-order swap, as in StateControl().
    static const int reset = 1;
    static const int recursive = 1;

    long nAnim;
    if (nButtonNum == 2) {
        nAnim = ANIM_BTN_VIS_01;
    } else if (nButtonNum == 3) {
        nAnim = ANIM_BTN_VIS_02;
    } else if (nButtonNum == 4) {
        nAnim = ANIM_BTN_VIS_03;
    }
    m_pPaneAccessor->PlayAnimation(PANE_MULTI_W00_POS, nAnim, 1.0f, 0.0f, recursive, reset);
}

void msw::SelectMenu::CPlayMode::CallbackButtonAnimIn(long nBtnKindId)
{
    switch (nBtnKindId) {
        case BTN_KIND_MODE_0:
        case BTN_KIND_MODE_1:
        case BTN_KIND_MODE_2:
        case BTN_KIND_MODE_3:
            SetCourtCharaObj(nBtnKindId);
            break;
    }
}

void msw::SelectMenu::CPlayMode::SetCourtCharaObj(long nBtnKindId)
{
    // static-order swap, as in StateControl().
    static const long camNo = 0;
    static const int starFlag = 0;

    long nSet;
    switch (nBtnKindId) {
        case BTN_KIND_MODE_0:
            nSet = 0;
            break;
        case BTN_KIND_MODE_1:
            nSet = 1;
            break;
        case BTN_KIND_MODE_2:
            nSet = 2;
            break;
        case BTN_KIND_MODE_3:
            nSet = 3;
            break;
        default:
            return;
    }

    CCourtChr *pCourtChr = static_cast<CCourtChr *>(GetSelectMenu()->FindObj(OBJ_COURT_CHR));
    if (pCourtChr == 0) {
        return;
    }

    const CourtCharaSet *pSet = &s_CourtCharaSet[m_nPlayerIdx][nSet];
    long nNum = pSet->m_nNum;
    // One model handle per character, on the stack.
    unsigned long *pMdlHandle = (unsigned long *)__alloca(nNum * sizeof(unsigned long));

    for (long i = 0; i < nNum; i++) {
        const CourtChara *pChara = &pSet->m_pChara[i];
        pMdlHandle[i] = pCourtChr->SetCharaModel(i, pChara->m_nChara, pChara->m_nUnk2, camNo, pChara->m_nUnk3,
                                                 (RFLDataSource)6, 0, starFlag);
        if (pMdlHandle[i] != 0) {
            pCourtChr->SetCharaMotion(i, 1, 2, 1.0f, 0.0f, 0.0f, 0);
        }
    }

    CChrCaption *pCaption = static_cast<CChrCaption *>(GetSelectMenu()->FindObj(OBJ_CHR_CAPTION));
    if (pCaption != 0) {
        bcs::FVEC3 offset(0.0f, 0.5f, 0.0f);
        for (long i = 0; i < nNum; i++) {
            const CourtChara *pChara = &pSet->m_pChara[i];
            if (pChara->m_nChara != CHARA_NONE) {
                if (pMdlHandle[i] != 0) {
                    pCaption->SetCaption(i, pChara->m_nCaption, pCourtChr->GetNextMdlHandleByCharaId(i), 0, &offset);
                } else {
                    pCaption->SetCaption(i, pChara->m_nCaption, pCourtChr->GetMdlHandleByCharaId(i), 0, &offset);
                }
            }
        }

        switch (nBtnKindId) {
            case BTN_KIND_MODE_0:
            case BTN_KIND_MODE_2:
                pCourtChr->WaitToPlayAnim(CCourtCharaObj::s_FadeInOutTime, 3, 1.0f);
                break;
            case BTN_KIND_MODE_1:
            case BTN_KIND_MODE_3:
                pCourtChr->WaitToPlayAnim(CCourtCharaObj::s_FadeInOutTime, 4, 1.0f);
                break;
        }
    }
}
