// msw::CCockpitDodgeCoder: the dodgeball game's score board (both teams'
// coin counters and ball power panels, special-shot gauges and prompts, the
// clock, the won-set marks, the period and the mission counter), and
// msw::CCockpitDodgeBallPower, the ball power numbers shown with it.
//
// Written from game/hockey/CCockpitHockeyCoder.cpp (the board),
// game/volley/CCockpitVolleyCoder.cpp (the won-set marks) and
// game/boss/CCockpitBossCoder.cpp (the ball power), against DodgeF.
#include "game/msw/CCockpitDodgeCoder.hpp"
#include "game/msw/CCockpitDodgeBallPower.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CSetCountUp.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 2;

    const char *const sLayoutName[] = {
        "DO_cockpit.brlyt"
    };

    const char *sAnimNames[] = {
        "DO_cockpit_Gauge_00.brlan",
        "DO_cockpit_Gauge_01.brlan",
        "DO_cockpit_Gauge_02.brlan",
        "DO_cockpit_Gauge_03.brlan",
        "DO_cockpit_Time_00.brlan",
        "DO_cockpit_Time_01.brlan",
        "DO_cockpit_SP_bar_00.brlan",
        "DO_cockpit_SP_bar_01.brlan",
        "DO_cockpit_SP_cont.brlan",
        "DO_cockpit_TC_00.brlan",
        "DO_cockpit_Time_vis_00.brlan",
        "DO_cockpit_Time_vis_01.brlan",
        "DO_cockpit_Time_vis_02.brlan",
        "DO_cockpit_C_ani_00.brlan",
        "DO_cockpit_C_ani_01.brlan",
        "DO_cockpit_C_ani_02.brlan",
        "DO_cockpit_C_ani_03.brlan",
        "DO_cockpit_Vis_00.brlan",
        "DO_cockpit_Vis_01.brlan",
        "DO_cockpit_Vis_set_ico_00.brlan",
        "DO_cockpit_Vis_set_ico_01.brlan",
        "DO_cockpit_Vis_set_ico_03.brlan",
        "DO_cockpit_SP_ca_00.brlan",
        "DO_cockpit_SP_ca_01.brlan",
        "DO_cockpit_SP_ca_02.brlan",
        "DO_cockpit_SP_ca_03.brlan",
        "DO_cockpit_SC_00.brlan",
        "DO_cockpit_SC_01.brlan",
        "DO_cockpit_SC_02.brlan",
        "DO_cockpit_SC_03.brlan",
        "DO_cockpit_SP_k_00.brlan",
        "DO_cockpit_SP_k_01.brlan"
    };

    // Animations (indexes into sAnimNames). The three parts of the board
    // (gauge, set count, coins) slide with the first four.
    enum {
        ANIM_SLIDE_IN = 0,
        ANIM_SLIDE_IN_WAIT = 1,
        ANIM_SLIDE_OUT = 2,
        ANIM_SLIDE_OUT_WAIT = 3,
        ANIM_TIME = 4,
        ANIM_TIME_ALL = 5,
        ANIM_SP_BAR = 6,
        ANIM_SP_BAR_MAX = 7,
        ANIM_SP_CONT = 8,
        ANIM_COUNTDOWN = 9,
        ANIM_TIME_NO = 10,
        ANIM_TIME_NO_HURRY = 11,
        ANIM_MISSION_ROLL = 13,
        ANIM_MISSION_ROLL_END = 14,
        ANIM_MISSION_WAIT = 15,
        ANIM_MISSION_STOP = 16,
        ANIM_MISSION_OUT = 17,
        ANIM_MISSION_IN = 18,
        ANIM_SET_ICO_OFF = 19,
        ANIM_SET_ICO_ON = 20,
        ANIM_SET_ICO_UP = 21,
        ANIM_SP_CA_0 = 22,
        ANIM_SP_CA_1 = 23,
        ANIM_SP_CA_2 = 24,
        ANIM_SP_CA_3 = 25,
        ANIM_COIN_DOWN_WAIT = 26,
        ANIM_COIN_UP = 27,
        ANIM_COIN_UP_WAIT = 28,
        ANIM_COIN_DOWN = 29,
        ANIM_SP_K = 30,
        ANIM_SP_K_MAX = 31
    };

    // The list ends with a null entry, which is not passed on.
    const char *sPaneNames[] = {
        "coin_L",
        "coin_R",
        "coinL_SP",
        "coinR_SP",
        "set_vis",
        "set_countLNull_B",
        "set_cntLNull_B01",
        "set_cntLNull_B02",
        "set_countRNull_B",
        "set_cntrNull_B01",
        "set_cntrNull_B02",
        "set_countLNull_A",
        "set_cntLNull_A01",
        "set_cntLNull_A02",
        "set_cntLNull_A03",
        "set_countRNull_A",
        "set_cntRNull_A01",
        "set_cntRNull_A02",
        "set_cntRNull_A03",
        "set_cnt_ico_k_00",
        "set_cnt_ico_00",
        "set_get_icoLA01",
        "set_cnt_ico_k_01",
        "set_cnt_ico_01",
        "set_get_icoLA02",
        "set_cnt_ico_k_05",
        "set_cnt_ico_05",
        "set_get_icoRA01",
        "set_cnt_ico_k_06",
        "set_cnt_ico_06",
        "set_get_icoRA02",
        "set_cnt_ico_k_07",
        "set_cnt_ico_07",
        "set_get_icoRB01",
        "set_cnt_ico_k_08",
        "set_cnt_ico_08",
        "set_get_icoRB02",
        "set_cnt_ico_k_09",
        "set_cnt_ico_09",
        "set_get_icoRB03",
        "set_cnt_ico_k_02",
        "set_cnt_ico_02",
        "set_get_icoLB01",
        "set_cnt_ico_k_03",
        "set_cnt_ico_03",
        "set_get_icoLB02",
        "set_cnt_ico_k_04",
        "set_cnt_ico_04",
        "set_get_icoLB03",
        "line_L",
        "line_R",
        "L_line_set",
        "R_line_set",
        "time",
        "text_time",
        "text_time02",
        "time_mge",
        "time_nom",
        "L_safe_ga1_A",
        "R_safe_ga1_A",
        "L_safe_ga2_A",
        "L_safe_ga2_B",
        "R_safe_ga2_A",
        "R_safe_ga2_B",
        "L_safe_ga3_A",
        "L_safe_ga3_B",
        "L_safe_ga3_C",
        "R_safe_ga3_A",
        "R_safe_ga3_B",
        "R_safe_ga3_C",
        "5_pass",
        "ball_04",
        "ball_03",
        "ball_02",
        "ball_01",
        "period_Null",
        "1st_pic",
        "2nd_pic",
        "3rd_pic",
        "4th_pic",
        "5th_pic",
        "SP_L",
        "SP_R",
        "SP_l",
        "SP_r",
        "SP_lf",
        "SP_rf",
        "SP_l_ka",
        "SP_r_ka",
        "f_l",
        "f_r",
        "R_null",
        "SP_cont_L",
        "btn_L_AB",
        "btn_L_12",
        "SP_cont_R",
        "btn_R_AB",
        "btn_R_12",
        "Time_co",
        "time_t",
        "C_SP_l",
        "C_SP_r",
        "time",
        "t_waku",
        "L_num_B_01",
        "L_num_B_02",
        "L_num_A",
        "tournament_Vis",
        "L_effect",
        "SP_CATCH_L",
        "SP_CATCH_R",
        "L_pow_Null",
        "R_pow_Null",
        "L_set_off",
        "R_set_off",
        "L_power_Null",
        "R_power_Null",
        "L_po_te",
        "R_po_te",
        "L_p_up_g",
        "R_p_up_g",
        "p_coin_L",
        "p_coin_R",
        "sc_p02",
        "sc_p04",
        0
    };

    // Number of panes in sPaneNames.
    const long PANE_NUM = 125;

    // Panes (indexes into sPaneNames) used by name in the code.
    enum {
        PANE_set_vis = 4,
        PANE_time = 53,
        PANE_text_time = 54,
        PANE_text_time02 = 55,
        PANE_time_mge = 56,
        PANE_time_nom = 57,
        PANE_5_pass = 70,
        PANE_period_Null = 75,
        PANE_Time_co = 98,
        PANE_time_t = 99,
        PANE_time_2 = 102, // a second "time": the clock with its hurry animation
        PANE_t_waku = 103
    };

    // Effects.
    enum {
        EFFECT_SP_MAX = 0x459,
        EFFECT_COUNTDOWN = 0x4D0
    };

    // Teams on the board.
    const long TEAM_NUM = 2;
    const long PERIOD_NUM = 5;

    // The tables below are indexed by team, unless said otherwise.

    // The won-set marks of each team, for a match of three and of five
    // sets.
    const unsigned char sSetCountPaneB[TEAM_NUM][5] = {
        { 7, 6 },
        { 10, 9 }
    };
    const unsigned char sSetCountPaneD[TEAM_NUM][5] = {
        { 14, 13, 12 },
        { 18, 17, 16 }
    };
    // The sets to win, by sets in the match minus 2.
    const unsigned char sSetWinNum[4] = { 2, 2, 3, 3 };
    // Not read by anything left in the file.
    const unsigned char sUnused0[4] = { 0 };
    // The set counter groups of each team, by sets in the match minus 2:
    // only the one of the match is shown.
    const unsigned char sSetCountNullPane[TEAM_NUM][4] = {
        { 5, 5, 11, 11 },
        { 8, 8, 15, 15 }
    };
    const unsigned char sCoinPane[TEAM_NUM] = { 0, 1 };
    const unsigned char sCoinSPPane[TEAM_NUM] = { 100, 101 };
    // Not read by anything left in the file.
    const unsigned char sCoinSPPane2[TEAM_NUM] = { 2, 3 };
    // The life gauges (they slide as the gauge part).
    const unsigned char sGaugePane[TEAM_NUM] = { 49, 50 };
    // The set count lines (they slide as the set count part).
    const unsigned char sLineSetPane[TEAM_NUM] = { 51, 52 };
    // The picture of each period ("1st" to "5th").
    const unsigned char sPeriodPane[PERIOD_NUM] = { 76, 77, 78, 79, 80 };
    // The special-shot gauge.
    const unsigned char sSPPane[TEAM_NUM] = { 81, 82 };
    const unsigned char sSPGaugePane[TEAM_NUM] = { 83, 84 };
    const unsigned char sSPGaugeUnkPane[TEAM_NUM] = { 85, 86 };
    const unsigned char sSPGaugeSubPane[TEAM_NUM] = { 87, 88 };
    // Where the "gauge full" effect plays.
    const unsigned char sSPFlashPane[TEAM_NUM] = { 89, 90 };
    // The special-shot prompt, and its buttons with a nunchaku and with a
    // remote alone.
    const unsigned char sSPContPane[TEAM_NUM] = { 92, 95 };
    const unsigned char sSPButtonPane[TEAM_NUM][2] = {
        { 93, 94 },
        { 96, 97 }
    };
    const unsigned char sSPTelopPane[TEAM_NUM] = { 109, 110 };
    // The mission counter (only the first team's is used).
    const unsigned char sMissionPane[TEAM_NUM] = { 107, 107 };
    const unsigned char sMissionEffectPane[TEAM_NUM] = { 108, 108 };
    const unsigned char sMissionNumPane[TEAM_NUM][3] = {
        { 104, 105, 106 },
        { 104, 105, 106 }
    };
    // The effect of a won-set mark, by team.
    const long sSetEffect[TEAM_NUM] = { 0x45E, 0x45F };

    // The ball power panel of each team (indexes into sPaneNames).
    struct PowerPane {
        unsigned char nNull;
        unsigned char nSetOff;
        unsigned char nSetOff2;
        unsigned char nPower; // slides with the coins
        unsigned char nText;
        unsigned char nUp;
    };

    const PowerPane sPowerPane[TEAM_NUM] = {
        { 111, 113, 113, 115, 117, 119 },
        { 112, 114, 114, 116, 118, 120 }
    };
    const unsigned char sPCoinPane[TEAM_NUM] = { 121, 122 };
    // The coin counter of each team.
    const unsigned char sCoinCountPane[TEAM_NUM] = { 123, 124 };

    // Stand-in for an unknown user of the tables above that nothing reads,
    // which the original keeps in .rodata (likely stripped code).
    inline void UnusedReferences()
    {
        (void)sUnused0;
        (void)sCoinSPPane2;
    }

    // By sets in the match minus 2.
    const unsigned char (*sSetCountPane[4])[5] = {
        sSetCountPaneB,
        sSetCountPaneB,
        sSetCountPaneD,
        sSetCountPaneD
    };
}

