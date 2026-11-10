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
}

#endif /* !MSW_DODGEPRM_HPP_ */
