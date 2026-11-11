// msw::CComDrvBoss, the computer driver of the boss battle: the action
// table, the constructor, and the home positions the characters keep round
// the Behemoth. The actions are in the file before
// (game/boss/unit_4F0C4.cpp), the message and think functions in the next
// one (game/boss/unit_51CE4.cpp).
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBoss.hpp"
#include "game/msw/CNpcBehemothBoss.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CStageMan.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"

namespace {
    // The Behemoth (the first NPC of kind 0), or null. Inline in the
    // original: the driver's functions call CChrMan::NpcSearch themselves,
    // although CComDrvBoss::GetBehemoth exists.
    inline msw::CNpcBehemothBoss *SearchBehemoth()
    {
        return static_cast<msw::CNpcBehemothBoss *>(msw::CChrMan::NpcSearch(0, 0));
    }

    // (guessed helper) The direction from rFrom to rTo in the XZ plane,
    // length 1 (zero when they are at one place).
    inline void GetDirXZ(bcs::FVEC3 &rDir, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 diff;
        diff.x = rTo.x - rFrom.x;
        diff.z = rTo.z - rFrom.z;
        bcs::BCS_Vec_Normalize(rDir, bcs::FVEC3(diff.x, 0.0f, diff.z));
    }

    // (guessed helper) fValue limited to fMin..fMax.
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // (guessed helper) fRot brought into 0..2 pi.
    inline float NormalizeRot(float fRot)
    {
        while (fRot > 6.2831855f) {
            fRot -= 6.2831855f;
        }
        while (fRot < 0.0f) {
            fRot += 6.2831855f;
        }
        return fRot;
    }

    // (guessed helper) Degrees to radians.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }
}

