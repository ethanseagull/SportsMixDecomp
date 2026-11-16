// msw::CCockpitHockeyClash: the board of the hockey party game (layout
// HO_crash). The same layout code as BasketF's CCockpitBasketHarapeko
// (game/basket/CCockpitBasketHarapeko.cpp), with a countdown over a player
// about to leave and a "shake the Wii Remote" icon added.
#include "game/msw/CCockpitHockeyClash.hpp"

#include <wchar.h>
#include "bcs/CMainApp.hpp"
#include "bcs/BCS_Vec.hpp"
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
#include "game/msw/cockpit/CCharaIcon.hpp"
#include "game/msw/menu/CDispTaskList.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {

    // The layout.
    const char *s_LayoutName = "HO_crash.brlyt";

    // Its animations (8-aligned, after a padding word).
    const char *s_AnimName[24] = {
        "HO_crash_count_00.brlan",
        "HO_crash_count_01.brlan",
        "HO_crash_IN_00.brlan",
        "HO_crash_IN_01.brlan",
        "HO_crash_IN_02.brlan",
        "HO_crash_IN_03.brlan",
        "HO_crash_sd_00.brlan",
        "HO_crash_sd_01.brlan",
        "HO_crash_sd_02.brlan",
        "HO_crash_sd_03.brlan",
        "HO_crash_sd_04.brlan",
        "HO_crash_Time_00.brlan",
        "HO_crash_Time_01.brlan",
        "HO_crash_t_00.brlan",
        "HO_crash_t_01.brlan",
        "HO_crash_t_02.brlan",
        "HO_crash_con_00.brlan",
        "HO_crash_no_00.brlan",
        "HO_crash_no_01.brlan",
        "HO_crash_no_02.brlan",
        "HO_crash_Time_vis_00.brlan",
        "HO_crash_Time_vis_01.brlan",
        "HO_crash_Time_vis_02.brlan",
        "HO_crash_TC_00.brlan",
    };

    // Animations, by index into s_AnimName.
    enum {
        ANIM_count_00,     // 0x00
        ANIM_count_01,     // 0x01
        ANIM_IN_00,        // 0x02
        ANIM_IN_01,        // 0x03
        ANIM_IN_02,        // 0x04
        ANIM_IN_03,        // 0x05
        ANIM_sd_00,        // 0x06
        ANIM_sd_01,        // 0x07
        ANIM_sd_02,        // 0x08
        ANIM_sd_03,        // 0x09
        ANIM_sd_04,        // 0x0a
        ANIM_Time_00,      // 0x0b
        ANIM_Time_01,      // 0x0c
        ANIM_t_00,         // 0x0d
        ANIM_t_01,         // 0x0e
        ANIM_t_02,         // 0x0f
        ANIM_con_00,       // 0x10
        ANIM_no_00,        // 0x11
        ANIM_no_01,        // 0x12
        ANIM_no_02,        // 0x13
        ANIM_Time_vis_00,  // 0x14
        ANIM_Time_vis_01,  // 0x15
        ANIM_Time_vis_02,  // 0x16
        ANIM_TC_00,        // 0x17
        ANIM_NUM
    };

    // Panes, by index into s_PaneName.
    enum {
        PANE_time,            // 0x00
        PANE_cs,              // 0x01
        PANE_text_time,       // 0x02
        PANE_text_time02,     // 0x03
        PANE_Time_co,         // 0x04
        PANE_time_t,          // 0x05
        PANE_time_no,         // 0x06
        PANE_t_waku,          // 0x07
        PANE_time_m_01,       // 0x08
        PANE_time_m_02,       // 0x09
        PANE_count,           // 0x0a
        PANE_3,               // 0x0b
        PANE_2,               // 0x0c
        PANE_1,               // 0x0d
        PANE_go,              // 0x0e
        PANE_shoot,           // 0x0f
        PANE_sd,              // 0x10
        PANE_F_Null,          // 0x11
        PANE_f_01,            // 0x12
        PANE_f_02,            // 0x13
        PANE_B_t_Null,        // 0x14
        PANE_R_t_Null,        // 0x15
        PANE_G_t_Null,        // 0x16
        PANE_Y_t_Null,        // 0x17
        PANE_tn_t_b_n,        // 0x18
        PANE_tn_t_r_n,        // 0x19
        PANE_tn_t_g_n,        // 0x1a
        PANE_tn_t_y_n,        // 0x1b
        PANE_tn_t_b_n_2,      // 0x1c
        PANE_tn_t_r_n_2,      // 0x1d
        PANE_tn_t_g_n_2,      // 0x1e
        PANE_tn_t_y_n_2,      // 0x1f
        PANE_c_b,             // 0x20
        PANE_c_r,             // 0x21
        PANE_c_g,             // 0x22
        PANE_c_y,             // 0x23
        PANE_cont_B,          // 0x24
        PANE_cont_R,          // 0x25
        PANE_cont_G,          // 0x26
        PANE_cont_Y,          // 0x27
        PANE_con_B,           // 0x28
        PANE_con_R,           // 0x29
        PANE_con_G,           // 0x2a
        PANE_con_Y,           // 0x2b
        PANE_crown_Null,      // 0x2c
        PANE_crown,           // 0x2d
        PANE_1P,              // 0x2e
        PANE_2P,              // 0x2f
        PANE_3P,              // 0x30
        PANE_4P,              // 0x31
        PANE_com_01,          // 0x32
        PANE_com_02,          // 0x33
        PANE_com_03,          // 0x34
        PANE_play_1P,         // 0x35
        PANE_play_p2,         // 0x36
        PANE_play_p3,         // 0x37
        PANE_play_p4,         // 0x38
        PANE_p1_Ca,           // 0x39
        PANE_p2_Ca,           // 0x3a
        PANE_p3_Ca,           // 0x3b
        PANE_p4_Ca,           // 0x3c
        PANE_cs_ico,          // 0x3d
        PANE_dupFace00,       // 0x3e
        PANE_dupFace01,       // 0x3f
        PANE_dupFace02,       // 0x40
        PANE_dupFace03,       // 0x41
        PANE_dupStar00,       // 0x42
        PANE_dupStar01,       // 0x43
        PANE_dupStar02,       // 0x44
        PANE_dupStar03,       // 0x45
        PANE_dPicSd00,        // 0x46
        PANE_dPicSd01,        // 0x47
        PANE_dPicSd02,        // 0x48
        PANE_dPicSd10,        // 0x49
        PANE_dPicSd11,        // 0x4a
        PANE_dPicSd12,        // 0x4b
        PANE_dPicSd20,        // 0x4c
        PANE_dPicSd21,        // 0x4d
        PANE_dPicSd22,        // 0x4e
        PANE_dPicSd30,        // 0x4f
        PANE_dPicSd31,        // 0x50
        PANE_dPicSd32,        // 0x51
        PANE_dNulSd00,        // 0x52
        PANE_dNulSd01,        // 0x53
        PANE_dNulSd02,        // 0x54
        PANE_dNulSd10,        // 0x55
        PANE_dNulSd11,        // 0x56
        PANE_dNulSd12,        // 0x57
        PANE_dNulSd20,        // 0x58
        PANE_dNulSd21,        // 0x59
        PANE_dNulSd22,        // 0x5a
        PANE_dNulSd30,        // 0x5b
        PANE_dNulSd31,        // 0x5c
        PANE_dNulSd32,        // 0x5d
        PANE_dNulSd00a,       // 0x5e
        PANE_dNulSd01a,       // 0x5f
        PANE_dNulSd02a,       // 0x60
        PANE_dNulSd10a,       // 0x61
        PANE_dNulSd11a,       // 0x62
        PANE_dNulSd12a,       // 0x63
        PANE_dNulSd20a,       // 0x64
        PANE_dNulSd21a,       // 0x65
        PANE_dNulSd22a,       // 0x66
        PANE_dNulSd30a,       // 0x67
        PANE_dNulSd31a,       // 0x68
        PANE_dNulSd32a,       // 0x69
        PANE_dPicSd00a,       // 0x6a
        PANE_dPicSd01a,       // 0x6b
        PANE_dPicSd02a,       // 0x6c
        PANE_dPicSd10a,       // 0x6d
        PANE_dPicSd11a,       // 0x6e
        PANE_dPicSd12a,       // 0x6f
        PANE_dPicSd20a,       // 0x70
        PANE_dPicSd21a,       // 0x71
        PANE_dPicSd22a,       // 0x72
        PANE_dPicSd30a,       // 0x73
        PANE_dPicSd31a,       // 0x74
        PANE_dPicSd32a,       // 0x75
        PANE_dPicSd00b,       // 0x76
        PANE_dPicSd01b,       // 0x77
        PANE_dPicSd02b,       // 0x78
        PANE_dPicSd10b,       // 0x79
        PANE_dPicSd11b,       // 0x7a
        PANE_dPicSd12b,       // 0x7b
        PANE_dPicSd20b,       // 0x7c
        PANE_dPicSd21b,       // 0x7d
        PANE_dPicSd22b,       // 0x7e
        PANE_dPicSd30b,       // 0x7f
        PANE_dPicSd31b,       // 0x80
        PANE_dPicSd32b,       // 0x81
        PANE_dPicCt01,        // 0x82
        PANE_dPicCt02,        // 0x83
        PANE_dPicCt03,        // 0x84
        PANE_dPicCt04,        // 0x85
        PANE_dPicCt05,        // 0x86
        PANE_dPicCt0G,        // 0x87
        PANE_dPicCt11,        // 0x88
        PANE_dPicCt12,        // 0x89
        PANE_dPicCt13,        // 0x8a
        PANE_dPicCt14,        // 0x8b
        PANE_dPicCt15,        // 0x8c
        PANE_dPicCt1G,        // 0x8d
        PANE_dPicCt21,        // 0x8e
        PANE_dPicCt22,        // 0x8f
        PANE_dPicCt23,        // 0x90
        PANE_dPicCt24,        // 0x91
        PANE_dPicCt25,        // 0x92
        PANE_dPicCt2G,        // 0x93
        PANE_dPicCt31,        // 0x94
        PANE_dPicCt32,        // 0x95
        PANE_dPicCt33,        // 0x96
        PANE_dPicCt34,        // 0x97
        PANE_dPicCt35,        // 0x98
        PANE_dPicCt3G,        // 0x99
        PANE_dNulCt0,         // 0x9a
        PANE_dNulCt1,         // 0x9b
        PANE_dNulCt2,         // 0x9c
        PANE_dNulCt3,         // 0x9d
        PANE_dNulCw0,         // 0x9e
        PANE_dNulCw1,         // 0x9f
        PANE_dNulCw2,         // 0xa0
        PANE_dNulCw3,         // 0xa1
        PANE_dPicCw0,         // 0xa2
        PANE_dPicCw1,         // 0xa3
        PANE_dPicCw2,         // 0xa4
        PANE_dPicCw3,         // 0xa5
        PANE_NUM
    };

    const char *s_PaneName[PANE_NUM] = {
        "time",
        "cs",
        "text_time",
        "text_time02",
        "Time_co",
        "time_t",
        "time_no",
        "t_waku",
        "time_m_01",
        "time_m_02",
        "count",
        "3",
        "2",
        "1",
        "go",
        "shoot",
        "sd",
        "F_Null",
        "f_01",
        "f_02",
        "B_t_Null",
        "R_t_Null",
        "G_t_Null",
        "Y_t_Null",
        "tn_t_b_n",
        "tn_t_r_n",
        "tn_t_g_n",
        "tn_t_y_n",
        "tn_t_b_n_2",
        "tn_t_r_n_2",
        "tn_t_g_n_2",
        "tn_t_y_n_2",
        "c_b",
        "c_r",
        "c_g",
        "c_y",
        "cont_B",
        "cont_R",
        "cont_G",
        "cont_Y",
        "con_B",
        "con_R",
        "con_G",
        "con_Y",
        "crown_Null",
        "crown",
        "1P",
        "2P",
        "3P",
        "4P",
        "com_01",
        "com_02",
        "com_03",
        "play_1P",
        "play_p2",
        "play_p3",
        "play_p4",
        "p1_Ca",
        "p2_Ca",
        "p3_Ca",
        "p4_Ca",
        "cs_ico",
        "dupFace00",
        "dupFace01",
        "dupFace02",
        "dupFace03",
        "dupStar00",
        "dupStar01",
        "dupStar02",
        "dupStar03",
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
        "dPicCt01",
        "dPicCt02",
        "dPicCt03",
        "dPicCt04",
        "dPicCt05",
        "dPicCt0G",
        "dPicCt11",
        "dPicCt12",
        "dPicCt13",
        "dPicCt14",
        "dPicCt15",
        "dPicCt1G",
        "dPicCt21",
        "dPicCt22",
        "dPicCt23",
        "dPicCt24",
        "dPicCt25",
        "dPicCt2G",
        "dPicCt31",
        "dPicCt32",
        "dPicCt33",
        "dPicCt34",
        "dPicCt35",
        "dPicCt3G",
        "dNulCt0",
        "dNulCt1",
        "dNulCt2",
        "dNulCt3",
        "dNulCw0",
        "dNulCw1",
        "dNulCw2",
        "dNulCw3",
        "dPicCw0",
        "dPicCw1",
        "dPicCw2",
        "dPicCw3",
    };

    // The panes of one player. Member names are guesses from the pane names.
    struct PlayerPane {
        unsigned char nScoreNull;         // played on when the score changes
        unsigned char nScore;             // the score below 100
        unsigned char nScore100;          // the score from 100
        unsigned char nFaceIcon;          // where the face picture goes
        unsigned char nLabelParent;       // where the "1P"/"COM" label goes
        unsigned char nStarParent;        // where the star goes
        unsigned char nFacePicture;       // name of the face picture made
        unsigned char nStar;              // the star of a star player
        unsigned char nCountUpNull[3];    // the pop-up points
        unsigned char nCountUpText[3];
        unsigned char nCountUpFrame[3];
        unsigned char nCountUpPicA[3];
        unsigned char nCountUpPicB[3];
        unsigned char nPlusAnim;          // pop-up animation of a gain
        unsigned char nMinusAnim;         // pop-up animation of a loss
        unsigned char nCountDownNull;     // the countdown ("3", "2", "1", go)
        unsigned char nCountDownPic[4];
        unsigned char nShakeIcon;         // the "shake" icon
        unsigned char nUnk1F;             // con_*, not used here
        unsigned char nCrownNull;         // the crown
        unsigned char nCrownPicture;
    };

    // Plus and minus animations: ANIM_sd_00 + player, ANIM_sd_04.
    const PlayerPane s_PlayerPane[4] = {
        {
            PANE_B_t_Null, PANE_tn_t_b_n, PANE_tn_t_b_n_2, PANE_c_b, PANE_play_1P, PANE_p1_Ca, PANE_dupFace00, PANE_dupStar00,
            { PANE_dNulSd00, PANE_dNulSd01, PANE_dNulSd02 },
            { PANE_dPicSd00, PANE_dPicSd01, PANE_dPicSd02 },
            { PANE_dNulSd00a, PANE_dNulSd01a, PANE_dNulSd02a },
            { PANE_dPicSd00a, PANE_dPicSd01a, PANE_dPicSd02a },
            { PANE_dPicSd00b, PANE_dPicSd01b, PANE_dPicSd02b },
            6, 10, PANE_dNulCt0, { PANE_dPicCt01, PANE_dPicCt02, PANE_dPicCt03, PANE_dPicCt0G },
            PANE_cont_B, PANE_con_B, PANE_dNulCw0, PANE_dPicCw0,
        },
        {
            PANE_R_t_Null, PANE_tn_t_r_n, PANE_tn_t_r_n_2, PANE_c_r, PANE_play_p2, PANE_p2_Ca, PANE_dupFace01, PANE_dupStar01,
            { PANE_dNulSd10, PANE_dNulSd11, PANE_dNulSd12 },
            { PANE_dPicSd10, PANE_dPicSd11, PANE_dPicSd12 },
            { PANE_dNulSd10a, PANE_dNulSd11a, PANE_dNulSd12a },
            { PANE_dPicSd10a, PANE_dPicSd11a, PANE_dPicSd12a },
            { PANE_dPicSd10b, PANE_dPicSd11b, PANE_dPicSd12b },
            7, 10, PANE_dNulCt1, { PANE_dPicCt11, PANE_dPicCt12, PANE_dPicCt13, PANE_dPicCt1G },
            PANE_cont_R, PANE_con_R, PANE_dNulCw1, PANE_dPicCw1,
        },
        {
            PANE_G_t_Null, PANE_tn_t_g_n, PANE_tn_t_g_n_2, PANE_c_g, PANE_play_p3, PANE_p3_Ca, PANE_dupFace02, PANE_dupStar02,
            { PANE_dNulSd20, PANE_dNulSd21, PANE_dNulSd22 },
            { PANE_dPicSd20, PANE_dPicSd21, PANE_dPicSd22 },
            { PANE_dNulSd20a, PANE_dNulSd21a, PANE_dNulSd22a },
            { PANE_dPicSd20a, PANE_dPicSd21a, PANE_dPicSd22a },
            { PANE_dPicSd20b, PANE_dPicSd21b, PANE_dPicSd22b },
            8, 10, PANE_dNulCt2, { PANE_dPicCt21, PANE_dPicCt22, PANE_dPicCt23, PANE_dPicCt2G },
            PANE_cont_G, PANE_con_G, PANE_dNulCw2, PANE_dPicCw2,
        },
        {
            PANE_Y_t_Null, PANE_tn_t_y_n, PANE_tn_t_y_n_2, PANE_c_y, PANE_play_p4, PANE_p4_Ca, PANE_dupFace03, PANE_dupStar03,
            { PANE_dNulSd30, PANE_dNulSd31, PANE_dNulSd32 },
            { PANE_dPicSd30, PANE_dPicSd31, PANE_dPicSd32 },
            { PANE_dNulSd30a, PANE_dNulSd31a, PANE_dNulSd32a },
            { PANE_dPicSd30a, PANE_dPicSd31a, PANE_dPicSd32a },
            { PANE_dPicSd30b, PANE_dPicSd31b, PANE_dPicSd32b },
            9, 10, PANE_dNulCt3, { PANE_dPicCt31, PANE_dPicCt32, PANE_dPicCt33, PANE_dPicCt3G },
            PANE_cont_Y, PANE_con_Y, PANE_dNulCw3, PANE_dPicCw3,
        },
    };

    // The countdown: the null pane copied per player and the four pictures
    // copied into it. Not const in the game (they are in .data).
    unsigned char s_CountDownNullPane = PANE_count;
    unsigned char s_CountDownPicPane[4] = {
        PANE_3, PANE_2, PANE_1, PANE_go,
    };

    // The player labels: a human player's pad ("1P" to "4P"), then the
    // computer players.
    unsigned char s_LabelPane[4] = {
        PANE_1P, PANE_2P, PANE_3P, PANE_4P,
    };
    unsigned char s_ComLabelPane[3] = {
        PANE_com_01, PANE_com_02, PANE_com_03,
    };

    // The message shown on the countdown's last picture: its file and label.
    const char *s_GoMessageFile = "Text_Party_Cockpit";
    const char *s_GoMessageLabel = "Party_Cockpit_07";
}

