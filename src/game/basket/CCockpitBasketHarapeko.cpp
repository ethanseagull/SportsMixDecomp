// msw::CCockpitBasketHarapeko: the cockpit of the basket "harapeko" (hungry)
// game. The same layout code exists for HockeyF's CCockpitHockeyClash.
#include "game/msw/CCockpitBasketHarapeko.hpp"

#include <wchar.h>
#include "bcs/CMainApp.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/cockpit/CCharaIcon.hpp"
#include "game/msw/menu/CDispTaskList.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // The layout and its animations.
    struct {
        const char *pLayout;
        const char *pAnim[23];
    } s_LayoutRes = {
        "BA_harapeko.brlyt",
        {
            "BA_harapeko_ca_00.brlan",
            "BA_harapeko_ca_01.brlan",
            "BA_harapeko_ca_02.brlan",
            "BA_harapeko_ca_03.brlan",
            "BA_harapeko_ca_04.brlan",
            "BA_harapeko_mov_00.brlan",
            "BA_harapeko_mov_01.brlan",
            "BA_harapeko_mov_02.brlan",
            "BA_harapeko_mov_03.brlan",
            "BA_harapeko_no_00.brlan",
            "BA_harapeko_no_01.brlan",
            "BA_harapeko_no_02.brlan",
            "BA_harapeko_time_00.brlan",
            "BA_harapeko_time_01.brlan",
            "BA_harapeko_t_00.brlan",
            "BA_harapeko_t_01.brlan",
            "BA_harapeko_t_02.brlan",
            "BA_harapeko_Vis_00.brlan",
            "BA_harapeko_Vis_01.brlan",
            "BA_harapeko_Time_vis_00.brlan",
            "BA_harapeko_Time_vis_01.brlan",
            "BA_harapeko_Time_vis_02.brlan",
            "BA_harapeko_TC_00.brlan",
        },
    };

    // Panes, by index into s_PaneName.
    enum {
        PANE_time_00,        // 0x00
        PANE_P_Null,         // 0x01
        PANE_Sco_A,          // 0x02
        PANE_Sco_B,          // 0x03
        PANE_Sco_C,          // 0x04
        PANE_Sco_D,          // 0x05
        PANE_M_ScoL_Null,    // 0x06
        PANE_M_ScoR_Null,    // 0x07
        PANE_P_Null_vis,     // 0x08
        PANE_M_sco_Null,     // 0x09
        PANE_time,           // 0x0a
        PANE_text_time,      // 0x0b
        PANE_text_time02,    // 0x0c
        PANE_Time_co,        // 0x0d
        PANE_time_t,         // 0x0e
        PANE_time_no,        // 0x0f
        PANE_t_waku,         // 0x10
        PANE_ca,             // 0x11
        PANE_ca_t,           // 0x12
        PANE_F_Null,         // 0x13
        PANE_f_01,           // 0x14
        PANE_f_02,           // 0x15
        PANE_P_1,            // 0x16
        PANE_P_2,            // 0x17
        PANE_P_3,            // 0x18
        PANE_P_4,            // 0x19
        PANE_p1_num,         // 0x1a
        PANE_p1_num_00,      // 0x1b
        PANE_p2_num,         // 0x1c
        PANE_p2_num_00,      // 0x1d
        PANE_p3_num,         // 0x1e
        PANE_p3_num_00,      // 0x1f
        PANE_p4_num,         // 0x20
        PANE_p4_num_00,      // 0x21
        PANE_p1_fe_icon,     // 0x22
        PANE_p2_fe_icon,     // 0x23
        PANE_p3_fe_icon,     // 0x24
        PANE_p4_fe_icon,     // 0x25
        PANE_play_1P,        // 0x26
        PANE_play_p2,        // 0x27
        PANE_play_p3,        // 0x28
        PANE_play_p4,        // 0x29
        PANE_sco_A,          // 0x2a
        PANE_sco_B,          // 0x2b
        PANE_sco_C,          // 0x2c
        PANE_sco_D,          // 0x2d
        PANE_scoL_t,         // 0x2e
        PANE_scoR_t,         // 0x2f
        PANE_L_Fa01,         // 0x30
        PANE_L_Fa02,         // 0x31
        PANE_L_Fa03,         // 0x32
        PANE_L_Pa01,         // 0x33
        PANE_L_Pa02,         // 0x34
        PANE_L_Pa03,         // 0x35
        PANE_R_Fa01,         // 0x36
        PANE_R_Fa02,         // 0x37
        PANE_R_Fa03,         // 0x38
        PANE_R_Pa01,         // 0x39
        PANE_R_Pa02,         // 0x3a
        PANE_R_Pa03,         // 0x3b
        PANE_L_Fb01,         // 0x3c
        PANE_L_Fb02,         // 0x3d
        PANE_L_Pb01,         // 0x3e
        PANE_L_Pb02,         // 0x3f
        PANE_R_Fb01,         // 0x40
        PANE_R_Fb02,         // 0x41
        PANE_R_Pb01,         // 0x42
        PANE_R_Pb02,         // 0x43
        PANE_1P,             // 0x44
        PANE_2P,             // 0x45
        PANE_3P,             // 0x46
        PANE_4P,             // 0x47
        PANE_com_01,         // 0x48
        PANE_com_02,         // 0x49
        PANE_com_03,         // 0x4a
        PANE_com_04,         // 0x4b
        PANE_com_05,         // 0x4c
        PANE_crown_Null,     // 0x4d
        PANE_crown,          // 0x4e
        PANE_ico_C,          // 0x4f
        PANE_ico_D,          // 0x50
        PANE_f01_Null,       // 0x51
        PANE_f02_Null,       // 0x52
        PANE_Sco_D_Vis,      // 0x53
        PANE_L_Ca_01,        // 0x54
        PANE_L_Ca_02,        // 0x55
        PANE_L_Ca_03,        // 0x56
        PANE_L_Ca_04,        // 0x57
        PANE_L_Ca_05,        // 0x58
        PANE_R_Ca_01,        // 0x59
        PANE_R_Ca_02,        // 0x5a
        PANE_R_Ca_03,        // 0x5b
        PANE_R_Ca_04,        // 0x5c
        PANE_R_Ca_05,        // 0x5d
        PANE_p1_Ca,          // 0x5e
        PANE_p2_Ca,          // 0x5f
        PANE_p3_Ca,          // 0x60
        PANE_p4_Ca,          // 0x61
        PANE_cs_ico,         // 0x62
        PANE_dupFace00,      // 0x63
        PANE_dupFace01,      // 0x64
        PANE_dupFace02,      // 0x65
        PANE_dupFace03,      // 0x66
        PANE_dupFace04,      // 0x67
        PANE_dupFace05,      // 0x68
        PANE_dupStar00,      // 0x69
        PANE_dupStar01,      // 0x6a
        PANE_dupStar02,      // 0x6b
        PANE_dupStar03,      // 0x6c
        PANE_dupStar04,      // 0x6d
        PANE_dupStar05,      // 0x6e
        PANE_dPicSd00,       // 0x6f
        PANE_dPicSd01,       // 0x70
        PANE_dPicSd02,       // 0x71
        PANE_dPicSd10,       // 0x72
        PANE_dPicSd11,       // 0x73
        PANE_dPicSd12,       // 0x74
        PANE_dPicSd20,       // 0x75
        PANE_dPicSd21,       // 0x76
        PANE_dPicSd22,       // 0x77
        PANE_dPicSd30,       // 0x78
        PANE_dPicSd31,       // 0x79
        PANE_dPicSd32,       // 0x7a
        PANE_dPicSd40,       // 0x7b
        PANE_dPicSd41,       // 0x7c
        PANE_dPicSd42,       // 0x7d
        PANE_dPicSd50,       // 0x7e
        PANE_dPicSd51,       // 0x7f
        PANE_dPicSd52,       // 0x80
        PANE_dNulSd00,       // 0x81
        PANE_dNulSd01,       // 0x82
        PANE_dNulSd02,       // 0x83
        PANE_dNulSd10,       // 0x84
        PANE_dNulSd11,       // 0x85
        PANE_dNulSd12,       // 0x86
        PANE_dNulSd20,       // 0x87
        PANE_dNulSd21,       // 0x88
        PANE_dNulSd22,       // 0x89
        PANE_dNulSd30,       // 0x8a
        PANE_dNulSd31,       // 0x8b
        PANE_dNulSd32,       // 0x8c
        PANE_dNulSd40,       // 0x8d
        PANE_dNulSd41,       // 0x8e
        PANE_dNulSd42,       // 0x8f
        PANE_dNulSd50,       // 0x90
        PANE_dNulSd51,       // 0x91
        PANE_dNulSd52,       // 0x92
        PANE_dNulSd00a,      // 0x93
        PANE_dNulSd01a,      // 0x94
        PANE_dNulSd02a,      // 0x95
        PANE_dNulSd10a,      // 0x96
        PANE_dNulSd11a,      // 0x97
        PANE_dNulSd12a,      // 0x98
        PANE_dNulSd20a,      // 0x99
        PANE_dNulSd21a,      // 0x9a
        PANE_dNulSd22a,      // 0x9b
        PANE_dNulSd30a,      // 0x9c
        PANE_dNulSd31a,      // 0x9d
        PANE_dNulSd32a,      // 0x9e
        PANE_dNulSd40a,      // 0x9f
        PANE_dNulSd41a,      // 0xa0
        PANE_dNulSd42a,      // 0xa1
        PANE_dNulSd50a,      // 0xa2
        PANE_dNulSd51a,      // 0xa3
        PANE_dNulSd52a,      // 0xa4
        PANE_dPicSd00a,      // 0xa5
        PANE_dPicSd01a,      // 0xa6
        PANE_dPicSd02a,      // 0xa7
        PANE_dPicSd10a,      // 0xa8
        PANE_dPicSd11a,      // 0xa9
        PANE_dPicSd12a,      // 0xaa
        PANE_dPicSd20a,      // 0xab
        PANE_dPicSd21a,      // 0xac
        PANE_dPicSd22a,      // 0xad
        PANE_dPicSd30a,      // 0xae
        PANE_dPicSd31a,      // 0xaf
        PANE_dPicSd32a,      // 0xb0
        PANE_dPicSd40a,      // 0xb1
        PANE_dPicSd41a,      // 0xb2
        PANE_dPicSd42a,      // 0xb3
        PANE_dPicSd50a,      // 0xb4
        PANE_dPicSd51a,      // 0xb5
        PANE_dPicSd52a,      // 0xb6
        PANE_dPicSd00b,      // 0xb7
        PANE_dPicSd01b,      // 0xb8
        PANE_dPicSd02b,      // 0xb9
        PANE_dPicSd10b,      // 0xba
        PANE_dPicSd11b,      // 0xbb
        PANE_dPicSd12b,      // 0xbc
        PANE_dPicSd20b,      // 0xbd
        PANE_dPicSd21b,      // 0xbe
        PANE_dPicSd22b,      // 0xbf
        PANE_dPicSd30b,      // 0xc0
        PANE_dPicSd31b,      // 0xc1
        PANE_dPicSd32b,      // 0xc2
        PANE_dPicSd40b,      // 0xc3
        PANE_dPicSd41b,      // 0xc4
        PANE_dPicSd42b,      // 0xc5
        PANE_dPicSd50b,      // 0xc6
        PANE_dPicSd51b,      // 0xc7
        PANE_dPicSd52b,      // 0xc8
        PANE_dupFace10L,     // 0xc9
        PANE_dupFace11L,     // 0xca
        PANE_dupFace12L,     // 0xcb
        PANE_dupFace10R,     // 0xcc
        PANE_dupFace11R,     // 0xcd
        PANE_dupFace12R,     // 0xce
        PANE_dupPl10L,       // 0xcf
        PANE_dupPl11L,       // 0xd0
        PANE_dupPl12L,       // 0xd1
        PANE_dupPl10R,       // 0xd2
        PANE_dupPl11R,       // 0xd3
        PANE_dupPl12R,       // 0xd4
        PANE_dupStar10L,     // 0xd5
        PANE_dupStar11L,     // 0xd6
        PANE_dupStar12L,     // 0xd7
        PANE_dupStar10R,     // 0xd8
        PANE_dupStar11R,     // 0xd9
        PANE_dupStar12R,     // 0xda
        PANE_dNulCw0,        // 0xdb
        PANE_dNulCw1,        // 0xdc
        PANE_dNulCw2,        // 0xdd
        PANE_dNulCw3,        // 0xde
        PANE_dNulCw4,        // 0xdf
        PANE_dNulCw5,        // 0xe0
        PANE_dPicCw0,        // 0xe1
        PANE_dPicCw1,        // 0xe2
        PANE_dPicCw2,        // 0xe3
        PANE_dPicCw3,        // 0xe4
        PANE_dPicCw4,        // 0xe5
        PANE_dPicCw5,        // 0xe6
        PANE_NUM
    };

    const char *s_PaneName[PANE_NUM] = {
        "time_00",
        "P_Null",
        "Sco_A",
        "Sco_B",
        "Sco_C",
        "Sco_D",
        "M_ScoL_Null",
        "M_ScoR_Null",
        "P_Null_vis",
        "M_sco_Null",
        "time",
        "text_time",
        "text_time02",
        "Time_co",
        "time_t",
        "time_no",
        "t_waku",
        "ca",
        "ca_t",
        "F_Null",
        "f_01",
        "f_02",
        "P_1",
        "P_2",
        "P_3",
        "P_4",
        "p1_num",
        "p1_num_00",
        "p2_num",
        "p2_num_00",
        "p3_num",
        "p3_num_00",
        "p4_num",
        "p4_num_00",
        "p1_fe_icon",
        "p2_fe_icon",
        "p3_fe_icon",
        "p4_fe_icon",
        "play_1P",
        "play_p2",
        "play_p3",
        "play_p4",
        "sco_A",
        "sco_B",
        "sco_C",
        "sco_D",
        "scoL_t",
        "scoR_t",
        "L_Fa01",
        "L_Fa02",
        "L_Fa03",
        "L_Pa01",
        "L_Pa02",
        "L_Pa03",
        "R_Fa01",
        "R_Fa02",
        "R_Fa03",
        "R_Pa01",
        "R_Pa02",
        "R_Pa03",
        "L_Fb01",
        "L_Fb02",
        "L_Pb01",
        "L_Pb02",
        "R_Fb01",
        "R_Fb02",
        "R_Pb01",
        "R_Pb02",
        "1P",
        "2P",
        "3P",
        "4P",
        "com_01",
        "com_02",
        "com_03",
        "com_04",
        "com_05",
        "crown_Null",
        "crown",
        "ico_C",
        "ico_D",
        "f01_Null",
        "f02_Null",
        "Sco_D_Vis",
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
        "p1_Ca",
        "p2_Ca",
        "p3_Ca",
        "p4_Ca",
        "cs_ico",
        "dupFace00",
        "dupFace01",
        "dupFace02",
        "dupFace03",
        "dupFace04",
        "dupFace05",
        "dupStar00",
        "dupStar01",
        "dupStar02",
        "dupStar03",
        "dupStar04",
        "dupStar05",
        "dPicSd00",
        "dPicSd01",
        "dPicSd02",
        "dPicSd10",
        "dPicSd11",
        "dPicSd12",
        "dPicSd20",
        "dPicSd21",
        "dPicSd22",
        "dPicSd30",
        "dPicSd31",
        "dPicSd32",
        "dPicSd40",
        "dPicSd41",
        "dPicSd42",
        "dPicSd50",
        "dPicSd51",
        "dPicSd52",
        "dNulSd00",
        "dNulSd01",
        "dNulSd02",
        "dNulSd10",
        "dNulSd11",
        "dNulSd12",
        "dNulSd20",
        "dNulSd21",
        "dNulSd22",
        "dNulSd30",
        "dNulSd31",
        "dNulSd32",
        "dNulSd40",
        "dNulSd41",
        "dNulSd42",
        "dNulSd50",
        "dNulSd51",
        "dNulSd52",
        "dNulSd00a",
        "dNulSd01a",
        "dNulSd02a",
        "dNulSd10a",
        "dNulSd11a",
        "dNulSd12a",
        "dNulSd20a",
        "dNulSd21a",
        "dNulSd22a",
        "dNulSd30a",
        "dNulSd31a",
        "dNulSd32a",
        "dNulSd40a",
        "dNulSd41a",
        "dNulSd42a",
        "dNulSd50a",
        "dNulSd51a",
        "dNulSd52a",
        "dPicSd00a",
        "dPicSd01a",
        "dPicSd02a",
        "dPicSd10a",
        "dPicSd11a",
        "dPicSd12a",
        "dPicSd20a",
        "dPicSd21a",
        "dPicSd22a",
        "dPicSd30a",
        "dPicSd31a",
        "dPicSd32a",
        "dPicSd40a",
        "dPicSd41a",
        "dPicSd42a",
        "dPicSd50a",
        "dPicSd51a",
        "dPicSd52a",
        "dPicSd00b",
        "dPicSd01b",
        "dPicSd02b",
        "dPicSd10b",
        "dPicSd11b",
        "dPicSd12b",
        "dPicSd20b",
        "dPicSd21b",
        "dPicSd22b",
        "dPicSd30b",
        "dPicSd31b",
        "dPicSd32b",
        "dPicSd40b",
        "dPicSd41b",
        "dPicSd42b",
        "dPicSd50b",
        "dPicSd51b",
        "dPicSd52b",
        "dupFace10L",
        "dupFace11L",
        "dupFace12L",
        "dupFace10R",
        "dupFace11R",
        "dupFace12R",
        "dupPl10L",
        "dupPl11L",
        "dupPl12L",
        "dupPl10R",
        "dupPl11R",
        "dupPl12R",
        "dupStar10L",
        "dupStar11L",
        "dupStar12L",
        "dupStar10R",
        "dupStar11R",
        "dupStar12R",
        "dNulCw0",
        "dNulCw1",
        "dNulCw2",
        "dNulCw3",
        "dNulCw4",
        "dNulCw5",
        "dPicCw0",
        "dPicCw1",
        "dPicCw2",
        "dPicCw3",
        "dPicCw4",
        "dPicCw5",
    };

    // No pane.
    const unsigned char PANE_NONE = 0xFF;

    // The panes of one player (four players, then two more computer
    // players). Member names are guesses from the pane names.
    struct PlayerPane {
        unsigned char nScoreNull;       // played on when the score changes
        unsigned char nScore;           // the score below 100
        unsigned char nScore100;        // the score from 100
        unsigned char nFaceIcon;        // where the face picture goes
        unsigned char nLabelParent;     // where the "1P"/"COM" label goes
        unsigned char nStarParent;      // where the star goes
        unsigned char nFacePicture;     // name of the face picture made
        unsigned char nStar;            // the star of a star player
        unsigned char nCountUpNull[3];  // the pop-up scores
        unsigned char nCountUpText[3];
        unsigned char nCountUpFrame[3];
        unsigned char nCountUpPicA[3];
        unsigned char nCountUpPicB[3];
        unsigned char nPlusAnim;        // pop-up animation of a gain
        unsigned char nMinusAnim;       // pop-up animation of a loss
        unsigned char nCrownNull;       // the crown
        unsigned char nCrownPicture;
    };

    const PlayerPane s_PlayerPane[6] = {
        {
            PANE_P_1, PANE_p1_num, PANE_p1_num_00, PANE_p1_fe_icon, PANE_play_1P, PANE_p1_Ca, PANE_dupFace00,
            PANE_dupStar00,
            { PANE_dNulSd00, PANE_dNulSd01, PANE_dNulSd02 },
            { PANE_dPicSd00, PANE_dPicSd01, PANE_dPicSd02 },
            { PANE_dNulSd00a, PANE_dNulSd01a, PANE_dNulSd02a },
            { PANE_dPicSd00a, PANE_dPicSd01a, PANE_dPicSd02a },
            { PANE_dPicSd00b, PANE_dPicSd01b, PANE_dPicSd02b },
            0, 4, PANE_dNulCw0, PANE_dPicCw0,
        },
        {
            PANE_P_2, PANE_p2_num, PANE_p2_num_00, PANE_p2_fe_icon, PANE_play_p2, PANE_p2_Ca, PANE_dupFace01,
            PANE_dupStar01,
            { PANE_dNulSd10, PANE_dNulSd11, PANE_dNulSd12 },
            { PANE_dPicSd10, PANE_dPicSd11, PANE_dPicSd12 },
            { PANE_dNulSd10a, PANE_dNulSd11a, PANE_dNulSd12a },
            { PANE_dPicSd10a, PANE_dPicSd11a, PANE_dPicSd12a },
            { PANE_dPicSd10b, PANE_dPicSd11b, PANE_dPicSd12b },
            1, 4, PANE_dNulCw1, PANE_dPicCw1,
        },
        {
            PANE_P_3, PANE_p3_num, PANE_p3_num_00, PANE_p3_fe_icon, PANE_play_p3, PANE_p3_Ca, PANE_dupFace02,
            PANE_dupStar02,
            { PANE_dNulSd20, PANE_dNulSd21, PANE_dNulSd22 },
            { PANE_dPicSd20, PANE_dPicSd21, PANE_dPicSd22 },
            { PANE_dNulSd20a, PANE_dNulSd21a, PANE_dNulSd22a },
            { PANE_dPicSd20a, PANE_dPicSd21a, PANE_dPicSd22a },
            { PANE_dPicSd20b, PANE_dPicSd21b, PANE_dPicSd22b },
            2, 4, PANE_dNulCw2, PANE_dPicCw2,
        },
        {
            PANE_P_4, PANE_p4_num, PANE_p4_num_00, PANE_p4_fe_icon, PANE_play_p4, PANE_p4_Ca, PANE_dupFace03,
            PANE_dupStar03,
            { PANE_dNulSd30, PANE_dNulSd31, PANE_dNulSd32 },
            { PANE_dPicSd30, PANE_dPicSd31, PANE_dPicSd32 },
            { PANE_dNulSd30a, PANE_dNulSd31a, PANE_dNulSd32a },
            { PANE_dPicSd30a, PANE_dPicSd31a, PANE_dPicSd32a },
            { PANE_dPicSd30b, PANE_dPicSd31b, PANE_dPicSd32b },
            3, 4, PANE_dNulCw3, PANE_dPicCw3,
        },
        {
            PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_dupFace04, PANE_dupStar04,
            { PANE_dNulSd40, PANE_dNulSd41, PANE_dNulSd42 },
            { PANE_dPicSd40, PANE_dPicSd41, PANE_dPicSd42 },
            { PANE_dNulSd40a, PANE_dNulSd41a, PANE_dNulSd42a },
            { PANE_dPicSd40a, PANE_dPicSd41a, PANE_dPicSd42a },
            { PANE_dPicSd40b, PANE_dPicSd41b, PANE_dPicSd42b },
            4, 4, PANE_dNulCw4, PANE_dPicCw4,
        },
        {
            PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_NONE, PANE_dupFace05, PANE_dupStar05,
            { PANE_dNulSd50, PANE_dNulSd51, PANE_dNulSd52 },
            { PANE_dPicSd50, PANE_dPicSd51, PANE_dPicSd52 },
            { PANE_dNulSd50a, PANE_dNulSd51a, PANE_dNulSd52a },
            { PANE_dPicSd50a, PANE_dPicSd51a, PANE_dPicSd52a },
            { PANE_dPicSd50b, PANE_dPicSd51b, PANE_dPicSd52b },
            4, 4, PANE_dNulCw5, PANE_dPicCw5,
        },
    };

    // The fruit ball scores.
    const unsigned char s_BallScorePane[4] = {
        PANE_sco_A, PANE_sco_B, PANE_sco_C, PANE_sco_D,
    };

    // The team scores.
    const unsigned char s_TeamScorePane[2] = {
        PANE_scoL_t, PANE_scoR_t,
    };

    // The panes of one player in the team game (SetTeamFaceIcon), as in
    // PlayerPane.
    struct TeamPane {
        unsigned char nFaceIcon;
        unsigned char nLabelParent;
        unsigned char nStarParent;
        unsigned char nFacePicture;
        unsigned char nUnk4; // dupPl*, not used here
        unsigned char nStar;
    };

    // By layout, team and player number / 2. SetTeamFaceIcon uses layout 0
    // for more than four players and 2 otherwise; 1 and 3 are not used.
    const TeamPane s_TeamPane[4][2][3] = {
        {
            {
                { PANE_L_Fa01, PANE_L_Pa01, PANE_L_Ca_01, PANE_dupFace10L, PANE_dupPl10L, PANE_dupStar10L },
                { PANE_L_Fa02, PANE_L_Pa02, PANE_L_Ca_02, PANE_dupFace11L, PANE_dupPl11L, PANE_dupStar11L },
                { PANE_L_Fa03, PANE_L_Pa03, PANE_L_Ca_03, PANE_dupFace12L, PANE_dupPl12L, PANE_dupStar12L },
            },
            {
                { PANE_R_Fa01, PANE_R_Pa01, PANE_R_Ca_01, PANE_dupFace10R, PANE_dupPl10R, PANE_dupStar10R },
                { PANE_R_Fa02, PANE_R_Pa02, PANE_R_Ca_02, PANE_dupFace11R, PANE_dupPl11R, PANE_dupStar11R },
                { PANE_R_Fa03, PANE_R_Pa03, PANE_R_Ca_03, PANE_dupFace12R, PANE_dupPl12R, PANE_dupStar12R },
            },
        },
        {
            {
                { PANE_L_Fa01, PANE_L_Pa01, PANE_L_Ca_01, PANE_dupFace10L, PANE_dupPl10L, PANE_dupStar10L },
                { PANE_L_Fa02, PANE_L_Pa02, PANE_L_Ca_02, PANE_dupFace11L, PANE_dupPl11L, PANE_dupStar11L },
                { PANE_L_Fa03, PANE_L_Pa03, PANE_L_Ca_03, PANE_dupFace12L, PANE_dupPl12L, PANE_dupStar12L },
            },
            {
                { PANE_R_Fa01, PANE_R_Pa01, PANE_R_Ca_01, PANE_dupFace10R, PANE_dupPl10R, PANE_dupStar10R },
                { PANE_R_Fa02, PANE_R_Pa02, PANE_R_Ca_02, PANE_dupFace11R, PANE_dupPl11R, PANE_dupStar11R },
                { PANE_R_Fa03, PANE_R_Pa03, PANE_R_Ca_03, PANE_dupFace12R, PANE_dupPl12R, PANE_dupStar12R },
            },
        },
        {
            {
                { PANE_L_Fb01, PANE_L_Pb01, PANE_L_Ca_04, PANE_dupFace10L, PANE_dupPl10L, PANE_dupStar10L },
                { PANE_L_Fb02, PANE_L_Pb02, PANE_L_Ca_05, PANE_dupFace11L, PANE_dupPl11L, PANE_dupStar11L },
                { 0 },
            },
            {
                { PANE_R_Fb01, PANE_R_Pb01, PANE_R_Ca_04, PANE_dupFace10R, PANE_dupPl10R, PANE_dupStar10R },
                { PANE_R_Fb02, PANE_R_Pb02, PANE_R_Ca_05, PANE_dupFace11R, PANE_dupPl11R, PANE_dupStar11R },
                { 0 },
            },
        },
        {
            {
                { PANE_L_Fb02, PANE_L_Pb02, PANE_L_Ca_04, PANE_dupFace11L, PANE_dupPl11L, PANE_dupStar10L },
                { PANE_L_Fb02, PANE_L_Pb02, PANE_L_Ca_05, PANE_dupFace11L, PANE_dupPl11L, PANE_dupStar11L },
                { 0 },
            },
            {
                { PANE_R_Fb02, PANE_R_Pb02, PANE_R_Ca_04, PANE_dupFace11R, PANE_dupPl11R, PANE_dupStar10R },
                { PANE_R_Fb02, PANE_R_Pb02, PANE_R_Ca_05, PANE_dupFace11R, PANE_dupPl11R, PANE_dupStar11R },
                { 0 },
            },
        },
    };

    // The player labels: a human player's pad ("1P" to "4P"), then the
    // computer players.
    const unsigned char s_LabelPane[9] = {
        PANE_1P, PANE_2P, PANE_3P, PANE_4P, PANE_com_01, PANE_com_02, PANE_com_03, PANE_com_04, PANE_com_05,
    };

    // The computer players' labels, in the order they are given out.
    const unsigned char s_ComLabelPane[5] = {
        PANE_com_01, PANE_com_02, PANE_com_03, PANE_com_04, PANE_com_05,
    };
}

