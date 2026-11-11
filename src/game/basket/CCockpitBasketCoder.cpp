// Written from src/game/hockey/CCockpitHockeyCoder.cpp (Hockey -> Basket) and checked against BasketF: basket adds a fifth score animation, the score effect (PlayAnimScore) and the display events in SendEvent.
// msw::CCockpitBasketCoder: the basket game's score board (scores, coin
// counters, special-shot gauges, clock, period, face icons and the mission
// counter).
//
// Written after game/boss/CCockpitBossCoder.cpp; CCockpitBasketCoder is the
// same code with basket's tables.
#include "game/msw/CCockpitBasketCoder.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

namespace {

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 2;

    const char *const sLayoutName[] = {
        "BA_cockpit.brlyt"
    };

    const char *sAnimNames[] = {
        "BA_cockpit_Score_00.brlan",
        "BA_cockpit_Score_01.brlan",
        "BA_cockpit_Score_02.brlan",
        "BA_cockpit_Score_03.brlan",
        "BA_cockpit_Score_nom_01.brlan",
        "BA_cockpit_Time_00.brlan",
        "BA_cockpit_Time_01.brlan",
        "BA_cockpit_SP_bar_00.brlan",
        "BA_cockpit_SP_bar_01.brlan",
        "BA_cockpit_SP_cont.brlan",
        "BA_cockpit_TC_00.brlan",
        "BA_cockpit_SPCO_00.brlan",
        "BA_cockpit_SPCO_01.brlan",
        "BA_cockpit_Time_vis_00.brlan",
        "BA_cockpit_Time_vis_01.brlan",
        "BA_cockpit_Time_vis_02.brlan",
        "BA_cockpit_C_ani_00.brlan",
        "BA_cockpit_C_ani_01.brlan",
        "BA_cockpit_C_ani_02.brlan",
        "BA_cockpit_C_ani_03.brlan",
        "BA_cockpit_Vis_00.brlan",
        "BA_cockpit_Vis_01.brlan",
        "BA_cockpit_SP_sh_00.brlan",
        "BA_cockpit_SP_sh_01.brlan",
        "BA_cockpit_SP_sh_02.brlan",
        "BA_cockpit_SP_sh_03.brlan",
        "BA_cockpit_S_sp_00.brlan",
        "BA_cockpit_S_sp_01.brlan",
        "BA_cockpit_S_sp_02.brlan",
        "BA_cockpit_S_sp_03.brlan",
        "BA_cockpit_SC_00.brlan",
        "BA_cockpit_SC_01.brlan",
        "BA_cockpit_SC_02.brlan",
        "BA_cockpit_SC_03.brlan",
        "BA_cockpit_SP_k_00.brlan",
        "BA_cockpit_SP_k_01.brlan"
    };

    // Animations (indexes into sAnimNames). The four parts of the board
    // (score, coins, time, period) slide with the first four.
    enum {
        ANIM_SLIDE_IN_WAIT = 0,
        ANIM_SLIDE_OUT = 1,
        ANIM_SLIDE_OUT_WAIT = 2,
        ANIM_SLIDE_IN = 3,
        ANIM_SCORE_NOM = 4,
        ANIM_TIME = 5,
        ANIM_TIME_ALL = 6,
        ANIM_SP_BAR = 7,
        ANIM_SP_BAR_MAX = 8,
        ANIM_SP_CONT = 9,
        ANIM_COUNTDOWN = 10,
        ANIM_TIME_NO = 13,
        ANIM_TIME_NO_HURRY = 14,
        ANIM_MISSION_ROLL = 16,
        ANIM_MISSION_ROLL_END = 17,
        ANIM_MISSION_WAIT = 18,
        ANIM_MISSION_STOP = 19,
        ANIM_MISSION_OUT = 20,
        ANIM_MISSION_IN = 21,
        ANIM_TELOP_3 = 22,
        ANIM_TELOP_0 = 23,
        ANIM_TELOP_1 = 24,
        ANIM_TELOP_2 = 25,
        ANIM_LABEL_PLAYER_OFF = 26,
        ANIM_LABEL_PLAYER = 27,
        ANIM_LABEL_COM = 29,
        ANIM_COIN_DOWN_WAIT = 30,
        ANIM_COIN_UP = 31,
        ANIM_COIN_UP_WAIT = 32,
        ANIM_COIN_DOWN = 33,
        ANIM_SP_K = 34,
        ANIM_SP_K_MAX = 35
    };

