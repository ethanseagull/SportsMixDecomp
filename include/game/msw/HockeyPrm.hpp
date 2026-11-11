#ifndef MSW_HOCKEYPRM_HPP_
#define MSW_HOCKEYPRM_HPP_

// Tuning values of the hockey mode (HockeyF), globals in namespace msw
// (HOCKEY_PRM_*__3msw). Only the ones used by source written so far are
// declared; the types are those of the loads (lwz: long, lfs: float).
namespace msw {

    // Stage 02, the wave stage (CStgGmk_Wave_Hockey).
    extern float HOCKEY_PRM_S02_WAVE_PART_INTERVAL;
    extern float HOCKEY_PRM_S02_WAVE_BIG_INTERVAL;
    extern float HOCKEY_PRM_S02_WAVE_ANIM_TIME;
    extern float HOCKEY_PRM_S02_WAVE_BIG_ANIM_TIME;
    extern float HOCKEY_PRM_S02_WAVE_BIG_OFFSET_X;
    extern float HOCKEY_PRM_S02_WAVE_PUSH_POWER;
    extern float HOCKEY_PRM_S02_WAVE_PULL_POWER;
    extern float HOCKEY_PRM_S02_WAVE_POS_MARGIN;
    extern long HOCKEY_PRM_S02_BIG_COIN_POP_NUM;
    extern long HOCKEY_PRM_S02_ITEM_POP_CYCLE;
    extern long HOCKEY_PRM_S02_ITEM_POP_NUM;
    extern float HOCKEY_PRM_S02_ITEM_POP_DELAY;
    extern float HOCKEY_PRM_S02_ITEM_POP_COME_FRM;
    extern float HOCKEY_PRM_S02_ITEM_POP_DIST;
    extern float HOCKEY_PRM_S02_ITEM_POP_POS_X;
    extern long HOCKEY_PRM_S02_BIG_ITEM_POP_CYCLE;
    extern long HOCKEY_PRM_S02_BIG_ITEM_POP_NUM;
    extern float HOCKEY_PRM_S02_BIG_ITEM_POP_DELAY;
    extern float HOCKEY_PRM_S02_BIG_ITEM_POP_COME_FRM;
    extern float HOCKEY_PRM_S02_BIG_ITEM_POP_DIST;
    extern float HOCKEY_PRM_S02_BIG_ITEM_POP_POS_X;
    extern float HOCKEY_PRM_S02_WAVE_BIG_INTERVAL_RND;
    extern float HOCKEY_PRM_S02_WAVE_OFFSET_X;
    extern long HOCKEY_PRM_S02_COIN_POP_NUM;

    // Stage 03, the fountain stage: the water pillars (WATER_*) and their
    // cluster (WC_*). Times are in seconds, angles in degrees.
    extern float HOCKEY_PRM_S03_WATER_RADIUS;
    extern float HOCKEY_PRM_S03_WATER_RADIUS2;
    extern float HOCKEY_PRM_S03_WATER_RADIUS3;
    extern float HOCKEY_PRM_S03_WATER_RESIST;
    extern float HOCKEY_PRM_S03_WATER_HEIGHT;
    extern float HOCKEY_PRM_S03_WATER_PRESTART_TIME;
    extern float HOCKEY_PRM_S03_WATER_STAY_TIME;
    extern float HOCKEY_PRM_S03_WATER_INTERVAL;
    extern float HOCKEY_PRM_S03_WC_ZONE_ANGLE;
    extern float HOCKEY_PRM_S03_WC_ZONE_WIDTH;
    extern float HOCKEY_PRM_S03_WC_ZONE_MARGIN;
    extern float HOCKEY_PRM_S03_WC_OFFSET_Z;
    extern float HOCKEY_PRM_S03_WC_LOC_RADIUS;
    extern long HOCKEY_PRM_S03_WC_LOC_NUM;
    extern long HOCKEY_PRM_S03_WC_LOC_NUM_PER_GRP;
    extern long HOCKEY_PRM_S03_WATER_SLOWDOWN_RATE;
    extern long HOCKEY_PRM_S03_DEB_DRAW_INFO; // in .bss