const unsigned char msw::CCockpitBasketHarapeko::s_CountUpPaneNum = 3;

namespace msw {
    namespace {

        // (name guessed; the class has no symbols) A display task that
        // moves a pop-up score's text pane over a point of the court, once.
        // Made by CCockpitBasketHarapeko::SetPointUpText.
        class CPointUpTextTask : public menu::CDispTaskLink {
            public:
                CPointUpTextTask()
                {
                    m_bEnd = 0;
                }

                virtual ~CPointUpTextTask();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long nView);
        };

        CPointUpTextTask::~CPointUpTextTask()
        {
        }

        void CPointUpTextTask::StatusCheck(CMenuPaneAccessor *)
        {
        }

        void CPointUpTextTask::Disp(CMenuPaneAccessor *pAccessor, long nView)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);

            if (pObj != 0) {
                bcs::FVEC3 screenPos;

                menu::GetWorldScreenPosition(m_Pos, nView, screenPos);
                // The text pane sits in a null pane; that one is moved.
                pObj->GetPane()->GetParent()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screenPos));
            }
            m_bEnd = 1;
        }
    }
}

namespace {

    // Events of SendEvent.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Values of m_nState.
    enum {
        STATE_START = 1,
        STATE_IN = 2,
        STATE_SHOW = 3,
        STATE_END = 4,
        STATE_END_WAIT = 5
    };