    const char *sPaneNames[] = {
        "coin_L",
        "coin_R",
        "coinL_SP",
        "coinR_SP",
        "coin_L_ani",
        "coin_R_ani",
        "score_L",
        "score_R",
        "score_countL",
        "score_countL_2",
        "score_countL_3",
        "score_countR",
        "score_countR_2",
        "score_countR_3",
        "score_countL_k",
        "score_countL_2k",
        "score_countL_3k",
        "score_countR_k",
        "score_countR_2k",
        "score_countR_3k",
        "L_flame_pic",
        "R_flame_pic",
        "period_Null",
        "1st_pic",
        "2nd_pic",
        "3rd_pic",
        "4th_pic",
        "5th_pic",
        "time",
        "text_time",
        "text_time02",
        "direction_Null",
        "direction_pic01",
        "direction_pic_k",
        "scoL_01",
        "scoL_02",
        "scoR_01",
        "scoR_02",
        "SP_l",
        "SP_r",
        "SP_lf",
        "SP_rf",
        "SP_l_ka",
        "SP_r_ka",
        "f_l",
        "f_r",
        "SP_cont_L",
        "btn_L_AB",
        "btn_L_12",
        "SP_cont_R",
        "btn_R_AB",
        "btn_R_12",
        "Time_co",
        "time_t",
        "SP_L",
        "SP_R",
        "C_SP_l",
        "C_SP_r",
        "time_no",
        "t_waku",
        "no_e_L",
        "no_e_R",
        "L_num_B_01",
        "L_num_B_02",
        "L_num_A",
        "tournament_Vis",
        "L_effect",
        "SP_SHOOT_L",
        "SP_SHOOT_R",
        "L_Pa",
        "L_Pa_2",
        "R_Pa",
        "R_Pa_2",
        "L_Fa01",
        "L_Fa02",
        "L_Fa03",
        "L_Fa04",
        "L_Fa05",
        "L_Pa01",
        "L_Pa02",
        "L_Pa03",
        "L_Pa04",
        "L_Pa05",
        "R_Fa01",
        "R_Fa02",
        "R_Fa03",
        "R_Fa04",
        "R_Fa05",
        "R_Pa01",
        "R_Pa02",
        "R_Pa03",
        "R_Pa04",
        "R_Pa05",
        "1P",
        "2P",
        "3P",
        "4P",
        "com",
        "L_Ca_01",
        "L_Ca_02",
        "L_Ca_03",
        "L_Ca_04",
        "L_Ca_05",
        "R_Ca_01",
        "R_Ca_02",
        "R_Ca_03",
        "R_Ca_04",
        "R_Ca_05",
        "cs_ico",
        "sc_p02",
        "sc_p04",
        "L_FcPic1",
        "L_FcPic2",
        "L_FcPic3",
        "R_FcPic1",
        "R_FcPic2",
        "R_FcPic3",
        "L_PNoPic1",
        "L_PNoPic2",
        "L_PNoPic3",
        "R_PNoPic1",
        "R_PNoPic2",
        "R_PNoPic3",
        "L_StPic1",
        "L_StPic2",
        "L_StPic3",
        "R_StPic1",
        "R_StPic2",
        "R_StPic3"
    };

