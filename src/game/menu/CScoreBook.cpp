#include "game/msw/SelectMenu/CScoreBook.hpp"

#include "bcs/CMiiMan.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The stage numbers of each game kind in menu order: the same table as
    // the one msw::_GetStageNoInfo reads (in CGamePadCheck.cpp), probably
    // from a shared header. Nothing in this unit reads it; it is kept by
    // UnusedReferences() below (see notes/decisions.md, "Unreferenced data
    // kept by dead references").
    const unsigned char s_StageNo[4][12] = {
        { 0x01, 0x02, 0x05, 0x06, 0x11, 0x07, 0x0A, 0x0B, 0x10, 0x03, 0x09, 0x0F },
        { 0x01, 0x02, 0x03, 0x05, 0x06, 0x11, 0x0A, 0x0B, 0x10, 0x09, 0x0C, 0x0F },
        { 0x01, 0x02, 0x03, 0x05, 0x04, 0x07, 0x09, 0x0B, 0x10, 0x11, 0x0C, 0x0F },
        { 0x01, 0x04, 0x03, 0x11, 0x09, 0x07, 0x0A, 0x0C, 0x10, 0x02, 0x0F, 0x0B }
    };

    // A stand-in for an unknown user of the table above; does nothing.
    inline void UnusedReferences()
    {
        (void)s_StageNo;
    }

    // [0] is the speed of the page's opening and closing animations. A
    // table, not a scalar, as in CScoreTop: the original converts the value
    // to float at run time and keeps the word in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_3B = 0x3B,
        OBJ_SCORE_BOOK = 0x44
    };

    // The button kind ids of the screen: the page buttons, and the back
    // button.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_PAGE_PREV = 0x8010,
        BTN_KIND_PAGE_NEXT = 0x8011
    };

    // CMenuPad input bits.
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_RIGHT = 0x40,
        PAD_LEFT = 0x80,
        PAD_PAGE = 0x100
    };

    // Frames before the page buttons take input again after a page turn.
    const float PAGE_INPUT_WAIT = 20.0f;
}

void msw::CSelectMenu::ScoreBook_In()
{
    Build(OBJ_SCORE_BOOK);

    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);

    // The title line, with the player's name put into the message.
    wchar_t text[0x100];
    swprintf(text, 0xFF, GetMessageLabelString("TopMenuMessage", "SCOREBOOK_01_01"),
             (const wchar_t *)pPlayerData);
    SendObjEventTrigger(OBJ_UNK_00, 0x11, text);

    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x35, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x3A, (void *)-1);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));
    m_fInputWait = 0.0f;
    m_nCurrentBtnKindId[0][m_nMode] = 0;
}

void msw::CSelectMenu::ScoreBook_Loop()
{
    // The game numbers `objId` after `padNo0` but stores it first; declared
    // this way the bytes are the original's and the names swapped:
    // static-order swap (see notes/decisions.md).
    static const long objId = OBJ_SCORE_BOOK;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    SelectMenu::CScoreBook *pObj = (SelectMenu::CScoreBook *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait <= 0.0f) {
        m_fInputWait = 0.0f;

        long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_PAGE) {
            switch (nBtnKindId) {
                case BTN_KIND_PAGE_PREV:
                    if (pObj->m_nPage != 0) {
                        pObj->PageChange(1);
                        pObj->PushButton(padNo0, BTN_KIND_PAGE_PREV);
                        m_fInputWait = PAGE_INPUT_WAIT;
                        return;
                    }
                    break;
                case BTN_KIND_PAGE_NEXT:
                    if (pObj->m_nPage != SelectMenu::CScoreBook::PAGE_NUM - 1) {
                        pObj->PageChange(0);
                        pObj->PushButton(padNo0, BTN_KIND_PAGE_NEXT);
                        m_fInputWait = PAGE_INPUT_WAIT;
                        return;
                    }
                    break;
            }
        }

        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
            pObj->PushButton(padNo0, nBtnKindId);
            if (nBtnKindId == BTN_KIND_BACK) {
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
            return;
        }
        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
            GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            return;
        }
        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
            if (pObj->PageChange(1)) {
                pObj->PushButton(padNo0, BTN_KIND_PAGE_PREV);
            }
            return;
        }
        if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
            if (pObj->PageChange(0)) {
                pObj->PushButton(padNo0, BTN_KIND_PAGE_NEXT);
            }
            return;
        }
    }
    InputDataToMenuObj(pObj, padNo0, nPad, nMode);
}

