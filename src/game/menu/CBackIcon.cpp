#include "game/msw/SelectMenu/CBackIcon.hpp"

#include <wchar.h>

#include "bcs/CPadWI.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CBgChr.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "game/msw/menu/MenuUtil.hpp"

// The names in this namespace are guesses; the objects have no symbols. A
// const pointer lands in .rodata behind its string, the name tables and the
// screen table in .data, the button table in .rodata.
namespace {

    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_BTN_05,
        ANIM_INOUT_00,
        ANIM_INOUT_01,
        ANIM_INOUT_02, // moving in
        ANIM_INOUT_03,
        ANIM_INOUT_04, // moving out
        ANIM_INOUT_05, // shown
        ANIM_INOUT_06, // swapping between two screens
        ANIM_INOUT_07, // hidden at once
        ANIM_NUM
    };

    enum {
        PANE_BACK_ICO,
        PANE_B_BACK_ICO,
        PANE_TS_BACK_ICO_S01,
        PANE_T_BACK_ICO_01,
        PANE_TV_L_00,
        PANE_NUM
    };

    // Values of CSelectMenuLayoutBase::m_Unk30 beyond the base's STATE_*.
    enum {
        STATE_HIDE = 6,    // set by event 0x54; Main does nothing in it
        STATE_HIDE_NOW = 7,
        STATE_SWAP = 8,
        STATE_SWAP_WAIT = 9
    };

    // Values of m_nIconPos.
    enum {
        ICON_SHOWN = 0,
        ICON_HIDDEN = 1,
        ICON_SWAP = 3
    };

    // What this sends to the background (menu object 0, see
    // game/menu/CBackground.cpp).
    enum {
        BG_OBJ_ID = 0,
        BG_EVENT_TAG_TEXT = 0,
        BG_EVENT_TAG_BAR = 2,
        BG_EVENT_TAG_IN = 3,
        BG_EVENT_TAG_OUT = 4,
        BG_EVENT_LINE_BOX_00 = 7,
        BG_EVENT_LINE_BOX_02 = 8,
        BG_EVENT_LINE_BOX_03 = 0xB,
        BG_EVENT_LINE_BOX_04 = 0xC,
        BG_EVENT_TEXT_LINE_00 = 0xD
    };

    // The background's colour bars (TagColor in s_TagTitle).
    enum {
        TAG_NONE = -1,
        TAG_BASE = 0,
        TAG_GAME = 1,      // the bar of the current game kind
        TAG_PARTY = 5,
        TAG_SCORE = 6,
        TAG_WIFI = 7,
        TAG_OTHER = 8
    };

    // Screens (CSelectMenu::m_nMode) handled apart.
    enum {
        MODE_TEAM = 0xE,
        MODE_ENTRY_PARTY = 0x11,
        MODE_ENTRY = 3,
        MODE_WIFI_CHARA_SELECT = 0x19
    };

    const char *const s_LayoutName = "me_back_ico.brlyt";

    // How much faster the background camera and tables run once sped up.
    // In .data with a second float nothing reads, before the name tables.
    float s_SpeedUpRate[2] = { 500.0f, 0.0f };

    const char *s_AnimNames[ANIM_NUM] = {
        "me_back_ico_btn_00.brlan",
        "me_back_ico_btn_01.brlan",
        "me_back_ico_btn_02.brlan",
        "me_back_ico_btn_03.brlan",
        "me_back_ico_btn_04.brlan",
        "me_back_ico_btn_05.brlan",
        "me_back_ico_inout_00.brlan",
        "me_back_ico_inout_01.brlan",
        "me_back_ico_inout_02.brlan",
        "me_back_ico_inout_03.brlan",
        "me_back_ico_inout_04.brlan",
        "me_back_ico_inout_05.brlan",
        "me_back_ico_inout_06.brlan",
        "me_back_ico_inout_07.brlan"
    };

    // Null-terminated; SetPaneAccessor is given PANE_NUM.
    const char *s_PaneNames[PANE_NUM + 1] = {
        "back_ico",
        "b_back_ico",
        "ts_back_ico_s01",
        "t_back_ico_01",
        "tv_L_00",
        0
    };

    // Size 0xC. The title of each screen of CSelectMenu (by m_nMode) and
    // the colour bar shown with it (TAG_*; TAG_NONE: TagTitle leaves the
    // tag as it is).
    struct TagTitleParam {
        const char *m_pGroup;
        const char *m_pLabel;
        long m_nTagColor;
    };

    TagTitleParam s_TagTitle[50] = {
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "TopMenuMessage", "P_SEL_00", TAG_BASE },
        { "ExhibitionMenuMessage", "Mii_00", TAG_GAME },
        { "TopMenuMessage", "TOP_00_MAINMENU", TAG_BASE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "TopMenuMessage", "SCOREBOOK_00", 3 },
        { "TopMenuMessage", "SCOREBOOK_00", 3 },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "ExhibitionMenuMessage", "TEM_00", TAG_GAME },
        { "ExhibitionMenuMessage", "CHA_00", TAG_GAME },
        { "", "", TAG_NONE },
        { "ExhibitionMenuMessage", "BG_00", TAG_GAME },
        { "ExhibitionMenuMessage", "Mii_00", TAG_GAME },
        { "PartyMenuMessage", "PAR_00", 2 },
        { "", "", 2 },
        { "", "", TAG_NONE },
        { "TournamentMenuMessage", "CUP_00", TAG_GAME },
        { "ExhibitionMenuMessage", "ENT_00_02", 4 },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "ExhibitionMenuMessage", "Mii_00", TAG_GAME },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "WifiMenuMessage", "W_GAME_00", 4 },
        { "", "", TAG_NONE },
        { "ExhibitionMenuMessage", "W_BG_00", 4 },
        { "WifiConnectMessage", "WVS_WAIT_00_00", 4 },
        { "", "", TAG_NONE },
        { "WifiMenuMessage", "F_COR_00", 4 },
        { "WifiMenuMessage", "F_LIS_00", 4 },
        { "WifiMenuMessage", "P_INF_00", 4 },
        { "WifiMenuMessage", "F_INF_00", 4 },
        { "WifiMenuMessage", "F_INF_00", 4 },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "WifiMenuMessage", "FVS_WAIT_02", 4 },
        { "ExhibitionMenuMessage", "OPT_00", TAG_GAME },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE },
        { "", "", TAG_NONE }
    };

    // Size 4. The constant part of the button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;      // 0x00
        unsigned char m_nHitPane;        // 0x01
        unsigned short m_nNextBtnKindId; // 0x02
    };

    const ButtonParam s_ButtonParam[1] = {
        { 0, PANE_B_BACK_ICO, 1 }
    };

    // Null when there is no such button.
    inline const ButtonParam *GetButtonParam(long nButton)
    {
        return nButton < 1 ? &s_ButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CBackIcon::CBackIcon(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
    m_bTagIn = 0;
    m_bLineIn = 0;
}

msw::SelectMenu::CBackIcon::~CBackIcon()
{
    Remove();
}

void msw::SelectMenu::CBackIcon::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_02, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TV_L_00, ANIM_INOUT_02)) {
                m_Unk30 = STATE_ACTIVE;
                m_nIconPos = ICON_SHOWN;
            }
            break;
        case STATE_ACTIVE: {
            CSelectMenu *pSelectMenu = GetSelectMenu();
            long nMode = pSelectMenu->m_nMode;

            if (nMode != m_nMode) {
                m_nMode = nMode;
                m_bPushed = 0;
                IndividualSelect(nMode);
                TagTitle(m_nMode);
                m_bSpeedUp = 0;
                m_fSpeedRate = 1.0f;
            }

            // The key text follows whether the leader's remote points at
            // the screen.
            bcs::FVEC2 pos(0.0f, 0.0f);
            long nPad = CMenuPadInfo::s_Instance.SelectPadLeader(0);
            if (nMode == MODE_ENTRY_PARTY || nMode == MODE_WIFI_CHARA_SELECT) {
                nPad = pSelectMenu->m_Unk27D;
            }
            int bOnScreen = bcs::CPadWI::GetScreenPosDPD_F(&pos, nPad);

            CMenuPad *pMenuPad = CMenuPad::GetInstance();
            if (pMenuPad != 0) {
                if (pMenuPad->GetPadStyle(nPad) == 0) {
                    menu::ReplaceKeyTextByPane(m_pPaneAccessor->GetPane(PANE_T_BACK_ICO_01), 0);
                } else if (bOnScreen) {
                    menu::ReplaceKeyTextByPane(m_pPaneAccessor->GetPane(PANE_T_BACK_ICO_01), 0);
                } else {
                    menu::ReplaceKeyTextByPane(m_pPaneAccessor->GetPane(PANE_T_BACK_ICO_01), 1);
                }

                for (long i = 0; i < 4; i++) {
                    if (CMenuPad::s_Instance.GetPadStyle(i) != 0) {
                        if (bcs::CPadWI::GetScreenPosDPD_F(&pos, i)) {
                            CMenuPad::s_Instance.m_nPointerOut[i] = 0;
                        } else {
                            CMenuPad::s_Instance.m_nPointerOut[i] = 1;
                        }
                    }
                }
            }

            // Speed the camera and the tables up while they move to the
            // new screen, when the leader presses a button or event 0x56
            // asks for it.
            if (nMode == MODE_TEAM && pSelectMenu->m_Unk9D == 0x11) {
                break;
            }
            if (nMode == MODE_ENTRY_PARTY || nMode == MODE_ENTRY || nMode <= 6) {
                break;
            }
            CCameraMenuBG *pCamera = &CCameraMenuBG::s_Instance;
            if (!pCamera->IsEndAnim() && m_bSpeedUp == 0) {
                long nLeader = CMenuPadInfo::s_Instance.SelectPadLeader(0);
                if ((CMenuPad::s_Instance.GetInput(nLeader) & 0x30) || m_bSpeedRequest != 0) {
                    m_fSpeedRate = s_SpeedUpRate[0];
                    pCamera->SetSpeed(m_fSpeedRate * pCamera->GetSpeed());

                    CBgChr *pBgChr = (CBgChr *)pSelectMenu->FindObj(0x3A);
                    if (pBgChr != 0 && pBgChr->GetTableAnimSpeed() < m_fSpeedRate) {
                        pBgChr->SetTableAnimSpeed(m_fSpeedRate * pBgChr->GetTableAnimSpeed());
                    }
                    CCourtChr *pCourtChr = (CCourtChr *)pSelectMenu->FindObj(0x3B);
                    if (pCourtChr != 0 && pCourtChr->GetTableAnimSpeed() < m_fSpeedRate) {
                        pCourtChr->SetTableAnimSpeed(m_fSpeedRate * pCourtChr->GetTableAnimSpeed());
                    }
                    pCourtChr = (CCourtChr *)pSelectMenu->FindObj(0x42);
                    if (pCourtChr != 0 && pCourtChr->GetTableAnimSpeed() < m_fSpeedRate) {
                        pCourtChr->SetTableAnimSpeed(m_fSpeedRate * pCourtChr->GetTableAnimSpeed());
                    }
                    m_bSpeedUp = 1;
                    m_bSpeedRequest = 0;
                }
            }
            break;
        }
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_04, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TV_L_00, ANIM_INOUT_04)) {
                m_nIconPos = ICON_HIDDEN;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_HIDE_NOW:
            m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_07, 1.0f, 0.0f, 1, 1);
            m_nIconPos = ICON_SHOWN;
            m_Unk30 = STATE_ACTIVE;
            break;
        case STATE_SWAP:
            m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_06, 1.0f, 0.0f, 1, 1);
            m_nIconPos = ICON_SWAP;
            m_Unk30 = STATE_SWAP_WAIT;
            break;
        case STATE_SWAP_WAIT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_TV_L_00, ANIM_INOUT_06)) {
                m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_05, 1.0f, 0.0f, 1, 1);
                m_nIconPos = ICON_HIDDEN;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CBackIcon::Disp()
{
}

