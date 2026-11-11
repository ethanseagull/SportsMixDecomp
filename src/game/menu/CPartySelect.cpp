#include "game/msw/SelectMenu/CPartySelect.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "nw4r/lyt/Pane.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the window's opening and closing animations. A
    // table, not a scalar, as in game/menu/CScoreTop.cpp: the original
    // converts the value to float at run time and keeps the word, first in
    // .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_PARTY_SELECT = 0x19,
        OBJ_UNK_40 = 0x40
    };

    // The button kind ids of the screen: the back button and the four
    // party games. SetDefaultMoveCursor falls back to the first game.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_GAME_0 = 0x8010,
        BTN_KIND_GAME_1 = 0x8011,
        BTN_KIND_GAME_2 = 0x8012,
        BTN_KIND_GAME_3 = 0x8013
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SELECT = 0,
        STEP_CLOSE = 1
    };

    // The modes the screen leads to.
    enum {
        MODE_UNK_12 = 0x12,
        MODE_PARTY_GAME = 0x13
    };
}

void msw::CSelectMenu::PartySelect_In()
{
    static const long padNo0 = 0;

    Build(OBJ_PARTY_SELECT);
    SetFadeIn(10.0f);
    SendObjEventTrigger(OBJ_TOP, 0x11, (void *)GetMessageLabelString("PartyMenuMessage", "PAR_01"));
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PARTY_SELECT);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        if (m_Unk23E == 1) {
            pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        }
        rBtnKindId = nBtnKindId;
    }

    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::PartySelect_Loop()
{
    switch (m_nStep) {
        case STEP_SELECT:
            PartySelect_Loop0();
            break;
        case STEP_CLOSE:
            PartySelect_Loop1();
            break;
    }
}

void msw::CSelectMenu::PartySelect_Loop0()
{
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PARTY_SELECT);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & 0x10) {
        float fWait = _GetModeChangeWaitTime();
        long nPartyGame = -1;
        long nGameKind = -1;
        switch (nBtnKindId) {
            case BTN_KIND_GAME_0:
                nPartyGame = 1;
                nGameKind = 0;
                break;
            case BTN_KIND_GAME_1:
                nPartyGame = 3;
                nGameKind = 1;
                break;
            case BTN_KIND_GAME_2:
                nPartyGame = 5;
                nGameKind = 2;
                break;
            case BTN_KIND_GAME_3:
                nPartyGame = 7;
                nGameKind = 3;
                break;
            default:
                SetNextMode(MODE_UNK_12, fWait);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
                PartySelect_ChangeStateInit(m_nNextMode);
                PlayMenuBGM(1);
                SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
                SetWhiteFadeOut(10.0f);
                break;
            case BTN_KIND_NONE:
                break;
        }

        if (nPartyGame >= 0) {
            pObj->PushButton(padNo0, nBtnKindId);
            m_Unk9C = nPartyGame;
            m_nGameKind = nGameKind;
            PartySelect_ChangeStateInit(MODE_PARTY_GAME);
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & 0x20) {
        GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
        PlayMenuBGM(1);
        PartySelect_ChangeStateInit(m_nNextMode);
        SendObjEventTrigger(OBJ_UNK_40, 0x47, 0);
        SetWhiteFadeOut(10.0f);
    } else {
        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
    }
}

void msw::CSelectMenu::PartySelect_ChangeStateInit(long nMode)
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PARTY_SELECT);
    if (pObj != 0) {
        pObj->m_Unk30 = CSelectMenuLayoutBase::STATE_CLOSE;
    }
    m_nNextMode = nMode;
    m_nStep = STEP_CLOSE;
}

void msw::CSelectMenu::PartySelect_Loop1()
{
    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_PARTY_SELECT);
    if (pObj == 0 || pObj->m_Unk30 == CSelectMenuLayoutBase::STATE_NONE) {
        SetNextMode(m_nNextMode, 0.0f);
    }
}

