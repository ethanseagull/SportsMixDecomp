// msw::CComDrvHockeyGame: the interrupt checks ActInterrupt makes every
// frame and the thinking ThkMain does when no action runs (field players of
// the hockey game). A file of its own (its float constants are pooled
// again); the file name is not known.
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManHockey.hpp"
#include "game/msw/CComManHockeyGame.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/CSimpleSortBase.hpp"

namespace {
    typedef bcs::CSimpleSortBase<32>::SORT SORT;

    // Status bit 16 (meaning not known; the keeper does not guard the
    // goal while it is set).
    inline int IsStatusBit16(const msw::CChrCore *pChr)
    {
        return (pChr->GetStatusFlag() >> 16) & 1;
    }

    // Whether the character has a star (condition 1).
    inline int IsStar(const msw::CChrCore *pChr)
    {
        return pChr->GetComLevelWork() == 1;
    }

    // The distance between two points in the XZ plane (name guessed).
    inline float DisXZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1)
    {
        bcs::FVEC3 pos0(rPos0.x, 0.0f, rPos0.z);
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        return bcs::FVEC3Dist(&pos0, &pos1);
    }

}

namespace msw {

// Drops the shots a pass asked for (see PassMsgJobs).
inline void CComDrvHockeyGame::ClearShootReq()
{
    m_nUnk3FC = 0;
    m_bShootReq = 0;
    m_bItemShootReq = 0;
}

// Skate away from an enemy with a star, unless the driver has one itself.
inline int CComDrvHockeyGame::ThkInterruptEscapeFromStar()
{
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (m_pChr->GetComLevelWork() == 1) {
        return 0;
    }
    CChrCore *pEnemy = GetNearStarEnemy();
    if (pEnemy == 0) {
        return 0;
    }
    Reset();
    ActCallGoEscape(pEnemy);
    return 1;
}

void CComDrvHockeyGame::ActInterrupt()
{
    if (!IsEntryAct(0xA)) {
        m_nUnk410 = 0;
    }
    if (!IsEmTeamHoldBall(-1)) {
        m_nUnk3B0 = 0;
        m_fUnk3B4 = 0.0f;
    }
    long nShooter = -1;
    if (!AreYouShot(1, nShooter)) {
        m_nUnk3BC = 0;
    }
    if (!isEnemyPass()) {
        m_nUnk3C0 = 0;
    }
    if (!m_pChr->IsHoldBall() || m_pChr->GetUnk17B() == 0) {
        m_nUnk39C = 0;
    }
    m_fUnk440 += m_pChr->GetFrameStep();

    if (!isFaceoffEnd() || m_pChr->IsHockeyDamage() || IsEntryAct(0x23)) {
        return;
    }
    if (IsEntryAct(0x19)) {
        return;
    }
    if (m_nUnk35C != 0) {
        m_fUnk360 -= m_pChr->GetFrameStep();
        if (m_fUnk360 <= 0.0f) {
            m_fUnk360 = 0.0f;
            m_nUnk35C = 0;
            Reset();
            return;
        }
    }

    if (m_pChr->IsHockeyFighting()) {
        m_nUnk42C = 1;
        SetThkStatus(2);
        if (!IsEntryAct(0x1B)) {
            Reset();
            ActCallFight();
        }
        return;
    }
    if (m_nUnk42C != 0) {
        if (m_pChr->GetState() == 0x56 || m_pChr->GetState() == 0xA) {
            return;
        }
        m_nUnk42C = 0;
    }

    if (isNowFighting()) {
        if (!IsEntryAct(0xB) && m_nUnk430 != 0) {
            m_nUnk430 = 0;
            if (LotRate() < CComMan::GetDatI(0x5A9, GetHockeyComLvF())) {
                Reset();
                ActCallGetBall(2);
                return;
            }
        }
    } else {
        m_nUnk430 = 1;
    }

    if (m_nUnk370 == 0 && isComCombination()) {
        m_nUnk370 = 1;
        return;
    }
    if (m_pChr->IsHockeyNowCombinationMode()) {
        long nReceiver;
        if (!AreYouPass(0, nReceiver)) {
            if (m_nUnk36C == 0) {
                m_nUnk36C = 1;
                ClearShootReq();
                if (m_nUnk370 != 0) {
                    if (m_pChr->IsHoldBall()) {
                        if (GetRootActType() != 0x25) {
                            Reset();
                            ActCallCombination();
                        }
                    } else {
                        m_nUnk36C = 0;
                        m_nUnk370 = 0;
                    }
                } else {
                    Reset();
                }
            }
            return;
        }
        m_nUnk36C = 0;
        m_nUnk370 = 0;
    } else {
        m_nUnk36C = 0;
        m_nUnk370 = 0;
    }

    if (ThkInterruptEscape()) {
        return;
    }
    if (m_nSpShootChr >= 0) {
        SetThkStatus(7);
        if (CheckEnemyCore(m_nSpShootChr) == 1) {
            if (GetRootActType() != 0x1A) {
                Reset();
                ActCallSpDefense(m_nSpShootChr);
            }
        } else if (GetRootActType() != 0x19 && GetRootActType() != 0x1C) {
            Reset();
            if (m_nSpShootChr == m_pChr->GetPlayerNo()) {
                ActCallSpShoot();
            } else {
                ActCallSpOffense();
            }
        }
    } else {
        ThkInterruptItemThrow();
        if (ThkInterruptOnBaller()) {
            return;
        }
        if (ThkInterruptMyTeamHoldBall()) {
            return;
        }
        if (ThkInterruptEnemyTeamHoldBall()) {
            return;
        }
        if (ThkInterruptLoseBall()) {
            return;
        }
    }
}

// Once in a while, throw the item at an enemy it would hit.
void CComDrvHockeyGame::ThkInterruptItemThrow()
{
    m_fUnk390 += m_pChr->GetFrameStep();
    if (CComMan::GetDatF(0xF85, GetHockeyComLvF()) <= m_fUnk390) {
        m_nUnk394 = 1;
        m_fUnk390 = 0.0f;
    } else {
        m_nUnk394 = 0;
    }
    if (m_nUnk394 != 0 && CanbeItemShoot()) {
        long aEnemy[4];
        long nNum = 0;
        for (long i = 0; i < GetEnemyNum(); i++) {
            if (CheckThrowItem(GetEnemyCore(i))) {
                aEnemy[nNum] = i;
                nNum++;
            }
        }
        if (nNum != 0) {
            if (nNum == 1) {
                const CChrCore *pEnemy = GetEnemyCore(aEnemy[0]);
                MethdItemShoot(pEnemy->GetPos());
            } else {
                const CChrCore *pEnemy = GetEnemyCore(aEnemy[CComManHockey::GetRandValueI(0, nNum - 1)]);
                MethdItemShoot(pEnemy->GetPos());
            }
        }
    }
}

// On a stage with something to escape from, skate to safety; while it
// runs, keep shooting or passing when the puck is held.
int CComDrvHockeyGame::ThkInterruptEscape()
{
    if (!IsEntryAct(0x18)) {
        bcs::FVEC3 target;
        bcs::FVEC3 pos = m_MyWorldPos;
        if (GetStageTargetPosition(target, pos, 0, 0) == 2) {
            ClearShootReq();
            HockeyGoalPosTrim(target);
            Reset();
            ActCallEscape(target);
            return 1;
        }
    } else {
        if (!m_pChr->IsHoldBall()) {
            ResetItemBallShootTiming();
            return 1;
        }
        if (IsThinkTimingForOnBaller()) {
            if (ThkInterruptOnBaller_Shoot()) {
                return 1;
            }
            ThkInterruptOnBaller_Pass();
            return 1;
        }
    }
    return 0;
}

// Nobody holds the puck.
int CComDrvHockeyGame::ThkInterruptLoseBall()
{
    if (GetRootActType() == 0x16) {
        return 0;
    }
    if (GetRootActType() != 0x24 && m_pChr->GetComLevelWork() != 1) {
        CChrCore *pEnemy = GetNearStarEnemy();
        if (pEnemy != 0) {
            Reset();
            ActCallGoEscape(pEnemy);
            return 1;
        }
    }
    if (ThkInterruptLoseBall_ShootSteal()) {
        return 1;
    }
    long nShooter;
    if (AreYouShot(0, nShooter)) {
        if (WhatIsTheStatePackRebound()) {
            if (GetThkStatus() != 6) {
                Reset();
                SetThkStatus(6);
            }
        } else if (GetRootActType() != 0xF) {
            Reset();
            ActCallGoShootArea(1);
        }
        return 1;
    }
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0x1A) {
        return 0;
    }
    if (GetRootActType() == 0xB) {
        return 0;
    }
    if (GetThkStatus() != 6) {
        Reset();
        SetThkStatus(6);
        return 1;
    }
    return 0;
}

