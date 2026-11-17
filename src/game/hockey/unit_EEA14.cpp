// msw::CComDrvHockeyGame: the questions the hockey computer asks about the
// game (passes, attacks, defence switches, special shots and the gimmicks
// of the stages). A file of its own after game/hockey/unit_EDBF0.cpp; the
// file name is not known.
//
// Every GetPos called on a character from GetFriendCore/GetEnemyCore here is
// the const one (GetPos__Q23msw8CChrCoreCFv): those getters probably return
// const characters in the original, but the header (shared with
// game/hockey/unit_AE7E8.cpp, which defines them) has them non-const, hence
// the static_casts. The `pMe = m_pChr` locals load m_pChr before the
// GetFriendCore call they are compared with, as the original does.
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManHockey.hpp"
#include "game/msw/CComManHockeyGame.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/CSimpleSortBase.hpp"

namespace {
    typedef bcs::CSimpleSortBase<32>::SORT SORT;

    // The distance between two points in the XZ plane (name guessed).
    inline float DisXZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1)
    {
        bcs::FVEC3 pos0(rPos0.x, 0.0f, rPos0.z);
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        return bcs::FVEC3Dist(&pos0, &pos1);
    }
}

namespace msw {

// Whether an enemy is the receiver of the pass in flight, the puck fast
// enough to be one.
int CComDrvHockeyGame::isEnemyPass()
{
    CBall *pBall = m_pBall;
    if (pBall == 0) {
        return 0;
    }
    if (m_nUnk464 < 0) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (pBall->GetState() != 3) {
        return 0;
    }
    float fPassSpeed = pBall->GetHockeyPassSpeed();
    bcs::FVEC3 vel = m_pBall->GetVelocity();
    float fSpeed = bcs::BCS_Vec_GetLenXZ(vel);
    if (fPassSpeed == 0.0f) {
        return 0;
    }
    if (fSpeed / fPassSpeed <= 0.93f) {
        return 0;
    }
    long nNum = GetEnemyNum();
    for (long i = 0; i < nNum; i++) {
        if (m_nUnk464 == GetEnemyCore(i)->GetPlayerNo()) {
            return 1;
        }
    }
    return 0;
}

int CComDrvHockeyGame::IsMyTeamHoldBallWithoutKeeper()
{
    if (IsMyTeamHoldBall(-1) && !GetHoldBallCore()->IsKeeper()) {
        return 1;
    }
    return 0;
}

// Whether to start a combination play: every so often, when no team mate
// is down, an enemy is near and no map cell between each team mate and the
// goal line is blocked.
int CComDrvHockeyGame::isComCombination()
{
    if (m_pChr->IsHockeyNowCombinationMode()) {
        return 0;
    }
    m_fUnk374 += m_pChr->GetFrameStep();
    if (m_fUnk374 < CComMan::GetDatF(0x1C5, GetHockeyComLvF())) {
        return 0;
    }
    m_fUnk374 = 0.0f;
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
    if (GetRootActType() == 0x25) {
        return 0;
    }
    if (GetExistPadDrvNum() > 0) {
        return 0;
    }
    if (!m_pChr->IsHoldBall()) {
        return 0;
    }
    if (GetMyTeamMemberNumWithoutMe() == 0) {
        return 0;
    }
    for (long i = 0; i < GetFriendNum(); i++) {
        const CChrCore *pMe = m_pChr;
        if (GetFriendCore(i) != pMe) {
            if (GetFriendCore(i)->IsHockeyDamage()) {
                return 0;
            }
            if (GetFriendCore(i)->GetStatusFlag() & 0x10000) {
                return 0;
            }
        }
    }
    if (SearchingForTheEenemy(m_MyPos, CComMan::GetDatF(0x1CD, GetHockeyComLvF())) < 0) {
        return 0;
    }

    bcs::FVEC3 goal = m_TargetGoalPos;
    for (long i = 0; i < GetFriendNum(); i++) {
        const CChrCore *pChr = GetFriendCore(i);
        if (pChr == m_pChr) {
            continue;
        }
        bcs::FVEC3 pos = pChr->GetPos();
        bcs::FVEC3 line;
        if (goal.z > 0.0f) {
            line.Set(pos.x, 0.0f, 8.0f);
        } else {
            line.Set(pos.x, 0.0f, -8.0f);
        }
        long nX0;
        long nZ0;
        long nX1;
        long nZ1;
        CComManHockey::GetGridIndex(nX0, nZ0, pos, 0xF, 0x16);
        CComManHockey::GetGridIndex(nX1, nZ1, line, 0xF, 0x16);
        CComManHockey::MAP_STATUS_DATA data;
        long nZ = nZ0;
        long nDiff = nZ1 - nZ0;
        if (nDiff > 0) {
            for (; nZ <= nZ1; nZ++) {
                if (GetMapStatus(nX0, nZ, data) && (data.m_nAttr & 0xF) == 1) {
                    return 0;
                }
            }
        } else if (nDiff < 0) {
            for (; nZ >= nZ1; nZ--) {
                if (GetMapStatus(nX0, nZ, data) && (data.m_nAttr & 0xF) == 1) {
                    return 0;
                }
            }
        }
    }

    long nRate = CComMan::GetDatI(0x1D5, GetHockeyComLvF());
    nRate = nRate * CComMan::GetDatI(0x1DD, (long)m_nStage) / 100;
    return LotRate() < nRate;
}

// Whether the way to pos is free of characters (nKind 0: all but the
// keepers, otherwise the other team's).
int CComDrvHockeyGame::CanMoveToTarget_BlueLine(bcs::FVEC3 pos, long nKind)
{
    float fLv = GetHockeyComLvF();
    float fWidth = CComMan::GetDatF(0x1637, fLv);
    float fAngle = CComMan::GetDatF(0x163F, fLv);
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &pos, &m_MyPos);
    bcs::FVEC3 dir = diff;
    float fLen = bcs::BCS_Vec_GetLenXZ(dir);
    unsigned long nBits;
    if (nKind == 0) {
        nBits = GetEnableBitWithoutKeeper();
    } else {
        nBits = GetTeamEnableBitWithoutKeeper(1);
    }
    return CComManHockeyGame::IsExistChrToTargetHockey(m_pChr, bcs::DEG_TO_RAD(fAngle), pos, fWidth, fLen, nBits);
}

