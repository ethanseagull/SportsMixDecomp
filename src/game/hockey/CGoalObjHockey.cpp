// msw::CGoalObjHockey: the goal of the hockey game. CStageManHockey makes
// one per side. Model 0 is an empty stand-in, model 1 the goal model
// (CGoalMdlHockey), whose animations show a goal being scored on.
#include "game/msw/CGoalObjHockey.hpp"

#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CStageMan.hpp"

// Animations of the goal model (SetAnmEx).
enum {
    ANM_WAIT,
    ANM_DUNK,
    ANM_DUNK_STRONG
};

// Effect played on the goal model when a goal is scored.
static const long EFF_GOAL = 0x62;

msw::CGoalObjHockey::CGoalObjHockey(bcs::CTask *pParent, long idx) : CGoalObj(pParent, idx)
{
}

msw::CGoalObjHockey::~CGoalObjHockey()
{
}

// The goal stands in the middle of its end of the court, 4.5 units in from
// the end line (odd goals at the back).
void msw::CGoalObjHockey::SetPosAuto()
{
    CStageMan *pStage = CStageMan::GetInstance();
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    if (m_nGoalIdx & 1) {
        pos.z = pStage->GetCourtBack() - 4.5f;
    } else {
        pos.z = 4.5f + pStage->GetCourtFront();
    }
    pos.x = 0.0f;
    SetPos(pos);
}

void msw::CGoalObjHockey::GoalIn()
{
    bcs::CScnObj *pObj = GetMdlObj(1);
    if (pObj != 0) {
        CEffectCtrl::CreateAttachEffectAuto(EFF_GOAL, static_cast<bcs::CMdlObj *>(pObj), 3);
    }
}

float msw::CGoalObjHockey::DunkStart(DUNK_POW pow, const bcs::FVEC3 &rPos)
{
    unsigned long anm = (pow == 2) + ANM_DUNK;
    CStgMdl *pMdl = GetMdl(1);
    if (pMdl != 0) {
        pMdl->SetAnmEx(anm, 1);
    }
    return 1.0f;
}

void msw::CGoalObjHockey::DunkCancel()
{
    CStgMdl *pMdl = GetMdl(1);
    if (pMdl != 0) {
        pMdl->SetAnmEx(ANM_WAIT, 1);
    }
}
