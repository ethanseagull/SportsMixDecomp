// msw::CGoalObjBasket: the goal of the basket game, and
// msw::CGoalObjBasket_BossPackun, the goal the boss Packun carries in the
// Hungry Boss Packun stages. The basket goal adds two body collision units
// for its stand, whose shapes depend on the stage (sCollTable); the boss's
// goal takes its model and ring from the boss.
#include "game/msw/CGoalObjBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGoalObjBasket_BossPackun.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStageMan.hpp"

// Set by the file's static initialiser and used by nothing in the file:
// about 60 files of the game modules start their .bss with such an angle,
// so it most likely comes from a shared header. Which one, and the name,
// are not known.
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace {

    typedef msw::CGoalObjBasket::COLL_TABLE COLL_TABLE;

    // The stand collision of each stage: the position of each unit relative
    // to the board, and its size. Stages not listed have none.
    COLL_TABLE sCollTable[] = {
        { 1,  { { 1, bcs::FVEC3(0.0f, -1.15f, -0.75f), bcs::FVEC3(1.55f, 1.85f, 0.8f) }, { 0 } } },
        { 3,  { { 1, bcs::FVEC3(0.0f, -1.5f, -0.65f), bcs::FVEC3(1.5f, 2.5f, 0.45f) }, { 0 } } },
        { 4,  { { 1, bcs::FVEC3(0.0f, -1.1f, -0.55f), bcs::FVEC3(1.4f, 2.45f, 0.5f) }, { 0 } } },
        { 7,  { { 1, bcs::FVEC3(0.0f, -1.0f, -0.45f), bcs::FVEC3(1.1f, 2.6f, 0.5f) }, { 0 } } },
        { 8,  { { 1, bcs::FVEC3(0.0f, -1.9f, -2.25f), bcs::FVEC3(0.5f, 2.6f, 0.5f) }, { 0 } } },
        { 13, { { 1, bcs::FVEC3(0.0f, -0.9f, -0.5f), bcs::FVEC3(1.7f, 2.0f, 0.55f) }, { 0 } } },
        { 14, { { 1, bcs::FVEC3(0.0f, -0.95f, -0.5f), bcs::FVEC3(1.45f, 1.6f, 0.5f) }, { 0 } } },
        { 15, { { 1, bcs::FVEC3(0.0f, -1.15f, -0.95f), bcs::FVEC3(3.3f, 1.5f, 1.0f) },
                { 1, bcs::FVEC3(0.0f, -2.7f, -0.55f), bcs::FVEC3(1.65f, 4.75f, 0.5f) } } },
        { 21, { { 1, bcs::FVEC3(0.0f, -0.7f, -0.6f), bcs::FVEC3(1.65f, 1.7f, 0.5f) }, { 0 } } },
        { 24, { { 1, bcs::FVEC3(0.0f, -0.95f, -0.55f), bcs::FVEC3(1.0f, 1.3f, 0.5f) }, { 0 } } },
        { -1 },
    };
}

msw::CGoalObjBasket::CGoalObjBasket(bcs::CTask *pParent, long idx) : CGoalObj(pParent, idx)
{
    m_pCollInfo = 0;
}

msw::CGoalObjBasket::~CGoalObjBasket() {}

void msw::CGoalObjBasket::Main()
{
    CGoalObj::Main();
    CollUpdate();
}

// The goal stands at the inner end of its team's court, offset by the
// default offset's z.
void msw::CGoalObjBasket::SetPosAuto()
{
    const bcs::FVEC3 &rOffset = GetDefaultOffset();
    CStageMan *pStage = CStageMan::GetInstance();
    long side = m_nGoalIdx & 1;
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    if (side != 0) {
        pos.z = rOffset.z + pStage->GetTeamCourtInnerAreaBack(side ^ 1);
    } else {
        pos.z = pStage->GetTeamCourtInnerAreaFront(side ^ 1) - rOffset.z;
    }
    SetPos(pos);
}