long CComDrvHockeyGame::GetFaceOffPower()
{
    long nMin = CComMan::GetDatI(0x1627, GetHockeyComLvF());
    return CComManHockey::GetRandValueI(nMin, CComMan::GetDatI(0x162F, GetHockeyComLvF()));
}

// Moves rPos to where the way there first touches a keeper.
void CComDrvHockeyGame::HockeyKeeperPosTrim(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 from = m_MyPos;
    bcs::FVEC3 to = rPos;
    bcs::FVEC3 hit;

    const CChrCore *pKeeper = GetMyTeamKeeperCore();
    if (pKeeper != 0 && !pKeeper->IsHockeyInvisible()) {
        float fKeeperRadius = 0.1f + pKeeper->GetHockeyBodyCollRadiusNow();
        float fMyRadius = 0.1f + m_pChr->GetHockeyBodyCollRadiusNow();
        float fTime = CCollMan::GetIntersectionTimeOfSegmentSphere(hit, from, to, pKeeper->GetPos(),
                                                                    fKeeperRadius + fMyRadius);
        if (fTime >= 0.0f && fTime <= 1.0f) {
            rPos = hit;
            return;
        }
    }
    pKeeper = GetEmTeamKeeperCore();
    if (pKeeper != 0 && !pKeeper->IsHockeyInvisible()) {
        float fKeeperRadius = 0.1f + pKeeper->GetHockeyBodyCollRadiusNow();
        float fMyRadius = 0.1f + m_pChr->GetHockeyBodyCollRadiusNow();
        float fTime = CCollMan::GetIntersectionTimeOfSegmentSphere(hit, from, to, pKeeper->GetPos(),
                                                                    fKeeperRadius + fMyRadius);
        if (fTime >= 0.0f && fTime <= 1.0f) {
            rPos = hit;
        }
    }
}

// The player number of the nearest enemy within fDis of pos; -1 for none.
long CComDrvHockeyGame::SearchingForTheEenemy(bcs::FVEC3 pos, float fDis)
{
    SORT aSort[32];
    bcs::FVEC3 my = pos;
    SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < GetEnemyNum(); i++) {
        float fDist = DisXZ(my, static_cast<const CChrCore *>(GetEnemyCore(i))->GetPos());
        if (fDist <= fDis) {
            pSort->m_nId = GetEnemyCore(i)->GetPlayerNo();
            pSort->m_fParam = fDist;
            pSort++;
            nNum++;
        }
    }
    if (nNum <= 0) {
        return -1;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
    }
    return aSort[0].m_nId;
}

