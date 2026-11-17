// msw::CCockpitDodgeLifeGauge: the life gauges of the dodgeball game. Each
// character of each team has a gauge with its face, a player or COM mark,
// a star for a star player and its life as up to six marks. The layout
// is picked by the characters' maximum life and the number of characters
// per team.
#include "game/msw/CCockpitDodgeLifeGauge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/ResourceAccessor.hpp"

// 60 degrees in radians; unused here, as in the other DodgeF files that
// have it (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // The resource file of the cockpit that holds the layouts.
    const long RES_FILE = 3;

    // The tables below are by the characters' maximum life (1 to 6), then
    // the number of characters per team (1 to 3).
    const long MAX_LIFE_NUM = 6;
    const long CHR_NUM = 3;
    const long SIDE_NUM = 2;

    const char *const sLayoutNames[MAX_LIFE_NUM][CHR_NUM] = {
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" },
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" },
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" },
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" },
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" },
        { "DO_cockpit_1_1.brlyt", "DO_cockpit_1_2.brlyt", "DO_cockpit_1_3.brlyt" }
    };

    // The animations of each layout. The one-character layout has no
    // S_sp animations: Life_00 stands in for them.
    const char *sAnimNames1[] = {
        "DO_cockpit_1_1_Gauge_00.brlan",
        "DO_cockpit_1_1_Gauge_01.brlan",
        "DO_cockpit_1_1_Gauge_02.brlan",
        "DO_cockpit_1_1_Gauge_03.brlan",
        "DO_cockpit_1_1_Hit_00.brlan",
        "DO_cockpit_1_1_Hit_01.brlan",
        "DO_cockpit_1_1_Hit_02.brlan",
        "DO_cockpit_1_1_Hit_03.brlan",
        "DO_cockpit_1_1_Hit_04.brlan",
        "DO_cockpit_1_1_Hit_05.brlan",
        "DO_cockpit_1_1_Hit_06.brlan",
        "DO_cockpit_1_1_Hit_07.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Vis_00.brlan",
        "DO_cockpit_1_1_Vis_01.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan",
        "DO_cockpit_1_1_Life_00.brlan"
    };

    const char *sAnimNames2[] = {
        "DO_cockpit_1_2_Gauge_00.brlan",
        "DO_cockpit_1_2_Gauge_01.brlan",
        "DO_cockpit_1_2_Gauge_02.brlan",
        "DO_cockpit_1_2_Gauge_03.brlan",
        "DO_cockpit_1_2_Hit_00.brlan",
        "DO_cockpit_1_2_Hit_01.brlan",
        "DO_cockpit_1_2_Hit_02.brlan",
        "DO_cockpit_1_2_Hit_03.brlan",
        "DO_cockpit_1_2_Hit_04.brlan",
        "DO_cockpit_1_2_Hit_05.brlan",
        "DO_cockpit_1_2_Hit_06.brlan",
        "DO_cockpit_1_2_Hit_07.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Life_00.brlan",
        "DO_cockpit_1_2_Vis_00.brlan",
        "DO_cockpit_1_2_Vis_01.brlan",
        "DO_cockpit_1_2_S_sp_00.brlan",
        "DO_cockpit_1_2_S_sp_01.brlan",
        "DO_cockpit_1_2_S_sp_02.brlan",
        "DO_cockpit_1_2_S_sp_03.brlan"
    };

    const char *sAnimNames3[] = {
        "DO_cockpit_1_3_Gauge_00.brlan",
        "DO_cockpit_1_3_Gauge_01.brlan",
        "DO_cockpit_1_3_Gauge_02.brlan",
        "DO_cockpit_1_3_Gauge_03.brlan",
        "DO_cockpit_1_3_Hit_00.brlan",
        "DO_cockpit_1_3_Hit_01.brlan",
        "DO_cockpit_1_3_Hit_02.brlan",
        "DO_cockpit_1_3_Hit_03.brlan",
        "DO_cockpit_1_3_Hit_04.brlan",
        "DO_cockpit_1_3_Hit_05.brlan",
        "DO_cockpit_1_3_Hit_06.brlan",
        "DO_cockpit_1_3_Hit_07.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Life_00.brlan",
        "DO_cockpit_1_3_Vis_00.brlan",
        "DO_cockpit_1_3_Vis_01.brlan",
        "DO_cockpit_1_3_S_sp_00.brlan",
        "DO_cockpit_1_3_S_sp_01.brlan",
        "DO_cockpit_1_3_S_sp_02.brlan",
        "DO_cockpit_1_3_S_sp_03.brlan"
    };

    const char **sAnimNames[MAX_LIFE_NUM][CHR_NUM] = {
        { sAnimNames1, sAnimNames2, sAnimNames3 },
        { sAnimNames1, sAnimNames2, sAnimNames3 },
        { sAnimNames1, sAnimNames2, sAnimNames3 },
        { sAnimNames1, sAnimNames2, sAnimNames3 },
        { sAnimNames1, sAnimNames2, sAnimNames3 },
        { sAnimNames1, sAnimNames2, sAnimNames3 }
    };

    const unsigned char sAnimNum[MAX_LIFE_NUM][CHR_NUM] = {
        { 25, 25, 25 },
        { 25, 25, 25 },
        { 25, 25, 25 },
        { 25, 25, 25 },
        { 25, 25, 25 },
        { 25, 25, 25 }
    };

    // Animations (indexes into sAnimNames1 to 3).
    enum {
        ANIM_GAUGE_IN = 0,
        ANIM_GAUGE_IN_WAIT = 1,
        ANIM_GAUGE_OUT = 2,
        ANIM_GAUGE_OUT_WAIT = 3,
        ANIM_HIT_00 = 4,
        ANIM_HIT_01 = 5,
        ANIM_HIT_02 = 6,
        ANIM_HIT_04 = 8,
        ANIM_HIT_05 = 9,
        ANIM_LIFE = 12,
        ANIM_MARK_BREAK = 17,
        ANIM_MARK_GUARD = 18,
        ANIM_VIS_01 = 20,
        ANIM_SP_00 = 21,
        ANIM_SP_01 = 22,
        ANIM_SP_03 = 24
    };

    const char *sPaneNames[] = {
        "line_L",
        "line_R",
        "L_life_ga1_A",
        "L_life1A",
        "L_life1A_fe_Null",
        "L_life1A_cha",
        "R_life_ga1_A",
        "R_life1A",
        "R_life1A_fe_Null",
        "R_life1A_cha",
        "L_life_ga2_A",
        "L_life2A",
        "L_life2A_fe_Null",
        "L_life2A_cha",
        "L_life_ga2_B",
        "L_life2B",
        "L_life2B_fe_Null",
        "L_life2B_cha",
        "R_life_ga2_A",
        "R_life2A",
        "R_life2A_fe_Null",
        "R_life2A_cha",
        "R_life_ga2_B",
        "R_life2B",
        "R_life2B_fe_Null",
        "R_life2B_cha",
        "L_life_ga3_A",
        "L_life3A",
        "L_life3A_fe_Null",
        "L_life3A_cha",
        "L_life_ga3_B",
        "L_life3B",
        "L_life3B_fe_Null",
        "L_life3B_cha",
        "L_life_ga3_C",
        "L_life3C",
        "L_life3C_fe_Null",
        "L_life3C_cha",
        "R_life_ga3_A",
        "R_life3A",
        "R_life3A_fe_Null",
        "R_life3A_cha",
        "R_life_ga3_B",
        "R_life3B",
        "R_life3B_fe_Null",
        "R_life3B_cha",
        "R_life_ga3_C",
        "R_life3C",
        "R_life3C_fe_Null",
        "R_life3C_cha",
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
        "L_life_ga3",
        "L_life_ga2",
        "L_life_ga1",
        "R_life_ga3",
        "R_life_ga2",
        "R_life_ga1",
        "L_life1A_life_1",
        "L_life1A_life_2",
        "L_life1A_life_3",
        "L_life1A_life_4",
        "L_life1A_life_5",
        "L_life1A_life_6",
        "R_life1A_life_1",
        "R_life1A_life_2",
        "R_life1A_life_3",
        "R_life1A_life_4",
        "R_life1A_life_5",
        "R_life1A_life_6",
        "L_life2A_life_1",
        "L_life2A_life_2",
        "L_life2A_life_3",
        "L_life2A_life_4",
        "L_life2A_life_5",
        "L_life2A_life_6",
        "L_life2B_life_1",
        "L_life2B_life_2",
        "L_life2B_life_3",
        "L_life2B_life_4",
        "L_life2B_life_5",
        "L_life2B_life_6",
        "R_life2A_life_1",
        "R_life2A_life_2",
        "R_life2A_life_3",
        "R_life2A_life_4",
        "R_life2A_life_5",
        "R_life2A_life_6",
        "R_life2B_life_1",
        "R_life2B_life_2",
        "R_life2B_life_3",
        "R_life2B_life_4",
        "R_life2B_life_5",
        "R_life2B_life_6",
        "L_life3A_life_1",
        "L_life3A_life_2",
        "L_life3A_life_3",
        "L_life3A_life_4",
        "L_life3A_life_5",
        "L_life3A_life_6",
        "L_life3B_life_1",
        "L_life3B_life_2",
        "L_life3B_life_3",
        "L_life3B_life_4",
        "L_life3B_life_5",
        "L_life3B_life_6",
        "L_life3C_life_1",
        "L_life3C_life_2",
        "L_life3C_life_3",
        "L_life3C_life_4",
        "L_life3C_life_5",
        "L_life3C_life_6",
        "R_life3A_life_1",
        "R_life3A_life_2",
        "R_life3A_life_3",
        "R_life3A_life_4",
        "R_life3A_life_5",
        "R_life3A_life_6",
        "R_life3B_life_1",
        "R_life3B_life_2",
        "R_life3B_life_3",
        "R_life3B_life_4",
        "R_life3B_life_5",
        "R_life3B_life_6",
        "R_life3C_life_1",
        "R_life3C_life_2",
        "R_life3C_life_3",
        "R_life3C_life_4",
        "R_life3C_life_5",
        "R_life3C_life_6",
        "L_life1A_life",
        "L_life1A_life_2",
        "R_life1A_life",
        "R_life1A_life_2",
        "L_life2A_life",
        "L_life2A_life_2",
        "L_life2B_life",
        "L_life2B_life_2",
        "R_life2A_life",
        "R_life2A_life_2",
        "R_life2B_life",
        "R_life2B_life_2",
        "L_life3A_life",
        "L_life3A_life_2",
        "L_life3B_life",
        "L_life3B_life_2",
        "L_life3C_life",
        "L_life3C_life_2",
        "R_life3A_life",
        "R_life3A_life_2",
        "R_life3B_life",
        "R_life3B_life_2",
        "R_life3C_life",
        "R_life3C_life_2",
        "L_life1A_num",
        "R_life1A_num",
        "L_life2A_num",
        "L_life2B_num",
        "R_life2A_num",
        "R_life2B_num",
        "L_life3A_num",
        "L_life3B_num",
        "L_life3C_num",
        "R_life3A_num",
        "R_life3B_num",
        "R_life3C_num",
        "R_life3A_life1p",
        "R_life3A_life2p",
        "R_life3A_life3p",
        "R_life3A_life4p",
        "R_life3A_life5p",
        "R_life3A_life6p",
        "R_life3B_life1p",
        "R_life3B_life2p",
        "R_life3B_life3p",
        "R_life3B_life4p",
        "R_life3B_life5p",
        "R_life3B_life6p",
        "R_life3C_life1p",
        "R_life3C_life2p",
        "R_life3C_life3p",
        "R_life3C_life4p",
        "R_life3C_life5p",
        "R_life3C_life6p",
        "R_life2A_life1p",
        "R_life2A_life2p",
        "R_life2A_life3p",
        "R_life2A_life4p",
        "R_life2A_life5p",
        "R_life2A_life6p",
        "R_life2B_life1p",
        "R_life2B_life2p",
        "R_life2B_life3p",
        "R_life2B_life4p",
        "R_life2B_life5p",
        "R_life2B_life6p",
        "R_life1A_life1p",
        "R_life1A_life2p",
        "R_life1A_life3p",
        "R_life1A_life4p",
        "R_life1A_life5p",
        "R_life1A_life6p",
        "L_life3A_life1p",
        "L_life3A_life2p",
        "L_life3A_life3p",
        "L_life3A_life4p",
        "L_life3A_life5p",
        "L_life3A_life6p",
        "L_life3B_life1p",
        "L_life3B_life2p",
        "L_life3B_life3p",
        "L_life3B_life4p",
        "L_life3B_life5p",
        "L_life3B_life6p",
        "L_life3C_life1p",
        "L_life3C_life2p",
        "L_life3C_life3p",
        "L_life3C_life4p",
        "L_life3C_life5p",
        "L_life3C_life6p",
        "L_life2A_life1p",
        "L_life2A_life2p",
        "L_life2A_life3p",
        "L_life2A_life4p",
        "L_life2A_life5p",
        "L_life2A_life6p",
        "L_life2B_life1p",
        "L_life2B_life2p",
        "L_life2B_life3p",
        "L_life2B_life4p",
        "L_life2B_life5p",
        "L_life2B_life6p",
        "L_life1A_life1p",
        "L_life1A_life2p",
        "L_life1A_life3p",
        "L_life1A_life4p",
        "L_life1A_life5p",
        "L_life1A_life6p",
        "life_Null",
        "C_life",
        "C_life1",
        "C_life3",
        "C_life5",
        "C_life2",
        "C_life4",
        "C_life6",
        "P_safe",
        "L_Pa_3A",
        "L_Pa_3B",
        "L_Pa_3C",
        "R_Pa_3A",
        "R_Pa_3B",
        "R_Pa_3C",
        "L_Pa_2A",
        "L_Pa_2B",
        "R_Pa_2A",
        "R_Pa_2B",
        "L_Pa_1A",
        "R_Pa_1A",
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
        "te_HP_L01",
        "te_HP_L02",
        "te_HP_L03",
        "te_HP_R01",
        "te_HP_R02",
        "te_HP_R03",
        "IcFace00",
        "IcFace01",
        "IcFace02",
        "IcFace03",
        "IcFace04",
        "IcFace05",
        "IcPNo00",
        "IcPNo01",
        "IcPNo02",
        "IcPNo03",
        "IcPNo04",
        "IcPNo05",
        "IcStar00",
        "IcStar01",
        "IcStar02",
        "IcStar03",
        "IcStar04",
        "IcStar05"
    };

    // Number of panes in sPaneNames.
    const long PANE_NUM = 309;

    // Panes (indexes into sPaneNames) used by name in the code.
    enum {
        PANE_C_life = 249,
        PANE_P_safe = 256,
        PANE_1P = 269,
        PANE_com = 273,
        PANE_cs_ico = 284
    };

    // Effects.
    enum {
        EFFECT_MARK_BREAK_5 = 0x681, // maximum life 5 or 6
        EFFECT_MARK_BREAK_3 = 0x682, // maximum life 3 or 4
        EFFECT_MARK_BREAK_1 = 0x683  // maximum life 1 or 2
    };

    // The two teams' gauges (they slide as the gauge part).
    const unsigned short sLinePane[SIDE_NUM] = { 0, 1 };

    // The panes of one character's gauge (indexes into sPaneNames).
    struct ChrPane {
        unsigned short nGauge;    // "L_life_ga1_A": the hit animations
        unsigned short nFace;     // "L_life1A_fe_Null": parent of the face
        unsigned short nLife;     // "L_life1A"
        unsigned short nGuard;    // "L_safe_ga1_A"
        unsigned short nChara;    // "L_life1A_cha" (not read)
        unsigned short nBar;      // "L_life1A_life": the first life bar
        unsigned short nBar2;     // "L_life1A_life_2": the second life bar
        unsigned short nMarkBase; // "L_Pa_1A": parent of the player mark
        unsigned short nTelop;    // "te_HP_L01" (not read)
        unsigned short nStarBase; // "L_Ca_01": parent of the star
    };

    // By the number of characters per team, team colour and character.
    const ChrPane sChrPane[CHR_NUM][SIDE_NUM][CHR_NUM] = {
        {
            {
                { 0x02, 0x04, 0x03, 0x32, 0x05, 0x8C, 0x8D, 0x10B, 0x11D, 0x112 }
            },
            {
                { 0x06, 0x08, 0x07, 0x33, 0x09, 0x8E, 0x8F, 0x10C, 0x120, 0x112 }
            }
        },
        {
            {
                { 0x0A, 0x0C, 0x0B, 0x34, 0x0D, 0x90, 0x91, 0x107, 0x11D, 0x115 },
                { 0x0E, 0x10, 0x0F, 0x35, 0x11, 0x92, 0x93, 0x108, 0x11E, 0x116 }
            },
            {
                { 0x12, 0x14, 0x13, 0x36, 0x15, 0x94, 0x95, 0x109, 0x120, 0x11A },
                { 0x16, 0x18, 0x17, 0x37, 0x19, 0x96, 0x97, 0x10A, 0x121, 0x11B }
            }
        },
        {
            {
                { 0x1A, 0x1C, 0x1B, 0x38, 0x1D, 0x98, 0x99, 0x101, 0x11F, 0x112 },
                { 0x1E, 0x20, 0x1F, 0x39, 0x21, 0x9A, 0x9B, 0x102, 0x11E, 0x113 },
                { 0x22, 0x24, 0x23, 0x3A, 0x25, 0x9C, 0x9D, 0x103, 0x11D, 0x114 }
            },
            {
                { 0x26, 0x28, 0x27, 0x3B, 0x29, 0x9E, 0x9F, 0x104, 0x120, 0x117 },
                { 0x2A, 0x2C, 0x2B, 0x3C, 0x2D, 0xA0, 0xA1, 0x105, 0x121, 0x118 },
                { 0x2E, 0x30, 0x2F, 0x3D, 0x31, 0xA2, 0xA3, 0x106, 0x122, 0x119 }
            }
        }
    };

    // The icon panes made for each character, by team colour and
    // character: the face ("IcFace00"), the player mark ("IcPNo00") and the
    // star ("IcStar00").
    const unsigned short sIconPane[SIDE_NUM][CHR_NUM][3] = {
        { { 0x123, 0x129, 0x12F }, { 0x124, 0x12A, 0x130 }, { 0x125, 0x12B, 0x131 } },
        { { 0x126, 0x12C, 0x132 }, { 0x127, 0x12D, 0x133 }, { 0x128, 0x12E, 0x134 } }
    };

    // The life marks of one character's gauge.
    struct LifePane {
        unsigned short nMark[6];  // "L_life1A_life_1" to 6, from the last
        unsigned short nMarkP[6]; // "L_life1A_life1p" to 6p, where a mark breaks
        unsigned short nNum;      // "L_life1A_num" (not read)
        unsigned short nCenter;   // "C_life1" to 6 (not read)
        unsigned short nUnk[14];  // (all zero)
    };

    // By the number of characters per team, team colour and character.
    const LifePane sLifePane[CHR_NUM][SIDE_NUM][CHR_NUM] = {
        {
            {
                { { 0x44, 0x45, 0x46, 0x47, 0x48, 0x49 }, { 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7 }, 0xA4, 0xFA }
            },
            {
                { { 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F }, { 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3 }, 0xA5, 0xFD }
            }
        },
        {
            {
                { { 0x50, 0x51, 0x52, 0x53, 0x54, 0x55 }, { 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB }, 0xA6, 0xFA },
                { { 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B }, { 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1 }, 0xA7, 0xFB }
            },
            {
                { { 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61 }, { 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7 }, 0xA8, 0xFD },
                { { 0x62, 0x63, 0x64, 0x65, 0x66, 0x67 }, { 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD }, 0xA9, 0xFE }
            }
        },
        {
            {
                { { 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D }, { 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9 }, 0xAA, 0xFA },
                { { 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73 }, { 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF }, 0xAB, 0xFB },
                { { 0x74, 0x75, 0x76, 0x77, 0x78, 0x79 }, { 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5 }, 0xAC, 0xFC }
            },
            {
                { { 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F }, { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 }, 0xAD, 0xFD },
                { { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85 }, { 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB }, 0xAE, 0xFE },
                { { 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B }, { 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1 }, 0xAF, 0xFF }
            }
        }
    };

    // Six life points per mark colour ("bar").
    const long MARK_NUM = 6;

    // The mark animations of each bar: full, breaking, empty, after the
    // break, charged. One table for a maximum life up to 6, one past it
    // (the same values). A bar of 2 reads past the first table's end into
    // the second, as the original does.
    const unsigned short sMarkAnim[2][5] = {
        { 12, 13, 14, 12, 15 },
        { 12, 13, 14, 12, 15 }
    };
    const unsigned short sMarkAnimLong[2][5] = {
        { 12, 13, 14, 12, 15 },
        { 12, 13, 14, 12, 15 }
    };

    // The guard marks, moved to the gauge they show on.
    const long GUARD_MARK_NUM = 1;
    const unsigned short sGuardPane[GUARD_MARK_NUM] = { PANE_P_safe };

    // "1P" to "4P", by controlling player.
    const unsigned short sPlayerMarkPane[4] = { PANE_1P, PANE_1P + 1, PANE_1P + 2, PANE_1P + 3 };

    // The display task kind of each character's gauge (CDispTaskList kind
    // minus 1000), by team colour and character.
    const long sChrTaskNo[SIDE_NUM][CHR_NUM] = {
        { 0, 2, 4 },
        { 1, 3, 5 }
    };

    // A display task of the gauge (kind 1000 plus sChrTaskNo); nothing
    // left in DodgeF creates one. Only the float at 0x3C is known.
    class CLifeTask : public msw::menu::CDispTaskLink {
        public:
            unsigned char m_Unk30[0x3C - 0x30];
            float m_fUnk3C; // 0x3C
    };

    // The layout for nChrNum characters per team of maximum life nMaxLife.
    inline const char *GetLayoutName(long nMaxLife, long nChrNum)
    {
        return sLayoutNames[nMaxLife - 1][nChrNum - 1];
    }

    // The mark animations for the maximum life of the game.
    inline const unsigned short *GetMarkAnim(long nBar)
    {
        if (msw::CGameDodge::GetChrMaxLife(0) > MARK_NUM) {
            return sMarkAnimLong[nBar];
        }
        return sMarkAnim[nBar];
    }

    // The bar of life point nLife: 0 for 1 to 6, 1 for 7 to 12, else 2.
    inline long GetBarNo(long nLife)
    {
        long nBar = (nLife - 1) / MARK_NUM;

        if (nBar < 0) {
            nBar = 0;
        }
        if (nBar >= 2) {
            nBar = 2;
        }
        return nBar;
    }

    // The mark of life point nLife in bar nBar.
    inline long GetPieceNo(long nLife, long nBar)
    {
        long nPiece = nLife - nBar * MARK_NUM;

        if (nPiece < 0) {
            nPiece = 0;
        }
        return nPiece;
    }
}

