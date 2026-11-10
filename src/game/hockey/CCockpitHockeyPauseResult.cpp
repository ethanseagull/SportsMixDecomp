// msw::CCockpitHockeyPauseResult: the hockey games' page of the pause score
// board, with the score of each period.
//
// Same code as game/basket/CCockpitBasketPauseResult.cpp; only the block
// marked "Sport-specific part" differs (the file names).
#include "game/msw/CCockpitHockeyPauseResult.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/menu/CMessage.hpp"

namespace {

    // ---- Sport-specific part (hockey) ----------------------------------

    const char *const s_pLayoutName = "HO_pose_scoreboad.brlyt";

    // Animations, by index into s_pAnimName.
    enum {
        ANIM_INOUT_00,
        ANIM_INOUT_01,
        ANIM_INOUT_02,
        ANIM_INOUT_03,
        ANIM_INOUT_04,
        ANIM_INOUT_05,
        ANIM_SCORE_OFF,
        ANIM_SCORE_ON,
        ANIM_WAIT,
        ANIM_NUM
    };

    const char *s_pAnimName[ANIM_NUM + 1] = {
        "HO_pose_scoreboad_inout_00.brlan",
        "HO_pose_scoreboad_inout_01.brlan",
        "HO_pose_scoreboad_inout_02.brlan",
        "HO_pose_scoreboad_inout_03.brlan",
        "HO_pose_scoreboad_inout_04.brlan",
        "HO_pose_scoreboad_inout_05.brlan",
        "HO_pose_scoreboad_scoer_off.brlan",
        "HO_pose_scoreboad_scoer_on.brlan",
        "HO_pose_scoreboad_wait.brlan",
        0,
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_SET,
        PANE_SCORE_BLUE,
        PANE_SCORE_RED,
        PANE_SCORE_00, // tn_scoer_00 to _09: the period scores
        PANE_NEXT_00 = PANE_SCORE_00 + 10, // next_00 to _09
        PANE_COUNT_00 = PANE_NEXT_00 + 10, // the period labels
        PANE_BLUE_POINT = PANE_COUNT_00 + 5,
        PANE_RED_POINT,
        PANE_NUM
    };

    const char *s_pPaneName[PANE_NUM] = {
        "Set",
        "scoer_Blue",
        "scoer_Red",
        "tn_scoer_00",
        "tn_scoer_01",
        "tn_scoer_02",
        "tn_scoer_03",
        "tn_scoer_04",
        "tn_scoer_05",
        "tn_scoer_06",
        "tn_scoer_07",
        "tn_scoer_08",
        "tn_scoer_09",
        "next_00",
        "next_01",
        "next_02",
        "next_03",
        "next_04",
        "next_05",
        "next_06",
        "next_07",
        "next_08",
        "next_09",
        "t_st_00",
        "t_nd_00",
        "t_rd_00",
        "t_th_00",
        "t_th_01",
        "t_blue_point",
        "t_red_point",
    };

    // Periods shown per team.
    const long PERIOD_NUM = 5;
    const long TEAM_NUM = 2;

    // Every pane that SetAnimResultWin animates: the score and the "next"
    // panes.
    const unsigned char s_WinPaneId[20] = {
        PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4,
        PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9,
        PANE_NEXT_00 + 0, PANE_NEXT_00 + 1, PANE_NEXT_00 + 2, PANE_NEXT_00 + 3, PANE_NEXT_00 + 4,
        PANE_NEXT_00 + 5, PANE_NEXT_00 + 6, PANE_NEXT_00 + 7, PANE_NEXT_00 + 8, PANE_NEXT_00 + 9,
    };

