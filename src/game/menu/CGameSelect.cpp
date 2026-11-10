#include "game/msw/SelectMenu/CGameSelect.hpp"

#include "bcs/CMovie.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/SelectMenu/CBgMdl.hpp"
#include "nw4r/lyt/Pane.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // [0] is the speed of the frame's opening and closing animations; [1]
    // is not read here. A table, not a scalar, as in game/menu/CHowTo.cpp:
    // the original converts the value to float at run time and keeps both
    // words, first in .rodata.
    const long s_WinAnimSpeed[2] = { 2, 0 };

    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_FLAME_IN,
        ANIM_FLAME_OUT,
        ANIM_G_VIS_PIC,
        ANIM_G_VIS_MOV,
        ANIM_WIN,
        ANIM_NUM
    };

    enum {
        PANE_T_GAMSELE01,
        PANE_T_GAMSELE02,
        PANE_T_GAMSELE03,
        PANE_T_GAMSELE04,
        PANE_RULE_ICO,
        PANE_SCOREBOOK_ICO,
        PANE_T_GAMSELE01_B01,
        PANE_T_GAMSELE02_B01,
        PANE_T_GAMSELE03_B01,
        PANE_T_GAMSELE04_B01,
        PANE_B_RULE_ICO,
        PANE_B_SCOREBOOK_ICO,
        PANE_I_GAMSELE01,
        PANE_I_GAMSELE02,
        PANE_I_GAMSELE03,
        PANE_I_GAMSELE04,
        PANE_POPUP01,
        PANE_POPUP02,
        PANE_GAME_KASAN,
        PANE_ICO_KASAN,
        PANE_BTN_GAME,
        PANE_POS,
        PANE_TV_R,
        PANE_MOVIEPIC_01,
        PANE_MOVIEPIC_02,
        PANE_MOVIEPIC_03,
        PANE_MOVIEPIC_04,
        PANE_NUM,
        PANE_NONE = 0xFF
    };

    enum {
        BUTTON_NUM = 6,
        MOVIE_PANE_NUM = 4
    };

    // The kind ids of the four game buttons and the rules and score book
    // buttons; the cursor falls back to the first game.
    enum {
        BTN_KIND_RULE = 3,
        BTN_KIND_SCOREBOOK = 4,
        BTN_KIND_GAME_0 = 0x16,
        BTN_KIND_GAME_1 = 0x17,
        BTN_KIND_GAME_2 = 0x18,
        BTN_KIND_GAME_3 = 0x19,
        BTN_KIND_DEFAULT = BTN_KIND_GAME_0
    };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_HIT = 0x13,
        EVENT_BACK = 0x20,
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // Menu object of CSelectMenu that shows the ball of the chosen game.
    enum {
        OBJ_BG_MDL = 0x1D
    };

    enum {
        BALL_NONE = 0xFF,
        BALL_NUM = 4
    };

    const char *const s_LayoutName = "main_game.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "main_game_Btn_00.brlan",
        "main_game_Btn_01.brlan",
        "main_game_Btn_02.brlan",
        "main_game_Btn_03.brlan",
        "main_game_Btn_04.brlan",
        "main_game_flame_in.brlan",
        "main_game_flame_out.brlan",
        "main_game_G_vis_pic.brlan",
        "main_game_G_vis_mov.brlan",
        "main_game_win_anm_00.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "t_gamsele01",
        "t_gamsele02",
        "t_gamsele03",
        "t_gamsele04",
        "rule_ico",
        "scorebook_ico",
        "t_gamsele01_b01",
        "t_gamsele02_b01",
        "t_gamsele03_b01",
        "t_gamsele04_b01",
        "b_rule_ico",
        "b_scorebook_ico",
        "i_gamsele01",
        "i_gamsele02",
        "i_gamsele03",
        "i_gamsele04",
        "popup01",
        "popup02",
        "game_kasan",
        "ico_kasan",
        "btn_game",
        "pos",
        "tv_R",
        "moviePic_01",
        "moviePic_02",
        "moviePic_03",
        "moviePic_04"
    };

    // Size 0x14. The constant part of a button's description.
    struct ButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned char m_nIconPane;          // 0x02, PANE_NONE when there is none
        unsigned short m_nNextBtnKindId[5]; // 0x04
        unsigned char m_nBall;              // 0x0E, BALL_NONE when there is none
        int m_bStickMode;                   // 0x10
    };

    const ButtonParam s_ButtonParam[BUTTON_NUM] = {
        { 0, PANE_T_GAMSELE01_B01, PANE_I_GAMSELE01,
          { BTN_KIND_GAME_0, BTN_KIND_GAME_0, BTN_KIND_GAME_3, BTN_KIND_GAME_1, BTN_KIND_GAME_0 }, 0, 1 },
        { 1, PANE_T_GAMSELE02_B01, PANE_I_GAMSELE02,
          { BTN_KIND_GAME_2, BTN_KIND_GAME_1, BTN_KIND_GAME_2, BTN_KIND_GAME_2, BTN_KIND_GAME_3 }, 2, 1 },
        { 2, PANE_T_GAMSELE03_B01, PANE_I_GAMSELE03,
          { BTN_KIND_GAME_1, BTN_KIND_GAME_1, BTN_KIND_GAME_2, BTN_KIND_GAME_1, BTN_KIND_GAME_0 }, 1, 1 },
        { 3, PANE_T_GAMSELE04_B01, PANE_I_GAMSELE04,
          { BTN_KIND_GAME_3, BTN_KIND_GAME_0, BTN_KIND_GAME_3, BTN_KIND_GAME_2, BTN_KIND_GAME_3 }, 3, 1 },
        { 4, PANE_B_RULE_ICO, PANE_NONE, { BTN_KIND_RULE, 0, 0, 0, 0 }, BALL_NONE, 0 },
        { 5, PANE_B_SCOREBOOK_ICO, PANE_NONE, { BTN_KIND_SCOREBOOK, 0, 0, 0, 0 }, BALL_NONE, 0 }
    };

    const char *s_MovieName = "movie/top.thp";

    // The panes the movie is shown in, one per game icon.
    const unsigned char s_MoviePanes[MOVIE_PANE_NUM] = {
        PANE_MOVIEPIC_01,
        PANE_MOVIEPIC_02,
        PANE_MOVIEPIC_03,
        PANE_MOVIEPIC_04
    };

    // What CreatePicturePane is given for the movie panes (meaning unknown).
    const unsigned char MOVIE_PANE_ARG = 3;

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

