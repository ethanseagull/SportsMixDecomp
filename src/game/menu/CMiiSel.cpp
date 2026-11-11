#include "game/msw/SelectMenu/CMiiSel.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CMiiSelectRender.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/layout.h"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CRuleSel.cpp: the original converts the
    // value to float at run time and keeps the word first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_UNK_01 = 1,
        OBJ_MII_SEL = 0x17,
        OBJ_BG_CHR = 0x3A,
        OBJ_DWC_WINDOW = 0x3F,
        OBJ_2P_ENTRY = 0xFF
    };

    // Events sent to the menu objects.
    enum {
        EVENT_LINE_MESSAGE = 0x11, // to OBJ_TOP: the help line
        EVENT_HIT = 0x13,          // to OBJ_UNK_01
        EVENT_PUSH = 0x21,         // to OBJ_UNK_01, plus the pad number
        EVENT_CLOSE = 0x2E,        // to the 2P entry window
        EVENT_WINDOW_CLOSE = 0x32, // to OBJ_DWC_WINDOW
        EVENT_BG_CHR_UNK = 0x33    // to OBJ_BG_CHR
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_PAGE_NEXT = 0x40,
        PAD_PAGE_PREV = 0x80,
        PAD_POINTER = 0x100
    };

    // The button kind ids of the Mii select screen.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_MII_0 = 0x8010,  // the ten Mii buttons, 0x8010 to 0x8019
        BTN_KIND_MII_2 = 0x8020,  // and a second set, 0x8020 to 0x8029
        BTN_KIND_MII_LAST = 0x8029,
        BTN_KIND_TAB_MII = 0x8030, // the tabs: own Miis (also 0x8035)
        BTN_KIND_TAB_GUEST = 0x8031, // and guests (also 0x8036)
        BTN_KIND_TAB_MII_2 = 0x8035,
        BTN_KIND_TAB_GUEST_2 = 0x8036,
        BTN_KIND_PAGE_PREV = 0x8040,
        BTN_KIND_PAGE_NEXT = 0x8041
    };

    // Values of CSelectMenu::m_Unk9D: where the screen was entered from.
    enum {
        ENTRY_CHARA = 2,
        ENTRY_WIFI_2P = 0x16
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_MAIN = 0,
        STEP_CONFIRM = 1,
        STEP_NO_MII = 2
    };

    // Answers of the message windows (CCockpit::GetResult).
    enum {
        RESULT_YES = 1000,
        RESULT_NO = 1001
    };

    // The cockpit object of the message window.
    enum {
        COCKPIT_WINDOW = 0x1042
    };

    // CSelectMenu::m_nNextMode after the Wi-Fi entry for two.
    enum {
        MODE_WIFI_2P_NEXT = 0x1A
    };

    // Frames the screen waits after a page change.
    const float PAGE_WAIT = 20.0f;
}


// Inline in the game (name guessed): whether the part is switched off.
static inline int IsOff(const msw::CMenuPartBase *pPart)
{
    return pPart->m_nFlags40 & 1;
}

// Inline in the game (name guessed): whether the part is playing its push
// animation (states 5 and 6).
static inline bool IsPushAnim(const msw::CMenuPartBase *pPart)
{
    bool bPush = true;
    long nState = pPart->m_Unk08;
    if (nState != 5 && nState != 6) {
        bPush = false;
    }
    return bPush;
}

// Whether button nBtnKindId can take the cursor: it is shown and enabled,
// not being pushed and not marked off.
inline int msw::SelectMenu::CMiiSel::IsSelectButton(unsigned short nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return 0;
    }
    if (!(pPart->m_nFlags40 & 2)) {
        return 0;
    }
    if (IsPushAnim(pPart)) {
        return 0;
    }
    if (pPart->m_Unk42_0F != 0) {
        return 0;
    }
    if (pPart->m_nFlags40 & 0x10) {
        return 0;
    }
    return !IsOff(m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId));
}

inline msw::SAVE_DATA_PLAYER::MiiData *msw::SelectMenu::CMiiSel::GetSlotMii(long nSlot)
{
    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
    long nIndex = m_nMiiIndex[nSlot];
    if (m_bGuest == 0) {
        return pPlayerData->GetIndexMii(nIndex);
    }
    return pPlayerData->GetGuestMii(nIndex);
}

void msw::CSelectMenu::MiiSel_In()
{
    SelectMenu::CMiiSel *pObj;
    m_nStep = STEP_MAIN;
    long nPad = m_Unk27D;
    Build(OBJ_MII_SEL);
    if (m_Unk9D == ENTRY_WIFI_2P) {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "Mii_03_01"));
    } else {
        SendObjEventTrigger(OBJ_TOP, EVENT_LINE_MESSAGE,
                            (void *)GetMessageLabelString("ExhibitionMenuMessage", "Mii_03_00"));
    }
    SendObjEventTrigger(OBJ_BG_CHR, EVENT_BG_CHR_UNK, 0);
    m_Unk27B = 3;

    // Only the pad that entered can use the pointer.
    for (long i = 0; i < 4; i++) {
        if (i == nPad) {
            _DisablePointStick(i, 0);
        } else {
            _DisablePointStick(i, 1);
        }
    }

    pObj = (SelectMenu::CMiiSel *)FindObj(OBJ_MII_SEL);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[nPad][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(nPad, rBtnKindId);
        if (m_aPadSelect[nPad].m_Unk00 == 1) {
            pObj->SetCursorMarkerPosition(nPad, nBtnKindId);
        }
        rBtnKindId = nBtnKindId;
        m_nCupBtnKindId[nPad] = nBtnKindId;
    }

    m_nMiiSelSlot = 0xFF;
    m_fInputWait = 0.0f;
    m_nStep = STEP_MAIN;
}

