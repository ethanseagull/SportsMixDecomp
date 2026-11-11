// msw::CCockpitVolleyCoder: the volley game's score board (the scores, the
// coins, the sets won, the players' face icons and the mission counter).
//
// The structure follows the other sports' coders (see
// game/boss/CCockpitBossCoder.cpp); the tables are volley's own. There are
// three layouts: one screen (with the mission counter), and the two split
// screen ones.
#include "game/msw/CCockpitVolleyCoder.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CSetCountUp.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 3;

    // Number of teams.
    const long TEAM_NUM = 2;

    // By layout (CGame::g_GameWork.m_nViewSplit: 0, 1 or 2).
    const char *sLayoutNameData[] = {
        "VO_cockpit.brlyt",
        "VO_cockpit_02.brlyt",
        "VO_cockpit_03.brlyt"
    };

    // A copy of sLayoutNameData made by the static initialiser; the code
    // reads only this one.
    const char *sLayoutName[] = {
        sLayoutNameData[0],
        sLayoutNameData[1],
        sLayoutNameData[2]
    };

    const char *sAnimNames01[] = {
        "VO_cockpit_Score_nom_01.brlan",
        "VO_cockpit_Set_00.brlan",
        "VO_cockpit_Set_01.brlan",
        "VO_cockpit_Set_02.brlan",
        "VO_cockpit_Set_03.brlan",
        "VO_cockpit_Set_coin_00.brlan",
        "VO_cockpit_Set_coin_01.brlan",
        "VO_cockpit_Set_coin_03.brlan",
        "VO_cockpit_TO_SET_OFF.brlan",
        "VO_cockpit_TO_SET_ON.brlan",
        "VO_cockpit_Vis_set_00.brlan",
        "VO_cockpit_Vis_set_01.brlan",
        "VO_cockpit_Vis_set_ico_00.brlan",
        "VO_cockpit_Vis_set_ico_01.brlan",
        "VO_cockpit_Vis_set_ico_02.brlan",
        "VO_cockpit_S_sp_00.brlan",
        "VO_cockpit_S_sp_01.brlan",
        "VO_cockpit_S_sp_02.brlan",
        "VO_cockpit_S_sp_03.brlan",
        "VO_cockpit_SC_00.brlan",
        "VO_cockpit_SC_01.brlan",
        "VO_cockpit_SC_02.brlan",
        "VO_cockpit_SC_03.brlan",
        "VO_cockpit_C_ani_00.brlan",
        "VO_cockpit_C_ani_01.brlan",
        "VO_cockpit_C_ani_02.brlan",
        "VO_cockpit_C_ani_03.brlan",
        "VO_cockpit_M_00.brlan",
        "VO_cockpit_M_01.brlan",
        "VO_cockpit_M_02.brlan",
        "VO_cockpit_M_03.brlan"
    };

    const char *sAnimNames02[] = {
        "VO_cockpit_02_Score_nom_01.brlan",
        "VO_cockpit_02_Set_00.brlan",
        "VO_cockpit_02_Set_01.brlan",
        "VO_cockpit_02_Set_02.brlan",
        "VO_cockpit_02_Set_03.brlan",
        "VO_cockpit_02_Set_coin_00.brlan",
        "VO_cockpit_02_Set_coin_01.brlan",
        "VO_cockpit_02_Set_coin_03.brlan",
        "VO_cockpit_02_TO_SET_OFF.brlan",
        "VO_cockpit_02_TO_SET_ON.brlan",
        "VO_cockpit_02_Vis_set_00.brlan",
        "VO_cockpit_02_Vis_set_01.brlan",
        "VO_cockpit_02_Vis_set_ico_00.brlan",
        "VO_cockpit_02_Vis_set_ico_01.brlan",
        "VO_cockpit_02_Vis_set_ico_02.brlan",
        "VO_cockpit_02_S_sp_00.brlan",
        "VO_cockpit_02_S_sp_01.brlan",
        "VO_cockpit_02_S_sp_02.brlan",
        "VO_cockpit_02_S_sp_03.brlan",
        "VO_cockpit_02_SC_00.brlan",
        "VO_cockpit_02_SC_01.brlan",
        "VO_cockpit_02_SC_02.brlan",
        "VO_cockpit_02_SC_03.brlan"
    };

    const char *sAnimNames03[] = {
        "VO_cockpit_03_Score_nom_01.brlan",
        "VO_cockpit_03_Set_00.brlan",
        "VO_cockpit_03_Set_01.brlan",
        "VO_cockpit_03_Set_02.brlan",
        "VO_cockpit_03_Set_03.brlan",
        "VO_cockpit_03_Set_coin_00.brlan",
        "VO_cockpit_03_Set_coin_01.brlan",
        "VO_cockpit_03_Set_coin_03.brlan",
        "VO_cockpit_03_TO_SET_OFF.brlan",
        "VO_cockpit_03_TO_SET_ON.brlan",
        "VO_cockpit_03_Vis_set_00.brlan",
        "VO_cockpit_03_Vis_set_01.brlan",
        "VO_cockpit_03_Vis_set_ico_00.brlan",
        "VO_cockpit_03_Vis_set_ico_01.brlan",
        "VO_cockpit_03_Vis_set_ico_02.brlan",
        "VO_cockpit_03_S_sp_00.brlan",
        "VO_cockpit_03_S_sp_01.brlan",
        "VO_cockpit_03_S_sp_02.brlan",
        "VO_cockpit_03_S_sp_03.brlan",
        "VO_cockpit_03_SC_00.brlan",
        "VO_cockpit_03_SC_01.brlan",
        "VO_cockpit_03_SC_02.brlan",
        "VO_cockpit_03_SC_03.brlan"
    };

    // By layout.
    const char **sAnimNames[] = {
        sAnimNames01,
        sAnimNames02,
        sAnimNames03,
        0
    };

    const long sAnimNum[] = {
        sizeof(sAnimNames01) / sizeof(sAnimNames01[0]),
        sizeof(sAnimNames02) / sizeof(sAnimNames02[0]),
        sizeof(sAnimNames03) / sizeof(sAnimNames03[0])
    };

    // Animations (indexes into the sAnimNames tables; the first 23 are the
    // same in the three layouts).
    enum {
        ANIM_SERVE_IN_WAIT = 2,
        ANIM_SET_0 = 1,
        ANIM_SET_1 = 2,
        ANIM_SET_2 = 3,
        ANIM_SET_3 = 4,
        ANIM_OUT = 5,
        ANIM_OUT_WAIT = 6,
        ANIM_IN = 7,
        ANIM_SET_OFF = 8,
        ANIM_SET_ON = 9,
        ANIM_SET_ICO_OFF = 12,
        ANIM_SET_ICO_ON = 13,
        ANIM_SET_ICO_UP = 14,
        ANIM_LABEL_PLAYER_OFF = 15,
        ANIM_LABEL_PLAYER = 16,
        ANIM_LABEL_COM = 18,
        ANIM_COIN_DOWN_WAIT = 19,
        ANIM_COIN_UP = 20,
        ANIM_COIN_UP_WAIT = 21,
        ANIM_COIN_DOWN = 22,
        ANIM_MISSION_OUT_WAIT = 27,
        ANIM_MISSION_OUT = 28,
        ANIM_MISSION_IN_WAIT = 29,
        ANIM_MISSION_IN = 30
    };

    const char *sPaneNames[] = {
        "set_countLNull_B",
        "set_cntLNull_B01",
        "set_cntLNull_B02",
        "set_countLNull_D",
        "set_cntLNull_D01",
        "set_cntLNull_D02",
        "set_cntLNull_D03",
        "set_countRNull_B",
        "set_cntRNull_B01",
        "set_cntRNull_B02",
        "set_countRNull_D",
        "set_cntRNull_D01",
        "set_cntRNull_D02",
        "set_cntRNull_D03",
        "set_L",
        "set_R",
        "set_line",
        "set_font_add",
        "coin_L",
        "coin_R",
        "coinL_SP",
        "coinR_SP",
        "score_L",
        "score_R",
        "tn_t_coin_L",
        "tn_t_coin_R",
        "score_countL_k",
        "score_countL",
        "score_countR_k",
        "score_countR",
        "score_countL_2k",
        "score_countL_2",
        "score_countR_2k",
        "score_countR_2",
        "coin_L_ani",
        "coin_R_ani",
        "SP_l",
        "SP_r",
        "SP_lf",
        "SP_rf",
        "f_l",
        "f_r",
        "SP_cont_L",
        "SP_cont_R",
        "R_null",
        "coin_L_vis",
        "coin_R_vis",
        "L_A01",
        "L_A02",
        "L_B01",
        "L_B02",
        "L_B03",
        "L_C01",
        "L_C02",
        "L_C03",
        "L_C04",
        "L_D01",
        "L_D02",
        "L_D03",
        "L_D04",
        "L_D05",
        "R_A01",
        "R_A02",
        "R_B01",
        "R_B02",
        "R_B03",
        "R_C01",
        "R_C02",
        "R_C03",
        "R_C04",
        "R_D01",
        "R_D02",
        "R_D03",
        "R_D04",
        "R_D05",
        "no_e_L",
        "no_e_R",
        "L_num_B_01",
        "L_num_B_02",
        "L_num_A",
        "tournament_L",
        "L_effect",
        "score_line",
        "score_sca",
        "score_font_add",
        "mov_t_LR",
        "text_L1",
        "text_L1",
        "text_R1",
        "text_R1",
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
        PANE_set_line = 16,
        PANE_set_font_add = 17,
        PANE_L_num_B_01 = 77,
        PANE_L_num_B_02 = 78,
        PANE_L_num_A = 79,
        PANE_tournament_L = 80,
        PANE_L_effect = 81,
        PANE_score_line = 82,
        PANE_score_sca = 83,
        PANE_score_font_add = 84,
        PANE_mov_t_LR = 85,
        PANE_cs_ico = 129
    };

    // The won-set marks of each team, for a match of three and of five
    // sets.
    const unsigned char sSetCountPaneB[TEAM_NUM][5] = {
        { 1, 2 },
        { 8, 9 }
    };
    const unsigned char sSetCountPaneD[TEAM_NUM][5] = {
        { 4, 5, 6 },
        { 11, 12, 13 }
    };

    // The sets to win, by sets in the match minus 2.
    const unsigned char sSetWinNum[4] = { 2, 2, 3, 3 };
    // The set counter groups of each team, by sets in the match minus 2:
    // only the one of the match is shown.
    const unsigned char sSetCountNullPane[TEAM_NUM][4] = {
        { 0, 0, 3, 3 },
        { 7, 7, 10, 10 }
    };

    // The score panes of each team, by layout.
    const unsigned char sScorePane[3][TEAM_NUM] = {
        { 22, 23 },
        { 82, 82 },
        { 22, 23 }
    };
    // The score text of each team: below 100, from 100.
    const unsigned char sScoreCountPane[TEAM_NUM][2] = {
        { 27, 31 },
        { 29, 33 }
    };
    // Their outlines.
    const unsigned char sScoreCountKPane[TEAM_NUM][2] = {
        { 26, 30 },
        { 28, 32 }
    };
    // The score labels of each team: from 100, below 100.
    const unsigned char sScoreTextPane[TEAM_NUM][2] = {
        { 86, 87 },
        { 88, 89 }
    };
    const unsigned char sCoinPane[TEAM_NUM] = { 18, 19 };
    // Not read by anything left in the file.
    const unsigned char sCoinVisPane[TEAM_NUM] = { 45, 46 };
    const unsigned char sCoinSPPane[TEAM_NUM] = { 20, 21 };
    const unsigned char sSetPane[TEAM_NUM] = { 14, 15 };

    // The mission counter's panes, by team; only the first team has one.
    const unsigned char sMissionPane[TEAM_NUM] = { 80, 80 };
    const unsigned char sMissionEffectPane[TEAM_NUM] = { 81, 81 };
    // The two count boxes and the maximum.
    const unsigned char sMissionNumPane[TEAM_NUM][3] = {
        { 77, 78, 79 },
        { 77, 78, 79 }
    };

    // The face icon panes, by players per team minus 1, team colour and
    // player.
    const unsigned char sFaceParentPane[3][2][3] = {
        { { 94 }, { 104 } },
        { { 97, 98 }, { 107, 108 } },
        { { 94, 95, 96 }, { 104, 105, 106 } }
    };
    // By team colour and player: the face picture's name.
    const unsigned char sFacePane[2][3] = {
        { 132, 133, 134 },
        { 135, 136, 137 }
    };
    const unsigned char sLabelParentPane[3][2][3] = {
        { { 99 }, { 109 } },
        { { 102, 103 }, { 112, 113 } },
        { { 99, 100, 101 }, { 109, 110, 111 } }
    };
    // The labels: a human player's ("1P" to "4P"), then the computer's.
    const unsigned char sLabelPane[5] = { 114, 115, 116, 117, 118 };
    // By team colour and player: the label copy's name.
    const unsigned char sLabelCopyPane[2][3] = {
        { 138, 139, 140 },
        { 141, 142, 143 }
    };
    // The label frames of each team: for two players, for three.
    const unsigned char sLabelFramePane[TEAM_NUM][2] = {
        { 91, 90 },
        { 93, 92 }
    };
    const unsigned char sStarParentPane[3][2][3] = {
        { { 119 }, { 124 } },
        { { 122, 123 }, { 127, 128 } },
        { { 119, 120, 121 }, { 124, 125, 126 } }
    };
    // By team colour and player: the star copy's name.
    const unsigned char sStarPane[2][3] = {
        { 144, 145, 146 },
        { 147, 148, 149 }
    };
    // The coin counter of each team.
    const unsigned char sCoinCountPane[TEAM_NUM] = { 130, 131 };
    // The effect of a won-set mark, by team.
    const long sSetEffect[TEAM_NUM] = { 0x45E, 0x45F };

    // Stand-in for an unknown user of sCoinVisPane, which the original keeps
    // in .rodata between the coin tables but nothing reads (likely stripped
    // code).
    inline void UnusedReferences()
    {
        (void)sCoinVisPane;
    }

    // By sets in the match minus 2.
    const unsigned char (*sSetCountPane[4])[5] = {
        sSetCountPaneB,
        sSetCountPaneB,
        sSetCountPaneD,
        sSetCountPaneD
    };
}

