#include "game/msw/SelectMenu/CRuleInfo.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "bcs/CLoadMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CXmlReader.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CRuleHelpTex.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CHowTo.cpp: the original converts the
    // value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_01 = 1,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_40 = 0x40,
        OBJ_RULE_INFO = 0x4E
    };

    // The button kind ids of the screen: the back button and the tabs
    // (the first set when there are six, the second when five).
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_TAB6_0 = 0x8010,
        BTN_KIND_TAB6_5 = 0x8015,
        BTN_KIND_TAB5_0 = 0x8020,
        BTN_KIND_TAB5_4 = 0x8024
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    // Frames of the white fade back to the party game information.
    const float BACK_FADE_TIME = 20.0f;
}

void msw::CSelectMenu::RuleInfo_In()
{
    static const long padNo0 = 0;

    Build(OBJ_RULE_INFO);
    SetFadeIn(60.0f);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_RULE_INFO);
    unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
    long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
    rBtnKindId = nBtnKindId;
    pObj->m_nPushButtonId[padNo0] = nBtnKindId;
    pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    m_nStep = 0;
}

void msw::CSelectMenu::RuleInfo_Loop()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    SelectMenu::CRuleInfo *pObj = (SelectMenu::CRuleInfo *)FindObj(OBJ_RULE_INFO);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        switch (nBtnKindId) {
            case BTN_KIND_TAB6_0:
            case BTN_KIND_TAB6_0 + 1:
            case BTN_KIND_TAB6_0 + 2:
            case BTN_KIND_TAB6_0 + 3:
            case BTN_KIND_TAB6_0 + 4:
            case BTN_KIND_TAB6_5:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB6_0);
                break;
            case BTN_KIND_TAB5_0:
            case BTN_KIND_TAB5_0 + 1:
            case BTN_KIND_TAB5_0 + 2:
            case BTN_KIND_TAB5_0 + 3:
            case BTN_KIND_TAB5_4:
                pObj->SetTabSelect(nBtnKindId - BTN_KIND_TAB5_0);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                PartyInfo_ChangeStateInit(m_nNextMode);
                PlayMenuBGM(1);
                SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
                SetWhiteFadeOut(BACK_FADE_TIME);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        PartyInfo_ChangeStateInit(m_nNextMode);
        PlayMenuBGM(1);
        SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
        SetWhiteFadeOut(BACK_FADE_TIME);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
        if (m_Unk23E == 1) {
            // A pad moves the tabs left and right; the pointer's
            // highlight goes.
            for (long i = 0; i < pObj->m_nTabNum; i++) {
                pObj->m_pPaneAccessor->StopAnimation(pObj->m_pTabPanes[i], 0xC, 1);
                pObj->m_pPaneAccessor->StopAnimation(pObj->m_pTabPanes[i], 0xF, 1);
                pObj->m_bTabHit[i] = 0;
            }
            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
                pObj->SetTabSelect(pObj->m_nTabSelect - 1);
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
                pObj->SetTabSelect(pObj->m_nTabSelect + 1);
            }
        }
    }
}

void msw::CSelectMenu::RuleInfo_Out()
{
    Scratch(OBJ_RULE_INFO);
}

// The names in this namespace are guesses; the objects have no symbols.
// They are defined here, after the CSelectMenu steps, because they follow
// those steps' constants in .rodata.
namespace {

    const char *const s_LayoutName = "rule_operation.brlyt";
    const char *const s_ButtonFileName = "rule_operation.btn";

    // The panes of the screen.
    enum {
        PANE_ROOT = 0,
        PANE_TITLE_TEXT = 0xD,
        PANE_TEXT = 0xE,
        PANE_PICT_0 = 0x15,
        PANE_PICT_1 = 0x16,
        PANE_PICT = 0x17     // holds the two pictures
    };

    // The animations of the screen.
    enum {
        ANIM_PICT_CHANGE = 4,
        ANIM_IN = 6,
        ANIM_OUT = 8,
        ANIM_TAB_OFF = 0xB,
        ANIM_TAB_HIT_ON = 0xC,
        ANIM_TAB_SELECT = 0xD,
        ANIM_TAB_HIT_OFF_SELECTED = 0xE,
        ANIM_TAB_HIT_ON_SELECTED = 0xF,
        ANIM_TAB_UNSELECT = 0x10
    };

