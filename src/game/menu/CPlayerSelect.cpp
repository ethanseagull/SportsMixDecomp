#include "game/msw/SelectMenu/CPlayerSelect.hpp"

#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"

namespace CPlayerSelect_cpp {
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
        OBJ_TASK = 1,
        OBJ_PLAYER_SELECT = 0x1F
    };

    // The button kind ids of the screen: the back button, the eight Mii
    // buttons and the "add" button.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_MII_1 = 0x86,
        BTN_KIND_MII_2,
        BTN_KIND_MII_3,
        BTN_KIND_MII_4,
        BTN_KIND_MII_5,
        BTN_KIND_MII_6,
        BTN_KIND_MII_7,
        BTN_KIND_MII_8,
        BTN_KIND_ADD,
        BTN_KIND_DEFAULT = BTN_KIND_MII_1
    };

    // The mode a Mii button leads to.
    enum {
        MODE_PLAYER_INFO = 0x18
    };
}

void msw::CSelectMenu::WifiPlayerSelect_In()
{
    Build(OBJ_PLAYER_SELECT);
    SendObjEventTrigger(OBJ_TOP, 0, (void *)GetMessageString("TitleWiFiPlSel", 0));
    SendObjEventTrigger(OBJ_TOP, 1, (void *)GetMessageString("TitleWiFiPlSel", 1));
    SendObjEventTrigger(OBJ_TOP, 2, 0);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYER_SELECT);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(0, nBtnKindId);
    }
}