msw::CCockpitDodgeCoder::CCockpitDodgeCoder(long nId)
    : CCockpitDodgeLayoutBase(nId)
{
    m_nStep = 0;
    m_nSetNum = 0;
    for (long i = 0; i < TEAM_NUM; i++) {
        m_nUnk64[i] = 0xFF;
        m_nSetPoint[i] = 0;
    }
}

msw::CCockpitDodgeCoder::~CCockpitDodgeCoder()
{
    Remove();
}

void msw::CCockpitDodgeCoder::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
    m_MissionCount.MainLoop(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();
        m_nStep = 2;
    }
}

void msw::CCockpitDodgeCoder::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitDodgeCoder::Set()
{
    static const long teamNum = TEAM_NUM;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    unsigned char nSetNum;

    switch (CGame::g_GameWork.m_Unk0B) {
        default:
            nSetNum = 1;
            break;
        case 2:
        case 3:
            nSetNum = 3;
            break;
        case 4:
        case 5:
            nSetNum = 5;
            break;
    }
    m_nSetNum = nSetNum;
    m_nChrNum = CGameDodge::GetVariableChrNum(0);
    CGameDodge::GetChrMaxLife(0);

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
            pPaneCtrl->BuildLayout(0, sLayoutName[0]);
            pPaneCtrl->BuildAnim(0, sAnimNames, sizeof(sAnimNames) / sizeof(sAnimNames[0]));
            SetPaneAccessor(pPaneCtrl, sPaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);

            const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            long i;

            // Only the set counter of the match is shown.
            if (m_nSetNum > 1) {
                m_pPaneAccessor->SetPaneDisp(PANE_set_vis, 1);

                unsigned char nIdx = m_nSetNum - 2;

                for (i = 0; i < TEAM_NUM; i++) {
                    m_pPaneAccessor->SetPaneDisp(sLineSetPane[i], 1);
                    for (long j = 0; j < 4; j++) {
                        m_pPaneAccessor->SetPaneDisp(sSetCountNullPane[i][j], 0);
                    }
                    m_pPaneAccessor->SetPaneDisp(sSetCountNullPane[i][nIdx], 1);
                    SetSetCountPoint(i, 0);
                }
            } else {
                m_pPaneAccessor->SetPaneDisp(PANE_set_vis, 0);
                for (i = 0; i < TEAM_NUM; i++) {
                    m_pPaneAccessor->SetPaneDisp(sLineSetPane[i], 0);
                }
            }

            // No period picture yet (auto-inlined here).
            m_pPaneAccessor->SetPaneDisp(PANE_period_Null, 1);
            SetPeriodValue(-1);

            m_EffectSet.Initialize();

            for (i = 0; i < teamNum; i++) {
                m_SPGauge[i].SetParam(pPaneCtrl, sSPGaugePane[i], sSPGaugeUnkPane[i], sSPGaugeSubPane[i], ANIM_SP_BAR,
                                      ANIM_SP_BAR_MAX, ANIM_SP_K, ANIM_SP_K_MAX);
                m_SPGauge[i].SetGaugeAnim(m_pPaneAccessor, 0.0f);
            }
            for (i = 0; i < teamNum; i++) {
                m_SPInputDisp[i].SetParam(i, sSPContPane[i], ANIM_SP_CONT, sSPButtonPane[i][0], sSPButtonPane[i][1]);
                m_SPInputDisp[i].SetDispAnim(m_pPaneAccessor, 0);
            }
            SetSPGaugeDisp(1);
            for (i = 0; i < teamNum; i++) {
                SetSPGaugeTelop(i, -1.0f);
            }

            m_MissionCount.SetParameter(sMissionPane[0], sMissionNumPane[0][0], sMissionNumPane[0][1],
                                        sMissionNumPane[0][2], sMissionEffectPane[0], ANIM_MISSION_STOP,
                                        ANIM_MISSION_ROLL, ANIM_MISSION_ROLL_END, ANIM_MISSION_WAIT, ANIM_MISSION_IN,
                                        ANIM_MISSION_OUT);
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitDodgeCoder::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitDodgeCoder::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
        m_EffectSet.SetDispFlag(bVisible);
    }
    return 0;
}

