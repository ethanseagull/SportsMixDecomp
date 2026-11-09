// msw::CStageManBoss: the stage manager of the boss game.
#include "game/msw/CStageManBoss.hpp"

msw::CStageManBoss::~CStageManBoss()
{
}

msw::CStageManBoss *msw::CStageManBoss::CreateInstance(bcs::CTask *pParent, long stageNo)
{
    CStageManBoss *pStageMan = new CStageManBoss(pParent);

    if (pStageMan != 0) {
        pStageMan->SetStageNo(stageNo);
        pStageMan->Initialize_();
    }
    return pStageMan;
}

float msw::CStageManBoss::GetBaseCourtWidth_() const
{
    return 46.0f;
}

float msw::CStageManBoss::GetBaseCourtDepth_() const
{
    return 23.0f;
}

float msw::CStageManBoss::GetBaseCourtRadius_() const
{
    return 23.0f;
}