const unsigned char msw::CCockpitHockeyClash::s_CountUpPaneNum = 3;

namespace msw {
    namespace {

        // (name guessed; the class has no symbols) A display task that
        // moves a pop-up text pane over a point of the court, once. Made by
        // CCockpitHockeyClash::SetPointUpText.
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

        // (name guessed) A display task that keeps a countdown over a
        // character until the animation m_Unk1C of pane m_Unk18 ends. Made
        // by CCockpitHockeyClash::SetLeaveCountText.
        class CLeaveCountTask : public menu::CDispTaskLink {
            public:
                CLeaveCountTask()
                {
                    m_bEnd = 0;
                    m_nChr = 0;
                }

                virtual ~CLeaveCountTask();
                virtual void StatusCheck(CMenuPaneAccessor *pAccessor);
                virtual void Disp(CMenuPaneAccessor *pAccessor, long nView);

                void SetAnim(long nAnim) { m_Unk1C = nAnim; }
                void SetChr(long nChr) { m_nChr = nChr; }

            private:
                long m_nChr; // 0x30, the character followed
        };

        CLeaveCountTask::~CLeaveCountTask()
        {
        }

        void CLeaveCountTask::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
            if (pAccessor->IsAnimEnd(m_Unk18, m_Unk1C)) {
                m_bEnd = 1;
            }
        }