// Whether to go for the "?" panel of kind nKind (0 to 2): rDist and rPos
// get its distance and position.
int CComDrvHockeyGame::isGetQPanel(long nKind, float &rDist, bcs::FVEC3 &rPos)
{
    int bRet = CComManHockeyGame::GetExisQPanelDist(nKind, m_MyPos, rDist, rPos);
    if (bRet == 0) {
        return 0;
    }
    float fMax;
    switch (nKind) {
        case 0:
            fMax = CComMan::GetDatF(0x1087, GetChrTypeID());
            break;
        case 1:
            if (CGame::g_GameWork.GetStage() == 3) {
                fMax = CComMan::GetDatF(0x10C1, GetChrTypeID());
                break;
            }
            return 0;
        case 2:
            if (CGame::g_GameWork.GetStage() == 3) {
                fMax = CComMan::GetDatF(0x10DE, GetChrTypeID());
                break;
            }
            return 0;
        default:
            return 0;
    }
    if (rDist > fMax) {
        return 0;
    }
    long nLv = GetHockeyComLvI();
    if (nKind == 0) {
        bcs::FVEC3 goal = m_TargetGoalPos;
        bcs::FVEC3 my = m_MyPos;
        switch (CComMan::GetDatI(0x109C, nLv)) {
            case 1:
                if (IsMyTeamHoldBall(-1)) {
                    if (goal.z > 0.0f) {
                        if (my.z < 0.0f && rPos.z < 0.0f) {
                            return bRet;
                        }
                        if (rPos.z < 0.0f) {
                            return 0;
                        }
                        return bRet;
                    }
                    if (my.z > 0.0f && rPos.z > 0.0f) {
                        return bRet;
                    }
                    if (rPos.z > 0.0f) {
                        return 0;
                    }
                    return bRet;
                }
                break;
            case 2:
                if (IsMyTeamHoldBall(-1)) {
                    if (goal.z > 0.0f) {
                        if (rPos.z < 0.0f) {
                            return 0;
                        }
                        return bRet;
                    }
                    if (rPos.z > 0.0f) {
                        return 0;
                    }
                    return bRet;
                }
                break;
        }
    }
    if (nKind == 1) {
        if (!isExistPillar()) {
            return 0;
        }
        if (CComMan::GetDatI(0x10D6, nLv) == 1 && IsEmTeamHoldBall(-1)) {
            return 0;
        }
    }
    return bRet;
}

// Whether a team mate holding the puck charges a shot.
int CComDrvHockeyGame::isFriendOnBallerNowCharge()
{
    for (long i = 0; i < GetFriendNum(); i++) {
        CChrCore *pChr = GetFriendCore(i);
        if (pChr != m_pChr && pChr->IsHoldBall() && pChr->IsHockeyChgArrowMarker()) {
            return 1;
        }
    }
    return 0;
}

// Whether the puck is near enough to steal it with a dive.
int CComDrvHockeyGame::isDivingStealDist()
{
    float fDist;
    if (CComManHockey::GetDistanceFromBalls(m_pChr, 0.0f, fDist) == 0) {
        return 0;
    }
    return !(fDist > CComMan::GetDatF(0x4F1, GetHockeyComLvF()));
}

// Whether a team mate is within fDis of the goal the team attacks, not
// behind it.
int CComDrvHockeyGame::isFriendNearTargetGoal(float fDis)
{
    bcs::FVEC3 goal = m_TargetGoalPos;
    for (long i = 0; i < GetFriendNum(); i++) {
        const CChrCore *pMe = m_pChr;
        if (GetFriendCore(i) == pMe) {
            continue;
        }
        bcs::FVEC3 pos = static_cast<const CChrCore *>(GetFriendCore(i))->GetPos();
        if (DisXZ(pos, goal) > fDis) {
            continue;
        }
        if (goal.z > 0.0f && pos.z > goal.z) {
            continue;
        }
        if (goal.z < 0.0f && pos.z < goal.z) {
            continue;
        }
        return 1;
    }
    return 0;
}

// Whether a team mate is in a fight.
int CComDrvHockeyGame::isNowFighting()
{
    for (long i = 0; i < GetFriendNum(); i++) {
        const CChrCore *pMe = m_pChr;
        if (GetFriendCore(i) != pMe && GetFriendCore(i)->IsHockeyFighting()) {
            return 1;
        }
    }
    return 0;
}

// The team mate charging a shot with the best shot score, if a pass to
// him is possible and the lot says so.
CChrCore *CComDrvHockeyGame::getChargePlayerCore()
{
    CChrCore *pBest = 0;
    long nBest = -1;
    for (long i = 0; i < GetFriendNum(); i++) {
        CChrCore *pChr = GetFriendCore(i);
        if (pChr != m_pChr && GetComDrv(pChr) == 0 && (pChr->GetStatusFlag() & 0x4000)) {
            long nScore = CalcActiveShootScore(pChr, 0.0f);
            if (nBest < nScore) {
                pBest = pChr;
                nBest = nScore;
            }
        }
    }
    if (pBest == 0) {
        return 0;
    }
    if (CheckPassToChrCore(pBest)) {
        if (LotRate() >= CComMan::GetDatI(0x169, GetHockeyComLvF())) {
            return 0;
        }
        return pBest;
    }
    return 0;
}