    // Panes (indexes into sPaneNames) used by name in the code.
    enum {
        PANE_period = 22,
        PANE_time = 28,
        PANE_text_time = 29,
        PANE_text_time02 = 30,
        PANE_Time_co = 52,
        PANE_time_t = 53,
        PANE_time_no = 58,
        PANE_t_waku = 59,
        PANE_cs_ico = 108
    };

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp);
    // pParam is nonzero to show, zero to hide.
    enum {
        EVENT_SET_DISP = 10001,
        EVENT_SET_SCORE_DISP = 10008,
        EVENT_SET_TIME_DISP = 10009,
        EVENT_SET_PERIOD_DISP = 10010,
        EVENT_SET_SP_FRAME_DISP = 10011,
        // pParam: 0 or 1, picks the animation of the special gauges.
        EVENT_SET_SP_GAUGE_DISP = 10012
    };

    // Effects.
    enum {
        EFFECT_SP_MAX = 0x459,
        EFFECT_SCORE = 0x45C,
        EFFECT_COUNTDOWN = 0x4D0
    };

    // Teams on the board, and the players of a team.
    const long TEAM_NUM = 2;
    const long PLAYER_NUM = 3;
    // Teams the board is set up for (SetupGame).
    const long TEAM_MAX = 4;
    const long PERIOD_NUM = 5;

    // The tables below are indexed by team, unless said otherwise. They are
    // in the order of the original's .rodata.
    const unsigned char sScorePane[TEAM_NUM] = { 6, 7 };
    const unsigned char sCoinPane[TEAM_NUM] = { 0, 1 };
    // The frames of the special gauges.
    const unsigned char sSPFramePane[TEAM_NUM] = { 56, 57 };
    const unsigned char sCoinSPPane[TEAM_NUM] = { 2, 3 };
    // The picture of each period ("1st" to "5th").
    const unsigned char sPeriodPane[PERIOD_NUM] = { 23, 24, 25, 26, 27 };
    // The special-shot prompt, and its buttons with a nunchaku and with a
    // remote alone.
    const unsigned char sSPContPane[TEAM_NUM] = { 46, 49 };
    // The animation of the special gauges by EVENT_SET_SP_GAUGE_DISP's
    // argument.
    const unsigned char sSPGaugeDispAnim[2] = { 12, 11 };
    const unsigned char sSPPane[TEAM_NUM] = { 54, 55 };
    const unsigned char sSPButtonPane[TEAM_NUM][2] = {
        { 47, 48 },
        { 50, 51 }
    };
    // The score's text boxes: below 100, below 1000, from 1000.
    const unsigned char sScoreCountPane[TEAM_NUM][3] = {
        { 8, 9, 10 },
        { 11, 12, 13 }
    };
    // What is shown with each of those text boxes.
    const unsigned char sScoreCountDispPane[TEAM_NUM][3] = {
        { 14, 15, 16 },
        { 17, 18, 19 }
    };
    // The special-shot gauge.
    const unsigned char sSPGaugePane[TEAM_NUM] = { 38, 39 };
    const unsigned char sSPGaugeUnkPane[TEAM_NUM] = { 40, 41 };
    const unsigned char sSPGaugeSubPane[TEAM_NUM] = { 42, 43 };
    // Where the "gauge full" effect plays.
    const unsigned char sSPFlashPane[TEAM_NUM] = { 44, 45 };
    const unsigned char sSPTelopPane[TEAM_NUM] = { 67, 68 };
    // The mission counter (only the first team's is used).
    const unsigned char sMissionPane[TEAM_NUM] = { 65, 65 };
    const unsigned char sMissionEffectPane[TEAM_NUM] = { 66, 66 };
    const unsigned char sMissionNumPane[TEAM_NUM][3] = {
        { 62, 63, 64 },
        { 62, 63, 64 }
    };
    // Where the score effect plays (PlayAnimScore).
    const unsigned char sScoreEffectPane[TEAM_NUM] = { 60, 61 };
    // Where the face icons go, by players per team (1 to 3), team colour
    // and player.
    const unsigned char sFaceParentPane[PLAYER_NUM][TEAM_NUM][PLAYER_NUM] = {
        {
            { 73, 0, 0 },
            { 83, 0, 0 }
        },
        {
            { 76, 77, 0 },
            { 86, 87, 0 }
        },
        {
            { 73, 74, 75 },
            { 83, 84, 85 }
        }
    };
    const unsigned char sFacePane[TEAM_NUM][PLAYER_NUM] = {
        { 111, 112, 113 },
        { 114, 115, 116 }
    };
    // Where a player's label goes, as sFaceParentPane.
    const unsigned char sLabelParentPane[PLAYER_NUM][TEAM_NUM][PLAYER_NUM] = {
        {
            { 78, 0, 0 },
            { 88, 0, 0 }
        },
        {
            { 81, 82, 0 },
            { 91, 92, 0 }
        },
        {
            { 78, 79, 80 },
            { 88, 89, 90 }
        }
    };
    // The labels: "1P" to "4P", then the computer's.
    const unsigned char sIconPicPane[5] = { 93, 94, 95, 96, 97 };
    const long ICON_PIC_COM = 4;
    const unsigned char sLabelPane[TEAM_NUM][PLAYER_NUM] = {
        { 117, 118, 119 },
        { 120, 121, 122 }
    };
    // The frames around the face icons, by team: for two players a team,
    // for one.
    const unsigned char sFramePane[TEAM_NUM][2] = {
        { 70, 69 },
        { 72, 71 }
    };
    // Where a star player's star goes, as sFaceParentPane.
    const unsigned char sStarParentPane[PLAYER_NUM][TEAM_NUM][PLAYER_NUM] = {
        {
            { 98, 0, 0 },
            { 103, 0, 0 }
        },
        {
            { 101, 102, 0 },
            { 106, 107, 0 }
        },
        {
            { 98, 99, 100 },
            { 103, 104, 105 }
        }
    };
    const unsigned char sStarPane[TEAM_NUM][PLAYER_NUM] = {
        { 123, 124, 125 },
        { 126, 127, 128 }
    };
    // The coin counter of each team.
    const unsigned char sCoinCountPane[TEAM_NUM] = { 109, 110 };
}

