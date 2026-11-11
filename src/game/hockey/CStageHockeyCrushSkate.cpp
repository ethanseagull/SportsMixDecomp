// msw::CStageHockeyCrushSkate: the common class of the hockey crush skate
// stages (85, 86, 87). The court rolls (CStageRotBase, a DOL class, does
// the work); this class hands the stage manager's hooks on to it and gives
// it the floor tuning.
#include "game/msw/CStageHockeyCrushSkate.hpp"
#include "game/msw/CGame.hpp"

// Ids of the model table.
enum {
    MDL_BG = 1,
    MDL_BG2 = 2
};

msw::CStageHockeyCrushSkate::CStageHockeyCrushSkate(bcs::CTask *pParent) : CStageManHockey(pParent)
{
}

msw::CStageHockeyCrushSkate::~CStageHockeyCrushSkate()
{
}

void msw::CStageHockeyCrushSkate::Main_(float frame)
{
    StageRot_Main_(frame);
    CStageManHockey::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockeyCrushSkate::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage[0-9]+_bg$", MDL_BG, 0, 0, NewMdl },
        { "^stage[0-9]+_bg2$", MDL_BG2, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The two background models scroll with the roll.
void msw::CStageHockeyCrushSkate::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                  CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_BG:
            SetScrollNull(*rHandle.Get());
            break;
        case MDL_BG2:
            SetScrollNull2(*rHandle.Get());
            break;
    }
}

void msw::CStageHockeyCrushSkate::OnTurnInit()
{
    InitWorldRot__();
}

void msw::CStageHockeyCrushSkate::OnVictoryEventStart()
{
    InitWorldRot__();
}

void msw::CStageHockeyCrushSkate::OnGimmickStop()
{
    InitWorldRot__();
}

float msw::CStageHockeyCrushSkate::GetBaseCourtWidth_() const
{
    return 18.0f;
}

float msw::CStageHockeyCrushSkate::GetBaseCourtDepth_() const
{
    return 9.0f;
}

float msw::CStageHockeyCrushSkate::GetBaseCourtRadius_() const
{
    return 3.0f;
}

long msw::CStageHockeyCrushSkate::GetLocPatternNo() const
{
    return CGame::GetPartyGameParam(0);
}

const msw::CStageMan *msw::CStageHockeyCrushSkate::GetStageMan() const
{
    return this;
}

bool msw::CStageHockeyCrushSkate::IsRotReady_() const
{
    return IsGimmickRunning();
}

float msw::CStageHockeyCrushSkate::GetParam_ShipCycle() const
{
    return HOCKEY_PRM_S85_FLOOR0_CYCLE;
}

float msw::CStageHockeyCrushSkate::GetParam_ShipStopTime() const
{
    return HOCKEY_PRM_S85_FLOOR0_STOP_TIME;
}

float msw::CStageHockeyCrushSkate::GetParam_ShipStopTimeRnd() const
{
    return HOCKEY_PRM_S85_FLOOR0_STOP_TIME_RND;
}

float msw::CStageHockeyCrushSkate::GetParam_ChrFlowVel() const
{
    return HOCKEY_PRM_S85_FLOOR0_CHR_FLOW_VEL;
}

// The floor does not drift sideways and the background does not turn.
float msw::CStageHockeyCrushSkate::GetParam_ShipTransWidth() const
{
    return 0.0f;
}

// 10 degrees.
float msw::CStageHockeyCrushSkate::GetParam_ShipRotWidth() const
{
    return 0.17453292f;
}

float msw::CStageHockeyCrushSkate::GetParam_BgRotSpeed() const
{
    return 0.0f;
}