void msw::CSelectMenu::MiiSel_Loop()
{
    long nPad = m_Unk27D;
    long nMode = m_nMode;
    SelectMenu::CMiiSel *pObj = (SelectMenu::CMiiSel *)FindObj(OBJ_MII_SEL);
    if (pObj == 0) {
        return;
    }
    // Only while the screen takes input.
    if (pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    switch (m_nStep) {
        case STEP_MAIN: {
            if (!CCockpit::IsEndObj(COCKPIT_WINDOW)) {
                break;
            }
            m_fInputWait -= GetFrameRateF();
            if (m_fInputWait < 0.0f) {
                m_fInputWait = 0.0f;
                long nBtnKindId = m_nCurrentBtnKindId[nPad][nMode];

                // The page arrows under the pointer.
                if (CMenuPad::s_Instance.GetInput(nPad) & PAD_POINTER) {
                    switch (nBtnKindId) {
                        case BTN_KIND_PAGE_PREV:
                            m_fInputWait = PAGE_WAIT;
                            pObj->PushButton(nPad, nBtnKindId);
                            pObj->ChangeMiiPage(0);
                            return;
                        case BTN_KIND_PAGE_NEXT:
                            m_fInputWait = PAGE_WAIT;
                            pObj->PushButton(nPad, nBtnKindId);
                            pObj->ChangeMiiPage(1);
                            return;
                    }
                }

                if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                    switch (nBtnKindId) {
                        default:
                            // A Mii.
                            if ((unsigned long)(nBtnKindId - BTN_KIND_MII_0) > 0x19) {
                                break;
                            }
                            pObj->PushButton(nPad, nBtnKindId);
                            if ((unsigned long)(nBtnKindId - BTN_KIND_MII_0) <= 9) {
                                m_nMiiSelSlot = nBtnKindId - BTN_KIND_MII_0;
                            } else if ((unsigned long)(nBtnKindId - BTN_KIND_MII_2) <= 9) {
                                m_nMiiSelSlot = nBtnKindId - BTN_KIND_MII_2;
                            }

                            long nEntry = GetPadEntryId(nPad);
                            if (m_Unk9D == ENTRY_CHARA) {
                                if (m_nMiiSelFlags == 1) {
                                    WindowYesNo_Button("CharaEntryMessage", "PMAKE_02_00", 1,
                                                       pObj->m_MiiName[m_nMiiSelSlot], 0, 1);
                                } else if (m_nMiiSelFlags == 2 || m_nMiiSelFlags == 3) {
                                    WindowYesNo_Button("CharaEntryMessage", "PMAKE_04_00", 0,
                                                       pObj->m_MiiName[m_nMiiSelSlot], 0, 1);
                                }
                                m_nStep = STEP_CONFIRM;
                            } else if (m_Unk9D == ENTRY_WIFI_2P) {
                                // The second player of a Wi-Fi entry for two.
                                unsigned char bGuest = pObj->m_bGuest;
                                WiFi_Set2PMiiParam(bGuest, pObj->GetSlotMiiNameId(m_nMiiSelSlot),
                                                   pObj->GetSlotMiiSource(m_nMiiSelSlot),
                                                   pObj->GetSlotMiiCreateId(m_nMiiSelSlot));
                                SelectMenu::CWiFi_2PEntry *pEntry = new SelectMenu::CWiFi_2PEntry(OBJ_2P_ENTRY);
                                AddObj(pEntry);
                                pEntry->Set();
                                m_pPlayerData = CSave::GetPlayerData(-1);
                                WindowYesNo_Button("WifiConnectMessage", "WiFi_A_00", 1, m_pPlayerData->m_Name,
                                                   pObj->m_MiiName[m_nMiiSelSlot], 0);
                                _DisablePointStick(0, 0);
                                _DisablePointStick(1, 1);
                                m_nStep = STEP_CONFIRM;
                            } else {
                                m_pPlayerData = CSave::GetPlayerData(-1);
                                long nIndex =
                                    m_pPlayerData->SearchMiiIndex(pObj->GetSlotMiiCreateId(m_nMiiSelSlot));
                                if (m_pPlayerData->GetSelectMii(nIndex) == 0) {
                                    Mii_SetMiiParam(nEntry, pObj->m_bGuest, pObj->GetSlotMiiNameId(m_nMiiSelSlot),
                                                    pObj->GetSlotMiiSource(m_nMiiSelSlot),
                                                    pObj->GetSlotMiiCreateId(m_nMiiSelSlot));
                                    m_pPlayerData->SetSelectMii(nIndex, 1);
                                    CharSel_CallMiiVoice(pObj->GetSlotMiiSource(m_nMiiSelSlot),
                                                         pObj->GetSlotMiiCreateId(m_nMiiSelSlot));
                                    GoBackToMode(nPad, GetPrevMode(), pObj, nBtnKindId);
                                }
                            }
                            break;
                        case BTN_KIND_TAB_MII:
                        case BTN_KIND_TAB_MII_2:
                            if (pObj->m_bGuest != 0) {
                                pObj->PushButton(nPad, nBtnKindId);
                                pObj->SetMiiPage();
                            }
                            break;
                        case BTN_KIND_TAB_GUEST:
                        case BTN_KIND_TAB_GUEST_2:
                            if (pObj->m_bGuest != 1) {
                                pObj->PushButton(nPad, nBtnKindId);
                                pObj->SetGuestMiiPage();
                            }
                            break;
                        case BTN_KIND_BACK:
                            m_nMiiSelFlags |= 0x80;
                            pObj->PushButton(nPad, nBtnKindId);
                            GoBackToMode(nPad, GetPrevMode(), pObj, 1);
                            break;
                    }
                    return;
                }

                if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
                    m_nMiiSelFlags |= 0x80;
                    GoBackToMode(nPad, GetPrevMode(), pObj, 1);
                    return;
                }

                if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_PAGE_PREV) && pObj->m_bGuest == 0) {
                    if (pObj->ChangeMiiPage(0) == 0) {
                        pObj->PushButton(nPad, BTN_KIND_PAGE_PREV);
                        m_fInputWait = PAGE_WAIT;
                    }
                    return;
                }

                if ((CMenuPad::s_Instance.GetInput(nPad) & PAD_PAGE_NEXT) && pObj->m_bGuest == 0) {
                    if (pObj->ChangeMiiPage(1) == 0) {
                        pObj->PushButton(nPad, BTN_KIND_PAGE_NEXT);
                        m_fInputWait = PAGE_WAIT;
                    }
                    return;
                }
            }
            InputDataToMenuObj(pObj, nPad, nPad, nMode);
            break;
        }

        case STEP_CONFIRM: {
            long nLeader = m_Unk27D;
            switch (CCockpit::GetResult()) {
                case RESULT_YES: {
                    WindowYesNo_Close();
                    SendObjEventTrigger(OBJ_2P_ENTRY, EVENT_CLOSE, 0);
                    long nEntry = GetPadEntryId(nLeader);
                    float fWait = _GetModeChangeWaitTime();
                    Mii_SetMiiParam(nEntry, pObj->m_bGuest, pObj->GetSlotMiiNameId(m_nMiiSelSlot),
                                    pObj->GetSlotMiiSource(m_nMiiSelSlot),
                                    pObj->GetSlotMiiCreateId(m_nMiiSelSlot));
                    if (m_pPlayerData->GetMiiCount() == 0) {
                        WindowA_Button("CharaEntryMessage", "PMAKE_12", 0, 0);
                        m_nStep = STEP_NO_MII;
                    } else if (m_Unk9D == ENTRY_CHARA) {
                        GoBackToMode(nLeader, GetPrevMode(), pObj, m_nMiiSelSlot + 0x5A);
                    } else if (m_Unk9D == ENTRY_WIFI_2P) {
                        SetNextMode(MODE_WIFI_2P_NEXT, fWait);
                    }
                    break;
                }
                case RESULT_NO:
                    SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                    if (m_Unk9D == ENTRY_WIFI_2P) {
                        _DisablePointStick(0, 1);
                        _DisablePointStick(1, 0);
                    }
                    WindowYesNo_Close();
                    SendObjEventTrigger(OBJ_2P_ENTRY, EVENT_CLOSE, 0);
                    m_nStep = STEP_MAIN;
                    break;
            }
            break;
        }

        case STEP_NO_MII: {
            long nLeader = m_Unk27D;
            float fWait = _GetModeChangeWaitTime();
            if (CCockpit::GetResult() == RESULT_YES) {
                if (m_Unk9D == ENTRY_CHARA) {
                    GoBackToMode(nLeader, GetPrevMode(), pObj, m_nMiiSelSlot + 0x5A);
                } else if (m_Unk9D == ENTRY_WIFI_2P) {
                    SetNextMode(MODE_WIFI_2P_NEXT, fWait);
                }
            }
            break;
        }
    }
}