int msw::SelectMenu::CBackIcon::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            static const int isChangeToSideway = 1;
            static const int btnNum = 1;

            for (long i = 0; i < btnNum; i++) {
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
                pPart->m_nAnim[5] = ANIM_BTN_02;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId;
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = 0xBBE;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = isChangeToSideway;
                pPart->m_bUnk40_0020 = 0;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            SetTextBoxFromMessage(pPaneCtrl);
            menu::ReplaceKeyTextByPane(m_pPaneAccessor->GetPane(PANE_T_BACK_ICO_01), 1);

            m_Unk30 = STATE_ACTIVE;
            m_Unk2C = 1;
            m_nMode = 0;
            m_bPushed = 0;
            m_bSpeedRequest = 0;
            m_nIconPos = ICON_HIDDEN;
            m_pPaneAccessor->PlayAnimation(PANE_TV_L_00, ANIM_INOUT_05, 1.0f, 0.0f, 1, 1);
        }
    }
    return 0;
}

void msw::SelectMenu::CBackIcon::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

// Takes the background's title tag and lines away.
inline void msw::SelectMenu::CBackIcon::TagOut(CSelectMenu *pSelectMenu)
{
    if (m_bTagIn == 1) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_OUT, 0);
        m_bTagIn = 0;
    }
    if (m_bLineIn == 1) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_LINE_BOX_02, 0);
        m_bLineIn = 0;
    }
}