msw::CCockpitVolleyCoder::CCockpitVolleyCoder(long nId)
    : CCockpitVolleyLayoutBase(nId)
{
    m_nStep = 0;
    m_nSetNum = 0;
    m_nSetPoint[0] = 0;
    m_nSetPoint[1] = 0;
    m_Unk63[0] = 0;
    m_Unk63[1] = 0;
    m_nLayout = 0;
    m_bMission = 0;
}

msw::CCockpitVolleyCoder::~CCockpitVolleyCoder()
{
    Remove();
}

void msw::CCockpitVolleyCoder::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_bMission) {
        m_MissionCount.MainLoop(m_pPaneAccessor);
    }

    if (m_nStep == 1) {
        SetupGame();
        PlayAnim(ANIM_SET_2, 0);
        SetFaceIcon(GetLytObj());
        m_nStep = 2;
    }
}

void msw::CCockpitVolleyCoder::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitVolleyCoder::Set()
{
    // static-order swap (notes/decisions.md): the game declares teamNum
    // first and stores it second.
    static const long iconPicTblLength = sizeof(sLabelPane) / sizeof(sLabelPane[0]);
    static const long teamNum = TEAM_NUM;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();

    // The sets in the match, by the match length setting.
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

    long nLayout;
    long bMission;

    switch (CGame::g_GameWork.m_nViewSplit) {
        case 1:
            nLayout = 1;
            bMission = 0;
            break;
        case 2:
            nLayout = 2;
            bMission = 0;
            break;
        default:
            nLayout = 0;
            bMission = 1;
            break;
    }
    m_nLayout = nLayout;
    m_bMission = bMission;

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
            pPaneCtrl->BuildLayout(0, sLayoutName[nLayout]);
            pPaneCtrl->BuildAnim(0, sAnimNames[nLayout], sAnimNum[nLayout]);
            SetPaneAccessor(pPaneCtrl, sPaneNames, sizeof(sPaneNames) / sizeof(sPaneNames[0]));
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(0);
            m_EffectSet.Initialize();

            if (m_bMission) {
                m_MissionCount.SetParameter(sMissionPane[0], sMissionNumPane[0][0], sMissionNumPane[0][1],
                                            sMissionNumPane[0][2], sMissionEffectPane[0], 26, 23, 24, 25, ANIM_SET_ON,
                                            ANIM_SET_OFF);
            }

            for (long i = 0; i < iconPicTblLength; i++) {
                m_pPaneAccessor->SetPaneDisp(sLabelPane[i], 0);
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitVolleyCoder::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
}

long msw::CCockpitVolleyCoder::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitVolleyCoder::SetupGame()
{
    // static-order swap (notes/decisions.md): the game declares an animId,
    // reset and recursive in each branch and stores them in the reverse
    // order; declared here so that the values come out in its order.
    static const int recursiveOff = 0;
    static const int resetOff = 1;
    static const long animIdOff = ANIM_SET_OFF;
    static const int recursive = 0;
    static const int reset = 1;
    static const long animId = ANIM_SET_ON;

    int bSetMatch = m_nSetNum > 1;

    UnusedReferences();

    if (bSetMatch == 1) {
        m_pPaneAccessor->PlayAnimation(14, animId, 1.0f, 0.0f, recursive, reset);
        m_pPaneAccessor->PlayAnimation(15, animId, 1.0f, 0.0f, recursive, reset);
        m_pPaneAccessor->PlayAnimation(PANE_set_line, animId, 1.0f, 0.0f, recursive, reset);
        m_pPaneAccessor->PlayAnimation(PANE_set_font_add, animId, 1.0f, 0.0f, recursive, reset);

        long nIdx = m_nSetNum - 2;

        if (nIdx >= 0) {
            for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                for (long i = 0; i < 4; i++) {
                    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetCountNullPane[nTeam][i]);

                    if (pObj != 0) {
                        pObj->GetPane()->SetVisible(i == nIdx);
                    }
                }
            }
        }
    } else {
        m_pPaneAccessor->PlayAnimation(14, animIdOff, 1.0f, 0.0f, recursiveOff, resetOff);
        m_pPaneAccessor->PlayAnimation(15, animIdOff, 1.0f, 0.0f, recursiveOff, resetOff);
        m_pPaneAccessor->PlayAnimation(PANE_set_line, animIdOff, 1.0f, 0.0f, recursiveOff, resetOff);
        m_pPaneAccessor->PlayAnimation(PANE_set_font_add, animIdOff, 1.0f, 0.0f, recursiveOff, resetOff);
    }

    long i;

    // Looked up and not used.
    for (i = 0; i < TEAM_NUM; i++) {
        m_pPaneAccessor->GetPaneObj(sCoinSPPane[i]);
        m_pPaneAccessor->GetPaneObj(sCoinPane[i]);
    }

    // The scores start in the normal font (see SetScoreFont).
    const char *pFontName = "FOT-RS_NO_02.brfnt";

    for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        for (i = 0; i < 2; i++) {
            long nKPane = sScoreCountKPane[nTeam][i];

            cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(sScoreCountPane[nTeam][i]),
                                 sLayoutName[m_nLayout], pFontName);
            cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(nKPane), sLayoutName[m_nLayout], pFontName);
        }
    }

    PlayAnimCoinBack(ANIM_COIN_DOWN);

    if (m_bMission) {
        m_MissionCount.SetMove(m_pPaneAccessor, ANIM_MISSION_OUT_WAIT);
    }
    if (m_bMission) {
        m_MissionCount.SetDisp(m_pPaneAccessor, 0);
    }
    return 0;
}