void msw::CSelectMenu::MiiSel_Out()
{
    Scratch(OBJ_MII_SEL);
}

namespace {

    // The panes of the window.
    enum {
        PANE_WIN = 0,
        PANE_NAME = 2,
        PANE_PAGE = 0x21,
        PANE_ARROW_PREV = 0x46,
        PANE_ARROW_NEXT = 0x48,
        PANE_TAB_GUEST = 0x49,
        PANE_TAB_MII = 0x4B
    };

    // The animations played on them.
    enum {
        ANIM_TAB_IN = 5,
        ANIM_TAB_OUT = 6,
        ANIM_WIN_IN = 7,
        ANIM_WIN_OUT = 8,
        ANIM_WIN_IN_2 = 9,
        ANIM_WIN_OUT_2 = 0xB,
        ANIM_PAGE = 0xC,
        ANIM_ON = 0x11,
        ANIM_OFF = 0x12,
        ANIM_MII_NONE = 0x13,
        ANIM_MII_ENABLE = 0x14
    };

    // The sounds of the page arrows.
    enum {
        SE_PAGE = 0xBCD,
        SE_PAGE_PREV = 0xBCE
    };

    const char *const s_LayoutName = "me_Mii.brlyt";
    const char *const s_ButtonFileName = "me_Mii.btn";