void msw::CGoalObjBasket::OnPostCreate()
{
    CGoalObj::OnPostCreate();
    m_BodyColl.Create(2, 0, 2, 0);
    for (long i = 0; i < 2; i++) {
        m_BodyColl.SetTargetFlag(i, 2);
        m_BodyColl.SetPushRank(i, 5);
        m_BodyColl.SetPushPower(i, 6);
    }
    m_pCollInfo = FindCollInfo(CGame::g_GameWork.GetStage());
}

// A unit is on when the stage has it and no camera looks at another goal.
void msw::CGoalObjBasket::CollUpdate()
{
    int bEnable;
    const COLL_INFO *pInfo = m_pCollInfo;

    for (long i = 0; i < 2; i++) {
        bEnable = pInfo != 0 ? CollUpdateSub(i, pInfo[i]) : 0;

        CGameCamera *pCamera = CGameCamera::GetInstance(0);
        if (pCamera != 0) {
            long goal = m_nGoalIdx;
            if (goal != pCamera->GetTargetGoalIdx()) {
                bEnable = 0;
            }
        }

        if (bEnable) {
            m_BodyColl.SetEnable(i);
            m_BodyColl.SetPushEnable(i);
        } else {
            m_BodyColl.SetDisable(i);
            m_BodyColl.SetPushDisable(i);
        }
    }
}

// Places unit idx; returns whether the unit is used.
int msw::CGoalObjBasket::CollUpdateSub(long idx, const COLL_INFO &rInfo)
{
    if (!rInfo.bEnable) {
        return 0;
    }

    float rotY = GetRot().y;
    bcs::FVEC3 pos;
    bcs::BCS_Vec_RotY(pos, rInfo.pos, rotY);
    bcs::FVEC3Add(&pos, &pos, &GetBoardPos());
    m_BodyColl.SetPos(idx, pos);
    m_BodyColl.SetYRot(idx, rotY);
    m_BodyColl.SetSize(idx, rInfo.size);
    return 1;
}

const msw::CGoalObjBasket::COLL_INFO *msw::CGoalObjBasket::FindCollInfo(long type) const
{
    for (const COLL_TABLE *pTable = sCollTable; pTable->nType >= 0; pTable++) {
        if (pTable->nType == type) {
            return pTable->aInfo;
        }
    }
    return 0;
}

msw::CGoalObjBasket_BossPackun::CGoalObjBasket_BossPackun(bcs::CTask *pParent, long idx, CNpcBossPackun *pNpc)
    : CGoalObjBasket(pParent, idx)
{
    m_pNpc = pNpc;
}

msw::CGoalObjBasket_BossPackun::~CGoalObjBasket_BossPackun() {}

unsigned long msw::CGoalObjBasket_BossPackun::GetMdlHandle() const
{
    if (m_pNpc != 0) {
        return m_pNpc->GetMdlHandle();
    }
    return 0;
}

long msw::CGoalObjBasket_BossPackun::GetRingNodeIdx() const
{
    if (m_pNpc != 0) {
        return m_pNpc->GetRingNodeIdx();
    }
    return 0;
}

// Only model 1, the boss's goal model, has a scene object.
bcs::CScnObj *msw::CGoalObjBasket_BossPackun::GetMdlObj(int idx)
{
    if (idx != 1) {
        return 0;
    }
    unsigned long handle = GetMdlHandle();
    if (handle == 0) {
        return 0;
    }
    return bcs::CScnObj::GetScnObj(handle);
}

// Unlike the other accessors this one does not check for the boss.
const bcs::CScnObj *msw::CGoalObjBasket_BossPackun::GetMdlObj(int idx) const
{
    if (idx != 1) {
        return 0;
    }
    unsigned long handle = m_pNpc->GetMdlHandle();
    if (handle == 0) {
        return 0;
    }
    return bcs::CScnObj::GetScnObj(handle);
}
