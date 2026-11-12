// msw::CComDrvBasketGame: running (MethdBKRun2, which turns aside to a
// dropped coin or a J-street panel on the way) and the Methd*/Canbe*
// wrappers of the character's actions (pass, trick move, dash, alley-oop,
// combination, air trick) with the tests the action picker uses. A file of
// its own with its own float constants, after game/basket/unit_AF3BC.cpp;
// the file name is not known. The original also had
// CComDrvBasketGame::CanbeRebound() const and CanbeReboundDunk() const here
// (each with a `static const float fMaxDist = 2.0f`, still in .rodata);
// nothing called them and the module link stripped their code, so their
// bodies are not known and they are not written.
#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CDropCoin.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CStage10Base.hpp"
#include "game/msw/CTournament.hpp"

namespace unit_B09B4_cpp {
// uncalled stand-in: the original's float pool starts with 3.5f, which no
// function left in the file uses (presumably one of the stripped functions
// above did). The module link strips this function.
float UnusedPoolHead(float f)
{
    return f * 3.5f;
}
}

// 70 degrees in radians, unused (as in CComManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {
    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi), and into (-pi, pi].
    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }

    // (guessed helper) rPos1 - rPos2.
    inline bcs::FVEC3 Diff(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 diff;
        return *bcs::FVEC3Sub(&diff, &rPos1, &rPos2);
    }

    // (guessed helper) The length of rVec in the XZ plane.
    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return PSVECMag((const Vec *)&xz);
    }

    // (guessed helper) The distance of two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 d = Diff(rPos1, rPos2);
        return LenXZ(d);
    }

    // (guessed helper) The computer manager of the basketball game.
    inline msw::CComManBasketGame *GetComManBK()
    {
        return static_cast<msw::CComManBasketGame *>(msw::CComMan::GetInstance());
    }
}