int msw::CCockpitVolleyCoder::ScoreOut()
{
    PlayAnim(ANIM_OUT, 0);
    return 0;
}

int msw::CCockpitVolleyCoder::ScoreIn()
{
    PlayAnim(ANIM_IN, 1);
    return 0;
}

int msw::CCockpitVolleyCoder::ScoreOutWait()
{
    PlayAnim(ANIM_OUT_WAIT, 0);
    return 0;
}

int msw::CCockpitVolleyCoder::ScoreInWait()
{
    PlayAnim(ANIM_SET_2, 0);
    return 0;
}

int msw::CCockpitVolleyCoder::CoinOut()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_OUT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    if (m_bMission) {
        m_MissionCount.SetMove(m_pPaneAccessor, ANIM_MISSION_OUT);
    }
    return 0;
}

int msw::CCockpitVolleyCoder::CoinIn()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_IN, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    if (m_bMission) {
        m_MissionCount.SetMove(m_pPaneAccessor, ANIM_MISSION_IN);
    }
    return 0;
}

int msw::CCockpitVolleyCoder::CoinOutWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_OUT_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    if (m_bMission) {
        m_MissionCount.SetMove(m_pPaneAccessor, ANIM_MISSION_OUT_WAIT);
    }
    return 0;
}

int msw::CCockpitVolleyCoder::CoinInWait()
{
    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(ANIM_SERVE_IN_WAIT, 1.0f, 0.0f, 0, 1, 1);
        }
    }
    if (m_bMission) {
        m_MissionCount.SetMove(m_pPaneAccessor, ANIM_MISSION_IN_WAIT);
    }
    return 0;
}