// An enemy shot: dive for the puck, likelier the further the shooter is
// from it (up to the computer data's distance).
int CComDrvHockeyGame::ThkInterruptLoseBall_ShootSteal()
{
    long nShooter;
    if (!AreYouShot(1, nShooter)) {
        return 0;
    }
    if (m_nUnk3BC == 1) {
        return 0;
    }
    if (GetRootActType() == 0xC) {
        return 0;
    }
    if (!m_pChr->CanbeDivingSteal(0)) {
        return 0;
    }
    if (!isDivingStealDist()) {
        return 0;
    }
    if (m_pBall->GetItemBallType() == 3) {
        return 0;
    }

    float fLv = GetHockeyComLvF();
    m_nUnk3BC = 1;
    bcs::FVEC3 chrPos = CChrMan::GetChrCore(nShooter)->GetPos();
    bcs::FVEC3 ballPos = m_pBall->GetPos();
    float fDist = DisXZ(chrPos, ballPos);
    long nRate = CComMan::GetDatI(0x4F9, fLv);
    float fMax = CComMan::GetDatF(0x501, fLv);
    if (fMax < 1.0f) {
        fMax = 1.0f;
    }
    if (fDist > fMax) {
        fDist = fMax;
    }
    long nLot = (long)(nRate * fDist) / fMax;
    if (LotRate() < nLot) {
        Reset();
        ActCallDivingSteal();
        SetThkStatus(6);
        return 1;
    }
    return 0;
}

