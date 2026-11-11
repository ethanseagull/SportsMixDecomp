#include "game/msw/SelectMenu/CRuleSel.hpp"

#include "bcs/CMainApp.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CEntry.cpp: the original converts the
    // value to float at run time and keeps the word first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_RULE_SEL = 0x4C
    };

    // Events sent to the menu objects.
    enum {
        EVENT_LINE_MESSAGE = 0x11, // to OBJ_TOP: the help line (0: none)
        EVENT_HIT = 0x13,          // to OBJ_UNK_01
        EVENT_PUSH = 0x21,         // to OBJ_UNK_01, plus the pad number
        EVENT_CLOSE = 0x2E         // to the rules screen
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_PAGE_NEXT = 0x40,
        PAD_PAGE_PREV = 0x80,
        PAD_POINTER = 0x100
    };

    // The button kind ids of the rules screen. Each row has a button and
    // two arrows; the rows of the first page are 0x8020 to 0x8024 (arrows
    // 0x8030 to 0x8039), those of the second 0x8010 to 0x8014 (arrows
    // 0x8040 to 0x8049), the left arrow even, the right one odd.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_ROW_PAGE1 = 0x8010,
        BTN_KIND_ROW_PAGE0 = 0x8020,
        BTN_KIND_ARROW_PAGE0 = 0x8030,
        BTN_KIND_ARROW_PAGE1 = 0x8040,
        BTN_KIND_OK = 0x8050,       // one per page: 0x8050, 0x8052
        BTN_KIND_RESET = 0x8051,    // 0x8051, 0x8053
        BTN_KIND_OK_2 = 0x8052,
        BTN_KIND_RESET_2 = 0x8053,
        BTN_KIND_PAGE0 = 0x8060,    // the two page tabs
        BTN_KIND_PAGE1 = 0x8061,
        BTN_KIND_PAGE0_2 = 0x8062, // and two more on the second page
        BTN_KIND_PAGE1_2 = 0x8063
    };

    // Values of CSelectMenu::m_Unk9B: where the rules screen was entered
    // from. The game compares the byte as a signed word, hence the casts.
    enum {
        ENTRY_EXHIBITION = 0,
        ENTRY_TOURNAMENT = 1,
        ENTRY_PARTY = 2
    };

    // CSelectMenu::m_nGameKind of the mix tournament.
    enum {
        GAME_KIND_MIX = 5
    };

    // The music of the menus (CSelectMenu::PlayMenuBGM).
    enum {
        BGM_MENU = 1,
        BGM_RULE = 6
    };

    // Frames between two steps of a setting, and after a page change.
    const float RULE_WAIT = 10.0f;
    const float PAGE_WAIT = 20.0f;

    // The button, right arrow and left arrow of each row.
    long s_RowButton[msw::SelectMenu::CRuleSel::OPTION_MAX][3] = {
        { 0x8020, 0x8031, 0x8030 }, { 0x8021, 0x8033, 0x8032 }, { 0x8022, 0x8035, 0x8034 },
        { 0x8023, 0x8037, 0x8036 }, { 0x8024, 0x8039, 0x8038 }, { 0x8010, 0x8041, 0x8040 },
        { 0x8011, 0x8043, 0x8042 }, { 0x8012, 0x8045, 0x8044 }, { 0x8013, 0x8047, 0x8046 },
        { 0x8014, 0x8049, 0x8048 }
    };
}

void msw::CSelectMenu::RuleSel_In()
{
    static const long padNo0 = 0;

    long nLevel = CSave::GetPlayerData(-1)->IsDifficult(m_nGameKind);
    CMenuOption::s_Instance.GetPlayerSaveData(CSave::GetUsePlayerID(), nLevel);
    CMenuOption::s_Instance.AllocTemporary();
    CMenuOption::s_Instance.BackupTemporary();
    Build(OBJ_RULE_SEL);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    SelectMenu::CRuleSel *pObj = (SelectMenu::CRuleSel *)FindObj(OBJ_RULE_SEL);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        if (m_Unk23E == 1) {
            RuleSel_SetRuleText(pObj->GetRotateButtonId(nBtnKindId), 0);
        } else {
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, 0);
        }
    }

    PlayMenuBGM(BGM_RULE);
    SetFadeIn(10.0f);
    m_fRuleInputWait = 0.0f;
    m_fInputWait = 0.0f;
    m_nStep = 0;
}