long msw::CCockpitVolleyCoder::SetServeScoreAnim(long nState)
{
    if (nState < 0 || nState > 3) {
        return -1;
    }

    long nAnim;
    int bSetCount;

    switch (nState) {
        case 0:
            nAnim = ANIM_SET_3;
            bSetCount = 1;
            break;
        case 1:
            nAnim = ANIM_SET_0;
            bSetCount = 0;
            break;
        case 2:
            nAnim = ANIM_SET_1;
            bSetCount = 0;
            break;
        case 3:
            nAnim = ANIM_SET_2;
            bSetCount = 0;
            break;
    }
    PlayAnim(nAnim, bSetCount);
    return 0;
}

long msw::CCockpitVolleyCoder::PlayAnim(long nAnim, int bSetCount)
{
    static const int reset = 1;

    long i;

    if (m_nSetNum > 1) {
        for (i = 0; i < TEAM_NUM; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetPane[i]);

            if (pObj != 0) {
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
            }
        }
        m_pPaneAccessor->PlayAnimation(PANE_set_line, nAnim, 1.0f, 0.0f, 0, 1);
        m_pPaneAccessor->PlayAnimation(PANE_set_font_add, nAnim, 1.0f, 0.0f, 0, 1);

        long nIdx = m_nSetNum - 2;

        if (nIdx >= 0) {
            long nWin = sSetWinNum[nIdx];

            if (bSetCount) {
                // The marks of the sets won light one after the other.
                for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                    long nPoint = m_nSetPoint[nTeam];

                    for (i = 0; i < nWin; i++) {
                        long nPane = sSetCountPane[nIdx][nTeam][i];
                        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                        if (pObj != 0) {
                            pObj->GetPane()->SetVisible(true);
                            pObj->PlayAnimation(ANIM_SET_ICO_OFF, 1.0f, 0.0f, 1, reset, 1);
                        }

                        if (i < nPoint) {
                            unsigned long nKind = nPane + 0x1000;
                            long nEffect = sSetEffect[nTeam];
                            long nFlag = m_bUnk65[nTeam];
                            menu::CDispTaskLink *pPrev = m_DispTaskList.FindNextKindObj(nKind, 0);

                            if (pPrev != 0) {
                                pPrev->m_bEnd = 1;
                            }

                            cockpit::CSetCountUp *pTask = new cockpit::CSetCountUp;

                            pTask->SetKind(nKind);
                            pTask->SetPane(nPane);
                            pTask->m_fDelay = 5.0f + 10.0f * i;
                            pTask->m_nUnk34 = 16;
                            pTask->m_nUnk36 = 4;
                            pTask->m_nUnk38 = nFlag;
                            pTask->m_nEffect = nEffect;
                            pTask->m_nWaitAnim = ANIM_SET_ICO_UP;
                            pTask->m_nAnim = ANIM_SET_ICO_ON;
                            m_DispTaskList.PushBack(pTask);
                        }
                    }
                }
            } else {
                for (long nTeam = 0; nTeam < TEAM_NUM; nTeam++) {
                    long nPoint = m_nSetPoint[nTeam];

                    for (i = 0; i < nWin; i++) {
                        long nPane = sSetCountPane[nIdx][nTeam][i];
                        long nSetAnim = ANIM_SET_ICO_OFF;

                        if (i < nPoint) {
                            nSetAnim = ANIM_SET_ICO_ON;
                        }

                        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

                        if (pObj != 0) {
                            pObj->GetPane()->SetVisible(true);
                            pObj->PlayAnimation(nSetAnim, 1.0f, 0.0f, 1, reset, 1);
                        }
                    }
                }
            }
        }
    }

    long nLayout = m_nLayout;

    for (i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nLayout][i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);

            if (m_bUnk65[i]) {
                m_pPaneAccessor->PlayAnimation(sScoreCountKPane[i][0], nAnim, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(sScoreCountKPane[i][1], nAnim, 1.0f, 0.0f, 1, 1);
                m_bUnk65[i] = 0;
            }
        }
    }

    if (nLayout > 0) {
        m_pPaneAccessor->PlayAnimation(PANE_score_line, nAnim, 1.0f, 0.0f, 0, 1);
        m_pPaneAccessor->PlayAnimation(PANE_score_sca, nAnim, 1.0f, 0.0f, 0, 1);
        m_pPaneAccessor->PlayAnimation(PANE_score_font_add, nAnim, 1.0f, 0.0f, 0, 1);
        m_pPaneAccessor->PlayAnimation(PANE_mov_t_LR, nAnim, 1.0f, 0.0f, 1, 1);
    }
    return 0;
}