// The driver holds the puck.
int CComDrvHockeyGame::ThkInterruptOnBaller()
{
    if (!m_pChr->IsHoldBall()) {
        ResetItemBallShootTiming();
        return 0;
    }
    if (IsThinkTimingForOnBaller()) {
        if (ThkInterruptOnBaller_Shoot()) {
            return 1;
        }
        if (ThkInterruptOnBaller_Pass()) {
            return 1;
        }
        if (ThkInterruptOnBaller_SpShoot()) {
            return 1;
        }
    }
    if (isItemBallShootTiming() && ThkInterruptOnBaller_ItemBallShoot()) {
        return 1;
    }
    if (ThkInterruptOnBaller_EscapeFromStar()) {
        return 1;
    }
    ThkInterruptOnBaller_Blockade();
    return 1;
}

// Pass to the best placed computer team mate that can take it, else to a
// team mate charging a shot.
int CComDrvHockeyGame::ThkInterruptOnBaller_Pass()
{
    if (GetRootActType() == 0x2F) {
        return 0;
    }
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0xA && m_nUnk410 == 0) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (CComMan::GetDatI(0, 0L) && IsEntryAct(0x26)) {
        return 0;
    }
    if (CComMan::GetDatI(4, 0L) && IsEntryAct(0x11)) {
        return 0;
    }

    long nNum = GetMyTeamMemberNumWithoutMe();
    const CChrCore *apChr[4];
    GetConditionScore(apChr, nNum);
    for (long i = 0; i < nNum; i++) {
        if (apChr[i] != 0 && GetComDrv(apChr[i]) != 0 &&
            m_pChr->CanbePass(const_cast<CChrCore *>(apChr[i])) && CheckPassToChrCore(apChr[i])) {
            Reset();
            ActCallPass(apChr[i]);
            return 1;
        }
    }
    CChrCore *pCharge = getChargePlayerCore();
    if (pCharge != 0) {
        Reset();
        ActCallPass(pCharge);
        return 1;
    }
    return 0;
}

int CComDrvHockeyGame::ThkInterruptOnBaller_ItemBallShoot()
{
    if (isAgeShoot()) {
        return 0;
    }
    if (GetRootActType() == 0x2F) {
        return 0;
    }
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0xA) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (IsEntryAct(0x27)) {
        return 0;
    }
    if (isEnemyGoalBehind() == 1) {
        return 0;
    }
    if (CComMan::GetDatI(2, 0L) && IsEntryAct(0x26)) {
        return 0;
    }
    if (CComMan::GetDatI(6, 0L) && IsEntryAct(0x11)) {
        return 0;
    }
    if (!CheckItemBallShoot()) {
        return 0;
    }
    Reset();
    ActCallItemShoot();
    return 1;
}

int CComDrvHockeyGame::ThkInterruptOnBaller_SpShoot()
{
    if (isAgeShoot()) {
        return 0;
    }
    if (GetExistPadDrvNum() > 0) {
        return 0;
    }
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0xA) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (!m_pChr->CanbeAirTrick()) {
        return 0;
    }
    if (isEnemyGoalBehind() == 1) {
        return 0;
    }
    CChrCore *pKeeper = GetEmTeamKeeperCore();
    if (pKeeper != 0 && pKeeper->IsHockeyDamage()) {
        return 0;
    }
    if (CComMan::GetDatI(3, 0L) && IsEntryAct(0x26)) {
        return 0;
    }
    if (CComMan::GetDatI(7, 0L) && IsEntryAct(0x11)) {
        return 0;
    }
    if (!CheckSpShootProbability()) {
        return 0;
    }
    Reset();
    ActCallSpShoot();
    return 1;
}

int CComDrvHockeyGame::ThkInterruptOnBaller_EscapeFromStar()
{
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0xA) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (IsStar(m_pChr) == 1) {
        return 0;
    }
    CChrCore *pEnemy = GetNearStarEnemy();
    if (pEnemy != 0) {
        Reset();
        ActCallGoEscape(pEnemy);
        return 1;
    }
    return 0;
}

