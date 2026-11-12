// msw::CComDrvDodgeGame's interrupt and think functions: ActInterrupt keeps
// the driver's timers and breaks off the running action for something more
// urgent, ThkMain and its helpers draw the next action by the level's rates.
// The second half of what VolleyF has in game/volley/unit_AB1D8.cpp; a
// file of its own after game/dodge/unit_A4AE8.cpp. The file name is not
// known.
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManDodgeGame.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Rand.hpp"

namespace {
    // (guessed helper, as in game/dodge/unit_A40EC.cpp) v brought into
    // lo..hi.
    inline long ClampI(long v, long lo, long hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }
}

namespace msw {

// Breaks off the running action for something more urgent, after keeping
// the driver's timers.
void CComDrvDodgeGame::ActInterrupt()
{
    CBall *pBall = GetActiveBall();
    if (pBall == 0) {
        return;
    }

    // Shoot before the shot clock runs out.
    if (m_pChr->IsHoldBall() && CGame::GetMaxShotTime() > 0.0f && CGame::g_GameWork.m_fShotTime < 60.0f) {
        MethdShoot();
        return;
    }

    if (pBall->CanbeCatch() && !CComManDodgeGame::IsBallOffensive(m_pChr, pBall->GetBallNo())) {
        m_fFreeBallTime += m_pChr->GetFrameStep();
    } else {
        m_fFreeBallTime = 0.0f;
    }

    if (m_fUnk21C > 0.0f) {
        m_fUnk21C -= m_pChr->GetFrameStep();
    } else {
        m_nUnk214 = 0;
        m_fUnk21C = 0.0f;
    }

    if (m_fUnk1F0Time > 0.0f) {
        m_fUnk1F0Time -= m_pChr->GetFrameStep();
    } else {
        m_fUnk1F0 = RandRangeF(-1.0f, 1.0f);
        m_fUnk1F0Time = RandRangeF(30.0f, 60.0f);
    }

    UpdateStance();

    if (m_bDefStanceTime && m_fDefStanceTime >= 0.0f) {
        m_fDefStanceTime -= m_pChr->GetFrameStep();
        if (m_fDefStanceTime < 0.0f) {
            m_bDefStanceTime = 0;
            SetDefStanceMode(1);
        }
    }

    // Start again when the driver becomes (or stops being) the team's
    // assigned outfield character, unless the ball is in state 6 or 7.
    long nBallState = pBall->GetState();
    if (nBallState != 6 && nBallState != 7) {
        long nAssign = CComManDodgeGame::GetAssignOuterChrCoreIdx(m_nMyTeam);
        if (nAssign == m_nMyChrIdx) {
            if (!m_bAssignOuter) {
                m_bAssignOuter = 1;
                Reset();
                return;
            }
        } else if (m_bAssignOuter) {
            m_bAssignOuter = 0;
            Reset();
            return;
        }
    }

    // Steal a pass flying past.
    if (GetRootActType() != 0x17 && !m_pChr->IsHoldBall() && m_nPassStealKind && pBall->IsPassMode()) {
        bcs::FVEC3 vel = pBall->GetVelocity();
        vel.y = 0.0f;
        float fDis = GetDisXZ(m_BallWorldPos, m_MyWorldPos);
        float fDisMin = 3.0f;
        float fDisMax = 3.2f;
        float fHeight = 1.5f;
        float fWidth = 0.8f;
        float fLength = 4.0f;
        if (m_nPassStealKind == 2) {
            fWidth = 3.0f;
            fDisMax = 6.0f;
            fLength = 2.5f;
        }
        if (fDis >= fDisMin && fDis <= fDisMax && m_BallWorldPos.y >= fHeight &&
            CComMan::IsPointInDirFaceXZ(m_MyWorldPos, m_BallWorldPos, vel, fWidth, fLength)) {
            Reset();
            ActCallPassSteal(m_nPassStealKind);
            return;
        }
    }

    // Roll out of the way of a shot.
    if (GetRootActType() != 0x18) {
        if (m_nDodgeRollKind == 1) {
            float fWidth = 0.8f;
            float fTime = 2.2f;
            if (CComManDodgeGame::IsReflectDodgeRoll(m_pChr)) {
                fWidth = 1.0f;
                fTime = 5.2f;
            }
            if (Util_GetMaybeHitChrBall(m_pChr, fWidth, fTime) >= 0) {
                Reset();
                ActCallDodgeRoll(m_nDodgeRollKind);
                return;
            }
        } else if (m_nDodgeRollKind == 2) {
            if (Util_GetMaybeHitChrBall(m_pChr, 2.0f, 4.0f) >= 0) {
                Reset();
                ActCallDodgeRoll(m_nDodgeRollKind);
                return;
            }
        }
        if (ShoudMissDodgeRoll()) {
            m_nUnk228 = -1;
            Reset();
            ActCallDodgeRoll(1);
            return;
        }
    }

    if (ShoudDivingCatch()) {
        Reset();
        ActCallDivingCatch();
        return;
    }
    if (ShoudGetBall()) {
        if (m_pChr->IsDodgeInfield()) {
            Reset();
            ActCallGetBall();
        } else {
            Reset();
            ActCallOutFieldGetBall();
        }
        return;
    }
    if (ShoudEscGimmick()) {
        Reset();
        ActCallEscGimmick();
        return;
    }
    if (ShoudItemThrowAndQPanel()) {
        Reset();
        ActCallThrowItem();
    }
}

// Picks the next action when none is running.
void CComDrvDodgeGame::ThkMain()
{
    long nChr;
    bcs::FVEC3 pos;

    // (both results unused)
    bcs::BCS_RandMax(1L);
    CChrMan::GetInnerChrNum(m_nEmTeam);

    if (m_pChr->IsDodgeInfield() && !m_pChr->GetUnk17B()) {
        if (CGame::g_GameWork.m_nFlag18 & 8) {
            ActCallJumpBall();
            return;
        }

        if (IsSpMode()) {
            if (GetLastSpChrCore() == 0) {
                return;
            }
            if (GetLastSpChrCore() == m_pChr) {
                WraperActCallOffSpStart();
            } else if (m_nMyTeam == GetLastSpChrCore()->GetTeamNo()) {
                WraperActCallOffSpSub();
            } else {
                WraperActCallDefSpStart();
            }
            return;
        }

        if (m_pChr->IsHoldBall()) {
            nChr = GetNearEmChrCoreInMyCourt(m_MyPos);
            if (IsNoBorder()) {
                if (LotRate() < 20) {
                    ActCallRunRand();
                    return;
                }
                CChrCore *pChr = CComManDodgeGame::GetInnerChrCoreRand(m_nEmTeam, 0, 1);
                if (pChr != 0) {
                    nChr = pChr->GetPlayerNo();
                }
            }
            if (m_pChr->GetState() == CChrCore::STATE_JUMP) {
                return;
            }
            if (nChr >= 0) {
                ThkMain_InFieldOnBall_EmMyCourt(nChr);
            } else {
                ThkMain_InFieldOnBall();
            }
            return;
        }

        int bFreeBall = 0;
        if (GetNearFreeBallPos(pos) >= 0) {
            bFreeBall = 1;
        }
        if (bFreeBall) {
            ActCallGetBall();
            return;
        }
        CChrCore *pHolder = CComMan::GetHoldBallChrCore(-1);
        if (pHolder != 0) {
            long nHolderTeam = pHolder->GetTeamNo();
            if (nHolderTeam == m_nMyTeam) {
                if (pHolder->IsDodgeInfield()) {
                    ThkMain_InFieldMyTeamInOnBall();
                } else {
                    ThkMain_InFieldMyTeamOutOnBall();
                }
            } else {
                ThkMain_InFieldEmOnBall();
            }
        } else {
            ThkMain_InFieldEmOnBall();
        }
        return;
    }

    if (m_pChr->IsDodgeInfield()) {
        if (CGame::g_GameWork.GetStage() != 3) {
            ActCallGoBack();
            return;
        }
        if (m_pChr->IsHoldBall()) {
            long nChr = GetTargetChrInEmZone();
            if (nChr >= 0) {
                ThkMain_InFieldOnBall_EmMyCourt(nChr);
            }
            return;
        }
        if (GetNearFreeBallPos(pos) >= 0) {
            ActCallGetBall();
            return;
        }
        if (IsNoBorder()) {
            if (LotRate() < 80) {
                ActCallRunRand();
            } else {
                ActCallGoBack();
            }
            return;
        }
        ActCallGoBack();
        return;
    }

    if (m_pChr->IsHoldBall()) {
        ThkMain_OutFieldOnBall();
        return;
    }
    bcs::FVEC3 ballPos;
    if (GetNearFreeBallPos(ballPos) >= 0) {
        ActCallOutFieldGetBall();
        return;
    }
    CComManDodgeGame::GetAssignOuterChrCoreIdx(m_nMyTeam); // (result unused)
    if (m_bAssignOuter && m_nMyChrIdx == GetNearBallOppositeChr(1)) {
        ActCallOutFieldOpposite(1);
        return;
    }
    ThkMain_OutFieldOffBall();
}

// In the infield with the ball: shoot (four kinds), change the target, run
// (to charge, forward, back, sideways), fetch a question panel or a coin,
// or start a combination with a team mate.
void CComDrvDodgeGame::ThkMain_InFieldOnBall()
{
    static const long nNum = 12;

    long nArea = Util_GetOnBallArea();
    if (nArea < 0) {
        return;
    }
    long nMyInner = CChrMan::GetInnerChrNum(m_nMyTeam);
    long nEmInner = CChrMan::GetInnerChrNum(m_nEmTeam);

    // Each area has its own table of rates, eight entries apart.
    long nId;
    if (nArea == 0) {
        nId = 0x2A6;
    } else if (nArea == 1) {
        nId = 0x306;
    } else if (nArea == 2) {
        nId = 0x366;
    } else {
        nId = 0x3C6;
    }
    long aRate[nNum];
    for (long i = 0; i < nNum; i++) {
        aRate[i] = CComMan::GetDatI(nId + i * 8, GetComLvF());
        aRate[i] = ClampI(aRate[i], 0, 1000);
    }

    aRate[0] = ShootChanceHosei(aRate[0]);
    if (nEmInner <= 1 || m_nUnk20C > 0) {
        aRate[4] = 0;
    }
    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[9] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[10] = 0;
    }
    if (IsExistPadDrv() || nMyInner <= 1 || CGame::g_GameWork.GetStage() == 3) {
        aRate[11] = 0;
    }