// Plays slide animation nAnim on both life gauges.
inline void msw::CCockpitDodgeCoder::PlayAnimGauge(long nAnim)
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sGaugePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

// Plays slide animation nAnim on the set counters, if the match has sets.
inline void msw::CCockpitDodgeCoder::PlayAnimSetCount(long nAnim)
{
    if (m_nSetNum > 1) {
        unsigned char nIdx = m_nSetNum - 2;

        for (long i = 0; i < TEAM_NUM; i++) {
            bcs::G2D::CPaneObj *pLine = m_pPaneAccessor->GetPaneObj(sLineSetPane[i]);

            if (pLine != 0) {
                pLine->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
            }

            bcs::G2D::CPaneObj *pNull = m_pPaneAccessor->GetPaneObj(sSetCountNullPane[i][nIdx]);

            if (pNull != 0) {
                pNull->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
            }
        }
    }
}

// Plays slide animation nAnim on both coin counters and power panels.
inline void msw::CCockpitDodgeCoder::PlayAnimCoin(long nAnim)
{
    long i;

    for (i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    for (i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[i].nPower);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

int msw::CCockpitDodgeCoder::SetupGame()
{
    long i;

    UnusedReferences();

    // Every part in place.
    PlayAnimGauge(ANIM_SLIDE_IN_WAIT);
    PlayAnimSetCount(ANIM_SLIDE_IN_WAIT);
    PlayAnimCoin(ANIM_SLIDE_IN_WAIT);

    for (i = 0; i < TEAM_NUM; i++) {
        CoinBackAnim(i);
    }
    // The special gauges not full.
    for (i = 0; i < TEAM_NUM; i++) {
        m_SPGauge[i].SetGaugeMaxAnim(m_pPaneAccessor, 0);
        m_EffectSet.RemoveEffect(i, 0);
    }
    if (m_nSetNum > 1) {
        SetSetCountDisp(1);
    }
    m_MissionCount.SetDisp(m_pPaneAccessor, 0);
    SetTimeValue(-1.0f, 0, 1);
    return 0;
}

inline void msw::CCockpitDodgeCoder::CoinBackAnim(long nIdx)
{
    static const int recursive = 1;
    static const int reset = 1;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[nIdx].nPower);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
    }
}

