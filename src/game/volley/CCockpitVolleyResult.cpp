// msw::CCockpitVolleyResult: the volley game's result page, with the score,
// a lamp and a perfect mark per set. Built like
// game/hockey/CCockpitHockeyResult.cpp, with the lamps of
// game/dodge/CCockpitDodgeResult.cpp (SetCountPoint).
#include "game/msw/CCockpitVolleyResult.hpp"

#include <wchar.h>
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/menu/CMessage.hpp"

namespace {

    const char *const s_pLayoutName = "result_VO.brlyt";

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
        "result_VO_inout_00.brlan",
        "result_VO_inout_01.brlan",
        "result_VO_inout_02.brlan",
        "result_VO_inout_03.brlan",
        "result_VO_inout_04.brlan",
        "result_VO_inout_05.brlan",
        "result_VO_scoer_off.brlan",
        "result_VO_scoer_on.brlan",
        "result_VO_wait.brlan",
        0,
    };

    // Panes, by index into s_pPaneName.
    enum {
        PANE_SET,
        PANE_SCORE_BLUE,
        PANE_SCORE_RED,
        PANE_SCORE_00, // tn_scoer_00 to _09: the set scores
        PANE_LAMP_00 = PANE_SCORE_00 + 10, // ranp_00 to _09: the set lamps
        PANE_LAMP_LIGHT_00 = PANE_LAMP_00 + 10, // ranp_light_00 to _09
        PANE_PERFECT_00 = PANE_LAMP_LIGHT_00 + 10, // parfect_00 to _09
        PANE_COUNT_00 = PANE_PERFECT_00 + 10, // the set labels
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
        "parfect_00",
        "parfect_01",
        "parfect_02",
        "parfect_03",
        "parfect_04",
        "parfect_05",
        "parfect_06",
        "parfect_07",
        "parfect_08",
        "parfect_09",
        "t_st_00",
        "t_nd_00",
        "t_rd_00",
        "t_th_00",
        "t_th_01",
        "t_blue_point",
        "t_red_point",
    };

    // Sets shown per team.
    const long PERIOD_NUM = 5;
    const long TEAM_NUM = 2;

    // The lamp panes of each team's sets.
    const unsigned char s_LampPaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_LAMP_00 + 0, PANE_LAMP_00 + 1, PANE_LAMP_00 + 2, PANE_LAMP_00 + 3, PANE_LAMP_00 + 4 },
        { PANE_LAMP_00 + 5, PANE_LAMP_00 + 6, PANE_LAMP_00 + 7, PANE_LAMP_00 + 8, PANE_LAMP_00 + 9 },
    };

    // The light of each lamp, lit for a set with points.
    const unsigned char s_LampLightPaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_LAMP_LIGHT_00 + 0, PANE_LAMP_LIGHT_00 + 1, PANE_LAMP_LIGHT_00 + 2, PANE_LAMP_LIGHT_00 + 3,
          PANE_LAMP_LIGHT_00 + 4 },
        { PANE_LAMP_LIGHT_00 + 5, PANE_LAMP_LIGHT_00 + 6, PANE_LAMP_LIGHT_00 + 7, PANE_LAMP_LIGHT_00 + 8,
          PANE_LAMP_LIGHT_00 + 9 },
    };

    // The perfect mark of each team's sets.
    const unsigned char s_PerfectPaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_PERFECT_00 + 0, PANE_PERFECT_00 + 1, PANE_PERFECT_00 + 2, PANE_PERFECT_00 + 3,
          PANE_PERFECT_00 + 4 },
        { PANE_PERFECT_00 + 5, PANE_PERFECT_00 + 6, PANE_PERFECT_00 + 7, PANE_PERFECT_00 + 8,
          PANE_PERFECT_00 + 9 },
    };

    // Every pane that SetAnimResultWin animates: the score panes, both sets
    // of lamps and the perfect marks.
    const unsigned char s_WinPaneId[40] = {
        PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4,
        PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9,
        PANE_LAMP_00 + 0, PANE_LAMP_00 + 1, PANE_LAMP_00 + 2, PANE_LAMP_00 + 3, PANE_LAMP_00 + 4,
        PANE_LAMP_00 + 5, PANE_LAMP_00 + 6, PANE_LAMP_00 + 7, PANE_LAMP_00 + 8, PANE_LAMP_00 + 9,
        PANE_LAMP_LIGHT_00 + 0, PANE_LAMP_LIGHT_00 + 1, PANE_LAMP_LIGHT_00 + 2, PANE_LAMP_LIGHT_00 + 3,
        PANE_LAMP_LIGHT_00 + 4, PANE_LAMP_LIGHT_00 + 5, PANE_LAMP_LIGHT_00 + 6, PANE_LAMP_LIGHT_00 + 7,
        PANE_LAMP_LIGHT_00 + 8, PANE_LAMP_LIGHT_00 + 9,
        PANE_PERFECT_00 + 0, PANE_PERFECT_00 + 1, PANE_PERFECT_00 + 2, PANE_PERFECT_00 + 3,
        PANE_PERFECT_00 + 4, PANE_PERFECT_00 + 5, PANE_PERFECT_00 + 6, PANE_PERFECT_00 + 7,
        PANE_PERFECT_00 + 8, PANE_PERFECT_00 + 9,
    };

    // The score panes of each team's sets.
    unsigned char s_ScorePaneId[TEAM_NUM][PERIOD_NUM] = {
        { PANE_SCORE_00 + 0, PANE_SCORE_00 + 1, PANE_SCORE_00 + 2, PANE_SCORE_00 + 3, PANE_SCORE_00 + 4 },
        { PANE_SCORE_00 + 5, PANE_SCORE_00 + 6, PANE_SCORE_00 + 7, PANE_SCORE_00 + 8, PANE_SCORE_00 + 9 },
    };

    // The set labels.
    unsigned char s_CountPaneId[PERIOD_NUM] = {
        PANE_COUNT_00 + 0, PANE_COUNT_00 + 1, PANE_COUNT_00 + 2, PANE_COUNT_00 + 3, PANE_COUNT_00 + 4,
    };

    // The message file of the point labels.
    const char *s_pMessageFileName[] = {
        "result_VO",
    };

    // Each team's point label, in the two forms GetTotalScoreUnitIndex
    // chooses between.
    const char *s_pPointLabel[TEAM_NUM][2] = {
        { "t_blue_point", "t_blue_point_01" },
        { "t_red_point", "t_red_point_01" },
    };

    // The text of a set not played for result 9 (a full-width dash); the
    // second entry is unused.
    const wchar_t *s_pNoScoreText[2] = {
        L"\xFF0D",
        0,
    };

    // The cockpit object id whose unplayed sets show a dash.
    const long OBJ_RESULT_DASH = 9;

    // The resource file of the cockpit that holds the layout, its layer and
    // display list memory area.
    const long RES_FILE = 2;
    const long LAYER = 0;
    const long GDL_MEM_AREA = 0;

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
        ANIM_WIN_IN = 4,
        ANIM_WIN_OUT = 5,
        ANIM_SCORE_COUNT = 8
    };

    // The speed of a score pane's count animation for a time of fTime
    // (none when the time is zero).
    inline float GetCountSpeed(float fTime)
    {
        if (__fabs(fTime) < 1.1920928955078125e-7) {
            return 0.0f;
        }
        return 1.0f / (fTime / 10.0f);
    }
}

