#include "game/msw/CStgGmk_Wave_Basket.hpp"
#include "game/msw/BasketPrm.hpp"

// The wave stage gimmick of the basketball mode (stage 02). One file, though
// the cutter split it in five units by class name: after these getters it
// emits the inline functions of the three classes' vtables (the factories
// and destructors in include/game/msw/CStgGmk_Wave_Basket.hpp, the accessors
// of msw::CStgGmk_WaveCluster, msw::CStgGmk_Wave and msw::CStgGmk_Wave_Act)
// and the this-adjusting thunks, and its .data holds the three vtables.

// Type 1 is the big wave.

long msw::CStgGmk_Wave_Basket_Act::GetParam_Effect(long type) const
{
    (void)type;
    return -1;
}

long msw::CStgGmk_Wave_Basket_Act::GetParam_PopCoinNum(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_COIN_POP_NUM;
    }
    return BASKET_PRM_S02_COIN_POP_NUM;
}

long msw::CStgGmk_Wave_Basket_Act::GetParam_PopItemNum(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_NUM;
    }
    return BASKET_PRM_S02_ITEM_POP_NUM;
}

float msw::CStgGmk_Wave_Basket_Act::GetParam_PopItemDelay(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_DELAY;
    }
    return BASKET_PRM_S02_ITEM_POP_DELAY;
}

float msw::CStgGmk_Wave_Basket_Act::GetParam_PopItemComeFrame(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_COME_FRM;
    }
    return BASKET_PRM_S02_ITEM_POP_COME_FRM;
}

float msw::CStgGmk_Wave_Basket_Act::GetParam_PopItemPopDist(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_DIST;
    }
    return BASKET_PRM_S02_ITEM_POP_DIST;
}

float msw::CStgGmk_Wave_Basket_Act::GetParam_ItemPopPosX(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_POS_X;
    }
    return BASKET_PRM_S02_ITEM_POP_POS_X;
}

// The parameters are in seconds; the cluster counts frames.

float msw::CStgGmk_WaveCluster_Basket::GetParam_PartCycle() const
{
    return 60.0f * BASKET_PRM_S02_WAVE_PART_INTERVAL;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_BigWaveCycle() const
{
    return 60.0f * BASKET_PRM_S02_WAVE_BIG_INTERVAL;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_BigWaveCycleRnd() const
{
    return 60.0f * BASKET_PRM_S02_WAVE_BIG_INTERVAL_RND;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_OffsetX() const
{
    return BASKET_PRM_S02_WAVE_OFFSET_X;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_BigWaveOffsetX() const
{
    return BASKET_PRM_S02_WAVE_BIG_OFFSET_X;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_PartAnimTime() const
{
    return 60.0f * BASKET_PRM_S02_WAVE_ANIM_TIME;
}

float msw::CStgGmk_WaveCluster_Basket::GetParam_BigPartAnimTime() const
{
    return 60.0f * BASKET_PRM_S02_WAVE_BIG_ANIM_TIME;
}

long msw::CStgGmk_WaveCluster_Basket::GetParam_PopItemCycle(long type) const
{
    if (type == 1) {
        return BASKET_PRM_S02_BIG_ITEM_POP_CYCLE;
    }
    return BASKET_PRM_S02_ITEM_POP_CYCLE;
}