msw::CCockpitDodgeLifeGauge::CCockpitDodgeLifeGauge(long nId)
    : CCockpitDodgeLayoutBase(nId)
{
    m_nStep = 0;

    for (long i = 0; i < SIDE_NUM; i++) {
        for (long j = 0; j < 6; j++) {
            m_GaugeState[i][j] = GaugeState();
        }
    }
}

msw::CCockpitDodgeLifeGauge::~CCockpitDodgeLifeGauge()
{
    Remove();
}

inline void msw::CCockpitDodgeLifeGauge::PlayAnimGauge(long nAnim)
{
    for (long i = 0; i < SIDE_NUM; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLinePane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

void msw::CCockpitDodgeLifeGauge::Main()
{
    if (m_nStep == 1) {
        PlayAnimGauge(ANIM_GAUGE_IN_WAIT);
        SetFaceIcon();
        m_nStep = 2;
    }
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
}

void msw::CCockpitDodgeLifeGauge::Disp()
{
    static const long camNo = 0;

    m_DispTaskList.DispLoop(m_pPaneAccessor, camNo);
}

int msw::CCockpitDodgeLifeGauge::Set()
{
    static const long animId = ANIM_VIS_01;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    long nNum = CGameDodge::GetVariableChrNum(0);

    m_nChrNum = nNum > 1 ? nNum : 2;

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 6, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(2), pLytCtrl->GetFontResourceName(2));

            long nMaxLife = CGameDodge::GetChrMaxLife(0);
            long nChrNum = m_nChrNum;

            pPaneCtrl->BuildLayout(0, sLayoutNames[nMaxLife - 1][nChrNum - 1]);
            pPaneCtrl->BuildAnim(0, sAnimNames[nMaxLife - 1][nChrNum - 1], sAnimNum[nMaxLife - 1][nChrNum - 1]);
            SetPaneAccessor(pPaneCtrl, sPaneNames, PANE_NUM);
            pPaneCtrl->SetSortZ(-988.0f);
            pPaneCtrl->SetVisible(0);
            pPaneCtrl->PlayAnim(animId, 1.0f, 0.0f, 1, 1);

            // The guard gauges start hidden.
            for (long i = 0; i < SIDE_NUM; i++) {
                for (long j = 0; j < nChrNum; j++) {
                    m_pPaneAccessor->SetPaneDisp(sChrPane[nChrNum - 1][i][j].nGuard, 0);
                }
            }

            // The centre pane goes to the top left corner of the screen.
            nw4r::lyt::Pane *pCenter = m_pPaneAccessor->GetPane(PANE_C_life);

            if (pCenter != 0) {
                float fWidth = (long)bcs::CMainApp::g_Rmode.fbWidth;
                float fHeight = (long)bcs::CMainApp::g_Rmode.efbHeight;
                nw4r::math::VEC3 pos(-(0.5f * fWidth), 0.5f * fHeight, 0.0f);

                pCenter->SetTranslate(pos);
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitDodgeLifeGauge::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

long msw::CCockpitDodgeLifeGauge::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitDodgeLifeGauge::GaugeOut()
{
    PlayAnimGauge(ANIM_GAUGE_OUT);
    return 0;
}

int msw::CCockpitDodgeLifeGauge::GaugeIn()
{
    PlayAnimGauge(ANIM_GAUGE_IN);
    return 0;
}

int msw::CCockpitDodgeLifeGauge::GaugeOutWait()
{
    PlayAnimGauge(ANIM_GAUGE_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeLifeGauge::GaugeInWait()
{
    PlayAnimGauge(ANIM_GAUGE_IN_WAIT);
    return 0;
}

long msw::CCockpitDodgeLifeGauge::LifeAnim(long nIdx, long nNo, long nAnim)
{
    if (nIdx >= SIDE_NUM) {
        return -1;
    }

    long nChrNum = m_nChrNum;

    if (nNo >= nChrNum) {
        return -1;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][nIdx][nNo].nLife);

    if (pObj != 0) {
        switch (nAnim) {
            case ANIM_HIT_05: {
                // The guard: the guard mark moves onto this gauge.
                bcs::G2D::CPaneObj *pGuard = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][nIdx][nNo].nGuard);

                if (pGuard != 0) {
                    for (long i = 0; i < GUARD_MARK_NUM; i++) {
                        bcs::G2D::CPaneObj *pMark = m_pPaneAccessor->GetPaneObj(sGuardPane[i]);

                        if (pMark != 0) {
                            bcs::G2D::SetPaneParent(pMark->GetPane(), pGuard->GetPane(), 1, 0);
                            pMark->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                        }
                    }
                    pGuard->GetPane()->SetVisible(true);
                }
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);
                pObj->PushNextAnimation(ANIM_HIT_04, 1.0f, 0.0f, 1, 1);
                break;
            }
            case ANIM_HIT_02: {
                bcs::G2D::CPaneObj *pGauge = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][nIdx][nNo].nGauge);

                if (pGauge != 0) {
                    pGauge->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);
                }
                break;
            }
            default:
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
                break;
        }
    }
    return 0;
}