    // Kinds of the display tasks (plus the player number).
    enum {
        TASK_SCORE = 1000,
        TASK_SCORE_2 = 1010,
        TASK_POINT_UP = 2000,
        TASK_CROWN = 3000
    };

    // Animations of the score panes.
    enum {
        ANIM_SCORE_IN = 5,
        ANIM_SCORE_IN_WAIT = 6,
        ANIM_SCORE_OUT = 7,
        ANIM_SCORE_OUT_WAIT = 8
    };

    // The resource file of the cockpit that holds the layout.
    const long RES_FILE = 2;
}

msw::CCockpitBasketHarapeko::CCockpitBasketHarapeko(long nId, float)
    : CCockpitBasketLayoutBase(nId)
{
    m_nStep = 0;
    m_nState = 0;
    m_nPlayerNum = 0;
    m_nCountUpIdx[0] = 0;
    m_nCountUpIdx[1] = 0;
    m_nCountUpIdx[2] = 0;
    m_nCountUpIdx[3] = 0;
    m_fSortZ = 0.0f;
}

namespace {

    // The destructor stores both vtable pointers before it destroys the
    // members, which the compiler only does when the body calls something,
    // so the original called a function that is empty in this build (as in
    // game/basket/CCockpitBasketPause.cpp).
    inline void Nothing() {}
}