void msw::CSelectMenu::PartySelect_Out()
{
    Scratch(OBJ_PARTY_SELECT);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata; a const pointer lands in .rodata behind its string, the other
// tables in .data.
namespace {

    // The window's animations, by index into the names the button file
    // lists (the names are not in the module).
    enum {
        ANIM_WIN_IN_BACK = 5,
        ANIM_WIN_OUT_BACK = 6,
        ANIM_WIN_IN = 7,
        ANIM_WIN_OUT = 9,
        ANIM_MOVIE_IN = 0xB // played on an icon when its movie is shown
    };

    enum {
        PANE_WIN = 0
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    const char *const s_LayoutName = "me_ptg_sel.brlyt";
    const char *const s_ButtonFileName = "me_ptg_sel.btn";

    const char *s_MovieName = "movie/per_exp.thp";

    // The picture panes made for the movie, one per game icon.
    const char *s_MoviePicNames[msw::SelectMenu::CPartySelect::MOVIE_PANE_NUM] = {
        "moviePic_01",
        "moviePic_02",
        "moviePic_03",
        "moviePic_04"
    };

    // The game icons the movie is shown on, by index into the button
    // file's pane names.
    const unsigned char s_MoviePanes[msw::SelectMenu::CPartySelect::MOVIE_PANE_NUM] = { 9, 10, 11, 12 };

    // What CreatePicturePane is given for the movie panes (meaning unknown).
    const unsigned char MOVIE_PANE_ARG = 3;

    // Frames before the movie is shown on an icon.
    const float MOVIE_WAIT = 30.0f;
}

msw::SelectMenu::CPartySelect::CPartySelect(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

msw::SelectMenu::CPartySelect::~CPartySelect()
{
    Remove();
}

void msw::SelectMenu::CPartySelect::Main()
{
    MovieControl();

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
                m_nMovieState = MOVIE_NONE;
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

void msw::SelectMenu::CPartySelect::MovieControl()
{
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie == 0) {
        return;
    }

    switch (m_nMovieState) {
        case MOVIE_STOP:
            m_nMovieState = MOVIE_OPEN;
            pMovie->Stop();
            break;
        case MOVIE_OPEN:
            if (pMovie->IsWait() == 1 && bcs::CSnd::IsReady() && pMovie->Open(s_MovieName, 1, 0) == 1) {
                m_nMovieState = MOVIE_START;
            }
            break;
        case MOVIE_START:
            if (pMovie->IsReady() && bcs::CSnd::IsReady() && pMovie->Start(0)) {
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

                static const int moviePaneNum = MOVIE_PANE_NUM;

                for (long i = 0; i < moviePaneNum; i++) {
                    nw4r::lyt::Pane *pPane = bcs::G2D::CreatePicturePane(s_MoviePicNames[i], MOVIE_PANE_ARG);
                    if (pPane == 0) {
                        continue;
                    }

                    bcs::G2D::CPaneObj *pPaneObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPane);
                    if (pPaneObj == 0) {
                        continue;
                    }

                    bcs::G2D::CPaneObj *pIcon = m_pPaneAccessor->GetPaneObj(s_MoviePanes[i]);

                    pPaneCtrl->AppendPaneObj(pPaneObj, (const char *)0);
                    bcs::G2D::SetPaneParent(pPaneObj->GetPane(), pIcon->GetPane(), 0, 0);
                    pPaneObj->GetPane()->SetSize(pIcon->GetPane()->GetSize());
                    bcs::G2D::SetupMoviePane(pPaneObj, pIcon);
                }
                m_nMovieState = MOVIE_PLAY;
            }
            break;
        case MOVIE_PLAY:
            if (pMovie->IsPlay() && bcs::CSnd::IsReady()) {
                static const int moviePaneNum = MOVIE_PANE_NUM;

                for (long i = 0; i < moviePaneNum; i++) {
                    bcs::G2D::RemapMoviePane(m_pPaneAccessor->GetPane(s_MoviePicNames[i], 1, 0));
                    if (pMovie->CanbeDraw() && m_bMovieWait[i] != 0) {
                        m_fMovieWait[i] -= bcs::CMainApp::GetAppFrameRateF();
                        if (m_fMovieWait[i] < 0.0f) {
                            m_pPaneAccessor->SetPaneDisp(s_MoviePanes[i], 1);
                            m_pPaneAccessor->PlayAnimation(s_MoviePanes[i], ANIM_MOVIE_IN, 1.0f, 0.0f, 1, 1);
                            m_bMovieWait[i] = 0;
                        }
                    }
                }
            } else if (!pMovie->CanbeDraw()) {
                m_nMovieState = MOVIE_NONE;
            }
            break;
    }
}

void msw::SelectMenu::CPartySelect::Disp()
{
}

int msw::SelectMenu::CPartySelect::Set()
{
    CMenuPartList *pMenuPartList;
    const char *pFileName;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
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

            pMenuPartList = new CMenuPartList;

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

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
            ClearPushButtonId();

            InitPaneAnim();

            // Hide the movie panes until their timers run out
            // (MovieControl).
            static const int moviePaneNum = MOVIE_PANE_NUM;

            m_nMovieState = MOVIE_STOP;
            for (long i = 0; i < moviePaneNum; i++) {
                m_pPaneAccessor->SetPaneDisp(s_MoviePanes[i], 0);
                m_bMovieWait[i] = 1;
                m_fMovieWait[i] = MOVIE_WAIT;
            }
        }
    }
    return 0;
}

void msw::SelectMenu::CPartySelect::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();

    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0 && pMovie->IsPlay()) {
        pMovie->Stop();
    }

    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CPartySelect::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CPartySelect::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartySelect::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartySelect::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CPartySelect::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CPartySelect::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CPartySelect::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_GAME_0;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CPartySelect::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CPartySelect::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

// Inlined into Main() although defined after it; the out-of-line copy is
// not called and the link drops it (as in game/menu/CScoreTop.cpp; marked
// `inline`, the compiler leaves its static out). Defined down here because
// its local static follows Set()'s constants in .rodata.
void msw::SelectMenu::CPartySelect::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

// Shows the four game icons the movie will be put on. Inlined into Set(),
// as CheckButtonAnim(); which of Set()'s two loops is this inline and which
// is Set()'s own is a guess (this way round the registers come closer).
inline void msw::SelectMenu::CPartySelect::InitPaneAnim()
{
    static const int moviePaneNum = MOVIE_PANE_NUM;

    for (long i = 0; i < moviePaneNum; i++) {
        bcs::G2D::CPaneObj *pIcon = m_pPaneAccessor->GetPaneObj(s_MoviePanes[i]);
        if (pIcon != 0) {
            pIcon->GetPane()->SetVisible(true);
        }
    }
}