// Shoot when the shot scores better than holding on, with what the
// stage allows.
int CComDrvHockeyGame::ThkInterruptOnBaller_Shoot()
{
    if (isAgeShoot()) {
        return 0;
    }
    if (GetRootActType() == 0x2F) {
        return 0;
    }
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0xA) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (isEnemyGoalBehind() == 1) {
        return 0;
    }
    if (CComMan::GetDatI(1, 0L) && IsEntryAct(0x26)) {
        return 0;
    }
    if (CComMan::GetDatI(5, 0L) && IsEntryAct(0x11)) {
        return 0;
    }
    if (IsEntryAct(0x27)) {
        return 0;
    }
    if (!m_pChr->CanbeShoot()) {
        return 0;
    }
    long nScore = GetHockeyShootScore();
    if (nScore >= CalcActiveShootScore(m_pChr, 0.0f)) {
        return 0;
    }
    if (!ButToShootOutsideShots()) {
        return 0;
    }

    int nKind = SetShootKind();
    int bCheck = 1;
    long nPanel = CComManHockeyGame::isExistScorePanelNowPos(m_MyPos);
    if (nPanel == 1) {
        m_nShootKind = 0;
        nKind = 1;
        m_fUnk3F4 = 0.0f;
        m_nUnk3FC = 0;
    } else if (nPanel == 0) {
        ResetShootKind();
        return 0;
    }
    if (m_nShootKind == 2 && CComMan::GetDatI(0x181, (long)m_nStage) == 0) {
        bCheck = 0;
    }
    if (bCheck == 1 && m_pChr->GetComLevelWork() == 1 && CComMan::GetDatI(0x193, (long)m_nStage) == 0) {
        bCheck = 0;
    }
    if (bCheck != 0) {
        if (ButToShootGimmickShots()) {
            Reset();
            ActCallShoot(nKind);
            return 1;
        }
        ResetShootKind();
        return 0;
    }
    Reset();
    ActCallShoot(nKind);
    return 1;
}

int CComDrvHockeyGame::ThkInterruptOnBaller_Blockade()
{
    if (GetRootActType() == 0xD) {
        return 0;
    }
    if (GetRootActType() == 0x17) {
        return 0;
    }
    if (GetRootActType() == 0xA) {
        return 0;
    }
    if (GetRootActType() == 0x19) {
        return 0;
    }
    if (GetRootActType() == 0x22) {
        return 0;
    }
    if (What1On1State()) {
        ClearShootReq();
        Reset();
        ActCallBlockade();
        return 1;
    }
    return 0;
}

// A team mate charging a shot: skate to the shooting area.
inline int CComDrvHockeyGame::ThkInterruptGoShootAreaForCharge()
{
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (GetRootActType() == 0xF) {
        return 0;
    }
    if (!isFriendOnBallerNowCharge()) {
        return 0;
    }
    if (GetRootActType() == 0x2A) {
        return 0;
    }
    Reset();
    ActCallGoShootArea(2);
    return 1;
}

// The own keeper holds the puck: go to take its pass.
inline int CComDrvHockeyGame::ThkInterruptGoKeeperPassCatch()
{
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (GetRootActType() == 0x2A) {
        return 1;
    }
    CChrCore *pKeeper = GetMyTeamKeeperCore();
    if (pKeeper == 0) {
        return 0;
    }
    if (pKeeper->IsHockeyInvisible() == 1) {
        return 0;
    }
    if (!pKeeper->IsHoldBall()) {
        return 0;
    }
    Reset();
    ActCallGoKeeperPassCatch();
    return 1;
}

// A team mate holds the puck.
int CComDrvHockeyGame::ThkInterruptMyTeamHoldBall()
{
    if (!IsMyTeamHoldBall(-1)) {
        return 0;
    }
    if (GetRootActType() == 0xB) {
        return 1;
    }
    if (ThkInterruptEscapeFromStar()) {
        return 1;
    }
    if (ThkInterruptGoShootAreaForCharge()) {
        return 1;
    }
    if (ThkInterruptGoKeeperPassCatch()) {
        return 1;
    }
    if (ThkInterruptMyTeamHoldBall_DirectShootCharge()) {
        return 1;
    }
    if (ThkInterruptMyTeamHoldBall_RunIntoBefore()) {
        return 1;
    }
    ThkInterruptMyTeamHoldBall_NoMarkJudge();
    return 1;
}

// Before the blue line, with no enemy near the way to it or to the goal:
// skate to the blue line.
int CComDrvHockeyGame::ThkInterruptMyTeamHoldBall_NoMarkJudge()
{
    if (IsEntryAct(0x28)) {
        return 0;
    }
    if (IsEntryAct(0x24)) {
        return 0;
    }
    if (IsEntryAct(0x2B)) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    m_fUnk3A8 += m_pChr->GetFrameStep();
    if (m_fUnk3A8 < CComMan::GetDatF(0x3D7, fLv)) {
        return 0;
    }
    m_fUnk3A8 = 0.0f;

    bcs::FVEC3 pos;
    bcs::FVEC3 goal;
    goal = m_TargetGoalPos;
    pos = m_MyPos;
    if (goal.z >= 0.0f) {
        if (pos.z >= BlueLineDef) {
            return 0;
        }
    } else if (pos.z <= -BlueLineDef) {
        return 0;
    }

    float fWidth = CComMan::GetDatF(0x3E7, fLv);
    bcs::FVEC3 line(pos.x, 0.0f, goal.z);
    float fLen = DisXZ(pos, line);
    for (long i = 0; i < GetEnemyNum(); i++) {
        const CChrCore *pEnemy = GetEnemyCore(i);
        if (CComMan::IsPointInFaceXZ(pEnemy->GetPos(), pos, line, fWidth, fLen)) {
            return 0;
        }
    }
    fWidth = CComMan::GetDatF(0x3EF, fLv);
    fLen = DisXZ(pos, goal);
    for (long i = 0; i < GetEnemyNum(); i++) {
        const CChrCore *pEnemy = GetEnemyCore(i);
        if (CComMan::IsPointInFaceXZ(pEnemy->GetPos(), pos, goal, fWidth, fLen)) {
            return 0;
        }
    }
    if (LotRate() >= CComMan::GetDatI(0x3DF, fLv)) {
        return 0;
    }
    Reset();
    ActCallGoBlueLine();
    return 1;
}