msw::CCockpitBasketCoder::CCockpitBasketCoder(long nId)
    : CCockpitBasketLayoutBase(nId)
{
    m_nStep = 0;
    m_nUnk60 = 0;
    for (long i = 0; i < 4; i++) {
        m_nUnk61[i] = 0xFF;
        m_nUnk65[i] = 0xFF;
    }
}

msw::CCockpitBasketCoder::~CCockpitBasketCoder()
{
    Remove();
}

void msw::CCockpitBasketCoder::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
    m_MissionCount.MainLoop(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();

        // Every part in place.
        PlayAnimScore(ANIM_SLIDE_IN_WAIT, 0);
        PlayAnimCoin(ANIM_SLIDE_IN_WAIT);

        bcs::G2D::CPaneObj *pTime = m_pPaneAccessor->GetPaneObj(PANE_time);

        if (pTime != 0) {
            pTime->PlayAnimation(ANIM_SLIDE_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }

        bcs::G2D::CPaneObj *pPeriod = m_pPaneAccessor->GetPaneObj(PANE_period);

        if (pPeriod != 0) {
            pPeriod->PlayAnimation(ANIM_SLIDE_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }

        SetFaceIcon(GetLytObj());
        m_nStep = 2;
    }
}

void msw::CCockpitBasketCoder::Disp()
{
}

int msw::CCockpitBasketCoder::Set()
{
    static const long teamNum = TEAM_NUM;
    static const long iconPicTblLength = sizeof(sIconPicPane) / sizeof(sIconPicPane[0]);

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
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
            SetPaneAccessor(pPaneCtrl, sPaneNames, sizeof(sPaneNames) / sizeof(sPaneNames[0]));
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);

            // The period pictures move with the period pane.
            const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
            bcs::G2D::CPaneObj *pPeriod = m_pPaneAccessor->GetPaneObj(PANE_period);
            long i;

            if (pPeriod != 0) {
                for (i = 0; i < PERIOD_NUM; i++) {
                    bcs::G2D::CPaneObj *pPic = m_pPaneAccessor->GetPaneObj(sPeriodPane[i]);

                    if (pPic != 0) {
                        bcs::G2D::SetPaneParent(pPic->GetPane(), pPeriod->GetPane(), 1, 0);
                        pPic->GetPane()->SetTranslate(zero);
                    }
                }
            }

            m_nUnk69 = 4;
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

            // The label sources stay hidden; SetFaceIcon copies them.
            for (i = 0; i < iconPicTblLength; i++) {
                m_pPaneAccessor->SetPaneDisp(sIconPicPane[i], 0);
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitBasketCoder::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitBasketCoder::SendEvent(long nEvent, void *pParam)
{
    int bDisp = reinterpret_cast<int>(pParam);

    switch (nEvent) {
        case EVENT_SET_DISP:
            if (GetLytObj() != 0) {
                GetLytObj()->SetVisible(bDisp);
            }
            m_EffectSet.SetDispFlag(bDisp);
            break;
        case EVENT_SET_SCORE_DISP:
            m_pPaneAccessor->SetPaneDisp(sScorePane[0], bDisp);
            m_pPaneAccessor->SetPaneDisp(sScorePane[1], bDisp);
            break;
        case EVENT_SET_TIME_DISP:
            m_pPaneAccessor->SetPaneDisp(PANE_time, bDisp);
            break;
        case EVENT_SET_PERIOD_DISP:
            m_pPaneAccessor->SetPaneDisp(PANE_period, bDisp);
            break;
        case EVENT_SET_SP_FRAME_DISP:
            m_pPaneAccessor->SetPaneDisp(sSPFramePane[0], bDisp);
            m_pPaneAccessor->SetPaneDisp(sSPFramePane[1], bDisp);
            break;
        case EVENT_SET_SP_GAUGE_DISP:
            SetSPGaugeDisp(bDisp);
            break;
    }
    return 0;
}

int msw::CCockpitBasketCoder::SetupGame()
{
    long i;

    for (i = 0; i < TEAM_MAX; i++) {
        SetCoinMaxState(i, 5);
    }
    for (i = 0; i < TEAM_NUM; i++) {
        PlayAnimCoinBack(i);
    }
    m_MissionCount.SetDisp(m_pPaneAccessor, 0);
    PlayAnimScore(ANIM_SCORE_NOM, 0);
    for (i = 0; i < TEAM_MAX; i++) {
        SetScoreFont(i, 0);
    }
    SetPeriodValue(0);
    return 0;
}

int msw::CCockpitBasketCoder::ScoreOut()
{
    static const long animId = ANIM_SLIDE_OUT;

    PlayAnimScore(animId, 0);
    return 0;
}

int msw::CCockpitBasketCoder::ScoreIn()
{
    static const long animId = ANIM_SLIDE_IN;

    // Sliding in, the scores also play their effect.
    PlayAnimScore(animId, 1);
    return 0;
}

int msw::CCockpitBasketCoder::ScoreOutWait()
{
    static const long animId = ANIM_SLIDE_OUT_WAIT;

    PlayAnimScore(animId, 0);
    return 0;
}

int msw::CCockpitBasketCoder::ScoreInWait()
{
    static const long animId = ANIM_SLIDE_IN_WAIT;

    PlayAnimScore(animId, 0);
    return 0;
}

// The mission counter slides with the coins.
int msw::CCockpitBasketCoder::CoinOut()
{
    static const long animId = ANIM_SLIDE_OUT;

    PlayAnimCoin(animId);
    m_MissionCount.SetMove(m_pPaneAccessor, animId);
    return 0;
}

int msw::CCockpitBasketCoder::CoinIn()
{
    static const long animId = ANIM_SLIDE_IN;

    PlayAnimCoin(animId);
    m_MissionCount.SetMove(m_pPaneAccessor, animId);
    return 0;
}

int msw::CCockpitBasketCoder::CoinOutWait()
{
    static const long animId = ANIM_SLIDE_OUT_WAIT;

    PlayAnimCoin(animId);
    m_MissionCount.SetMove(m_pPaneAccessor, animId);
    return 0;
}

int msw::CCockpitBasketCoder::CoinInWait()
{
    static const long animId = ANIM_SLIDE_IN_WAIT;

    PlayAnimCoin(animId);
    m_MissionCount.SetMove(m_pPaneAccessor, animId);
    return 0;
}

int msw::CCockpitBasketCoder::TimeOut()
{
    static const long animId = ANIM_SLIDE_OUT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::TimeIn()
{
    static const long animId = ANIM_SLIDE_IN;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::TimeOutWait()
{
    static const long animId = ANIM_SLIDE_OUT_WAIT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::TimeInWait()
{
    static const long animId = ANIM_SLIDE_IN_WAIT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::PeriodOut()
{
    static const long animId = ANIM_SLIDE_OUT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_period);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::PeriodIn()
{
    static const long animId = ANIM_SLIDE_IN;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_period);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::PeriodOutWait()
{
    static const long animId = ANIM_SLIDE_OUT_WAIT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_period);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::PeriodInWait()
{
    static const long animId = ANIM_SLIDE_IN_WAIT;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_period);

    if (pObj != 0) {
        pObj->PlayAnimation(animId, 1.0f, 0.0f, 0, 1, 1);
    }
    return 0;
}

int msw::CCockpitBasketCoder::PlayAnimScore(long nAnim, int bEffect)
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);

            if (bEffect) {
                // The score effect, started after a delay.
                m_EffectSet.RemoveEffect(i, 2);
                m_EffectSet.SetEffect(i, 2, EFFECT_SCORE, m_pPaneAccessor->GetPaneObj(sScoreEffectPane[i]));

                unsigned long hEffect = m_EffectSet.GetEffectHandle(i, 2);

                if (hEffect != 0) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);

                    if (pCluster != 0) {
                        pCluster->CallEffectFunc(&bcs::EF::CEffect::SetDelayTime, 30.0f);
                    }
                }
            }
        }
    }
    return 0;
}