// Brings the background's lines in if they are not, then plays line
// animation nEvent.
inline void msw::SelectMenu::CBackIcon::LineIn(long nEvent)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();

    if (m_bLineIn == 0) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_LINE_BOX_00, 0);
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TEXT_LINE_00, 0);
        m_bLineIn = 1;
    }
    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, nEvent, 0);
}

long msw::SelectMenu::CBackIcon::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (m_pPaneAccessor == 0) {
        return -1;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();

    switch (nEvent) {
        case 0x12: {
            // The text of the button.
            const wchar_t *pText = (const wchar_t *)pParam;
            m_pPaneAccessor->SetTextBoxString(PANE_T_BACK_ICO_01, pText, wcslen(pText));
            m_pPaneAccessor->SetTextBoxString(PANE_TS_BACK_ICO_S01, pText, wcslen(pText));
            break;
        }
        case 0x13:
            nResult = HitCheck((bcs::FVEC3 *)pParam);
            break;
        case 0x14:
            break;
        case 0x20: {
            // pParam: the pushed button ids of the four pads; only the
            // leader's counts.
            long nPad = CMenuPadInfo::s_Instance.SelectPadLeader(0);
            long nMode = GetSelectMenu()->m_nMode;
            if (nMode == MODE_ENTRY_PARTY || nMode == MODE_ENTRY || nMode == MODE_WIFI_CHARA_SELECT) {
                nPad = GetSelectMenu()->m_Unk27D;
            }
            unsigned short pushButtonId[4] = { 0, 0, 0, 0 };
            pushButtonId[nPad] = ((const unsigned short *)pParam)[nPad];
            m_pMenuPartList->CheckButtonAnim(pushButtonId, 0);
            nResult = 0;
            break;
        }
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24: {
            unsigned short nPush = (unsigned short)(unsigned long)pParam;
            if (nPush == 1) {
                nResult = PushButton(nEvent - 0x21, nPush);
                m_bPushed = 1;
            }
            break;
        }
        case 0x25:
            TagOut(pSelectMenu);
            m_Unk30 = STATE_CLOSE;
            break;
        case 0x26:
            nResult = m_pMenuPartList->IsPushButtonAnimEnd();
            break;
        case 0x27:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_CLOSE;
            }
            break;
        case 0x28:
            if (m_nIconPos == ICON_HIDDEN) {
                m_Unk30 = STATE_SET;
            }
            break;
        case 0x29:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_SWAP;
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_LINE_BOX_04, 0);
            }
            break;
        case 0x2A:
            if (m_nIconPos == ICON_HIDDEN) {
                m_Unk30 = STATE_HIDE_NOW;
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_LINE_BOX_03, 0);
            }
            break;
        case 0x2B:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_CLOSE;
            }
            TagOut(pSelectMenu);
            break;
        case 0xB:
            LineIn(BG_EVENT_LINE_BOX_03);
            break;
        case 0x2C:
            nResult = m_bPushed;
            break;
        case 0x51:
            m_fSpeedRate = 1.0f;
            m_bSpeedUp = 0;
            break;
        case 0x52:
            m_fSpeedRate = 1.0f;
            m_bSpeedUp = 1;
            break;
        case 0x53:
            nResult = m_fSpeedRate;
            break;
        case 0x54:
            m_Unk30 = STATE_HIDE;
            break;
        case 0x55:
            m_Unk30 = STATE_ACTIVE;
            break;
        case 0x56:
            m_bSpeedRequest = 1;
            break;
        default:
            nResult = 0;
            break;
    }
    return nResult;
}