namespace msw {

int CComDrvBasketGame::MethdBKRunTest(bcs::FVEC3 &rPos, float fDist, float fDpm, int bKeepRunning)
{
    return MethdBKRun2(rPos, fDist, fDpm, bKeepRunning, 1);
}

int CComDrvBasketGame::MethdBKRun2(bcs::FVEC3 &rPos, float fDist, float fDpm, int bKeepRunning, int bTurnAround)
{
    bcs::FVEC3 asked = rPos;
    bcs::FVEC3 pos = rPos;
    NormalizeCanMovePos(pos);
    bcs::FVEC3 world = Local2World(pos, -1);

    int bDetour = 0;
    int bArrived = m_pChr->GetDist_ToPosXZ(world) <= fDist;
    if (bArrived && !bKeepRunning) {
        return bArrived;
    }

    bcs::FVEC3 target = world;

    // Pick up the nearest dropped coin on the way while the team can carry
    // more.
    if ((m_nFlag17C & 0x20) && GetRootActType() != 0x14) {
        unsigned long nMaxCoin = CGame::GetMaxCoin();
        if (CGame::GetTeamCoin(m_pChr->GetTeamNo()) < nMaxCoin) {
            CDropCoin *pNear = 0;
            float fMinDist;
            if (CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
                if (m_pChr->IsHoldBall()) {
                    fMinDist = CComMan::GetDatF(0x51B, GetComLvF());
                } else {
                    fMinDist = CComMan::GetDatF(0x523, GetComLvF());
                }
            } else if (m_pMarkChr != 0 && m_pMarkChr->IsHoldBall()) {
                fMinDist = CComMan::GetDatF(0x52B, GetComLvF());
            } else {
                fMinDist = CComMan::GetDatF(0x533, GetComLvF());
            }

            for (CDropCoin *pCoin = CComMan::DropCoin_GetFirst(); pCoin != 0; pCoin = CComMan::DropCoin_GetNext(pCoin)) {
                if (!pCoin->CanbePickup()) {
                    continue;
                }
                if (pCoin->GetPos().y > 2.0f) {
                    continue;
                }
                if (!CComManBasketGame::InMoveEnableArea(CComMan::World2Local(pCoin->GetPos(), -1))) {
                    continue;
                }
                float fCoinDist = DistXZ(pCoin->GetPos(), m_pChr->GetPos());
                if (fCoinDist > fMinDist) {
                    continue;
                }
                fMinDist = fCoinDist;
                pNear = pCoin;
            }

            if (pNear != 0) {
                bDetour = 1;
                target = pNear->GetPos();
            }
        }
    }

    // On the J-street stage, cross the nearest bonus panel ahead while the
    // team holds the ball.
    if (CGame::g_GameWork.GetStage() == 10) {
        CStage10Base::CPanelInfoBase *pBest = 0;
        if (IsMyTeamHoldBall(0) && !CComManBasketGame::IsJStreetBonus()) {
            float fRotTarget = m_pChr->GetRotY_ToPosXZ(target);
            float fMaxRotDiff = PI / 4.0f;
            float fMinDist = CComMan::GetDatF(0xFA3, GetComLvF());
            for (CStage10Base::CPanelInfoBase *pPanel = CComManBasketGame::GetJStreetPanelFirst(); pPanel != 0;
                 pPanel = CComManBasketGame::GetJStreetPanelNext(pPanel)) {
                if (pPanel->GetPanelBonusRate() <= 0) {
                    continue;
                }
                if (!CComManBasketGame::InMoveEnableArea(pPanel->GetPos())) {
                    continue;
                }
                float fPanelDist = DistXZ(pPanel->GetPos(), m_pChr->GetPos());
                if (fMinDist < fPanelDist) {
                    continue;
                }
                float fRotDiff = RoundRadPI(m_pChr->GetRotY_ToPosXZ(pPanel->GetPos()) - fRotTarget);
                if (fabsf(fRotDiff) > fMaxRotDiff) {
                    continue;
                }
                pBest = pPanel;
                fMinDist = fPanelDist;
            }
        }

        if ((m_nFlag17C & 0x40) && pBest != 0) {
            if (DistXZ(pBest->GetPos(), m_pChr->GetPos()) < 1.5f) {
                m_fUnk1DC = 0.0f;
                m_nFlag17C &= ~0x40;
            } else {
                bDetour = 1;
                target = pBest->GetPos();
            }
        }
    }

    target = CComMan::World2Local(target, -1);
    if (bTurnAround && GetPosTurnAround(target)) {
        NormalizeCanMovePos(target);
        bKeepRunning = 0;
        bDetour = 0;
    }
    world = Local2World(target, -1);

    if (!bKeepRunning && !bDetour) {
        if (m_nUnk1E8 == 0) {
            if (DistXZ(asked, m_MyPos) <= fDist) {
                return 1;
            }
        } else if (m_pChr->GetDist_ToPosXZ(world) <= fDist) {
            return 1;
        }
    }

    float fRot = m_pChr->GetRotY_ToPosXZ(world);
    if (m_pChr->IsHoldBall()) {
        if (m_pChr->GetState() == 0x2F) {
            float fRotToGoal = RoundRad(fRot - m_pChr->GetRotY_ToGoal());
            long nDir = CChrManBasket::GetInstance()->GetDir4ToBattle_(fRotToGoal);
            if (nDir == 0 || nDir == 2) {
                fDpm = 0.5f;
            }
        }
        if (m_fUnk1C8 == -1.0f) {
            m_fUnk1C8 = fDpm;
        } else {
            fDpm = m_fUnk1C8;
        }
        if (!m_pChr->MethdAutoDPM(fRot, fDpm)) {
            CChrCore::INPUT_WORK work;
            work.m_nUnk00 = 0;
            m_pChr->MethdRDB(&work, fRot);
        }
    } else {
        m_pChr->MethdSliding(fRot);
        m_pChr->MethdRun(fRot);
    }

    if (bDetour) {
        bArrived = 0;
    }
    return bArrived;
}

int CComDrvBasketGame::CanbeJumpShoot()
{
    if (static_cast<CComManBasketGame *>(CComMan::GetInstance())->CanbeShootState_(m_pChr->GetTeamNo())) {
        return CComDrvBasket::CanbeJumpShoot();
    }
    return 0;
}

int CComDrvBasketGame::MethdPass(CChrCore *pChr)
{
    return m_pChr->MethdPass(pChr, 0);
}

int CComDrvBasketGame::MethdPass(const bcs::FVEC3 &rPos)
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    float fCamRot = m_pChr->m_fCamRotY;
    float fRot = RoundRad(CComManBasket::GetRotY_P1ToP2XZ(m_MyPos, rPos) + fCamRot);
    return m_pChr->MethdPass(fRot, 0);
}

int CComDrvBasketGame::MethdPassBK(float fRot, int nArg)
{
    if (m_pChr->MethdAerialPass(fRot, nArg)) {
        return 1;
    }
    return m_pChr->MethdPass(fRot, nArg) != 0;
}

int CComDrvBasketGame::MethdPassRequest()
{
    if (m_pChr->CanbePassRequest()) {
        return m_pChr->MethdPassRequest();
    }
    return 0;
}

// A trick move to the side of the goal.
int CComDrvBasketGame::MethdTrickMove()
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    float fRot = m_pChr->GetRotY();
    float fRotDiff = RoundRadPI(m_pChr->GetRotY_ToPosXZ(m_pChr->GetTargetGoalPos()) - fRot);
    // (the slot takes a float; the side goes in as 1 or 0)
    int bRight = fRotDiff > 0.0f;
    return m_pChr->MethdTrickMove(bRight);
}

int CComDrvBasketGame::MethdPassSign()
{
    return static_cast<CChrCoreBasket *>(m_pChr)->MethdPassSign();
}

int CComDrvBasketGame::MethdReboundDunk()
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdReboundDunk();
}