    // Values of CSelectMenuLayoutBase::m_Unk30 beyond the base's STATE_*.
    enum {
        STATE_PICT = 6 // ChangePict runs
    };

    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // The two picture panes, swapped by ChangePict.
    const long s_PictPanes[2] = { PANE_PICT_0, PANE_PICT_1 };
    // The tab panes, with six tabs and with five, and the panes of the
    // tab titles. The 0 after the five tab panes is not read here.
    const long s_TabPanes6[6] = { 2, 4, 6, 8, 0xA, 0xC };
    const long s_TabPanes5[6] = { 0x1B, 0x1D, 0x1F, 0x21, 0x23, 0 };
    const long s_TabTitles6[6] = { 0xF, 0x10, 0x11, 0x12, 0x13, 0x14 };
    const long s_TabTitles5[5] = { 0x24, 0x25, 0x26, 0x27, 0x28 };
}

msw::SelectMenu::CRuleInfo::CRuleInfo(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CRuleInfo::~CRuleInfo()
{
    Remove();
}

void msw::SelectMenu::CRuleInfo::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap (notes/decisions.md): the game numbers
            // these recursive, reset@0, recursive@1, reset@2 and stores
            // them in the reverse order; this compiler stores them in
            // declaration order, so the order here gives the original
            // bytes and exchanges the symbol names.
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_IN) && bcs::CLoadMan::IsComplete()) {
                GXTexObj *pTexObj = m_pHelpTex->CreateTex(0);
                bcs::G2D::ReplacePanePicture(m_pPaneAccessor->GetPane(PANE_PICT_0), pTexObj, 0);
                m_pPaneAccessor->SetPaneDisp(PANE_PICT_0, 1);
                m_pPaneAccessor->PlayAnimation(PANE_PICT, ANIM_PICT_CHANGE, -1.0f, 60.0f, 1, 0);
                m_nPict = 1;
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            if (m_fTime != 0.0f) {
                m_fTime -= bcs::CMainApp::GetAppFrameRateF();
                if (m_fTime <= 0.0f) {
                    // The next item of the page: its text, and its picture
                    // starts loading. The same code is in SetPageNode: the
                    // original repeats it (an inline member helper would
                    // move its strings out of the unit's string pool).
                    bcs::CXmlReader *pItem = m_pPage->GetChild(m_nItem);
                    m_fTime = atof(pItem->GetProperty("time", 0));
                    const char *pTextGroup = pItem->GetProperty("file", 0);
                    const wchar_t *pText = GetMessageLabelString(pTextGroup, pItem->GetProperty("text_label", 0));
                    m_pPaneAccessor->SetTextBoxString(PANE_TEXT, pText, wcslen(pText));
                    const char *pPictName = pItem->GetValue();
                    bcs::CLoadMan::FILE_INPUT *pFile = &CRuleHelpTex::g_sFileInput[m_pHelpTex->m_nLoad];
                    sprintf(pFile->m_szPath, "menu/rulehelp/%s/%s.bin", m_pHelpTex->GetLanguageFileName(), pPictName);
                    pFile->m_nFlags = (pFile->m_nFlags & 0xFFFE) | 2;
                    bcs::CLoadMan::LoadAsync(pFile, 1);
                    m_nItem++;
                    if (m_nItem >= m_pPage->GetChildrenNum()) {
                        m_nItem = 0;
                    }
                    m_Unk30 = STATE_PICT;
                }
            }
            break;
        case STATE_PICT:
            ChangePict();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_ROOT, ANIM_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_ROOT, ANIM_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CRuleInfo::Disp()
{
}

void msw::SelectMenu::CRuleInfo::ChangePict()
{
    switch (m_nPictStep) {
        case 0:
            // The next picture is loaded: put it in the back pane.
            if (bcs::CLoadMan::IsComplete()) {
                GXTexObj *pTexObj = m_pHelpTex->CreateTex(0);
                long nPane = s_PictPanes[m_nPict];
                bcs::G2D::ReplacePanePicture(m_pPaneAccessor->GetPane(nPane), pTexObj, 0);
                m_pPaneAccessor->SetPaneDisp(nPane, 1);
                m_nPictStep = 1;
            }
            break;
        case 1:
            // Turn the panes over, one way or the other.
            if (m_nPict != 0) {
                m_pPaneAccessor->PlayAnimation(PANE_PICT, ANIM_PICT_CHANGE, 20.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_PICT, ANIM_PICT_CHANGE, -20.0f, 60.0f, 1, 0);
            }
            m_nPictStep = 2;
            break;
        case 2: {
            // The game reads both of these and uses neither.
            m_pPaneAccessor->GetPaneObj(PANE_PICT);
            m_pPaneAccessor->GetPaneObj(s_PictPanes[m_nPict])->GetAnimCurrentFrame(ANIM_PICT_CHANGE);
            if (m_pPaneAccessor->IsAnimEnd(PANE_PICT, ANIM_PICT_CHANGE)) {
                m_nPictStep = 0;
                m_nPict ^= 1;
                bcs::CLoadMan::UnLoad(&CRuleHelpTex::g_sFileInput[m_nPict], 1);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        }
    }
}

int msw::SelectMenu::CRuleInfo::Set()
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
                pPart->m_bUnk40_0020 = 0;
                pPart->m_bUnk40_0004 = 1;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            nw4r::lyt::ArcResourceAccessor *pPackAccessor =
                pLytCtrl->NewArcResourceAccessor("menu/bin/General_pack.bin", "arc");
            m_nTabSelect = -1;
            m_nItem = 0;
            m_nPict = 0;
            m_nPictStep = 0;
            m_Unk5E4 = 0;
            m_Unk5E8 = 0;

            // The element of the game kind in the rules file.
            char szKind[0x10];
            switch (pSelectMenu->m_nGameKind) {
                case 0:
                    sprintf(szKind, "BASKET");
                    break;
                case 1:
                    sprintf(szKind, "VOLLEY");
                    break;
                case 2:
                    sprintf(szKind, "DODGE");
                    break;
                case 3:
                    sprintf(szKind, "HOCKEY");
                    break;
            }

            m_pHelpTex = new CRuleHelpTex;
            m_pXml = new bcs::CXmlReader(
                static_cast<const char *>(pPackAccessor->GetResource(0, "RuleExplanation.xml", 0)), 0);
            for (long i = 0; i < m_pXml->GetChildrenNum(); i++) {
                m_pTabNode = m_pXml->GetChild(i);
                if (m_pTabNode != 0 && strcmp(m_pTabNode->GetKey(), szKind) == 0) {
                    m_nTabNum = atoi(m_pTabNode->GetProperty("Tab", 0));
                    break;
                }
            }

            if (m_nTabNum == 6) {
                m_pTabPanes = s_TabPanes6;
            } else {
                m_pTabPanes = s_TabPanes5;
            }
            SetTabSelect(0);

            // The colour of the game kind.
            long nAnim;
            switch (GetSelectMenu()->m_nGameKind) {
                case 0:
                    nAnim = 0;
                    break;
                case 1:
                    nAnim = 1;
                    break;
                case 2:
                    nAnim = 2;
                    break;
                case 3:
                    nAnim = 3;
                    break;
                default:
                    nAnim = 0;
                    break;
            }
            m_pPaneAccessor->PlayAnimation(PANE_ROOT, nAnim, 1.0f, 0.0f, 1, 1);

            m_bVibEnable = 1;
            m_bSeEnable = 1;
        }
    }
    return 0;
}

