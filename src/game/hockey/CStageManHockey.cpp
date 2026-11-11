// Written from src/game/basket/CStageManBasket.cpp by tools/decomp/port_unit.py (Basket -> Hockey),
// then rewritten against HockeyF.
// msw::CStageManHockey: the stage manager of the hockey game.
#include "game/msw/CStageManHockey.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGoalObjHockey.hpp"
#include "game/msw/CStageFactoryHockey.hpp"
#include "game/msw/CStageMdlListNode.hpp"

namespace msw {

    namespace {

        // An angle brought into [0, 2 pi).
        inline float WrapRad(float rad)
        {
            while (rad > 6.2831855f) {
                rad -= 6.2831855f;
            }
            while (rad < 0.0f) {
                rad += 6.2831855f;
            }
            return rad;
        }
    }
}

msw::CStageManHockey::CStageManHockey(bcs::CTask *pParent)
    : CStageMan(pParent), m_nBallTeam(-1), m_pRotNode(0), m_fRot(0.0f)
{
    for (int i = 0; i < 4; i++) {
        m_pGoal[i] = 0;
    }
}

msw::CStageManHockey::~CStageManHockey()
{
}

msw::CStageManHockey *msw::CStageManHockey::CreateInstance(bcs::CTask *pParent, long stageNo)
{
    CStageManHockey *pStageMan = CStageFactoryHockey::Create(stageNo, pParent);

    pStageMan->SetStageNo(stageNo);
    return pStageMan;
}

// Plays sound event 4 when the ball changes teams (not while any of the
// game's 0x4306 flags is set).
void msw::CStageManHockey::Main_(float frame)
{
    CStageMan::Main_(frame);
    UpdateRotNode();

    long prevTeam = m_nBallTeam;
    long team = GetBallTeam();

    if (team >= 0) {
        m_nBallTeam = team;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 0x4306) == 0 && team >= 0 && prevTeam != team) {
        OnSoundEvent(4);
    }
}

// The outer field is the base outer area, centred on x and ending at z 0.
float msw::CStageManHockey::GetOuterFieldLeft() const
{
    return -(0.5f * GetBaseOuterWidth_());
}

float msw::CStageManHockey::GetOuterFieldRight() const
{
    return 0.5f * GetBaseOuterWidth_();
}

float msw::CStageManHockey::GetOuterFieldFront() const
{
    return -GetBaseOuterDepth_();
}

float msw::CStageManHockey::GetOuterFieldBack() const
{
    return GetBaseOuterDepth_();
}

float msw::CStageManHockey::GetBaseCourtWidth_() const
{
    return 23.0f;
}

float msw::CStageManHockey::GetBaseCourtDepth_() const
{
    return 25.0f;
}

float msw::CStageManHockey::GetBaseCourtRadius_() const
{
    return 6.0f;
}

const bcs::FVEC3 &msw::CStageManHockey::GetGoalPos(long goal) const
{
    return GetGoal(goal)->GetRingPos();
}

const bcs::FVEC3 &msw::CStageManHockey::GetGoalRot(long goal) const
{
    return GetGoal(goal)->GetRot();
}

const bcs::FVEC3 &msw::CStageManHockey::GetGoalVec(long goal) const
{
    return GetGoal(goal)->GetBoardVec();
}

// The goal mouth: the straight part plus a rounded corner on each side.
float msw::CStageManHockey::GetGoalWidth(long goal) const
{
    float x1 = GetGoalX1Length(goal);

    return 2.0f * GetGoalRRRadius(goal) + x1;
}

float msw::CStageManHockey::GetGoalX1Length(long goal) const
{
    return 2.1f;
}

float msw::CStageManHockey::GetGoalRRRadius(long goal) const
{
    return 1.5f;
}

float msw::CStageManHockey::GetGoalDepth(long goal) const
{
    return GetGoalRRRadius(goal);
}

void msw::CStageManHockey::UpdateRotNode()
{
    if (m_pRotNode != 0) {
        // Half a turn when the team of colour 0 attacks goal 1.
        float rot = CGame::GetTargetGoalIdx(CGame::GetTeamColorIdx(0) != 0) == 0 ? 0.0f : 3.1415927f;

        rot = WrapRad(rot + m_fRot);

        bcs::CScnObj *pObj = m_pRotNode->GetObj();

        if (pObj != 0) {
            pObj->m_fRotY = WrapRad(rot);
            pObj->CalcRootMtx();
            pObj->CalcWorldMtx(0);
        }
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageManHockey::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageMan.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage[0-9]+_gl$", 50000, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageManHockey::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                            CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                            long n)
{
    if (pRule->m_nId == 50000) {
        m_pRotNode = pNode;
    }
}

void msw::CStageManHockey::OnPostInitialize()
{
    CreateGoal();
}

void msw::CStageManHockey::CreateGoal()
{
    for (long i = 0; i < 2; i++) {
        CGoalObj *pGoal = NewGoalObj(this, i);

        if (pGoal != 0) {
            pGoal->Create();
            pGoal->SetPosAuto();
            pGoal->SetRotAuto();
            pGoal->SceneEntry();
            m_pGoal[i] = pGoal;
        } else if (pGoal != 0) {
            // Never reached. The original has this guarded delete in the
            // failure branch (most likely a clean-up macro); it is kept for
            // the code it produces.
            delete pGoal;
        }
    }
}

CGoalObj *msw::CStageManHockey::NewGoalObj(bcs::CTask *pParent, long goal)
{
    return new CGoalObjHockey(pParent, goal);
}

void msw::CStageManHockey::GoalIn(long goal)
{
    CGoalObj *pGoal = GetGoal(goal);

    if (pGoal != 0) {
        pGoal->GoalIn();
    }
    OnGoalIn(goal, static_cast<GOAL_IN_TYPE>(1));
}

long msw::CStageManHockey::GetBallTeam() const
{
    long team = -1;
    long num = CChrMan::GetChrCoreNum();

    for (long i = 0; i < num; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);

        if (pChr != 0 && pChr->IsEnable() && (pChr->GetStatusFlag() & 1)) {
            team = pChr->GetTeamNo();
            break;
        }
    }
    return team;
}

void msw::CStageManHockey::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageMan::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case 0x5D:
            OnHockeyGoalResult(arg1);
            break;
        case 0x5E:
            OnHockeyShoot(arg1);
            break;
        case 0x13:
            for (long i = 0; i < 4; i++) {
                CGoalObj *pGoal = GetGoal(i);

                if (pGoal != 0) {
                    pGoal->DunkCancel();
                }
            }
            break;
        case 5:
            InitAnim_AudienceFlag();
            break;
    }

    switch (msg) {
        case 3:
        case 0x13:
            PlayAnim_AudienceFlash(1, -1.0f);
            InitAnim_AudienceFlag();
            break;
        case 0x61:
        case 5:
            PlayAnim_AudienceFlash(2, 180.0f);
            break;
    }

    switch (msg) {
        case 3:
        case 0x13:
            m_nBallTeam = GetBallTeam();
            OnSoundEvent(0);
            break;
        case 0x61:
        case 5:
            switch (arg3) {
                case 1:
                    OnSoundEvent(2);
                    break;
                case 2:
                    OnSoundEvent(3);
                    break;
                default:
                    OnSoundEvent(1);
                    break;
            }
            break;
    }
}