    switch (LotProbability(aRate, nNum)) {
        case 0:
            ActCallOnballAttack(0);
            break;
        case 1:
            ActCallOnballAttack(1);
            break;
        case 2:
            ActCallOnballAttack(2);
            break;
        case 3:
            ActCallOnballAttack(3);
            break;
        case 4:
            ActCallChangeTarget();
            break;
        case 5:
            ActCallOffRunCharge();
            break;
        case 6:
            ActCallOffRunFront();
            break;
        case 7:
            ActCallOffRunBack();
            break;
        case 8:
            ActCallOffRunSide();
            break;
        case 9:
            ActCallGetQPanel();
            break;
        case 10:
            ActCallGetDropCoin();
            break;
        case 11:
            ActCallCombination();
            break;
    }
}

// In the infield with the ball and an enemy (nChr) in the own court: shoot,
// or run at it; the rates differ when it is close.
void CComDrvDodgeGame::ThkMain_InFieldOnBall_EmMyCourt(long nChr)
{
    int bNear = 0;
    if (nChr >= 0 && CComMan::GetChrDisXZ(m_nMyChrIdx, nChr) <= 5.0f) {
        bNear = 1;
    }

    long aRate[4];
    long nNum;
    if (bNear) {
        aRate[0] = CComMan::GetDatI(0x4B6, GetComLvF());
        aRate[1] = CComMan::GetDatI(0x4BE, GetComLvF());
        aRate[2] = CComMan::GetDatI(0x4C6, GetComLvF());
        aRate[3] = CComMan::GetDatI(0x4CE, GetComLvF());
        nNum = 4;
    } else {
        aRate[0] = CComMan::GetDatI(0x4D6, GetComLvF());
        aRate[1] = CComMan::GetDatI(0x4DE, GetComLvF());
        aRate[2] = CComMan::GetDatI(0x4E6, GetComLvF());
        aRate[3] = CComMan::GetDatI(0x4EE, GetComLvF());
        nNum = 4;
    }

    switch (LotProbability(aRate, nNum)) {
        case 0:
            ActCallOnballAttack(0);
            break;
        case 1:
            ActCallOnballAttack(1);
            break;
        case 2:
            ActCallOnballAttack(3);
            break;
        case 3:
            ActCallOffRunChr(nChr, RandRange(60.0f, 180.0f));
            break;
    }
}

