#include "game/msw/CSceneCalc.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventScene.hpp"
#include "game/msw/CStageMan.hpp"

msw::CSceneCalc::CSceneCalc(CEventScene *pScene) : bcs::CCameraAnmCalc(pScene)
{
    m_pScene = pScene;
    m_pFloorChr = 0;
    m_bCourtCenter = 0;
}

msw::CSceneCalc::~CSceneCalc() {}

void msw::CSceneCalc::Calc(bcs::CCameraAnm &rAnm, bcs::FVEC3 &rPos, bcs::FVEC3 &rAt, float &rRoll)
{
    _CalcRotOffset(rAnm, rPos, rAt, rRoll);
    _CalcFloor(rPos, rAt);
    _CalcRev(rAnm, rPos, rAt, rRoll);
}

// Moves the position and the target by the stage's court centre (in z) and
// by the offset of the model being followed.
void msw::CSceneCalc::_CalcFloor(bcs::FVEC3 &rPos, bcs::FVEC3 &rAt)
{
    if (m_bCourtCenter) {
        bcs::FVEC3 vCenter(0.0f, 0.0f, CStageMan::GetInstance()->GetCourtCenter());
        bcs::FVEC3Add(&rPos, &rPos, &vCenter);
        bcs::FVEC3Add(&rAt, &rAt, &vCenter);
    }

    if (m_pFloorChr != 0) {
        bcs::FVEC3 vOffset = m_pFloorChr->GetPosOffset();
        bcs::FVEC3Add(&rPos, &rPos, &vOffset);
        bcs::FVEC3Add(&rAt, &rAt, &vOffset);
    }
}

void msw::CSceneCalc::SetCalc(int bSet)
{
    bcs::CCameraAnm *pCamera = m_pScene->m_pCamera;
    if (bSet) {
        pCamera->SetCalc(this);
    } else {
        pCamera->SetCalc(pCamera->GetDefaultCalc());
    }
}

void msw::CSceneCalc::SetFloorChr(long nChr)
{
    m_pFloorChr = m_pScene->GetChr(nChr);
}