// Near the puck holder: go on to attack past the blue line, or to it.
int CComDrvHockeyGame::ThkInterruptMyTeamHoldBall_RunIntoBefore()
{
    if (GetRootActType() == 0xE) {
        return 0;
    }
    if (GetRootActType() == 0x28) {
        return 0;
    }
    if (IsEntryAct(0x24)) {
        return 0;
    }
    if (IsEntryAct(0x2B)) {
        return 0;
    }
    const CChrCore *pHold = GetHoldBallCore();
    if (pHold != 0) {
        float fDis = CComMan::GetDatF(0x3C7, GetHockeyComLvF());
        float fLine = CComMan::GetDatF(0x3CF, GetHockeyComLvF());
        bcs::FVEC3 pos = m_MyPos;
        if (DisXZ(m_MyPos, pHold->GetPos()) <= fDis) {
            bcs::FVEC3 goal = m_TargetGoalPos;
            if (goal.z >= 0.0f) {
                if (pos.z >= fLine) {
                    Reset();
                    ActCallGoOffense(0);
                    return 1;
                }
                Reset();
                ActCallGoBlueLine();
                return 1;
            }
            if (pos.z <= -fLine) {
                Reset();
                ActCallGoOffense(0);
                return 1;
            }
            Reset();
            ActCallGoBlueLine();
            return 1;
        }
    }
    return 0;
}

// At the right distance from the goal with a free line to it: charge a
// direct shot.
int CComDrvHockeyGame::ThkInterruptMyTeamHoldBall_DirectShootCharge()
{
    if (GetRootActType() == 0x2B) {
        return 1;
    }
    if (GetRootActType() == 0xF) {
        return 0;
    }
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (GetRootActType() == 0x2A) {
        return 0;
    }
    m_fUnk3A4 += m_pChr->GetFrameStep();
    if (m_fUnk3A4 < CComMan::GetDatF(0x397, GetHockeyComLvF())) {
        return 0;
    }
    m_fUnk3A4 = 0.0f;

    float fLv = GetHockeyComLvF();
    float fDist = GetTargetGoalDistXZ(m_pChr, 0.0f);
    float fMin = CComMan::GetDatF(0x39F, fLv);
    float fMax = CComMan::GetDatF(0x3A7, fLv);
    if (fDist < fMin || fDist > fMax) {
        return 0;
    }
    if (!isExistFromTargetGoalAngle(CComMan::GetDatF(0x3AF, fLv))) {
        return 0;
    }
    if (isExistEnemyBetweenTargetGoal(CComMan::GetDatF(0x3B7, fLv)) == 1) {
        return 0;
    }
    if (LotRate() >= CComMan::GetDatI(0x3BF, fLv)) {
        return 0;
    }
    Reset();
    ActCallDirectShootCharge();
    return 1;
}

// The other team holds the puck (or passes it).
int CComDrvHockeyGame::ThkInterruptEnemyTeamHoldBall()
{
    int bPass = 0;
    if (!IsEmTeamHoldBall(-1)) {
        if (!isEnemyPass()) {
            return 0;
        }
        bPass = 1;
    }
    if (GetRootActType() == 0x1A) {
        return 0;
    }
    if (ThkInterruptEnemyTeamHoldBall_PassSteal()) {
        return 1;
    }
    if (ThkInterruptEscapeFromStar()) {
        return 1;
    }
    if (bPass == 0 && ThkInterruptEnemyTeamHoldBall_AttackChance()) {
        return 1;
    }
    ThkInterruptEnemyTeamHoldBall_GoalProtect();
    return 1;
}

// An enemy pass: dive for the puck, likelier the further the receiver is
// from it (other rates for a keeper).
int CComDrvHockeyGame::ThkInterruptEnemyTeamHoldBall_PassSteal()
{
    if (!isEnemyPass()) {
        return 0;
    }
    if (m_nUnk3C0 == 1) {
        return 0;
    }
    if (GetRootActType() == 0xC) {
        return 0;
    }
    if (!m_pChr->CanbeDivingSteal(0)) {
        return 0;
    }
    if (!isDivingStealDist()) {
        return 0;
    }

    float fLv = GetHockeyComLvF();
    m_nUnk3C0 = 1;
    bcs::FVEC3 chrPos = CChrMan::GetChrCore(m_nUnk464)->GetPos();
    bcs::FVEC3 ballPos = m_pBall->GetPos();
    float fDist = DisXZ(chrPos, ballPos);
    if (CChrMan::GetChrCore(m_nUnk464)->IsKeeper()) {
        long nRate = CComMan::GetDatI(0x519, fLv);
        float fMax = CComMan::GetDatF(0x521, fLv);
        if (fMax < 1.0f) {
            fMax = 1.0f;
        }
        if (fDist > fMax) {
            fDist = fMax;
        }
        long nLot = (long)(nRate * fDist) / fMax;
        if (LotRate() < nLot) {
            Reset();
            ActCallDivingSteal();
            SetThkStatus(6);
            return 1;
        }
    } else {
        long nRate = CComMan::GetDatI(0x509, fLv);
        float fMax = CComMan::GetDatF(0x511, fLv);
        if (fMax < 1.0f) {
            fMax = 1.0f;
        }
        if (fDist > fMax) {
            fDist = fMax;
        }
        long nLot = (long)(nRate * fDist) / fMax;
        if (LotRate() < nLot) {
            Reset();
            ActCallDivingSteal();
            SetThkStatus(6);
            return 1;
        }
    }
    return 0;
}