    // Stage 07, the boss Packun stage (CStageHockey07): the order of the
    // stage's parameter table (GetPrmTbl).
    extern float HOCKEY_PRM_S07_PACKUN_SCALE;
    extern float HOCKEY_PRM_S07_PACKUN_COLL_RAD;
    extern float HOCKEY_PRM_S07_PACKUN_LOCATE_X;
    extern float HOCKEY_PRM_S07_PACKUN_LOCATE_Z; // not in the table
    extern float HOCKEY_PRM_S07_PACKUN_SPEED_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_ROT_WAIT_TIME;
    extern float HOCKEY_PRM_S07_PACKUN_ROT_TIME;
    extern float HOCKEY_PRM_S07_PACKUN_MOVE_DIST;
    extern float HOCKEY_PRM_S07_PACKUN_MOVE_WIN_RATE;
    extern float HOCKEY_PRM_S07_PACKUN_MOVE_LOSE_RATE;
    extern float HOCKEY_PRM_S07_PACKUN_MOVE_NOMAL_RATE;
    extern float HOCKEY_PRM_S07_PACKUN_MOVE_BALL_RATE;
    extern float HOCKEY_PRM_S07_PACKUN_SEARCH_DIST;
    extern long HOCKEY_PRM_S07_PACKUN_SEARCH_CHR;
    extern float HOCKEY_PRM_S07_PACKUN_SEARCH_NO_TIME;
    extern float HOCKEY_PRM_S07_PACKUN_WAIT_TIME_MIN;
    extern float HOCKEY_PRM_S07_PACKUN_WAIT_TIME_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_SLEEP_RATE;
    extern float HOCKEY_PRM_S07_PACKUN_SLEEP_TIME_MIN;
    extern float HOCKEY_PRM_S07_PACKUN_SLEEP_TIME_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_ATK_Z;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_ATK_X;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_NUM;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_START_POS_Y;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_START_POS_Z;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_ROT_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_SEARCH_ROT;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_SEARCH_ROT_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_BLT_TIME;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_BLT_TIME_RND;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_SCL_MAX;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_SCL_MIN;
    extern float HOCKEY_PRM_S07_PACKUN_MAD_TIME;
    extern long HOCKEY_PRM_S07_PACKUN_EXPLODE_SIZE;
    extern float HOCKEY_PRM_S07_PACKUN_NEAR_RANG;
    extern float HOCKEY_PRM_S07_PACKUN_FAR_RANG;
    extern float HOCKEY_PRM_S07_PACKUN_ANGLE_ALPHA;
    extern float HOCKEY_PRM_S07_GOAL_X1_LENGTH;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX0;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX1;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN2;
    extern float HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX2;

    // Stage 04, the pylon stage.
    extern float HOCKEY_PRM_S04_PYLON_MDL_RADIUS;
    extern float HOCKEY_PRM_S04_PYLON_MDL_HEIGHT;
    extern float HOCKEY_PRM_S04_PYLON_RADIUS;
    extern float HOCKEY_PRM_S04_PYLON_HEIGHT;
    extern float HOCKEY_PRM_S04_PYLON_LIFE;
    extern float HOCKEY_PRM_S04_PYLON_FIELD_MARGIN;
    extern long HOCKEY_PRM_S04_PYLON_PUSH_POWER;
    extern float HOCKEY_PRM_S04_PYLON_PUSH_RESIST;
    extern float HOCKEY_PRM_S04_PYLON_ACC;
    extern float HOCKEY_PRM_S04_PYLON_DEC;
    extern long HOCKEY_PRM_S04_PYLON_LOC_PAT_TYPE;
    extern long HOCKEY_PRM_S04_PYLON_LOC_PAT_NO;