inline void msw::CCockpitBasketCoder::PlayAnimCoin(long nAnim)
{
    static const int recursive = 0;
    static const int reset = 1;

    for (long i = 0; i < TEAM_NUM; i++) {
        m_pPaneAccessor->PlayAnimation(sCoinPane[i], nAnim, 1.0f, 0.0f, recursive, reset);
    }
}

inline void msw::CCockpitBasketCoder::PlayAnimCoinBack(long nTeam)
{
    static const int recursive = 1;
    static const int reset = 1;

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nTeam]);

    if (pObj != 0) {
        pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
        pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
    }
}

void msw::CCockpitBasketCoder::SetScoreValue(long nTeam, long nScore)
{
    static const long tblLen = 3;

    if (nTeam >= TEAM_NUM) {
        return;
    }

    // Which text box shows the score: one for each number of digits.
    long nDigits;

    if (nScore >= 1000) {
        nDigits = 2;
    } else {
        nDigits = nScore >= 100;
    }

    for (long i = 0; i < tblLen; i++) {
        long nPane = sScoreCountPane[nTeam][i];
        long nDispPane = sScoreCountDispPane[nTeam][i];

        if (i == nDigits) {
            m_pPaneAccessor->SetTextBoxString(nPane, nScore > 9999 ? 9999 : nScore, 0);
            m_pPaneAccessor->SetPaneDisp(nDispPane, 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(nDispPane, 0);
        }
    }
}

void msw::CCockpitBasketCoder::SetCoinValue(long nTeam, long nDiff)
{
    if (nDiff > 0) {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nTeam]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_UP) && !pCount->IsAnimPlay(ANIM_COIN_UP_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nTeam]);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_UP, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_UP_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    } else {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nTeam]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_DOWN) && !pCount->IsAnimPlay(ANIM_COIN_DOWN_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nTeam]);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