void msw::CCockpitDodgeLifeGauge::SetLifeValue(long nIdx, long nNo, float fRate, float fRate2)
{
    // static-order swap (notes/decisions.md): declared in reverse.
    static const int reset = 1;
    static const int recursive = 1;
    static const float speed = 0.0f;
    static const long animId = ANIM_LIFE;

    if (nIdx >= SIDE_NUM) {
        return;
    }

    long nChrNum = m_nChrNum;

    if (nNo >= nChrNum) {
        return;
    }

    float fLife = fRate;

    if (fRate > 1.0f) {
        fLife = 1.0f;
    } else if (fRate < 0.0f) {
        fLife = 0.0f;
    }

    float fLife2 = fRate2;

    if (fRate2 > 1.0f) {
        fLife2 = 1.0f;
    } else if (fRate2 < 0.0f) {
        fLife2 = 0.0f;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][nIdx][nNo].nBar);
    bcs::G2D::CPaneObj *pObj2 = m_pPaneAccessor->GetPaneObj(sChrPane[nChrNum - 1][nIdx][nNo].nBar2);

    if (pObj == 0) {
        return;
    }

    // The bars run backwards: a full life is frame 0.
    float fMax = pObj->GetAnimMaxFrame(animId);
    float fFrame = fMax * (1.0f - fLife);
    float fFrame2 = fMax * (1.0f - fLife2);

    pObj->PlayAnimation(animId, speed, fFrame, recursive, reset, 1);
    if (pObj2 != 0) {
        pObj2->PlayAnimation(animId, speed, fFrame2, recursive, reset, 1);
    }
}

