#include "game/msw/CGoalMdlHockey.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CLoadData.hpp"
#include "stdio.h"

// The goal model of the hockey game. Model 1 of a goal is the goal itself
// (with animations), model 4 its net; the other indices have no model.

namespace {

    // The suffixes of the goal animations' names.
    const char *s_AnmNameTbl[] = { "_idle", "_shot", "_sp_shot", 0 };

    // How each goal animation is played (the table ends with id -1).
    const msw::CGoalMdl::ANM_PARAM s_AnmParamTbl[] = {
        { 0, 2, 1.0f, 0.0f, 0.0f, -1 },
        { 1, 0, 1.0f, 0.0f, 0.0f, -1 },
        { 2, 0, 1.0f, 0.0f, 0.0f, -1 },
        { -1, 0, 0.0f, 0.0f, 0.0f, 0 },
    };
}

msw::CGoalMdlHockey::CGoalMdlHockey(CStgGmk *pParent)
    : CGoalMdl(pParent)
{
}

msw::CGoalMdlHockey::~CGoalMdlHockey()
{
}

// The goal itself (model 1, and model 5) fades out when it stands between
// the camera and the court; it faces the side of its goal.
bcs::CMdlObj *msw::CGoalMdlHockey::NewMdl(long heap) const
{
    CFgMdl *pMdl = 0;
    unsigned long idx = GetIndex();

    if (idx == 1 || idx == 5) {
        const CGoalObj *pGoal = static_cast<const CGoalObj *>(GetParent());
        if (CGameCamera::IsBehindCamera()) {
            pMdl = new (heap) CFgMdl();
            pMdl->SetFadeDir(pGoal->m_nGoalIdx != 0 ? 0.0f : 3.1415927f);
            pMdl->SetFadeAngle1(1.3962634f);
            pMdl->SetFadeAngle0(1.5707964f);
            pMdl->SetUnk214And218(0.4f);
            pMdl->SetFadeEnable(1);
            pMdl->CalcRootMtx();
            pMdl->OnFlag(0x4000);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->OnFlag(0x10000);
        } else if (idx == 1) {
            pMdl = new (heap) CFgMdl();
        }
    } else {
        pMdl = new (heap) CFgMdl();
    }
    return pMdl;
}

const msw::CGoalMdl::ANM_PARAM *msw::CGoalMdlHockey::GetAnmParamTbl() const
{
    return s_AnmParamTbl;
}

const char *const *msw::CGoalMdlHockey::GetAnmNameTbl() const
{
    return s_AnmNameTbl;
}

void msw::CGoalMdlHockey::Main(float rate)
{
    // The index is read and not used.
    GetIndex();
    CGoalMdl::Main(rate);
}

// The model names are numbered by stage and sub-type; only the goal (model
// 1) has animations.
void msw::CGoalMdlHockey::Create()
{
    if (!CLoadData::GetRes(1).IsValid()) {
        return;
    }

    MDL_PARAM param;
    const char *mdlNameTbl[] = { 0, "s%02d_goal_%02d", "s%02d_goal_lo_%02d", 0, "s%02d_goalnet_%02d", 0 };
    const char *shadowNameTbl[6] = { 0 };
    const char *refNameTbl[6] = { 0 };

    char anmName[0x40];
    snprintf(anmName, sizeof(anmName) - 1, "s%02d_goal", CGame::g_GameWork.m_nStage);

    unsigned long idx = GetIndex();
    char mdlName[0x80];
    char shadowName[0x80];
    char refName[0x80];

    if (mdlNameTbl[idx] != 0) {
        sprintf(mdlName, mdlNameTbl[idx], CGame::g_GameWork.GetStage(), GetSubType());
        param.pMdlName = mdlName;
    } else {
        param.pMdlName = 0;
    }
    if (shadowNameTbl[idx] != 0) {
        sprintf(shadowName, shadowNameTbl[idx], CGame::g_GameWork.GetStage(), GetSubType());
        param.pShadowName = shadowName;
    } else {
        param.pShadowName = 0;
    }
    if (refNameTbl[idx] != 0) {
        sprintf(refName, refNameTbl[idx], CGame::g_GameWork.GetStage(), GetSubType());
        param.pRefName = refName;
    } else {
        param.pRefName = 0;
    }
    param.unk0C = 4;
    param.ppAnmSuffix = idx == 1 ? GetAnmNameTbl() : 0;
    param.pAnmName = idx == 1 ? anmName : 0;
    param.unk18 = 0;
    param.pAnmParam = idx == 1 ? GetAnmParamTbl() : 0;
    Create(1, &param);
}

void msw::CGoalMdlHockey::Create(long res, const MDL_PARAM *pParam)
{
    CStgMdl::Create(CGenericMVAGimmickCreator(CLoadData::GetRes(res), pParam));
}
