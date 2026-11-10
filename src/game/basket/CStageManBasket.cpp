// msw::CStageManBasket: the stage manager of the basket game.
#include "game/msw/CStageManBasket.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGoalObjBasket.hpp"
#include "game/msw/CStageFactoryBasket.hpp"
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

msw::CStageManBasket::CStageManBasket(bcs::CTask *pParent)
    : CStageMan(pParent), m_nBallTeam(-1), m_pRotNode(0), m_fRotNodeOffset(0.0f)
{
    for (int i = 0; i < 4; i++) {
        m_apGoal[i] = 0;
    }
}

msw::CStageManBasket::~CStageManBasket()
{
}

msw::CStageManBasket *msw::CStageManBasket::CreateInstance(bcs::CTask *pParent, long stageNo)
{
    CStageManBasket *pStageMan = CStageFactoryBasket::Create(stageNo, pParent);

    pStageMan->SetStageNo(stageNo);
    return pStageMan;
}

void msw::CStageManBasket::Main_(float frame)
{
    CStageMan::Main_(frame);
    UpdateRotNode();

    long prevTeam = m_nBallTeam;
    long team = GetBallTeam();

    if (team >= 0) {
        m_nBallTeam = team;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 0x4306) == 0 && team >= 0 && prevTeam != team) {
        OnSoundEvent(3);
        OnBasketSwitchOffense(team);
    }
}

float msw::CStageManBasket::GetBaseCourtWidth_() const
{
    return 15.0f;
}

float msw::CStageManBasket::GetBaseCourtDepth_() const
{
    return 11.5f;
}

const bcs::FVEC3 &msw::CStageManBasket::GetGoalOffsetPos() const
{
    static const bcs::FVEC3 pos_tbl(0.0f, 0.0f, -1.0f);

    return pos_tbl;
}

const bcs::FVEC3 &msw::CStageManBasket::GetGoalPos(long goal) const
{
    return GetGoal(goal)->GetRingPos();
}

const bcs::FVEC3 &msw::CStageManBasket::GetGoalRot(long goal) const
{
    return GetGoal(goal)->GetRot();
}

const bcs::FVEC3 &msw::CStageManBasket::GetBoardPos(long goal) const
{
    return GetGoal(goal)->GetBoardPos();
}

const bcs::FVEC3 &msw::CStageManBasket::GetGoalVec(long goal) const
{
    return GetGoal(goal)->GetBoardVec();
}

bool msw::CStageManBasket::Is3PointArea(long goal, const bcs::FVEC3 &rPos, float radius) const
{
    bcs::FVEC3 center = Get3PointAreaPos(goal);

    center.y = rPos.y;
    if (bcs::FVEC3Dist(&center, &rPos) <= radius) {
        return true;
    }

    // The band from the centre to the end of the court on the goal's side,
    // with FLT_MIN and FLT_MAX as its y bounds (no <float.h> here).
    bcs::FVEC3 min(center.x - radius, 1.17549435e-38f, center.z);
    bcs::FVEC3 max(center.x + radius, 3.40282347e+38f, center.z);

    if (goal == 0) {
        min.z = GetTeamCourtFront(goal ^ 1);
    } else {
        max.z = GetTeamCourtBack(goal ^ 1);
    }
    if (min.x <= rPos.x && rPos.x <= max.x && min.z <= rPos.z && rPos.z <= max.z) {
        return true;
    }
    return false;
}

void msw::CStageManBasket::GoalCrush(long goal)
{
    CGoalObj *pGoal = GetGoal(goal);

    if (pGoal != 0) {
        pGoal->CrushStart();
    }
    OnGoalIn(goal, static_cast<GOAL_IN_TYPE>(2));
}

void msw::CStageManBasket::GoalRepairAll()
{
    for (long i = 0; i < 4; i++) {
        CGoalObj *pGoal = GetGoal(i);

        if (pGoal != 0) {
            pGoal->CrushEnd();
        }
    }
}

void msw::CStageManBasket::UpdateRotNode()
{
    if (m_pRotNode != 0) {
        // Half a turn when the team of colour 0 attacks goal 1.
        float rot = CGame::GetTargetGoalIdx(CGame::GetTeamColorIdx(0) != 0) == 0 ? 0.0f : 3.1415927f;

        rot = WrapRad(rot + m_fRotNodeOffset);

        bcs::CScnObj *pObj = m_pRotNode->GetObj();

        if (pObj != 0) {
            pObj->m_fRotY = WrapRad(rot);
            pObj->CalcRootMtx();
            pObj->CalcWorldMtx(0);
        }
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageManBasket::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageMan.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage[0-9]+_gl$", 50000, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageManBasket::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                            CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                            long n)
{
    if (pRule->m_nId == 50000) {
        RegistRotNode(pNode, 0.0f);
    }
}

void msw::CStageManBasket::OnPostInitialize()
{
    CreateGoal();
}

void msw::CStageManBasket::CreateGoal()
{
    for (long i = 0; i < 2; i++) {
        CGoalObjBasket *pGoal = new CGoalObjBasket(this, i);

        if (pGoal != 0) {
            pGoal->Create();
            pGoal->SetDefaultOffset(GetGoalOffsetPos());
            pGoal->SetPosAuto();
            pGoal->SetRotAuto();
            pGoal->SceneEntry();
            m_apGoal[i] = pGoal;
        } else if (pGoal != 0) {
            // Never reached. The original has this guarded delete in the
            // failure branch (most likely a clean-up macro); it is kept for
            // the code it produces.
            delete pGoal;
        }
    }
}

long msw::CStageManBasket::GetBallTeam() const
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

long msw::CStageManBasket::GetGoalSE(GOAL_SE_TYPE type) const
{
    long se = -1;

    if (type == GOAL_SE_TYPE_0) {
        switch (GetStageNo()) {
            case 3:
            case 10:
            case 11:
            case 16:
            case 17:
                se = 0xC6C;
                break;
            case 9:
                se = 0xC6E;
                break;
            case 6:
                se = 0xC6D;
                break;
            case 7:
                se = 0xD44;
                break;
            default:
                se = 0xC6B;
                break;
        }
    }
    return se;
}

// The fourth argument is not passed on: CStageMan gets 0.
void msw::CStageManBasket::SendMessage(long msg, long arg1, long arg2, long)
{
    switch (msg) {
        case 0x32:
            OnBasketGoalResult(arg1);
            break;
        case 0x34:
            OnBasketShoot(arg1, arg2);
            break;
        case 0x13:
            for (long i = 0; i < 4; i++) {
                CGoalObj *pGoal = GetGoal(i);

                if (pGoal != 0) {
                    pGoal->DunkCancel();
                }
            }
            break;
    }

    CStageMan::SendMessage(msg, arg1, arg2, 0);

    switch (msg) {
        case 3:
        case 0x13:
            PlayAnim_AudienceFlash(1, -1.0f);
            InitAnim_AudienceFlag();
            // The original makes this call and ignores the result (perhaps
            // a check whose body is empty in this build).
            SE_IsAlive(0);
            m_nBallTeam = GetBallTeam();
            break;
        case 0x36:
            PlayAnim_AudienceFlash(2, 120.0f);
            break;
        case 5:
            InitAnim_AudienceFlag();
            break;
    }

    switch (msg) {
        case 3:
        case 0x13:
            OnSoundEvent(0);
            break;
        case 0x36:
            if (arg2 >= 9 && arg2 <= 11) {
                OnSoundEvent(2);
            } else {
                OnSoundEvent(1);
            }
            break;
    }
}