    // Stage 11.
    extern float HOCKEY_PRM_S11_COURT_INIT_WAIT;
    extern float HOCKEY_PRM_S11_COURT_WAIT;
    extern float HOCKEY_PRM_S11_COURT_ANGLE;
    extern float HOCKEY_PRM_S11_COURT_ROT_SPEED;
    extern float HOCKEY_PRM_S11_COURT_ROT_ACC;
    extern float HOCKEY_PRM_S11_CHR_FLOW_SPEED;
    extern float HOCKEY_PRM_S11_BUBBLE_SCALE;
    extern float HOCKEY_PRM_S11_BUBBLE_PRESTART;
    extern float HOCKEY_PRM_S11_BUBBLE_JUMP_HEIGHT;
    extern float HOCKEY_PRM_S11_BUBBLE_JUMP_FRM;
    extern float HOCKEY_PRM_S11_BUBBLE_JUMP_DOF_FRM;
    extern float HOCKEY_PRM_S11_BUBBLE_DOWN_FRM;
    extern float HOCKEY_PRM_S11_BUBBLE_INIT_INTERVAL;
    extern float HOCKEY_PRM_S11_BUBBLE_INTERVAL;
    extern float HOCKEY_PRM_S11_BUBBLE_ZONE_Z;

    // Stage 12.
    extern float HOCKEY_PRM_S12_BALL_KICK_POW;
    extern float HOCKEY_PRM_S12_BALL_GUIDE_DECAY;
    extern float HOCKEY_PRM_S12_BALL_FORCE_ACC;
    extern long HOCKEY_PRM_S12_BALL_END_LINE;
    extern float HOCKEY_PRM_S12_BALL_SIZE;
    extern float HOCKEY_PRM_S12_BALL_COLLSIZE;
    extern float HOCKEY_PRM_S12_BALL_SPEED;
    extern float HOCKEY_PRM_S12_BALL_SPEED_LIMIT;
    extern float HOCKEY_PRM_S12_BALL_BALL_KICK_POW;
    extern float HOCKEY_PRM_S12_BALL_DECEL;
    extern float HOCKEY_PRM_S12_BALL_INIT_SLEEP;
    extern float HOCKEY_PRM_S12_BALL_INTERVAL;
    extern float HOCKEY_PRM_S12_BALL_ANGLE;
    extern float HOCKEY_PRM_S12_BALL_SIDE_RATE;
    extern long HOCKEY_PRM_S12_BALL_NUM;
    extern float HOCKEY_PRM_S12_BALL_DOWN_TIME;
    extern float HOCKEY_PRM_S12_BALL_HIT_DECAY;

    // Stage 85 (the crush skate stages, CStageHockeyCrushSkate*), floor 0.
    extern float HOCKEY_PRM_S85_FLOOR0_CYCLE;
    extern float HOCKEY_PRM_S85_FLOOR0_STOP_TIME;
    extern float HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND;
    extern float HOCKEY_PRM_S85_FLOOR0_CHR_FLOW_VEL;

    // The rest of stage 85 (CStageHockeyCrushSkate85::GetPrmTbl order).
    extern float HOCKEY_PRM_S85_COURT_WIDTH;
    extern float HOCKEY_PRM_S85_COURT_DEPTH;
    extern float HOCKEY_PRM_S85_COURT_RADIUS;
    extern float HOCKEY_PRM_S85_BG_ROT_SPEED;
    extern long HOCKEY_PRM_S85_FLOOR0_ROT_ENABLE;
    extern float HOCKEY_PRM_S85_FLOOR0_INIT_WAIT;
    extern float HOCKEY_PRM_S85_FLOOR0_STOP_TIME2;
    extern float HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S85_FLOOR0_ROT_ANGLE;
    extern float HOCKEY_PRM_S85_ICE0_ACC;
    extern float HOCKEY_PRM_S85_ICE0_DEC;
    extern float HOCKEY_PRM_S85_ICE0_INTERVAL;
    extern long HOCKEY_PRM_S85_ICE0_LOC_PAT_TYPE;
    extern long HOCKEY_PRM_S85_ICE0_LOC_PAT_NO;
    extern float HOCKEY_PRM_S85_ICE0_LOC_RND_X;
    extern float HOCKEY_PRM_S85_ICE0_LOC_RND_Z;
    extern long HOCKEY_PRM_S85_FLOOR1_ROT_ENABLE;
    extern float HOCKEY_PRM_S85_FLOOR1_INIT_WAIT;
    extern float HOCKEY_PRM_S85_FLOOR1_CYCLE;
    extern float HOCKEY_PRM_S85_FLOOR1_STOP_TIME;
    extern float HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND;
    extern float HOCKEY_PRM_S85_FLOOR1_STOP_TIME2;
    extern float HOCKEY_PRM_S85_FLOOR1_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S85_FLOOR1_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S85_FLOOR1_ROT_ANGLE;
    extern float HOCKEY_PRM_S85_ICE1_ACC;
    extern float HOCKEY_PRM_S85_ICE1_DEC;
    extern float HOCKEY_PRM_S85_ICE1_INTERVAL;
    extern long HOCKEY_PRM_S85_ICE1_LOC_PAT_TYPE;
    extern long HOCKEY_PRM_S85_ICE1_LOC_PAT_NO;
    extern float HOCKEY_PRM_S85_ICE1_LOC_RND_X;
    extern float HOCKEY_PRM_S85_ICE1_LOC_RND_Z;
    extern float HOCKEY_PRM_S85_ICECUBE_MDL_RADIUS;
    extern float HOCKEY_PRM_S85_ICECUBE_MDL_HEIGHT;
    extern float HOCKEY_PRM_S85_ICECUBE_RADIUS;
    extern float HOCKEY_PRM_S85_ICECUBE_HEIGHT;
    extern float HOCKEY_PRM_S85_ICECUBE_HEIGHT2;
    extern float HOCKEY_PRM_S85_ICECUBE_LOC_POS_Y;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_OFS;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_OFS2;
    extern float HOCKEY_PRM_S85_ICECUBE_EFF_VEL;
    extern float HOCKEY_PRM_S85_ICECUBE_VEL;
    extern long HOCKEY_PRM_S85_ICECUBE_PUSH_POWER;
    extern float HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_ACC;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_ACC2;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_ACC3;
    extern float HOCKEY_PRM_S85_ICECUBE_ATK_ACC4;
    extern long HOCKEY_PRM_S85_ICECUBE_PUSH_POWER_2;
    extern float HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST_2;