void msw::CSelectMenu::WifiPlayerSelect_Loop()
{
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PLAYER_SELECT);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        float fWait = _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_MII_1:
            case BTN_KIND_MII_2:
            case BTN_KIND_MII_3:
            case BTN_KIND_MII_4:
            case BTN_KIND_MII_5:
            case BTN_KIND_MII_6:
            case BTN_KIND_MII_7:
            case BTN_KIND_MII_8:
                pObj->PushButton(0, nBtnKindId);
                SetNextMode(MODE_PLAYER_INFO, fWait);
                break;
            default:
                SetNextMode(nMode, fWait);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_NONE:
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        GoBackToMode(0, GetPrevMode(), pObj, 1);
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::WifiPlayerSelect_Out()
{
    Scratch(OBJ_PLAYER_SELECT);
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
        ANIM_FLAME_IN,
        ANIM_FLAME_OUT,
        ANIM_FLAME_WAIT,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_TW,
        PANE_TV_R,
        PANE_TUIKA_ICO,
        PANE_B_TUIKA_ICO,
        PANE_T_POPUP01,
        PANE_MII_POS_01,
        PANE_M_MII_01,
        PANE_B_M_MII_01,
        PANE_T_MII_NAME_01,
        PANE_MII_POS_02,
        PANE_M_MII_02,
        PANE_B_M_MII_02,
        PANE_T_MII_NAME_02,
        PANE_MII_POS_03,
        PANE_M_MII_03,
        PANE_B_M_MII_03,
        PANE_T_MII_NAME_03,
        PANE_MII_POS_04,
        PANE_M_MII_04,
        PANE_B_M_MII_04,
        PANE_T_MII_NAME_04,
        PANE_MII_POS_05,
        PANE_M_MII_05,
        PANE_B_M_MII_05,
        PANE_T_MII_NAME_05,
        PANE_MII_POS_06,
        PANE_M_MII_06,
        PANE_B_M_MII_06,
        PANE_T_MII_NAME_06,
        PANE_MII_POS_07,
        PANE_M_MII_07,
        PANE_B_M_MII_07,
        PANE_T_MII_NAME_07,
        PANE_MII_POS_08,
        PANE_M_MII_08,
        PANE_B_M_MII_08,
        PANE_T_MII_NAME_08,
        PANE_NUM
    };

    enum {
        BUTTON_NUM = 9
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    const char *const s_LayoutName = "main_p_sel.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_p_sel_Btn_00.brlan",
        "main_p_sel_Btn_01.brlan",
        "main_p_sel_Btn_02.brlan",
        "main_p_sel_Btn_03.brlan",
        "main_p_sel_Btn_04.brlan",
        "main_p_sel_flame_in.brlan",
        "main_p_sel_flame_out.brlan",
        "main_p_sel_flame_wait.brlan",
        "main_p_sel_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "tw",
        "tv_R",
        "tuika_ico",
        "b_tuika_ico",
        "t_popup01",
        "mii_pos_01",
        "m_mii_01",
        "b_m_mii_01",
        "t_Mii_name_01",
        "mii_pos_02",
        "m_mii_02",
        "b_m_mii_02",
        "t_Mii_name_02",
        "mii_pos_03",
        "m_mii_03",
        "b_m_mii_03",
        "t_Mii_name_03",
        "mii_pos_04",
        "m_mii_04",
        "b_m_mii_04",
        "t_Mii_name_04",
        "mii_pos_05",
        "m_mii_05",
        "b_m_mii_05",
        "t_Mii_name_05",
        "mii_pos_06",
        "m_mii_06",
        "b_m_mii_06",
        "t_Mii_name_06",
        "mii_pos_07",
        "m_mii_07",
        "b_m_mii_07",
        "t_Mii_name_07",
        "mii_pos_08",
        "m_mii_08",
        "b_m_mii_08",
        "t_Mii_name_08"
    };

    // Size 0xE. A button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nIconPane;          // 0x02, not read here
        unsigned char m_nTextPane;          // 0x03, not read here
        unsigned short m_nNextBtnKindId[5]; // 0x04: its own kind, then its neighbours
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { PANE_MII_POS_01, PANE_B_M_MII_01, PANE_M_MII_01, PANE_T_MII_NAME_01,
          { BTN_KIND_MII_1, 0, 0, 0, BTN_KIND_MII_2 } },
        { PANE_MII_POS_02, PANE_B_M_MII_02, PANE_M_MII_02, PANE_T_MII_NAME_02,
          { BTN_KIND_MII_2, 0, 0, BTN_KIND_MII_1, BTN_KIND_MII_3 } },
        { PANE_MII_POS_03, PANE_B_M_MII_03, PANE_M_MII_03, PANE_T_MII_NAME_03,
          { BTN_KIND_MII_3, 0, 0, BTN_KIND_MII_2, BTN_KIND_MII_4 } },
        { PANE_MII_POS_04, PANE_B_M_MII_04, PANE_M_MII_04, PANE_T_MII_NAME_04,
          { BTN_KIND_MII_4, 0, 0, BTN_KIND_MII_3, BTN_KIND_MII_5 } },
        { PANE_MII_POS_05, PANE_B_M_MII_05, PANE_M_MII_05, PANE_T_MII_NAME_05,
          { BTN_KIND_MII_5, 0, 0, BTN_KIND_MII_4, BTN_KIND_MII_6 } },
        { PANE_MII_POS_06, PANE_B_M_MII_06, PANE_M_MII_06, PANE_T_MII_NAME_06,
          { BTN_KIND_MII_6, 0, 0, BTN_KIND_MII_5, BTN_KIND_MII_7 } },
        { PANE_MII_POS_07, PANE_B_M_MII_07, PANE_M_MII_07, PANE_T_MII_NAME_07,
          { BTN_KIND_MII_7, 0, 0, BTN_KIND_MII_6, BTN_KIND_MII_8 } },
        { PANE_MII_POS_08, PANE_B_M_MII_08, PANE_M_MII_08, PANE_T_MII_NAME_08,
          { BTN_KIND_MII_8, 0, 0, BTN_KIND_MII_7, BTN_KIND_ADD } },
        // 0x26 is past the pane table; the field is not read here.
        { PANE_TUIKA_ICO, PANE_B_TUIKA_ICO, 0x26, PANE_T_POPUP01,
          { BTN_KIND_ADD, 0, 0, BTN_KIND_MII_8, 0 } }
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < BUTTON_NUM ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CPlayerSelect::CPlayerSelect(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPlayerSelect::~CPlayerSelect()
{
    Remove();
}

void msw::SelectMenu::CPlayerSelect::Main()
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
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            GetSelectMenu()->SendObjEventTrigger(1, EVENT_BACK, m_nPushButtonId);
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

void msw::SelectMenu::CPlayerSelect::Disp()
{
}

int msw::SelectMenu::CPlayerSelect::Set()
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
                pPart->m_nSeCode[1] = 0xBBB;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                pPart->m_bUnk40_0020 = 1;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            bcs::G2D::CPaneCtrl *pLyt = GetLytObj();
            if (pLyt != 0) {
                pLyt->PlayAnim(ANIM_FLAME_IN, 1.0f, 0.0f, 1, 1);
                pLyt->PlayAnim(ANIM_WIN, 1.0f, 0.0f, 1, 1);
            }
        }

        m_Unk2C = 0;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CPlayerSelect::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPlayerSelect::SendEvent(long, void *)
{
    return -1;
}

long msw::SelectMenu::CPlayerSelect::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CPlayerSelect::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayerSelect::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayerSelect::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CPlayerSelect::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPlayerSelect::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CPlayerSelect::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_DEFAULT;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CPlayerSelect::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CPlayerSelect::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}
