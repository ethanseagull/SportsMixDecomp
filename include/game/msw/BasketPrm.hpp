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

    // Stage 70 (the boss Piranha Plant), "just shot" timing: the three
    // shot distances the ball's timing frames are given for.
    extern float BASKET_PRM_S70_JS_MIN_DIST;
    extern float BASKET_PRM_S70_JS_MID_DIST;
    extern float BASKET_PRM_S70_JS_MAX_DIST;
}

#endif /* !MSW_BASKETPRM_HPP_ */