msw::CCockpitBasketHarapeko::~CCockpitBasketHarapeko()
{
    Nothing();
}

int msw::CCockpitBasketHarapeko::Set()
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

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -3;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

    if (pPaneCtrl == 0) {
        return -5;
    }

    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
    pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));
    pPaneCtrl->BuildLayout(0, s_LayoutRes.pLayout);
    pPaneCtrl->BuildAnim(0, s_LayoutRes.pAnim, 23);
    pPaneCtrl->SetSortZ(m_fSortZ);
    pPaneCtrl->SetVisible(0);
    SetPaneAccessor(pPaneCtrl, s_PaneName, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);

    m_nPlayerNum = CTournament::IsExtraMatchGame() ? SetTeamFaceIcon(pPaneCtrl) : SetFaceIcon(pPaneCtrl);

    CreateCountUp(pPaneCtrl);
    CreateCrownIcon(pPaneCtrl);
    m_EffectSet.Initialize();
    m_nStep = 1;
    m_nState = STATE_START;
    return 0;
}

void msw::CCockpitBasketHarapeko::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitBasketHarapeko::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    switch (m_nState) {
        case STATE_START:
            m_nState = STATE_IN;
            break;

        case STATE_IN:
            m_nState = STATE_SHOW;
            break;

        case STATE_END:
            m_nState = STATE_END_WAIT;
            break;

        case STATE_END_WAIT:
            // (meaning guessed) tells the owner the layout is done.
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitBasketHarapeko::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

long msw::CCockpitBasketHarapeko::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
        m_EffectSet.SetDispFlag(bVisible);
    }
    return -1;
}