long msw::CCockpitVolleyCoder::PlayAnimCoinBack(long nAnim)
{
    static const int recursive = 1;
    static const int reset = 1;

    long nLayout = m_nLayout;

    if (nLayout == 1) {
        return 0;
    }

    for (long i = 0; i < TEAM_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nLayout][i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            if (nAnim == ANIM_COIN_UP) {
                pObj->PushNextAnimation(ANIM_COIN_UP_WAIT, 1.0f, 0.0f, recursive, reset);
            } else if (nAnim == ANIM_COIN_DOWN) {
                pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
    return 0;
}

void msw::CCockpitVolleyCoder::SetScoreValue(long nTeam, long nScore)
{
    if (nTeam >= TEAM_NUM) {
        return;
    }

    // The game checks the team twice: the switch is its second, unsigned,
    // range check (it also rejects a negative team).
    switch (nTeam) {
        case 0:
        case 1: {
            // Two digits and three digits have their own labels and text
            // boxes.
            int bTwoDigits = nScore < 100;
            int bThreeDigits = nScore >= 100;

            m_pPaneAccessor->SetPaneDisp(sScoreTextPane[nTeam][bTwoDigits], 0);
            m_pPaneAccessor->SetPaneDisp(sScoreTextPane[nTeam][bThreeDigits], 1);

            long nPane = sScoreCountPane[nTeam][nScore >= 100];

            if (nScore >= 0) {
                m_pPaneAccessor->SetTextBoxString(nPane, nScore, 0);
                m_pPaneAccessor->SetPaneDisp(nPane, 1);
            } else {
                m_pPaneAccessor->SetPaneDisp(nPane, 0);
            }
            break;
        }
    }
}

void msw::CCockpitVolleyCoder::SetCoinValue(long nTeam, long nDiff)
{
    long nLayout = m_nLayout;

    if (nLayout == 1) {
        return;
    }

    if (nDiff > 0) {
        static const int recursive = 1;
        static const int reset = 1;

        bcs::G2D::CPaneObj *pCount = m_pPaneAccessor->GetPaneObj(sCoinCountPane[nTeam]);

        if (pCount != 0 && !pCount->IsAnimPlay(ANIM_COIN_UP) && !pCount->IsAnimPlay(ANIM_COIN_UP_WAIT)) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nLayout][nTeam]);

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
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sScorePane[nLayout][nTeam]);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_COIN_DOWN, 1.0f, 0.0f, recursive, reset, 1);
                pObj->PushNextAnimation(ANIM_COIN_DOWN_WAIT, 1.0f, 0.0f, recursive, reset);
            }
        }
    }
}

