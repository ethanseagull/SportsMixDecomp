#ifndef MSW_BASKETPRM_HPP_
#define MSW_BASKETPRM_HPP_

// Tuning values of the basketball mode (BasketF), globals in namespace msw
// (BASKET_PRM_*__3msw). Only the ones used by source written so far are
// declared; the types are those of the loads (lwz: long, lfs: float).
namespace msw {

    // Stage 02, the wave stage. Times are in seconds (the cluster turns them
    // into frames), positions in world units.
    extern float BASKET_PRM_S02_WAVE_PART_INTERVAL;
    extern float BASKET_PRM_S02_WAVE_BIG_INTERVAL;
    extern float BASKET_PRM_S02_WAVE_BIG_INTERVAL_RND;
    extern float BASKET_PRM_S02_WAVE_ANIM_TIME;
    extern float BASKET_PRM_S02_WAVE_BIG_ANIM_TIME;
    extern float BASKET_PRM_S02_WAVE_OFFSET_X;
    extern float BASKET_PRM_S02_WAVE_BIG_OFFSET_X;
    extern long BASKET_PRM_S02_COIN_POP_NUM;
    extern long BASKET_PRM_S02_BIG_COIN_POP_NUM;
    extern long BASKET_PRM_S02_ITEM_POP_CYCLE;
    extern long BASKET_PRM_S02_ITEM_POP_NUM;
    extern float BASKET_PRM_S02_ITEM_POP_DELAY;
    extern float BASKET_PRM_S02_ITEM_POP_COME_FRM;
    extern float BASKET_PRM_S02_ITEM_POP_DIST;
    extern float BASKET_PRM_S02_ITEM_POP_POS_X;
    extern long BASKET_PRM_S02_BIG_ITEM_POP_CYCLE;
    extern long BASKET_PRM_S02_BIG_ITEM_POP_NUM;
    extern float BASKET_PRM_S02_BIG_ITEM_POP_DELAY;
    extern float BASKET_PRM_S02_BIG_ITEM_POP_COME_FRM;
    extern float BASKET_PRM_S02_BIG_ITEM_POP_DIST;
    extern float BASKET_PRM_S02_BIG_ITEM_POP_POS_X;

    // Stage 06, the lit stage, listed in CStageBasket06::GetPrmTbl.
    extern float BASKET_PRM_S06_MOVE_SPEED;
    extern float BASKET_PRM_S06_MOVE_SPEED_SHOOT_CATCH;
    extern float BASKET_PRM_S06_MOVE_ROTATE_RATE;
    extern float BASKET_PRM_S06_MOVE_HEIGHT;
    extern float BASKET_PRM_S06_MOVE_Y_SP_MIN;
    extern float BASKET_PRM_S06_MOVE_Y_SP_MAX;
    extern long BASKET_PRM_S06_BALL_HOLD_MOVE;
    extern float BASKET_PRM_S06_CATCH1_COL_R;
    extern float BASKET_PRM_S06_CATCH1_COL_HEIGHT;
    extern float BASKET_PRM_S06_CATCH2_COL_RADIUS;
    extern float BASKET_PRM_S06_PUSH1_COL_SIZE_R;
    extern float BASKET_PRM_S06_PUSH1_COL_SIZE_HEIGHT;
    extern float BASKET_PRM_S06_DAMAGE_COLL_BALL_R;
    extern long BASKET_PRM_S06_DAMAGE_COLL_TYPE;
    extern float BASKET_PRM_S06_BW_ALPHA_RATE;
    extern float BASKET_PRM_S06_ALPHA_TEST_NEAR_RANG;
    extern float BASKET_PRM_S06_ALPHA_TEST_FAR_RANG;
    extern long BASKET_PRM_S06_PASS_CALL_WAIT_MINUTES;
    extern long BASKET_PRM_S06_PASS_RANDUM_PARCENT;
    extern long BASKET_PRM_S06_PASS_MISS_TO_HAPPY_COUNT;
    extern float BASKET_PRM_S06_LOOSE_BALL_GET_DIST;
    extern long BASKET_PRM_S06_CUT_APPEND_MINUTES;
    extern float BASKET_PRM_S06_RIGHT_OFF_MOVE_R1;
    extern float BASKET_PRM_S06_RIGHT_OFF_MOVE_R2;
    extern float BASKET_PRM_S06_RIGHT_OFF_MOVE_R1_HEIGHT;
    extern long BASKET_PRM_S06_LIGHT_SWITCH_CNT;
    extern float BASKET_PRM_S06_LIGHT_ON_TIME; // seconds
    extern long BASKET_PRM_S06_FLASH_COLOR_R;
    extern long BASKET_PRM_S06_FLASH_COLOR_G;
    extern long BASKET_PRM_S06_FLASH_COLOR_B;
    extern long BASKET_PRM_S06_FLASH_COLOR_A;
    extern float BASKET_PRM_S06_FLASH_TIME;
    extern float BASKET_PRM_S06_FLASH_GAIN;
    extern float BASKET_PRM_S06_FLASH_DECAY;
    extern long BASKET_PRM_S06_DBG_MAIN_PROC_ENABLE;
    extern long BASKET_PRM_S06_DBG_TARGET_DISP;
    extern long BASKET_PRM_S06_PUSH1_COL_DBG_ENABLE;