void msw::CCockpitDodgeLifeGauge::SetLifeValueText(long nIdx, long nNo, unsigned short nLife)
{
}

void msw::CCockpitDodgeLifeGauge::SetLifeValue(long nIdx, long nNo, long nLife, long nGuard)
{
    if (nIdx >= SIDE_NUM) {
        return;
    }

    long nChrNum = m_nChrNum;

    if (nNo >= nChrNum) {
        return;
    }

    long nBar = GetBarNo(nLife);
    long nRest = GetPieceNo(nLife, nBar);
    long nMaxLife = CGameDodge::GetChrMaxLife(0);

    if (nLife >= 0) {

        for (long i = 0; i < nMaxLife; i++) {
            long nMark = nMaxLife - i - 1;

            if (nMark < 0) {
                break;
            }

            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[nChrNum - 1][nIdx][nNo].nMark[nMark]);

            if (pObj != 0) {
                long nAnim;

                if (i < nRest) {
                    nAnim = GetMarkAnim(nBar)[0];
                } else if (nBar > 0) {
                    nAnim = GetMarkAnim(nBar - 1)[0];
                } else {
                    nAnim = GetMarkAnim(nBar)[2];
                }
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
            }
        }
    }

    if ((unsigned long)nGuard <= MARK_NUM) {

        for (long i = 0; i < MARK_NUM; i++) {
            long nMark = nMaxLife - i - 1;

            if (nMark < 0) {
                break;
            }

            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[nChrNum - 1][nIdx][nNo].nMark[nMark]);

            if (pObj != 0 && i < nGuard) {
                pObj->PlayAnimation(ANIM_MARK_GUARD, 1.0f, 0.0f, 0, 1, 1);
            }
        }
    }
}