msw::SelectMenu::CGameSelect::CGameSelect(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_nMovieState = MOVIE_NONE;
    m_Unk5C = 0;
}

msw::SelectMenu::CGameSelect::~CGameSelect()
{
    Remove();
}

void msw::SelectMenu::CGameSelect::Main()
{
    MovieControl();

    switch (m_Unk30) {
        case STATE_SET: {
            // The game stores these four statics in the reverse of the
            // order their symbols are numbered in; all are 1, so the bytes
            // are the same and only the symbol addresses are exchanged:
            // static-order swap (see notes/decisions.md).
            static const int recursive = 1;
            static const int reset = 1;
            m_pPaneAccessor->PlayAnimation(PANE_BTN_GAME, ANIM_FLAME_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_pPaneAccessor->PlayAnimation(PANE_POS, ANIM_FLAME_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_pPaneAccessor->PlayAnimation(PANE_TV_R, ANIM_FLAME_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
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
                static const int recursive = 1;
                static const int reset = 1;
                m_pPaneAccessor->PlayAnimation(PANE_BTN_GAME, ANIM_FLAME_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_POS, ANIM_FLAME_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_TV_R, ANIM_FLAME_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
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

void msw::SelectMenu::CGameSelect::MovieControl()
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
            if (pMovie->IsWait() == 1 && pMovie->Open(s_MovieName, 1, 0) == 1) {
                m_nMovieState = MOVIE_START;
            }
            break;
        case MOVIE_START:
            if (pMovie->IsReady() && pMovie->Start(0)) {
                bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

                static const int moviePaneNum = MOVIE_PANE_NUM;

                for (long i = 0; i < moviePaneNum; i++) {
                    nw4r::lyt::Pane *pPane = bcs::G2D::CreatePicturePane(s_PaneNames[s_MoviePanes[i]], MOVIE_PANE_ARG);
                    if (pPane == 0) {
                        continue;
                    }

                    bcs::G2D::CPaneObj *pPaneObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPane);
                    if (pPaneObj == 0) {
                        continue;
                    }

                    const ButtonParam *pParam = GetButtonParam(i);
                    bcs::G2D::CPaneObj *pIcon = m_pPaneAccessor->GetPaneObj(pParam->m_nIconPane);

                    pPaneCtrl->AppendPaneObj(pPaneObj, (const char *)0);
                    bcs::G2D::SetPaneParent(pPaneObj->GetPane(), pIcon->GetPane(), 0, 0);
                    pPaneObj->GetPane()->SetSize(pIcon->GetPane()->GetSize());
                    bcs::G2D::SetupMoviePane(pPaneObj, pIcon);
                }
                m_nMovieState = MOVIE_PLAY;
            }
            break;
    }
}

void msw::SelectMenu::CGameSelect::Disp()
{
}

int msw::SelectMenu::CGameSelect::Set()
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

            static const int btnNum = BUTTON_NUM;

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
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[1] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[2] = pParam->m_nNextBtnKindId[2];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[3];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[4];
                pPart->m_nSeCode[0] = 0xBC0;
                pPart->m_nSeCode[1] = 0xBBB;
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                // Written on the whole flag word, as in CButtonEntry.cpp.
                pPart->m_nFlags40 = pParam->m_bStickMode ? (pPart->m_nFlags40 | 0x0020) : (pPart->m_nFlags40 & ~0x0020);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            InitPaneAnim();
        }

        m_Unk2C = 1;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
        m_Unk5C = 0;
        m_nMovieState = MOVIE_STOP;
    }
    return 0;
}

void msw::SelectMenu::CGameSelect::Remove()
{
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

long msw::SelectMenu::CGameSelect::HitCheck(bcs::FVEC3 *pPos)
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

long msw::SelectMenu::CGameSelect::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CGameSelect::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CGameSelect::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CGameSelect::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CGameSelect::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CGameSelect::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList == 0) {
        return BTN_KIND_DEFAULT;
    }
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_DEFAULT;
}