// In the infield while the other team has the ball: run away or about,
// fetch a question panel or a coin, or throw an item.
void CComDrvDodgeGame::ThkMain_InFieldEmOnBall()
{
    long aRate[16];
    aRate[0] = CComMan::GetDatI(0x53E, GetComLvF());
    aRate[1] = CComMan::GetDatI(0x546, GetComLvF());
    aRate[2] = CComMan::GetDatI(0x54E, GetComLvF());
    aRate[3] = CComMan::GetDatI(0x556, GetComLvF());
    aRate[4] = CComMan::GetDatI(0x55E, GetComLvF());

    // With no centre line, half the escapes become random runs.
    if (IsNoBorder()) {
        aRate[1] += aRate[0];
        aRate[0] /= 2;
    }
    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[2] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[3] = 0;
    }
    if (!IsHaveMyStockItem() || IsExistPadDrv()) {
        aRate[4] = 0;
    }

    switch (LotProbability(aRate, 5)) {
        case 0:
            ActCallRunEsc();
            break;
        case 1:
            ActCallRunRand();
            break;
        case 2:
            ActCallGetQPanel();
            break;
        case 3:
            ActCallGetDropCoin();
            break;
        case 4:
            ActCallThrowItem();
            break;
    }
}

// In the infield while a team mate in the infield has the ball.
void CComDrvDodgeGame::ThkMain_InFieldMyTeamInOnBall()
{
    long aRate[16];
    aRate[0] = CComMan::GetDatI(0x566, GetComLvF());
    aRate[1] = CComMan::GetDatI(0x56E, GetComLvF());
    aRate[2] = CComMan::GetDatI(0x576, GetComLvF());
    aRate[3] = CComMan::GetDatI(0x57E, GetComLvF());
    aRate[4] = CComMan::GetDatI(0x586, GetComLvF());

    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[2] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[3] = 0;
    }
    if (!IsHaveMyStockItem() || IsExistPadDrv()) {
        aRate[4] = 0;
    }

    switch (LotProbability(aRate, 5)) {
        case 0:
            ActCallGoHome();
            break;
        case 1:
            ActCallRunRand();
            break;
        case 2:
            ActCallGetQPanel();
            break;
        case 3:
            ActCallGetDropCoin();
            break;
        case 4:
            ActCallThrowItem();
            break;
    }
}