        void CLeaveCountTask::Disp(CMenuPaneAccessor *pAccessor, long nView)
        {
            bcs::G2D::CPaneObj *pObj = pAccessor->GetPaneObj(m_Unk18);

            if (pObj == 0) {
                m_bEnd = 1;
                return;
            }

            CChrCore *pChr = CChrMan::GetChrCore(m_nChr);

            if (pChr == 0 || pChr->IsDisable()) {
                m_bEnd = 1;
                return;
            }

            // m_Pos is an offset from the character.
            bcs::FVEC3 pos;
            bcs::FVEC3 screenPos;

            bcs::FVEC3Add(&pos, &pChr->GetPos(), &m_Pos);
            menu::GetWorldScreenPosition(pos, nView, screenPos);
            pObj->GetPane()->GetParent()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(screenPos));
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
        TASK_SHAKE = 2000,
        TASK_CROWN = 3000,
        TASK_POINT_UP = 4000
    };

    // The resource file of the cockpit that holds the layout.
    const long RES_FILE = 2;

    // The destructor stores both vtable pointers before it destroys the
    // members, which the compiler only does when the body calls something,
    // so the original called a function that is empty in this build (as in
    // game/basket/CCockpitBasketHarapeko.cpp).
    inline void Nothing() {}
}

msw::CCockpitHockeyClash::CCockpitHockeyClash(long nId, float fSortZ)
    : CCockpitHockeyLayoutBase(nId), m_fLytSortZ(fSortZ)
{
    m_nStep = 0;
    m_nState = 0;
    m_nPlayerNum = 0;
    m_nCountUpIdx[0] = 0;
    m_nCountUpIdx[1] = 0;
    m_nCountUpIdx[2] = 0;
    m_nCountUpIdx[3] = 0;
}