void msw::CCockpitDodgeLifeGauge::AddLifeValue(long nIdx, long nNo, long nPrevLife, long nLife, long nPrevGuard,
                                               long nGuard)
{
    if (nIdx >= SIDE_NUM) {
        return;
    }
    if (nNo >= m_nChrNum) {
        return;
    }

    SetLifeValue(nIdx, nNo, nPrevLife, nPrevGuard);

    if (nPrevLife >= 0 && nLife >= 0) {
        long nLost = nPrevLife - nLife;

        if (nLost > 0) {
            _AddLifeValue(nIdx, nNo, nPrevLife, nLife, nPrevGuard, nGuard);

            // A break effect on every lost mark.
            for (long i = nLife; i < nPrevLife; i++) {
                long nBar = GetBarNo(i);
                long nPiece = GetPieceNo(i, nBar);

                if (nPiece == MARK_NUM) {
                    nPiece = 0;
                    nBar++;
                }
                SetBreakPieceAnim(nIdx, nNo, nPiece, nBar);
            }
        }
    } else {
        long nTask = nIdx >= SIDE_NUM ? 0 : sChrTaskNo[nIdx][nNo];
        CLifeTask *pTask = static_cast<CLifeTask *>(m_DispTaskList.FindNextKindObj(nTask + 1000, 0));

        if (pTask != 0) {
            pTask->m_fUnk3C = 0.0f;
        }
    }

    if ((unsigned long)nPrevGuard <= MARK_NUM && (unsigned long)nGuard <= MARK_NUM) {
        long nAdd = nGuard - nPrevGuard;

        if (nAdd > 0) {
            _AddLifeValue(nIdx, nNo, nPrevLife, nLife, nPrevGuard, nGuard);
        }
    }
}