    // The score panes of each team's periods.
    unsigned char s_ScorePaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4 },
        { PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9 },
    };

    // The "next" panes of each team's periods.
    unsigned char s_NextPaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_NEXT_00 + 0, PANE_NEXT_00 + 1, PANE_NEXT_00 + 2, PANE_NEXT_00 + 3, PANE_NEXT_00 + 4 },
        { PANE_NEXT_00 + 5, PANE_NEXT_00 + 6, PANE_NEXT_00 + 7, PANE_NEXT_00 + 8, PANE_NEXT_00 + 9 },
    };

    // The period labels.
    unsigned char s_CountPaneId[PERIOD_NUM] = {
        PANE_COUNT_00 + 0, PANE_COUNT_00 + 1, PANE_COUNT_00 + 2, PANE_COUNT_00 + 3, PANE_COUNT_00 + 4,
    };

    // The message file of the point labels.
    const char *s_pMessageFileName[] = {
        "HO_pose_scoreboad",
        0,
    };

    // Each team's point label, in the two forms GetTotalScoreUnitIndex
    // chooses between.
    const char *s_pPointLabel[TEAM_NUM][2] = {
        { "t_blue_point", "t_blue_point_01" },
        { "t_red_point", "t_red_point_01" },
    };

    // The resource file of the cockpit that holds the layout, its layer and
    // display list memory area.
    const long RES_FILE = 2;
    const long LAYER = 6;
    const long GDL_MEM_AREA = 1;

    // ---- Common part ---------------------------------------------------

    // m_nState.
    enum {
        STATE_IN = 1,
        STATE_IN_WAIT = 2,
        STATE_SHOW = 3,
        STATE_OUT = 4,
        STATE_OUT_WAIT = 5
    };

    // Animations of the whole page and of the score panes.
    enum {
        ANIM_PAGE_IN = 0,
        ANIM_PAGE_OUT = 2,
        ANIM_PAGE_WAIT = 3,
        ANIM_WIN_IN = 4,
        ANIM_WIN_OUT = 5
    };
}

msw::CCockpitHockeyPauseResult::CCockpitHockeyPauseResult(long nId, float fSortZ)
    : CCockpitPauseLayoutBase(nId, fSortZ)
{
    m_nStep = 1;
    m_nState = 0;
}

// (guess) The original destructor stores both vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitHockeyPauseResult::~CCockpitHockeyPauseResult()
{
    Nothing();
}

int msw::CCockpitHockeyPauseResult::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();

    if (pCockpit == 0) {
        return -1;
    }

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -2;
    }

    const char *pArchiveName = pCockpit->GetArchiveFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -3;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, LAYER, GDL_MEM_AREA);

    if (pPaneCtrl == 0) {
        return -5;
    }

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->BuildLayout(0, s_pLayoutName);
    pPaneCtrl->BuildAnim(0, s_pAnimName, ANIM_NUM);
    pPaneCtrl->SetSortZ(m_fSortZ);
    SetPaneAccessor(pPaneCtrl, s_pPaneName, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);

    // static-order swap (notes/decisions.md): the game numbers `recursive`
    // before `reset` but stores it after; this compiler stores local
    // statics in declaration order, so this order gives the original bytes
    // with the two symbol names exchanged.
    static const int reset = 1;
    static const int recursive = 0;

    m_pPaneAccessor->PlayAnimation(PANE_SET, ANIM_PAGE_WAIT, 1.0f, 0.0f, recursive, reset);
    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitHockeyPauseResult::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitHockeyPauseResult::Main()
{
    if (m_nStep == 1) {
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitHockeyPauseResult::StateControl()
{
    switch (m_nState) {
        case STATE_IN: {
            // static-order swap, as in Set.
            static const int reset = 1;
            static const int recursive = 0;

            m_pPaneAccessor->PlayAnimation(PANE_SET, ANIM_PAGE_IN, 1.0f, 0.0f, recursive, reset);
            SetAnimResultWin(ANIM_WIN_IN);
            m_nState = STATE_IN_WAIT;
            break;
        }

        case STATE_IN_WAIT: {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_SET);

            if (pObj->IsAnimEnd(ANIM_PAGE_IN)) {
                m_nState = STATE_SHOW;
            }
            break;
        }

        case STATE_OUT: {
            // static-order swap, as in Set.
            static const int reset = 1;
            static const int recursive = 0;

            m_pPaneAccessor->PlayAnimation(PANE_SET, ANIM_PAGE_OUT, 1.0f, 0.0f, recursive, reset);
            SetAnimResultWin(ANIM_WIN_OUT);
            m_nState = STATE_OUT_WAIT;
            break;
        }

        case STATE_OUT_WAIT: {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_SET);

            if (pObj->IsAnimEnd(ANIM_PAGE_OUT)) {
                // (meaning guessed) tells the owner the page is closed.
                m_Unk08 = 2;
            }
            break;
        }
    }
}