void msw::CSelectMenu::RuleSel_Loop()
{
    // static-order swap, as in game/menu/CTeam.cpp (see notes/decisions.md).
    static const long objId = OBJ_RULE_SEL;
    static const long padNo0 = 0;

    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();
    long nMode = m_nMode;
    CMenuOption &rOption = CMenuOption::s_Instance;
    SelectMenu::CRuleSel *pObj = (SelectMenu::CRuleSel *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    m_fRuleInputWait -= GetFrameRateF();
    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait <= 0.0f && m_fRuleInputWait <= 0.0f) {
        m_fRuleInputWait = 0.0f;
        m_fInputWait = 0.0f;

        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_POINTER) {
            switch (nBtnKindId) {
                case 0x8031:
                case 0x8033:
                case 0x8035:
                case 0x8037:
                case 0x8039:
                case 0x8041:
                case 0x8043:
                case 0x8045:
                case 0x8047:
                case 0x8049:
                    if (m_fRuleInputWait <= 0.0f) {
                        m_fRuleInputWait = RULE_WAIT;
                        if (RuleSel_SetRuleText(pObj->GetRotateButtonId(nBtnKindId), 1)) {
                            pObj->PushButton(padNo0, nBtnKindId);
                            pObj->SetRuleState(0);
                        }
                    }
                    return;
                case 0x8030:
                case 0x8032:
                case 0x8034:
                case 0x8036:
                case 0x8038:
                case 0x8040:
                case 0x8042:
                case 0x8044:
                case 0x8046:
                case 0x8048:
                    if (m_fRuleInputWait <= 0.0f) {
                        m_fRuleInputWait = RULE_WAIT;
                        if (RuleSel_SetRuleText(pObj->GetRotateButtonId(nBtnKindId), -1)) {
                            pObj->PushButton(padNo0, nBtnKindId);
                            pObj->SetRuleState(0);
                        }
                    }
                    return;
                case BTN_KIND_PAGE1:
                    m_fInputWait = PAGE_WAIT;
                    pObj->PushButton(padNo0, nBtnKindId);
                    pObj->PageChange(1);
                    return;
                case BTN_KIND_PAGE0:
                    m_fInputWait = PAGE_WAIT;
                    pObj->PushButton(padNo0, nBtnKindId);
                    pObj->PageChange(0);
                    return;
            }
        }

        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
            switch (nBtnKindId) {
                case BTN_KIND_OK:
                case BTN_KIND_OK_2:
                    pObj->PushButton(padNo0, nBtnKindId);
                    // OK does nothing until a setting has been changed or reset.
                    if (pObj->m_bUnchanged == 0) {
                        PlayMenuBGM(BGM_MENU);
                        GoBackToMode(padNo0, GetPrevMode(), 0, 1);
                        pObj->SendEvent(EVENT_CLOSE, 0);
                    }
                    break;
                case BTN_KIND_RESET:
                case BTN_KIND_RESET_2:
                    pObj->PushButton(padNo0, nBtnKindId);
                    pObj->SetRuleState(0);
                    RuleSel_ResetRule();
                    break;
                case BTN_KIND_BACK:
                    pObj->PushButton(padNo0, nBtnKindId);
                    rOption.RestoreTemporary();
                    PlayMenuBGM(BGM_MENU);
                    GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                    break;
            }
            return;
        }

        if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) && nBtnKindId < BTN_KIND_OK) {
            if (m_fRuleInputWait <= 0.0f) {
                long nOption = pObj->GetRotateButtonId(nBtnKindId);
                if (nOption >= 0) {
                    m_fRuleInputWait = RULE_WAIT;
                    pObj->PushButton(padNo0, s_RowButton[pObj->GetOptionRow(nOption)][1]);
                    if (RuleSel_SetRuleText(nOption, 1)) {
                        pObj->SetRuleState(0);
                    }
                    return;
                }
            }
        } else if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) && nBtnKindId < BTN_KIND_OK) {
            if (m_fRuleInputWait <= 0.0f) {
                long nOption = pObj->GetRotateButtonId(nBtnKindId);
                if (nOption >= 0) {
                    m_fRuleInputWait = RULE_WAIT;
                    pObj->PushButton(padNo0, s_RowButton[pObj->GetOptionRow(nOption)][2]);
                    if (RuleSel_SetRuleText(nOption, -1)) {
                        pObj->SetRuleState(0);
                    }
                    return;
                }
            }
        } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_PAGE_NEXT) {
            m_fInputWait = PAGE_WAIT;
            pObj->PageChange(1);
            pObj->PushButton(padNo0, BTN_KIND_PAGE1);
        } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_PAGE_PREV) {
            m_fInputWait = PAGE_WAIT;
            pObj->PageChange(0);
            pObj->PushButton(padNo0, BTN_KIND_PAGE0);
        } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
            rOption.RestoreTemporary();
            PlayMenuBGM(BGM_MENU);
            GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            return;
        }
    }

    InputDataToMenuObj(pObj, padNo0, nPad, nMode);

    // A new button: show its help line.
    long nNewBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (nBtnKindId != nNewBtnKindId) {
        if (nNewBtnKindId == BTN_KIND_OK || nNewBtnKindId == BTN_KIND_OK_2) {
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                                (void *)GetMessageLabelString("OptionRuleList", "OptionRule_LMes_10"));
        } else if (nNewBtnKindId == BTN_KIND_RESET || nNewBtnKindId == BTN_KIND_RESET_2) {
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                                (void *)GetMessageLabelString("OptionRuleList", "OptionRule_LMes_11"));
        } else if (nNewBtnKindId == 0 || nNewBtnKindId == BTN_KIND_BACK) {
            // No button, or the back button: no help line.
            SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE, 0);
        } else {
            long nOption = pObj->GetRotateButtonId(nNewBtnKindId);
            if (nOption >= 0) {
                RuleSel_SetRuleText(nOption, 0);
            }
        }
    }
}