void msw::CCockpitDodgeLifeGauge::_AddLifeValue(long nIdx, long nNo, long nPrevLife, long nLife, long nPrevGuard,
                                                long nGuard)
{
    if (nIdx >= SIDE_NUM) {
        return;
    }

    long nChrNum = m_nChrNum;

    if (nNo >= nChrNum) {
        return;
    }

    SetLifeValue(nIdx, nNo, nLife, nGuard);

    long nMaxLife = CGameDodge::GetChrMaxLife(nNo);

    // The lost marks break.
    if (nPrevLife >= 0 && nLife >= 0 && nPrevLife - nLife > 0) {

        for (; nLife < nPrevLife; nLife++) {
            long nBar = GetBarNo(nLife);

            if (GetPieceNo(nLife, nBar) == MARK_NUM) {
                nBar++;
            }

            long nMark = nMaxLife - nLife - 1;

            if (nMark < 0) {
                break;
            }

            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[nChrNum - 1][nIdx][nNo].nMark[nMark]);

            if (pObj != 0) {
                pObj->PlayAnimation(GetMarkAnim(nBar)[1], 1.0f, 0.0f, 0, 1, 1);
                if (nBar > 0) {
                    pObj->PushNextAnimation(GetMarkAnim(nBar - 1)[3], 1.0f, 0.0f, 0, 1);
                }
            }
        }
    }

    // The new guard marks. The loop stops unless the mark is the first
    // one (the original's check, kept as it is).
    if ((unsigned long)nPrevGuard <= MARK_NUM && (unsigned long)nGuard <= MARK_NUM && nGuard - nPrevGuard > 0) {

        for (; nPrevGuard < nGuard; nPrevGuard++) {
            long nMark = nMaxLife - nPrevGuard - 1;

            if (nMark != 0) {
                break;
            }

            bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[nChrNum - 1][nIdx][nNo].nMark[nMark]);

            if (pObj != 0) {
                pObj->PlayAnimation(ANIM_MARK_BREAK, 1.0f, 0.0f, 0, 1, 1);
            }
        }
    }
}