msw::CCockpitVolleyResult::CCockpitVolleyResult(long nId, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ), CCockpitResultTime(nId != OBJ_RESULT_DASH)
{
    m_nStep = 0;
    m_nState = 0;
}

// (guess) The original destructor stores the vtable pointers, which the
// compiler only keeps when the body calls something; see
// game/boss/CCockpitBossLayoutBase.cpp and notes/decisions.md.
static inline void Nothing() {}

msw::CCockpitVolleyResult::~CCockpitVolleyResult()
{
    Nothing();
}

int msw::CCockpitVolleyResult::Set()
{
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
    m_nStep = 1;
    m_nState = STATE_IN;
    return 0;
}

void msw::CCockpitVolleyResult::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitVolleyResult::Main()
{
    StateControl();
}

void msw::CCockpitVolleyResult::StateControl()
{
    switch (m_nState) {
        case STATE_IN: {
            // static-order swap (notes/decisions.md): the game numbers
            // `recursive` before `reset` but stores it after.
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
            // static-order swap, as above.
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

long msw::CCockpitVolleyResult::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitVolleyResult::SetAnimResultWin(long nAnim)
{
    // static-order swap: the game has `recursive`, `reset@0`, `len@1`,
    // stored in the reverse order.
    static const long len = 40;
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

long msw::CCockpitVolleyResult::SetCountScore(long *pScore0, long *pScore1, long nPeriod, long nNum)
{
    long *pScore[TEAM_NUM] = { pScore0, pScore1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            // static-order swap: in the game the inner `animId` (7) is
            // numbered first and stored second.
            static const long animId = ANIM_SCORE_OFF;
            long nPane = s_ScorePaneId[nTeam][i];
            long nScore = pScore[nTeam][i];

            if (i < nNum) {
                m_pPaneAccessor->SetTextBoxString(nPane, nScore, 0);
            } else if (i < nPeriod) {
                if (m_nId == OBJ_RESULT_DASH) {
                    const wchar_t *pText = s_pNoScoreText[0];

                    m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
                } else {
                    const wchar_t *pText = L"";

                    m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
                }
            }

            if (i < nNum) {
                static const long animId = ANIM_SCORE_ON;
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                if (pObj != 0) {
                    float fDelay = m_fUnk04 * i;
                    float fSpeed = GetCountSpeed(m_fUnk00 + fDelay);

                    if (fSpeed > 0.0f) {
                        pObj->PlayAnimation(ANIM_SCORE_COUNT, fSpeed, 0.0f, 0, 1, 1);
                        pObj->PushNextAnimation(animId, 1.0f, 0.0f, 0, 1);
                    } else {
                        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
                    }
                }
            } else {
                m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, 0, 1);
            }
        }
    }
    return 0;
}

// The lamps: a set played shows its lamp, lit when the team scored points
// in it.
long msw::CCockpitVolleyResult::SetCountPoint(long *pPoint0, long *pPoint1, long nPeriod)
{
    long *pPoint[TEAM_NUM] = { pPoint0, pPoint1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            static const long animId = ANIM_SCORE_OFF;
            long nLamp = s_LampPaneId[nTeam][i];
            long nLight = s_LampLightPaneId[nTeam][i];

            if (i < nPeriod) {
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nLamp);

                if (pObj != 0) {
                    pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
                }

                if (pPoint[nTeam][i] != 0) {
                    static const long animId1 = ANIM_SCORE_ON;
                    bcs::G2D::CPaneObj *pLight = m_pPaneAccessor->GetPaneObj(nLight);

                    if (pLight != 0) {
                        float fDelay = m_fUnk04 * i;
                        float fSpeed = GetCountSpeed(m_fUnk00 + fDelay);

                        if (fSpeed > 0.0f) {
                            pLight->PlayAnimation(ANIM_SCORE_COUNT, fSpeed, 0.0f, 0, 1, 1);
                            pLight->PushNextAnimation(animId1, 1.0f, 0.0f, 0, 1);
                        } else {
                            pLight->PlayAnimation(animId1, 1.0f, 0.0f, 0, 1, 1);
                        }
                    }
                } else {
                    m_pPaneAccessor->PlayAnimation(nLight, animId, 1.0f, 0.0f, 0, 1);
                }
            } else {
                m_pPaneAccessor->PlayAnimation(nLamp, animId, 1.0f, 0.0f, 0, 1);
                m_pPaneAccessor->PlayAnimation(nLight, animId, 1.0f, 0.0f, 0, 1);
            }
        }
    }
    return 0;
}

