#include "game/msw/SelectMenu/CPlayerInfo.hpp"

#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"

namespace CPlayerInfo_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float constant of this
// file with its own lis, which this compiler does when the first .rodata object of the file is
// weak (a function-local const). The module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_PLAYER_INFO = 0x20
    };

    // What the top object (0) is sent: the two title lines and a clear.
    enum {
        EVENT_TITLE_0 = 0,
        EVENT_TITLE_1 = 1,
        EVENT_TITLE_CLEAR = 2
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_RULE = 3,
        BTN_KIND_TAB_0 = 0x8F,  // the five tabs
        BTN_KIND_TAB_1 = 0x90,
        BTN_KIND_TAB_2 = 0x91,
        BTN_KIND_TAB_3 = 0x92,
        BTN_KIND_TAB_4 = 0x93,
        BTN_KIND_PREV_TAB = 0x94,
        BTN_KIND_NEXT_TAB = 0x95,
        BTN_KIND_OK = 0x96,
        BTN_KIND_DEFAULT = BTN_KIND_TAB_0
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // The mode the OK button moves on to.
    enum {
        MODE_NEXT = 0x1B
    };
}

void msw::CSelectMenu::WifiPlayerInfo_In()
{
    Build(OBJ_PLAYER_INFO);
    SendObjEventTrigger(OBJ_TOP, EVENT_TITLE_0, (void *)GetMessageString("TitleWiFiPlInfo", 0));
    SendObjEventTrigger(OBJ_TOP, EVENT_TITLE_1, (void *)GetMessageString("TitleWiFiPlInfo", 1));
    SendObjEventTrigger(OBJ_TOP, EVENT_TITLE_CLEAR, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYER_INFO);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(0, nBtnKindId);
    }
}