long msw::CCockpitBasketHarapeko::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return 0;
    }

    // Never used.
    nw4r::math::VEC2 iconSize(32.0f, 32.0f);

    long nChrNum = CChrMan::GetChrCoreNum();
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    const PlayerPane *pPlayerPane = s_PlayerPane;
    const unsigned char *pComLabel = s_ComLabelPane;
    long nPlayerNum = 0;

    for (long i = 0; i < nChrNum; i++, pPlayerPane++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);

        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        nPlayerNum++;

        bcs::G2D::CPaneObj *pIconObj = m_pPaneAccessor->GetPaneObj(pPlayerPane->nFaceIcon);

        if (pIconObj == 0) {
            continue;
        }

        // The face picture, in the icon pane.
        const char *pPictureName = s_PaneName[pPlayerPane->nFacePicture];
        long nChrId = pChr->GetChrIdS();
        bcs::G2D::CPaneObj *pPictureObj = 0;
        const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

        if (pTexObj != 0) {
            bcs::G2D::CTexMap texMap(*pTexObj);
            nw4r::lyt::Pane *pPicturePane = bcs::G2D::CreatePicturePane(pPictureName, texMap);

            if (pPicturePane != 0) {
                pPictureObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPicturePane);
            }
        }

        if (pPictureObj != 0) {
            pPaneCtrl->AppendPaneObj(pPictureObj, *pIconObj);
            pPictureObj->GetPane()->SetSize(pIconObj->GetPane()->GetSize());
            pPictureObj->GetPane()->SetScale(menu::GetCockpitFaceIconScale(nChrId));
            pPictureObj->GetPane()->SetVisible(true);
        }

        // The label: the pad of a human player, else the next "COM".
        nw4r::lyt::Pane *pLabelParent = m_pPaneAccessor->GetPane(pPlayerPane->nLabelParent);
        nw4r::lyt::Pane *pLabel;

        if (pChr->IsPadCtrl()) {
            pLabel = m_pPaneAccessor->GetPane(s_LabelPane[pChr->GetPadNo()]);
        } else {
            pLabel = m_pPaneAccessor->GetPane(*pComLabel++);
        }

        if (pLabel != 0 && pLabelParent != 0) {
            bcs::G2D::SetPaneParent(pLabel, pLabelParent, 1, 0);
            pLabel->SetVisible(true);
            pLabel->SetTranslate(zero);
        }

        // The star of a star player: a copy of cs_ico.
        nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
        nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(pPlayerPane->nStarParent);

        if (pStarSrc == 0 || pStarParent == 0) {
            continue;
        }

        long nStar = pPlayerPane->nStar;
        nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

        if (pStar == 0) {
            pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, s_PaneName[nStar], s_LayoutRes.pLayout, 0);
            bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }

        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer() != 0);
        }
    }
    return nPlayerNum;
}