namespace msw {

CComDrvBoss::ActFunc CComDrvBoss::g_ActFuncMap[ACT_NUM] = {
    &CComDrv::ActLoopNop,
    &CComDrv::ActLoopWait,
    &CComDrv::ActLoopWaitNeutral,
    &CComDrv::ActLoopLookTarget,
    &CComDrv::ActLoopLookFront,
    &CComDrv::ActLoopLookBall,
    &CComDrv::ActLoopRun,
    &CComDrv::ActLoopRunTurn,
    &CComDrv::ActLoopJump,
    &CComDrv::ActLoopJumpBlock,
    &CComDrvBoss::ActLoopGoHome,
    &CComDrvBoss::ActLoopGoRand,
    &CComDrvBoss::ActLoopGetBall,
    &CComDrvBoss::ActLoopReceiveGo,
    &CComDrvBoss::ActLoopSpikeGo,
    &CComDrvBoss::ActLoopStickAttack,
    &CComDrvBoss::ActLoopShoot,
    &CComDrvBoss::ActLoopThrowItem,
    &CComDrvBoss::ActLoopGetQpanel,
};


// The action work is at least 0x2C bytes, this driver's own need.
static inline unsigned long CalcWorkMaxSize(const CComDrv *pDrv)
{
    unsigned long nBase = pDrv->CalcWorkBaseMaxSize();
    unsigned long nSize = 0x2C;
    if (nBase > nSize) {
        nSize = nBase;
    }
    return nSize;
}

CComDrvBoss::CComDrvBoss() : CComDrv(CalcWorkMaxSize(this))
{
    ClearAll();
}

CComDrvBoss::~CComDrvBoss()
{
}

void CComDrvBoss::ClearAll()
{
    CComDrv::ClearAll();
    for (long i = 0; i < 4; i++) {
        m_aHomePos[i].Set(0.0f, 0.0f, 0.0f);
        m_aHomeNearPos[i].Set(0.0f, 0.0f, 0.0f);
    }
    m_nHomeIdx = 1;
    m_BossPos.Set(0.0f, 0.0f, 0.0f);
    m_fBossRotY = 0.0f;
    m_RelayPos.Set(0.0f, 0.0f, 0.0f);
    m_nRelayStep = -1;
    m_nStickAttackStep = 0;
}

void CComDrvBoss::UpdateInfo()
{
    CComDrv::UpdateInfo();
    CNpcBehemothBoss *pBoss = SearchBehemoth();
    if (pBoss != 0) {
        m_BossPos = pBoss->GetPos();
        m_BossPos.y = 0.0f;
        m_fBossRotY = pBoss->GetRot().y;
    }
    UpdateHomePos();
}

void CComDrvBoss::EntryActCallBack(long)
{
    if (GetEntryActNum() == 1) {
        m_nStickAttackStep = 0;
    }
}

// The length of the way from rFrom to rTo, through the relay point when the
// Behemoth is in between.
inline float CComDrvBoss::GetRelayDistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
{
    bcs::FVEC3 relay;
    if (Util_GetRelayPosForBoss(relay, rFrom, rTo)) {
        return CComMan::GetDisXZ(rFrom, relay) + CComMan::GetDisXZ(relay, rTo);
    }
    return CComMan::GetDisXZ(rFrom, rTo);
}

// Keeps rPos out of the Behemoth's body and inside the court.
void CComDrvBoss::NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const
{
    CNpcBehemothBoss *pBoss = SearchBehemoth();
    if (pBoss == 0) {
        return;
    }

    // The way from the character to rPos, against the Behemoth's body (a
    // capsule), all on the ground.
    bcs::FVEC3 hit;
    bcs::FVEC3 body0;
    bcs::FVEC3 body1;
    bcs::FVEC3 from = m_MyWorldPos;
    float fSize = m_pChr->GetCollSize_BodyPush();
    float fRadius = pBoss->GetBodyCollInfo(body0, body1) + fSize;
    rPos.y = 0.0f;
    from.y = 0.0f;
    body0.y = 0.0f;
    body1.y = 0.0f;
    hit.Set(0.0f, 0.0f, 0.0f);
    if (CCollMan::DetectHitPos_SegmentCapsule(from, rPos, body0, body1, 0.5f + fRadius, hit)) {
        rPos = hit;
    }

    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    float fDist = GetDisXZ(center, rPos);
    float fSize2 = m_pChr->GetCollSize_BodyPush();
    float fMax = CStageMan::GetInstance()->GetCourtRadius() - fSize2 - 0.5f;
    if (fDist >= fMax) {
        bcs::FVEC3 dir;
        GetDirXZ(dir, center, rPos);
        dir.x *= fMax;
        dir.z *= fMax;
        rPos.x = dir.x + center.x;
        rPos.y = 0.0f;
        rPos.z = dir.z + center.z;
    }
}

CNpcBehemothBoss *CComDrvBoss::GetBehemoth() const
{
    return SearchBehemoth();
}

// Places the four home positions round the Behemoth (in front, 72 degrees
// to either side, behind) and picks the side home (1 or 2) for the driver:
// the side it is on, or the other one when its team mate is on the same
// side and nearer.
void CComDrvBoss::UpdateHomePos()
{
    const float aRot[4] = {0.0f, 1.2566371f, -1.2566371f, 3.1415927f};
    const float aFar[4] = {15.0f, 10.0f, 10.0f, 15.0f};
    const float aNear[4] = {7.0f, 4.0f, 4.0f, 7.0f};
    for (long i = 0; i < 4; i++) {
        float fRot = NormalizeRot(m_fBossRotY + aRot[i]);
        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, aFar[i], fRot);
        bcs::FVEC3Add(&pos, &pos, &m_BossPos);
        m_aHomePos[i] = pos;
        bcs::BCS_VecZ_RotY(pos, aNear[i], fRot);
        bcs::FVEC3Add(&pos, &pos, &m_BossPos);
        m_aHomeNearPos[i] = pos;
    }

    long nMate = 0;
    bcs::FVEC3 matePos(0.0f, 0.0f, 0.0f);
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && !pChr->IsPadCtrl() && m_nMyTeam == pChr->GetTeamNo()) {
            nMate++;
            if (m_nMyChrIdx != pChr->GetPlayerNo()) {
                matePos = CComMan::GetChrPos(pChr);
            }
        }
    }

    if (nMate == 2) {
        long nMySide = bcs::BCS_Util_IsPointSide(m_MyPos.x, m_MyPos.z, m_aHomeNearPos[3].x, m_aHomeNearPos[3].z,
                                                 m_aHomeNearPos[0].x, m_aHomeNearPos[0].z);
        long nMateSide = bcs::BCS_Util_IsPointSide(matePos.x, matePos.z, m_aHomeNearPos[3].x, m_aHomeNearPos[3].z,
                                                   m_aHomeNearPos[0].x, m_aHomeNearPos[0].z);
        if (nMySide >= 0) {
            m_nHomeIdx = 2;
        } else {
            m_nHomeIdx = 1;
        }
        if ((nMySide >= 0 && nMateSide >= 0) || (nMySide <= 0 && nMateSide <= 0)) {
            float fMyDist = GetRelayDistXZ(m_MyPos, m_aHomePos[m_nHomeIdx]);
            float fMateDist = GetRelayDistXZ(matePos, m_aHomePos[m_nHomeIdx]);
            if (fMyDist > fMateDist) {
                if (nMySide >= 0) {
                    m_nHomeIdx = 1;
                } else {
                    m_nHomeIdx = 2;
                }
            }
        }
    } else {
        if (bcs::BCS_Util_IsPointSide(m_MyPos.x, m_MyPos.z, m_aHomeNearPos[3].x, m_aHomeNearPos[3].z,
                                      m_aHomeNearPos[0].x, m_aHomeNearPos[0].z) >= 0) {
            m_nHomeIdx = 2;
        } else {
            m_nHomeIdx = 1;
        }
    }
}