    // For each slot: the pane its frame animations play on, the pane its
    // picture is put over and its button.
    long s_MiiFramePane[msw::SelectMenu::CMiiSel::SLOT_MAX] = {
        0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15,
        0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36
    };
    long s_MiiPicturePane[msw::SelectMenu::CMiiSel::SLOT_MAX] = {
        0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
        0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44
    };
    long s_MiiButton[msw::SelectMenu::CMiiSel::SLOT_MAX] = {
        0x8010, 0x8011, 0x8012, 0x8013, 0x8014, 0x8015, 0x8016, 0x8017, 0x8018, 0x8019,
        0x8020, 0x8021, 0x8022, 0x8023, 0x8024, 0x8025, 0x8026, 0x8027, 0x8028, 0x8029
    };
}

msw::SelectMenu::CMiiSel::CMiiSel(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    for (long i = 0; i < SLOT_MAX; i++) {
        m_pMiiRender[i] = 0;
        m_pMiiPane[i] = 0;
    }
}

msw::SelectMenu::CMiiSel::~CMiiSel()
{
    Remove();
}

void msw::SelectMenu::CMiiSel::Main()
{
    StateControl();
}

void msw::SelectMenu::CMiiSel::Disp()
{
}

void msw::SelectMenu::CMiiSel::StateControl()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, reset, recursive);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, reset, recursive);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_2, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_WIN_IN_2, s_WinAnimSpeed[0], 0.0f, reset,
                                               recursive);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_WIN_IN_2, s_WinAnimSpeed[0], 0.0f, reset,
                                               recursive);
            }
            m_pPaneAccessor->PlayAnimation(PANE_TAB_MII, ANIM_TAB_IN, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_TAB_GUEST, ANIM_TAB_IN, 1.0f, 0.0f, 1, 0);
            m_nWinPane = PANE_WIN;
            m_nPagePane = PANE_PAGE;
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN: {
            int bEnd = 0;
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    bEnd = 1;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_2)) {
                    bEnd = 1;
                }
            }
            if (bEnd) {
                CSelectMenu *pSelectMenu = GetSelectMenu();
                long nPad = pSelectMenu->m_Unk27D;
                long nMode = pSelectMenu->m_nMode;
                long nBtnKindId = SetDefaultMoveCursor(nPad, 0);
                pSelectMenu->m_nCurrentBtnKindId[nPad][nMode] = nBtnKindId;
                if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
                    m_nPushButtonId[nPad] = nBtnKindId;
                    SetCursorMarkerPosition(nPad, nBtnKindId);
                }
                m_Unk30 = STATE_ACTIVE;
                if (m_nMiiCount > PAGE_MII_NUM) {
                    m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
                    m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                }
            }
            break;
        }
        case STATE_ACTIVE: {
            CheckButtonAnim();
            CMenuPartBase *pPrev = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_PREV);
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_NEXT);
            if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
                pPrev->m_nSeCode[1] = SE_PAGE_PREV;
                pNext->m_nSeCode[1] = SE_PAGE;
            } else {
                pPrev->m_nSeCode[1] = SE_PAGE;
                pNext->m_nSeCode[1] = SE_PAGE;
            }
            break;
        }
        case STATE_PAGE:
            CheckPageChange();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(m_nWinPane, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(m_nWinPane, ANIM_WIN_OUT_2, s_WinAnimSpeed[0], 0.0f, recursive,
                                                   reset);
                }
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_TAB_MII, ANIM_TAB_OUT, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_TAB_GUEST, ANIM_TAB_OUT, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(m_nWinPane, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(m_nWinPane, ANIM_WIN_OUT_2)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

int msw::SelectMenu::CMiiSel::Set()
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
                // Only the stick moves between these buttons.
                if ((unsigned short)SetUpButton(pPart, i, 0) >= 0x40) {
                    pPart->m_bUnk40_0020 = 0;
                }
                pPart->m_bUnk40_0004 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_pPaneAccessor->PlayAnimation(PANE_PAGE, ANIM_PAGE, 1.0f, 0.0f, 1, 1);
            m_nMiiCount = CSave::GetPlayerData(-1)->GetMiiCount();
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
            m_nPage = 0;
            if (m_nMiiCount != 0) {
                SetMiiPage();
            } else {
                SetGuestMiiPage();
            }
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CMiiSel::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    for (long i = 0; i < SLOT_MAX; i++) {
        if (m_pMiiRender[i] != 0) {
            m_pMiiRender[i]->Delete();
            m_pMiiRender[i] = 0;
        }
        m_pMiiPane[i] = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CMiiSel::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CMiiSel::SetCursorMarkerPosition(long nPad, long nBtnKindId)
{
    // The name of the Mii under the cursor (by its button number, which the
    // game does not limit to the page).
    long nIndex = -1;
    if ((unsigned long)(nBtnKindId - BTN_KIND_MII_0) <= 0x19) {
        nIndex = nBtnKindId - BTN_KIND_MII_0;
    }
    if (nIndex >= 0 && nIndex < m_nMiiCount) {
        SetTextPaneString(PANE_NAME, m_MiiName[nIndex]);
    } else {
        SetTextPaneString(PANE_NAME, L"\x3000");
    }
    return 0;
}

long msw::SelectMenu::CMiiSel::StickMoveCheck(long nPad, long nBtnKindId)
{
    long nCurrent = nBtnKindId;
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nCurrent = SetDefaultMoveCursor(nPad, 0);
    }

    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, nInput);
    if (!IsSelectButton(nNext)) {
        nNext = nCurrent;
    }

    // Nothing to move to in that direction: look further.
    if ((nInput & 0xF) && nNext == nCurrent) {
        if ((nInput & 1) || (nInput & 4)) {
            unsigned short nId = nCurrent;
            for (;;) {
                CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
                if (pPart == 0) {
                    nId = 0;
                } else if (nInput & 1) {
                    nId = pPart->m_nNextBtnKindId[1];
                } else if (nInput & 4) {
                    nId = pPart->m_nNextBtnKindId[2];
                } else if (nInput & 8) {
                    nId = pPart->m_nNextBtnKindId[3];
                } else if (nInput & 2) {
                    nId = pPart->m_nNextBtnKindId[4];
                } else {
                    nId = 0;
                }
                if (IsSelectButton(nId)) {
                    nNext = nId;
                    break;
                }
                if (nId == 0) {
                    break;
                }
                nNext = GetRightButtonKindId(nId);
                if (nNext != 0) {
                    break;
                }
            }
        } else {
            nNext = GetNextButtonKindId(nCurrent, nInput);
        }
        if (nNext == 0) {
            nNext = nCurrent;
        }
    }
    return nNext;
}