// Now and then a chance comes to attack the puck holder.
int CComDrvHockeyGame::ThkInterruptEnemyTeamHoldBall_AttackChance()
{
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (GetRootActType() == 0x16) {
        return 0;
    }
    if (m_pChr->GetStatusFlag() & 0x10000) {
        return 0;
    }
    if (m_nUnk3B0 != 0) {
        if (isAttack(0)) {
            m_nUnk3B0 = 0;
            m_fUnk3B4 = 0.0f;
            Reset();
            ActCallAttackChance();
            return 1;
        }
        m_nUnk3B0 = 0;
        m_fUnk3B4 = 0.0f;
    } else {
        m_fUnk3B4 += m_pChr->GetFrameStep();
        if (m_fUnk3B4 >= CComMan::GetDatF(0x4E9, GetHockeyComLvF())) {
            m_nUnk3B0 = 1;
            m_fUnk3B4 = 0.0f;
        }
    }
    return 0;
}

// When the own keeper cannot guard the goal (gone, down or flagged), guard
// it in its place.
int CComDrvHockeyGame::ThkInterruptEnemyTeamHoldBall_GoalProtect()
{
    if (GetRootActType() == 0x24) {
        return 0;
    }
    if (GetRootActType() == 0x16) {
        return 0;
    }
    if (GetRootActType() == 0x29) {
        return 0;
    }
    CChrCore *pKeeper = GetMyTeamKeeperCore();
    if (pKeeper != 0) {
        if (m_nUnk378 != 0) {
            if (pKeeper->IsHockeyInvisible() == 1 || pKeeper->IsHockeyDamage() == 1 ||
                IsStatusBit16(pKeeper) == 1) {
                m_nUnk378 = 0;
                if (LotRate() < CComMan::GetDatI(0x529, GetHockeyComLvF())) {
                    Reset();
                    ActCallGoalProtect();
                    return 1;
                }
            }
        } else if (pKeeper->IsHockeyInvisible() == 0 && pKeeper->IsHockeyDamage() == 0 &&
                   !(pKeeper->GetStatusFlag() & 0x10000)) {
            m_nUnk378 = 1;
        }
    }
    return 0;
}

// Before the face-off is over: skate about or wait.
inline int CComDrvHockeyGame::ThkFaceoff()
{
    if (isFaceoffEnd()) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x579, fLv);
    aRate[1] = CComMan::GetDatI(0x581, fLv);
    if (LotProbability(aRate, 2) == 0) {
        ActCallGoRunRand();
    } else {
        float fMin = CComMan::GetDatF(0x589, fLv);
        ActCallWaitBrake(RandRange(fMin, CComMan::GetDatF(0x591, fLv)));
    }
    return 1;
}

inline int CComDrvHockeyGame::ThkBallKeep()
{
    int bDone = 0;
    if (m_pChr->IsHoldBall()) {
        ActCallBallKeep();
        bDone = 1;
    }
    return bDone;
}

inline int CComDrvHockeyGame::ThkOffense()
{
    if (!IsMyTeamHoldBall(-1)) {
        long nShooter;
        if (!AreYouShot(0, nShooter)) {
            return 0;
        }
        if (WhatIsTheStatePackRebound()) {
            return 0;
        }
    }
    ActCallOffense();
    return 1;
}

// Nobody holds the puck: go for it, or skate about or brake.
inline void CComDrvHockeyGame::ThkLooseBall()
{
    if (LotRate() < GoGetPackToGetEstablished()) {
        ActCallGetBall(1);
        return;
    }
    float fLv = GetHockeyComLvF();
    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x569, fLv);
    aRate[1] = CComMan::GetDatI(0x571, fLv);
    if (LotProbability(aRate, 2) == 0) {
        ActCallGoRunRand();
    } else {
        ActCallBrake();
    }
}

void CComDrvHockeyGame::ThkMain()
{
    m_nUnk35C = 0;
    m_fUnk360 = 0.0f;
    if (m_nUnk36C != 0) {
        return;
    }
    if (ThkFaceoff()) {
        SetThkStatus(1);
        return;
    }
    if (ThkBallKeep()) {
        SetThkStatus(3);
        return;
    }
    if (ThkOffense()) {
        SetThkStatus(4);
        return;
    }
    if (ThkDefense()) {
        SetThkStatus(5);
        return;
    }
    if (CComMan::GetHoldBallChrCore(-1) == 0) {
        ThkLooseBall();
    }
    SetThkStatus(6);
}