// The mission counter slides with the gauges.
int msw::CCockpitDodgeCoder::GaugeOut()
{
    PlayAnimGauge(ANIM_SLIDE_OUT);
    m_MissionCount.SetMove(m_pPaneAccessor, ANIM_SLIDE_OUT);
    return 0;
}

int msw::CCockpitDodgeCoder::GaugeIn()
{
    PlayAnimGauge(ANIM_SLIDE_IN);
    m_MissionCount.SetMove(m_pPaneAccessor, ANIM_SLIDE_IN);
    return 0;
}

int msw::CCockpitDodgeCoder::GaugeOutWait()
{
    PlayAnimGauge(ANIM_SLIDE_OUT_WAIT);
    m_MissionCount.SetMove(m_pPaneAccessor, ANIM_SLIDE_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoder::GaugeInWait()
{
    PlayAnimGauge(ANIM_SLIDE_IN_WAIT);
    m_MissionCount.SetMove(m_pPaneAccessor, ANIM_SLIDE_IN_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoder::SetCountOut()
{
    PlayAnimSetCount(ANIM_SLIDE_OUT);
    return 0;
}

// The won-set marks light up again as the counter comes in.
int msw::CCockpitDodgeCoder::SetCountIn()
{
    PlayAnimSetCount(ANIM_SLIDE_IN);
    SetCountEffect(1);
    return 0;
}

int msw::CCockpitDodgeCoder::SetCountOutWait()
{
    PlayAnimSetCount(ANIM_SLIDE_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoder::SetCountInWait()
{
    PlayAnimSetCount(ANIM_SLIDE_IN_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoder::CoinOut()
{
    PlayAnimCoin(ANIM_SLIDE_OUT);
    return 0;
}

int msw::CCockpitDodgeCoder::CoinIn()
{
    PlayAnimCoin(ANIM_SLIDE_IN);
    return 0;
}

int msw::CCockpitDodgeCoder::CoinOutWait()
{
    PlayAnimCoin(ANIM_SLIDE_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoder::CoinInWait()
{
    PlayAnimCoin(ANIM_SLIDE_IN_WAIT);
    return 0;
}

void msw::CCockpitDodgeCoder::SetCountEffect(int)
{
    long nIdx = (unsigned char)(m_nSetNum - 2);

    if (nIdx < 0) {
        return;
    }

    long nWin = sSetWinNum[nIdx];

    // The marks of the sets won light one after the other.
    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        long nPoint = m_nSetPoint[(unsigned char)nTeam];

        for (long i = 0; i < nWin; i++) {
            long nPane = sSetCountPane[nIdx][nTeam][i];
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

            if (pObj != 0) {
                pObj->GetPane()->SetVisible(true);
                pObj->PlayAnimation(ANIM_SET_ICO_OFF, 1.0f, 0.0f, 1, 1, 1);
            }

            if (i < nPoint) {
                long nEffect = sSetEffect[nTeam];
                unsigned long nKind = nPane + 0x1000;
                menu::CDispTaskLink *pPrev = m_DispTaskList.FindNextKindObj(nKind, 0);

                if (pPrev != 0) {
                    pPrev->m_bEnd = 1;
                }

                cockpit::CSetCountUp *pTask = new cockpit::CSetCountUp;

                pTask->SetKind(nKind);
                pTask->SetPane(nPane);
                pTask->m_fDelay = 5.0f + 10.0f * i;
                pTask->m_nUnk34 = 51;
                pTask->m_nUnk36 = 0;
                pTask->m_nUnk38 = 1;
                pTask->m_nEffect = nEffect;
                pTask->m_nWaitAnim = ANIM_SET_ICO_UP;
                pTask->m_nAnim = ANIM_SET_ICO_ON;
                m_DispTaskList.PushBack(pTask);
            }
        }
    }
}

void msw::CCockpitDodgeCoder::SetCoinValue(long nIdx, long nCoin)
{
    if (nCoin > 0) {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nIdx]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_UP) && !pCount->IsAnimPlay(ANIM_COIN_UP_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[nIdx].nPower);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_UP, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_UP_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    } else {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nIdx]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_DOWN) && !pCount->IsAnimPlay(ANIM_COIN_DOWN_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerPane[nIdx].nPower);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

void msw::CCockpitDodgeCoder::SetPassBallValue(long)
{
}

void msw::CCockpitDodgeCoder::AddPassBallValue(long, long)
{
}

void msw::CCockpitDodgeCoder::SetCoinDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(sCoinSPPane[0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sCoinSPPane[1], bDisp);
    m_pPaneAccessor->SetPaneDisp(sPCoinPane[0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sPCoinPane[1], bDisp);
}

void msw::CCockpitDodgeCoder::SetCoinMaxState(long nIdx, long nState)
{
    if (nIdx >= TEAM_NUM) {
        return;
    }

    switch (nState) {
        case 5:
            m_SPGauge[nIdx].SetGaugeMaxAnim(m_pPaneAccessor, 0);
            m_EffectSet.RemoveEffect(nIdx, 0);
            break;
        case 6:
            m_SPGauge[nIdx].SetGaugeMaxAnim(m_pPaneAccessor, 1);
            m_EffectSet.SetEffect(nIdx, 0, EFFECT_SP_MAX, m_pPaneAccessor->GetPaneObj(sSPFlashPane[nIdx]));
            break;
    }
}

void msw::CCockpitDodgeCoder::SetCoinState(long, long)
{
}

void msw::CCockpitDodgeCoder::SetSetCountDisp(int bDisp)
{
    if (m_nSetNum <= 1) {
        return;
    }

    m_pPaneAccessor->SetPaneDisp(PANE_set_vis, bDisp);

    unsigned char nIdx = m_nSetNum - 2;
    long nWin = sSetWinNum[nIdx];

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        m_pPaneAccessor->SetPaneDisp(sSetCountNullPane[nTeam][nIdx], bDisp);
        for (long i = 0; i < nWin; i++) {
            m_pPaneAccessor->SetPaneDisp(sSetCountPane[nIdx][nTeam][i], bDisp);
        }
        m_pPaneAccessor->SetPaneDisp(sLineSetPane[nTeam], bDisp);
    }
}

void msw::CCockpitDodgeCoder::SetTimeDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_time, bDisp);
}

void msw::CCockpitDodgeCoder::SetLifeDisp(int bDisp)
{
    for (long i = 0; i < TEAM_NUM; i++) {
        m_pPaneAccessor->SetPaneDisp(sGaugePane[i], bDisp);
    }
}

void msw::CCockpitDodgeCoder::SetPassBallDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_5_pass, bDisp);
}

void msw::CCockpitDodgeCoder::SetTimeValue(float fTime, int nArg, int bArg)
{
    // No time limit: the mark instead of the clock.
    if (fTime < 0.0f) {
        m_pPaneAccessor->SetPaneDisp(PANE_time_mge, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_time_2, 1);
        m_pPaneAccessor->SetPaneDisp(PANE_time_nom, 0);
        return;
    }

    m_pPaneAccessor->SetPaneDisp(PANE_time_mge, 0);
    m_pPaneAccessor->SetPaneDisp(PANE_time_nom, 1);
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fTime, nArg, ANIM_TIME, ANIM_TIME_ALL, PANE_text_time,
                             PANE_text_time02, bArg);

    long nSec = m_TimeCount.GetSec(fTime);

    if (nSec == m_TimeCount.GetShowSec()) {
        return;
    }

    // The countdown of the last seconds.
    m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, ANIM_COUNTDOWN);
    m_EffectSet.RemoveEffect(0, 1);
    if (nSec >= 1 && nSec <= 9) {
        m_EffectSet.SetEffect(0, 1, EFFECT_COUNTDOWN, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_2);

    if (pObj != 0) {
        if (nSec <= 10 && !pObj->IsAnimPlay(ANIM_TIME_NO_HURRY)) {
            pObj->PlayAnimation(ANIM_TIME_NO_HURRY, 1.0f, 0.0f, 0, 1, 1);
        } else if (nSec > 10 && !pObj->IsAnimPlay(ANIM_TIME_NO)) {
            pObj->PlayAnimation(ANIM_TIME_NO, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    m_TimeCount.SetShowSec(nSec);
}

void msw::CCockpitDodgeCoder::SetSetCountPoint(long nIdx, long nCount)
{
    static const int recursive = 1;
    static const int reset = 1;

    if (nIdx >= TEAM_NUM) {
        return;
    }

    unsigned char nSetNum = m_nSetNum;

    if (nSetNum <= 1) {
        return;
    }

    if (nCount > nSetNum) {
        nCount = nSetNum;
    }
    m_nSetPoint[(unsigned char)nIdx] = nCount;

    long nTblIdx = nSetNum - 2;

    if (nTblIdx >= 0) {
        long nWin = sSetWinNum[nTblIdx];

        for (long i = 0; i < nWin; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetCountPane[nTblIdx][nIdx][i]);

            if (pObj != 0) {
                long nAnim = ANIM_SET_ICO_OFF;

                if (i < nCount) {
                    nAnim = ANIM_SET_ICO_ON;
                }
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            }
        }
    }
}

void msw::CCockpitDodgeCoder::SetPeriodValue(long nPeriod)
{
    for (long i = 0; i < PERIOD_NUM; i++) {
        int bDisp;

        if (i == nPeriod) {
            bDisp = 1;
        } else {
            bDisp = 0;
        }
        m_pPaneAccessor->SetPaneDisp(sPeriodPane[i], bDisp);
    }
}

void msw::CCockpitDodgeCoder::SetSPGauge(long nIdx, float fRatio)
{
    // The gauge moves in fifths.
    static const float f1_5 = 0.2f;

    if (nIdx < TEAM_NUM) {
        m_SPGauge[nIdx].SetGaugeAnim(m_pPaneAccessor, f1_5 * (int)(fRatio / f1_5));
    }
}

void msw::CCockpitDodgeCoder::SetSPControllerState(long nIdx, int nState)
{
    if (nIdx < TEAM_NUM) {
        m_SPInputDisp[nIdx].SetDispAnim(m_pPaneAccessor, nState);
    }
}

void msw::CCockpitDodgeCoder::SetSPGaugeDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(sSPPane[0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sSPPane[1], bDisp);
}

void msw::CCockpitDodgeCoder::SetSPGaugeTelop(long nIdx, float fRatio)
{
    cockpit::SetSPGaugeTelop(m_pPaneAccessor, sSPTelopPane[nIdx], ANIM_SP_CA_1, ANIM_SP_CA_2, ANIM_SP_CA_3,
                             ANIM_SP_CA_0, fRatio);
}

void msw::CCockpitDodgeCoder::SetMissionCountDisp(int bDisp)
{
    m_MissionCount.SetDisp(m_pPaneAccessor, bDisp);
}

void msw::CCockpitDodgeCoder::SetMissionCountNum(long nNum, long nMax)
{
    m_MissionCount.SetNum(m_pPaneAccessor, nNum, nMax);
}

void msw::CCockpitDodgeCoder::SetMissionCountUpNum(long nNum, long nMax)
{
    m_MissionCount.SetCountUpNum(m_pPaneAccessor, nNum, nMax);
}

void msw::CCockpitDodgeCoder::SetMissionCountActiveColor(int bActive)
{
    m_MissionCount.SetActiveColor(m_pPaneAccessor, bActive);
}

int msw::CCockpitDodgeCoder::SetBallPowerState(long, long)
{
    return 0;
}

int msw::CCockpitDodgeCoder::SetBallPowerShift(long, long)
{
    return 0;
}

int msw::CCockpitDodgeCoder::SetTelopPowerUp(long, long)
{
    return 0;
}

// msw::CCockpitDodgeBallPower

namespace {

    const char *sPowerLayoutNames[] = {
        "DO_cockpit_power_p.brlyt"
    };

    const char *sPowerAnimNames[] = {
        "DO_cockpit_power_p_Gauge_00.brlan",
        "DO_cockpit_power_p_Gauge_01.brlan",
        "DO_cockpit_power_p_Gauge_02.brlan",
        "DO_cockpit_power_p_Gauge_03.brlan",
        "DO_cockpit_power_p_B_p_00.brlan",
        "DO_cockpit_power_p_B_p_01.brlan",
        "DO_cockpit_power_p_B_p_02.brlan"
    };

    const char *sPowerPaneNames[] = {
        "L_pow_Null",
        "R_pow_Null",
        "L_set_off",
        "R_set_off",
        "L_power_Null",
        "R_power_Null",
        "L_po_te",
        "R_po_te"
    };

    // Animations (indexes into sPowerAnimNames).
    enum {
        ANIM_POWER_IN = 0,
        ANIM_POWER_IN_WAIT = 1,
        ANIM_POWER_OUT = 2,
        ANIM_POWER_OUT_WAIT = 3,
        ANIM_POWER_NORMAL = 4,
        ANIM_POWER_UP = 5,
        ANIM_POWER_UP2 = 6
    };

    // The resource layer of the layout object.
    const long LAYER_POWER = 6;

    // The panes of each team's numbers (indexes into sPowerPaneNames), as
    // PowerPane without the last.
    struct PowerNumPane {
        unsigned char nNull;
        unsigned char nSetOff;
        unsigned char nSetOff2;
        unsigned char nPower; // slides with the gauges
        unsigned char nText;
    };

    const PowerNumPane sPowerNumPane[TEAM_NUM] = {
        { 0, 2, 2, 4, 6 },
        { 1, 3, 3, 5, 7 }
    };
}

msw::CCockpitDodgeBallPower::CCockpitDodgeBallPower(long nId)
    : CCockpitDodgeLayoutBase(nId)
{
    m_nStep = 0;
}

msw::CCockpitDodgeBallPower::~CCockpitDodgeBallPower()
{
    Remove();
}

void msw::CCockpitDodgeBallPower::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        long i;

        for (i = 0; i < TEAM_NUM; i++) {
            m_pPaneAccessor->SetPaneDisp(sPowerNumPane[i].nText, 0);
        }
        for (i = 0; i < TEAM_NUM; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerNumPane[i].nPower);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_POWER_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
            }
        }
        m_nStep = 2;
    }
}

