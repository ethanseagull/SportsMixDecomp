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
}

#endif /* !MSW_HOCKEYPRM_HPP_ */