    // Stage 85 floors 2 and 3: read by its GetParam_* functions, not in its
    // parameter table.
    extern long HOCKEY_PRM_S85_FLOOR2_ROT_ENABLE;
    extern float HOCKEY_PRM_S85_FLOOR2_INIT_WAIT;
    extern float HOCKEY_PRM_S85_FLOOR2_CYCLE;
    extern float HOCKEY_PRM_S85_FLOOR2_STOP_TIME;
    extern float HOCKEY_PRM_S85_FLOOR2_STOP_TIME_RND;
    extern float HOCKEY_PRM_S85_FLOOR2_STOP_TIME2;
    extern float HOCKEY_PRM_S85_FLOOR2_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S85_FLOOR2_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S85_FLOOR2_ROT_ANGLE;
    extern long HOCKEY_PRM_S85_FLOOR3_ROT_ENABLE;
    extern float HOCKEY_PRM_S85_FLOOR3_INIT_WAIT;
    extern float HOCKEY_PRM_S85_FLOOR3_CYCLE;
    extern float HOCKEY_PRM_S85_FLOOR3_STOP_TIME;
    extern float HOCKEY_PRM_S85_FLOOR3_STOP_TIME_RND;
    extern float HOCKEY_PRM_S85_FLOOR3_STOP_TIME2;
    extern float HOCKEY_PRM_S85_FLOOR3_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S85_FLOOR3_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S85_FLOOR3_ROT_ANGLE;