// Away from the Behemoth: 15 to 20 beyond it seen from the court's centre,
// up to 45 degrees off.
inline void CComDrvBoss::GetEscPos(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 dir;
    float fRotRange = 45.0f;
    float fDist = RandRangeF(15.0f, 20.0f);
    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    fRotRange = Clamp(fRotRange, 0.0f, 90.0f);
    float fRot = NormalizeRot(DegToRad(RandRangeF(180.0f - fRotRange, 180.0f + fRotRange)));
    GetDirXZ(dir, center, m_BossPos);
    bcs::BCS_Vec_RotY(dir, dir, fRot);
    dir.x *= fDist;
    dir.z *= fDist;
    bcs::FVEC3Add(&rPos, &dir, &center);
    rPos.y = 0.0f;
}

// Behind the Behemoth while it is dangerous; otherwise beside the driver's
// side home.
void CComDrvBoss::GetHomePos(bcs::FVEC3 &rPos)
{
    float fRotRange = 80.0f;
    float fDist = RandRangeF(9.0f, 11.0f);
    bcs::FVEC3 ballPos;
    if (CComManBoss::GetBallTargetPos(m_pBall, ballPos)) {
        fDist = RandRangeF(7.0f, 9.0f);
    }

    if (ShoudEscFromBoss()) {
        GetEscPos(rPos);
        return;
    }
    GetHomeRandPosBase(rPos, fRotRange, fDist);
}

// The same, further out.
void CComDrvBoss::GetHomeRandPos(bcs::FVEC3 &rPos)
{
    float fRotRange = 80.0f;
    float fDist = RandRangeF(9.0f, 18.0f);

    if (ShoudEscFromBoss()) {
        GetEscPos(rPos);
        return;
    }
    GetHomeRandPosBase(rPos, fRotRange, fDist);
}

// A place fDist beyond one end of the Behemoth's body, on the driver's side
// (fRotRange degrees round 90 from its front); either side when it is near
// the court's edge.
void CComDrvBoss::GetHomeRandPosBase(bcs::FVEC3 &rPos, float fRotRange, float fDist)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    CNpcBehemothBoss *pBoss = SearchBehemoth();
    if (pBoss == 0) {
        return;
    }

    fRotRange = Clamp(fRotRange, 0.0f, 90.0f);
    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    int bNearEdge = 0;
    float fBossDist = GetDisXZ(center, m_BossPos);
    if (fBossDist >= CStageMan::GetInstance()->GetCourtRadius() - 2.0f) {
        bNearEdge = 1;
    }

    bcs::FVEC3 body0;
    bcs::FVEC3 body1;
    fDist = fDist + pBoss->GetBodyCollInfo(body0, body1);
    float fRot = RandRangeF(90.0f - fRotRange, 90.0f + fRotRange);
    if (m_nHomeIdx == 2) {
        fRot = -fRot;
    }
    if (bNearEdge && (RandI() & 1)) {
        fRot = -fRot;
    }
    fRot = NormalizeRot(m_fBossRotY + DegToRad(fRot));

    bcs::FVEC3 ofs;
    bcs::BCS_VecZ_RotY(ofs, fDist, fRot);
    if (RandI() & 1) {
        bcs::FVEC3Add(&rPos, &ofs, &body0);
    } else {
        bcs::FVEC3Add(&rPos, &ofs, &body1);
    }
    rPos.y = 0.0f;
}

