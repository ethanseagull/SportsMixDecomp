// msw::CCockpitDodgePauseResult: the dodge games' page of the pause score
// board, with the score of each period and a lamp per period.
//
// Same code as game/basket/CCockpitBasketPauseResult.cpp except the block
// marked "Sport-specific part", SetCountPoint (the lamps, dodge and volley
// only) and SetTextNext (empty here: dodge has no "next" panes).
#include "game/msw/CCockpitDodgePauseResult.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/menu/CMessage.hpp"

namespace {

    // ---- Sport-specific part (dodge) -----------------------------------

    const char *const s_pLayoutName = "DO_pose_scoreboad.brlyt";

    // Animations, by index into s_pAnimName.
    enum {
        ANIM_INOUT_00,
        ANIM_INOUT_01,
        ANIM_INOUT_02,
        ANIM_INOUT_03,
        ANIM_INOUT_04,
        ANIM_INOUT_05,
        ANIM_LAMP_00,
        ANIM_LAMP_01,
        ANIM_SCORE_OFF,
        ANIM_SCORE_ON,
        ANIM_WAIT,
        ANIM_NUM
    };

    const char *s_pAnimName[ANIM_NUM + 1] = {
        "DO_pose_scoreboad_inout_00.brlan",
        "DO_pose_scoreboad_inout_01.brlan",
        "DO_pose_scoreboad_inout_02.brlan",
        "DO_pose_scoreboad_inout_03.brlan",
        "DO_pose_scoreboad_inout_04.brlan",
        "DO_pose_scoreboad_inout_05.brlan",
        "DO_pose_scoreboad_ranp_00.brlan",
        "DO_pose_scoreboad_ranp_01.brlan",
        "DO_pose_scoreboad_scoer_off.brlan",
        "DO_pose_scoreboad_scoer_on.brlan",
        "DO_pose_scoreboad_wait.brlan",
        0,
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_SET,
        PANE_SCORE_BLUE,
        PANE_SCORE_RED,
        PANE_SCORE_00, // tn_scoer_00 to _09: the period scores
        PANE_LAMP_00 = PANE_SCORE_00 + 10, // ranp_00 to _09: the period lamps
        PANE_LAMP_LIGHT_00 = PANE_LAMP_00 + 10, // ranp_light_00 to _09
        PANE_COUNT_00 = PANE_LAMP_LIGHT_00 + 10, // the period labels
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
        "ranp_00",
        "ranp_01",
        "ranp_02",
        "ranp_03",
        "ranp_04",
        "ranp_05",
        "ranp_06",
        "ranp_07",
        "ranp_08",
        "ranp_09",
        "ranp_light_00",
        "ranp_light_01",
        "ranp_light_02",
        "ranp_light_03",
        "ranp_light_04",
        "ranp_light_05",
        "ranp_light_06",
        "ranp_light_07",
        "ranp_light_08",
        "ranp_light_09",
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

    // The lamp panes of each team's periods (SetCountPoint).
    const unsigned char s_LampPaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_LAMP_00 + 0, PANE_LAMP_00 + 1, PANE_LAMP_00 + 2, PANE_LAMP_00 + 3, PANE_LAMP_00 + 4 },
        { PANE_LAMP_00 + 5, PANE_LAMP_00 + 6, PANE_LAMP_00 + 7, PANE_LAMP_00 + 8, PANE_LAMP_00 + 9 },
    };

    // Every pane that SetAnimResultWin animates: the score panes and both
    // sets of lamps.
    const unsigned char s_WinPaneId[30] = {
        PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4,
        PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9,
        PANE_LAMP_00 + 0, PANE_LAMP_00 + 1, PANE_LAMP_00 + 2, PANE_LAMP_00 + 3, PANE_LAMP_00 + 4,
        PANE_LAMP_00 + 5, PANE_LAMP_00 + 6, PANE_LAMP_00 + 7, PANE_LAMP_00 + 8, PANE_LAMP_00 + 9,
        PANE_LAMP_LIGHT_00 + 0, PANE_LAMP_LIGHT_00 + 1, PANE_LAMP_LIGHT_00 + 2, PANE_LAMP_LIGHT_00 + 3,
        PANE_LAMP_LIGHT_00 + 4, PANE_LAMP_LIGHT_00 + 5, PANE_LAMP_LIGHT_00 + 6, PANE_LAMP_LIGHT_00 + 7,
        PANE_LAMP_LIGHT_00 + 8, PANE_LAMP_LIGHT_00 + 9,
    };

    // The score panes of each team's periods.
    unsigned char s_ScorePaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4 },
        { PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9 },
    };

    // The period labels.
    unsigned char s_CountPaneId[PERIOD_NUM] = {
        PANE_COUNT_00 + 0, PANE_COUNT_00 + 1, PANE_COUNT_00 + 2, PANE_COUNT_00 + 3, PANE_COUNT_00 + 4,
    };

    // The message file of the point labels.
    const char *s_pMessageFileName[] = {
        "DO_pose_scoreboad",
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

msw::CCockpitDodgePauseResult::CCockpitDodgePauseResult(long nId, float fSortZ)
    : CCockpitPauseLayoutBase(nId, fSortZ)
{
    m_nStep = 1;
    m_nState = 0;
}

// (guess) The original destructor stores both vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitDodgePauseResult::~CCockpitDodgePauseResult()
{
    Nothing();
}

int msw::CCockpitDodgePauseResult::Set()
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

void msw::CCockpitDodgePauseResult::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitDodgePauseResult::Main()
{
    if (m_nStep == 1) {
        m_nStep = 2;
    }
    StateControl();
}

void msw::CCockpitDodgePauseResult::StateControl()
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

long msw::CCockpitDodgePauseResult::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitDodgePauseResult::SetAnimResultWin(long nAnim)
{
    // static-order swap: the game has `recursive`, `reset@0`, `len@1`,
    // stored in the reverse order.
    static const long len = sizeof(s_WinPaneId);
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

long msw::CCockpitDodgePauseResult::SetCountScore(long *pScore0, long *pScore1, long nNum)
{
    long *pScore[TEAM_NUM] = { pScore0, pScore1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            // static-order swap: in the game the inner `animId` (scoer_on) is
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

long msw::CCockpitDodgePauseResult::SetCountPoint(long *pPoint0, long *pPoint1, long, long nNum)
{
    long *pPoint[TEAM_NUM] = { pPoint0, pPoint1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            static const long animId = ANIM_SCORE_OFF;
            long nPane = s_LampPaneId[nTeam][i];

            if (i <= nNum) {
                // The lamp is lit for a period with points.
                long nAnim = ANIM_SCORE_OFF;

                if (pPoint[nTeam][i] != 0) {
                    nAnim = ANIM_SCORE_ON;
                }

                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                if (pObj != 0) {
                    pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);

                    if (nAnim == ANIM_SCORE_ON) {
                        pObj->PlayAnimation(ANIM_LAMP_01, 1.0f, 0.0f, 0, 0, 1);
                    }
                }
            } else {
                m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, 0, 1);
            }
        }
    }
    return 0;
}

// Dodge has no "next" panes.
long msw::CCockpitDodgePauseResult::SetTextNext(long, long)
{
    return 0;
}

long msw::CCockpitDodgePauseResult::SetCountNum(long nPeriod)
{
    for (long i = 0; i < PERIOD_NUM; i++) {
        // Shows the labels of the periods played so far.
        unsigned char bDisp = i < nPeriod ? 1 : 0;
        m_pPaneAccessor->SetPaneDisp(s_CountPaneId[i], bDisp);
    }
    return 0;
}

long msw::CCockpitDodgePauseResult::SetTotalPoint(long nTotal0, long nTotal1)
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
