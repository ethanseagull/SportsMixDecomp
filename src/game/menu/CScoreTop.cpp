#include "game/msw/SelectMenu/CScoreTop.hpp"

#include "bcs/CMainApp.hpp"
#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CTournament.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar: the original converts the value to float at run
    // time and keeps the word, first in .rodata (a scalar const is folded
    // to 2.0f and not emitted).
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_BG_CHR = 0x3A,
        OBJ_UNK_3B = 0x3B,
        OBJ_UNK_40 = 0x40,
        OBJ_SCORE_TOP = 0x43
    };

    // The button kind ids of the screen: one per game kind, and the back
    // button. SetDefaultMoveCursor falls back to the first.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_GAME_0 = 0x8010,
        BTN_KIND_GAME_1 = 0x8011,
        BTN_KIND_GAME_2 = 0x8012,
        BTN_KIND_GAME_3 = 0x8013
    };

    // The mode the score book moves on to when a game kind is chosen.
    enum {
        MODE_SCORE_BOOK = 8
    };

    // The camera position of the score book.
    enum {
        CAM_SCORE_BOOK = 0x29
    };
}

void msw::CSelectMenu::ScoreTop_In()
{
    CSelectMenuLayoutBase *pObj;
    static const long padNo0 = 0;

    Build(OBJ_SCORE_TOP);

    CSave::PlayerData *pPlayerData = CSave::GetPlayerData(-1);

    // The title line, with the player's name (taken to be at the start of
    // the player data) put into the message.
    wchar_t text[0x100];
    swprintf(text, 0xFF, GetMessageLabelString("TopMenuMessage", "SCOREBOOK_01_00"),
             (const wchar_t *)pPlayerData);
    SendObjEventTrigger(OBJ_UNK_00, 0x11, text);

    SendObjEventTrigger(OBJ_UNK_40, 0x47, (void *)1);
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x35, 0);
    SendObjEventTrigger(OBJ_UNK_3B, 0x3A, (void *)-1);
    CCameraMenuBG::s_Instance.Play(CAM_SCORE_BOOK, 1.0f, 0.0f);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_SCORE_TOP);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        if (m_Unk23E == 1) {
            pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        }
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }

    if (m_Unk9D == 4) {
        m_nSavedGameKind = m_nGameKind;
    }
    SetFadeIn(10.0f);
}

void msw::CSelectMenu::ScoreTop_Loop()
{
    // The game has both of these as symbols, `padNo0` numbered before
    // `objId` but stored after it: static-order swap (see
    // notes/decisions.md).
    static const long objId = OBJ_SCORE_TOP;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE ||
        bcs::CMainApp::IsPlayMasterFade()) {
        return;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        pObj->PushButton(padNo0, nBtnKindId);
        switch (nBtnKindId) {
            case BTN_KIND_GAME_0:
            case BTN_KIND_GAME_1:
            case BTN_KIND_GAME_2:
            case BTN_KIND_GAME_3:
                m_nGameKind = nBtnKindId - BTN_KIND_GAME_0;
                SetNextMode(MODE_SCORE_BOOK, pObj);
                break;
            case BTN_KIND_BACK:
                m_nGameKind = m_nSavedGameKind;
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                PlayMenuBGM(1);
                SetWhiteFadeOut(10.0f);
                SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        m_nGameKind = m_nSavedGameKind;
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        PlayMenuBGM(1);
        SetWhiteFadeOut(10.0f);
        SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::ScoreTop_Out()
{
    Scratch(OBJ_SCORE_TOP);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata; a const pointer lands in .rodata behind its string, the pane
// table in .data.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN_BACK = 5,
        ANIM_WIN_OUT_BACK = 6,
        ANIM_WIN_IN = 7,
        ANIM_WIN_OUT = 9
    };

    enum {
        PANE_WIN = 0
    };

    enum {
        CUP_NUM = 4
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    const char *const s_LayoutName = "me_scorebook_A.brlyt";
    const char *const s_ButtonFileName = "me_scorebook_A.btn";

    // The pane shown for each cup that has been won.
    long s_CupPanes[CUP_NUM] = { 10, 9, 11, 12 };
}

msw::SelectMenu::CScoreTop::CScoreTop(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CScoreTop::~CScoreTop()
{
    Remove();
}

void msw::SelectMenu::CScoreTop::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // The game numbers these `recursive`, `reset@0`, `recursive@1`,
            // `reset@2` and stores them in the reverse order; declared this
            // way the bytes are the original's and the names swapped:
            // static-order swap (see notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            if (IsBackFrameIn(0)) {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            } else {
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            }
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN_BACK)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                    m_Unk30 = STATE_ACTIVE;
                }
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                if (IsBackFrameOut(0)) {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT_BACK, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                } else {
                    m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                }
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            // The game asks IsBackFrameIn here, not IsBackFrameOut.
            if (IsBackFrameIn(0)) {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT_BACK)) {
                    m_Unk30 = STATE_NONE;
                }
            } else {
                if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                    m_Unk30 = STATE_NONE;
                }
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CScoreTop::Disp()
{
}

int msw::SelectMenu::CScoreTop::Set()
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            for (long i = 0; i < CUP_NUM; i++) {
                if (CTournament::IsCupClear(3, i, 0, -1)) {
                    m_pPaneAccessor->SetPaneDisp(s_CupPanes[i], 1);
                } else {
                    m_pPaneAccessor->SetPaneDisp(s_CupPanes[i], 0);
                }
            }

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CScoreTop::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CScoreTop::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CScoreTop::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreTop::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreTop::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CScoreTop::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CScoreTop::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CScoreTop::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_GAME_0;
}

int msw::SelectMenu::CScoreTop::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CScoreTop::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined down here because its local
// static follows Set()'s constants in .rodata.
void msw::SelectMenu::CScoreTop::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}