// The first Mii of the page that can be chosen, else a tab.
inline long msw::SelectMenu::CMiiSel::GetFirstButton()
{
    for (long nId = BTN_KIND_MII_0; nId < BTN_KIND_MII_0 + PAGE_MII_NUM; nId++) {
        if (!(m_pMenuPartList->FindObjectByCurrentBtnKindId(nId)->m_nFlags40 & 1)) {
            return nId;
        }
    }
    return m_nPage != 0 ? BTN_KIND_TAB_GUEST : BTN_KIND_TAB_MII;
}

long msw::SelectMenu::CMiiSel::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        return GetFirstButton();
    }
    return 0;
}

int msw::SelectMenu::CMiiSel::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CMiiSel::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

long msw::SelectMenu::CMiiSel::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CMiiSel::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CMiiSel::ClearCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CMiiSel::SetCursorMarkerVisible(long, int)
{
    return 0;
}

// The last page of the player's Miis (ten to a page).
static inline long GetLastPage(long nCount)
{
    long nLast = nCount / msw::SelectMenu::CMiiSel::PAGE_MII_NUM;
    if (nCount % msw::SelectMenu::CMiiSel::PAGE_MII_NUM == 0) {
        nLast--;
    }
    return nLast;
}

long msw::SelectMenu::CMiiSel::ChangeMiiPage(int bNext)
{
    if (m_nMiiCount != 0) {
        if (bNext == 1) {
            long nLast = GetLastPage(m_nMiiCount);
            if (++m_nPage > nLast) {
                m_nPage = nLast;
                return -1;
            }
            SetMiiPage();
            m_bPagePrev = 0;
        } else {
            if (--m_nPage < 0) {
                m_nPage = 0;
                return -1;
            }
            SetMiiPage();
            m_bPagePrev = 1;
        }
        for (long nId = BTN_KIND_MII_0; nId <= BTN_KIND_MII_LAST; nId++) {
            CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
            if (pPart != 0) {
                pPart->InitButtonAnim();
            }
        }
        m_Unk30 = STATE_PAGE;
    }
    return 0;
}

void msw::SelectMenu::CMiiSel::CheckPageChange()
{
    int bEnd = 0;
    int bPrevEnd = m_pPaneAccessor->IsAnimEnd(PANE_ARROW_PREV, 4);
    int bNextEnd = m_pPaneAccessor->IsAnimEnd(PANE_ARROW_NEXT, 4);
    if (m_bPagePrev == 0) {
        if (m_pPaneAccessor->IsAnimEnd(m_nWinPane, ANIM_WIN_IN_2) && bPrevEnd && bNextEnd) {
            bEnd = 1;
        }
    } else {
        if (m_pPaneAccessor->IsAnimEnd(m_nWinPane, ANIM_WIN_IN) && bPrevEnd && bNextEnd) {
            bEnd = 1;
        }
    }
    if (!bEnd) {
        return;
    }

    m_pPaneAccessor->PlayAnimation(m_nPagePane, ANIM_PAGE, 1.0f, 0.0f, 1, 0);
    CMenuPartBase *pPrev = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_PREV);
    CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_NEXT);

    // The page arrows: none for the guests or a single page.
    if (m_bGuest == 1) {
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
        pPrev->m_nFlags40 |= 1;
        pNext->m_nFlags40 |= 1;
    } else if (m_nMiiCount != 0) {
        long nLast = GetLastPage(m_nMiiCount);
        if (nLast != 0) {
            if (nLast == m_nPage) {
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
                pPrev->m_nFlags40 &= ~1;
                pNext->m_nFlags40 |= 1;
            } else if (m_nPage == 0) {
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                pPrev->m_nFlags40 |= 1;
                pNext->m_nFlags40 &= ~1;
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                pPrev->m_nFlags40 &= ~1;
                pNext->m_nFlags40 &= ~1;
            }
        }
    }

    // Once every slot has finished its animation, the cursor goes back on.
    long nEndNum = 0;
    for (long i = 0; i < PAGE_MII_NUM; i++) {
        if (m_pPaneAccessor->IsAnimEnd(s_MiiFramePane[i], ANIM_MII_NONE)) {
            nEndNum++;
        }
    }
    if (nEndNum == PAGE_MII_NUM) {
        CSelectMenu *pSelectMenu = GetSelectMenu();
        long nPad = pSelectMenu->m_Unk27D;
        long nMode = pSelectMenu->m_nMode;
        long nBtnKindId = SetDefaultMoveCursor(nPad, 0);
        pSelectMenu->m_nCurrentBtnKindId[nPad][nMode] = nBtnKindId;
        if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
            m_nPushButtonId[nPad] = nBtnKindId;
            SetCursorMarkerPosition(nPad, nBtnKindId);
        }
        m_Unk30 = STATE_ACTIVE;
    }
}