void msw::CSelectMenu::WifiPlayerInfo_Loop()
{
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    SelectMenu::CPlayerInfo *pObj = (SelectMenu::CPlayerInfo *)FindObj(OBJ_PLAYER_INFO);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        float fWaitTime = _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_TAB_0:
            case BTN_KIND_TAB_1:
            case BTN_KIND_TAB_2:
            case BTN_KIND_TAB_3:
            case BTN_KIND_TAB_4: {
                pObj->PushButton(0, nBtnKindId);
                long nTabNo = nBtnKindId - BTN_KIND_TAB_0;
                pObj->m_nTabNo = nTabNo;
                pObj->ChangeTab(nTabNo);
                break;
            }
            case BTN_KIND_PREV_TAB:
                pObj->PushButton(0, nBtnKindId);
                pObj->ChangeTab(pObj->DecTabNo(pObj->m_nTabNo));
                break;
            case BTN_KIND_NEXT_TAB:
                pObj->PushButton(0, nBtnKindId);
                pObj->ChangeTab(pObj->IncTabNo(pObj->m_nTabNo));
                break;
            case BTN_KIND_OK:
                pObj->PushButton(0, nBtnKindId);
                SetNextMode(MODE_NEXT, fWaitTime);
                break;
            case BTN_KIND_RULE:
                pObj->PushButton(0, nBtnKindId);
                break;
            default:
                SetNextMode(nMode, fWaitTime);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_NONE:
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(0, GetPrevMode(), pObj, 1);
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::WifiPlayerInfo_Out()
{
    Scratch(OBJ_PLAYER_INFO);
}

// The names in this namespace are guesses; the objects have no symbols.
// They are defined here, after the CSelectMenu steps, because their strings
// come first in the unit's .rodata.
namespace {

    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_FLAME_IN,
        ANIM_FLAME_OUT,
        ANIM_FLAME_WAIT,
        ANIM_TUB1_ON,     // one per tab
        ANIM_TUB2_ON,
        ANIM_TUB3_ON,
        ANIM_TUB4_ON,
        ANIM_TUB5_ON,
        ANIM_TUB_1_00,
        ANIM_TUB_1_01,
        ANIM_TUB_1_02,
        ANIM_TUB_ALL_OFF,
        ANIM_WIN,
        ANIM_YAJI_OFF,
        ANIM_YAJI_ON,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_TV_R,
        PANE_YAJI_L,
        PANE_YAJI_R,
        PANE_B_YAJI_L_00,
        PANE_B_YAJI_R_00,
        PANE_TUB_01,
        PANE_TUB_02,
        PANE_TUB_03,
        PANE_TUB_04,
        PANE_TUB_05,
        PANE_BOUNDING_00,
        PANE_BOUNDING_02,
        PANE_BOUNDING_03,
        PANE_BOUNDING_04,
        PANE_BOUNDING_05,
        PANE_RULE_ICO,
        PANE_B_RULE_ICO,
        PANE_POPUP01,
        PANE_T_POPUP01,
        PANE_BTN_00,
        PANE_B_BTN_00,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 9,
        TAB_NUM = 5
    };

    // No animation in ButtonParam::m_nAnimPushCancel.
    const unsigned char ANIM_NONE = 0xFF;

    const char *const s_LayoutName = "main_p_inf.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_p_inf_Btn_00.brlan",
        "main_p_inf_Btn_01.brlan",
        "main_p_inf_Btn_02.brlan",
        "main_p_inf_Btn_03.brlan",
        "main_p_inf_Btn_04.brlan",
        "main_p_inf_flame_in.brlan",
        "main_p_inf_flame_out.brlan",
        "main_p_inf_flame_wait.brlan",
        "main_p_inf_Tub1_on.brlan",
        "main_p_inf_Tub2_on.brlan",
        "main_p_inf_Tub3_on.brlan",
        "main_p_inf_Tub4_on.brlan",
        "main_p_inf_Tub5_on.brlan",
        "main_p_inf_Tub_1_00.brlan",
        "main_p_inf_Tub_1_01.brlan",
        "main_p_inf_Tub_1_02.brlan",
        "main_p_inf_Tub_all_off.brlan",
        "main_p_inf_win_anm_00.brlan",
        "main_p_inf_yaji_off.brlan",
        "main_p_inf_yaji_on.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "tv_R",
        "yaji_L",
        "yaji_R",
        "b_yaji_L_00",
        "b_yaji_R_00",
        "Tub_01",
        "Tub_02",
        "Tub_03",
        "Tub_04",
        "Tub_05",
        "Bounding_00",
        "Bounding_02",
        "Bounding_03",
        "Bounding_04",
        "Bounding_05",
        "rule_ico",
        "b_rule_ico",
        "popup01",
        "t_popup01",
        "btn_00",
        "b_btn_00"
    };

    // Size 0x20. A button's description. The animations go to
    // CMenuPartBase::m_nAnim in the order 3, 0, 1, 2, 4.
    struct ButtonParam {
        // 0x00. The pane the button shows; also its m_nBtnKindId and
        // m_nCurrentBtnKindId (SetVisibleArrow passes the latter as a pane).
        unsigned char m_nPane;
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nAnimWait;          // 0x02 -> m_nAnim[3]
        unsigned char m_nAnimIn;            // 0x03 -> m_nAnim[0]
        unsigned char m_nAnimOut;           // 0x04 -> m_nAnim[1]
        unsigned char m_nAnimPush;          // 0x05 -> m_nAnim[2]
        unsigned char m_nAnimPushCancel;    // 0x06 -> m_nAnim[4], ANIM_NONE for none
        unsigned short m_nNextBtnKindId[5]; // 0x08
        long m_nSeCode1;                    // 0x14 -> m_nSeCode[1]
        long m_nSeCode0;                    // 0x18 -> m_nSeCode[0]
        unsigned char m_bFlag4;             // 0x1C, flag 0x0004 of the part
        unsigned char m_bStickMode;         // 0x1D, flag 0x0020 of the part
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { PANE_TUB_01, PANE_BOUNDING_00, ANIM_BTN_00, ANIM_TUB_1_00, ANIM_TUB_1_01, ANIM_TUB_1_02, ANIM_NONE,
          { BTN_KIND_TAB_0, 0, BTN_KIND_OK, BTN_KIND_TAB_4, BTN_KIND_TAB_1 }, 0xBCD, -1, 1, 1 },
        { PANE_TUB_02, PANE_BOUNDING_02, ANIM_BTN_00, ANIM_TUB_1_00, ANIM_TUB_1_01, ANIM_TUB_1_02, ANIM_NONE,
          { BTN_KIND_TAB_1, 0, BTN_KIND_OK, BTN_KIND_TAB_0, BTN_KIND_TAB_2 }, 0xBCD, -1, 1, 1 },
        { PANE_TUB_03, PANE_BOUNDING_03, ANIM_BTN_00, ANIM_TUB_1_00, ANIM_TUB_1_01, ANIM_TUB_1_02, ANIM_NONE,
          { BTN_KIND_TAB_2, 0, BTN_KIND_OK, BTN_KIND_TAB_1, BTN_KIND_TAB_3 }, 0xBCD, -1, 1, 1 },
        { PANE_TUB_04, PANE_BOUNDING_04, ANIM_BTN_00, ANIM_TUB_1_00, ANIM_TUB_1_01, ANIM_TUB_1_02, ANIM_NONE,
          { BTN_KIND_TAB_3, 0, BTN_KIND_OK, BTN_KIND_TAB_2, BTN_KIND_TAB_4 }, 0xBCD, -1, 1, 1 },
        { PANE_TUB_05, PANE_BOUNDING_05, ANIM_BTN_00, ANIM_TUB_1_00, ANIM_TUB_1_01, ANIM_TUB_1_02, ANIM_NONE,
          { BTN_KIND_TAB_4, 0, BTN_KIND_OK, BTN_KIND_TAB_3, BTN_KIND_TAB_0 }, 0xBCD, -1, 1, 1 },
        { PANE_YAJI_L, PANE_B_YAJI_L_00, ANIM_BTN_00, ANIM_BTN_01, ANIM_BTN_02, ANIM_BTN_03, ANIM_BTN_04,
          { BTN_KIND_PREV_TAB, 0, 0, 0, 0 }, 0xBCE, 0xBC0, 1, 0 },
        { PANE_YAJI_R, PANE_B_YAJI_R_00, ANIM_BTN_00, ANIM_BTN_01, ANIM_BTN_02, ANIM_BTN_03, ANIM_BTN_04,
          { BTN_KIND_NEXT_TAB, 0, 0, 0, 0 }, 0xBCD, 0xBC0, 1, 0 },
        { PANE_BTN_00, PANE_B_BTN_00, ANIM_BTN_00, ANIM_BTN_01, ANIM_BTN_02, ANIM_BTN_03, ANIM_BTN_04,
          { BTN_KIND_OK, 0, 0, BTN_KIND_RULE, 0 }, 0xBBB, 0xBC0, 0, 1 },
        { PANE_RULE_ICO, PANE_B_RULE_ICO, ANIM_BTN_00, ANIM_BTN_01, ANIM_BTN_02, ANIM_BTN_03, ANIM_BTN_04,
          { BTN_KIND_RULE, 0, 0, BTN_KIND_OK, 0 }, 0xBBB, 0xBC0, 0, 1 }
    };

    // The animation that shows each tab.
    const unsigned char s_TabAnim[TAB_NUM] = {
        ANIM_TUB1_ON,
        ANIM_TUB2_ON,
        ANIM_TUB3_ON,
        ANIM_TUB4_ON,
        ANIM_TUB5_ON
    };

    // The button kind id of each tab.
    const unsigned char s_TabBtnKindId[TAB_NUM] = {
        BTN_KIND_TAB_0,
        BTN_KIND_TAB_1,
        BTN_KIND_TAB_2,
        BTN_KIND_TAB_3,
        BTN_KIND_TAB_4
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }

    // The index of the first button whose first neighbour kind is
    // nBtnKindId, or -1.
    inline long FindButtonIndex(long nBtnKindId)
    {
        long nButton = -1;
        for (long i = 0; i < BUTTON_NUM; i++) {
            if (nBtnKindId == s_ButtonParam[i].m_nNextBtnKindId[0]) {
                nButton = i;
                break;
            }
        }
        return nButton;
    }

    inline const ButtonParam *FindButtonParam(long nBtnKindId)
    {
        long nButton = FindButtonIndex(nBtnKindId);
        return nButton >= 0 ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CPlayerInfo::CPlayerInfo(long nId)
    : CSelectMenuLayoutBase(nId)
    , CInfoTabChange(TAB_NUM)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPlayerInfo::~CPlayerInfo()
{
    Remove();
}

void msw::SelectMenu::CPlayerInfo::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TV_R, ANIM_FLAME_IN)) {
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                m_pPaneAccessor->PlayAnimation(PANE_TW, ANIM_FLAME_OUT, 1.0f, 0.0f, 0, 1);
                m_pPaneAccessor->PlayAnimation(PANE_TV_R, ANIM_FLAME_OUT, 1.0f, 0.0f, 0, 1);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TV_R, ANIM_FLAME_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CPlayerInfo::Disp()
{
}

int msw::SelectMenu::CPlayerInfo::Set()
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

                const ButtonParam *pParam = GetButtonParam(i);

                pPart->m_nBtnKindId = pParam->m_nPane;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nPane;
                pPart->m_nAnim[0] = pParam->m_nAnimIn;
                pPart->m_nAnim[1] = pParam->m_nAnimOut;
                pPart->m_nAnim[2] = pParam->m_nAnimPush;
                pPart->m_nAnim[3] = pParam->m_nAnimWait;
                if (pParam->m_nAnimPushCancel != ANIM_NONE) {
                    pPart->m_nAnim[4] = pParam->m_nAnimPushCancel;
                }
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                pPart->m_nSeCode[0] = pParam->m_nSeCode0;
                pPart->m_nSeCode[1] = pParam->m_nSeCode1;
                pPart->m_nSeCode[2] = 1;
                // Written on the whole flag word, as in CButtonEntry.cpp.
                pPart->m_nFlags40 = pParam->m_bFlag4 ? (pPart->m_nFlags40 | 0x0004) : (pPart->m_nFlags40 & ~0x0004);
                pPart->m_nFlags40 = pParam->m_bStickMode ? (pPart->m_nFlags40 | 0x0020) : (pPart->m_nFlags40 & ~0x0020);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            InitPaneAnim();
        }

        m_Unk2C = 1;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