    // Stage 86 (the fire pillars, msw::CStgGmk_FirePillar_Hockey_Act).
    extern float HOCKEY_PRM_S86_FIRE_RADIUS;
    extern float HOCKEY_PRM_S86_FIRE_RADIUS2;
    extern float HOCKEY_PRM_S86_FIRE_RADIUS3;
    extern float HOCKEY_PRM_S86_FIRE_HEIGHT;
    extern float HOCKEY_PRM_S86_FIRE_TIME2;
    extern float HOCKEY_PRM_S86_FIRE0_PRESTART_TIME2;
    extern float HOCKEY_PRM_S86_FIRE1_PRESTART_TIME2;
    extern float HOCKEY_PRM_S86_FIRE2_PRESTART_TIME2;
    extern float HOCKEY_PRM_S86_FIRE3_PRESTART_TIME2;
    extern float HOCKEY_PRM_S86_FIRE_TIME1;
    extern float HOCKEY_PRM_S86_FIRE0_PRESTART_TIME1;
    extern float HOCKEY_PRM_S86_FIRE1_PRESTART_TIME1;
    extern float HOCKEY_PRM_S86_FIRE2_PRESTART_TIME1;
    extern float HOCKEY_PRM_S86_FIRE3_PRESTART_TIME1;
    extern float HOCKEY_PRM_S86_FIRE0_PRESTART_TIME0;
    extern float HOCKEY_PRM_S86_FIRE1_PRESTART_TIME0;
    extern float HOCKEY_PRM_S86_FIRE2_PRESTART_TIME0;
    extern float HOCKEY_PRM_S86_FIRE3_PRESTART_TIME0;
    extern float HOCKEY_PRM_S86_FIRE0_STAY_TIME2;
    extern float HOCKEY_PRM_S86_FIRE1_STAY_TIME2;
    extern float HOCKEY_PRM_S86_FIRE2_STAY_TIME2;
    extern float HOCKEY_PRM_S86_FIRE3_STAY_TIME2;
    extern float HOCKEY_PRM_S86_FIRE0_STAY_TIME1;
    extern float HOCKEY_PRM_S86_FIRE1_STAY_TIME1;
    extern float HOCKEY_PRM_S86_FIRE2_STAY_TIME1;
    extern float HOCKEY_PRM_S86_FIRE3_STAY_TIME1;
    extern float HOCKEY_PRM_S86_FIRE0_STAY_TIME0;
    extern float HOCKEY_PRM_S86_FIRE1_STAY_TIME0;
    extern float HOCKEY_PRM_S86_FIRE2_STAY_TIME0;
    extern float HOCKEY_PRM_S86_FIRE3_STAY_TIME0;
    extern float HOCKEY_PRM_S86_FIRE0_INTERVAL2;
    extern float HOCKEY_PRM_S86_FIRE1_INTERVAL2;
    extern float HOCKEY_PRM_S86_FIRE2_INTERVAL2;
    extern float HOCKEY_PRM_S86_FIRE3_INTERVAL2;
    extern float HOCKEY_PRM_S86_FIRE0_INTERVAL1;
    extern float HOCKEY_PRM_S86_FIRE1_INTERVAL1;
    extern float HOCKEY_PRM_S86_FIRE2_INTERVAL1;
    extern float HOCKEY_PRM_S86_FIRE3_INTERVAL1;
    extern float HOCKEY_PRM_S86_FIRE0_INTERVAL0;
    extern float HOCKEY_PRM_S86_FIRE1_INTERVAL0;
    extern float HOCKEY_PRM_S86_FIRE2_INTERVAL0;
    extern float HOCKEY_PRM_S86_FIRE3_INTERVAL0;
    extern float HOCKEY_PRM_S86_FIRE_RESIST;

    // Stage 87 (CStageHockeyCrushSkate87::GetPrmTbl order).
    extern float HOCKEY_PRM_S87_COURT_WIDTH;
    extern float HOCKEY_PRM_S87_COURT_DEPTH;
    extern float HOCKEY_PRM_S87_COURT_RADIUS;
    extern float HOCKEY_PRM_S87_HOLE_WIDTH;
    extern float HOCKEY_PRM_S87_HOLE_DEPTH;
    extern float HOCKEY_PRM_S87_HOLE_RADIUS;
    extern float HOCKEY_PRM_S87_BG_ROT_SPEED;
    extern long HOCKEY_PRM_S87_FLOOR0_ROT_ENABLE;
    extern float HOCKEY_PRM_S87_FLOOR0_INIT_WAIT;
    extern float HOCKEY_PRM_S87_FLOOR0_CYCLE;
    extern float HOCKEY_PRM_S87_FLOOR0_STOP_TIME;
    extern float HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND;
    extern float HOCKEY_PRM_S87_FLOOR0_STOP_TIME2;
    extern float HOCKEY_PRM_S87_FLOOR0_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S87_FLOOR0_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S87_FLOOR0_ROT_ANGLE;
    extern long HOCKEY_PRM_S87_FLOOR1_ROT_ENABLE;
    extern float HOCKEY_PRM_S87_FLOOR1_INIT_WAIT;
    extern float HOCKEY_PRM_S87_FLOOR1_CYCLE;
    extern float HOCKEY_PRM_S87_FLOOR1_STOP_TIME;
    extern float HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND;
    extern float HOCKEY_PRM_S87_FLOOR1_STOP_TIME2;
    extern float HOCKEY_PRM_S87_FLOOR1_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S87_FLOOR1_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S87_FLOOR1_ROT_ANGLE;
    extern long HOCKEY_PRM_S87_FLOOR2_ROT_ENABLE;
    extern float HOCKEY_PRM_S87_FLOOR2_INIT_WAIT;
    extern float HOCKEY_PRM_S87_FLOOR2_CYCLE;
    extern float HOCKEY_PRM_S87_FLOOR2_STOP_TIME;
    extern float HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND;
    extern float HOCKEY_PRM_S87_FLOOR2_STOP_TIME2;
    extern float HOCKEY_PRM_S87_FLOOR2_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S87_FLOOR2_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S87_FLOOR2_ROT_ANGLE;
    extern long HOCKEY_PRM_S87_FLOOR3_ROT_ENABLE;
    extern float HOCKEY_PRM_S87_FLOOR3_INIT_WAIT;
    extern float HOCKEY_PRM_S87_FLOOR3_CYCLE;
    extern float HOCKEY_PRM_S87_FLOOR3_STOP_TIME;
    extern float HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND;
    extern float HOCKEY_PRM_S87_FLOOR3_STOP_TIME2;
    extern float HOCKEY_PRM_S87_FLOOR3_STOP_TIME_RND2;
    extern float HOCKEY_PRM_S87_FLOOR3_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S87_FLOOR3_ROT_ANGLE;

