#ifndef MSW_DODGEPRM_HPP_
#define MSW_DODGEPRM_HPP_

// Tuning values of the dodgeball mode (DodgeF), globals in namespace msw
// (DODGE_PRM_*__3msw). Only the ones used by source written so far are
// declared; the types are those of the loads (lwz: long, lfs: float).
namespace msw {

    // Stage 02, the wave stage. Times are in seconds (the cluster turns them
    // into frames), positions in world units.
    extern float DODGE_PRM_S02_WAVE_PART_INTERVAL;
    extern float DODGE_PRM_S02_WAVE_BIG_INTERVAL;
    extern float DODGE_PRM_S02_WAVE_BIG_INTERVAL_RND;
    extern float DODGE_PRM_S02_WAVE_ANIM_TIME;
    extern float DODGE_PRM_S02_WAVE_BIG_ANIM_TIME;
    extern float DODGE_PRM_S02_WAVE_OFFSET_X;
    extern float DODGE_PRM_S02_WAVE_BIG_OFFSET_X;
    extern long DODGE_PRM_S02_COIN_POP_NUM;
    extern long DODGE_PRM_S02_BIG_COIN_POP_NUM;
    extern long DODGE_PRM_S02_ITEM_POP_CYCLE;
    extern long DODGE_PRM_S02_ITEM_POP_NUM;
    extern float DODGE_PRM_S02_ITEM_POP_DELAY;
    extern float DODGE_PRM_S02_ITEM_POP_COME_FRM;
    extern float DODGE_PRM_S02_ITEM_POP_DIST;
    extern float DODGE_PRM_S02_ITEM_POP_POS_X;
    extern long DODGE_PRM_S02_BIG_ITEM_POP_CYCLE;
    extern long DODGE_PRM_S02_BIG_ITEM_POP_NUM;
    extern float DODGE_PRM_S02_BIG_ITEM_POP_DELAY;
    extern float DODGE_PRM_S02_BIG_ITEM_POP_COME_FRM;
    extern float DODGE_PRM_S02_BIG_ITEM_POP_DIST;
    extern float DODGE_PRM_S02_BIG_ITEM_POP_POS_X;

    // Stage 04, the pylon stage (the pylon models, actors and cluster).
    extern float DODGE_PRM_S04_PYLON_MDL_RADIUS;
    extern float DODGE_PRM_S04_PYLON_MDL_HEIGHT;
    extern float DODGE_PRM_S04_PYLON_RADIUS;
    extern float DODGE_PRM_S04_PYLON_HEIGHT;
    extern float DODGE_PRM_S04_PYLON_LIFE;
    extern float DODGE_PRM_S04_PYLON_FIELD_MARGIN;
    extern long DODGE_PRM_S04_PYLON_PUSH_POWER;
    extern float DODGE_PRM_S04_PYLON_PUSH_RESIST;
    extern float DODGE_PRM_S04_PYLON_ACC;
    extern float DODGE_PRM_S04_PYLON_DEC;
    extern long DODGE_PRM_S04_PYLON_LOC_PAT_TYPE;
    extern long DODGE_PRM_S04_PYLON_LOC_PAT_NO;

    // Stage 05, the floating rafts (CStageDodge05::GetPrmTbl).
    extern float DODGE_PRM_S05_START_TIME_SEC;
    extern float DODGE_PRM_S05_FLOAT_DEPTH;
    extern float DODGE_PRM_S05_DEPTH_CYCLE;
    extern float DODGE_PRM_S05_FLOAT_STOP_TIME;
    extern float DODGE_PRM_S05_HEAVY_TIME_SEC;
    extern float DODGE_PRM_S05_HEAVY_FLOAT_DEPTH;
    extern float DODGE_PRM_S05_HEAVY_DEPTH_CYCLE;
    extern float DODGE_PRM_S05_HEAVY_FLOAT_STOP_TIME;
    extern float DODGE_PRM_S05_HEAVY_ACC_TIME_SEC;
    extern float DODGE_PRM_S05_TURRET_EARLY_HIT;
    extern float DODGE_PRM_S05_ENV_SE_TIME;
    extern float DODGE_PRM_S05_ENV_SE_TIME_RND;

    // Stage 07, the boss Packun (CStageDodge07::GetPrmTbl).
    extern float DODGE_PRM_S07_PACKUN_CLOSE_STANDBY_TIME_RND;
    extern float DODGE_PRM_S07_PACKUN_OPEN_STANDBY_TIME_RND;
    extern float DODGE_PRM_S07_PACKUN_OPEN_PASSCATCH_FRM_RND;
    extern float DODGE_PRM_S07_PACKUN_BLT_TIME_RND;
    extern float DODGE_PRM_S07_PACKUN_SCALE;
    extern float DODGE_PRM_S07_PACKUN_COLL_SIZE_XZ;
    extern float DODGE_PRM_S07_PACKUN_COLL_SIZE_Y;
    extern float DODGE_PRM_S07_PACKUN_TURN_SPEED;
    extern float DODGE_PRM_S07_PACKUN_TGT_TURN_SPEED;
    extern float DODGE_PRM_S07_PACKUN_CLOSE_STANDBY_TIME;
    extern float DODGE_PRM_S07_PACKUN_CLOSE_PASSCATCH_FRM;
    extern float DODGE_PRM_S07_PACKUN_OPEN_STANDBY_TIME;
    extern float DODGE_PRM_S07_PACKUN_OPEN_PASSCATCH_FRM;
    extern float DODGE_PRM_S07_PACKUN_SHOOT_N_VEL;
    extern float DODGE_PRM_S07_PACKUN_SHOOT_TRACE_ANGLE;
    extern float DODGE_PRM_S07_PACKUN_SHOOT_TRACE_PITCH;
    extern float DODGE_PRM_S07_PACKUN_BLT_NUM;
    extern float DODGE_PRM_S07_PACKUN_BLT_HEIGHT;
    extern float DODGE_PRM_S07_PACKUN_BLT_TIME;
    extern float DODGE_PRM_S07_PACKUN_BLT_LIFE;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_TIME;
    extern float DODGE_PRM_S07_PACKUN_STAY_TIME;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_DELAY;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_MDL_SIZE;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_SIZE;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_SIZE_Y;
    extern float DODGE_PRM_S07_PACKUN_EXPLODE_SCALE_FCTR;

    // Stage 12, the pinball stage (CStageDodge12::GetPrmTbl and
    // CPinballManDodge).
    extern float DODGE_PRM_S12_BALL_SIZE;
    extern float DODGE_PRM_S12_BALL_COLLSIZE;
    extern float DODGE_PRM_S12_BALL_SPEED;
    extern float DODGE_PRM_S12_BALL_SPEED_LIMIT;
    extern float DODGE_PRM_S12_BALL_KICK_POW;
    extern float DODGE_PRM_S12_BALL_BALL_KICK_POW;
    extern float DODGE_PRM_S12_BALL_DECEL;
    extern float DODGE_PRM_S12_BALL_GUIDE_DECAY;
    extern float DODGE_PRM_S12_BALL_FORCE_ACC;
    extern float DODGE_PRM_S12_BALL_INIT_SLEEP;
    extern float DODGE_PRM_S12_BALL_INTERVAL;
    extern float DODGE_PRM_S12_BALL_ANGLE;
    extern float DODGE_PRM_S12_BALL_SIDE_RATE;
    extern long DODGE_PRM_S12_BALL_NUM;
    extern float DODGE_PRM_S12_BALL_DOWN_TIME;
}

#endif /* !MSW_DODGEPRM_HPP_ */