void msw::CCockpitDodgeLifeGauge::SetLifeAnim(long nIdx, long nNo, long nKind, long nTo, long nFrom)
{
    if (nIdx >= SIDE_NUM) {
        return;
    }

    int bPiece = 0;

    switch (nKind) {
        case 0:
            LifeAnim(nIdx, nNo, ANIM_HIT_00);
            break;
        case 1:
            LifeAnim(nIdx, nNo, ANIM_HIT_01);
            bPiece = 1;
            break;
        case 2:
            LifeAnim(nIdx, nNo, ANIM_HIT_05);
            bPiece = 1;
            break;
        case 3:
            LifeAnim(nIdx, nNo, ANIM_HIT_02);
            break;
    }

    if (bPiece == 0) {
        return;
    }
    if (nTo < 0 && nFrom < 0) {
        return;
    }
    if (nTo - nFrom > 0) {
        for (; nFrom < nTo; nFrom++) {
            long nBar = GetBarNo(nFrom);
            long nPiece = GetPieceNo(nFrom, nBar);
            long nAnim = -1;

            if (nPiece == MARK_NUM) {
                nPiece = 0;
                nBar++;
            }

            switch (nKind) {
                case 1:
                    nAnim = GetMarkAnim(nBar)[4];
                    break;
                case 2:
                    nAnim = GetMarkAnim(nBar)[0];
                    break;
            }
            SetLifePieceAnim(nIdx, nNo, nPiece, nBar, nAnim, -1);
        }
    }
}

long msw::CCockpitDodgeLifeGauge::SetBreakPieceAnim(long nIdx, long nNo, long nPiece, long nBar)
{
    if (nIdx >= SIDE_NUM) {
        return -1;
    }

    long nMaxLife = CGameDodge::GetChrMaxLife(nNo);
    long nMark = nMaxLife - nPiece - 1;

    if (nMark < 0) {
        return -1;
    }

    long nChrNum = m_nChrNum;
    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sLifePane[nChrNum - 1][nIdx][nNo].nMarkP[nMark]);

    if (pObj == 0) {
        return -1;
    }

    // No effect is picked for a maximum life past 6: the original then
    // passes whatever the register holds.
    long nEffect;

    switch (nMaxLife) {
        case 1:
        case 2:
            nEffect = EFFECT_MARK_BREAK_1;
            break;
        case 3:
        case 4:
            nEffect = EFFECT_MARK_BREAK_3;
            break;
        case 5:
        case 6:
            nEffect = EFFECT_MARK_BREAK_5;
            break;
    }
    CEffectCtrl::CreateAttachEffect(nEffect, pObj, 0, -1, 0, 0, 0, 4, 0, 0);
    return 0;
}

long msw::CCockpitDodgeLifeGauge::SetLifePieceAnim(long nIdx, long nNo, long nPiece, long nBar, long nAnim,
                                                   long nNext)
{
    if (nIdx >= SIDE_NUM) {
        return -1;
    }

    long nChrNum = m_nChrNum;

    if (nNo >= nChrNum) {
        return -1;
    }

    long nMaxLife = CGameDodge::GetChrMaxLife(nNo);
    long nMark = nMaxLife - nPiece - 1;

    if (nMark < 0) {
        return -1;
    }

    bcs::G2D::CPaneObj *pObj =
        m_pPaneAccessor->GetPaneObj(sPaneNames[sLifePane[nChrNum - 1][nIdx][nNo].nMark[nMark]]);

    if (pObj == 0) {
        return -2;
    }

    pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 1, 1, 1);
    if (nNext >= 0) {
        pObj->PushNextAnimation(nNext, 1.0f, 0.0f, 0, 1);
    }
    return 0;
}

void msw::CCockpitDodgeLifeGauge::SetLifeDisp(int bDisp)
{
    for (long i = 0; i < SIDE_NUM; i++) {
        m_pPaneAccessor->SetPaneDisp(sLinePane[i], bDisp);
    }
}

long msw::CCockpitDodgeLifeGauge::ChangeChargeGauge(long nIdx, long nNo, float fRate, float fRate2)
{
    return 0;
}

void msw::CCockpitDodgeLifeGauge::SetFaceIcon()
{
    SetFaceIcon(GetLytObj());
}

void msw::CCockpitDodgeLifeGauge::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pPaneCtrl == 0) {
        return;
    }

    // The character of each gauge, by team and character.
    long nChr[SIDE_NUM][CHR_NUM] = {
        { 0, 2, 4 },
        { 1, 3, 5 }
    };
    long nChrNum = m_nChrNum;

    for (long i = 0; i < SIDE_NUM; i++) {
        for (long j = 0; j < nChrNum; j++) {
            CChrCore *pChr = CChrMan::GetChrCore(nChr[i][j]);

            if (pChr != 0 && !pChr->IsDisable()) {
                SetFaceIcon(pPaneCtrl, i, j, pChr);
            }
        }
    }
}