void msw::CCockpitDodgeBallPower::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitDodgeBallPower::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    unsigned char nSetNum;

    switch (CGame::g_GameWork.m_Unk0B) {
        default:
            nSetNum = 1;
            break;
        case 2:
        case 3:
            nSetNum = 3;
            break;
        case 4:
        case 5:
            nSetNum = 5;
            break;
    }
    m_nSetNum = nSetNum;

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, LAYER_POWER, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, sPowerLayoutNames[0]);
            pPaneCtrl->BuildAnim(0, sPowerAnimNames, sizeof(sPowerAnimNames) / sizeof(sPowerAnimNames[0]));
            SetPaneAccessor(pPaneCtrl, sPowerPaneNames, sizeof(sPowerPaneNames) / sizeof(sPowerPaneNames[0]));
            pPaneCtrl->SetSortZ(-900.0f);
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitDodgeBallPower::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

long msw::CCockpitDodgeBallPower::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitDodgeBallPower::GaugeOut()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerNumPane[i].nPower);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_POWER_OUT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitDodgeBallPower::GaugeIn()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerNumPane[i].nPower);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_POWER_IN, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitDodgeBallPower::GaugeOutWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerNumPane[i].nPower);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_POWER_OUT_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitDodgeBallPower::GaugeInWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPowerNumPane[i].nPower);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_POWER_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    return 0;
}