// Empties the ten slots of the page before it is filled.
inline void msw::SelectMenu::CMiiSel::ClearPage()
{
    for (long i = 0; i < PAGE_MII_NUM; i++) {
        if (m_pMiiPane[i] != 0) {
            m_pMiiPane[i]->SetVisible(false);
        }
        memset(m_MiiName[i], 0, 10 * sizeof(wchar_t));
        m_pPaneAccessor->PlayAnimation(s_MiiFramePane[i], ANIM_MII_NONE, 1.0f, 0.0f, 1, 1);
        m_pMenuPartList->FindObjectByCurrentBtnKindId(s_MiiButton[i])->m_nFlags40 |= 1;
    }
}

// The buttons of the slots start their animations again.
inline void msw::SelectMenu::CMiiSel::InitSlotButtons()
{
    for (long nId = BTN_KIND_MII_0; nId <= BTN_KIND_MII_LAST; nId++) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (pPart != 0) {
            pPart->InitButtonAnim();
        }
    }
}

void msw::SelectMenu::CMiiSel::SetMiiPage()
{
    long nFree = 0;
    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
    m_nMiiCount = pPlayerData->GetMiiCount();
    ClearPage();
    m_pPaneAccessor->PlayAnimation(PANE_TAB_GUEST, ANIM_OFF, 1.0f, 0.0f, 1, 0);
    m_pPaneAccessor->PlayAnimation(PANE_TAB_MII, ANIM_ON, 1.0f, 0.0f, 1, 0);

    long nTop = m_nPage * PAGE_MII_NUM;
    m_bGuest = 0;
    long nEnd = nTop + PAGE_MII_NUM;
    if (nEnd >= m_nMiiCount) {
        nEnd = m_nMiiCount;
    }

    long nSlot = 0;
    for (long i = nTop; i < nEnd; i++, nSlot++) {
        SAVE_DATA_PLAYER::MiiData *pMii = pPlayerData->GetIndexMii(i);
        m_nMiiIndex[nSlot] = i;
        RFLStoreData storeData;
        bcs::CMiiMan::GetStoreData(storeData, pMii->m_Source, pMii->m_CreateId);
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, storeData);

        RFLStoreData renderData = storeData;
        long nPane = s_MiiPicturePane[nSlot];
        nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(nPane)->GetPane()->GetSize();
        if (m_pMiiRender[nSlot] != 0) {
            m_pMiiRender[nSlot]->Delete();
        }
        m_pMiiRender[nSlot] = new CMiiSelectRender(0, renderData, size.width, size.height);

        GXTexObj texObj;
        GXInitTexObj(&texObj, m_pMiiRender[nSlot]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                     GX_CLAMP, GX_CLAMP, 0);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
        bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
        if (m_pMiiPane[nSlot] == 0) {
            m_pMiiPane[nSlot] = menu::CreateMiiPicturePane(&texObj, pLytObj, pPane, "RENDER_MII");
        } else {
            bcs::G2D::ReplacePanePicture(m_pMiiPane[nSlot], &texObj, 0);
        }
        m_pMiiPane[nSlot]->SetVisible(true);

        RFLAdditionalInfo nameInfo;
        bcs::CMiiMan::GetAdditionalInfo(nameInfo, storeData);
        wcscpy(m_MiiName[nSlot], nameInfo.name);
        if (pPlayerData->GetSelectMii(i) == 0) {
            m_pPaneAccessor->PlayAnimation(s_MiiFramePane[nSlot], ANIM_MII_ENABLE, 1.0f, 0.0f, 1, 0);
            m_pMenuPartList->FindObjectByCurrentBtnKindId(s_MiiButton[nSlot])->m_nFlags40 &= ~1;
            nFree++;
        }
    }

    // Every Mii of the page is taken: the cursor goes to the tab.
    if (nFree == 0) {
        CSelectMenu *pSelectMenu = GetSelectMenu();
        long nPad = pSelectMenu->m_Unk27D;
        pSelectMenu->m_nCurrentBtnKindId[nPad][pSelectMenu->m_nMode] = BTN_KIND_TAB_MII;
        if (!CMenuPad::s_Instance.IsInputPointerMode(nPad)) {
            m_nPushButtonId[nPad] = BTN_KIND_TAB_MII;
            SetCursorMarkerPosition(nPad, BTN_KIND_TAB_MII);
        }
    }

    InitSlotButtons();
    m_Unk30 = STATE_PAGE;
}