int msw::SelectMenu::CGameSelect::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

inline void msw::SelectMenu::CGameSelect::SetIconVisible(unsigned short nBtnKindId, bool bVisible)
{
    const ButtonParam *pParam = FindButtonParam(nBtnKindId);
    if (pParam != 0) {
        bcs::G2D::CPaneObj *pIcon = m_pPaneAccessor->GetPaneObj(pParam->m_nIconPane);
        if (pIcon != 0) {
            pIcon->GetPane()->SetVisible(bVisible);
        }
    }
}

long msw::SelectMenu::CGameSelect::CheckButtonAnim()
{
    static const int backEvent = EVENT_BACK;

    unsigned long bSelected[BUTTON_NUM];

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, bSelected);
    long nResult = GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);

    for (long i = 0; i < BUTTON_NUM; i++) {
        const ButtonParam *pParam = &s_ButtonParam[i];
        if (bSelected[i] != 0) {
            if (pParam->m_nBall < BALL_NUM) {
                CBgMdl *pBgMdl = static_cast<CBgMdl *>(GetSelectMenu()->FindObj(OBJ_BG_MDL));
                if (pBgMdl != 0) {
                    pBgMdl->ChangeBall(pParam->m_nBall);
                }
            }
            SetIconVisible(pParam->m_nNextBtnKindId[0], true);
        } else {
            SetIconVisible(pParam->m_nNextBtnKindId[0], false);
        }
    }
    return nResult;
}

// Plays the frame's opening animation and hides every game icon. Inlined
// into Set(); it is defined down here because its local statics follow
// those of CheckButtonAnim() in .rodata.
inline void msw::SelectMenu::CGameSelect::InitPaneAnim()
{
    // The game stores `reset` before `recursive`; both are 1, so only the
    // two symbol addresses are exchanged: static-order swap (see
    // notes/decisions.md).
    static const int recursive = 1;
    static const int reset = 1;

    m_pPaneAccessor->PlayAnimation(PANE_POS, ANIM_WIN, 1.0f, 0.0f, recursive, reset);
    for (long i = 0; i < BUTTON_NUM; i++) {
        SetIconVisible(s_ButtonParam[i].m_nNextBtnKindId[0], false);
    }
}

long msw::SelectMenu::CGameSelect::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}