    // Stage 15.
    extern float HOCKEY_PRM_S15_CASE_INTERVAL;
    extern float HOCKEY_PRM_S15_CASE_POS_Y;
    extern float HOCKEY_PRM_S15_CASE_LIFE_0;
    extern float HOCKEY_PRM_S15_CASE_LIFE_1;
    extern float HOCKEY_PRM_S15_CASE_LIFE_2;
    extern long HOCKEY_PRM_S15_GOAL_MOVE;
    extern float HOCKEY_PRM_S15_CASE_WIDTH;
    extern float HOCKEY_PRM_S15_CASE_DEPTH;
    extern float HOCKEY_PRM_S15_CASE_HEIGHT;
    extern float HOCKEY_PRM_S15_CASE_OFFSET_Z;
    extern float HOCKEY_PRM_S15_CASE_SPACING_Z;
    extern float HOCKEY_PRM_S15_CASE_ACC_0;
    extern float HOCKEY_PRM_S15_CASE_DEC_0;
    extern float HOCKEY_PRM_S15_CASE_DEC_RND_0;
    extern float HOCKEY_PRM_S15_CASE_VEL_0;
    extern float HOCKEY_PRM_S15_CASE_ACC_1;
    extern float HOCKEY_PRM_S15_CASE_DEC_1;
    extern float HOCKEY_PRM_S15_CASE_DEC_RND_1;
    extern float HOCKEY_PRM_S15_CASE_VEL_1;
    extern float HOCKEY_PRM_S15_CASE_ACC_2;
    extern float HOCKEY_PRM_S15_CASE_DEC_2;
    extern float HOCKEY_PRM_S15_CASE_DEC_RND_2;
    extern float HOCKEY_PRM_S15_CASE_VEL_2;
    extern float HOCKEY_PRM_S15_RAINY_INTERVAL;
    extern float HOCKEY_PRM_S15_RAINY_TIME;
    extern float HOCKEY_PRM_S15_SHIP_CYCLE;
    extern float HOCKEY_PRM_S15_SHIP_ROT_ANGLE;
    extern float HOCKEY_PRM_S15_SHIP_STOP_TIME;
    extern float HOCKEY_PRM_S15_SHIP_STOP_TIME_RND;
    extern float HOCKEY_PRM_S15_CHR_FLOW_VEL;
    extern float HOCKEY_PRM_S15_GOAL_FLOW_VEL;
    extern long HOCKEY_PRM_S15_FLASH_COLOR_R;
    extern long HOCKEY_PRM_S15_FLASH_COLOR_G;
    extern long HOCKEY_PRM_S15_FLASH_COLOR_B;
    extern long HOCKEY_PRM_S15_FLASH_COLOR_A;
    extern float HOCKEY_PRM_S15_FLASH_TIME;
    extern float HOCKEY_PRM_S15_FLASH_GAIN;
    extern float HOCKEY_PRM_S15_FLASH_DECAY;
    extern float HOCKEY_PRM_S15_KARON_SCALE;
    extern float HOCKEY_PRM_S15_KARON_COLL_SCALE;
    extern float HOCKEY_PRM_S15_KARON_SPEED;