void msw::SelectMenu::CBackIcon::IndividualSelect(long nMode)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();

    // m_Unk9B is compared as a signed value (cmpwi), as in
    // game/menu/CCourtTab.cpp.
    if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii != 0) {
        if (m_nIconPos == ICON_SHOWN) {
            m_Unk30 = STATE_CLOSE;
        }
        return;
    }

    // Screens past 49 and screen 45 leave the button as it is.
    switch (nMode) {
        case 0:
        case 1:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_CLOSE;
            }
            break;
        case 42:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_SWAP;
            }
            break;
        case 14:
        case 30:
        case 32:
            if ((long)pSelectMenu->m_Unk9B == 4 && pSelectMenu->m_bStarMii != 0) {
                if (m_nIconPos == ICON_SHOWN) {
                    m_Unk30 = STATE_SWAP;
                }
            } else if (m_nIconPos == ICON_HIDDEN) {
                m_Unk30 = STATE_SET;
            }
            break;
        case 16:
        case 44:
            if (pSelectMenu->m_UnkA68 == 0) {
                if (m_nIconPos == ICON_HIDDEN) {
                    m_Unk30 = STATE_SET;
                }
            } else if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_CLOSE;
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 15:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 31:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 43:
        case 46:
        case 47:
        case 48:
            if (m_nIconPos == ICON_HIDDEN) {
                m_Unk30 = STATE_SET;
            }
            break;
        case 33:
        case 49:
            if (m_nIconPos == ICON_SHOWN) {
                m_Unk30 = STATE_SWAP;
            }
            break;
    }
}

