#include "game/msw/CStgGmk_Wave_Hockey.hpp"
#include "game/msw/HockeyPrm.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"

// The wave stage gimmick of the hockey mode (stage 02). One file, though
// the cutter split it in five units by class name: after these getters it
// emits the inline functions of the three classes' vtables (the factories
// and destructors in include/game/msw/CStgGmk_Wave_Hockey.hpp, the accessors
// of msw::CStgGmk_WaveCluster, msw::CStgGmk_Wave and msw::CStgGmk_Wave_Act)
// and the this-adjusting thunks, and its .data holds the three vtables.

// Type 1 is the big wave.

// Hockey turns the big wave off when bit 3 of the game flags is set (name
// of the flag unknown).
bool msw::CStgGmk_WaveCluster_Hockey::IsNowEnableBigWave() const
{
    bool bEnable = CStgGmk_WaveCluster::IsNowEnableBigWave();
    if (bEnable && (CGame::g_GameWork.m_nFlag18 & 8)) {
        bEnable = false;
    }
    return bEnable;
}

long msw::CStgGmk_Wave_Hockey_Act::GetParam_Effect(long type) const
{
    (void)type;
    return -1;
}

long msw::CStgGmk_Wave_Hockey_Act::GetParam_PopCoinNum(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_COIN_POP_NUM;
    }
    return HOCKEY_PRM_S02_COIN_POP_NUM;
}

// The court's sides, with the front and back taken from the two goals.
void msw::CStgGmk_Wave_Hockey_Act::GetParam_CourtRect(float &rLeft, float &rRight, float &rFront, float &rBack) const
{
    CStageMan *pStageMan = CStageMan::GetInstance();
    pStageMan->GetCourtRect(&rLeft, &rRight, &rFront, &rBack);
    rFront = pStageMan->GetGoalPos(0).z;
    rBack = pStageMan->GetGoalPos(1).z;
}

// The parameters are in seconds; the cluster counts frames.

float msw::CStgGmk_WaveCluster_Hockey::GetParam_PartCycle() const
{
    return 60.0f * HOCKEY_PRM_S02_WAVE_PART_INTERVAL;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_BigWaveCycle() const
{
    return 60.0f * HOCKEY_PRM_S02_WAVE_BIG_INTERVAL;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_BigWaveCycleRnd() const
{
    return 60.0f * HOCKEY_PRM_S02_WAVE_BIG_INTERVAL_RND;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_OffsetX() const
{
    return HOCKEY_PRM_S02_WAVE_OFFSET_X;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_BigWaveOffsetX() const
{
    return HOCKEY_PRM_S02_WAVE_BIG_OFFSET_X;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_PartAnimTime() const
{
    return 60.0f * HOCKEY_PRM_S02_WAVE_ANIM_TIME;
}

float msw::CStgGmk_WaveCluster_Hockey::GetParam_BigPartAnimTime() const
{
    return 60.0f * HOCKEY_PRM_S02_WAVE_BIG_ANIM_TIME;
}

long msw::CStgGmk_WaveCluster_Hockey::GetParam_PopItemCycle(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_CYCLE;
    }
    return HOCKEY_PRM_S02_ITEM_POP_CYCLE;
}

long msw::CStgGmk_Wave_Hockey_Act::GetParam_PopItemNum(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_NUM;
    }
    return HOCKEY_PRM_S02_ITEM_POP_NUM;
}

float msw::CStgGmk_Wave_Hockey_Act::GetParam_PopItemDelay(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_DELAY;
    }
    return HOCKEY_PRM_S02_ITEM_POP_DELAY;
}

float msw::CStgGmk_Wave_Hockey_Act::GetParam_PopItemComeFrame(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_COME_FRM;
    }
    return HOCKEY_PRM_S02_ITEM_POP_COME_FRM;
}

float msw::CStgGmk_Wave_Hockey_Act::GetParam_PopItemPopDist(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_DIST;
    }
    return HOCKEY_PRM_S02_ITEM_POP_DIST;
}

float msw::CStgGmk_Wave_Hockey_Act::GetParam_ItemPopPosX(long type) const
{
    if (type == 1) {
        return HOCKEY_PRM_S02_BIG_ITEM_POP_POS_X;
    }
    return HOCKEY_PRM_S02_ITEM_POP_POS_X;
}