    // Stage 16.
    extern float HOCKEY_PRM_S16_POP_TIME_RND;
    extern float HOCKEY_PRM_S16_DROP_AREA_Z;
    extern long HOCKEY_PRM_S16_MAX_NUM;
    extern float HOCKEY_PRM_S16_POP_TIME;
    extern float HOCKEY_PRM_S16_POP_POS_Y;
    extern float HOCKEY_PRM_S16_DROP_SPEED;
    extern float HOCKEY_PRM_S16_DROP_ANGLE;
    extern long HOCKEY_PRM_S16_BOUND_NUM;
    extern float HOCKEY_PRM_S16_BOUND_REDUCE_Y;
    extern float HOCKEY_PRM_S16_BOUND_REDUCE_XZ;
    extern long HOCKEY_PRM_S16_ADD_COIN_0;
    extern float HOCKEY_PRM_S16_WARP_INTERVAL;
    extern float HOCKEY_PRM_S16_WARP_TIME;
    extern float HOCKEY_PRM_S16_WARP_SPEED;
    extern float HOCKEY_PRM_S16_WARP_BLUR_SHIFT;
    extern long HOCKEY_PRM_S16_FLASH_COLOR_R;
    extern long HOCKEY_PRM_S16_FLASH_COLOR_G;
    extern long HOCKEY_PRM_S16_FLASH_COLOR_B;
    extern long HOCKEY_PRM_S16_FLASH_COLOR_A;
    extern float HOCKEY_PRM_S16_FLASH_TIME;
    extern float HOCKEY_PRM_S16_FLASH_GAIN;
    extern float HOCKEY_PRM_S16_FLASH_DECAY;

    // Stage 09, the conveyor belt stage (CStageHockey09): belts (CV_*),
    // containers (OBJ_*, CONTAINER_*, BOMB_*, TREASURE_*). Times are in
    // seconds.
    extern float HOCKEY_PRM_S09_EV_INTERVAL;
    extern float HOCKEY_PRM_S09_CV_STOP_TIME;
    extern float HOCKEY_PRM_S09_CV_ACC_TIME;
    extern float HOCKEY_PRM_S09_CV_PRE_ACC_TIME;
    extern float HOCKEY_PRM_S09_CV_POS;
    extern float HOCKEY_PRM_S09_CV_WIDTH;
    extern float HOCKEY_PRM_S09_CV_SPEED_0;
    extern float HOCKEY_PRM_S09_CV_ACC_SPEED_0;
    extern float HOCKEY_PRM_S09_OBJ_INTERVAL;
    extern float HOCKEY_PRM_S09_OBJ_BOMB_RATE;
    extern float HOCKEY_PRM_S09_TREASURE_INTERVAL;
    extern long HOCKEY_PRM_S09_TREASURE_NUM;
    extern float HOCKEY_PRM_S09_TREASURE_RUN_TIME;
    extern float HOCKEY_PRM_S09_CONTAINER_WIDTH;
    extern float HOCKEY_PRM_S09_CONTAINER_HEIGHT;
    extern float HOCKEY_PRM_S09_CONTAINER_DEC_RND;
    extern float HOCKEY_PRM_S09_CONTAINER_VEL;
    extern float HOCKEY_PRM_S09_CONTAINER_INTERVAL;
    extern long HOCKEY_PRM_S09_CONTAINER_BOX_TYPE;
    extern float HOCKEY_PRM_S09_CONTAINER_LIFE;
    extern float HOCKEY_PRM_S09_CONTAINER_OFFSET_X;
    extern float HOCKEY_PRM_S09_BOMB_SIZE;
    extern float HOCKEY_PRM_S09_BOMB_EXPLODE_SIZE;
    extern float HOCKEY_PRM_S09_BOMB_EXPLODE_EFF_SCALE;
    extern float HOCKEY_PRM_S09_BOMB_EXPLODE_TIME;
    extern float HOCKEY_PRM_S09_TREASURE_WIDTH;
    extern float HOCKEY_PRM_S09_TREASURE_HEIGHT;
    extern long HOCKEY_PRM_S09_TREASURE_COIN_NUM;
    extern float HOCKEY_PRM_S09_EV_INTERVAL_RND; // in .bss
    extern long HOCKEY_PRM_S09_CV_DIR_0; // in .bss
    extern float HOCKEY_PRM_S09_CONTAINER_DEC; // in .bss