// In the infield while a team mate in the outfield has the ball; a
// combination is possible as well.
void CComDrvDodgeGame::ThkMain_InFieldMyTeamOutOnBall()
{
    long aRate[16];
    aRate[0] = CComMan::GetDatI(0x58E, GetComLvF());
    aRate[1] = CComMan::GetDatI(0x596, GetComLvF());
    aRate[2] = CComMan::GetDatI(0x59E, GetComLvF());
    aRate[3] = CComMan::GetDatI(0x5A6, GetComLvF());
    aRate[4] = CComMan::GetDatI(0x5AE, GetComLvF());
    aRate[5] = CComMan::GetDatI(0x5B6, GetComLvF());

    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[2] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[3] = 0;
    }
    if (!IsHaveMyStockItem() || IsExistPadDrv()) {
        aRate[4] = 0;
    }
    if (IsExistPadDrv() || CGame::g_GameWork.GetStage() == 3) {
        aRate[5] = 0;
    }

    switch (LotProbability(aRate, 6)) {
        case 0:
            ActCallGoHome();
            break;
        case 1:
            ActCallRunRand();
            break;
        case 2:
            ActCallGetQPanel();
            break;
        case 3:
            ActCallGetDropCoin();
            break;
        case 4:
            ActCallThrowItem();
            break;
        case 5:
            ActCallCombination();
            break;
    }
}