msw::CCockpitHockeyClash::~CCockpitHockeyClash()
{
    Nothing();
}

int msw::CCockpitHockeyClash::Set()
{
    // Never read here (a mission version of the board is not built).
    static const int isMission = 0;

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
    pPaneCtrl->BuildLayout(0, s_LayoutName);
    pPaneCtrl->BuildAnim(0, s_AnimName, ANIM_NUM);
    pPaneCtrl->SetSortZ(m_fLytSortZ);
    pPaneCtrl->SetVisible(0);
    SetPaneAccessor(pPaneCtrl, s_PaneName, PANE_NUM);
    SetTextBoxFromMessage(pPaneCtrl);

    m_nPlayerNum = SetFaceIcon(pPaneCtrl);

    CreateCountUp(pPaneCtrl);
    CreateCountDown(pPaneCtrl);
    CreateCrownIcon(pPaneCtrl);
    InitPaneAnim(0);
    m_EffectSet.Initialize();
    m_nStep = 1;
    m_nState = STATE_START;
    return 0;
}

void msw::CCockpitHockeyClash::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_EffectSet.Finalize();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitHockeyClash::Main()
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

void msw::CCockpitHockeyClash::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

long msw::CCockpitHockeyClash::SendEvent(long nEvent, void *pParam)
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