void msw::CCockpitBasketHarapeko::CreateCountUp(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const int animUse = 1;

    if (pPaneCtrl == 0) {
        return;
    }

    nw4r::lyt::Pane *pTextSrc = m_pPaneAccessor->GetPane(PANE_ca_t);
    nw4r::lyt::Pane *pFrameSrc = m_pPaneAccessor->GetPane(PANE_F_Null);
    nw4r::lyt::Pane *pPicASrc = m_pPaneAccessor->GetPane(PANE_f_01);
    nw4r::lyt::Pane *pNullSrc = m_pPaneAccessor->GetPane(PANE_ca);
    nw4r::lyt::Pane *pPicBSrc = m_pPaneAccessor->GetPane(PANE_f_02);

    if (pNullSrc == 0 || pTextSrc == 0 || pFrameSrc == 0 || pPicASrc == 0 || pPicBSrc == 0) {
        return;
    }

    // The pop-ups are placed in screen coordinates: the origin goes to the
    // top left corner.
    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    const int nWidth = bcs::CMainApp::g_Rmode.fbWidth;
    const int nHeight = bcs::CMainApp::g_Rmode.efbHeight;
    const nw4r::math::VEC3 corner(-(0.5f * nWidth), 0.5f * nHeight, 0.0f);

    pNullSrc->SetVisible(true);
    pNullSrc->SetTranslate(corner);
    pNullSrc->SetLocationAdjust(false);
    pTextSrc->SetVisible(false);

    const PlayerPane *pPlayerPane = s_PlayerPane;
    long nPlayerNum = m_nPlayerNum;

    for (long i = 0; i < nPlayerNum; i++, pPlayerPane++) {
        const unsigned char *pTextPane = pPlayerPane->nCountUpText;
        const unsigned char *pFramePane = pPlayerPane->nCountUpFrame;
        const unsigned char *pPicAPane = pPlayerPane->nCountUpPicA;
        const unsigned char *pPicBPane = pPlayerPane->nCountUpPicB;
        const unsigned char *pNullPane = pPlayerPane->nCountUpNull;

        for (long j = 0; j < s_CountUpPaneNum; j++) {
            bcs::G2D::CPaneObj *pNullObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pNullSrc, s_PaneName[pNullPane[j]], s_LayoutRes.pLayout, 0);

            if (pNullObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pNullObj, *pNullSrc);
            pNullObj->GetPane()->SetVisible(false);
            pNullObj->GetPane()->SetTranslate(zero);

            bcs::G2D::CPaneObj *pFrameObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pFrameSrc, s_PaneName[pFramePane[j]], s_LayoutRes.pLayout, animUse);

            if (pFrameObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pFrameObj, *pNullObj);
            pFrameObj->GetPane()->SetVisible(true);

            bcs::G2D::CPaneObj *pObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pPicASrc, s_PaneName[pPicAPane[j]], s_LayoutRes.pLayout, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pPicBSrc, s_PaneName[pPicBPane[j]],
                                              s_LayoutRes.pLayout, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pTextSrc, s_PaneName[pTextPane[j]],
                                              s_LayoutRes.pLayout, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pNullObj);
            pObj->GetPane()->SetVisible(true);
            pObj->GetPane()->SetTranslate(zero);
        }
    }
}

void msw::CCockpitBasketHarapeko::SetAnimScore(long nAnim)
{
    // static-order swap (notes/decisions.md): the game numbers `recursive`
    // before `reset` but stores it after.
    static const int reset = 1;
    static const int recursive = 0;

    m_pPaneAccessor->PlayAnimation(PANE_P_Null, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_time_00, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_Sco_A, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_Sco_B, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_Sco_C, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_Sco_D, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_M_ScoL_Null, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_M_ScoR_Null, nAnim, 1.0f, 0.0f, recursive, reset);
}

int msw::CCockpitBasketHarapeko::ScoreIn()
{
    SetAnimScore(ANIM_SCORE_IN);
    return 0;
}

int msw::CCockpitBasketHarapeko::ScoreInWait()
{
    SetAnimScore(ANIM_SCORE_IN_WAIT);
    return 0;
}

int msw::CCockpitBasketHarapeko::ScoreOut()
{
    SetAnimScore(ANIM_SCORE_OUT);
    return 0;
}

int msw::CCockpitBasketHarapeko::ScoreOutWait()
{
    SetAnimScore(ANIM_SCORE_OUT_WAIT);
    return 0;
}

