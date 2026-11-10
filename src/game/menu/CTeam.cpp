#include "game/msw/SelectMenu/CTeam.hpp"

#include "game/msw/CCameraMenuBG.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CChrCaption.hpp"
#include "game/msw/SelectMenu/CCourtChr.hpp"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations; [1]
    // is not read here. A table, not a scalar, as in game/menu/CHowTo.cpp:
    // the original converts the value to float at run time and keeps both
    // words, first in .rodata.
    const long s_WinAnimSpeed[2] = { 2, 0 };

    // The play modes (CSelectMenu::m_nPlayMode) of a team match, in four
    // groups; the team screen cycles through one group. Each list ends
    // with -1 (not read: the screen knows the last index of each).
    const long s_TeamGroup0[] = { 7, 20, 17, -1 };
    const long s_TeamGroup1[] = { 15, 21, 16, -1 };
    const long s_TeamGroup2[] = { 8, 22, 23, -1 };
    const long s_TeamGroup3[] = { 14, 10, 24, 25, 19, -1 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_UNK_00 = 0,
        OBJ_UNK_01 = 1,
        OBJ_TEAM = 0xE,
        OBJ_BG_CHR = 0x3A,
        OBJ_COURT_CHR = 0x3B,
        OBJ_CHR_CAPTION = 0x3C
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_BACK = 1,
        BTN_KIND_PATTERN = 0x36,     // what CallbackButtonAnimPush reacts to
        BTN_KIND_OK = 0x8010,
        BTN_KIND_SELECT = 0x8011,    // the pattern selector
        BTN_KIND_PREV = 0x8020,      // its two arrows
        BTN_KIND_NEXT = 0x8021
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08,
        PAD_A = 0x10,
        PAD_B = 0x20,
        PAD_POINTER = 0x100
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_CAMERA = 0,
        STEP_SELECT = 1,
        STEP_CLOSE = 2
    };

    // The mode a chosen team moves on to.
    enum {
        MODE_TEAM_CHARA_SELECT = 0xE
    };

    // Frames the pattern selector waits after each change.
    const float PATTERN_WAIT = 20.0f;
}

void msw::CSelectMenu::Team_In()
{
    static const long padNo0 = 0;

    Build(OBJ_CHR_CAPTION);
    Build(OBJ_TEAM);
    SendObjEventTrigger(OBJ_UNK_00, 0x11, (void *)GetMessageLabelString("ExhibitionMenuMessage", "TEM_02"));
    SendObjEventTrigger(OBJ_BG_CHR, 0x33, 0);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x35, (void *)1);
    SendObjEventTrigger(OBJ_COURT_CHR, 0x36, (void *)3);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TEAM);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
    }

    // Start from the first play mode of the group.
    switch (m_nPlayMode) {
        case 7:
        case 17:
        case 20:
            m_nPlayMode = 7;
            break;
        case 15:
        case 16:
        case 21:
            m_nPlayMode = 15;
            break;
        case 8:
        case 22:
        case 23:
            m_nPlayMode = 8;
            break;
        case 10:
        case 14:
        case 19:
        case 24:
        case 25:
            m_nPlayMode = 14;
            break;
    }

    m_fInputWait = 0.0f;
    if (SendObjEventTrigger(OBJ_UNK_01, 0x2C, 0) != 0) {
        m_nStep = STEP_SELECT;
    } else {
        m_nStep = STEP_CAMERA;
    }
}

void msw::CSelectMenu::Team_Loop()
{
    switch (m_nStep) {
        case STEP_CAMERA:
            if (CCameraMenuBG::s_Instance.IsEndAnim()) {
                m_nStep = STEP_SELECT;
            }
            break;
        case STEP_SELECT:
            Team_Loop0();
            break;
        case STEP_CLOSE:
            Team_Loop1();
            break;
    }
}