    // Hungry Boss Packun stage 70: the boss's tables, listed in
    // CStageBasketHungryBossPackun70::GetPrmTbl. (meaning not known)
    extern long BASKET_PRM_S70_STATE0_TBL0_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL0;
    extern long BASKET_PRM_S70_STATE0_TBL1_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL1;
    extern long BASKET_PRM_S70_STATE0_TBL2_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL2;
    extern long BASKET_PRM_S70_STATE0_TBL3_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL3;
    extern long BASKET_PRM_S70_STATE0_TBL4_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL4;
    extern long BASKET_PRM_S70_STATE0_TBL5_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL5;
    extern long BASKET_PRM_S70_STATE0_TBL6_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL6;
    extern long BASKET_PRM_S70_STATE0_TBL7_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL7;
    extern long BASKET_PRM_S70_STATE0_TBL8_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL8;
    extern long BASKET_PRM_S70_STATE0_TBL9_IDX;
    extern long BASKET_PRM_S70_STATE0_TBL9;
    extern long BASKET_PRM_S70_STATE1_TBL0_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL0;
    extern long BASKET_PRM_S70_STATE1_TBL1_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL1;
    extern long BASKET_PRM_S70_STATE1_TBL2_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL2;
    extern long BASKET_PRM_S70_STATE1_TBL3_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL3;
    extern long BASKET_PRM_S70_STATE1_TBL4_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL4;
    extern long BASKET_PRM_S70_STATE1_TBL5_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL5;
    extern long BASKET_PRM_S70_STATE1_TBL6_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL6;
    extern long BASKET_PRM_S70_STATE1_TBL7_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL7;
    extern long BASKET_PRM_S70_STATE1_TBL8_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL8;
    extern long BASKET_PRM_S70_STATE1_TBL9_IDX;
    extern long BASKET_PRM_S70_STATE1_TBL9;
    extern long BASKET_PRM_S70_STATE2_TBL0_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL0;
    extern long BASKET_PRM_S70_STATE2_TBL1_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL1;
    extern long BASKET_PRM_S70_STATE2_TBL2_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL2;
    extern long BASKET_PRM_S70_STATE2_TBL3_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL3;
    extern long BASKET_PRM_S70_STATE2_TBL4_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL4;
    extern long BASKET_PRM_S70_STATE2_TBL5_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL5;
    extern long BASKET_PRM_S70_STATE2_TBL6_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL6;
    extern long BASKET_PRM_S70_STATE2_TBL7_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL7;
    extern long BASKET_PRM_S70_STATE2_TBL8_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL8;
    extern long BASKET_PRM_S70_STATE2_TBL9_IDX;
    extern long BASKET_PRM_S70_STATE2_TBL9;
    extern long BASKET_PRM_S70_DELIVERY_PATTERN;