int msw::CCockpitDodgeBallPower::SetBallPowerValue(long nIdx, short nValue)
{
    const PowerNumPane *pPane = &sPowerNumPane[nIdx];

    m_pPaneAccessor->SetTextBoxString(pPane->nText, nValue, 0);
    return 0;
}

int msw::CCockpitDodgeBallPower::SetBallPowerValueColor(long nIdx, int nColor)
{
    const PowerNumPane *pPane = &sPowerNumPane[nIdx];

    ChangeTextFont(pPane->nText, nColor);
    return 0;
}

int msw::CCockpitDodgeBallPower::SetBallPowerState(long nIdx, long nState)
{
    // static-order swap: the original emits reset before recursive.
    static const int reset = 1;
    static const int recursive = 0;

    long nAnim;

    switch (nState) {
        case 1:
            nAnim = ANIM_POWER_UP;
            break;
        case 2:
            nAnim = ANIM_POWER_UP2;
            break;
        default:
            nAnim = ANIM_POWER_NORMAL;
            break;
    }
    const PowerNumPane *pPane = &sPowerNumPane[nIdx];

    m_pPaneAccessor->PlayAnimation(pPane->nText, nAnim, 1.0f, 0.0f, recursive, reset);
    return 0;
}

int msw::CCockpitDodgeBallPower::SetBallPowerShift(long, long)
{
    return 0;
}