void msw::CCockpitBasketCoder::SetTimeValue(float fFrame, int nArg, int nMode)
{
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fFrame, nArg, ANIM_TIME, ANIM_TIME_ALL, PANE_text_time,
                             PANE_text_time02, nMode);

    long nSec = m_TimeCount.GetSec(fFrame);

    if (nSec == m_TimeCount.GetShowSec()) {
        return;
    }

    // The countdown of the last seconds.
    m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, ANIM_COUNTDOWN);
    m_EffectSet.RemoveEffect(0, 1);
    if (nSec >= 1 && nSec <= 9) {
        m_EffectSet.SetEffect(0, 1, EFFECT_COUNTDOWN, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_no);

    if (pObj != 0) {
        if (nSec <= 10 && !pObj->IsAnimPlay(ANIM_TIME_NO_HURRY)) {
            pObj->PlayAnimation(ANIM_TIME_NO_HURRY, 1.0f, 0.0f, 0, 1, 1);
        } else if (nSec > 10 && !pObj->IsAnimPlay(ANIM_TIME_NO)) {
            pObj->PlayAnimation(ANIM_TIME_NO, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    m_TimeCount.SetShowSec(nSec);
}

void msw::CCockpitBasketCoder::SetPeriodValue(long nPeriod)
{
    for (long i = 0; i < PERIOD_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sPeriodPane[i]);

        if (pObj != 0) {
            pObj->GetPane()->SetVisible(i == nPeriod);
        }
    }
}

void msw::CCockpitBasketCoder::SetCoinMaxState(long nTeam, long nState)
{
    if (nTeam >= TEAM_NUM) {
        return;
    }

    // Looked up and not used.
    m_pPaneAccessor->GetPaneObj(sCoinSPPane[nTeam]);

    switch (nState) {
        case 5:
            m_SPGauge[nTeam].SetGaugeMaxAnim(m_pPaneAccessor, 0);
            m_EffectSet.RemoveEffect(nTeam, 0);
            break;
        case 6:
            m_SPGauge[nTeam].SetGaugeMaxAnim(m_pPaneAccessor, 1);
            m_EffectSet.SetEffect(nTeam, 0, EFFECT_SP_MAX, m_pPaneAccessor->GetPaneObj(sSPFlashPane[nTeam]));
            break;
    }
}

void msw::CCockpitBasketCoder::SetArrowStateAnim(long)
{
}

void msw::CCockpitBasketCoder::SetScoreLineAnim(long)
{
}

void msw::CCockpitBasketCoder::SetScoreFont(long nTeam, int bColor)
{
    if (nTeam >= TEAM_NUM) {
        return;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    const char *pFontName = "FOT-RS_NO_02.brfnt";

    if (bColor) {
        pFontName = "FOT-RS_NO_01.brfnt";
    }

    for (long i = 0; i < 3; i++) {
        long nPane = sScoreCountPane[nTeam][i];
        long nDispPane = sScoreCountDispPane[nTeam][i];

        cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(nPane), sLayoutName[0], pFontName);
        cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(nDispPane), sLayoutName[0], pFontName);
    }
}

void msw::CCockpitBasketCoder::SetSPGauge(long nTeam, float fRate)
{
    // The gauge moves in fifths.
    static const float f1_5 = 0.2f;

    if (nTeam < TEAM_NUM) {
        m_SPGauge[nTeam].SetGaugeAnim(m_pPaneAccessor, f1_5 * (int)(fRate / f1_5));
    }
}

void msw::CCockpitBasketCoder::SetSPControllerState(long nTeam, int bDisp)
{
    if (nTeam < TEAM_NUM) {
        m_SPInputDisp[nTeam].SetDispAnim(m_pPaneAccessor, bDisp);
    }
}

// Plays the special gauges' animation for bDisp (0 or 1). Set and
// SendEvent inline it; nothing calls it, so the original link dead-stripped
// the function itself and kept only its two constants in .rodata.
void msw::CCockpitBasketCoder::SetSPGaugeDisp(int bDisp)
{
    static const int recursive = 0;
    static const int reset = 1;

    long nAnim = sSPGaugeDispAnim[bDisp];

    m_pPaneAccessor->PlayAnimation(sSPPane[0], nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(sSPPane[1], nAnim, 1.0f, 0.0f, recursive, reset);
}

void msw::CCockpitBasketCoder::SetSPGaugeTelop(long nTeam, float fRate)
{
    cockpit::SetSPGaugeTelop(m_pPaneAccessor, sSPTelopPane[nTeam], ANIM_TELOP_0, ANIM_TELOP_1, ANIM_TELOP_2, ANIM_TELOP_3, fRate);
}

void msw::CCockpitBasketCoder::SetMissionCountDisp(int bDisp)
{
    m_MissionCount.SetDisp(m_pPaneAccessor, bDisp);
}

void msw::CCockpitBasketCoder::SetMissionCountNum(long nNum, long nMax)
{
    m_MissionCount.SetNum(m_pPaneAccessor, nNum, nMax);
}

void msw::CCockpitBasketCoder::SetMissionCountUpNum(long nNum, long nMax)
{
    m_MissionCount.SetCountUpNum(m_pPaneAccessor, nNum, nMax);
}

void msw::CCockpitBasketCoder::SetMissionCountActiveColor(int bActive)
{
    m_MissionCount.SetActiveColor(m_pPaneAccessor, bActive);
}

void msw::CCockpitBasketCoder::SetFaceIcon()
{
    SetFaceIcon(GetLytObj());
}

void msw::CCockpitBasketCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return;
    }

    // The characters of each team.
    long chrNo[TEAM_NUM][PLAYER_NUM] = {
        { 0, 2, 4 },
        { 1, 3, 5 }
    };
    long nPlayerNum = menu::GetTeamPlayerNum();

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        for (long i = 0; i < nPlayerNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(chrNo[nTeam][i]);

            if (pChr != 0 && !pChr->IsDisable()) {
                SetFaceIcon(pPaneCtrl, nTeam, i, pChr);
            }
        }
    }

    // The frames for two players a team, or for one.
    int bTwo;
    int bOne;

    if (nPlayerNum == 2) {
        bTwo = 1;
        bOne = 0;
    } else {
        bTwo = 0;
        bOne = 1;
    }

    m_pPaneAccessor->SetPaneDisp(sFramePane[0][0], bTwo);
    m_pPaneAccessor->SetPaneDisp(sFramePane[1][0], bTwo);
    m_pPaneAccessor->SetPaneDisp(sFramePane[0][1], bOne);
    m_pPaneAccessor->SetPaneDisp(sFramePane[1][1], bOne);
}