void msw::CSelectMenu::ScoreBook_Out()
{
    Scratch(OBJ_SCORE_BOOK);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata; a const pointer lands in .rodata behind its string, the pane
// tables in .data.
namespace {

    const char *const s_LayoutName = "me_scorebook_B.brlyt";
    const char *const s_ButtonFileName = "me_scorebook_B.btn";

    // The page's animations, by index into the names the button file lists
    // (the names are not in the module).
    enum {
        ANIM_SPORT_BASKET = 5,
        ANIM_SPORT_VOLLEY = 6,
        ANIM_SPORT_DODGE = 7,
        ANIM_SPORT_HOCKEY = 8,
        ANIM_PAGE_IN_BACK = 9,
        ANIM_PAGE_OUT_BACK = 10,
        ANIM_PAGE_IN = 11,
        ANIM_PAGE_OUT = 13,
        ANIM_PAGE_INIT = 14,
        ANIM_ARROW_IN = 15,
        ANIM_ARROW_LOOP = 16,
        ANIM_ARROW_OUT = 17,
        ANIM_ON = 19,
        ANIM_OFF = 20,
        ANIM_ARROW_END = 4,
        ANIM_ARROW_INIT = 0
    };

    enum {
        PANE_PAGE_0 = 0, // the pages are panes 0 to 3
        PANE_PAGE_3 = 3,
        PANE_SPORT_LEVEL = 10,  // where the difficulty mark goes
        PANE_ARROW_PREV = 0x31,
        PANE_ARROW_NEXT = 0x33,
        PANE_LEVEL_0 = 0x35,
        PANE_LEVEL_1 = 0x37,
        PANE_LEVEL_2 = 0x3A,
        PANE_CHARA_ICON = 0x3E, // plus the character number
        PANE_ARROWS = 0x50,
        PANE_PAGE_NUM = 0x51
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // The sound of the page buttons: the first plays when it turns a page.
    enum {
        SE_PAGE = 0xBCD,
        SE_PAGE_FIRST = 0xBCE
    };

    // The character number with no ranking row.
    enum {
        CHARA_NO_RANK = 0x13
    };

    // The number of guest Miis.
    enum {
        GUEST_MII_NUM = 6
    };

    // Per ranking row: the name of the Mii picture, the pane the face or the
    // character icon goes in, the name and score panes.
    const char *s_FaceNames[msw::SelectMenu::CScoreBook::RANK_NUM] = { "FACE01", "FACE02", "FACE03", "FACE04",
                                                                      "FACE05" };
    long s_FacePanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x19, 0x1B, 0x1D, 0x1F, 0x21 };
    long s_NamePanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x18, 0x1A, 0x1C, 0x1E, 0x20 };
    long s_ScorePanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x22, 0x23, 0x24, 0x25, 0x26 };
    long s_StageNamePanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x27, 0x28, 0x29, 0x2A, 0x2B };
    long s_StageScorePanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x2C, 0x2D, 0x2E, 0x2F, 0x30 };
    // Per sport, per page: the panes switched on for the sport shown.
    long s_BasketPanes[msw::SelectMenu::CScoreBook::PAGE_NUM] = { 0x52, 0x56, 0x5A, 0x5E };
    long s_VolleyPanes[msw::SelectMenu::CScoreBook::PAGE_NUM] = { 0x53, 0x57, 0x5B, 0x5F };
    long s_DodgePanes[msw::SelectMenu::CScoreBook::PAGE_NUM] = { 0x54, 0x58, 0x5C, 0x60 };
    long s_HockeyPanes[msw::SelectMenu::CScoreBook::PAGE_NUM] = { 0x55, 0x59, 0x5D, 0x61 };
    // The place marks: copied from these panes into the rows.
    const char *s_RankNames[msw::SelectMenu::CScoreBook::RANK_NUM] = { "1st", "2nd", "3rd", "4th", "5th" };
    long s_RankPanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x67, 0x68, 0x69, 0x6A, 0x6B };
    const char *s_StageRankNames[msw::SelectMenu::CScoreBook::RANK_NUM] = { "1st_stage", "2nd_stage", "3rd_stage",
                                                                           "4th_stage", "5th_stage" };
    long s_StageRankPanes[msw::SelectMenu::CScoreBook::RANK_NUM] = { 0x6C, 0x6D, 0x6E, 0x6F, 0x70 };
}

msw::SelectMenu::CScoreBook::CScoreBook(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    for (long i = 0; i < 4; i++) {
        m_pFaceTex[i] = 0;
    }
}

msw::SelectMenu::CScoreBook::~CScoreBook()
{
    Remove();
}