// The enemy holding the puck, when the driver can reach it (not the keeper).
inline const CChrCore *CComDrvHockeyGame::GetAttackTargetCore()
{
    const CChrCore *pChr = GetHoldBallCore();
    if (pChr == 0) {
        return 0;
    }
    if (FriendOrEnemy(pChr->GetPlayerNo()) == 0) {
        return 0;
    }
    if (pChr->IsKeeper()) {
        return 0;
    }
    bcs::FVEC3 from = m_MyPos;
    bcs::FVEC3 to = pChr->GetPos();
    if (CanReachTarget(0, 1, from, to, 0.0f) == 0) {
        return 0;
    }
    return pChr;
}

// Whether to make attack nAttack (0 to 2, the one the character is at) on
// the enemy holding the puck.
int CComDrvHockeyGame::isAttack(long nAttack)
{
    if (nAttack < 0 || nAttack > 2) {
        return 0;
    }
    if (nAttack != m_pChr->GetNow1on1AttackNum()) {
        return 0;
    }
    if (m_pChr->IsHoldBall()) {
        return 0;
    }
    int bRet = 0;
    long nType = GetChrTypeID();
    if (nAttack == 0) {
        const CChrCore *pTarget = GetAttackTargetCore();
        if (pTarget == 0) {
            return 0;
        }
        float f0;
        float f1;
        float f2;
        GetAttack1Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistTargetCore(m_pChr, pTarget, f0, f1)) {
            if (CComManHockey::IsExistEnemyAngle(m_pChr, pTarget, 0x50)) {
                if (LotRate() < CComMan::GetDatI(0x4D9, GetHockeyComLvF())) {
                    bRet = 1;
                }
            } else {
                if (LotRate() < CComMan::GetDatI(0x4E1, GetHockeyComLvF())) {
                    bRet = 1;
                }
            }
        }
    }
    if (nAttack == 1) {
        const CChrCore *pTarget = GetAttackTargetCore();
        if (pTarget == 0) {
            return 0;
        }
        float f0;
        float f1;
        float f2;
        GetAttack2Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistTargetCore(m_pChr, pTarget, f0, f1, f2)) {
            if (CComManHockey::IsExistEnemyAngle(m_pChr, pTarget, 0x50)) {
                if (LotRate() < CComMan::GetDatI(0x15C7, GetHockeyComLvF())) {
                    bRet = 1;
                }
            } else {
                if (LotRate() < CComMan::GetDatI(0x15CF, GetHockeyComLvF())) {
                    bRet = 1;
                }
            }
        } else {
            if (LotRate() < CComMan::GetDatI(0x15D7, GetHockeyComLvF())) {
                bRet = 1;
            }
        }
    }
    if (nAttack == 2) {
        const CChrCore *pTarget = GetAttackTargetCore();
        if (pTarget == 0) {
            return 0;
        }
        float f0;
        float f1;
        float f2;
        GetAttack3Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistInAttack3Area(m_pChr, pTarget, f0, f1, f2)) {
            if (LotRate() < CComMan::GetDatI(0x15DF, GetHockeyComLvF())) {
                bRet = 1;
            }
        } else {
            if (LotRate() < CComMan::GetDatI(0x15E7, GetHockeyComLvF())) {
                bRet = 1;
            }
        }
    }
    return bRet;
}

// Whether pTarget is in reach of attack nAttack (0 to 2, the one the
// character is at).
int CComDrvHockeyGame::isAttack2(long nAttack, const CChrCore *pTarget)
{
    if (nAttack < 0 || nAttack > 2) {
        return 0;
    }
    if (nAttack != m_pChr->GetNow1on1AttackNum()) {
        return 0;
    }
    int bRet = 0;
    long nType = GetChrTypeID();
    if (nAttack == 0) {
        if (pTarget == 0) {
            return 0;
        }
        float f0;
        float f1;
        float f2;
        GetAttack1Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistTargetCore(m_pChr, pTarget, f0, f1)) {
            bRet = 1;
        }
    }
    if (nAttack == 1) {
        float f0;
        float f1;
        float f2;
        GetAttack2Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistTargetCore(m_pChr, pTarget, f0, f1, f2)) {
            bRet = 1;
        }
    }
    if (nAttack == 2) {
        float f0;
        float f1;
        float f2;
        GetAttack3Param(nType, f0, f1, f2);
        if (CComManHockey::IsExistInAttack3Area(m_pChr, pTarget, f0, f1, f2)) {
            bRet = 1;
        }
    }
    return bRet;
}