long msw::CSelectMenu::RuleSel_GetOptionKindId()
{
    long nKind;
    if ((long)m_Unk9B == ENTRY_PARTY) {
        switch (m_Unk9C) {
            default:
                nKind = 4;
                break;
            case 2:
                nKind = 5;
                break;
            case 3:
                nKind = 6;
                break;
            case 4:
                nKind = 7;
                break;
            case 5:
                nKind = 8;
                break;
            case 6:
                nKind = 9;
                break;
            case 7:
                nKind = 10;
                break;
            case 8:
                nKind = 11;
                break;
        }
    } else if ((long)m_Unk9B == ENTRY_TOURNAMENT) {
        switch (m_nGameKind) {
            default:
                nKind = 12;
                break;
            case 1:
                nKind = 13;
                break;
            case 2:
                nKind = 14;
                break;
            case 3:
                nKind = 15;
                break;
            case GAME_KIND_MIX:
                nKind = 16;
                break;
        }
    } else {
        switch (m_nGameKind) {
            default:
                nKind = 0;
                break;
            case 1:
                nKind = 1;
                break;
            case 2:
                nKind = 2;
                break;
            case 3:
                nKind = 3;
                break;
        }
    }
    return nKind;
}

void msw::CSelectMenu::RuleSel_Out()
{
    // Declared first: the game keeps it in the register `this` had.
    long nLevel;

    Scratch(OBJ_RULE_SEL);
    CMenuOption::s_Instance.FreeTemporary();

    // Which set of saved options: exhibition, tournament, mix tournament.
    long nType = 0;
    if ((long)m_Unk9B == ENTRY_TOURNAMENT) {
        nType = (m_nGameKind == GAME_KIND_MIX) + 1;
    }
    nLevel = CSave::GetPlayerData(-1)->IsDifficult(m_nGameKind);
    CMenuOption::s_Instance.SetPlayerSaveData(CSave::GetUsePlayerID(), nType, nLevel);
}

int msw::CSelectMenu::RuleSel_SetRuleText(long nOption, long nStep)
{
    int bChanged = 0;
    if (nOption < 0) {
        return 0;
    }

    SelectMenu::CRuleSel *pObj = (SelectMenu::CRuleSel *)FindObj(OBJ_RULE_SEL);
    if (pObj == 0) {
        return 0;
    }

    CMenuOption &rOption = CMenuOption::s_Instance;
    long nKind = RuleSel_GetOptionKindId();
    // The result is not used.
    rOption.GetMenuNum(nKind);
    long nNum = rOption.GetMenuParameterNum(nKind, nOption);
    long nId = rOption.GetMenuParameterId(nKind, nOption);
    if (nStep != 0) {
        long nOldId = nId;
        nId += nStep;
        if (nId >= nNum - 1) {
            nId = nNum - 1;
            pObj->SetArrowWait(pObj->GetOptionRow(nOption), 0, 1);
        } else if (nId <= 0) {
            nId = 0;
            pObj->SetArrowWait(pObj->GetOptionRow(nOption), 1, 0);
        } else {
            pObj->SetArrowWait(pObj->GetOptionRow(nOption), 0, 0);
        }
        rOption.SetMenuParameterId(nKind, nOption, nId);
        if (nOldId != nId) {
            bChanged = 1;
        }
    }

    SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                        (void *)rOption.GetMenuParameterTelop(nKind, nOption, nId));
    pObj->SetOptionText();
    return bChanged;
}