void msw::CCockpitDodgeLifeGauge::SetFaceIcon(bcs::G2D::CPaneCtrl *pPaneCtrl, long nTeam, long nNo,
                                              const CChrCore *pChr)
{
    // static-order swap (notes/decisions.md): the original declares
    // texMapId first and a reset/recursive pair in each branch of the mark
    // code below; declared here, in reverse, they take the original's
    // places.
    static const int reset = 1;
    static const int recursive = 0;
    static const int resetNew = 1;
    static const int recursiveNew = 0;
    static const long texMapId = 0;

    long nChrNum = m_nChrNum;
    long nIdx = CGame::GetTeamColorIdx(nTeam);

    // The face picture, made once.
    nw4r::lyt::Pane *pFaceBase = m_pPaneAccessor->GetPane(sChrPane[nChrNum - 1][nIdx][nNo].nFace);

    if (pFaceBase == 0) {
        return;
    }

    nw4r::math::VEC3 zero(0.0f, 0.0f, 0.0f);
    long nFace = sIconPane[nIdx][nNo][0];
    nw4r::lyt::Pane *pFace = m_pPaneAccessor->GetPane(nFace);

    if (pFace == 0) {
        pFace = menu::CreateFaceIconPicturePane(pChr->GetPlayerNo(), pPaneCtrl, pFaceBase, sPaneNames[nFace]);
        bcs::G2D::SetPaneSizeFromParent(pFace);
    } else {
        const _GXTexObj *pTexObj = pChr->GetCockpitIconTexObj();

        if (pTexObj != 0) {
            bcs::G2D::ReplacePanePicture(pFace, pTexObj, texMapId);
        }
    }
    pFace->SetScale(menu::GetCockpitFaceIconScale(pChr->GetChrIdS()));

    // The player or COM mark.
    nw4r::lyt::Pane *pMarkBase = m_pPaneAccessor->GetPane(sChrPane[nChrNum - 1][nIdx][nNo].nMarkBase);

    if (pMarkBase == 0) {
        return;
    }

    long nMark;

    if (pChr->IsPadCtrl()) {
        nMark = sPlayerMarkPane[pChr->GetCtrlPlayerNo()];
    } else {
        nMark = PANE_com;
    }

    nw4r::lyt::Pane *pMark = m_pPaneAccessor->GetPane(nMark);

    if (pMark == 0) {
        return;
    }

    const CChrCore *pLeader = cockpit::GetTeamLeaderChrCore(nTeam);
    long nPNo = sIconPane[nIdx][nNo][1];
    nw4r::lyt::Pane *pPNo = m_pPaneAccessor->GetPane(nPNo);

    if (pPNo == 0) {
        pPNo = bcs::G2D::DuplicatePane(pPaneCtrl, pMark, sPaneNames[nPNo],
                                       GetLayoutName(CGameDodge::GetChrMaxLife(0), m_nChrNum), 0);
        bcs::G2D::SetPaneParent(pPNo, pMarkBase, 1, 0);
        pPNo->SetVisible(true);
        pPNo->SetTranslate(zero);

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pMarkBase);

        if (pObj != 0 && nChrNum > 1) {
            long nAnim;

            if (pChr->IsPadCtrl() || pChr == pLeader) {
                nAnim = ANIM_SP_01;
            } else {
                nAnim = ANIM_SP_00;
            }
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursiveNew, resetNew, 1);
        }
    } else {
        if (pLeader != 0 && !pLeader->IsPadCtrl()) {
            return;
        }

        if (nMark != PANE_com) {
            bcs::G2D::ReplacePanePicture(pPNo, pMark);
            pPNo->SetSize(pMark->GetSize());
            for (long i = 0; i < 4; i++) {
                pPNo->SetVtxColor(i, pMark->GetVtxColor(i));
            }
        }

        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(pMarkBase);

        if (pObj != 0 && nChrNum > 1) {
            long nAnim = -1;

            if (nMark == PANE_com) {
                if (pObj->IsAnimPlay(ANIM_SP_01)) {
                    nAnim = ANIM_SP_03;
                }
            } else if (!pObj->IsAnimPlay(ANIM_SP_01)) {
                nAnim = ANIM_SP_01;
            }
            if (nAnim >= 0) {
                pObj->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
            }
        }
    }

    // The star, a copy of cs_ico made once, shown for a star player.
    nw4r::lyt::Pane *pStarSrc = m_pPaneAccessor->GetPane(PANE_cs_ico);
    nw4r::lyt::Pane *pStarBase = m_pPaneAccessor->GetPane(sChrPane[nChrNum - 1][nIdx][nNo].nStarBase);

    if (pStarSrc != 0 && pStarBase != 0) {
        long nStar = sIconPane[nIdx][nNo][2];
        nw4r::lyt::Pane *pStar = m_pPaneAccessor->GetPane(nStar);

        if (pStar == 0) {
            pStar = bcs::G2D::DuplicatePane(pPaneCtrl, pStarSrc, sPaneNames[nStar],
                                            GetLayoutName(CGameDodge::GetChrMaxLife(0), m_nChrNum), 0);
            bcs::G2D::SetPaneParent(pStar, pStarBase, 1, 0);
            bcs::G2D::SetPaneSizeFromParent(pStar);
            pStar->SetTranslate(zero);
        }
        if (pStar != 0) {
            pStar->SetVisible(pChr->IsStarPlayer() != 0);
        }
    }
}
