#ifndef MSW_VOLLEYPRM_HPP_
#define MSW_VOLLEYPRM_HPP_

// Tuning values of the volleyball mode (VolleyF), globals in namespace msw
// (VOLLEY_PRM_*__3msw). Only the ones used by source written so far are
// declared; the types are those of the loads (lwz: long, lfs: float).
namespace msw {

    // Stage 02, the wave stage (CStgGmk_Wave_Volley). Times are in seconds.
    extern float VOLLEY_PRM_S02_WAVE_PART_INTERVAL;
    extern float VOLLEY_PRM_S02_WAVE_BIG_INTERVAL;
    extern float VOLLEY_PRM_S02_WAVE_ANIM_TIME;
    extern float VOLLEY_PRM_S02_WAVE_BIG_ANIM_TIME;
    extern float VOLLEY_PRM_S02_WAVE_OFFSET_X;
    extern float VOLLEY_PRM_S02_WAVE_PUSH_POWER;
    extern float VOLLEY_PRM_S02_WAVE_PULL_POWER;
    extern float VOLLEY_PRM_S02_WAVE_POS_MARGIN;
    extern long VOLLEY_PRM_S02_BIG_COIN_POP_NUM;
    extern long VOLLEY_PRM_S02_ITEM_POP_CYCLE;
    extern long VOLLEY_PRM_S02_ITEM_POP_NUM;
    extern float VOLLEY_PRM_S02_ITEM_POP_DELAY;
    extern float VOLLEY_PRM_S02_ITEM_POP_COME_FRM;
    extern float VOLLEY_PRM_S02_ITEM_POP_DIST;
    extern float VOLLEY_PRM_S02_ITEM_POP_POS_X;
    extern long VOLLEY_PRM_S02_BIG_ITEM_POP_CYCLE;
    extern long VOLLEY_PRM_S02_BIG_ITEM_POP_NUM;
    extern float VOLLEY_PRM_S02_BIG_ITEM_POP_DELAY;
    extern float VOLLEY_PRM_S02_BIG_ITEM_POP_COME_FRM;
    extern float VOLLEY_PRM_S02_BIG_ITEM_POP_DIST;
    extern float VOLLEY_PRM_S02_BIG_ITEM_POP_POS_X;
    extern float VOLLEY_PRM_S02_WAVE_BIG_INTERVAL_RND;
    extern float VOLLEY_PRM_S02_WAVE_BIG_OFFSET_X;
    extern long VOLLEY_PRM_S02_COIN_POP_NUM;
    extern long VOLLEY_PRM_S06_LIGHT_SWITCH_CNT;
    extern float VOLLEY_PRM_S06_LIGHT_ON_TIME;
    extern long VOLLEY_PRM_S06_FLASH_COLOR_R;
    extern long VOLLEY_PRM_S06_FLASH_COLOR_G;
    extern long VOLLEY_PRM_S06_FLASH_COLOR_B;
    extern long VOLLEY_PRM_S06_FLASH_COLOR_A;
    extern float VOLLEY_PRM_S06_FLASH_TIME;
    extern float VOLLEY_PRM_S06_FLASH_GAIN;
    extern float VOLLEY_PRM_S06_FLASH_DECAY;
    extern float VOLLEY_PRM_S16_DROP_AREA_Z;
    extern long VOLLEY_PRM_S16_MAX_NUM;
    extern float VOLLEY_PRM_S16_POP_TIME;
    extern float VOLLEY_PRM_S16_POP_TIME_RND;
    extern float VOLLEY_PRM_S16_POP_POS_Y;
    extern float VOLLEY_PRM_S16_DROP_SPEED;
    extern float VOLLEY_PRM_S16_DROP_ANGLE;
    extern long VOLLEY_PRM_S16_BOUND_NUM;
    extern float VOLLEY_PRM_S16_BOUND_REDUCE_Y;
    extern float VOLLEY_PRM_S16_BOUND_REDUCE_XZ;
    extern long VOLLEY_PRM_S16_ADD_COIN_0;
    extern float VOLLEY_PRM_S16_WARP_INTERVAL;
    extern float VOLLEY_PRM_S16_WARP_TIME;
    extern float VOLLEY_PRM_S16_WARP_SPEED;
    extern float VOLLEY_PRM_S16_WARP_BLUR_SHIFT;
    extern long VOLLEY_PRM_S16_FLASH_COLOR_R;
    extern long VOLLEY_PRM_S16_FLASH_COLOR_G;
    extern long VOLLEY_PRM_S16_FLASH_COLOR_B;
    extern long VOLLEY_PRM_S16_FLASH_COLOR_A;
    extern float VOLLEY_PRM_S16_FLASH_TIME;
    extern float VOLLEY_PRM_S16_FLASH_GAIN;
    extern float VOLLEY_PRM_S16_FLASH_DECAY;
    extern float VOLLEY_PRM_S12_BALL_SIZE;
    extern float VOLLEY_PRM_S12_BALL_COLLSIZE;
    extern float VOLLEY_PRM_S12_BALL_SPEED;
    extern float VOLLEY_PRM_S12_BALL_SPEED_LIMIT;
    extern float VOLLEY_PRM_S12_BALL_KICK_POW;
    extern float VOLLEY_PRM_S12_BALL_BALL_KICK_POW;
    extern float VOLLEY_PRM_S12_BALL_DECEL;
    extern float VOLLEY_PRM_S12_BALL_GUIDE_DECAY;
    extern float VOLLEY_PRM_S12_BALL_FORCE_ACC;
    extern float VOLLEY_PRM_S12_BALL_INIT_SLEEP;
    extern float VOLLEY_PRM_S12_BALL_INTERVAL;
    extern float VOLLEY_PRM_S12_BALL_ANGLE;
    extern float VOLLEY_PRM_S12_BALL_SIDE_RATE;
    extern long VOLLEY_PRM_S12_BALL_NUM;
    extern float VOLLEY_PRM_S12_BALL_DOWN_TIME;
    extern float VOLLEY_PRM_S15_CASE_WIDTH;
    extern float VOLLEY_PRM_S15_CASE_DEPTH;
    extern float VOLLEY_PRM_S15_CASE_HEIGHT;
    extern float VOLLEY_PRM_S15_CASE_INTERVAL;
    extern float VOLLEY_PRM_S15_CASE_OFFSET_Z;
    extern float VOLLEY_PRM_S15_CASE_SPACING_Z;
    extern float VOLLEY_PRM_S15_CASE_POS_Y;
    extern float VOLLEY_PRM_S15_CASE_ACC_0;
    extern float VOLLEY_PRM_S15_CASE_DEC_0;
    extern float VOLLEY_PRM_S15_CASE_DEC_RND_0;
    extern float VOLLEY_PRM_S15_CASE_VEL_0;
    extern float VOLLEY_PRM_S15_CASE_ACC_1;
    extern float VOLLEY_PRM_S15_CASE_DEC_1;
    extern float VOLLEY_PRM_S15_CASE_DEC_RND_1;
    extern float VOLLEY_PRM_S15_CASE_VEL_1;
    extern float VOLLEY_PRM_S15_CASE_ACC_2;
    extern float VOLLEY_PRM_S15_CASE_DEC_2;
    extern float VOLLEY_PRM_S15_CASE_DEC_RND_2;
    extern float VOLLEY_PRM_S15_CASE_VEL_2;
    extern float VOLLEY_PRM_S15_RAINY_INTERVAL;
    extern float VOLLEY_PRM_S15_RAINY_TIME;
    extern float VOLLEY_PRM_S15_SHIP_CYCLE;
    extern float VOLLEY_PRM_S15_SHIP_ROT_ANGLE;
    extern float VOLLEY_PRM_S15_SHIP_STOP_TIME;
    extern float VOLLEY_PRM_S15_SHIP_STOP_TIME_RND;
    extern float VOLLEY_PRM_S15_CHR_FLOW_VEL;
    extern long VOLLEY_PRM_S15_FLASH_COLOR_R;
    extern long VOLLEY_PRM_S15_FLASH_COLOR_G;
    extern long VOLLEY_PRM_S15_FLASH_COLOR_B;
    extern long VOLLEY_PRM_S15_FLASH_COLOR_A;
    extern float VOLLEY_PRM_S15_FLASH_TIME;
    extern float VOLLEY_PRM_S15_FLASH_GAIN;
    extern float VOLLEY_PRM_S15_FLASH_DECAY;
    extern float VOLLEY_PRM_S15_KARON_SCALE;
    extern float VOLLEY_PRM_S15_KARON_COLL_SCALE;
    extern float VOLLEY_PRM_S15_KARON_SPEED;
    extern float VOLLEY_PRM_S09_CV_SPEED;
    extern float VOLLEY_PRM_S09_CV_ACC_SPEED;
    extern float VOLLEY_PRM_S09_CV_STOP_TIME;
    extern float VOLLEY_PRM_S09_EV_INTERVAL; // float in the parameter table
    extern float VOLLEY_PRM_S09_CV_ACC_TIME;
    extern float VOLLEY_PRM_S09_CV_PRE_ACC_TIME;
    extern long VOLLEY_PRM_S09_CV_FORCE_MOVE; // long in the parameter table
    extern float VOLLEY_PRM_S03_WATER_HEIGHT;
    extern float VOLLEY_PRM_S03_WATER_PRESTART_TIME;
    extern float VOLLEY_PRM_S03_WATER_STAY_TIME;
    extern float VOLLEY_PRM_S03_WATER_INTERVAL;
    extern float VOLLEY_PRM_S03_WATER_SPEED;
    extern long VOLLEY_PRM_S03_WATER_NUM;
    extern long VOLLEY_PRM_S03_WATER_PATTERN;
    extern long VOLLEY_PRM_S03_PAT0_NUM;
    extern long VOLLEY_PRM_S03_PAT0B_NUM;
    extern long VOLLEY_PRM_S03_PAT1_NUM;
    extern float VOLLEY_PRM_S03_PAT2_SPEED;
    extern float VOLLEY_PRM_S03_PAT2_UNIT_STAY_TIME;
    extern float VOLLEY_PRM_S03_PAT2_PRESTART_TIME;
    extern long VOLLEY_PRM_S03_TEST_ALWAYS; // long in the parameter table
}

#endif /* !MSW_VOLLEYPRM_HPP_ */