void msw::CSelectMenu::RuleSel_ResetRule()
{
    // Declared first: the game keeps it in the first saved register.
    long nNum;
    CMenuOption &rOption = CMenuOption::s_Instance;
    long nKind = RuleSel_GetOptionKindId();
    nNum = rOption.GetMenuNum(nKind);
    CSave::PlayerData *pData = CSave::GetPlayerData(-1);
    for (long i = 0; i < nNum; i++) {
        rOption.SetMenuParameterId(
            nKind, i, CMenuOption::GetMenuDefaultId(nKind, i, pData->IsDifficult(m_nGameKind)));
    }

    SelectMenu::CRuleSel *pObj = (SelectMenu::CRuleSel *)FindObj(OBJ_RULE_SEL);
    if (pObj != 0) {
        pObj->SetOptionText();
    }
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    // The panes of the window, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        PANE_WIN = 0,
        PANE_PAGE0 = 2,         // the window of each page
        PANE_UNK_04 = 4,
        PANE_UNK_06 = 6,
        PANE_PAGE1 = 8,
        PANE_UNK_0A = 0xA,
        PANE_OK = 0xC,
        PANE_PAGE_PREV = 0x2E,  // the page arrows
        PANE_PAGE_NEXT = 0x30,
        PANE_OK_2 = 0x31,
        PANE_PAGE_ICON = 0x60,
        PANE_PAGE_TEXT = 0x61
    };

    // The animations played on them.
    enum {
        ANIM_PAGE_ARROW_ON = 0x16,
        ANIM_PAGE_ARROW_OFF = 0x17,
        ANIM_WIN_IN = 9,
        ANIM_WIN_OUT = 0xA,
        ANIM_PAGE_ICON = 0xB,
        ANIM_PAGE_ICON_OUT = 0xD,
        ANIM_WIN_IN_BACK = 0xF,
        ANIM_WIN_OUT_BACK = 0x10,
        ANIM_OK_CHANGED = 5,
        ANIM_OK_UNCHANGED = 6,
        ANIM_COLOR = 8
    };

    // System sounds of the buttons (CMenuPartBase::m_nSeCode).
    enum {
        SE_NONE = -1,
        SE_DECIDE = 0xBBB,
        SE_ARROW_OFF = 0xBBE,
        SE_BUZZER = 0xBBF,
        SE_CURSOR = 0xBC0,
        SE_ARROW = 0xBC1,
        SE_PAGE = 0xBCD,
        SE_PAGE_PREV = 0xBCE
    };

    const char *const s_LayoutName = "me_opt.brlyt";
    const char *const s_ButtonFileName = "me_opt.btn";

    // The text panes of each row: the option's name, its setting.
    long s_NamePane[msw::SelectMenu::CRuleSel::OPTION_MAX] = {
        0x16, 0x1C, 0x22, 0x28, 0x2D, 0x3B, 0x41, 0x47, 0x4D, 0x52
    };
    long s_ValuePane[msw::SelectMenu::CRuleSel::OPTION_MAX] = {
        0x11, 0x17, 0x1D, 0x23, 0x29, 0x36, 0x3C, 0x42, 0x48, 0x4E
    };
    // The button of each row, as in s_RowButton.
    long s_RowBtnKindId[msw::SelectMenu::CRuleSel::OPTION_MAX] = {
        0x8020, 0x8021, 0x8022, 0x8023, 0x8024, 0x8010, 0x8011, 0x8012, 0x8013, 0x8014
    };
    // The left and right arrow of each row.
    long s_ArrowBtnKindId[msw::SelectMenu::CRuleSel::OPTION_MAX][2] = {
        { 0x8030, 0x8031 }, { 0x8032, 0x8033 }, { 0x8034, 0x8035 }, { 0x8036, 0x8037 },
        { 0x8038, 0x8039 }, { 0x8040, 0x8041 }, { 0x8042, 0x8043 }, { 0x8044, 0x8045 },
        { 0x8046, 0x8047 }, { 0x8048, 0x8049 }
    };

    // The opening animation of a page's window by its number of options.
    long s_PageAnim[msw::SelectMenu::CRuleSel::PAGE_OPTION_NUM] = { 0x15, 0x14, 0x13, 0x12, 0x11 };

    // The options shown on each row (-1: an empty row), by rule set: by
    // game kind for an exhibition match, by party game, by game kind for
    // a tournament. Separate objects, not one table: the game addresses
    // them from the start of the section.
    long s_OptionsExhibition0[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, 1, 2, 3, -1, -1, -1, -1, -1, -1 };
    long s_OptionsExhibition1[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, 1, 2, -1, -1, 3, 4, -1, -1, -1 };
    long s_OptionsExhibition2[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, 1, 2, 3, 4, 5, -1, -1, -1, -1 };
    long s_OptionsExhibition3[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, 1, 2, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsParty[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsParty3[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsParty5[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsParty7[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsTournament0[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsTournament1[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsTournament2[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsTournament3[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    long s_OptionsTournamentMix[msw::SelectMenu::CRuleSel::OPTION_MAX] = { 0, 1, -1, -1, -1, -1, -1, -1, -1, -1 };
}

inline void msw::SelectMenu::CRuleSel::SetRuleState(int bUnchanged)
{
    m_bUnchanged = bUnchanged;
    m_pPaneAccessor->PlayAnimation(PANE_OK, bUnchanged ? ANIM_OK_UNCHANGED : ANIM_OK_CHANGED, 1.0f, 0.0f, 1, 0);
    m_pPaneAccessor->PlayAnimation(PANE_OK_2, bUnchanged ? ANIM_OK_UNCHANGED : ANIM_OK_CHANGED, 1.0f, 0.0f, 1, 0);
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_OK);
    pPart->m_nSeCode[0] = SE_CURSOR;
    pPart->m_nSeCode[1] = bUnchanged ? SE_BUZZER : SE_DECIDE;
}

inline void msw::SelectMenu::CRuleSel::SetOptionText()
{
    CMenuOption &rOption = CMenuOption::s_Instance;
    long nKind = GetSelectMenu()->RuleSel_GetOptionKindId();
    long nOption;
    for (long i = 0; i < OPTION_MAX; i++) {
        nOption = GetRotateButtonId(i);
        if (nOption >= 0) {
            const wchar_t *pName = rOption.GetMenuName(nKind, nOption);
            m_pPaneAccessor->SetTextBoxString(s_NamePane[i], pName, wcslen(pName));
            const wchar_t *pText = rOption.GetMenuParameterText(
                nKind, nOption, rOption.GetMenuParameterId(nKind, nOption));
            m_pPaneAccessor->SetTextBoxString(s_ValuePane[i], pText, wcslen(pText));
        }
    }
}

inline void msw::SelectMenu::CRuleSel::GetPageAnim(long *pAnim)
{
    pAnim[0] = -1;
    pAnim[1] = -1;
    for (long i = 0; i < PAGE_OPTION_NUM; i++) {
        if (m_nOptionId[i] == -1) {
            break;
        }
        pAnim[0] = s_PageAnim[i];
    }
    for (long i = PAGE_OPTION_NUM; i < OPTION_MAX; i++) {
        if (m_nOptionId[i] == -1) {
            break;
        }
        pAnim[1] = s_PageAnim[i - PAGE_OPTION_NUM];
    }
}

msw::SelectMenu::CRuleSel::CRuleSel(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CRuleSel::~CRuleSel()
{
    Remove();
}

void msw::SelectMenu::CRuleSel::Main()
{
    // static-order swap, as in game/menu/CTeam.cpp (see notes/decisions.md).
    static const int reset = 1;
    static const int recursive = 0;

    switch (m_Unk30) {
        case STATE_SET:
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_PAGE0, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_UNK_04, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                if (m_bPageText) {
                    m_pPaneAccessor->PlayAnimation(PANE_PAGE_ICON, ANIM_PAGE_ICON, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_PAGE0, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_UNK_04, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                if (m_bPageText) {
                    m_pPaneAccessor->PlayAnimation(PANE_PAGE_ICON, ANIM_PAGE_ICON, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
            }
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    bEnd = 1;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                bEnd = 1;
            }
            if (bEnd) {
                m_Unk30 = STATE_ACTIVE;
                m_nOutPane[0] = PANE_WIN;
                m_nOutPane[1] = PANE_PAGE0;
                m_nOutPane[2] = PANE_UNK_04;
            }
            break;
        }
        case STATE_ACTIVE: {
            CheckButtonAnim();
            CMenuPartBase *pPage0 = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE0);
            CMenuPartBase *pPage1 = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE1);
            CMenuPartBase *pPage0b = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE0_2);
            CMenuPartBase *pPage1b = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE1_2);
            if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
                pPage0->m_nSeCode[1] = SE_PAGE_PREV;
                pPage1->m_nSeCode[1] = SE_PAGE;
                pPage0b->m_nSeCode[1] = SE_PAGE_PREV;
                pPage1b->m_nSeCode[1] = SE_PAGE;
            } else {
                pPage0->m_nSeCode[1] = SE_PAGE;
                pPage1->m_nSeCode[1] = SE_PAGE;
                pPage0b->m_nSeCode[1] = SE_PAGE;
                pPage1b->m_nSeCode[1] = SE_PAGE;
            }
            break;
        }
        case STATE_CHANGE:
            if (m_nArrowRow >= 0) {
                // A setting has changed: update its arrows.
                m_fWait -= bcs::CMainApp::GetAppFrameRateF();
                if (m_fWait <= 0.0f) {
                    SetArrowDisable(m_nArrowRow, 0, m_bArrowOff[0]);
                    SetArrowDisable(m_nArrowRow, 1, m_bArrowOff[1]);
                    m_nArrowRow = -1;
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                // The page has changed: wait for its animations.
                int bEnd = 0;
                int bPrevEnd = m_pPaneAccessor->IsAnimEnd(PANE_PAGE_PREV, 4);
                int bNextEnd = m_pPaneAccessor->IsAnimEnd(PANE_PAGE_NEXT, 4);
                if (m_nPage == 0) {
                    if (m_pPaneAccessor->IsAnimEnd(m_nOutPane[0], ANIM_WIN_IN_BACK) && bPrevEnd && bNextEnd) {
                        bEnd = 1;
                    }
                } else if (m_pPaneAccessor->IsAnimEnd(m_nOutPane[0], ANIM_WIN_IN) && bPrevEnd && bNextEnd) {
                    bEnd = 1;
                }
                if (bEnd) {
                    SetArrow();
                    m_fWait -= bcs::CMainApp::GetAppFrameRateF();
                    if (m_fWait <= 0.0f) {
                        m_Unk30 = STATE_ACTIVE;
                    }
                }
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[0], ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[1], ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[2], ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                } else {
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[0], ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[1], ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                    m_pPaneAccessor->PlayAnimation(m_nOutPane[2], ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                }
                if (m_bPageText) {
                    m_pPaneAccessor->PlayAnimation(PANE_PAGE_ICON, ANIM_PAGE_ICON_OUT, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(m_nOutPane[0], ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else if (m_pPaneAccessor->IsAnimEnd(m_nOutPane[0], ANIM_WIN_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CRuleSel::Disp()
{
}

int msw::SelectMenu::CRuleSel::Set()
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
                int nBtnKindId = SetUpButton(pPart, i, 1);
                // Only the stick moves between these buttons.
                if ((unsigned short)(nBtnKindId - 0x30) <= 0x1F || (unsigned short)nBtnKindId >= 0x60) {
                    pPart->m_bUnk40_0020 = 0;
                }
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            m_pPaneAccessor->PlayAnimation(PANE_UNK_06, ANIM_COLOR, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_PAGE1, ANIM_COLOR, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_UNK_0A, ANIM_COLOR, 1.0f, 0.0f, 1, 0);

            // The options of the rule set.
            CSave::PlayerData *pData = CSave::GetPlayerData(-1);
            if ((long)pSelectMenu->m_Unk9B == ENTRY_PARTY) {
                switch (pSelectMenu->m_Unk9C) {
                    default:
                        memcpy(m_nOptionId, s_OptionsParty, sizeof(m_nOptionId));
                        break;
                    case 3:
                        memcpy(m_nOptionId, s_OptionsParty3, sizeof(m_nOptionId));
                        break;
                    case 5:
                        memcpy(m_nOptionId, s_OptionsParty5, sizeof(m_nOptionId));
                        break;
                    case 7:
                        memcpy(m_nOptionId, s_OptionsParty7, sizeof(m_nOptionId));
                        break;
                }
            } else if ((long)pSelectMenu->m_Unk9B == ENTRY_TOURNAMENT) {
                switch (pSelectMenu->m_nGameKind) {
                    default:
                        memcpy(m_nOptionId, s_OptionsTournament0, sizeof(m_nOptionId));
                        break;
                    case 1:
                        memcpy(m_nOptionId, s_OptionsTournament1, sizeof(m_nOptionId));
                        break;
                    case 2:
                        memcpy(m_nOptionId, s_OptionsTournament2, sizeof(m_nOptionId));
                        break;
                    case 3:
                        memcpy(m_nOptionId, s_OptionsTournament3, sizeof(m_nOptionId));
                        if ((long)pSelectMenu->m_nPlayerNum == 1 && pSelectMenu->m_nGameKind != 2) {
                            m_nOptionId[1] = 1;
                        }
                        break;
                    case GAME_KIND_MIX:
                        memcpy(m_nOptionId, s_OptionsTournamentMix, sizeof(m_nOptionId));
                        if ((long)pSelectMenu->m_nPlayerNum == 1) {
                            m_nOptionId[2] = 2;
                        }
                        break;
                }
            } else {
                switch (pSelectMenu->m_nGameKind) {
                    default:
                        memcpy(m_nOptionId, s_OptionsExhibition0, sizeof(m_nOptionId));
                        if (pSelectMenu->CheckCharChange()) {
                            m_nOptionId[5] = 4;
                        }
                        if (!pData->IsDifficult(pSelectMenu->m_nGameKind)) {
                            if (m_nOptionId[5] == -1) {
                                m_nOptionId[5] = 6;
                            } else if (m_nOptionId[6] == -1) {
                                m_nOptionId[6] = 6;
                            } else {
                                m_nOptionId[7] = 6;
                            }
                        }
                        break;
                    case 1:
                        memcpy(m_nOptionId, s_OptionsExhibition1, sizeof(m_nOptionId));
                        if (!pData->IsDifficult(pSelectMenu->m_nGameKind)) {
                            m_nOptionId[7] = 5;
                        }
                        break;
                    case 2:
                        memcpy(m_nOptionId, s_OptionsExhibition2, sizeof(m_nOptionId));
                        if (pSelectMenu->CheckCharChange()) {
                            m_nOptionId[6] = 6;
                        }
                        if (!pData->IsDifficult(pSelectMenu->m_nGameKind)) {
                            if (m_nOptionId[6] == -1) {
                                m_nOptionId[6] = 7;
                            } else {
                                m_nOptionId[7] = 7;
                            }
                        }
                        break;
                    case 3:
                        memcpy(m_nOptionId, s_OptionsExhibition3, sizeof(m_nOptionId));
                        if (pSelectMenu->CheckCharChange()) {
                            m_nOptionId[5] = 3;
                        }
                        if ((long)pSelectMenu->m_nPlayerNum == 1) {
                            if (m_nOptionId[5] == -1) {
                                m_nOptionId[5] = 4;
                            } else {
                                m_nOptionId[6] = 4;
                            }
                        }
                        if (!pData->IsDifficult(pSelectMenu->m_nGameKind)) {
                            if (m_nOptionId[5] == -1) {
                                m_nOptionId[5] = 5;
                            } else if (m_nOptionId[6] == -1) {
                                m_nOptionId[6] = 5;
                            } else {
                                m_nOptionId[7] = 5;
                            }
                        }
                        break;
                }
            }

            m_nPage = 0;
            SetArrow();

            long anPageAnim[2];
            GetPageAnim(anPageAnim);
            if (anPageAnim[1] == -1) {
                // One page only.
                m_bPageText = 0;
                m_pPaneAccessor->SetPaneDisp(PANE_PAGE_ICON, 0);
            } else {
                m_bPageText = 1;
                const wchar_t *pText = GetMessageLabelString("OptionRuleList", "OptionRule_LMes_12");
                m_pPaneAccessor->SetTextBoxString(PANE_PAGE_TEXT, pText, wcslen(pText));
            }

            SetRuleState(1);
            m_nArrowRow = -1;
            SetUpOprion();
        }
    }
    return 0;
}

void msw::SelectMenu::CRuleSel::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CRuleSel::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);
    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CRuleSel::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleSel::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleSel::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleSel::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CRuleSel::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
    if (m_nPage != 0) {
        // From a row of the first page to the last shown row of the
        // second.
        if (nNext >= BTN_KIND_ROW_PAGE0 && nNext <= BTN_KIND_ROW_PAGE0 + 4) {
            for (nNext -= BTN_KIND_ROW_PAGE0 - BTN_KIND_ROW_PAGE1; nNext > BTN_KIND_ROW_PAGE1; nNext--) {
                if (!(m_pMenuPartList->FindObjectByCurrentBtnKindId(nNext)->m_nFlags40 & 1)) {
                    break;
                }
            }
        }
        // And to the second page's OK and reset buttons.
        if (nNext >= BTN_KIND_OK_2 && nNext <= BTN_KIND_RESET_2) {
            nNext += 2;
        }
    }
    return nNext;
}

long msw::SelectMenu::CRuleSel::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_ROW_PAGE0;
}

int msw::SelectMenu::CRuleSel::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CRuleSel::PushButton(long nPad, long nBtnKindId)
{
    // OK does nothing until a setting has been changed or reset.
    if (m_bUnchanged != 0 && (nBtnKindId == BTN_KIND_OK || nBtnKindId == BTN_KIND_OK_2)) {
        return 0;
    }

    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
        if ((nBtnKindId >= BTN_KIND_ARROW_PAGE0 && nBtnKindId <= BTN_KIND_ARROW_PAGE0 + 9) ||
            (nBtnKindId >= BTN_KIND_ARROW_PAGE1 && nBtnKindId <= BTN_KIND_ARROW_PAGE1 + 9)) {
            if (CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
                pPart->SetButtonIn();
            } else {
                pPart->ClearPadNoBits();
            }
        }
        if (nBtnKindId >= BTN_KIND_PAGE0 && nBtnKindId <= BTN_KIND_PAGE1) {
            pPart->ClearPadNoBits();
        }
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined down here because its local
// static follows Set()'s constants in .rodata.
void msw::SelectMenu::CRuleSel::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CRuleSel::SetUpOprion()
{
    CMenuOption &rOption = CMenuOption::s_Instance;
    long nKind = GetSelectMenu()->RuleSel_GetOptionKindId();

    long anPageAnim[2];
    GetPageAnim(anPageAnim);
    if (anPageAnim[0] > 0) {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE0, anPageAnim[0], 1.0f, 0.0f, 1, 0);
    }
    if (anPageAnim[1] > 0) {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE1, anPageAnim[1], 1.0f, 0.0f, 1, 0);
    }

    for (long i = 0; i < OPTION_MAX; i++) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(s_RowBtnKindId[i]);
        long nOption = GetRotateButtonId(i);
        if (nOption >= 0) {
            pPart->m_nFlags40 &= ~1;
            const wchar_t *pName = rOption.GetMenuName(nKind, nOption);
            m_pPaneAccessor->SetTextBoxString(s_NamePane[i], pName, wcslen(pName));

            // No arrow past the first or the last setting.
            long nNum = rOption.GetMenuParameterNum(nKind, nOption);
            long nId = rOption.GetMenuParameterId(nKind, nOption);
            if (nId >= nNum - 1) {
                SetArrowDisable(i, 1, 1);
            } else if (nId <= 0) {
                SetArrowDisable(i, 0, 1);
            }

            const wchar_t *pText = rOption.GetMenuParameterText(nKind, nOption, nId);
            m_pPaneAccessor->SetTextBoxString(s_ValuePane[i], pText, wcslen(pText));
        } else {
            // An empty row.
            pPart->m_nFlags40 |= 1;
        }
    }
}

long msw::SelectMenu::CRuleSel::GetRotateButtonId(long nBtnKindId)
{
    long nRow = nBtnKindId;
    if (nRow >= BTN_KIND_ROW_PAGE0 && nRow <= BTN_KIND_ROW_PAGE0 + 4) {
        nRow -= BTN_KIND_ROW_PAGE0;
    }
    if (nRow >= BTN_KIND_ROW_PAGE1 && nRow <= BTN_KIND_ROW_PAGE1 + 4) {
        nRow -= BTN_KIND_ROW_PAGE1 - PAGE_OPTION_NUM;
    }
    if (nRow >= BTN_KIND_ARROW_PAGE0 && nRow <= BTN_KIND_ARROW_PAGE0 + 9) {
        nRow = (nRow & 0xFFFE) - BTN_KIND_ARROW_PAGE0;
        nRow >>= 1;
    }
    if (nRow >= BTN_KIND_ARROW_PAGE1 && nRow <= BTN_KIND_ARROW_PAGE1 + 9) {
        nRow = (nRow & 0xFFFE) - BTN_KIND_ARROW_PAGE1;
        nRow = (nRow >> 1) + PAGE_OPTION_NUM;
    }
    if (nRow > OPTION_MAX) {
        return -1;
    }
    return m_nOptionId[nRow];
}

void msw::SelectMenu::CRuleSel::PageChange(int nPage)
{
    if (nPage == 1 && m_nPage == 1) {
        return;
    }
    if (nPage == 0 && m_nPage == 0) {
        return;
    }

    long anPageAnim[2];
    GetPageAnim(anPageAnim);
    // No second page to go to.
    if (nPage == 1 && anPageAnim[1] < 0) {
        return;
    }

    long nBtnKindId;
    if (nPage == 0) {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE0, ANIM_WIN_IN_BACK, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_PAGE1, ANIM_WIN_OUT_BACK, 1.0f, 0.0f, 1, 0);
        const wchar_t *pText = GetMessageLabelString("OptionRuleList", "OptionRule_LMes_12");
        m_pPaneAccessor->SetTextBoxString(PANE_PAGE_TEXT, pText, wcslen(pText));
        nBtnKindId = BTN_KIND_ROW_PAGE0;
        m_nOutPane[1] = PANE_PAGE0;
        m_nPage = 0;
    } else {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE0, ANIM_WIN_OUT, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_PAGE1, ANIM_WIN_IN, 1.0f, 0.0f, 1, 0);
        const wchar_t *pText = GetMessageLabelString("OptionRuleList", "OptionRule_LMes_13");
        m_pPaneAccessor->SetTextBoxString(PANE_PAGE_TEXT, pText, wcslen(pText));
        nBtnKindId = BTN_KIND_ROW_PAGE1;
        m_nOutPane[1] = PANE_PAGE1;
        m_nPage = 1;
    }

    // The cursor goes to the first row of the page.
    CSelectMenu *pSelectMenu = GetSelectMenu();
    pSelectMenu->m_nCurrentBtnKindId[0][pSelectMenu->m_nMode] = nBtnKindId;
    if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
        m_nPushButtonId[0] = nBtnKindId;
        SetCursorMarkerPosition(0, nBtnKindId);
    }
    m_fWait = 2.0f;
    m_Unk30 = STATE_CHANGE;
}

void msw::SelectMenu::CRuleSel::SetArrow()
{
    CMenuPartBase *pPrev = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE0);
    CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE1);

    long anPageAnim[2];
    GetPageAnim(anPageAnim);
    if (anPageAnim[1] == -1) {
        // One page only: no page arrows.
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_PREV, ANIM_PAGE_ARROW_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_NEXT, ANIM_PAGE_ARROW_OFF, 1.0f, 0.0f, 1, 0);
        pPrev->m_nFlags40 |= 1;
        pNext->m_nFlags40 |= 1;
        return;
    }

    if (m_nPage == 0) {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_PREV, ANIM_PAGE_ARROW_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_NEXT, ANIM_PAGE_ARROW_ON, 1.0f, 0.0f, 1, 0);
        pPrev->m_nFlags40 |= 1;
        pNext->m_nFlags40 &= ~1;
    } else if (m_nPage == 1) {
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_PREV, ANIM_PAGE_ARROW_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_PAGE_NEXT, ANIM_PAGE_ARROW_OFF, 1.0f, 0.0f, 1, 0);
        pPrev->m_nFlags40 &= ~1;
        pNext->m_nFlags40 |= 1;
    }
}