    // Hungry Boss Packun stage 71: the boss's tables (as for stage 70),
    // then for each of the three party game settings the floors that rise and
    // sink (CStageBasketHungryBossPackun71). Times are in seconds.
    extern long BASKET_PRM_S71_STATE0_TBL0_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL0;
    extern long BASKET_PRM_S71_STATE0_TBL1_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL1;
    extern long BASKET_PRM_S71_STATE0_TBL2_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL2;
    extern long BASKET_PRM_S71_STATE0_TBL3_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL3;
    extern long BASKET_PRM_S71_STATE0_TBL4_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL4;
    extern long BASKET_PRM_S71_STATE0_TBL5_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL5;
    extern long BASKET_PRM_S71_STATE0_TBL6_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL6;
    extern long BASKET_PRM_S71_STATE0_TBL7_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL7;
    extern long BASKET_PRM_S71_STATE0_TBL8_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL8;
    extern long BASKET_PRM_S71_STATE0_TBL9_IDX;
    extern long BASKET_PRM_S71_STATE0_TBL9;
    extern long BASKET_PRM_S71_STATE1_TBL0_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL0;
    extern long BASKET_PRM_S71_STATE1_TBL1_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL1;
    extern long BASKET_PRM_S71_STATE1_TBL2_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL2;
    extern long BASKET_PRM_S71_STATE1_TBL3_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL3;
    extern long BASKET_PRM_S71_STATE1_TBL4_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL4;
    extern long BASKET_PRM_S71_STATE1_TBL5_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL5;
    extern long BASKET_PRM_S71_STATE1_TBL6_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL6;
    extern long BASKET_PRM_S71_STATE1_TBL7_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL7;
    extern long BASKET_PRM_S71_STATE1_TBL8_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL8;
    extern long BASKET_PRM_S71_STATE1_TBL9_IDX;
    extern long BASKET_PRM_S71_STATE1_TBL9;
    extern long BASKET_PRM_S71_STATE2_TBL0_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL0;
    extern long BASKET_PRM_S71_STATE2_TBL1_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL1;
    extern long BASKET_PRM_S71_STATE2_TBL2_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL2;
    extern long BASKET_PRM_S71_STATE2_TBL3_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL3;
    extern long BASKET_PRM_S71_STATE2_TBL4_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL4;
    extern long BASKET_PRM_S71_STATE2_TBL5_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL5;
    extern long BASKET_PRM_S71_STATE2_TBL6_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL6;
    extern long BASKET_PRM_S71_STATE2_TBL7_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL7;
    extern long BASKET_PRM_S71_STATE2_TBL8_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL8;
    extern long BASKET_PRM_S71_STATE2_TBL9_IDX;
    extern long BASKET_PRM_S71_STATE2_TBL9;
    extern float BASKET_PRM_S71_FLOOR0_START_TIME_SEC;
    extern long BASKET_PRM_S71_FLOOR0_START_MOVE_DIR;
    extern float BASKET_PRM_S71_FLOOR0_FLOAT_DEPTH;
    extern float BASKET_PRM_S71_FLOOR0_DEPTH_CYCLE;
    extern float BASKET_PRM_S71_FLOOR0_FLOAT_STOP_TIME;
    extern float BASKET_PRM_S71_FLOOR1_START_TIME_SEC;
    extern long BASKET_PRM_S71_FLOOR1_START_MOVE_DIR;
    extern float BASKET_PRM_S71_FLOOR1_FLOAT_DEPTH;
    extern float BASKET_PRM_S71_FLOOR1_DEPTH_CYCLE;
    extern float BASKET_PRM_S71_FLOOR1_FLOAT_STOP_TIME;
    extern float BASKET_PRM_S71_FLOOR2_START_TIME_SEC;
    extern long BASKET_PRM_S71_FLOOR2_START_MOVE_DIR;
    extern float BASKET_PRM_S71_FLOOR2_FLOAT_DEPTH;
    extern float BASKET_PRM_S71_FLOOR2_DEPTH_CYCLE;
    extern float BASKET_PRM_S71_FLOOR2_FLOAT_STOP_TIME;
    extern long BASKET_PRM_S71_DELIVERY_PATTERN;