long msw::CCockpitVolleyResult::SetTextNext(long, long nPeriod)
{
    for (long i = 0; i < PERIOD_NUM; i++) {
        // Shows the labels of the periods played so far.
        m_pPaneAccessor->SetPaneDisp(s_CountPaneId[i], i < nPeriod ? 1 : 0);
    }
    return 0;
}

// The perfect marks: a set won without a point lost shows its mark (the
// score pane is turned off); nNum is not read.
long msw::CCockpitVolleyResult::SetPerfectIcon(long *pPerfect0, long *pPerfect1, long)
{
    long *pPerfect[TEAM_NUM] = { pPerfect0, pPerfect1 };

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < PERIOD_NUM; i++) {
            long nPane = s_PerfectPaneId[nTeam][i];

            if (pPerfect[nTeam][i] != 0) {
                static const long animId = ANIM_SCORE_ON;
                bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                if (pObj != 0) {
                    float fDelay = m_fUnk04 * i;
                    float fSpeed = GetCountSpeed(m_fUnk00 + fDelay);

                    if (fSpeed > 0.0f) {
                        pObj->PlayAnimation(ANIM_SCORE_COUNT, fSpeed, 0.0f, 0, 1, 1);
                        pObj->PushNextAnimation(animId, 1.0f, 0.0f, 0, 1);
                    } else {
                        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
                    }
                }

                static const long offAnimId = ANIM_SCORE_OFF;

                m_pPaneAccessor->PlayAnimation(s_ScorePaneId[nTeam][i], offAnimId, 1.0f, 0.0f, 0, 1);
            } else {
                static const long animId = ANIM_SCORE_OFF;

                m_pPaneAccessor->PlayAnimation(nPane, animId, 1.0f, 0.0f, 0, 1);
            }
        }
    }
    return 0;
}

long msw::CCockpitVolleyResult::SetTotalPoint(long nTotal0, long nTotal1)
{
    menu::CMessage *pMessage = menu::CMessage::GetInstance();

    if (pMessage == 0) {
        return -1;
    }

    long total[TEAM_NUM] = { nTotal0, nTotal1 };
    unsigned char paneId[TEAM_NUM] = { PANE_BLUE_POINT, PANE_RED_POINT };
    static const long teamNum = TEAM_NUM;

    for (long nTeam = 0; nTeam < teamNum; nTeam++) {
        // A negative total counts as zero.
        long nTotal = total[nTeam] >= 0 ? total[nTeam] : 0;
        long nUnit = cockpit::GetTotalScoreUnitIndex(nTotal, 0, 1);
        const wchar_t *pText =
            pMessage->GetTextByLabel(pMessage->GetFileIndex(s_pMessageFileName[0]), s_pPointLabel[nTeam][nUnit]);

        if (pText != 0) {
            m_pPaneAccessor->SetTextBoxString(paneId[nTeam], pText, wcslen(pText));
        }
    }
    return 0;
}

long msw::CCockpitVolleyResult::CancelMenu()
{
    m_nState = STATE_OUT;
    return 0;
}