// In the outfield with the ball: shoot, change the target, move along the
// line, or fetch a question panel or a coin. The higher 0x58 is, the less
// the driver moves about.
void CComDrvDodgeGame::ThkMain_OutFieldOnBall()
{
    long nEmInner = CChrMan::GetInnerChrNum(m_nEmTeam);

    int bNear = 0;
    long nChr = GetNearEmInnerChr(m_MyPos, 1, 0);
    if (nChr >= 0 && CComMan::GetChrDisXZ(m_nMyChrIdx, nChr) <= 5.0f) {
        bNear = 1;
    }

    bcs::FVEC3 home;
    long aRate[9];
    long nNum;
    if (bNear) {
        aRate[0] = CComMan::GetDatI(0x19E, GetComLvF());
        aRate[1] = CComMan::GetDatI(0x1A6, GetComLvF());
        aRate[2] = CComMan::GetDatI(0x1AE, GetComLvF());
        aRate[3] = CComMan::GetDatI(0x1B6, GetComLvF());
        aRate[4] = CComMan::GetDatI(0x1BE, GetComLvF());
        aRate[5] = CComMan::GetDatI(0x1C6, GetComLvF());
        aRate[6] = CComMan::GetDatI(0x1CE, GetComLvF());
        aRate[7] = CComMan::GetDatI(0x1D6, GetComLvF());
        aRate[8] = CComMan::GetDatI(0x1DE, GetComLvF());
        nNum = 9;
    } else {
        aRate[0] = CComMan::GetDatI(0x1E6, GetComLvF());
        aRate[1] = CComMan::GetDatI(0x1EE, GetComLvF());
        aRate[2] = CComMan::GetDatI(0x1F6, GetComLvF());
        aRate[3] = CComMan::GetDatI(0x1FE, GetComLvF());
        aRate[4] = CComMan::GetDatI(0x206, GetComLvF());
        aRate[5] = CComMan::GetDatI(0x20E, GetComLvF());
        aRate[6] = CComMan::GetDatI(0x216, GetComLvF());
        aRate[7] = CComMan::GetDatI(0x21E, GetComLvF());
        aRate[8] = CComMan::GetDatI(0x226, GetComLvF());
        nNum = 9;
    }

    aRate[0] = ShootChanceHosei(aRate[0]);
    if (nEmInner <= 1 || m_nUnk20C > 0) {
        aRate[4] = 0;
    }
    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[7] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[8] = 0;
    }
    if (m_fUnk58 > 180.0f) {
        aRate[5] /= 4;
        aRate[6] /= 4;
    } else if (m_fUnk58 > 120.0f) {
        aRate[5] /= 2;
        aRate[6] /= 2;
    }
    GetHomePos(home);
    if (GetDisXZ(m_MyPos, home) < 2.0f) {
        aRate[5] = 0;
    }

    switch (LotProbability(aRate, nNum)) {
        case 0:
            ActCallOnballAttack(0);
            break;
        case 1:
            ActCallOnballAttack(1);
            break;
        case 2:
            ActCallOnballAttack(2);
            break;
        case 3:
            ActCallOnballAttack(3);
            break;
        case 4:
            ActCallChangeTarget();
            break;
        case 5:
            ActCallOutFieldHome();
            break;
        case 6:
            ActCallOutFieldRand();
            break;
        case 7:
            ActCallOutFieldGetQpanel();
            break;
        case 8:
            ActCallOutFieldGetDropCoin();
            break;
    }
}

// In the outfield without the ball.
void CComDrvDodgeGame::ThkMain_OutFieldOffBall()
{
    long aRate[16];
    aRate[0] = CComMan::GetDatI(0x22E, GetComLvF());
    aRate[1] = CComMan::GetDatI(0x236, GetComLvF());
    aRate[2] = CComMan::GetDatI(0x23E, GetComLvF());
    aRate[3] = CComMan::GetDatI(0x246, GetComLvF());
    aRate[4] = CComMan::GetDatI(0x24E, GetComLvF());

    if (!IsShoudGetQPanelorFallGShell()) {
        aRate[2] = 0;
    }
    if (!IsShoudGetDropCoin()) {
        aRate[3] = 0;
    }
    if (!IsHaveMyStockItem() || IsExistPadDrv()) {
        aRate[4] = 0;
    }

    switch (LotProbability(aRate, 5)) {
        case 0:
            ActCallOutFieldHome();
            break;
        case 1:
            ActCallOutFieldRand();
            break;
        case 2:
            ActCallOutFieldGetQpanel();
            break;
        case 3:
            ActCallOutFieldGetDropCoin();
            break;
        case 4:
            ActCallThrowItem();
            break;
    }
}

// The shot's rate raised (by the level's factor) when the locked target is
// down or stunned and within the level's range; kept within 0 to 1000.
long CComDrvDodgeGame::ShootChanceHosei(long nRate)
{
    long nTarget = m_pChr->GetDodgeLockTargetChrIdx();
    if (nTarget < 0) {
        return nRate;
    }
    CChrCore *pTarget = CChrMan::GetChrCore(nTarget);
    if (pTarget == 0) {
        return nRate;
    }
    if (pTarget->GetState() == CChrCore::STATE_DAMAGE || (pTarget->GetStatusFlag() & 0x10000)) {
        bcs::FVEC3 pos = CComMan::GetChrPos(pTarget);
        float fDis = GetDisXZ(pos, m_MyPos);
        float fRange;
        float fRate;
        if (m_pChr->IsDodgeInfield()) {
            fRange = CComMan::GetDatF(0x52E, GetComLvF());
            fRate = CComMan::GetDatF(0x536, GetComLvF());
        } else {
            fRange = CComMan::GetDatF(0x286, GetComLvF());
            fRate = CComMan::GetDatF(0x28E, GetComLvF());
        }
        if (fDis <= fRange) {
            nRate = nRate * fRate;
        }
    }
    if (nRate > 1000) {
        return 1000;
    }
    return nRate < 0 ? 0 : nRate;
}

} // namespace msw
