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

    // The Hungry Boss Packun stages: the size of the court.
    extern float BASKET_PRM_BOSS_PACKUN_COURT_WIDTH;
    extern float BASKET_PRM_BOSS_PACKUN_COURT_DEPTH;
}

#endif /* !MSW_BASKETPRM_HPP_ */