// With a pad player in the team: collects the team mates near enough to
// rData's enemy to take it over; 1 when the driver and a pad player are
// both among two or more of them.
int CComDrvHockeyGame::CheckSwitchRangeForPad(DEFENSE_CORE_DATA &rData)
{
    rData.m_apFriend[0] = 0;
    rData.m_apFriend[1] = 0;
    rData.m_apFriend[2] = 0;
    if (!IsExistPadDrv()) {
        return 0;
    }
    if (GetHoldBallerTeamNo() == -1 || GetHoldBallerTeamNo() == GetMyTeamNo()) {
        return 0;
    }
    int bHold = GetHoldBallCore() == rData.m_pEnemy;
    int bMe = 0;
    int bPad = 0;
    int bRet = 0;
    long nNum = 0;
    if (bHold) {
        float fDis = CComMan::GetDatF(0x1317, GetHockeyComLvF());
        long nDeg = CComMan::GetDatI(0x131F, GetHockeyComLvF());
        for (long i = 0; i < GetFriendNum(); i++) {
            float fDist = DisXZ(GetFriendWorldPos(i), CComMan::GetChrWorldPos(rData.m_pEnemy));
            if (fDis < fDist) {
                continue;
            }
            if (CComManHockey::IsExistToAngleLR(rData.m_pEnemy, GetFriendCore(i), nDeg)) {
                const CChrCore *pMe = m_pChr;
                if (GetFriendCore(i) == pMe) {
                    bMe = 1;
                }
                if (GetFriendCore(i)->IsPadCtrl()) {
                    bPad = 1;
                }
                rData.m_apFriend[nNum] = GetFriendCore(i);
                nNum++;
            }
        }
    } else {
        float fDis = CComMan::GetDatF(0x1327, GetHockeyComLvF());
        for (long i = 0; i < GetFriendNum(); i++) {
            float fDist = DisXZ(GetFriendWorldPos(i), CComMan::GetChrWorldPos(rData.m_pEnemy));
            if (fDis < fDist) {
                continue;
            }
            const CChrCore *pMe = m_pChr;
            if (GetFriendCore(i) == pMe) {
                bMe = 1;
            }
            if (GetFriendCore(i)->IsPadCtrl()) {
                bPad = 1;
            }
            rData.m_apFriend[nNum] = GetFriendCore(i);
            nNum++;
        }
    }
    if (nNum >= 2 && bMe == 1 && bPad == 1) {
        bRet = 1;
    }
    return bRet;
}

// Which enemy the driver should mark when the team mates of rData take
// over its enemy: rChr gets its player number, rHold whether it holds the
// puck. 0 for none.
int CComDrvHockeyGame::ChangeDefense2(DEFENSE_CORE_DATA &rData, long &rChr, long &rHold)
{
    long nTeam = GetMyTeamNo();
    bcs::FVEC3 aMyPos[3];
    bcs::FVEC3 aEmPos[3];
    CChrCore *apEnemy[3];
    bcs::FVEC3 *pMyPos = aMyPos;
    CChrCore **ppEnemy = apEnemy;
    bcs::FVEC3 *pEmPos = aEmPos;
    long nMyNum = 0;
    long nEmNum = 0;
    long nMyIdx = 0;
    for (long i = 0; i < GetCharaNum(); i++) {
        CChrCore *pChr = GetCharaCore(i);
        if (nTeam == pChr->GetTeamNo()) {
            if ((pChr == rData.m_apFriend[0] && pChr != m_pChr) || (pChr == rData.m_apFriend[1] && pChr != m_pChr) ||
                (pChr == rData.m_apFriend[2] && pChr != m_pChr)) {
                *pMyPos = CComMan::GetChrWorldPos(rData.m_pEnemy);
            } else {
                if (m_pChr == pChr) {
                    nMyIdx = nMyNum;
                }
                *pMyPos = GetFriendWorldPos(nMyNum);
            }
            pMyPos++;
            nMyNum++;
        } else {
            *ppEnemy = pChr;
            *pEmPos = GetEnemyWorldPos(nEmNum);
            nEmNum++;
            ppEnemy++;
            pEmPos++;
        }
    }
    long nBest = CComMan::GetNearBestPosBaseEx(aMyPos, aEmPos, nMyNum, nEmNum, nMyIdx);
    if (nBest < 0) {
        return 0;
    }
    CChrCore *pEnemy = apEnemy[nBest];
    rChr = pEnemy->GetPlayerNo();
    rHold = pEnemy->IsHoldBall();
    return 1;
}