void msw::SelectMenu::CScoreBook::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap (see notes/decisions.md), as in CScoreTop.
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_pPaneAccessor->PlayAnimation(PANE_ARROWS, ANIM_ARROW_IN, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_IN_BACK)) {
                    m_pPaneAccessor->PlayAnimation(PANE_ARROWS, ANIM_ARROW_LOOP, 1.0f, 0.0f, 1, 0);
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_IN)) {
                    m_pPaneAccessor->PlayAnimation(PANE_ARROWS, ANIM_ARROW_LOOP, 1.0f, 0.0f, 1, 0);
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE: {
            CheckButtonAnim();
            // With the pointer both page buttons make the same sound.
            CMenuPartBase *pPrev = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_PREV);
            CMenuPartBase *pNext = m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_PAGE_NEXT);
            if (!CMenuPad::s_Instance.IsInputPointerMode(0)) {
                pPrev->m_nSeCode[1] = SE_PAGE_FIRST;
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
                    m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive,
                                                   reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_pPaneAccessor->PlayAnimation(PANE_ARROWS, ANIM_ARROW_OUT, 1.0f, 0.0f, 1, 0);
                switch (m_nPage) {
                    case 0:
                        m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                        break;
                    case PAGE_NUM - 1:
                        m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                        break;
                    default:
                        m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                        m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                        break;
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CScoreBook::Disp()
{
}

int msw::SelectMenu::CScoreBook::Set()
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
                SetUpButton(pPart, i, 1);
                pPart->m_bUnk40_0020 = 0;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;
            m_Unk2C = 1;
            m_Unk30 = STATE_SET;

            UnusedReferences();
            for (long i = PANE_PAGE_0; i <= PANE_PAGE_3; i++) {
                m_pPaneAccessor->PlayAnimation(i, ANIM_PAGE_INIT, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(i, ANIM_ON, 1.0f, 0.0f, 1, 0);
            }
            // The result is not used.
            for (long i = 0; i <= PANE_SPORT_LEVEL; i++) {
                m_pPaneAccessor->GetPane(i);
            }

            m_nPage = 0;
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ARROW_INIT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ARROW_INIT, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
            m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);

            wchar_t text[0x10];
            swprintf(text, 0x10, L"%d/4", m_nPage + 1);
            SetTextPaneString(PANE_PAGE_NUM, text);

            switch (pSelectMenu->m_nGameKind) {
                case 0:
                    SetUpBasketScore();
                    break;
                case 1:
                    SetUpVolleyScore();
                    break;
                case 2:
                    SetUpDodgeScore();
                    break;
                case 3:
                    SetUpHockeyScore();
                    break;
            }
            SetUpCommonScore();
        }
    }
    return 0;
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined here, after Set(), because its
// local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CScoreBook::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Inlined into its callers, as CheckButtonAnim() into Main(); its local
// static comes next in .rodata.
void msw::SelectMenu::CScoreBook::SetTextPaneString(long nPane, const wchar_t *pText)
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

void msw::SelectMenu::CScoreBook::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    for (long i = 0; i < 4; i++) {
        if (m_pFaceTex[i] != 0) {
            delete m_pFaceTex[i];
        }
        m_pFaceTex[i] = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CScoreBook::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CScoreBook::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreBook::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreBook::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreBook::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CScoreBook::StickMoveCheck(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreBook::SetDefaultMoveCursor(long, long)
{
    return 0;
}

int msw::SelectMenu::CScoreBook::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CScoreBook::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId)->ClearPadNoBits();
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

int msw::SelectMenu::CScoreBook::PageChange(int bBack)
{
    if (bBack == 1 && m_nPage == 0) {
        return 0;
    }
    if (bBack == 0 && m_nPage == PAGE_NUM - 1) {
        return 0;
    }

    long nPage;
    if (bBack == 0) {
        nPage = m_nPage + 1;
        m_pPaneAccessor->PlayAnimation(nPage, ANIM_PAGE_IN, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_OUT, 1.0f, 0.0f, 1, 0);
        m_bPageBack = 0;
    } else {
        nPage = m_nPage - 1;
        m_pPaneAccessor->PlayAnimation(nPage, ANIM_PAGE_IN_BACK, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(m_nPage, ANIM_PAGE_OUT_BACK, 1.0f, 0.0f, 1, 0);
        m_bPageBack = 1;
    }
    m_nPage = nPage;

    wchar_t text[0x10];
    swprintf(text, 0x10, L"%d/4", nPage + 1);
    SetTextPaneString(PANE_PAGE_NUM, text);

    m_Unk30 = STATE_PAGE;
    return 1;
}

void msw::SelectMenu::CScoreBook::CheckPageChange()
{
    int bEnd = 0;
    int bPrevEnd = m_pPaneAccessor->IsAnimEnd(PANE_ARROW_PREV, ANIM_ARROW_END);
    int bNextEnd = m_pPaneAccessor->IsAnimEnd(PANE_ARROW_NEXT, ANIM_ARROW_END);

    if (m_bPageBack == 0) {
        if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_IN) && bPrevEnd && bNextEnd) {
            bEnd = 1;
        }
    } else {
        if (m_pPaneAccessor->IsAnimEnd(m_nPage, ANIM_PAGE_IN_BACK) && bPrevEnd && bNextEnd) {
            bEnd = 1;
        }
    }

    if (bEnd) {
        switch (m_nPage) {
            case 0:
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
                break;
            case PAGE_NUM - 1:
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_OFF, 1.0f, 0.0f, 1, 0);
                break;
            default:
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_PREV, ANIM_ON, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_ARROW_NEXT, ANIM_ON, 1.0f, 0.0f, 1, 0);
                break;
        }
        m_Unk30 = STATE_ACTIVE;
    }
}