// Whether the driver is the character that should go for the ball: the one
// with the shortest way to where it comes down.
int CComDrvBoss::ShoudReceiveGo()
{
    bcs::CSimpleSortBase<32>::SORT *pSort;
    bcs::FVEC3 target;
    if (!CComManBoss::GetBallTargetPos(m_pBall, target)) {
        return 0;
    }
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return 0;
    }

    bcs::CSimpleSortBase<32>::SORT aSort[32];
    pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && CComManBoss::CanbeTouch(pChr, nBall)) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            float fDist = GetRelayDistXZ(pos, target);
            nNum++;
            pSort->m_nId = pChr->GetPlayerNo();
            pSort->m_fParam = fDist;
            pSort++;
        }
    }

    if (nNum <= 0) {
        return 0;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
    }
    if (aSort[0].m_nId == m_nMyChrIdx) {
        return 1;
    }
    return 0;
}

// Whether the driver should go for the nearest "?" panel: when it is the
// team's character nearest to it.
int CComDrvBoss::IsShoudGetQPanel() const
{
    unsigned long nPanel = CComMan::QPanel_GetNear(m_MyPos, 1);
    if (nPanel == 0) {
        return 0;
    }
    CQPanel *pPanel = CComMan::QPanel_GetPtr(nPanel);
    if (pPanel == 0) {
        return 0;
    }
    return QPanel_GetNearMyTeamChr(pPanel) == m_nMyChrIdx;
}

// The character of the driver's team, without an item, nearest to pPanel
// with the Behemoth not in between; -1 for none.
long CComDrvBoss::QPanel_GetNearMyTeamChr(CQPanel *pPanel) const
{
    if (pPanel == 0) {
        return -1;
    }
    if (!pPanel->CanAccess()) {
        return -1;
    }

    bcs::FVEC3 panelPos = pPanel->GetPos();
    bcs::FVEC3 pos = World2Local(panelPos, -1);
    long nNear = -1;
    float fMin = 99999.0f;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && !pChr->IsHaveStockItem()) {
            bcs::FVEC3 chrPos = CComMan::GetChrPos(pChr);
            float z;
            float x;
            if (!bcs::BCS_GetSegLineToSegLineCross(&x, &z, chrPos.x, chrPos.z, pos.x, pos.z, m_aHomeNearPos[0].x,
                                                   m_aHomeNearPos[0].z, m_aHomeNearPos[3].x, m_aHomeNearPos[3].z)) {
                float fDist = GetDisXZ(chrPos, pos);
                if (fDist < fMin) {
                    nNear = i;
                    fMin = fDist;
                }
            }
        }
    }
    return nNear;
}

// Whether the characters should keep away from the Behemoth: in its state
// 9, or while one of its meteors (obstacle 0x12) is out.
int CComDrvBoss::ShoudEscFromBoss() const
{
    CNpcBehemothBoss *pBoss = SearchBehemoth();
    if (pBoss == 0) {
        return 0;
    }
    if (pBoss->GetState() == 9) {
        return 1;
    }
    for (CChrObstacle *pObstacle = CComMan::ObstacleSearch(0); pObstacle != 0;
         pObstacle = CComMan::ObstacleSearch(pObstacle)) {
        if (pObstacle->GetObstacleType() == 0x12) {
            return 1;
        }
    }
    return 0;
}

// The point to run through on the way from rFrom to rTo when the line
// through the Behemoth (front to back) is in the way: the front or the back
// home, whichever is nearer to rFrom. Zero, and rTo itself, when the way
// is free.
int CComDrvBoss::Util_GetRelayPosForBoss(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
{
    float z;
    float x;
    if (!bcs::BCS_GetSegLineToSegLineCross(&x, &z, rFrom.x, rFrom.z, rTo.x, rTo.z, m_aHomeNearPos[0].x,
                                           m_aHomeNearPos[0].z, m_aHomeNearPos[3].x, m_aHomeNearPos[3].z)) {
        rOut = rTo;
        return 0;
    }
    float fFrontDist = CComMan::GetDisXZ(rFrom, m_aHomePos[0]);
    if (fFrontDist < CComMan::GetDisXZ(rFrom, m_aHomePos[3])) {
        rOut = m_aHomePos[0];
    } else {
        rOut = m_aHomePos[3];
    }
    return 1;
}

int CComDrvBoss::GetBallTargetPos(bcs::FVEC3 &rPos) const
{
    return CComManBoss::GetBallTargetPos(m_pBall, rPos);
}

int CComDrvBoss::CanbeMyTouch() const
{
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return 0;
    }
    return CComManBoss::CanbeTouch(m_pChr, nBall);
}

} // namespace msw