void msw::CCockpitVolleyCoder::SetScoreDisp(int bDisp)
{
    long nLayout = m_nLayout;

    m_pPaneAccessor->SetPaneDisp(sScorePane[nLayout][0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sScorePane[nLayout][1], bDisp);
    m_pPaneAccessor->SetPaneDisp(PANE_score_font_add, bDisp);
}

void msw::CCockpitVolleyCoder::SetSetCountDisp(int bDisp)
{
    int bSetMatch = m_nSetNum > 1;

    if (bSetMatch != 0) {
        for (long i = 0; i < TEAM_NUM; i++) {
            m_pPaneAccessor->SetPaneDisp(sSetPane[i], bDisp);
        }
    }
}

void msw::CCockpitVolleyCoder::SetSetCountPoint(long nTeam, long nPoint)
{
    static const int recursive = 1;
    static const int reset = 1;

    if (nTeam >= TEAM_NUM) {
        return;
    }

    long nSetNum = m_nSetNum;

    if (nPoint > nSetNum) {
        nPoint = nSetNum;
    }
    m_nSetPoint[nTeam] = nPoint;

    long nIdx = nSetNum - 2;

    if (nIdx >= 0) {
        long nWin = sSetWinNum[nIdx];

        for (long i = 0; i < nWin; i++) {
            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sSetCountPane[nIdx][nTeam][i]);

            if (pObj != 0) {
                long nAnim = ANIM_SET_ICO_OFF;

                if (i < nPoint) {
                    nAnim = ANIM_SET_ICO_ON;
                }
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            }
        }
    }
}