void msw::SelectMenu::CMiiSel::SetGuestMiiPage()
{
    m_nMiiCount = GUEST_MII_NUM;
    ClearPage();
    m_pPaneAccessor->PlayAnimation(PANE_TAB_MII, ANIM_OFF, 1.0f, 0.0f, 1, 0);
    m_pPaneAccessor->PlayAnimation(PANE_TAB_GUEST, ANIM_ON, 1.0f, 0.0f, 1, 0);
    m_bGuest = 1;

    SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
    for (long i = 0; i < GUEST_MII_NUM; i++) {
        SAVE_DATA_PLAYER::MiiData *pMii = pPlayerData->GetGuestMii(i);
        m_nMiiIndex[i] = i;
        RFLStoreData storeData;
        bcs::CMiiMan::GetStoreData(storeData, pMii->m_Source, pMii->m_CreateId);
        RFLAdditionalInfo info;
        bcs::CMiiMan::GetAdditionalInfo(info, storeData);

        RFLStoreData renderData = storeData;
        long nPane = s_MiiPicturePane[i];
        nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(nPane)->GetPane()->GetSize();
        if (m_pMiiRender[i] != 0) {
            m_pMiiRender[i]->Delete();
        }
        m_pMiiRender[i] = new CMiiSelectRender(0, renderData, size.width, size.height);

        GXTexObj texObj;
        GXInitTexObj(&texObj, m_pMiiRender[i]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                     GX_CLAMP, GX_CLAMP, 0);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
        bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
        nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);
        if (m_pMiiPane[i] == 0) {
            m_pMiiPane[i] = menu::CreateMiiPicturePane(&texObj, pLytObj, pPane, "RENDER_MII");
        } else {
            bcs::G2D::ReplacePanePicture(m_pMiiPane[i], &texObj, 0);
        }
        m_pMiiPane[i]->SetVisible(true);

        wcscpy(m_MiiName[i], GetMessageString("List_Mii_Name", pMii->m_nNameId));
        if (pPlayerData->GetSelectGuestMii(i) == 0) {
            m_pPaneAccessor->PlayAnimation(s_MiiFramePane[i], ANIM_MII_ENABLE, 1.0f, 0.0f, 1, 0);
            m_pMenuPartList->FindObjectByCurrentBtnKindId(s_MiiButton[i])->m_nFlags40 &= ~1;
        }
    }

    InitSlotButtons();
    m_Unk30 = STATE_PAGE;
}

unsigned short msw::SelectMenu::CMiiSel::GetNextButtonKindId(long nBtnKindId, unsigned long nInput)
{
    unsigned short nResult = 0;
    long nId = nBtnKindId;

    // Steps in the direction of nInput and tries the button there and its
    // two neighbours across, until one can be chosen or it is back where
    // it started.
    while (nResult == 0) {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (pPart == 0) {
            return 0;
        }

        unsigned short nCandidate[3];
        if (nInput & 1) {
            nCandidate[0] = pPart->m_nNextBtnKindId[1];
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCandidate[0]);
            if (pNext == 0) {
                return 0;
            }
            nCandidate[1] = pNext->m_nNextBtnKindId[3];
            nCandidate[2] = pNext->m_nNextBtnKindId[4];
        } else if (nInput & 4) {
            nCandidate[0] = pPart->m_nNextBtnKindId[2];
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCandidate[0]);
            if (pNext == 0) {
                return 0;
            }
            nCandidate[1] = pNext->m_nNextBtnKindId[3];
            nCandidate[2] = pNext->m_nNextBtnKindId[4];
        } else if (nInput & 8) {
            nCandidate[0] = pPart->m_nNextBtnKindId[3];
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCandidate[0]);
            if (pNext == 0) {
                return 0;
            }
            nCandidate[1] = pNext->m_nNextBtnKindId[1];
            nCandidate[2] = pNext->m_nNextBtnKindId[2];
        } else if (nInput & 2) {
            nCandidate[0] = pPart->m_nNextBtnKindId[4];
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(nCandidate[0]);
            if (pNext == 0) {
                return 0;
            }
            nCandidate[1] = pNext->m_nNextBtnKindId[1];
            nCandidate[2] = pNext->m_nNextBtnKindId[2];
        }

        for (long i = 0; i < 3; i++) {
            if (nCandidate[i] != 0 && IsSelectButton(nCandidate[i])) {
                nResult = nCandidate[i];
                break;
            }
        }

        nId = nCandidate[0];
        if (nId == nBtnKindId) {
            break;
        }
    }
    return nResult;
}