void msw::CSelectMenu::Team_Loop0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    if (!CCameraMenuBG::s_Instance.IsEndAnim()) {
        return;
    }
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TEAM);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nLast = 0;
    // Left unset for a play mode outside the four (Team_In() has already
    // moved m_nPlayMode to the first of its group); only read on OK.
    const long *pGroup;
    switch (m_nPlayMode) {
        case 7:
            pGroup = s_TeamGroup0;
            nLast = 2;
            break;
        case 15:
            pGroup = s_TeamGroup1;
            nLast = 2;
            break;
        case 8:
            pGroup = s_TeamGroup2;
            nLast = 2;
            break;
        case 14:
            pGroup = s_TeamGroup3;
            nLast = 4;
            break;
    }

    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];

    m_fInputWait -= GetFrameRateF();
    if (m_fInputWait > 0.0f) {
        return;
    }
    m_fInputWait = 0.0f;

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_POINTER) {
        switch (nBtnKindId) {
            case BTN_KIND_PREV: {
                long nPattern = m_Unk26C - 1;
                m_fInputWait = PATTERN_WAIT;
                if (nPattern < 0) {
                    nPattern = nLast;
                }
                m_Unk26C = nPattern;
                pObj->PushButton(padNo0, nBtnKindId);
                break;
            }
            case BTN_KIND_NEXT: {
                long nPattern = m_Unk26C + 1;
                m_fInputWait = PATTERN_WAIT;
                if (nPattern > nLast) {
                    nPattern = 0;
                }
                m_Unk26C = nPattern;
                pObj->PushButton(padNo0, nBtnKindId);
                break;
            }
        }
    }

    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_OK:
                m_nPlayMode = pGroup[m_Unk26C];
                pObj->PushButton(padNo0, nBtnKindId);
                Team_ChangeStateInit(MODE_TEAM_CHARA_SELECT);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                Team_ChangeStateInit(m_nNextMode);
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        Team_ChangeStateInit(m_nNextMode);
    } else {
        if (nBtnKindId == BTN_KIND_SELECT) {
            if (CMenuPad::s_Instance.GetInput(nPad) & PAD_LEFT) {
                long nPattern = m_Unk26C - 1;
                m_fInputWait = PATTERN_WAIT;
                if (nPattern < 0) {
                    nPattern = nLast;
                }
                m_Unk26C = nPattern;
                pObj->PushButton(padNo0, BTN_KIND_PREV);
            } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_RIGHT) {
                long nPattern = m_Unk26C + 1;
                m_fInputWait = PATTERN_WAIT;
                if (nPattern > nLast) {
                    nPattern = 0;
                }
                m_Unk26C = nPattern;
                pObj->PushButton(padNo0, BTN_KIND_NEXT);
            }
        }
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::Team_ChangeStateInit(long nMode)
{
    if (nMode == MODE_TEAM_CHARA_SELECT) {
        m_pPlayerData->ClearSelectMii();
        ChangeTeamCharaSelectParam();
    }

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TEAM);
    if (pObj != 0) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
    InitCharaSelectParam();
    m_nNextMode = nMode;
    m_nStep = STEP_CLOSE;
}

void msw::CSelectMenu::Team_Loop1()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_TEAM);
    if (pObj == 0 || pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
        SetNextMode(m_nNextMode, 0.0f);
    }
}