long msw::CCockpitHockeyClash::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return 0;
    }

    // Never used.
    nw4r::math::VEC2 iconSize(32.0f, 32.0f);

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nChrNum = CChrMan::GetChrCoreNum();
    const PlayerPane *pPlayerPane = s_PlayerPane;
    const unsigned char *pComLabel = s_ComLabelPane;
    long nPlayerNum = 0;

    for (long i = 0; i < nChrNum; i++, pPlayerPane++) {
        bcs::G2D::CPaneObj *pIconObj = m_pPaneAccessor->GetPaneObj(pPlayerPane->nFaceIcon);

        if (pIconObj == 0) {
            continue;
        }

        CChrCore *pChr = CChrMan::GetChrCore(i);

        if (pChr == 0 || pChr->IsDisable()) {
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

        // The label: the pad of a human player, else the next "COM". The
        // first character has none.
        long nLabelParent = pPlayerPane->nLabelParent;

        if (i > 0) {
            long nLabel;

            if (pChr->IsPadCtrl()) {
                nLabel = s_LabelPane[pChr->GetPadNo()];
            } else {
                nLabel = *pComLabel++;
            }

            nw4r::lyt::Pane *pLabel = m_pPaneAccessor->GetPane(nLabel);
            nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(nLabelParent);

            if (pLabel != 0 && pParent != 0) {
                bcs::G2D::SetPaneParent(pLabel, pParent, 1, 0);
                pLabel->SetTranslate(zero);
                pLabel->SetVisible(true);
            }
        }

        // The star of a star player: a copy of cs_ico.
        nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
        nw4r::lyt::Pane *pStarParent = m_pPaneAccessor->GetPane(pPlayerPane->nStarParent);

        if (pStarSrc != 0 && pStarParent != 0) {
            long nStar = pPlayerPane->nStar;
            nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

            if (pStar == 0) {
                pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, s_PaneName[nStar], s_LayoutName, 0);
                bcs::G2D::SetPaneParent(pStar, pStarParent, 1, 0);
                bcs::G2D::SetPaneSizeFromParent(pStar);
                pStar->SetTranslate(zero);
            }

            if (pStar != 0) {
                pStar->SetVisible(pChr->IsStarPlayer() != 0);
            }
        }
        nPlayerNum++;
    }
    return nPlayerNum;
}