// Without pad players: collects the computer team mates near enough to
// rData's enemy to take it over, rChr the nearest one's player number.
// -1 when the driver's team holds the puck (or nobody), 0 with fewer than
// two computer team mates, 1 when another enemy holds the puck.
int CComDrvHockeyGame::CheckSwitchRangeForCOM(DEFENSE_CORE_DATA &rData, long &rChr)
{
    rChr = -1;
    rData.m_apFriend[0] = 0;
    rData.m_apFriend[1] = 0;
    rData.m_apFriend[2] = 0;
    if (GetHoldBallerTeamNo() == -1 || GetHoldBallerTeamNo() == GetMyTeamNo()) {
        return -1;
    }
    long nComNum = 0;
    for (long i = 0; i < GetFriendNum(); i++) {
        if (!GetFriendCore(i)->IsPadCtrl()) {
            nComNum++;
        }
    }
    if (nComNum < 2) {
        return 0;
    }
    if (GetHoldBallCore() != rData.m_pEnemy) {
        return 1;
    }
    float fMin = 100.0f;
    long nNum = 0;
    float fDis = CComMan::GetDatF(0x133F, GetHockeyComLvF());
    long nDeg = CComMan::GetDatI(0x1347, GetHockeyComLvF());
    for (long i = 0; i < GetFriendNum(); i++) {
        if (GetFriendCore(i)->IsPadCtrl()) {
            continue;
        }
        float fDist = DisXZ(GetFriendWorldPos(i), CComMan::GetChrWorldPos(rData.m_pEnemy));
        if (fDis < fDist) {
            continue;
        }
        if (CComManHockey::IsExistToAngleLR(rData.m_pEnemy, GetFriendCore(i), nDeg)) {
            if (fDist < fMin) {
                rChr = GetFriendIdx(i);
                fMin = fDist;
            }
            rData.m_apFriend[nNum] = GetFriendCore(i);
            nNum++;
        }
    }
    return nNum >= 1;
}

// Whether a pad player of the other team stands in the way of pChr's shot
// at the goal.
int CComDrvHockeyGame::CheckAhoShoot(const CChrCore *pChr)
{
    float fWidth = CComMan::GetDatF(0x99, GetHockeyComLvF());
    if (fWidth <= 0.0f) {
        return 0;
    }
    float fLv = GetHockeyComLvF();
    bcs::FVEC3 pos = pChr->GetPos();
    bcs::FVEC3 goal = pChr->GetTargetGoalPos();
    float fLen = DisXZ(pos, goal);
    for (long i = 0; i < GetEnemyNum(); i++) {
        if (GetEnemyCore(i)->IsPadCtrl() &&
            CComMan::IsPointInFaceXZ(static_cast<const CChrCore *>(GetEnemyCore(i))->GetPos(), pos, goal, fWidth, fLen)) {
            return 1;
        }
    }
    return 0;
}

// Whether to go for a green shell (stage 2), rPos gets where it is.
int CComDrvHockeyGame::isGetGreenShell(bcs::FVEC3 &rPos)
{
    if (m_nStage != 2) {
        return 0;
    }
    if (m_pChr->IsHaveStockItem()) {
        return 0;
    }
    float fDist;
    int bRet = CComManHockeyGame::GetExisGreenShellDist(m_MyPos, fDist, rPos);
    if (bRet == 0) {
        return 0;
    }
    if (fDist > CComMan::GetDatF(0x10AC, GetChrTypeID())) {
        return 0;
    }
    return bRet;
}

