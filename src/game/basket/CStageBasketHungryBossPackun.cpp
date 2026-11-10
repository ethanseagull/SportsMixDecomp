// msw::CStageBasketHungryBossPackun: base of the Hungry Boss Packun stages
// of the basket game, where the boss carries goal 0 about the court.
#include "game/msw/CStageBasketHungryBossPackun.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStageMdlListNode.hpp"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CStageBasketHungryBossPackun::CStageBasketHungryBossPackun(bcs::CTask *pParent)
    : CStageManBasket(pParent), m_pBgNode(0)
{
    for (int i = 0; i < 4; i++) {
        m_apNpc[i] = 0;
        m_apGoal[i] = 0;
    }
}

msw::CStageBasketHungryBossPackun::~CStageBasketHungryBossPackun()
{
}

void msw::CStageBasketHungryBossPackun::Main_(float frame)
{
    UpdateCourtSize_();

    // Goal 0 follows the boss that carries it.
    CGoalObj *pGoal = GetGoal(0);
    CNpcBossPackun *pNpc = m_apNpc[0];

    if (pGoal != 0 && pNpc != 0) {
        const bcs::FVEC3 &rRot = pNpc->GetRot();

        pGoal->SetPos(pNpc->GetPos());
        // Built from the three floats (z loaded first), not copied: a copy
        // interleaves the loads and stores.
        pGoal->SetRot(bcs::FVEC3(rRot.x, rRot.y, rRot.z));
    }
    CStageManBasket::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasketHungryBossPackun::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg$", 10 },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasketHungryBossPackun::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                         CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile,
                                                         CArn &rArn, long n)
{
    if (pRule->m_nId == 10) {
        m_pBgNode = pNode;
    }
    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasketHungryBossPackun::OnSceneEntry()
{
    bcs::CScnObj *pObj = m_pBgNode != 0 ? m_pBgNode->GetObj() : 0;

    if (pObj != 0) {
        pObj->CalcRootMtx();
    }
}

void msw::CStageBasketHungryBossPackun::UpdateCourtSize_()
{
    // 11.5 is the court depth of CStageManBasket.
    for (long i = 0; i < 2; i++) {
        SetTeamCourtOffsetZ(i, GetBaseCourtDepth_() - 11.5f);
    }
}

float msw::CStageBasketHungryBossPackun::GetCourtLeft() const
{
    return GetTeamCourtLeft(0);
}

float msw::CStageBasketHungryBossPackun::GetCourtRight() const
{
    return GetTeamCourtRight(0);
}

float msw::CStageBasketHungryBossPackun::GetCourtFront() const
{
    return GetTeamCourtFront(1);
}

float msw::CStageBasketHungryBossPackun::GetCourtBack() const
{
    return GetTeamCourtBack(0);
}

float msw::CStageBasketHungryBossPackun::GetBaseCourtWidth_() const
{
    return BASKET_PRM_BOSS_PACKUN_COURT_WIDTH;
}

float msw::CStageBasketHungryBossPackun::GetBaseCourtDepth_() const
{
    return BASKET_PRM_BOSS_PACKUN_COURT_DEPTH;
}

void msw::CStageBasketHungryBossPackun::CreateGoal()
{
    UpdateCourtSize_();

    for (long i = 0; i < 2; i++) {
        CGoalObjBasket_BossPackun *pGoal = new CGoalObjBasket_BossPackun(this, i, 0);

        if (pGoal != 0) {
            pGoal->Create();
            pGoal->SetDefaultOffset(GetGoalOffsetPos());
            pGoal->SetPosAuto();
            pGoal->SetRotAuto();
            pGoal->SceneEntry();
            m_apGoal[i] = pGoal;

            if (i == 0) {
                // The boss stands where goal 0 is; it carries the goal from
                // then on (Main_), so the goal stops placing itself.
                bcs::FVEC3 pos = GetGoalPos(i);
                const bcs::FVEC3 &rRot = GetGoalRot(i); // not used

                m_apNpc[i] = static_cast<CChrManBasket *>(CChrMan::GetInstance())->CreateBossPackun_(pos, i);
                pGoal->SetNpc(m_apNpc[i]);
            }
            pGoal->SetAutoPosFlag(0);
            pGoal->SetAutoRotFlag(0);
        } else if (pGoal != 0) {
            // Never reached. The original has this guarded delete in the
            // failure branch (most likely a clean-up macro); it is kept for
            // the code it produces.
            delete pGoal;
        }
    }
}