// Guard the enemy the driver is best placed for, or the pass course to
// it, when the computer data's rates say so.
inline void CComDrvHockeyGame::ActCallGoDefenseFor(long nNo)
{
    float fLv = GetHockeyComLvF();
    const CChrCore *pHold = GetHoldBallCore();
    bcs::FVEC3 pos = m_Pos148;
    if (pHold != 0 && DisXZ(pos, pHold->GetPos()) >= CComMan::GetDatF(0x4C9, fLv) &&
        LotRate() < CComMan::GetDatI(0x4D1, fLv)) {
        ActCallGoDefPassCourse(nNo);
        return;
    }
    ActCallGoDefense(nNo, 0);
}

// The other team has the puck: pick an enemy to mark, or an item to get.
int CComDrvHockeyGame::ThkDefense()
{
    int bPass = 0;
    if (!IsEmTeamHoldBall(-1)) {
        if (!isEnemyPass()) {
            return 0;
        }
        bPass = 1;
    }
    if (m_pChr->IsHockeyDamage()) {
        return 0;
    }

    SORT aSort[32];
    bcs::FVEC3 aMyPos[3];
    bcs::FVEC3 aEnemyPos[3];
    const CChrCore *apEnemy[3];
    float fLv = GetHockeyComLvF();
    bcs::FVEC3 guard = m_GuardGoalPos;
    long nTeam = GetMyTeamNo();
    long nMyNum = 0;
    long nEmNum = 0;
    long nMyIdx = 0;
    long nAdd = 0;
    float fLine;
    if (guard.z > 0.0f) {
        fLine = guard.z - CComMan::GetDatF(0x487, fLv);
    } else {
        fLine = guard.z + CComMan::GetDatF(0x487, fLv);
    }

    bcs::FVEC3 *pMyPos = aMyPos;
    SORT *pSort = aSort;
    const CChrCore **ppEnemy = apEnemy;
    bcs::FVEC3 *pEnemyPos = aEnemyPos;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable() || pChr->IsKeeper()) {
            continue;
        }
        if (nTeam == pChr->GetTeamNo()) {
            if (m_pChr == pChr) {
                nMyIdx = nMyNum;
            }
            *pMyPos = CComMan::GetChrWorldPos(pChr);
            float fDist = GetWorldDisXZ(CComMan::GetChrWorldPos(pChr), guard);
            pMyPos++;
            nMyNum++;
            pSort->m_nId = pChr->GetPlayerNo();
            pSort->m_fParam = fDist;
            pSort++;
        } else {
            bcs::FVEC3 pos = pChr->GetPos();
            if (guard.z > 0.0f) {
                if (pos.z >= fLine) {
                    nAdd += CComMan::GetDatI(0x48F, fLv);
                }
            } else if (pos.z <= fLine) {
                nAdd += CComMan::GetDatI(0x48F, fLv);
            }
            *ppEnemy = pChr;
            *pEnemyPos = CComMan::GetChrWorldPos(pChr);
            ppEnemy++;
            nEmNum++;
            pEnemyPos++;
        }
    }

    long nBest = CComMan::GetNearBestPosBaseEx(aMyPos, aEnemyPos, nMyNum, nEmNum, nMyIdx);
    if (nBest < 0) {
        return 0;
    }
    if (nMyNum != 1) {
        bcs::BCS_ShakerSort(aSort, nMyNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
    }

    if (bPass == 0) {
        long nOrder;
        if (apEnemy[nBest]->IsHoldBall()) {
            nOrder = 0;
        } else {
            // 1 when the driver is its team's nearest to its own goal.
            const CChrCore *pMe = m_pChr;
            nOrder = pMe == CChrMan::GetChrCore(aSort[0].m_nId) ? 1 : 2;
        }
        if (ThkDefense_AttackBossPackun(nOrder, nAdd)) {
            return 1;
        }
        if (ThkDefense_GetStopPanel(nOrder, nAdd)) {
            return 1;
        }
        if (ThkDefense_GetHatenaPanel(nOrder, nAdd)) {
            return 1;
        }
        if (ThkDefense_GetCoin(nOrder, nAdd)) {
            return 1;
        }
        if (ThkDefense_GetGreenShell(nOrder, nAdd)) {
            return 1;
        }
    }

    if (apEnemy[nBest]->IsHoldBall() || (bPass == 1 && nBest == m_nUnk464)) {
        if (LotRate() < CComMan::GetDatI(0x4A9, fLv)) {
            ActCallGoDefense(apEnemy[nBest]->GetPlayerNo(), 1);
            return 1;
        }
    } else if (LotRate() < CComMan::GetDatI(0x4B1, fLv)) {
        ActCallGoDefenseFor(apEnemy[nBest]->GetPlayerNo());
        return 1;
    }

    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x4B9, fLv);
    aRate[1] = CComMan::GetDatI(0x4C1, fLv);
    if (LotProbability(aRate, 2) == 0) {
        ActCallGoRunRand();
    } else {
        ActCallBrake();
    }
    return 1;
}