int CComDrvBasketGame::CanbeAlleyoop() const
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->CanbeAlleyoop();
}

int CComDrvBasketGame::MethdAlleyoop()
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdAlleyoop();
}

int CComDrvBasketGame::MethdSpinMove(int bRight)
{
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdTrickMove(bRight ? 1.0f : -1.0f);
}

int CComDrvBasketGame::CanbeJumpBlock()
{
    if ((CChrManBasket::GetInstance()->GetFlagAC() & 2) && !(m_nFlag17C & 0x800)) {
        return 0;
    }
    return CComDrvBasket::CanbeJumpBlock();
}

int CComDrvBasketGame::MethdDash()
{
    if (CanbeDash()) {
        return m_pChr->MethdDash();
    }
    return 0;
}

int CComDrvBasketGame::CanbeItemThrow()
{
    if (m_fUnk1BC > 0.0f) {
        return 0;
    }
    return m_pChr->CanbeItemShoot();
}

int CComDrvBasketGame::CanbeCombination() const
{
    if (m_pChr->CanbeCombination()) {
        return IsAllComTeam();
    }
    return 0;
}

int CComDrvBasketGame::MethdCombination()
{
    if (!CanbeCombination()) {
        return 0;
    }
    static_cast<CChrManBasket *>(CChrMan::GetInstance())->PlayCombination_(m_pChr);
    return (static_cast<CChrCoreBasket *>(m_pChr)->m_nBasketFlag & 0x800) != 0;
}

int CComDrvBasketGame::CanbeAirTrick()
{
    if (!static_cast<CComManBasketGame *>(CComMan::GetInstance())->CanbeShootState_(m_pChr->GetTeamNo())) {
        return 0;
    }
    if (CTournament::IsMissionComSpecialShootDisable(m_pChr->GetTeamNo())) {
        return 0;
    }
    return m_pChr->CanbeAirTrick();
}

int CComDrvBasketGame::MethdAirTrick()
{
    if (CanbeAirTrick()) {
        return m_pChr->MethdAirTrick();
    }
    return 0;
}

int CComDrvBasketGame::CanbeActCallJumpShoot()
{
    if (!CanbeJumpShoot()) {
        return 0;
    }
    return !(m_fUnk1D4 > 0.0f);
}

int CComDrvBasketGame::CanbeActCallItemShoot()
{
    if (!CanbeJumpShoot()) {
        return 0;
    }
    return !(m_fUnk1D4 > 0.0f);
}

int CComDrvBasketGame::CanbeActCutEsp() const
{
    if (!m_pChr->IsHoldBall()) {
        return 0;
    }
    if (m_pChr->GetStatusFlag() & 0x1400) {
        return 0;
    }
    long nAct = GetRootActType();
    if (nAct == 0x4A || nAct == 0x3C) {
        return 0;
    }
    return 1;
}

// Whether a "?" panel is near enough to fetch (with the stock full of
// items, only while the team can carry more coins).
int CComDrvBasketGame::CanbeActGetHatenaPanel() const
{
    if (m_pChr->IsHaveStockItem()) {
        unsigned long nMaxCoin = CGame::GetMaxCoin();
        if (CGame::GetTeamCoin(m_pChr->GetTeamNo()) >= nMaxCoin) {
            return 0;
        }
    }
    unsigned long nPanel = CComMan::QPanel_GetNear(m_MyPos, 1);
    if (nPanel == 0) {
        return 0;
    }
    bcs::FVEC3 pos = CComMan::QPanel_GetPos(nPanel);
    if (!CComManBasketGame::InMoveEnableArea(pos)) {
        return 0;
    }
    float fArea = GetSearchHatenaPanelArea();
    return !(CComManBasket::GetDistXZ(pos, m_MyPos) > fArea);
}

// Whether a green shell lies near enough to fetch (beach stage only).
int CComDrvBasketGame::CanbeActGetGShell() const
{
    if (CGame::g_GameWork.GetStage() != 2) {
        return 0;
    }
    if (m_pChr->IsHaveStockItem()) {
        return 0;
    }
    CGimmickMan::GetInstance();
    float fMinDist = 9999.9f;
    CItem *pNear = 0;
    for (CItem *pItem = CGimmickMan::ItemGetFirst(); pItem != 0; pItem = CGimmickMan::ItemGetNext(pItem)) {
        if (pItem->IsTouchDown()) {
            continue;
        }
        if (!pItem->CanbePickup()) {
            continue;
        }
        if (!CComManBasketGame::InMoveEnableArea(pItem->GetPos())) {
            continue;
        }
        float fDist = CComManBasket::GetDistXZ(pItem->GetPos(), m_pChr->GetPos());
        if (fDist > fMinDist) {
            continue;
        }
        fMinDist = fDist;
        pNear = pItem;
    }
    if (pNear == 0) {
        return 0;
    }
    return !(fMinDist > GetSearchHatenaPanelArea());
}

} // namespace msw