void msw::CCockpitVolleyCoder::SetScoreFont(long nTeam, int bFont)
{
    if (nTeam >= TEAM_NUM) {
        return;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    const char *pFontName = bFont ? "FOT-RS_NO_01.brfnt" : "FOT-RS_NO_02.brfnt";

    for (long i = 0; i < 2; i++) {
        long nKPane = sScoreCountKPane[nTeam][i];

        cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(sScoreCountPane[nTeam][i]), sLayoutName[m_nLayout],
                             pFontName);
        cockpit::SetTextFont(pPaneCtrl, m_pPaneAccessor->GetPane(nKPane), sLayoutName[m_nLayout], pFontName);
    }
}

void msw::CCockpitVolleyCoder::SetMissionCountDisp(int bDisp)
{
    if (m_bMission) {
        m_MissionCount.SetDisp(m_pPaneAccessor, bDisp);
    }
}

void msw::CCockpitVolleyCoder::SetMissionCountNum(long nNum, long nMax)
{
    if (m_bMission) {
        m_MissionCount.SetNum(m_pPaneAccessor, nNum, nMax);
    }
}

void msw::CCockpitVolleyCoder::SetMissionCountUpNum(long nNum, long nMax)
{
    if (m_bMission) {
        m_MissionCount.SetCountUpNum(m_pPaneAccessor, nNum, nMax);
    }
}