// Inlined into Main() although defined after it (as in game/menu/CTeam.cpp);
// defined down here because its local static follows Set()'s constants in
// .rodata.
void msw::SelectMenu::CRuleInfo::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CRuleInfo::Remove()
{
    if (m_pHelpTex != 0) {
        delete m_pHelpTex;
    }
    m_pHelpTex = 0;
    if (m_pXml != 0) {
        delete m_pXml;
    }
    m_pXml = 0;
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CRuleInfo::SendEvent(long, void *)
{
    return -1;
}

long msw::SelectMenu::CRuleInfo::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }

    long nResult = m_pMenuPartList->CheckHitButton(pPos);

    // The pointer's highlight on the tabs.
    for (long i = 0; i < m_nTabNum; i++) {
        long nBtnKindId = BTN_KIND_TAB5_0 + i;
        if (m_nTabNum == 6) {
            nBtnKindId = BTN_KIND_TAB6_0 + i;
        }
        if (m_bTabHit[i] == 0 && nBtnKindId == nResult) {
            if (i == m_nTabSelect) {
                m_pPaneAccessor->PlayAnimation(m_pTabPanes[i], ANIM_TAB_HIT_ON_SELECTED, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(m_pTabPanes[i], ANIM_TAB_HIT_ON, 1.0f, 0.0f, 1, 0);
            }
            m_bTabHit[i] = 1;
        }
        if (m_bTabHit[i] == 1 && nBtnKindId != nResult) {
            m_pPaneAccessor->StopAnimation(m_pTabPanes[i], ANIM_TAB_HIT_ON, 1);
            m_pPaneAccessor->StopAnimation(m_pTabPanes[i], ANIM_TAB_HIT_ON_SELECTED, 1);
            if (i == m_nTabSelect) {
                m_pPaneAccessor->PlayAnimation(m_pTabPanes[i], ANIM_TAB_HIT_OFF_SELECTED, 1.0f, 0.0f, 1, 0);
            } else {
                m_pPaneAccessor->PlayAnimation(m_pTabPanes[i], ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
            }
            m_bTabHit[i] = 0;
        }
    }

    if (nResult == 0) {
        nResult = GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, EVENT_HIT, pPos);
    }
    return nResult;
}