    // Hungry Boss Packun stage 72: the boss's tables, then for each of the
    // three party game settings the conveyor belt
    // (CStageBasketHungryBossPackun72). Times are in seconds.
    extern long BASKET_PRM_S72_STATE0_TBL0_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL0;
    extern long BASKET_PRM_S72_STATE0_TBL1_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL1;
    extern long BASKET_PRM_S72_STATE0_TBL2_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL2;
    extern long BASKET_PRM_S72_STATE0_TBL3_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL3;
    extern long BASKET_PRM_S72_STATE0_TBL4_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL4;
    extern long BASKET_PRM_S72_STATE0_TBL5_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL5;
    extern long BASKET_PRM_S72_STATE0_TBL6_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL6;
    extern long BASKET_PRM_S72_STATE0_TBL7_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL7;
    extern long BASKET_PRM_S72_STATE0_TBL8_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL8;
    extern long BASKET_PRM_S72_STATE0_TBL9_IDX;
    extern long BASKET_PRM_S72_STATE0_TBL9;
    extern long BASKET_PRM_S72_STATE1_TBL0_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL0;
    extern long BASKET_PRM_S72_STATE1_TBL1_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL1;
    extern long BASKET_PRM_S72_STATE1_TBL2_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL2;
    extern long BASKET_PRM_S72_STATE1_TBL3_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL3;
    extern long BASKET_PRM_S72_STATE1_TBL4_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL4;
    extern long BASKET_PRM_S72_STATE1_TBL5_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL5;
    extern long BASKET_PRM_S72_STATE1_TBL6_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL6;
    extern long BASKET_PRM_S72_STATE1_TBL7_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL7;
    extern long BASKET_PRM_S72_STATE1_TBL8_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL8;
    extern long BASKET_PRM_S72_STATE1_TBL9_IDX;
    extern long BASKET_PRM_S72_STATE1_TBL9;
    extern long BASKET_PRM_S72_STATE2_TBL0_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL0;
    extern long BASKET_PRM_S72_STATE2_TBL1_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL1;
    extern long BASKET_PRM_S72_STATE2_TBL2_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL2;
    extern long BASKET_PRM_S72_STATE2_TBL3_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL3;
    extern long BASKET_PRM_S72_STATE2_TBL4_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL4;
    extern long BASKET_PRM_S72_STATE2_TBL5_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL5;
    extern long BASKET_PRM_S72_STATE2_TBL6_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL6;
    extern long BASKET_PRM_S72_STATE2_TBL7_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL7;
    extern long BASKET_PRM_S72_STATE2_TBL8_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL8;
    extern long BASKET_PRM_S72_STATE2_TBL9_IDX;
    extern long BASKET_PRM_S72_STATE2_TBL9;
    extern float BASKET_PRM_S72_CV0_INTERVAL;
    extern float BASKET_PRM_S72_CV0_INTERVAL_RND;
    extern float BASKET_PRM_S72_CV0_STOP_TIME;
    extern float BASKET_PRM_S72_CV0_SPEED;
    extern long BASKET_PRM_S72_CV0_DIR;
    extern float BASKET_PRM_S72_CV1_INTERVAL;
    extern float BASKET_PRM_S72_CV1_INTERVAL_RND;
    extern float BASKET_PRM_S72_CV1_STOP_TIME;
    extern float BASKET_PRM_S72_CV1_SPEED;
    extern long BASKET_PRM_S72_CV1_DIR;
    extern float BASKET_PRM_S72_CV2_INTERVAL;
    extern float BASKET_PRM_S72_CV2_INTERVAL_RND;
    extern float BASKET_PRM_S72_CV2_STOP_TIME;
    extern float BASKET_PRM_S72_CV2_SPEED;
    extern long BASKET_PRM_S72_CV2_DIR;
    extern long BASKET_PRM_S72_DELIVERY_PATTERN;