long msw::CCockpitBasketHarapeko::SetScore(long nPlayer, long nScore, int bAnim, int bBig)
{
    // static-order swap (notes/decisions.md): the game declares these in the
    // opposite order (`recursive`, `reset`, `animId` for the first branch,
    // then `min`, `max`, `recursive`, `reset`) and stores them backwards.
    static const int reset2 = 1;
    static const int recursive2 = 1;
    static const long max = 999;
    static const long min = 0;
    static const long animId = 14;
    static const int reset = 1;
    static const int recursive = 1;

    const PlayerPane &rPane = s_PlayerPane[nPlayer];
    long nHidePane;
    long nShowPane;

    if (nScore >= 100) {
        nShowPane = rPane.nScore100;
        nHidePane = rPane.nScore;
    } else {
        nShowPane = rPane.nScore;
        nHidePane = rPane.nScore100;
    }
    long nNullPane = rPane.nScoreNull;

    menu::CDispTaskLink *pLink = m_DispTaskList.FindNextKindObj(nPlayer + TASK_SCORE, 0);

    if (pLink != 0) {
        m_DispTaskList.RemoveObject(pLink);
    }

    pLink = m_DispTaskList.FindNextKindObj(nPlayer + TASK_SCORE_2, 0);

    if (pLink != 0) {
        m_DispTaskList.RemoveObject(pLink);
    }

    if (bAnim == 0) {
        m_pPaneAccessor->SetTextBoxString(nShowPane, nScore, 0);
        m_pPaneAccessor->PlayAnimation(nNullPane, animId, 1.0f, 0.0f, recursive, reset);
        m_pPaneAccessor->SetPaneDisp(nHidePane, 0);
        m_pPaneAccessor->SetPaneDisp(nShowPane, 1);
        return nScore;
    }

    long nResult = nScore;

    if (nScore < min) {
        nResult = min;
    } else if (nScore > max) {
        nResult = max;
    }

    long nAnim = 16;

    if (bBig) {
        nAnim = 15;
    }

    m_pPaneAccessor->SetTextBoxString(nShowPane, nScore, 0);
    m_pPaneAccessor->SetPaneDisp(nHidePane, 0);
    m_pPaneAccessor->SetPaneDisp(nShowPane, 1);
    m_pPaneAccessor->PlayAnimation(nNullPane, nAnim, 1.0f, 0.0f, recursive2, reset2);
    return nResult;
}

long msw::CCockpitBasketHarapeko::SetTime(float fTime, int nArg, int bArg)
{
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fTime, nArg, PANE_text_time02, PANE_Time_co, PANE_text_time,
                             PANE_text_time02, bArg);

    long nSec = m_TimeCount.GetSec(fTime);

    if (nSec != m_TimeCount.GetShowSec()) {
        m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, 22);
        m_EffectSet.RemoveEffect(0, 0);

        // An effect on the frame for the last nine seconds.
        if (nSec >= 1 && nSec <= 9) {
            m_EffectSet.SetEffect(0, 0, 0x4D0, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_no);

        if (pObj != 0) {
            if (nSec <= 10 && !pObj->IsAnimPlay(20)) {
                pObj->PlayAnimation(20, 1.0f, 0.0f, 0, 1, 1);
            } else if (nSec > 10 && !pObj->IsAnimPlay(19)) {
                pObj->PlayAnimation(19, 1.0f, 0.0f, 0, 1, 1);
            }
        }
        m_TimeCount.SetShowSec(nSec);
    }
    return 0;
}

long msw::CCockpitBasketHarapeko::SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint)
{
    long nIdx = m_nCountUpIdx[nPlayer];
    long nTextPane = s_PlayerPane[nPlayer].nCountUpText[nIdx];
    wchar_t text[5] = L"+xxx";
    long nAnim;

    if (nPoint >= 0) {
        menu::GetNumericText(nPoint, L"+%d", text);
        nAnim = s_PlayerPane[nPlayer].nPlusAnim;
    } else {
        menu::GetNumericText(-nPoint, L"-%d", text);
        nAnim = s_PlayerPane[nPlayer].nMinusAnim;
    }
    m_pPaneAccessor->SetTextBoxString(nTextPane, text, wcslen(text));

    CPointUpTextTask *pTask = new CPointUpTextTask;

    pTask->SetPane(nTextPane);
    pTask->SetPos(rPos);
    pTask->SetKind(nPlayer + TASK_POINT_UP);
    m_DispTaskList.PushBack(pTask);

    // static-order swap, as in SetAnimScore.
    static const int reset = 1;
    static const int recursive = 1;
    long nNullPane = s_PlayerPane[nPlayer].nCountUpNull[nIdx];

    m_pPaneAccessor->PlayAnimation(nNullPane, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->SetPaneDisp(nNullPane, 1);

    nIdx++;
    if (nIdx >= 3) {
        nIdx = 0;
    }
    m_nCountUpIdx[nPlayer] = nIdx;
    return 0;
}

long msw::CCockpitBasketHarapeko::ClearPointUpText(long nPlayer)
{
    static const int recursive = 1;

    menu::CDispTaskLink *pLink = m_DispTaskList.FindNextKindObj(nPlayer + TASK_POINT_UP, 0);

    if (pLink != 0) {
        m_DispTaskList.RemoveObject(pLink);
    }

    for (long i = 0; i < 3; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(s_PlayerPane[nPlayer].nCountUpNull[i]);

        if (pObj != 0) {
            pObj->StopAllAnimation(recursive);
            pObj->GetPane()->SetVisible(false);
        }
    }
    return 0;
}

long msw::CCockpitBasketHarapeko::SetBallScore(long nBall, long nScore)
{
    long nPane = s_BallScorePane[nBall];
    wchar_t text[5] = L"+xxx";

    if (nScore >= 0) {
        menu::GetNumericText(nScore, L"+%d", text);
    } else {
        menu::GetNumericText(nScore, L"%d", text);
    }
    m_pPaneAccessor->SetTextBoxString(nPane, text, wcslen(text));
    return 0;
}

long msw::CCockpitBasketHarapeko::SetDispBallFruit4(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_Sco_D_Vis, bDisp);

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);

    if (bDisp) {
        // Each fruit icon in its own frame.
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_f01_Null);
        nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(PANE_ico_C);

        if (pParent != 0 && pIcon != 0) {
            bcs::G2D::SetPaneParent(pIcon, pParent, 1, 0);
            pIcon->SetVisible(true);
            pIcon->SetTranslate(zero);
        }

        pParent = m_pPaneAccessor->GetPane(PANE_f02_Null);
        pIcon = m_pPaneAccessor->GetPane(PANE_ico_D);

        if (pParent != 0 && pIcon != 0) {
            bcs::G2D::SetPaneParent(pIcon, pParent, 1, 0);
            pIcon->SetVisible(true);
            pIcon->SetTranslate(zero);
        }
    } else {
        // The fourth fruit's icon takes the third one's frame.
        nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_f01_Null);
        nw4r::lyt::Pane *pIcon = m_pPaneAccessor->GetPane(PANE_ico_D);

        if (pParent != 0 && pIcon != 0) {
            bcs::G2D::SetPaneParent(pIcon, pParent, 1, 0);
            pIcon->SetVisible(true);
            pIcon->SetTranslate(zero);
        }
        m_pPaneAccessor->SetPaneDisp(PANE_ico_C, bDisp);
    }
    return 0;
}

long msw::CCockpitBasketHarapeko::SetTeamScore(long nTeam, long nScore)
{
    m_pPaneAccessor->SetTextBoxString(s_TeamScorePane[nTeam], nScore >= 10000 ? 9999 : nScore, 0);
    return 0;
}