unsigned short msw::SelectMenu::CMiiSel::GetRightButtonKindId(long nBtnKindId)
{
    long nId = nBtnKindId;
    unsigned short nResult = 0;

    do {
        CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (pPart == 0) {
            return 0;
        }
        nId = pPart->m_nNextBtnKindId[4];
        if (nId == 0) {
            return 0;
        }
        CMenuPartBase *pRight = m_pMenuPartList->FindObjectByCurrentBtnKindId(nId);
        if (pRight == 0) {
            return 0;
        }
        if (IsSelectButton(nId)) {
            nResult = nId;
            break;
        }
    } while (nId != nBtnKindId);
    return nResult;
}

// Inlined into StateControl() although defined after it; the out-of-line
// copy is not called and the link drops it. Defined down here because its
// local static follows the constants of Set() in .rodata (as in
// game/menu/CTournamentLoad.cpp). Only the pad that entered can push.
void msw::SelectMenu::CMiiSel::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    unsigned short nPushButtonId[4];
    for (long i = 0; i < 4; i++) {
        nPushButtonId[i] = i == GetSelectMenu()->m_Unk27D ? m_nPushButtonId[i] : 0;
    }
    m_pMenuPartList->CheckButtonAnim(nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
}

// Inlined into SetCursorMarkerPosition(), as CheckButtonAnim() into
// StateControl().
void msw::SelectMenu::CMiiSel::SetTextPaneString(long nPane, const wchar_t *pText)
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

namespace {

    // The animations and panes of the 2P entry window.
    const char *s_2PEntryAnimNames[] = {
        "me_2p_entry_Mii_in_win_0.brlan",
        "me_2p_entry_Mii_in_win_1.brlan",
        "me_2p_entry_Mii_in_win_2.brlan"
    };

    const char *s_2PEntryPaneNames[] = {
        "Mii_null",    "mii_00",   "box_shadow_00", "flame_bg_00", "s_glow_00",     "unable_p_00", "m_mii_00",
        "flame_HL_00", "flame_00", "b_mii_00",      "mii_01",      "box_shadow_01", "flame_bg_01", "s_glow_01",
        "unable_p_01", "m_mii_01", "flame_HL_01",   "flame_01",    "b_mii_01"
    };

    enum {
        ANIM_2P_IN = 0,
        ANIM_2P_LOOP = 1,
        ANIM_2P_OUT = 2
    };
}

msw::SelectMenu::CWiFi_2PEntry::~CWiFi_2PEntry()
{
    m_pMiiRender[0]->Delete();
    m_pMiiRender[1]->Delete();
    Remove();
}

void msw::SelectMenu::CWiFi_2PEntry::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_2P_IN, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_2P_IN)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_2P_LOOP, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            break;
        case STATE_CLOSE:
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_2P_OUT, 1.0f, 0.0f, 1, 1);
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_2P_OUT)) {
                m_Unk30 = STATE_NONE;
                m_Unk08 = 2;
            }
            break;
        default:
            return;
    }
}

int msw::SelectMenu::CWiFi_2PEntry::Set()
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
            pPaneCtrl->BuildLayout(0, "me_2p_entry.brlyt");
            pPaneCtrl->BuildAnim(0, s_2PEntryAnimNames, 3);
            SetPaneAccessor(pPaneCtrl, s_2PEntryPaneNames, 19);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(120.0f);

            // The player's own Mii and the second player's.
            SAVE_DATA_PLAYER *pPlayerData = CSave::GetPlayerData(-1);
            RFLStoreData storeData;
            bcs::CMiiMan::GetStoreData(storeData, pPlayerData->m_MiiSource, pPlayerData->m_MiiCreateId);
            {
                RFLStoreData renderData = storeData;
                const char *pPaneName = "m_mii_00";
                nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(pPaneName)->GetPane()->GetSize();
                m_pMiiRender[0] = new CMiiSelectRender(0, renderData, size.width, size.height);

                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pMiiRender[0]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                             GX_CLAMP, GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
                menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(pPaneName, 1, 0), "RENDER_MII");
            }
            {
                RFLStoreData renderData = pSelectMenu->WiFi_Get2PMiiStoreData();
                const char *pPaneName = "m_mii_01";
                nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(pPaneName)->GetPane()->GetSize();
                m_pMiiRender[1] = new CMiiSelectRender(0, renderData, size.width, size.height);

                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pMiiRender[1]->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                             GX_CLAMP, GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
                menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(pPaneName, 1, 0), "RENDER_MII");
            }

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CWiFi_2PEntry::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
}

long msw::SelectMenu::CWiFi_2PEntry::SendEvent(long nEvent, void *pParam)
{
    long nResult = -1;

    if (nEvent == EVENT_CLOSE) {
        if (m_Unk30 < STATE_CLOSE) {
            m_Unk30 = STATE_CLOSE;
        }
        nResult = 0;
    }
    return nResult;
}

void msw::SelectMenu::CWiFi_2PEntry::Disp()
{
}

inline long msw::SelectMenu::CMiiSel::GetSlotMiiNameId(long nSlot)
{
    return GetSlotMii(nSlot)->m_nNameId;
}

inline RFLDataSource msw::SelectMenu::CMiiSel::GetSlotMiiSource(long nSlot)
{
    return GetSlotMii(nSlot)->m_Source;
}

inline RFLCreateID msw::SelectMenu::CMiiSel::GetSlotMiiCreateId(long nSlot)
{
    return GetSlotMii(nSlot)->m_CreateId;
}