    // The Hungry Boss Packun stages: the size of the court.
    extern float BASKET_PRM_BOSS_PACKUN_COURT_WIDTH;
    extern float BASKET_PRM_BOSS_PACKUN_COURT_DEPTH;

    // The fruit balls (msw::CBallBasketFruit). Fruit suffixes: OR orange,
    // BA blue apple, RA red apple, SU watermelon (suica), ME melon (guessed).
    // Lives in frames: how long a fruit hanging on a string or held by a
    // character lasts once loose.
    extern long BASKET_PRM_COMMON_HUNGRY_STRING_BALL_LIFE_MIN;
    extern long BASKET_PRM_COMMON_HUNGRY_STRING_BALL_LIFE_MAX;
    extern long BASKET_PRM_COMMON_HUNGRY_CHR_BALL_LIFE_MIN;
    extern long BASKET_PRM_COMMON_HUNGRY_CHR_BALL_LIFE_MAX;
    extern float BASKET_PRM_S70_SACLE_ORANGE;
    extern float BASKET_PRM_S70_SACLE_BLUE_APPLE;
    extern float BASKET_PRM_S70_SACLE_RED_APPLE;
    extern float BASKET_PRM_S70_SACLE_SUICA;
    extern float BASKET_PRM_S70_SACLE_BOMB;
    extern float BASKET_PRM_S70_GRAV_VALUE_BOMB;
    extern float BASKET_PRM_S70_GRAV_VALUE_EXT;
    extern float BASKET_PRM_S70_BOUND_RATE_Y_OR;
    extern float BASKET_PRM_S70_BOUND_RATE_Y_BA;
    extern float BASKET_PRM_S70_BOUND_RATE_Y_RA;
    extern float BASKET_PRM_S70_BOUND_RATE_Y_SU;
    extern float BASKET_PRM_S70_BOUND_RATE_Y_ME;
    extern float BASKET_PRM_S70_BOUND_RATE_XZ_OR;
    extern float BASKET_PRM_S70_BOUND_RATE_XZ_BA;
    extern float BASKET_PRM_S70_BOUND_RATE_XZ_RA;
    extern float BASKET_PRM_S70_BOUND_RATE_XZ_SU;
    extern float BASKET_PRM_S70_BOUND_RATE_XZ_ME;
    extern float BASKET_PRM_S70_FIRST_BOUND_MIN_VEL_X;
    extern float BASKET_PRM_S70_FIRST_BOUND_MAX_VEL_X;
    extern float BASKET_PRM_S70_JS_MIN_ST_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MIN_ST_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MIN_ST_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MIN_ST_FRAME_SU;
    extern float BASKET_PRM_S70_JS_MIN_ED_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MIN_ED_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MIN_ED_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MIN_ED_FRAME_SU;
    extern float BASKET_PRM_S70_JS_MID_ST_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MID_ST_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MID_ST_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MID_ST_FRAME_SU;
    extern float BASKET_PRM_S70_JS_MID_ED_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MID_ED_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MID_ED_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MID_ED_FRAME_SU;
    extern float BASKET_PRM_S70_JS_MAX_ST_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MAX_ST_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MAX_ST_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MAX_ST_FRAME_SU;
    extern float BASKET_PRM_S70_JS_MAX_ED_FRAME_OR;
    extern float BASKET_PRM_S70_JS_MAX_ED_FRAME_BA;
    extern float BASKET_PRM_S70_JS_MAX_ED_FRAME_RA;
    extern float BASKET_PRM_S70_JS_MAX_ED_FRAME_SU;
    extern long BASKET_PRM_HB_BOMB_DOWN_FRM;
    extern float BASKET_PRM_HB_BOMB_EXPLODE_EFF_SCALE;

    // Stage 70 (the boss Piranha Plant), "just shot" timing: the three
    // shot distances the ball's timing frames are given for.
    extern float BASKET_PRM_S70_JS_MIN_DIST;
    extern float BASKET_PRM_S70_JS_MID_DIST;
    extern float BASKET_PRM_S70_JS_MAX_DIST;
}

#endif /* !MSW_BASKETPRM_HPP_ */