long msw::CCockpitBasketHarapeko::SetTeamFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const long iconPicNum = 9;

    if (pPaneCtrl == 0) {
        return 0;
    }

    long i;

    for (i = 0; i < iconPicNum; i++) {
        m_pPaneAccessor->SetPaneDisp(s_LabelPane[i], 0);
    }

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nChrNum = CChrMan::GetChrCoreNum();
    long nPlayerNum = 0;

    for (i = 0; i < nChrNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);

        if (pChr != 0 && !pChr->IsDisable()) {
            nPlayerNum++;
        }
    }

    const TeamPane(*pLayout)[3] = nPlayerNum <= 4 ? s_TeamPane[2] : s_TeamPane[0];
    const unsigned char *pComLabel = s_ComLabelPane;

    for (i = 0; i < nChrNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);

        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }

        const TeamPane *pTeamPane = &pLayout[pChr->GetTeamNo()][pChr->GetPlayerNo() >> 1];
        nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(pTeamPane->nFaceIcon);

        if (pFace == 0) {
            continue;
        }

        nw4r::lyt::Pane *pPicture = menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), pPaneCtrl, pFace,
                                                                   s_PaneName[pTeamPane->nFacePicture]);

        if (pPicture != 0) {
            bcs::G2D::SetPaneSizeFromParent(pPicture);
        }

        // The label, as in SetFaceIcon.
        nw4r::lyt::Pane *pLabelParent = m_pPaneAccessor->GetPane(pTeamPane->nLabelParent);
        nw4r::lyt::Pane *pLabel;

        if (pChr->IsPadCtrl()) {
            pLabel = m_pPaneAccessor->GetPane(s_LabelPane[pChr->GetPadNo()]);
        } else {
            pLabel = m_pPaneAccessor->GetPane(*pComLabel++);
        }

        if (pLabel != 0 && pLabelParent != 0) {
            bcs::G2D::SetPaneParent(pLabel, pLabelParent, 1, 0);
            pLabel->SetVisible(true);
            pLabel->SetTranslate(zero);
        }

        // The star, as in SetFaceIcon.
        nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
        nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(pTeamPane->nStarParent);

        if (pStarSrc == 0 || pStarParent == 0) {
            continue;
        }

        long nStar = pTeamPane->nStar;
        nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

        if (pStar == 0) {
            pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, s_PaneName[nStar], s_LayoutRes.pLayout, 0);
            bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }

        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer() != 0);
        }
    }
    return nPlayerNum;
}

void msw::CCockpitBasketHarapeko::CreateCrownIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    nw4r::lyt::Pane *pPictureSrc;
    static const int animUse = 1;

    if (pPaneCtrl == 0) {
        return;
    }

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    const nw4r::math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const nw4r::math::VEC2 scale(1.0f, 1.0f);
    nw4r::lyt::Pane *pNullSrc = m_pPaneAccessor->GetPane(PANE_crown_Null);

    if (pNullSrc == 0) {
        return;
    }

    // The crowns are placed in screen coordinates, as the pop-up scores.
    const int nWidth = bcs::CMainApp::g_Rmode.fbWidth;
    const int nHeight = bcs::CMainApp::g_Rmode.efbHeight;
    const nw4r::math::VEC3 corner(-(0.5f * nWidth), 0.5f * nHeight, 0.0f);

    pNullSrc->SetVisible(true);
    pNullSrc->SetAlpha(0xFF);
    pNullSrc->SetTranslate(corner);
    pNullSrc->SetRotate(rotate);
    pNullSrc->SetScale(scale);
    pNullSrc->SetLocationAdjust(false);

    pPictureSrc = m_pPaneAccessor->GetPane(PANE_crown);

    if (pPictureSrc == 0) {
        return;
    }
    pPictureSrc->SetVisible(false);

    const PlayerPane *pPlayerPane = s_PlayerPane;
    long nPlayerNum = m_nPlayerNum;

    for (long i = 0; i < nPlayerNum; i++, pPlayerPane++) {
        unsigned char nPicture = pPlayerPane->nCrownPicture;
        bcs::G2D::CPaneObj *pNullObj = bcs::G2D::DuplicatePaneObj(
            pPaneCtrl, pNullSrc, s_PaneName[pPlayerPane->nCrownNull], s_LayoutRes.pLayout, animUse);

        if (pNullObj == 0) {
            continue;
        }
        pPaneCtrl->AppendPaneObj(pNullObj, pNullSrc->GetName());
        pNullObj->GetPane()->SetVisible(false);
        pNullObj->GetPane()->SetTranslate(zero);
        pNullObj->GetPane()->SetAlpha(0xFF);

        bcs::G2D::CPaneObj *pPictureObj = bcs::G2D::DuplicatePaneObj(
            pPaneCtrl, pPictureSrc, s_PaneName[nPicture], s_LayoutRes.pLayout, 0);

        if (pPictureObj == 0) {
            continue;
        }
        pPaneCtrl->AppendPaneObj(pPictureObj, *pNullObj);
        pPictureObj->GetPane()->SetVisible(true);
        pPictureObj->GetPane()->SetTranslate(zero);
        pPictureObj->GetPane()->SetAlpha(0xFF);
    }
}

long msw::CCockpitBasketHarapeko::SetCrownIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, long nAction)
{
    // static-order swap (notes/decisions.md): the game declares these in the
    // opposite order and stores them backwards.
    static const long animId2 = 11;
    static const long animId = 10;
    static const long animId1 = 10;
    static const long animId0 = 9;
    static const int reset = 1;
    static const int recursive = 0;

    long nPane = s_PlayerPane[nPlayer].nCrownNull;
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

    if (pObj == 0) {
        return -1;
    }

    int bTask = 0;

    switch (nAction) {
        case 0:
            pObj->PlayAnimation(animId0, 1.0f, 0.0f, recursive, reset, 1);
            pObj->PushNextAnimation(animId1, 1.0f, 0.0f, recursive, reset);
            bTask = 1;
            break;

        case 1:
            pObj->PushNextAnimation(animId, 1.0f, 0.0f, recursive, reset);
            bTask = 1;
            break;

        case 2:
            if (pObj->IsAnimPlay(animId0) || pObj->IsAnimPlay(animId1)) {
                pObj->PlayAnimation(animId2, 1.0f, 0.0f, recursive, reset, 1);
                bTask = 1;
            }
            break;

        default:
            pObj->StopAllAnimation(1);
            pObj->GetPane()->SetVisible(false);
            break;
    }

    if (bTask == 1) {
        unsigned long nKind = nPlayer + TASK_CROWN;

        if (m_DispTaskList.FindNextKindObj(nKind, 0) == 0) {
            cockpit::CCharaIcon *pIcon = new cockpit::CCharaIcon;

            pIcon->SetKind(nKind);
            pIcon->SetPane(nPane);
            pIcon->SetPos(rPos);
            pIcon->SetUnk30(nArg);
            m_DispTaskList.PushBack(pIcon);
        }
    }
    return 0;
}

void msw::CCockpitBasketHarapeko::SetDispPlayerScore(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_P_Null_vis, bDisp);
}

void msw::CCockpitBasketHarapeko::SetDispTeamScore(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_M_sco_Null, bDisp);
}