// Brings the background's title tag in if it is not.
inline void msw::SelectMenu::CBackIcon::TagIn(CSelectMenu *pSelectMenu)
{
    if (m_bTagIn == 0) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_IN, 0);
        m_bTagIn = 1;
    }
}

// Shows the colour bar of the current game kind (TAG_GAME).
inline void msw::SelectMenu::CBackIcon::SetTagColorByGameKind()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    long nGameKind = pSelectMenu->m_nGameKind;

    if (nGameKind == -1 || nGameKind == 0xFF) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_BASE);
    } else if (nGameKind == 5) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_OTHER);
    } else if ((long)pSelectMenu->m_Unk9B == 2) {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_PARTY);
    } else {
        pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)(nGameKind + 1));
    }
}

void msw::SelectMenu::CBackIcon::TagTitle(long nMode)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();

    // Screens past 45 leave the tag as it is.
    switch (nMode) {
        case 0:
        case 1:
            TagOut(pSelectMenu);
            break;
        case 5:
        case 6:
            TagIn(pSelectMenu);
            if ((long)pSelectMenu->m_Unk9B == 2) {
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_PARTY);
            } else {
                switch (pSelectMenu->m_nGameKind) {
                    case 0:
                        pSelectMenu->SendObjEventTrigger(
                            BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_00_02"));
                        break;
                    case 1:
                        pSelectMenu->SendObjEventTrigger(
                            BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_00_00"));
                        break;
                    case 2:
                        pSelectMenu->SendObjEventTrigger(
                            BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_00_01"));
                        break;
                    case 3:
                        pSelectMenu->SendObjEventTrigger(
                            BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "MODE_00_03"));
                        break;
                }
            }
            if (pSelectMenu->m_nGameKind != 5) {
                SetTagColorByGameKind();
            }
            if (pSelectMenu->m_nGameKind != 5) {
                LineIn(BG_EVENT_LINE_BOX_03);
            }
            break;
        case 9:
            TagIn(pSelectMenu);
            switch (pSelectMenu->m_Unk9B) {
                default:
                    pSelectMenu->SendObjEventTrigger(
                        BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                        (void *)GetMessageLabelString("ExhibitionMenuMessage", "ENT_00_01"));
                    break;
                case 0:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("ExhibitionMenuMessage", "ENT_00"));
                    break;
                case 2:
                    switch (pSelectMenu->m_Unk9C) {
                        case 1:
                            pSelectMenu->SendObjEventTrigger(
                                BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_02"));
                            break;
                        case 5:
                            pSelectMenu->SendObjEventTrigger(
                                BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_03"));
                            break;
                        case 7:
                            pSelectMenu->SendObjEventTrigger(
                                BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_01"));
                            break;
                        case 3:
                            pSelectMenu->SendObjEventTrigger(
                                BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_00"));
                            break;
                    }
                    break;
            }
            SetTagColorByGameKind();
            LineIn(BG_EVENT_LINE_BOX_03);
            break;
        case 12:
            TagIn(pSelectMenu);
            // The game switches on the player count less one.
            switch (pSelectMenu->m_nPlayerNum - 1) {
                case 0:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("ExhibitionMenuMessage", "VS_00_00"));
                    break;
                case 1:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("ExhibitionMenuMessage", "VS_00_01"));
                    break;
                case 2:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("ExhibitionMenuMessage", "VS_00_02"));
                    break;
                case 3:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("ExhibitionMenuMessage", "VS_00_03"));
                    break;
            }
            SetTagColorByGameKind();
            LineIn(BG_EVENT_LINE_BOX_03);
            break;
        case 19:
            TagIn(pSelectMenu);
            switch (pSelectMenu->m_Unk9C) {
                case 1:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_02"));
                    break;
                case 5:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_03"));
                    break;
                case 7:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_01"));
                    break;
                case 3:
                    pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                     (void *)GetMessageLabelString("PartyMenuMessage", "PAR_TUT_00_00"));
                    break;
            }
            SetTagColorByGameKind();
            LineIn(BG_EVENT_LINE_BOX_04);
            break;
        case 4:
        case 14:
        case 33:
        case 37:
        case 38:
        case 39:
        case 42:
            SetTagTitleIn(nMode);
            LineIn(BG_EVENT_LINE_BOX_04);
            break;
        case 26:
            TagIn(pSelectMenu);
            if ((long)pSelectMenu->m_nPlayerNum == 2) {
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                 (void *)GetMessageLabelString("WifiConnectMessage", "WiFi_B_01_01"));
            } else {
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                 (void *)GetMessageLabelString("WifiConnectMessage", "WiFi_B_01_00"));
            }
            pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_WIFI);
            break;
        case 44: {
            TagIn(pSelectMenu);
            int bRR = 0;
            if ((long)pSelectMenu->m_Unk9B == 2 && pSelectMenu->m_Unk9C == 3) {
                bRR = 1;
            }
            if (bRR) {
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                 (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_RR_00"));
            } else {
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_TEXT,
                                                 (void *)GetMessageLabelString("ExhibitionMenuMessage", "BG_PT_00"));
            }
            pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_PARTY);
            LineIn(BG_EVENT_LINE_BOX_03);
            break;
        }
        case 2:
        case 3:
        case 7:
        case 8:
        case 10:
        case 11:
        case 13:
        case 15:
        case 16:
        case 17:
        case 18:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 34:
        case 35:
        case 36:
        case 40:
        case 41:
        case 43:
            if (s_TagTitle[nMode].m_nTagColor != TAG_NONE) {
                SetTagTitleIn(nMode);
                LineIn(BG_EVENT_LINE_BOX_03);
            }
            break;
        case 45:
            TagOut(pSelectMenu);
            break;
    }
}