int CComDrvHockeyGame::ThkDefense_GetHatenaPanel(long nOrder, long nAdd)
{
    if (isTeamCoinMAX() == 1 && IsHaveMyStockItem() == 1) {
        return 0;
    }
    float fDist;
    bcs::FVEC3 pos;
    if (!isGetQPanel(0, fDist, pos)) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    switch (nOrder) {
        case 0: {
            long nRate = nAdd + CComMan::GetDatI(0x3F7, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetHatenaPanel();
                return 1;
            }
            break;
        }
        case 1: {
            long nRate = nAdd + CComMan::GetDatI(0x427, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetHatenaPanel();
                return 1;
            }
            break;
        }
        default: {
            long nRate = nAdd + CComMan::GetDatI(0x457, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetHatenaPanel();
                return 1;
            }
            break;
        }
    }
    return 0;
}

int CComDrvHockeyGame::ThkDefense_GetCoin(long nOrder, long nAdd)
{
    if (isTeamCoinMAX()) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    long nStageRate = CComMan::GetDatI(0x497, (long)m_nStage);
    float fDist = CComMan::DropCoin_GetNearDisXZ(m_MyPos, -1);
    if (fDist < 0.0f || fDist > (float)CComMan::GetDatI(0x1072, GetChrTypeID())) {
        return 0;
    }
    switch (nOrder) {
        case 0: {
            long nRate = nAdd + CComMan::GetDatI(0x3FF, fLv) * nStageRate / 100;
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetCoin();
                return 1;
            }
            break;
        }
        case 1: {
            long nRate = nAdd + CComMan::GetDatI(0x42F, fLv) * nStageRate / 100;
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetCoin();
                return 1;
            }
            break;
        }
        case 2: {
            long nRate = nAdd + CComMan::GetDatI(0x45F, fLv) * nStageRate / 100;
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetCoin();
                return 1;
            }
            break;
        }
    }
    return 0;
}

int CComDrvHockeyGame::ThkDefense_GetGreenShell(long nOrder, long nAdd)
{
    bcs::FVEC3 pos;
    if (!isGetGreenShell(pos)) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    switch (nOrder) {
        case 0: {
            long nRate = nAdd + CComMan::GetDatI(0x407, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetGreenShell();
                return 1;
            }
            break;
        }
        case 1: {
            long nRate = nAdd + CComMan::GetDatI(0x437, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetGreenShell();
                return 1;
            }
            break;
        }
        default: {
            long nRate = nAdd + CComMan::GetDatI(0x467, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetGreenShell();
                return 1;
            }
            break;
        }
    }
    return 0;
}

int CComDrvHockeyGame::ThkDefense_GetStopPanel(long nOrder, long nAdd)
{
    float fDist;
    bcs::FVEC3 pos;
    if (!isGetQPanel(1, fDist, pos)) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    switch (nOrder) {
        case 0: {
            long nRate = nAdd + CComMan::GetDatI(0x40F, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetStopPanel();
                return 1;
            }
            break;
        }
        case 1: {
            long nRate = nAdd + CComMan::GetDatI(0x43F, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetStopPanel();
                return 1;
            }
            break;
        }
        default: {
            long nRate = nAdd + CComMan::GetDatI(0x46F, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallGetStopPanel();
                return 1;
            }
            break;
        }
    }
    return 0;
}

int CComDrvHockeyGame::ThkDefense_AttackBossPackun(long nOrder, long nAdd)
{
    bcs::FVEC3 pos;
    if (!isExistBossPackun(pos)) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    switch (nOrder) {
        case 0: {
            long nRate = nAdd + CComMan::GetDatI(0x417, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallAttackBossPackun();
                return 1;
            }
            break;
        }
        case 1: {
            long nRate = nAdd + CComMan::GetDatI(0x447, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallAttackBossPackun();
                return 1;
            }
            break;
        }
        default: {
            long nRate = nAdd + CComMan::GetDatI(0x477, fLv);
            if (nRate < 0) {
                nRate = 0;
            }
            if (LotRate() < nRate) {
                ActCallAttackBossPackun();
                return 1;
            }
            break;
        }
    }
    return 0;
}

// The face-off starts: the team's face-off player (nNo0 for team 0, nNo1
// for team 1) takes it, the others skate about or wait.
void CComDrvHockeyGame::ThkFaceoffStart(long nNo0, long nNo1)
{
    if (GetMyTeamNo() == 0) {
        if (nNo0 == m_pChr->GetPlayerNo()) {
            Reset();
            ActCallFaceoff();
            return;
        }
    } else if (nNo1 == m_pChr->GetPlayerNo()) {
        Reset();
        ActCallFaceoff();
        return;
    }

    float fLv = GetHockeyComLvF();
    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x579, fLv);
    aRate[1] = CComMan::GetDatI(0x581, fLv);
    if (LotProbability(aRate, 2) == 0) {
        Reset();
        ActCallGoRunRand();
        return;
    }
    float fMin = CComMan::GetDatF(0x589, fLv);
    float fMax = CComMan::GetDatF(0x591, fLv);
    Reset();
    ActCallWaitBrake(RandRange(fMin, fMax));
}

}