void msw::CCockpitVolleyCoder::SetMissionCountActiveColor(int bActive)
{
    if (m_bMission) {
        m_MissionCount.SetActiveColor(m_pPaneAccessor, bActive);
    }
}

void msw::CCockpitVolleyCoder::SetFaceIcon()
{
    SetFaceIcon(GetLytObj());
}

void msw::CCockpitVolleyCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return;
    }

    // The characters of each team.
    long chrNo[TEAM_NUM][3] = {
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

    int bTwo;
    int bThree;

    if (nPlayerNum == 2) {
        bTwo = 1;
        bThree = 0;
    } else {
        bTwo = 0;
        bThree = 1;
    }
    m_pPaneAccessor->SetPaneDisp(sLabelFramePane[0][0], bTwo);
    m_pPaneAccessor->SetPaneDisp(sLabelFramePane[1][0], bTwo);
    m_pPaneAccessor->SetPaneDisp(sLabelFramePane[0][1], bThree);
    m_pPaneAccessor->SetPaneDisp(sLabelFramePane[1][1], bThree);
}

void msw::CCockpitVolleyCoder::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nPlayer,
                                           const CChrCore *pChr)
{
    // static-order swap (notes/decisions.md): the game declares texMapId
    // first and a recursive and reset in each label branch, and stores them
    // in the reverse order; declared here so that the values come out in
    // its order.
    static const int resetUpdate = 1;
    static const int recursiveUpdate = 0;
    static const int reset = 1;
    static const int recursive = 0;
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
        nLabelSrc = sLabelPane[pChr->GetCtrlPlayerNo()];
    } else {
        nLabelSrc = sLabelPane[4];
    }

    nw4r::lyt::Pane *pLabelSrc = m_pPaneAccessor->GetPane(nLabelSrc);

    if (pLabelSrc == 0) {
        return;
    }

    const CChrCore *pLeader = cockpit::GetTeamLeaderChrCore(nTeam);
    long nLabel = sLabelCopyPane[nColor][nPlayer];
    nw4r::lyt::Pane *pLabel = m_pPaneAccessor->GetPane(nLabel);

    if (pLabel == 0) {
        pLabel = bcs::G2D::DuplicatePane(pPaneCtrl, pLabelSrc, sPaneNames[nLabel], sLayoutName[m_nLayout], 0);
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
        // The label changes only while a human player leads the team.
        if (pLeader != 0 && !pLeader->IsPadCtrl()) {
            return;
        }

        if (nLabelSrc != sLabelPane[4]) {
            bcs::G2D::ReplacePanePicture(pLabel, pLabelSrc);
            pLabel->SetSize(pLabelSrc->GetSize());
            for (long i = 0; i < 4; i++) {
                pLabel->SetVtxColor(i, pLabelSrc->GetVtxColor(i));
            }
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pLabelParent);

        if (pObj != 0) {
            long nAnim = -1;

            if (nLabelSrc == sLabelPane[4]) {
                if (pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                    nAnim = ANIM_LABEL_COM;
                }
            } else if (!pObj->IsAnimPlay(ANIM_LABEL_PLAYER)) {
                nAnim = ANIM_LABEL_PLAYER;
            }
            if (nAnim >= 0) {
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursiveUpdate, resetUpdate, 1);
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
            pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, sPaneNames[nStar], sLayoutName[m_nLayout], 0);
            bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }

        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer());
        }
    }
}