long msw::CCockpitHockeyPauseResult::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitHockeyPauseResult::SetAnimResultWin(long nAnim)
{
    // static-order swap: the game has `recursive`, `reset@0`, `len@1`,
    // stored in the reverse order.
    static const long len = 20;
    static const int reset = 0;
    static const int recursive = 0;

    if (nAnim == ANIM_WIN_IN) {
        for (long i = 0; i < len; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_WinPaneId[i]);

            if (pObj != 0) {
                pObj->StopAnimation(ANIM_WIN_OUT, 1);
                pObj->PushNextAnimation(nAnim, 1.0f, 0.0f, recursive, reset);
            }
        }
    } else if (nAnim == ANIM_WIN_OUT) {
        for (long i = 0; i < len; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_WinPaneId[i]);

            if (pObj != 0) {
                pObj->StopAnimation(ANIM_WIN_IN, 1);
                pObj->PushNextAnimation(nAnim, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

long msw::CCockpitHockeyPauseResult::SetCountScore(long *pScore0, long *pScore1, long nNum)
{
    long *pScore[TEAM_NUM] = { pScore0, pScore1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            // static-order swap: in the game the inner `animId` (7) is
            // numbered first and stored second.
            static const long animId = ANIM_SCORE_OFF;
            long nPane = s_ScorePaneId[nTeam][i];

            if (i <= nNum) {
                static const long animId = ANIM_SCORE_ON;

                m_pPaneAccessor->SetTextBoxString(nPane, pScore[nTeam][i], 0);

                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                if (pObj != 0) {
                    pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
                }
            } else {
                m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, 0, 1);
            }
        }
    }
    return 0;
}

long msw::CCockpitHockeyPauseResult::SetTextNext(long, long)
{
    static const long animId = ANIM_SCORE_OFF;

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            m_pPaneAccessor->PlayAnimation(s_NextPaneId[nTeam][i], animId, 1.0f, 0.0f, 0, 1);
        }
    }
    return 0;
}

long msw::CCockpitHockeyPauseResult::SetCountNum(long nPeriod)
{
    for (long i = 0; i < PERIOD_NUM; i++) {
        // Shows the labels of the periods played so far.
        m_pPaneAccessor->SetDisp(s_CountPaneId[i], i < nPeriod ? 1 : 0);
    }
    return 0;
}

long msw::CCockpitHockeyPauseResult::SetTotalPoint(long nTotal0, long nTotal1)
{
    menu::CMessage *pMessage = menu::CMessage::GetInstance();

    if (pMessage == 0) {
        return -1;
    }

    long total[TEAM_NUM] = { nTotal0, nTotal1 };
    unsigned char paneId[TEAM_NUM] = { PANE_BLUE_POINT, PANE_RED_POINT };
    static const long teamNum = TEAM_NUM;

    for (long nTeam = 0; nTeam < teamNum; nTeam++) {
        long nUnit = cockpit::GetTotalScoreUnitIndex(total[nTeam], 0, 1);
        const wchar_t *pText =
            pMessage->GetTextByLabel(pMessage->GetFileIndex(s_pMessageFileName[0]), s_pPointLabel[nTeam][nUnit]);

        if (pText != 0) {
            m_pPaneAccessor->SetTextBoxString(paneId[nTeam], pText, wcslen(pText));
        }
    }
    return 0;
}