long msw::SelectMenu::CRuleInfo::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleInfo::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleInfo::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CRuleInfo::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CRuleInfo::StickMoveCheck(long nPad, long nBtnKindId)
{
    // From a button the stick does not move, start at the first tab.
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_TAB5_0;
        if (m_nTabNum == 6) {
            nBtnKindId = BTN_KIND_TAB6_0;
        }
    }
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CRuleInfo::SetDefaultMoveCursor(long, long nBtnKindId)
{
    return nBtnKindId;
}

int msw::SelectMenu::CRuleInfo::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CRuleInfo::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(OBJ_UNK_01, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

void msw::SelectMenu::CRuleInfo::SetTabSelect(long nTab)
{
    long nPrev = m_nTabSelect;
    if (nTab == nPrev || nTab >= m_nTabNum || nTab < 0) {
        return;
    }
    m_nTabSelect = nTab;

    if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
        if (nTab > nPrev) {
            SE_Play(0xBCD, 1);
        } else {
            SE_Play(0xBCE, 1);
        }
    } else {
        SE_Play(0xBCD, 1);
    }

    const long *pTitles = s_TabTitles5;
    if (m_nTabNum == 6) {
        pTitles = s_TabTitles6;
    }
    for (long i = 0; i < m_nTabNum; i++) {
        m_pPaneAccessor->StopAnimation(m_pTabPanes[i], ANIM_TAB_HIT_ON, 1);
        if (nTab == i) {
            m_pPaneAccessor->PlayAnimation(m_pTabPanes[nTab], ANIM_TAB_SELECT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[nTab], 1);
        } else if (nPrev == i) {
            m_pPaneAccessor->PlayAnimation(m_pTabPanes[nPrev], ANIM_TAB_UNSELECT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[nPrev], 0);
        } else {
            m_pPaneAccessor->PlayAnimation(m_pTabPanes[i], ANIM_TAB_OFF, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->SetPaneDisp(pTitles[i], 0);
        }
    }

    m_nItem = 0;
    m_pPaneAccessor->SetPaneDisp(PANE_PICT_0, 0);
    m_pPaneAccessor->SetPaneDisp(PANE_PICT_1, 0);
    bcs::CLoadMan::UnLoad(CRuleHelpTex::g_sFileInput, 2);
    SetPageNode(m_pTabNode, m_nTabSelect);
    if (nPrev != -1) {
        m_Unk30 = STATE_PICT;
    }
}

void msw::SelectMenu::CRuleInfo::SetPageNode(void *pNode, long nTab)
{
    bcs::CXmlReader *pTabNode = static_cast<bcs::CXmlReader *>(pNode);

    char szPage[0x10];
    sprintf(szPage, "PAGE%d", nTab + 1);
    m_pPage = 0;
    for (long i = 0; i < pTabNode->GetChildrenNum(); i++) {
        bcs::CXmlReader *pPage = pTabNode->GetChild(i);
        if (pPage != 0 && strcmp(pPage->GetKey(), szPage) == 0) {
            const char *pGroup = pPage->GetProperty("file", 0);
            const wchar_t *pTitle = GetMessageLabelString(pGroup, pPage->GetProperty("title_label", 0));
            m_pPaneAccessor->SetTextBoxString(PANE_TITLE_TEXT, pTitle, wcslen(pTitle));
            m_pPage = pPage;

            // The first item of the page: its text, and its picture starts
            // loading. The same as in Main.
            bcs::CXmlReader *pItem = pPage->GetChild(m_nItem);
            m_fTime = atof(pItem->GetProperty("time", 0));
            const char *pTextGroup = pItem->GetProperty("file", 0);
            const wchar_t *pText = GetMessageLabelString(pTextGroup, pItem->GetProperty("text_label", 0));
            m_pPaneAccessor->SetTextBoxString(PANE_TEXT, pText, wcslen(pText));
            const char *pPictName = pItem->GetValue();
            bcs::CLoadMan::FILE_INPUT *pFile = &CRuleHelpTex::g_sFileInput[m_pHelpTex->m_nLoad];
            sprintf(pFile->m_szPath, "menu/rulehelp/%s/%s.bin", m_pHelpTex->GetLanguageFileName(), pPictName);
            pFile->m_nFlags = (pFile->m_nFlags & 0xFFFE) | 2;
            bcs::CLoadMan::LoadAsync(pFile, 1);
            m_nItem++;
            if (m_nItem >= m_pPage->GetChildrenNum()) {
                m_nItem = 0;
            }
            break;
        }
    }
}

const char *msw::SelectMenu::CRuleHelpTex::GetLanguageFileName()
{
    static const char *LangNameJP_JA = "JpJa";
    static const char *LangNameUS_EN = "UsEn";
    static const char *LangNameUS_FR = "UsFr";
    static const char *LangNameUS_SP = "UsSp";
    static const char *LangNameEU_EN = "EuEn";
    static const char *LangNameEU_GE = "EuGe";
    static const char *LangNameEU_FR = "EuFr";
    static const char *LangNameEU_IT = "EuIt";
    static const char *LangNameEU_SP = "EuSp";
    static const char *LangNameEU_DU = "EuDu";

    const char *pName;
    switch (bcs::CMainApp::GetRegionLanguage()) {
        case 0:
            pName = LangNameJP_JA;
            break;
        case 1:
            pName = LangNameUS_EN;
            break;
        case 2:
            pName = LangNameUS_FR;
            break;
        case 3:
            pName = LangNameUS_SP;
            break;
        case 4:
            pName = LangNameEU_EN;
            break;
        case 5:
            pName = LangNameEU_GE;
            break;
        case 6:
            pName = LangNameEU_FR;
            break;
        case 8:
            pName = LangNameEU_IT;
            break;
        case 7:
            pName = LangNameEU_SP;
            break;
        case 9:
            pName = LangNameEU_DU;
            break;
        default:
            pName = 0;
            break;
    }
    return pName;
}

bcs::CLoadMan::FILE_INPUT msw::SelectMenu::CRuleHelpTex::g_sFileInput[2] = {
    { "chr/c00TE.mot", 0xF0010, 1, 3, 1, -32, 0 },
    { "chr/c00TE.mot", 0xF0011, 1, 3, 1, -32, 0 }
};

msw::SelectMenu::CRuleHelpTex::~CRuleHelpTex()
{
    bcs::CLoadMan::UnLoad(g_sFileInput, 2);
}

void msw::SelectMenu::CRuleHelpTex::Main()
{
}

void msw::SelectMenu::CRuleHelpTex::Disp()
{
}

GXTexObj *msw::SelectMenu::CRuleHelpTex::CreateTex(unsigned long nIndex)
{
    // The original asks for the resource twice and keeps the second answer.
    nw4r::g3d::ResFile resFile(static_cast<nw4r::g3d::ResFileData *>(bcs::CLoadMan::GetRes(&g_sFileInput[m_nTex])));
    resFile = nw4r::g3d::ResFile(static_cast<nw4r::g3d::ResFileData *>(bcs::CLoadMan::GetRes(&g_sFileInput[m_nTex])));
    if (!resFile.IsValid()) {
        return 0;
    }
    resFile.CheckRevision();
    resFile.Init();
    resFile.Bind(resFile);

    nw4r::g3d::ResTex resTex = resFile.GetResTex(nIndex);
    if (!resTex.IsValid()) {
        return 0;
    }

    void *pImage;
    unsigned short nWidth;
    unsigned short nHeight;
    _GXTexFmt format;
    float fMinLod;
    float fMaxLod;
    unsigned char bMipmap;
    resTex.GetTexObjParam(&pImage, &nWidth, &nHeight, &format, &fMinLod, &fMaxLod, &bMipmap);
    GXInitTexObj(&m_TexObj[m_nTex], pImage, nWidth, nHeight, format, GX_CLAMP, GX_CLAMP, bMipmap);
    GXInitTexObjLOD(&m_TexObj[m_nTex], GX_LINEAR, GX_LINEAR, fMinLod, fMaxLod, 0.0f, 0, 0, GX_ANISO_1);

    GXTexObj *pTexObj = &m_TexObj[m_nTex];
    m_nLoad ^= 1;
    m_nTex ^= 1;
    return pTexObj;
}