long msw::SelectMenu::CScoreBook::SetTextPaneString(long nPane, long nValue, long nType)
{
    static const bool recursive = true;

    CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
    nw4r::lyt::Pane *pPane =
        (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
    if (pPane == 0) {
        return -2;
    }

    wchar_t text[0x10];
    switch (nType) {
        case 0:
            swprintf(text, 0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_UNIT_00"), nValue);
            break;
        case 1:
            swprintf(text, 0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_UNIT_01"), nValue);
            break;
        case 2:
            swprintf(text, 0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_UNIT_02"), nValue);
            break;
        case -1:
            swprintf(text, 0x10, L"%d", nValue);
            break;
        case -2:
            swprintf(text, 0x10, L"");
            break;
    }
    m_pPaneAccessor->SetTextBoxString(nPane, text, wcslen(text));
    return 0;
}

// The four SetUp*Score functions have one shape. Two things a port must
// guard against, kept here as the game has them: with no cup won the
// difficulty (and so the pane) is left unset, and the averages and
// percentages divide by counts that can be 0 (the Wii's divw does not trap).
void msw::SelectMenu::CScoreBook::SetUpBasketScore()
{
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    CSave::PlayerData::SportScore &rScore = pPlayerData->m_BasketScore;

    for (long i = PANE_PAGE_0; i <= PANE_PAGE_3; i++) {
        m_pPaneAccessor->PlayAnimation(i, ANIM_SPORT_BASKET, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_BasketPanes[i], ANIM_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_VolleyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_DodgePanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_HockeyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
    }

    SetTextPaneString(9, rScore.m_nPlayNum, 1);

    // The highest difficulty with a cup won.
    unsigned char *pCupBit = pPlayerData->m_Cup[0].m_nCupBit;
    long nLevelBit = pCupBit[0] | pCupBit[1] | pCupBit[2] | pCupBit[3];
    long nLevel;
    if (nLevelBit & 1) {
        nLevel = 0;
    }
    if (nLevelBit & 2) {
        nLevel = 1;
    }
    if (nLevelBit & 4) {
        nLevel = 2;
    }

    nw4r::lyt::Pane *pLevelPane;
    switch (nLevel) {
        case 0:
            SetTextPaneString(0x36, rScore.m_nLevelNum[0], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_0);
            break;
        case 1:
            SetTextPaneString(0x38, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x39, rScore.m_nLevelNum[1], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_1);
            break;
        case 2:
            SetTextPaneString(0x3B, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x3C, rScore.m_nLevelNum[1], -1);
            SetTextPaneString(0x3D, rScore.m_nLevelNum[2], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_2);
            break;
    }
    nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
    bcs::G2D::SetPaneParent(pLevelPane, m_pPaneAccessor->GetPane(PANE_SPORT_LEVEL), 1, 0);
    pLevelPane->SetTranslate(pos);

    SetTextPaneString(0xB, rScore.m_nValue[0] * 100 / rScore.m_nPlayNum, 0);
    SetTextPaneString(0xC, rScore.m_nValue[1] / rScore.m_nPlayNum, 2);
    SetTextPaneString(0xD, rScore.m_nValue[2] * 100 / rScore.m_nValue[3], 0);
    SetTextPaneString(0xE, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ba_00"));
    SetTextPaneString(0xF, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ba_01"));
    SetTextPaneString(0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ba_02"));
    SetTextPaneString(0x11, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ba_03"));
    SetTextPaneString(0x12, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ba_04"));
    SetTextPaneString(0x13, rScore.m_nValue[5] * 100 / rScore.m_nValue[4], 0);
    SetTextPaneString(0x14, rScore.m_nValue[7] * 100 / rScore.m_nValue[6], 0);
    SetTextPaneString(0x15, rScore.m_nValue[9] * 100 / rScore.m_nValue[8], 0);
    SetTextPaneString(0x16, rScore.m_nValue[11] * 100 / rScore.m_nValue[10], 0);
    SetTextPaneString(0x17, rScore.m_nValue[12] * 100 / rScore.m_nValue[13], 0);
}

void msw::SelectMenu::CScoreBook::SetUpVolleyScore()
{
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    CSave::PlayerData::SportScore &rScore = pPlayerData->m_VolleyScore;

    for (long i = PANE_PAGE_0; i <= PANE_PAGE_3; i++) {
        m_pPaneAccessor->PlayAnimation(i, ANIM_SPORT_VOLLEY, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_BasketPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_VolleyPanes[i], ANIM_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_DodgePanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_HockeyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
    }

    SetTextPaneString(9, rScore.m_nPlayNum, 1);

    // The highest difficulty with a cup won.
    unsigned char *pCupBit = pPlayerData->m_Cup[1].m_nCupBit;
    long nLevelBit = pCupBit[0] | pCupBit[1] | pCupBit[2] | pCupBit[3];
    long nLevel;
    if (nLevelBit & 1) {
        nLevel = 0;
    }
    if (nLevelBit & 2) {
        nLevel = 1;
    }
    if (nLevelBit & 4) {
        nLevel = 2;
    }

    nw4r::lyt::Pane *pLevelPane;
    switch (nLevel) {
        case 0:
            SetTextPaneString(0x36, rScore.m_nLevelNum[0], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_0);
            break;
        case 1:
            SetTextPaneString(0x38, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x39, rScore.m_nLevelNum[1], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_1);
            break;
        case 2:
            SetTextPaneString(0x3B, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x3C, rScore.m_nLevelNum[1], -1);
            SetTextPaneString(0x3D, rScore.m_nLevelNum[2], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_2);
            break;
    }
    nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
    bcs::G2D::SetPaneParent(pLevelPane, m_pPaneAccessor->GetPane(PANE_SPORT_LEVEL), 1, 0);
    pLevelPane->SetTranslate(pos);

    SetTextPaneString(0xB, rScore.m_nValue[0] * 100 / rScore.m_nPlayNum, 0);
    SetTextPaneString(0xC, rScore.m_nValue[1] / rScore.m_nPlayNum, 2);
    SetTextPaneString(0xD, rScore.m_nValue[2] * 100 / rScore.m_nValue[3], 0);
    SetTextPaneString(0xE, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Vo_00"));
    SetTextPaneString(0xF, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Vo_01"));
    SetTextPaneString(0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Vo_02"));
    SetTextPaneString(0x11, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Vo_03"));
    SetTextPaneString(0x12, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Vo_04"));
    SetTextPaneString(0x13, rScore.m_nValue[5] * 100 / rScore.m_nValue[4], 0);
    SetTextPaneString(0x14, rScore.m_nValue[7] * 100 / rScore.m_nValue[6], 0);
    SetTextPaneString(0x15, rScore.m_nValue[9] * 100 / rScore.m_nValue[8], 0);
    SetTextPaneString(0x16, rScore.m_nValue[11] * 100 / rScore.m_nValue[10], 0);
    SetTextPaneString(0x17, rScore.m_nValue[13] * 100 / rScore.m_nValue[12], 0);
}

void msw::SelectMenu::CScoreBook::SetUpDodgeScore()
{
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    CSave::PlayerData::SportScore &rScore = pPlayerData->m_DodgeScore;

    for (long i = PANE_PAGE_0; i <= PANE_PAGE_3; i++) {
        m_pPaneAccessor->PlayAnimation(i, ANIM_SPORT_DODGE, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_BasketPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_VolleyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_DodgePanes[i], ANIM_ON, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_HockeyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
    }

    SetTextPaneString(9, rScore.m_nPlayNum, 1);

    // The highest difficulty with a cup won.
    unsigned char *pCupBit = pPlayerData->m_Cup[2].m_nCupBit;
    long nLevelBit = pCupBit[0] | pCupBit[1] | pCupBit[2] | pCupBit[3];
    long nLevel;
    if (nLevelBit & 1) {
        nLevel = 0;
    }
    if (nLevelBit & 2) {
        nLevel = 1;
    }
    if (nLevelBit & 4) {
        nLevel = 2;
    }

    nw4r::lyt::Pane *pLevelPane;
    switch (nLevel) {
        case 0:
            SetTextPaneString(0x36, rScore.m_nLevelNum[0], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_0);
            break;
        case 1:
            SetTextPaneString(0x38, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x39, rScore.m_nLevelNum[1], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_1);
            break;
        case 2:
            SetTextPaneString(0x3B, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x3C, rScore.m_nLevelNum[1], -1);
            SetTextPaneString(0x3D, rScore.m_nLevelNum[2], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_2);
            break;
    }
    nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
    bcs::G2D::SetPaneParent(pLevelPane, m_pPaneAccessor->GetPane(PANE_SPORT_LEVEL), 1, 0);
    pLevelPane->SetTranslate(pos);

    SetTextPaneString(0xB, rScore.m_nValue[0] * 100 / rScore.m_nPlayNum, 0);
    SetTextPaneString(0xC, rScore.m_nValue[1] / rScore.m_nPlayNum, 2);
    SetTextPaneString(0xD, rScore.m_nValue[2] * 100 / rScore.m_nValue[3], 0);
    SetTextPaneString(0xE, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Do_00"));
    SetTextPaneString(0xF, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Do_01"));
    SetTextPaneString(0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Do_02"));
    SetTextPaneString(0x11, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Do_03"));
    SetTextPaneString(0x12, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Do_04"));
    SetTextPaneString(0x13, rScore.m_nValue[5] * 100 / rScore.m_nValue[4], 0);
    SetTextPaneString(0x14, rScore.m_nValue[7] * 100 / rScore.m_nValue[6], 0);
    SetTextPaneString(0x15, rScore.m_nValue[9] * 100 / rScore.m_nValue[8], 0);
    SetTextPaneString(0x16, rScore.m_nValue[11] * 100 / rScore.m_nValue[10], 0);
    SetTextPaneString(0x17, rScore.m_nValue[13] * 100 / rScore.m_nValue[12], 0);
}

void msw::SelectMenu::CScoreBook::SetUpHockeyScore()
{
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);
    CSave::PlayerData::HockeyScore &rScore = pPlayerData->m_HockeyScore;

    for (long i = PANE_PAGE_0; i <= PANE_PAGE_3; i++) {
        m_pPaneAccessor->PlayAnimation(i, ANIM_SPORT_HOCKEY, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_BasketPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_VolleyPanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_DodgePanes[i], ANIM_OFF, 1.0f, 0.0f, 1, 0);
        m_pPaneAccessor->PlayAnimation(s_HockeyPanes[i], ANIM_ON, 1.0f, 0.0f, 1, 0);
    }

    SetTextPaneString(9, rScore.m_nPlayNum, 1);

    // The highest difficulty with a cup won.
    unsigned char *pCupBit = pPlayerData->m_Cup[3].m_nCupBit;
    long nLevelBit = pCupBit[0] | pCupBit[1] | pCupBit[2] | pCupBit[3];
    long nLevel;
    if (nLevelBit & 1) {
        nLevel = 0;
    }
    if (nLevelBit & 2) {
        nLevel = 1;
    }
    if (nLevelBit & 4) {
        nLevel = 2;
    }

    nw4r::lyt::Pane *pLevelPane;
    switch (nLevel) {
        case 0:
            SetTextPaneString(0x36, rScore.m_nLevelNum[0], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_0);
            break;
        case 1:
            SetTextPaneString(0x38, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x39, rScore.m_nLevelNum[1], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_1);
            break;
        case 2:
            SetTextPaneString(0x3B, rScore.m_nLevelNum[0], -1);
            SetTextPaneString(0x3C, rScore.m_nLevelNum[1], -1);
            SetTextPaneString(0x3D, rScore.m_nLevelNum[2], -1);
            pLevelPane = m_pPaneAccessor->GetPane(PANE_LEVEL_2);
            break;
    }
    nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
    bcs::G2D::SetPaneParent(pLevelPane, m_pPaneAccessor->GetPane(PANE_SPORT_LEVEL), 1, 0);
    pLevelPane->SetTranslate(pos);

    SetTextPaneString(0xB, rScore.m_nValue[0] * 100 / rScore.m_nPlayNum, 0);
    SetTextPaneString(0xC, rScore.m_nValue[1] / rScore.m_nPlayNum, 2);
    SetTextPaneString(0xD, rScore.m_nValue[2] * 100 / rScore.m_nValue[3], 0);
    SetTextPaneString(0xE, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ho_00"));
    SetTextPaneString(0xF, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ho_01"));
    SetTextPaneString(0x10, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ho_02"));
    SetTextPaneString(0x11, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ho_03"));
    SetTextPaneString(0x12, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Ho_04"));
    SetTextPaneString(0x13, rScore.m_nValue[5] * 100 / rScore.m_nValue[4], 0);
    SetTextPaneString(0x14, rScore.m_nValue[7] * 100 / rScore.m_nValue[6], 0);
    SetTextPaneString(0x15, rScore.m_nValue[11] * 100 / rScore.m_nValue[10], 0);
    SetTextPaneString(0x16, rScore.m_nValue[13] * 100 / rScore.m_nValue[12], 0);
    SetTextPaneString(0x17, rScore.m_nValue[15] * 100 / rScore.m_nValue[14], 0);
}

// The place of a score in a ranking of RANK_NUM rows sorted from the
// highest: the rows below it move down one (the last one drops out) and the
// score is written into its row. -1 when it does not make the ranking.
// Inlined into SetUpCommonScore. The rows overlap: the game relies on its
// library's memcpy copying backwards when the destination is higher (a port
// should use memmove).
template <class T>
static inline long InsertRank(T *pRank, long nScore)
{
    for (long i = 0; i < msw::SelectMenu::CScoreBook::RANK_NUM; i++) {
        if (pRank[i].m_nScore < nScore) {
            memcpy(&pRank[i + 1], &pRank[i], (msw::SelectMenu::CScoreBook::RANK_NUM - 1 - i) * sizeof(T));
            pRank[i].m_nScore = nScore;
            return i;
        }
    }
    return -1;
}

void msw::SelectMenu::CScoreBook::SetUpCommonScore()
{
    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);

    SetTextPaneString(4, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Co_00"));
    SetTextPaneString(5, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Co_01"));
    SetTextPaneString(6, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Co_02"));
    SetTextPaneString(7, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Co_03"));
    SetTextPaneString(8, GetMessageLabelString("ScoreBookList", "SCOREBOOK_Co_04"));

    // The character ranking: the characters' records, then the Miis'.
    memset(m_CharaRank, 0, sizeof(m_CharaRank));
    CSelectMenu *pSelectMenu = GetSelectMenu();
    CSave::PlayerData::CharaScore *pChara = 0;
    switch (pSelectMenu->m_nGameKind) {
        case 0:
            pChara = pPlayerData->m_CharaScore[0];
            break;
        case 1:
            pChara = pPlayerData->m_CharaScore[1];
            break;
        case 2:
            pChara = pPlayerData->m_CharaScore[2];
            break;
        case 3:
            pChara = pPlayerData->m_CharaScore[3];
            break;
    }
    for (long i = 0; i < RANK_NUM; i++) {
        m_CharaRank[i].m_nScore = -1;
    }

    for (long nChr = 0; nChr < CSave::PlayerData::CHARA_SCORE_NUM; nChr++, pChara++) {
        if (nChr != CHARA_NO_RANK && pChara->m_nPlayNum != 0) {
            long nRank = InsertRank(m_CharaRank, pChara->m_nScore);
            if (nRank != -1) {
                m_CharaRank[nRank].m_nChr = nChr;
            }
        }
    }

    for (long i = 0; i < pPlayerData->GetMiiCount(); i++) {
        CSave::PlayerData::MiiData *pMii = pPlayerData->GetIndexMii(i);
        long nScore;
        switch (pSelectMenu->m_nGameKind) {
            case 0:
                nScore = pMii->m_nScore[0];
                break;
            case 1:
                nScore = pMii->m_nScore[1];
                break;
            case 2:
                nScore = pMii->m_nScore[2];
                break;
            case 3:
                nScore = pMii->m_nScore[3];
                break;
        }
        long nRank = InsertRank(m_CharaRank, nScore);
        if (nRank != -1) {
            m_CharaRank[nRank].m_nChr = -1;
            m_CharaRank[nRank].m_Source = pMii->m_Source;
            m_CharaRank[nRank].m_CreateId = pMii->m_CreateId;

            RFLStoreData data;
            bcs::CMiiMan::GetStoreData(data, pMii->m_Source, pMii->m_CreateId);
            const wchar_t *pName;
            if (pMii->m_Source == RFLDataSource_Default) {
                pName = GetMessageString("List_Mii_Name", pMii->m_nNameId);
            } else {
                RFLAdditionalInfo info;
                bcs::CMiiMan::GetAdditionalInfo(info, data);
                pName = info.name;
            }
            wcscpy(m_Name, pName);
            wcscpy(m_CharaRank[nRank].m_Name, m_Name);
        }
    }

    for (long i = 0; i < GUEST_MII_NUM; i++) {
        CSave::PlayerData::MiiData *pMii = pPlayerData->GetGuestMii(i);
        long nScore;
        switch (pSelectMenu->m_nGameKind) {
            case 0:
                nScore = pMii->m_nScore[0];
                break;
            case 1:
                nScore = pMii->m_nScore[1];
                break;
            case 2:
                nScore = pMii->m_nScore[2];
                break;
            case 3:
                nScore = pMii->m_nScore[3];
                break;
        }
        long nRank = InsertRank(m_CharaRank, nScore);
        if (nRank != -1) {
            m_CharaRank[nRank].m_nChr = -1;
            m_CharaRank[nRank].m_Source = pMii->m_Source;
            m_CharaRank[nRank].m_CreateId = pMii->m_CreateId;

            RFLStoreData data;
            bcs::CMiiMan::GetStoreData(data, pMii->m_Source, pMii->m_CreateId);
            const wchar_t *pName;
            if (pMii->m_Source == RFLDataSource_Default) {
                pName = GetMessageString("List_Mii_Name", pMii->m_nNameId);
            } else {
                RFLAdditionalInfo info;
                bcs::CMiiMan::GetAdditionalInfo(info, data);
                pName = info.name;
            }
            wcscpy(m_Name, pName);
            wcscpy(m_CharaRank[nRank].m_Name, m_Name);
        }
    }

    // Rows with the same score share a place. The place mark of each row is
    // a copy, called pOrderName, of the mark's pane.
    long nPlaceScore = m_CharaRank[0].m_nScore;
    const char *pOrderName = "ChOrder";
    long nPlace = 0;
    for (long i = 0; i < RANK_NUM; i++) {
        SetTextPaneString(s_ScorePanes[i], m_CharaRank[i].m_nScore, 1);
        if (m_CharaRank[i].m_nChr == -1) {
            SetTextPaneString(s_NamePanes[i], m_CharaRank[i].m_Name);
            RFLStoreData data;
            bcs::CMiiMan::GetStoreData(data, m_CharaRank[i].m_Source, m_CharaRank[i].m_CreateId);
            CreateMiiFacePane(i, data);
        } else {
            SetTextPaneString(s_NamePanes[i], GetMessageString("List_Character", m_CharaRank[i].m_nChr));
            nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
            nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(PANE_CHARA_ICON + m_CharaRank[i].m_nChr);
            bcs::G2D::SetPaneParent(pIcon, m_pPaneAccessor->GetPane(s_FacePanes[i]), 1, 0);
            pIcon->SetTranslate(pos);
        }

        if (nPlaceScore != m_CharaRank[i].m_nScore) {
            nPlaceScore = m_CharaRank[i].m_nScore;
            nPlace = i;
        }
        nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
        const char *pRankName = s_RankNames[nPlace];
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_RankPanes[i]);
        bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
        bcs::G2D::CPaneObj *pCopy = bcs::G2D::DuplicatePaneObj(pLytObj, pRankName, pOrderName, s_LayoutName, 0);
        pLytObj->AppendPaneObj(pCopy, (const char *)0);
        nw4r::lyt::Pane *pPane = pCopy->GetPane();
        bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
        pPane->SetTranslate(pos);
    }

    // The stage ranking.
    for (long i = 0; i < RANK_NUM; i++) {
        m_StageRank[i].m_pName = 0;
        m_StageRank[i].m_nScore = -1;
    }
    for (long nStage = 1; nStage < 20; nStage++) {
        long nScore = pPlayerData->CheckStage(pSelectMenu->m_nGameKind, nStage);
        if (nScore >= 0) {
            long nRank = InsertRank(m_StageRank, nScore);
            if (nRank != -1) {
                m_StageRank[nRank].m_pName = GetMessageString("List_Stage", nStage);
            }
        }
    }

    nPlaceScore = m_StageRank[0].m_nScore;
    nPlace = 0;
    for (long i = 0; i < RANK_NUM; i++) {
        if (m_StageRank[i].m_nScore == -1) {
            SetTextPaneString(s_StageNamePanes[i], L"");
            SetTextPaneString(s_StageScorePanes[i], 0, -2);
        } else {
            SetTextPaneString(s_StageNamePanes[i], m_StageRank[i].m_pName);
            SetTextPaneString(s_StageScorePanes[i], m_StageRank[i].m_nScore, 1);
            if (nPlaceScore != m_StageRank[i].m_nScore) {
                nPlaceScore = m_StageRank[i].m_nScore;
                nPlace = i;
            }
            nw4r::math::VEC3 pos(0.0f, 0.0f, 0.0f);
            const char *pRankName = s_StageRankNames[nPlace];
            nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(s_StageRankPanes[i]);
            bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
            bcs::G2D::CPaneObj *pCopy = bcs::G2D::DuplicatePaneObj(pLytObj, pRankName, "StOrder", s_LayoutName, 0);
            pLytObj->AppendPaneObj(pCopy, (const char *)0);
            nw4r::lyt::Pane *pPane = pCopy->GetPane();
            bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
            pPane->SetTranslate(pos);
        }
    }
}

void msw::SelectMenu::CScoreBook::CreateMiiFacePane(long nRow, RFLStoreData data)
{
    bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();

    // The texture has the size of the row's picture pane (RGB5A3: two bytes
    // per texel).
    nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(s_FacePanes[nRow])->GetPane()->GetSize();
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(s_FacePanes[nRow]);
    long nTexSize = 2.0f * (size.width * size.height);
    unsigned char *pTex = new (1) unsigned char[nTexSize];
    m_pFaceTex[nRow] = pTex;

    GXColor bgColor = { 0, 0, 0, 0 };
    RFLIconSetting setting;
    setting.width = (long)size.width;
    setting.height = (long)size.height;
    setting.bgType = 1;
    setting.bgColor = bgColor;
    setting.drawXluOnly = 0;
    bcs::CMiiMan::MakeFaceIcon(m_pFaceTex[nRow], data, 0, setting, 1);

    GXTexObj texObj;
    GXInitTexObj(&texObj, m_pFaceTex[nRow], (long)size.width, (long)size.height, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP,
                 0);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    menu::CreateMiiPicturePane(&texObj, pLytObj, pPane, s_FaceNames[nRow]);
}
