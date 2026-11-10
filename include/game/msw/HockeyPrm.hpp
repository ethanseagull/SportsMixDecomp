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
}

#endif /* !MSW_HOCKEYPRM_HPP_ */
