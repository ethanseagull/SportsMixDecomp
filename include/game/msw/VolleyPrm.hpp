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
}

#endif /* !MSW_VOLLEYPRM_HPP_ */
