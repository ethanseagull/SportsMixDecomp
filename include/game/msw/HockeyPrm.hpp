#ifndef MSW_HOCKEYPRM_HPP_
#define MSW_HOCKEYPRM_HPP_

// Tuning values of the hockey mode (HockeyF), globals in namespace msw
// (HOCKEY_PRM_*__3msw). Only the ones used by source written so far are
// declared; the types are those of the loads (lwz: long, lfs: float).
namespace msw {

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