void msw::CCockpitHockeyClash::CreateCountUp(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const int animUse = 1;

    if (pPaneCtrl == 0) {
        return;
    }

    nw4r::lyt::Pane *pNullSrc = m_pPaneAccessor->GetPane(PANE_shoot);
    nw4r::lyt::Pane *pTextSrc = m_pPaneAccessor->GetPane(PANE_sd);
    nw4r::lyt::Pane *pFrameSrc = m_pPaneAccessor->GetPane(PANE_F_Null);
    nw4r::lyt::Pane *pPicASrc = m_pPaneAccessor->GetPane(PANE_f_01);
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
        const unsigned char *pNullPane = pPlayerPane->nCountUpNull;
        const unsigned char *pTextPane = pPlayerPane->nCountUpText;
        const unsigned char *pFramePane = pPlayerPane->nCountUpFrame;
        const unsigned char *pPicAPane = pPlayerPane->nCountUpPicA;
        const unsigned char *pPicBPane = pPlayerPane->nCountUpPicB;

        // (as in the game) one pop-up fewer than there are players, not
        // s_CountUpPaneNum.
        for (long j = 0; j < nPlayerNum - 1; j++) {
            bcs::G2D::CPaneObj *pNullObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pNullSrc, s_PaneName[pNullPane[j]], s_LayoutName, 0);

            if (pNullObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pNullObj, *pNullSrc);
            pNullObj->GetPane()->SetVisible(false);
            pNullObj->GetPane()->SetTranslate(zero);

            bcs::G2D::CPaneObj *pFrameObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pFrameSrc, s_PaneName[pFramePane[j]], s_LayoutName, animUse);

            if (pFrameObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pFrameObj, *pNullObj);
            pFrameObj->GetPane()->SetVisible(true);

            bcs::G2D::CPaneObj *pObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pPicASrc, s_PaneName[pPicAPane[j]], s_LayoutName, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pPicBSrc, s_PaneName[pPicBPane[j]],
                                              s_LayoutName, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pFrameObj);
            pObj->GetPane()->SetVisible(true);

            pObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, pTextSrc, s_PaneName[pTextPane[j]],
                                              s_LayoutName, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pNullObj);
            pObj->GetPane()->SetVisible(true);
            pObj->GetPane()->SetTranslate(zero);
        }
    }
}

void msw::CCockpitHockeyClash::CreateCountDown(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const int animUse = 1;
    static const long dupPicCountNum = 4;
    static const long CountPicPaneIdTableLength = 4;

    if (pPaneCtrl == 0) {
        return;
    }

    nw4r::lyt::Pane *pNullSrc = m_pPaneAccessor->GetPane(s_CountDownNullPane);

    if (pNullSrc == 0) {
        return;
    }

    // In screen coordinates, as the pop-up points.
    const int nWidth = bcs::CMainApp::g_Rmode.fbWidth;
    const int nHeight = bcs::CMainApp::g_Rmode.efbHeight;
    const nw4r::math::VEC3 corner(-(0.5f * nWidth), 0.5f * nHeight, 0.0f);

    pNullSrc->SetVisible(true);
    pNullSrc->SetTranslate(corner);
    pNullSrc->SetLocationAdjust(false);

    nw4r::lyt::Pane *pPicSrc[CountPicPaneIdTableLength];
    long i;

    for (i = 0; i < CountPicPaneIdTableLength; i++) {
        pPicSrc[i] = m_pPaneAccessor->GetPane(s_CountDownPicPane[i]);

        if (pPicSrc[i] != 0) {
            pPicSrc[i]->SetVisible(false);
        }
    }

    const nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nPlayerNum = m_nPlayerNum;

    for (i = 0; i < nPlayerNum; i++) {
        bcs::G2D::CPaneObj *pNullObj = bcs::G2D::DuplicatePaneObj(
            pPaneCtrl, pNullSrc, s_PaneName[s_PlayerPane[i].nCountDownNull], s_LayoutName, 0);

        if (pNullObj == 0) {
            continue;
        }
        pPaneCtrl->AppendPaneObj(pNullObj, pNullSrc->GetName());
        pNullObj->GetPane()->SetVisible(false);
        pNullObj->GetPane()->SetTranslate(zero);

        const wchar_t *pText = menu::CMessage::s_Instance.GetTextByLabel(
            menu::CMessage::s_Instance.GetFileIndex(s_GoMessageFile), s_GoMessageLabel);

        const unsigned char *pPic = s_PlayerPane[i].nCountDownPic;

        for (long j = 0; j < dupPicCountNum; j++) {
            if (pPicSrc[j] == 0) {
                continue;
            }

            bcs::G2D::CPaneObj *pObj = bcs::G2D::DuplicatePaneObj(
                pPaneCtrl, pPicSrc[j], s_PaneName[pPic[j]], s_LayoutName, animUse);

            if (pObj == 0) {
                continue;
            }
            pPaneCtrl->AppendPaneObj(pObj, *pNullObj);
            pObj->GetPane()->SetVisible(true);
            pObj->GetPane()->SetTranslate(zero);

            // The last picture is a text box: the "go" text of the messages.
            if (s_CountDownPicPane[j] == PANE_go) {
                m_pPaneAccessor->SetTextBoxString(pPic[j], pText, wcslen(pText));
            }
        }
    }
}