void msw::CCockpitDodgeBallPower::ChangeTextFont(long nPane, int bColor)
{
    // Looked up and not used.
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return;
    }

    nw4r::lyt::ResourceAccessor *pResAccessor = pLytCtrl->GetLayoutResourceAccesor(0, sPowerLayoutNames[0]);

    if (pResAccessor == 0) {
        return;
    }

    void *pFont = pResAccessor->GetResource('font', bColor ? "FOT-RS_NO_01.brfnt" : "FOT-RS_NO_02.brfnt", 0);

    if (pFont == 0) {
        return;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(nPane);

    if (pPane == 0) {
        return;
    }

    bcs::G2D::CTextBoxDL *pTextBox = nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pPane);

    if (pTextBox != 0) {
        // Setting the font resets the size: keep it.
        nw4r::lyt::Size size = pTextBox->GetFontSize();

        pTextBox->SetFontByResource(pFont);
        pTextBox->SetFontSize(size);
    }
}

long msw::CCockpitDodgeBallPower::GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    if (nTeam >= TEAM_NUM) {
        return -1;
    }

    const PowerNumPane *pNumPane = &sPowerNumPane[nTeam];
    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(pNumPane->nText);

    if (pPane == 0) {
        return -2;
    }

    rSize = pPane->GetSize();

    nw4r::math::MTX34 mtx = pPane->GetGlobalMtx();

    rPos.x = mtx._03;
    rPos.y = mtx._13;
    rPos.z = mtx._23;
    return 0;
}