void msw::CCockpitBasketCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer,
                                           const CChrCore *pChr)
{
    static const long texMapId = 0;

    long nPlayerNum = menu::GetTeamPlayerNum();
    long nColor = CGame::GetTeamColorIdx(nTeam);

    // The face picture.
    nw4r::lyt::Pane *pFaceParent = m_pPaneAccessor->GetPane(sFaceParentPane[nPlayerNum - 1][nColor][nPlayer]);

    if (pFaceParent == 0) {
        return;
    }

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nFace = sFacePane[nColor][nPlayer];
    nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(nFace);

    if (pFace == 0) {
        pFace = menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), pPaneCtrl, pFaceParent, sPaneNames[nFace]);
        bcs::G2D::SetPaneSizeFromParent(pFace);
    } else {
        const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

        if (pTexObj != 0) {
            bcs::G2D::ReplacePanePicture(pFace, pTexObj, texMapId);
        }
    }
    if (pFace != 0) {
        pFace->SetScale(menu::GetCockpitFaceIconScale(pChr->GetChrIdS()));
    }

    // The label: the pad of a human player, else "COM".
    nw4r::lyt::Pane *pLabelParent = m_pPaneAccessor->GetPane(sLabelParentPane[nPlayerNum - 1][nColor][nPlayer]);

    if (pLabelParent == 0) {
        return;
    }

    long nLabelSrc;

    if (pChr->IsPadCtrl()) {
        nLabelSrc = sIconPicPane[pChr->GetCtrlPlayerNo()];
    } else {
        nLabelSrc = sIconPicPane[ICON_PIC_COM];
    }

    nw4r::lyt::Pane *pLabelSrc = m_pPaneAccessor->GetPane(nLabelSrc);

    if (pLabelSrc == 0) {
        return;
    }

    const CChrCore *pLeader = cockpit::GetTeamLeaderChrCore(nTeam);
    long nLabel = sLabelPane[nColor][nPlayer];
    nw4r::lyt::Pane *pLabel = m_pPaneAccessor->GetPane(nLabel);

    if (pLabel == 0) {
        static const int recursive = 0;
        static const int reset = 1;

        pLabel = bcs::G2D::DuplicatePane(pPaneCtrl, pLabelSrc, sPaneNames[nLabel], sLayoutName[0], 0);
        bcs::G2D::SetPaneParent(pLabel, pLabelParent, 1, 0);
        pLabel->SetVisible(true);
        pLabel->SetTranslate(zero);

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pLabelParent);

        if (pObj != 0) {
            long nAnim;

            if (pChr->IsPadCtrl() || pChr == pLeader) {
                nAnim = ANIM_LABEL_PLAYER;
            } else {
                nAnim = ANIM_LABEL_PLAYER_OFF;
            }
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
        }
    } else {
        static const int recursive = 0;
        static const int reset = 1;

        // The label changes only while a human player leads the team.
        if (pLeader != 0 && !pLeader->IsPadCtrl()) {
            return;
        }

        if (nLabelSrc != sIconPicPane[ICON_PIC_COM]) {
            bcs::G2D::ReplacePanePicture(pLabel, pLabelSrc);
            pLabel->SetSize(pLabelSrc->GetSize());
            for (long i = 0; i < 4; i++) {
                pLabel->SetVtxColor(i, pLabelSrc->GetVtxColor(i));
            }
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pLabelParent);

        if (pObj != 0) {
            long nAnim = -1;

            if (nLabelSrc == sIconPicPane[ICON_PIC_COM]) {
                if (pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                    nAnim = ANIM_LABEL_COM;
                }
            } else if (!pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                nAnim = ANIM_LABEL_PLAYER;
            }
            if (nAnim >= 0) {
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            }
        }
    }

    // The star of a star player: a copy of cs_ico.
    nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
    nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(sStarParentPane[nPlayerNum - 1][nColor][nPlayer]);

    if (pStarSrc != 0 && pStarParent != 0) {
        long nStar = sStarPane[nColor][nPlayer];
        nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

        if (pStar == 0) {
            pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, sPaneNames[nStar], sLayoutName[0], 0);
            bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }

        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer());
        }
    }
}