// Whether the way to pos runs into the invisible wall of the view.
int CComDrvHockeyGame::isHitInvisibleWall(bcs::FVEC3 pos)
{
    if (CComMan::IsEnableViewColl()) {
        float fWallZ = CComMan::GetViewCollPosZ();
        if (fWallZ < 999.9f) {
            long nDir = CComMan::GetViewCollTargetDir();
            if (nDir >= 0) {
                bcs::FVEC3 my = m_MyPos;
                float fMargin = 1.9f;
                if (nDir == 0) {
                    if (pos.z < fWallZ) {
                        return 0;
                    }
                    if (my.z + fMargin >= fWallZ) {
                        return 1;
                    }
                } else {
                    if (pos.z > fWallZ) {
                        return 0;
                    }
                    if (my.z - fMargin <= fWallZ) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// How far the character reaches (character type 9 further).
float CComDrvHockeyGame::GetCharaTypeReachDist()
{
    float fDist;
    if (GetChrTypeID() != 9) {
        fDist = 1.5f;
    } else {
        fDist = 2.0f;
    }
    return fDist;
}

int CComDrvHockeyGame::isExistBossPackun(bcs::FVEC3 &rPos)
{
    if (m_nStage != 7) {
        return 0;
    }
    CComManHockey *pMan = CComManHockey::GetInstance();
    if (pMan == 0) {
        rPos.Set(0.0f, 0.0f, 0.0f);
        return 0;
    }
    return pMan->isExistBossPackun(rPos);
}

// The width of the character's special shot, from the computer data.
float CComDrvHockeyGame::GetSpJudgeWidth()
{
    float fLv = GetHockeyComLvF();
    switch (GetChrTypeID()) {
        case 0:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
            return CComMan::GetDatF(0x6A8, fLv);
        case 1:
            return CComMan::GetDatF(0x6D8, fLv);
        case 2:
            return CComMan::GetDatF(0x6F0, fLv);
        case 3:
            return CComMan::GetDatF(0x720, fLv);
        case 4:
            return CComMan::GetDatF(0x750, fLv);
        case 5:
            return CComMan::GetDatF(0x780, fLv);
        case 6:
            return CComMan::GetDatF(0x7B0, fLv);
        case 7:
            return CComMan::GetDatF(0x7E0, fLv);
        case 8:
            return CComMan::GetDatF(0x810, fLv);
        case 9:
            return CComMan::GetDatF(0x840, fLv);
        case 10:
            return CComMan::GetDatF(0x870, fLv);
        case 11:
            return CComMan::GetDatF(0x8A0, fLv);
        case 12:
            return CComMan::GetDatF(0x8D0, fLv);
        case 13:
            return CComMan::GetDatF(0x900, fLv);
        case 14:
            return CComMan::GetDatF(0x930, fLv);
        case 15:
            return CComMan::GetDatF(0x960, fLv);
        case 16:
            return CComMan::GetDatF(0x998, fLv);
        case 17:
            return CComMan::GetDatF(0x9C8, fLv);
        default:
            return 0.0f;
    }
}

// Whether an enemy that can hit the puck stands in the way of a special
// shot at pos.
int CComDrvHockeyGame::isExistEnemySpArea(bcs::FVEC3 pos)
{
    float fWidth = GetSpJudgeWidth();
    if (fWidth == 0.0f) {
        return 0;
    }
    float fLen = m_pChr->GetDist_ToPosXZ(pos);
    for (long i = 0; i < GetEnemyNum(); i++) {
        if (GetEnemyCore(i)->IsHockeyBallHit() &&
            CComMan::IsPointInFaceXZ(static_cast<const CChrCore *>(GetEnemyCore(i))->GetPos(), m_MyPos, pos, fWidth, fLen)) {
            return 1;
        }
    }
    return 0;
}

// The course of a special shot at pos (-100 to 100): away from the side
// most enemies in the way stand on (the nearest one's on a tie), at random
// with none.
long CComDrvHockeyGame::GetSpShootCourse(bcs::FVEC3 pos)
{
    float fWidth = GetSpJudgeWidth();
    if (fWidth == 0.0f) {
        return 0;
    }
    float fLen = m_pChr->GetDist_ToPosXZ(pos);
    long nMin = 100;
    long nNearSide = 0;
    long nNum = 0;
    long nLeft = 0;
    long nRight = 0;
    for (long i = 0; i < GetEnemyNum(); i++) {
        if (!GetEnemyCore(i)->IsHockeyBallHit()) {
            continue;
        }
        if (!CComMan::IsPointInFaceXZ(static_cast<const CChrCore *>(GetEnemyCore(i))->GetPos(), m_MyPos, pos, fWidth, fLen)) {
            continue;
        }
        const CChrCore *pMe = m_pChr;
        long nSide = CComManHockey::IsExistTargetLR(pMe, GetEnemyCore(i), pos, 0x5A);
        if (nSide > 0) {
            if (nSide == 1) {
                nLeft++;
            }
            if (nSide != 1) {
                nRight++;
            }
            pMe = m_pChr;
            float fDist = pMe->GetDist_ToPosXZ(static_cast<const CChrCore *>(GetEnemyCore(i))->GetPos());
            if (fDist < nMin) {
                nNearSide = nSide;
                nMin = fDist;
            }
            nNum++;
        }
    }
    if (nNum <= 0) {
        return CComManHockey::GetRandValueI(-100, 100);
    }
    if (nLeft > nRight) {
        return 100;
    }
    if (nLeft < nRight) {
        return -100;
    }
    long nCourse = -100;
    if (nNearSide == 1) {
        nCourse = 100;
    }
    return nCourse;
}

// The positions of the ninja's two clones (obstacles of kind 0xE); the
// number found.
int CComDrvHockeyGame::GetCloneNinja(bcs::FVEC3 &rPos0, bcs::FVEC3 &rPos1)
{
    long nObsNum = CChrMan::ObstacleGetNum();
    if (nObsNum == 0) {
        return 0;
    }
    CChrObstacle *pObs = 0;
    int nNum = 0;
    for (long i = 0; i < nObsNum; i++) {
        pObs = CChrMan::ObstacleSearch(pObs);
        if (pObs == 0) {
            break;
        }
        if (pObs->IsAttackEnd() == 1 || pObs->GetObstacleType() != 0xE) {
            continue;
        }
        if (nNum == 0) {
            nNum++;
            rPos0 = pObs->GetPos();
        } else if (nNum == 1) {
            nNum++;
            rPos1 = pObs->GetPos();
        }
    }
    return nNum;
}

// Whether pos is in a magic circle (obstacles of kind 0xC); rEscPos gets a
// place out of it. -1 without obstacles.
int CComDrvHockeyGame::isEnterMagicCircle(bcs::FVEC3 pos, bcs::FVEC3 &rEscPos)
{
    long nObsNum = CChrMan::ObstacleGetNum();
    if (nObsNum == 0) {
        return -1;
    }
    float fRadius = 6.0f;
    CChrObstacle *pObs = 0;
    for (long i = 0; i < nObsNum; i++) {
        pObs = CChrMan::ObstacleSearch(pObs);
        if (pObs == 0) {
            break;
        }
        if (pObs->IsAttackEnd() == 1 || pObs->GetObstacleType() != 0xC) {
            continue;
        }
        bcs::FVEC3 my = pos;
        bcs::FVEC3 center = pObs->GetPos();
        if (bcs::BCS_IsPointInEllipse(my.x - center.x, my.z - center.z, fRadius, fRadius)) {
            bcs::FVEC3 offset;
            GetRandomOffsetPos(fRadius, 2.0f + fRadius, offset);
            bcs::FVEC3 esc;
            bcs::FVEC3Sum(esc, center, offset);
            rEscPos = esc;
            HockeyPosTrim(rEscPos);
            HockeyGoalPosTrim(rEscPos);
            return 1;
        }
    }
    return 0;
}

// Whether the driver is in a smoke screen (obstacles of kind 4 on the
// ice). -1 without obstacles.
int CComDrvHockeyGame::isEnterSmokeScreen()
{
    long nObsNum = CChrMan::ObstacleGetNum();
    if (nObsNum == 0) {
        return -1;
    }
    float fHeight = 1.0f;
    float fRadius = 6.0f;
    CChrObstacle *pObs = 0;
    for (long i = 0; i < nObsNum; i++) {
        pObs = CChrMan::ObstacleSearch(pObs);
        if (pObs == 0) {
            break;
        }
        if (pObs->IsAttackEnd() == 1 || pObs->GetObstacleType() != 4) {
            continue;
        }
        bcs::FVEC3 my = m_MyPos;
        bcs::FVEC3 center = pObs->GetPos();
        if (center.y < fHeight && bcs::BCS_IsPointInEllipse(my.x - center.x, my.z - center.z, fRadius, fRadius)) {
            return 1;
        }
    }
    return 0;
}

int CComDrvHockeyGame::isAgeShoot() const
{
    if (m_fShootWait <= 0.0f && m_bEarlyGame == 0) {
        return 0;
    }
    return 1;
}

int CComDrvHockeyGame::isTeamCoinMAX() const
{
    return CComManHockey::isTeamCoinMAX(m_pChr);
}

// Whether pos is behind the goal the team attacks or the one at 0x148.
int CComDrvHockeyGame::isBehindGoal(bcs::FVEC3 pos) const
{
    bcs::FVEC3 target = m_TargetGoalPos;
    bcs::FVEC3 goal = m_Pos148;
    if (target.z > 0.0f) {
        if (pos.z >= target.z) {
            return 1;
        }
    } else {
        if (pos.z <= target.z) {
            return 1;
        }
    }
    if (goal.z > 0.0f) {
        if (pos.z >= goal.z) {
            return 1;
        }
    } else {
        if (pos.z <= goal.z) {
            return 1;
        }
    }
    return 0;
}

int CComDrvHockeyGame::isSpCatchSuccess()
{
    long nRate = CComMan::GetDatI(0xA00, GetHockeyComLvF());
    return LotRate() < nRate;
}

// Whether pos is in the fire circle in front of the goal at 0x148; rEscPos
// gets a place out of it. -1 without obstacles.
int CComDrvHockeyGame::isEnterFireCircle(bcs::FVEC3 pos, bcs::FVEC3 &rEscPos)
{
    if (m_nUnk458 == 0 && CChrMan::ObstacleGetNum() == 0) {
        return -1;
    }
    if (CChrMan::ObstacleGetNum() > 0) {
        m_nUnk45C = 1;
    }
    if (m_nUnk45C == 1 && CChrMan::ObstacleGetNum() == 0) {
        m_nUnk458 = 0;
    }
    bcs::FVEC3 my = pos;
    bcs::FVEC3 center = m_Pos148;
    if (center.z > 0.0f) {
        center.z -= 2.0f;
    } else {
        center.z += 2.0f;
    }
    float fRadius = 6.0f;
    if (bcs::BCS_IsPointInEllipse(my.x - center.x, my.z - center.z, fRadius, fRadius)) {
        GetRandomSectorPositionToDefenseGoal(4.0f + fRadius, 4.5f + fRadius, 80.0f, rEscPos);
        HockeyPosTrim(rEscPos);
        HockeyGoalPosTrim(rEscPos);
        return 1;
    }
    return 0;
}

// The player number of the character making a special shot (state 0x27);
// -1 for none.
long CComDrvHockeyGame::GetSpShootCharIdx()
{
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && !pChr->IsKeeper() && pChr->GetState() == 0x27) {
            return pChr->GetPlayerNo();
        }
    }
    return -1;
}

}