    // Stage 17, the turntable and trolley stage (CStageHockey17). Times are
    // in seconds, speeds in degrees per second, rates in percent.
    extern float HOCKEY_PRM_S17_TT_INIT_STOP_TIME;
    extern float HOCKEY_PRM_S17_TT_STOP_TIME;
    extern float HOCKEY_PRM_S17_TT_ROT_TIME;
    extern float HOCKEY_PRM_S17_TT_SPEED;
    extern long HOCKEY_PRM_S17_TT_DIR;
    extern float HOCKEY_PRM_S17_TT_ACC_TIME;
    extern float HOCKEY_PRM_S17_TT_RADIUS;
    extern float HOCKEY_PRM_S17_TT_OPEN_RATE;
    extern float HOCKEY_PRM_S17_TT_WARN_TIME;
    extern float HOCKEY_PRM_S17_TT_WARN_AFTER_TIME;
    extern float HOCKEY_PRM_S17_TT_WARN_SE_STOP_TIME;
    extern float HOCKEY_PRM_S17_TROLLEY_RATE;
    extern float HOCKEY_PRM_S17_TROLLEY_PRE_WAIT;
    extern float HOCKEY_PRM_S17_TROLLEY_SPEED;
    extern long HOCKEY_PRM_S17_TROLLEY_NUM_MIN;
    extern long HOCKEY_PRM_S17_TROLLEY_NUM_MAX;
    extern float HOCKEY_PRM_S17_TROLLEY_LOC_OFFSET;
    extern float HOCKEY_PRM_S17_TROLLEY_WIDTH;
    extern float HOCKEY_PRM_S17_TROLLEY_DEPTH;
    extern float HOCKEY_PRM_S17_TROLLEY_HEIGHT;
    extern float HOCKEY_PRM_S17_TROLLEY_ATK_WIDTH;
    extern float HOCKEY_PRM_S17_TROLLEY_ATK_DEPTH;
    extern float HOCKEY_PRM_S17_TROLLEY_ATK_HEIGHT;
    extern float HOCKEY_PRM_S17_TROLLEY_MDL_WIDTH;
    extern float HOCKEY_PRM_S17_TROLLEY_MDL_DEPTH;
    extern float HOCKEY_PRM_S17_TROLLEY_MDL_HEIGHT;
    extern float HOCKEY_PRM_S17_HEYHO_COIN_RATE;
    extern float HOCKEY_PRM_S17_HEYHO_B_DIST_MIN;
    extern float HOCKEY_PRM_S17_HEYHO_B_DIST_MAX;
    extern float HOCKEY_PRM_S17_HEYHO_C_DIST_MIN;
    extern float HOCKEY_PRM_S17_HEYHO_C_DIST_MAX;
    extern float HOCKEY_PRM_S17_GOAL_X1_LENGTH;
    extern float HOCKEY_PRM_S17_GOAL_RADIUS;
    extern float HOCKEY_PRM_S17_TT_STOP_TIME_RND; // in .bss
    extern float HOCKEY_PRM_S17_TT_ROT_TIME_RND; // in .bss
    extern float HOCKEY_PRM_S17_TROLLEY_INIT_RATE; // in .bss
    extern long HOCKEY_PRM_S17_TROLLEY_DIR_TYPE; // in .bss
    extern long HOCKEY_PRM_S17_TROLLEY_PAT_TYPE; // in .bss
    extern long HOCKEY_PRM_S17_TROLLEY_PAT_NO; // in .bss
    extern long HOCKEY_PRM_S17_DEB_RELOAD; // in .bss
}

#endif /* !MSW_HOCKEYPRM_HPP_ */