void msw::SelectMenu::CBackIcon::SetTagTitleIn(long nMode)
{
    CSelectMenu *pSelectMenu = GetSelectMenu();

    TagIn(pSelectMenu);
    pSelectMenu->SendObjEventTrigger(
        BG_OBJ_ID, BG_EVENT_TAG_TEXT,
        (void *)GetMessageLabelString(s_TagTitle[nMode].m_pGroup, s_TagTitle[nMode].m_pLabel));

    if (nMode == MODE_TEAM || nMode == MODE_ENTRY_PARTY || nMode == MODE_ENTRY || nMode == MODE_WIFI_CHARA_SELECT) {
        switch (pSelectMenu->m_Unk9B) {
            case 2:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_PARTY);
                break;
            case 4:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_WIFI);
                break;
        }
    } else {
        switch (s_TagTitle[nMode].m_nTagColor) {
            case 0:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_BASE);
                break;
            case 1:
                SetTagColorByGameKind();
                break;
            case 2:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_PARTY);
                break;
            case 3:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_SCORE);
                break;
            case 4:
                pSelectMenu->SendObjEventTrigger(BG_OBJ_ID, BG_EVENT_TAG_BAR, (void *)TAG_WIFI);
                break;
        }
    }
}

long msw::SelectMenu::CBackIcon::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CBackIcon::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CBackIcon::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CBackIcon::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}

// The base's empty animation callbacks have their only copy here, after
// PushButton and in reverse order. They are probably inline in the base
// class and kept here because this is the first unit of the module that
// needs them; written out of line so that no other unit changes.
void msw::CSelectMenuLayoutBase::CallbackButtonAnimMultiOut(long, long)
{
}

void msw::CSelectMenuLayoutBase::CallbackButtonAnimMultiIn(long, long)
{
}

void msw::CSelectMenuLayoutBase::CallbackButtonAnimCancel(long)
{
}

void msw::CSelectMenuLayoutBase::CallbackButtonAnimPush(long)
{
}

void msw::CSelectMenuLayoutBase::CallbackButtonAnimOut(long)
{
}

void msw::CSelectMenuLayoutBase::CallbackButtonAnimIn(long)
{
}

long msw::SelectMenu::CBackIcon::SetDefaultMoveCursor(long, long)
{
    return 0;
}

long msw::SelectMenu::CBackIcon::StickMoveCheck(long, long)
{
    return 0;
}

long msw::SelectMenu::CBackIcon::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CBackIcon::SetCursorMarkerPosition(long, long)
{
    return 0;
}