void msw::CCockpitHockeyClash::CreateCrownIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
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

    // The crowns are placed in screen coordinates, as the pop-up points.
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
            pPaneCtrl, pNullSrc, s_PaneName[pPlayerPane->nCrownNull], s_LayoutName, animUse);

        if (pNullObj == 0) {
            continue;
        }
        pPaneCtrl->AppendPaneObj(pNullObj, pNullSrc->GetName());
        pNullObj->GetPane()->SetVisible(false);
        pNullObj->GetPane()->SetTranslate(zero);
        pNullObj->GetPane()->SetAlpha(0xFF);

        bcs::G2D::CPaneObj *pPictureObj = bcs::G2D::DuplicatePaneObj(
            pPaneCtrl, pPictureSrc, s_PaneName[nPicture], s_LayoutName, 0);

        if (pPictureObj == 0) {
            continue;
        }
        pPaneCtrl->AppendPaneObj(pPictureObj, *pNullObj);
        pPictureObj->GetPane()->SetVisible(true);
        pPictureObj->GetPane()->SetTranslate(zero);
        pPictureObj->GetPane()->SetAlpha(0xFF);
    }
}

void msw::CCockpitHockeyClash::InitPaneAnim(int bAlt)
{
    // static-order swap (notes/decisions.md): the game numbers `recursive`
    // before `reset` but stores it after.
    static const int reset = 1;
    static const int recursive = 0;

    const PlayerPane *pPlayerPane = s_PlayerPane;
    long nPlayerNum = m_nPlayerNum;

    for (long i = 0; i < nPlayerNum; i++, pPlayerPane++) {
        m_pPaneAccessor->PlayAnimation(pPlayerPane->nScoreNull, ANIM_t_00, 1.0f, 0.0f, recursive, reset);
    }

    // The time and its frame go into one of two null panes.
    nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(bAlt ? PANE_time_m_01 : PANE_time_m_02);

    if (pParent == 0) {
        return;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(PANE_t_waku);

    if (pPane != 0) {
        bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
    }

    pPane = m_pPaneAccessor->GetPane(PANE_Time_co);

    if (pPane != 0) {
        bcs::G2D::SetPaneParent(pPane, pParent, 1, 0);
    }
}

void msw::CCockpitHockeyClash::SetAnimScore(long nAnim)
{
    // static-order swap, as in InitPaneAnim.
    static const int reset = 1;
    static const int recursive = 0;

    m_pPaneAccessor->PlayAnimation(PANE_cs, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(PANE_time, nAnim, 1.0f, 0.0f, recursive, reset);
}

int msw::CCockpitHockeyClash::ScoreIn()
{
    SetAnimScore(ANIM_IN_00);
    return 0;
}

int msw::CCockpitHockeyClash::ScoreInWait()
{
    SetAnimScore(ANIM_IN_01);
    return 0;
}

int msw::CCockpitHockeyClash::ScoreOut()
{
    SetAnimScore(ANIM_IN_02);
    return 0;
}

int msw::CCockpitHockeyClash::ScoreOutWait()
{
    SetAnimScore(ANIM_IN_03);
    return 0;
}

long msw::CCockpitHockeyClash::SetScore(long nPlayer, long nScore, int bAnim, int bBig)
{
    // static-order swap (notes/decisions.md): the game declares these in the
    // opposite order (`recursive`, `reset`, `animId` for the first branch,
    // then `min`, `max`, `recursive`, `reset`) and stores them backwards.
    static const int reset2 = 1;
    static const int recursive2 = 1;
    static const long max = 999;
    static const long min = 0;
    static const long animId = ANIM_t_00;
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

    if (nScore < min) {
        nScore = min;
    } else if (nScore > max) {
        nScore = max;
    }

    long nAnim = ANIM_t_02;

    if (bBig) {
        nAnim = ANIM_t_01;
    }

    m_pPaneAccessor->SetTextBoxString(nShowPane, nScore, 0);
    m_pPaneAccessor->SetPaneDisp(nHidePane, 0);
    m_pPaneAccessor->SetPaneDisp(nShowPane, 1);
    m_pPaneAccessor->PlayAnimation(nNullPane, nAnim, 1.0f, 0.0f, recursive2, reset2);
    return nScore;
}

long msw::CCockpitHockeyClash::SetTimeValue(float fFrame, int nArg, int nMode)
{
    m_TimeCount.SetTimeValue(m_pPaneAccessor, fFrame, nArg, ANIM_Time_00, ANIM_Time_01, PANE_text_time,
                             PANE_text_time02, nMode);

    long nSec = m_TimeCount.GetSec(fFrame);

    if (nSec != m_TimeCount.GetShowSec()) {
        m_TimeCount.SetCountDown(m_pPaneAccessor, nSec, PANE_Time_co, PANE_time_t, ANIM_TC_00);
        m_EffectSet.RemoveEffect(0, 0);

        // An effect on the frame for the last nine seconds.
        if (nSec >= 1 && nSec <= 9) {
            m_EffectSet.SetEffect(0, 0, 0x4D0, m_pPaneAccessor->GetPaneObj(PANE_t_waku));
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(PANE_time_no);

        if (pObj != 0) {
            if (nSec <= 10 && !pObj->IsAnimPlay(ANIM_Time_vis_01)) {
                pObj->PlayAnimation(ANIM_Time_vis_01, 1.2f, 0.0f, 0, 1, 1);
            } else if (nSec > 10 && !pObj->IsAnimPlay(ANIM_Time_vis_00)) {
                pObj->PlayAnimation(ANIM_Time_vis_00, 1.0f, 0.0f, 0, 1, 1);
            }
        }
        m_TimeCount.SetShowSec(nSec);
    }
    return 0;
}

long msw::CCockpitHockeyClash::SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint)
{
    long nIdx = m_nCountUpIdx[nPlayer];
    wchar_t text[5] = L"+xxx";
    long nTextPane = s_PlayerPane[nPlayer].nCountUpText[nIdx];
    long nFramePane = s_PlayerPane[nPlayer].nCountUpFrame[nIdx];
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

    // static-order swap, as in InitPaneAnim.
    static const int reset = 1;
    static const int recursive = 1;

    m_pPaneAccessor->PlayAnimation(nTextPane, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->PlayAnimation(nFramePane, nAnim, 1.0f, 0.0f, recursive, reset);
    m_pPaneAccessor->SetPaneDisp(s_PlayerPane[nPlayer].nCountUpNull[nIdx], 1);

    nIdx++;
    if (nIdx >= 3) {
        nIdx = 0;
    }
    m_nCountUpIdx[nPlayer] = nIdx;
    return 0;
}

long msw::CCockpitHockeyClash::ClearPointUpText(long nPlayer)
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

long msw::CCockpitHockeyClash::SetLeaveCountText(long nPlayer, const bcs::FVEC3 &rPos, long nChr, int bHide)
{
    // static-order swap (notes/decisions.md): stored backwards.
    static const long animId = ANIM_count_00;
    static const int reset = 1;
    static const int recursive = 1;

    long nNullPane = s_PlayerPane[nPlayer].nCountDownNull;
    long nPicPane = s_PlayerPane[nPlayer].nCountDownPic[0];

    if (bHide == 1) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nNullPane);

        if (pObj != 0) {
            pObj->StopAllAnimation(1);
            pObj->GetPane()->SetVisible(false);
        }
    } else {
        CLeaveCountTask *pTask = new CLeaveCountTask;

        pTask->SetPane(nPicPane);
        pTask->SetAnim(animId);
        pTask->SetPos(rPos);
        pTask->SetChr(nChr);
        m_DispTaskList.PushBack(pTask);

        m_pPaneAccessor->PlayAnimation(nNullPane, animId, 1.0f, 0.0f, recursive, reset);
        m_pPaneAccessor->SetPaneDisp(nNullPane, 1);
    }
    return 0;
}

long msw::CCockpitHockeyClash::SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bHide)
{
    // static-order swap (notes/decisions.md): stored backwards.
    static const long animId = ANIM_con_00;
    static const int reset = 1;
    static const int recursive = 1;

    long nPane = s_PlayerPane[nPlayer].nShakeIcon;
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);

    if (pObj == 0) {
        return -1;
    }

    if (bHide == 1) {
        pObj->StopAllAnimation(1);
        pObj->GetPane()->SetVisible(false);
    } else {
        unsigned long nKind = nPlayer + TASK_SHAKE;

        if (m_DispTaskList.FindNextKindObj(nKind, 0) == 0) {
            cockpit::CCharaIcon *pIcon = new cockpit::CCharaIcon;

            pIcon->SetKind(nKind);
            pIcon->SetPane(nPane);
            pIcon->SetPos(rPos);
            pIcon->SetUnk30(nArg);
            m_DispTaskList.PushBack(pIcon);

            // Into the screen-coordinate null pane of the pop-up points.
            nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_shoot);

            if (pParent != 0) {
                bcs::G2D::SetPaneParent(pObj->GetPane(), pParent, 1, 0);
            }
        }
        pObj->PlayAnimation(animId, 1.0f, 0.0f, recursive, reset, 1);
        pObj->GetPane()->SetVisible(true);
    }
    return 0;
}

long msw::CCockpitHockeyClash::SetCrownIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, long nAction)
{
    // static-order swap (notes/decisions.md): the game declares these in the
    // opposite order and stores them backwards.
    static const long animId2 = ANIM_no_02;
    static const long animId = ANIM_no_01;
    static const long animId1 = ANIM_no_01;
    static const long animId0 = ANIM_no_00;
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
            pObj->CancelNextAnimationAll(1);
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