void msw::CSelectMenu::Team_Out()
{
    Scratch(OBJ_TEAM);
    Scratch(OBJ_CHR_CAPTION);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN = 0x11,
        ANIM_WIN_OUT = 0x13,
        ANIM_WIN_IN_BACK = 0x14,
        ANIM_WIN_OUT_BACK = 0x16,
        ANIM_COLOR_0 = 0x0A,         // one per game kind
        ANIM_COLOR_NONE = 0x0F
    };

    enum {
        PANE_WIN = 0,
        PANE_TEXT_TITLE = 4,
        PANE_TEXT_TEAM = 5,
        PANE_TEXT_PATTERN = 6
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    const char *const s_LayoutName = "me_multiplay_01.brlyt";
    const char *const s_ButtonFileName = "me_multiplay_01.btn";

    // Size 5. One character of a team pattern.
    struct TeamMember {
        unsigned char m_nId;      // 0x00, not read
        unsigned char m_nPlayer;  // 0x01, 0xFF: nobody stands here
        unsigned char m_nType;    // 0x02
        unsigned char m_nPos;     // 0x03
        unsigned char m_nCaption; // 0x04
    };

    enum {
        TEAM_MEMBER_NUM = 6,
        TEAM_GROUP_NUM = 4,
        TEAM_PATTERN_MAX = 5
    };

    // The patterns of group 0 (s_TeamGroup0).
    TeamMember s_Team0_0[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 2, 1 }, { 2, 2, 0, 4, 2 },
        { 3, 0, 0xFD, 5, 4 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };
    TeamMember s_Team0_1[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 4, 1 }, { 2, 2, 0, 2, 2 },
        { 3, 0, 0xFD, 5, 4 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };
    TeamMember s_Team0_2[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 4, 1 }, { 2, 2, 0, 5, 2 },
        { 3, 0, 0xFE, 2, 4 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };

    // Group 1.
    TeamMember s_Team1_0[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 1, 0, 1, 1 }, { 2, 0, 0xFE, 2, 4 },
        { 3, 2, 0, 3, 2 }, { 4, 0, 0xFD, 4, 4 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team1_1[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 2, 0, 1, 2 }, { 2, 0, 0xFE, 2, 4 },
        { 3, 1, 0, 3, 1 }, { 4, 0, 0xFD, 4, 4 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team1_2[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 0, 0xFE, 1, 4 }, { 2, 0, 0xFE, 2, 4 },
        { 3, 1, 0, 3, 1 }, { 4, 2, 0, 4, 2 }, { 5, 0, 0xFD, 5, 4 }
    };

    // Group 2.
    TeamMember s_Team2_0[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 2, 1 }, { 2, 2, 0, 4, 2 },
        { 3, 10, 0, 5, 3 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };
    TeamMember s_Team2_1[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 4, 1 }, { 2, 2, 0, 2, 2 },
        { 3, 10, 0, 5, 3 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };
    TeamMember s_Team2_2[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 1, 0 }, { 1, 1, 0, 4, 1 }, { 2, 2, 0, 5, 2 },
        { 3, 10, 0, 2, 3 }, { 4, 0xFF, 0xFF, 0, 4 }, { 5, 0xFF, 0xFF, 3, 4 }
    };

    // Group 3.
    TeamMember s_Team3_0[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 1, 0, 1, 1 }, { 2, 0, 0xFE, 2, 4 },
        { 3, 2, 0, 3, 2 }, { 4, 10, 0, 4, 3 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team3_1[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 1, 0, 1, 1 }, { 2, 2, 0, 2, 2 },
        { 3, 10, 0, 3, 3 }, { 4, 0, 0xFD, 4, 4 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team3_2[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 1, 0, 1, 1 }, { 3, 10, 0, 2, 3 },
        { 2, 2, 0, 3, 2 }, { 4, 0, 0xFD, 4, 4 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team3_3[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 2, 2, 0, 1, 2 }, { 3, 10, 0, 2, 3 },
        { 1, 1, 0, 3, 1 }, { 4, 0, 0xFD, 4, 4 }, { 5, 0, 0xFD, 5, 4 }
    };
    TeamMember s_Team3_4[TEAM_MEMBER_NUM] = {
        { 0, 0, 0, 0, 0 }, { 1, 0, 0xFE, 1, 4 }, { 2, 0, 0xFE, 2, 4 },
        { 3, 1, 0, 3, 1 }, { 4, 2, 0, 4, 2 }, { 5, 10, 0, 5, 3 }
    };

    struct TeamPattern {
        long m_nNum;
        TeamMember *m_pMember;
    };

    // Every pattern of every group, by group and CSelectMenu::m_Unk26C.
    const TeamPattern s_TeamPatterns[TEAM_GROUP_NUM][TEAM_PATTERN_MAX] = {
        { { TEAM_MEMBER_NUM, s_Team0_0 }, { TEAM_MEMBER_NUM, s_Team0_1 }, { TEAM_MEMBER_NUM, s_Team0_2 } },
        { { TEAM_MEMBER_NUM, s_Team1_0 }, { TEAM_MEMBER_NUM, s_Team1_1 }, { TEAM_MEMBER_NUM, s_Team1_2 } },
        { { TEAM_MEMBER_NUM, s_Team2_0 }, { TEAM_MEMBER_NUM, s_Team2_1 }, { TEAM_MEMBER_NUM, s_Team2_2 } },
        { { TEAM_MEMBER_NUM, s_Team3_0 }, { TEAM_MEMBER_NUM, s_Team3_1 }, { TEAM_MEMBER_NUM, s_Team3_2 },
          { TEAM_MEMBER_NUM, s_Team3_3 }, { TEAM_MEMBER_NUM, s_Team3_4 } }
    };
}

msw::SelectMenu::CTeam::CTeam(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CTeam::~CTeam()
{
    Remove();
}

void msw::SelectMenu::CTeam::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CScoreTop.cpp (see
            // notes/decisions.md).
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
            SelectTeamPatternText();
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

void msw::SelectMenu::CTeam::Disp()
{
}

int msw::SelectMenu::CTeam::Set()
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
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            // The window colour of the game kind. The game stores `reset`
            // before `recursive`; both are 1, so only the two symbol
            // addresses are exchanged: static-order swap (see
            // notes/decisions.md).
            static const int recursive = 1;
            static const int reset = 1;
            long nColorAnim;
            switch (GetSelectMenu()->m_nGameKind) {
                case 0:
                    nColorAnim = ANIM_COLOR_0;
                    break;
                case 1:
                    nColorAnim = ANIM_COLOR_0 + 1;
                    break;
                case 2:
                    nColorAnim = ANIM_COLOR_0 + 2;
                    break;
                case 3:
                    nColorAnim = ANIM_COLOR_0 + 3;
                    break;
                default:
                    nColorAnim = ANIM_COLOR_NONE;
                    break;
            }
            m_pPaneAccessor->PlayAnimation(PANE_WIN, nColorAnim, 1.0f, 0.0f, recursive, reset);

            const wchar_t *pText = GetMessageLabelString("ExhibitionMenuMessage", "TEM_01_00");
            m_pPaneAccessor->SetTextBoxString(PANE_TEXT_TEAM, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "TEM_01_01");
            m_pPaneAccessor->SetTextBoxString(PANE_TEXT_TITLE, pText, wcslen(pText));

            m_nPatternShown = -1;
            SelectTeamPatternText();

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CTeam::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CTeam::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CTeam::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTeam::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CTeam::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CTeam::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CTeam::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
    // The stick does not stop on the selector's arrows.
    switch (nNext) {
        case BTN_KIND_PREV:
        case BTN_KIND_NEXT:
            nNext = BTN_KIND_SELECT;
            break;
    }
    return nNext;
}

long msw::SelectMenu::CTeam::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_OK;
}

int msw::SelectMenu::CTeam::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CTeam::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it. Defined down here because its local
// static follows Set()'s constants in .rodata.
void msw::SelectMenu::CTeam::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Inlined into Main() and Set(), as CheckButtonAnim().
void msw::SelectMenu::CTeam::SelectTeamPatternText()
{
    static const int textBufLen = 0x20;

    long nPattern = GetSelectMenu()->m_Unk26C + 1;
    if (m_nPatternShown != nPattern) {
        wchar_t text[textBufLen];
        swprintf(text, textBufLen, GetMessageLabelString("ExhibitionMenuMessage", "TEM_03"), nPattern);
        m_pPaneAccessor->SetTextBoxString(PANE_TEXT_PATTERN, text, wcslen(text));
        SetCourtCharaObj(BTN_KIND_PATTERN);
        m_nPatternShown = nPattern;
    }
}

void msw::SelectMenu::CTeam::CallbackButtonAnimPush(long nBtnKindId)
{
    if (nBtnKindId == BTN_KIND_PATTERN) {
        SetCourtCharaObj(nBtnKindId);
    }
}

void msw::SelectMenu::CTeam::SetCourtCharaObj(long nBtnKindId)
{
    CCourtChr *pCourtChr = static_cast<CCourtChr *>(GetSelectMenu()->FindObj(OBJ_COURT_CHR));
    if (pCourtChr == 0) {
        return;
    }

    long nPattern;
    switch (nBtnKindId) {
        case BTN_KIND_PATTERN:
            nPattern = GetSelectMenu()->m_Unk26C;
            break;
        default:
            return;
    }

    long nGroup;
    switch (GetSelectMenu()->m_nPlayMode) {
        case 7:
        case 17:
        case 20:
            nGroup = 0;
            break;
        case 15:
        case 16:
        case 21:
            nGroup = 1;
            break;
        case 8:
        case 22:
        case 23:
            nGroup = 2;
            break;
        case 10:
        case 14:
        case 19:
        case 24:
        case 25:
            nGroup = 3;
            break;
        default:
            return;
    }

    const TeamPattern &rPattern = s_TeamPatterns[nGroup][nPattern];
    long nNum = rPattern.m_nNum;
    // One model handle per character, on the stack.
    unsigned long *phMdl = (unsigned long *)__alloca(nNum * sizeof(unsigned long));

    // `camNo` is stored before `starFlag`; both are 0, so only the two
    // symbol addresses are exchanged: static-order swap (see
    // notes/decisions.md).
    for (long i = 0; i < nNum; i++) {
        static const int starFlag = 0;
        const TeamMember *pMember = &rPattern.m_pMember[i];
        phMdl[i] = pCourtChr->SetCharaModel(i, pMember->m_nPlayer, pMember->m_nType, 0, pMember->m_nPos,
                                            (RFLDataSource)6, 0, starFlag);
        if (phMdl[i] != 0) {
            pCourtChr->SetCharaMotion(i, 1, 2, 1.0f, 0.0f, 0.0f, 0);
        }
    }

    CChrCaption *pCaption = static_cast<CChrCaption *>(GetSelectMenu()->FindObj(OBJ_CHR_CAPTION));
    if (pCaption == 0) {
        return;
    }

    bcs::FVEC3 offset(0.0f, 0.5f, 0.0f);
    for (long i = 0; i < nNum; i++) {
        static const long camNo = 0;
        const TeamMember *pMember = &rPattern.m_pMember[i];
        if (pMember->m_nPlayer != 0xFF) {
            if (phMdl[i] != 0) {
                pCaption->SetCaption(i, pMember->m_nCaption, pCourtChr->GetNextMdlHandleByCharaId(i), camNo, &offset);
            } else {
                pCaption->SetCaption(i, pMember->m_nCaption, pCourtChr->GetMdlHandleByCharaId(i), camNo, &offset);
            }
        }
    }
}