long msw::SelectMenu::CRuleSel::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_CLOSE) {
        m_Unk30 = STATE_CLOSE;
    }
    return 0;
}

void msw::SelectMenu::CRuleSel::SetArrowDisable(long nRow, int nSide, int bDisable)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(s_ArrowBtnKindId[nRow][nSide]);
    if (pPart == 0) {
        return;
    }

    if (bDisable) {
        pPart->ClearPadNoBits();
        pPart->SetButtonOut();
        pPart->m_nAnim[0] = 0;
        pPart->m_nAnim[1] = 0;
        pPart->m_nAnim[2] = 0;
        pPart->m_nAnim[3] = 0;
        pPart->m_nAnim[4] = 0;
        pPart->m_nAnim[5] = 0;
        pPart->m_nSeCode[0] = SE_NONE;
        pPart->m_nSeCode[1] = SE_ARROW_OFF;
    } else {
        pPart->m_nAnim[0] = 1;
        pPart->m_nAnim[1] = 2;
        pPart->m_nAnim[2] = 3;
        pPart->m_nAnim[3] = 0;
        pPart->m_nAnim[4] = 4;
        pPart->m_nAnim[5] = 0xFF;
        pPart->m_nSeCode[0] = SE_CURSOR;
        pPart->m_nSeCode[1] = SE_ARROW;
    }
}