// Opens the window on the current tab with both arrows shown. Inlined into
// Set().
inline void msw::SelectMenu::CPlayerInfo::InitPaneAnim()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl == 0) {
        return;
    }

    pPaneCtrl->PlayAnim(ANIM_FLAME_IN, 1.0f, 0.0f, 1, 1);
    pPaneCtrl->PlayAnim(ANIM_WIN, 1.0f, 0.0f, 1, 1);
    ChangeTab(m_nTabNo);
    SetVisibleArrow(BTN_KIND_PREV_TAB, 1);
    SetVisibleArrow(BTN_KIND_NEXT_TAB, 1);
}

void msw::SelectMenu::CPlayerInfo::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPlayerInfo::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(1, 0x13, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CPlayerInfo::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayerInfo::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayerInfo::SetCursorMarkerPosition(long nPad, long)
{
    return nPad != 0 ? -1 : 0;
}

long msw::SelectMenu::CPlayerInfo::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPlayerInfo::StickMoveCheck(long nPad, long nBtnKindId)
{
    long nCurrent = nBtnKindId;
    if (!IsButtonKindOfStickMode(nCurrent)) {
        nCurrent = BTN_KIND_DEFAULT;
    }

    // From the OK button, the stick goes back to the tab that is shown.
    if (nCurrent == BTN_KIND_OK) {
        const ButtonParam *pParam = FindButtonParam(s_TabBtnKindId[m_nTabNo]);
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCurrent);
        if (pParam != 0 && pPart != 0) {
            pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[0];
        }
    }

    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CPlayerInfo::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_DEFAULT;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CPlayerInfo::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CPlayerInfo::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + 0x21, (void *)nBtnKindId);
}

long msw::SelectMenu::CPlayerInfo::ChangeTab(long nTabNo)
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    if (pPaneCtrl == 0) {
        return -1;
    }

    long nAnim;
    if (nTabNo >= 0 && nTabNo < TAB_NUM) {
        nAnim = s_TabAnim[nTabNo];
    } else {
        nAnim = ANIM_TUB_ALL_OFF;
    }
    pPaneCtrl->PlayAnim(nAnim, 1.0f, 0.0f, 1, 1);
    return 0;
}

long msw::SelectMenu::CPlayerInfo::SetVisibleArrow(unsigned short nBtnKindId, int bVisible)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return -1;
    }

    // The current kind id of the arrows is their pane (yaji_L, yaji_R).
    m_pPaneAccessor->PlayAnimation(pPart->m_nCurrentBtnKindId, bVisible ? ANIM_YAJI_ON : ANIM_YAJI_OFF, 1.0f, 0.0f, 1, 1);
    return 0;
}

