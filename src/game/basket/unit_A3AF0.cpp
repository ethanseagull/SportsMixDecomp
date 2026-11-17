// msw::CComDrvBasketGame: constructor and destructor, the home positions
// on offence and defence, the scores and draws the think functions ask
// (CalcHoldBallScore, LotRate*, Judge*), the answers to the game's shot
// messages, the court "panels" and the stage gimmick avoidance
// (Stage*_Get*, GetPosTurnAround). The file also holds the action table
// g_ActFuncMap (its first .rodata object: every function here addresses its
// float constants from it) and the vtable. The file name is not known.
#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CItemBallBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CStage10Base.hpp"
#include "game/msw/CStageBasket03.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_WaterPillar.hpp"

#include "game/msw/CStageMan.hpp"

// 70 degrees in radians, unused (as in CComManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {
    typedef bcs::CSimpleSortBase<32> SortBase;

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi], and into (-pi, pi].
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

    // (guessed helper) rPos1 + rPos2.
    inline bcs::FVEC3 Sum(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 sum;
        return *bcs::FVEC3Add(&sum, &rPos1, &rPos2);
    }

    // (guessed helper) rIn's direction in the XZ plane, length 1 (zero
    // when rIn has no XZ part).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
    }

    // (guessed helper) A character's position through a const pointer (the
    // original reads it so in some places).
    inline const bcs::FVEC3 &GetPosC(const msw::CChrCore *pChr)
    {
        return pChr->GetPos();
    }

    // (guessed helper) The computer manager of the basketball game.
    inline msw::CComManBasketGame *GetComManBK()
    {
        return static_cast<msw::CComManBasketGame *>(msw::CComMan::GetInstance());
    }

    // (guessed helper) The basketball game.
    inline msw::CGameBasket *GetGameBK()
    {
        return static_cast<msw::CGameBasket *>(msw::CGame::m_pInstance);
    }
}

namespace msw {

// The actions by number (ACT_TYPE); OnMainSub_ runs the one on top every
// frame.
const CComDrvBasketGame::ActFunc CComDrvBasketGame::g_ActFuncMap[] = {
    &CComDrv::ActLoopNop,
    &CComDrv::ActLoopWait,
    &CComDrv::ActLoopWaitNeutral,
    &CComDrv::ActLoopLookTarget,
    &CComDrv::ActLoopLookFront,
    &CComDrv::ActLoopLookBall,
    &CComDrv::ActLoopRun,
    &CComDrv::ActLoopRunTurn,
    &CComDrvBasketGame::ActLoopJump,
    &CComDrv::ActLoopJumpBlock,
    &CComDrvBasketGame::ActLoopJumpBall,
    &CComDrvBasketGame::ActLoopGoOffense,
    &CComDrvBasketGame::ActLoopGoOffense2,
    &CComDrvBasketGame::ActLoopGoDefense,
    &CComDrvBasketGame::ActLoopGoDefense2,
    &CComDrvBasketGame::ActLoopGoDefense3,
    &CComDrvBasketGame::ActLoopGoDefPassCourse,
    &CComDrvBasketGame::ActLoopGoDefOnBaller,
    &CComDrvBasketGame::ActLoopGoHome,
    &CComDrvBasketGame::ActLoopGoFreeArea,
    &CComDrvBasketGame::ActLoopGetBall,
    &CComDrvBasketGame::ActLoopGetHatenaPanel,
    &CComDrvBasketGame::ActLoopGoRunRand,
    &CComDrvBasketGame::ActLoopGoRun,
    &CComDrvBasketGame::ActLoopPass,
    &CComDrvBasketGame::ActLoopPassGhost,
    &CComDrvBasketGame::ActLoopShootCancelPass,
    &CComDrvBasketGame::ActLoopCancelPass,
    &CComDrvBasketGame::ActLoopPassSign,
    &CComDrvBasketGame::ActLoopJumpShoot,
    &CComDrvBasketGame::ActLoopDunk,
    &CComDrvBasketGame::ActLoopGoReboundDunk,
    &CComDrvBasketGame::ActLoopReboundDunk,
    &CComDrvBasketGame::ActLoopAlleyoop,
    &CComDrvBasketGame::ActLoopAssistPlay,
    &CComDrvBasketGame::ActLoopBlockade,
    &CComDrvBasketGame::ActLoopGoBack,
    &CComDrvBasketGame::ActLoopGoZiguzagu,
    &CComDrvBasketGame::ActLoopGoSide,
    &CComDrvBasketGame::ActLoopSpinMove,
    &CComDrvBasketGame::ActLoopAirTrick,
    &CComDrvBasketGame::ActLoopStickDash,
    &CComDrvBasketGame::ActLoopGoTargetGoal,
    &CComDrvBasketGame::ActLoopBallKeep,
    &CComDrvBasketGame::ActLoopGoDunkArea,
    &CComDrvBasketGame::ActLoopGoDunk,
    &CComDrvBasketGame::ActLoopLayUp,
    &CComDrvBasketGame::ActLoopCancelLayUp,
    &CComDrvBasketGame::ActLoopItemShoot,
    &CComDrvBasketGame::ActLoopCombination,
    &CComDrvBasketGame::ActLoopGoEnemy,
    &CComDrvBasketGame::ActLoopGoBall,
    &CComDrvBasketGame::ActLoopRebound,
    &CComDrvBasketGame::ActLoopCut,
    &CComDrvBasketGame::ActLoopMissCut,
    &CComDrvBasketGame::ActLoopJump,
    &CComDrvBasketGame::ActLoopDefJump,
    &CComDrvBasketGame::ActLoopJumpCut,
    &CComDrvBasketGame::ActLoopJumpUpper,
    &CComDrvBasketGame::ActLoopDefAlleyoop,
    &CComDrvBasketGame::ActLoopItemEscape,
    &CComDrvBasketGame::ActLoopGoEscape,
    &CComDrvBasketGame::ActLoopGoEscape2,
    &CComDrvBasketGame::ActLoopItemThrow,
    &CComDrvBasketGame::ActLoopDefGhost,
    &CComDrvBasketGame::ActLoopCutChance,
    &CComDrvBasketGame::ActLoopGoSmash,
    &CComDrvBasketGame::ActLoopMoveBlock,
    &CComDrvBasketGame::ActLoopGoDefenseGoal,
    &CComDrvBasketGame::ActLoopShootBlock,
    &CComDrvBasketGame::ActLoopJumpUpperChance,
    &CComDrvBasketGame::ActLoopJumpCutChance,
    &CComDrvBasketGame::ActLoopGoPointPanel,
    &CComDrvBasketGame::ActLoopGetGShell,
    &CComDrvBasketGame::ActLoopGoEspBubble,
    &CComDrvBasketGame::ActLoopScript,
};

// The action work is at least 0x3C bytes, this driver's own need.
static inline unsigned long CalcWorkMaxSize(const CComDrv *pDrv)
{
    unsigned long nBase = pDrv->CalcWorkBaseMaxSize();
    unsigned long nSize = 0x3C;
    if (nBase > nSize) {
        nSize = nBase;
    }
    return nSize;
}

CComDrvBasketGame::CComDrvBasketGame() : CComDrvBasket(CalcWorkMaxSize(this))
{
    m_pMarkChr = 0;
    m_nUnk174 = -1;
    m_nFlag17C = 0;
    m_nUnk180 = 0;
    m_fUnk18C = 0.0f;
    m_fUnk194 = 0.0f;
    m_fUnk198 = 0.0f;
    m_fUnk1A8 = 0.0f;
    m_fUnk1AC = 0.0f;
    m_fUnk1BC = 0.0f;
    m_pScript = 0;
    m_nUnk680 = 0;
    ClearAll();
}

CComDrvBasketGame::~CComDrvBasketGame()
{
    DeleteScript();
}

void CComDrvBasketGame::Reset()
{
    long nRoot = GetRootActType();
    CComDrv::Reset();
    m_fUnk190 = 0.0f;
    m_fUnk194 = 0.0f;
    m_fUnk1A8 = 0.0f;
    m_fUnk1AC = 0.0f;
    m_fUnk1B0 = 0.0f;
    m_fUnk1A0 = 0.0f;
    m_nUnk180 = 0;
    if (CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
        m_pMarkChr = 0;
    }
    m_nUnk1EC = 0;
    m_fUnk1F0 = 0.0f;
    m_fUnk1F4 = 0.0f;
    m_fUnk1F8 = 0.0f;
    m_fUnk1FC = 0.0f;
    unsigned long nFlag = m_nFlag17C;
    m_nFlag17C = nFlag & ~0x7000;
    if (nFlag & 0x800) {
        ResetScript();
    }
}

void CComDrvBasketGame::ClearAll()
{
    CComDrv::ClearAll();
    m_pMarkChr = 0;
    m_fUnk18C = 0.0f;
    m_fUnk190 = 0.0f;
    m_nFlag17C = 0;
    m_nUnk188 = 0;
    m_nUnk184 = 0;
    m_fUnk1BC = 0.0f;
    m_fUnk1B0 = 0.0f;
    m_fUnk1B4 = 0.0f;
    m_fUnk194 = 0.0f;
    m_fUnk19C = 0.0f;
    m_nUnk1E4 = 0;
    m_fUnk1A4 = 0.0f;
    m_fUnk1C0 = 0.0f;
    m_fUnk1C8 = -1.0f;
    m_fUnk1C4 = 0.0f;
    m_fUnk1CC = 0.0f;
    m_fUnk1D8 = 0.0f;
    m_fUnk1DC = 0.0f;
    m_fUnk1D4 = 0.0f;
    DeleteScript();
}

void CComDrvBasketGame::UpdateInfo()
{
    CComDrv::UpdateInfo();
    m_fComBaseLv = static_cast<CComManBasketGame *>(CComMan::GetInstance())->GetBaseComLvF_(m_pChr->GetTeamNo());
    m_fComLv = static_cast<CComManBasketGame *>(CComMan::GetInstance())->GetComLvF_(m_pChr->GetTeamNo());
}

void CComDrvBasketGame::NormalizeCanMovePos(bcs::FVEC3 &rPos) const
{
    CComManBasketGame::NormalizeCanMovePosToInvisibleWall(rPos);
    CComDrv::NormalizeCanMovePos(rPos);
}

// Where the driver stands when nothing else is to be done.
void CComDrvBasketGame::GetHomePos(bcs::FVEC3 &rPos)
{
    if (CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
        GetOffenseAPos(rPos);
    } else {
        GetDefenseAPos(rPos);
    }
}

// Whether a team mate driven by a pad is better placed to mark the
// character the driver marks.
int CComDrvBasketGame::CheckChangeMarkCore() const
{
    static const long nNum = 3;
    if (m_pMarkChr == 0) {
        return 0;
    }
    long nTeam = m_nMyTeam;
    if (CComManBasket::IsAllComTeam(nTeam) == 0 && IsMarkChangeable()) {
        CChrCore *apChr[nNum];
        long nChr = CComManBasketGame::GetMemBer(nTeam, apChr, nNum, 1, 0);
        if (nChr != 0) {
            for (long i = 0; i < nChr; i++) {
                if (ChangeMarkToPlayer(m_pMarkChr, apChr[i])) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Whether the ball holder broke past the driver or the driver is behind
// the holder: then the defence goes on without stopping.
int CComDrvBasketGame::IsNoStopDefense() const
{
    if (GetComManBK()->IsDefBreakthrough_(m_pChr)) {
        return 1;
    }
    return GetComManBK()->IsThanOnBallerBack_(m_pChr) != 0;
}

// Whether the driver may start its own defence action against the
// character it marks.
int CComDrvBasketGame::IsActCallDefenseExt()
{
    bcs::FVEC3 pos;
    GetHomePos(pos);
    if (m_pMarkChr == 0 || (m_pMarkChr != 0 && !m_pMarkChr->IsHoldBall()) || IsNoStopDefense()) {
        return 0;
    }
    return !CheckChangeMarkCore();
}

// Whether the driver stands in the three-point area of its goal.
int CComDrvBasketGame::In3PointArea() const
{
    return CComManBasket::Is3PointArea(m_nUnk8C, m_MyPos);
}

float CComDrvBasketGame::GetAttainmentMoveMinDist() const
{
    return CComMan::GetDatF(0x503, GetComLvF());
}

} // namespace msw

namespace unit_A3AF0_cpp {
// uncalled stand-in: the original creates the constant 4 here, in a
// function its link stripped (its body is not known); later functions
// share the constant. Nothing calls this; the module link strips it.
float UnusedConst4()
{
    return 4.0f;
}
}

namespace msw {

// (guessed helper) Which third of the court rPos is in along z, counted
// from the goal attacked: 0 near it, 2 far from it.
long CComDrvBasketGame::GetCourtAreaZ(const bcs::FVEC3 &rPos) const
{
    float fDepth = static_cast<const CChrCoreBasket *>(m_pChr)->GetCourtDepth();
    float fUnit = fDepth / 3.0f;
    long nArea = 1;
    float fDist = fabsf(rPos.z - m_TargetGoalPos.z);
    if (fDist < fUnit) {
        nArea = 0;
    } else if (fDist >= fDepth - fUnit) {
        nArea = 2;
    }
    return nArea;
}

// A random position around the three-point line of the goal attacked,
// fMin to fMax from the goal and within nDeg degrees of the driver's
// direction from it (on the court side).
void CComDrvBasketGame::Get3PointAreaRandPos(bcs::FVEC3 &rPos, float fMin, float fMax, long nDeg)
{
    float fGoalRot = CGame::GetGoalYRot(m_pChr->GetTargetGoalIdx());
    float fRot = m_pChr->GetRotY_FromGoal();
    float fRange = bcs::DEG_TO_RAD(nDeg);
    float fRot0 = RoundRad(fRot - fRange - fGoalRot);
    float fRot1 = RoundRad(fRot + fRange - fGoalRot);
    if (fRot0 > PI) {
        fRot0 -= PI2;
    }
    if (fRot1 > PI) {
        fRot1 -= PI2;
    }
    fRot0 = fRot0 > PI / 2 ? PI / 2 : (fRot0 < -PI / 2 ? -PI / 2 : fRot0);
    fRot1 = fRot1 > PI / 2 ? PI / 2 : (fRot1 < -PI / 2 ? -PI / 2 : fRot1);
    if (fRot0 > fRot1) {
        fRot0 = fRot1;
    }
    float fAngle = RoundRad(RandRangeF(fRot0, fRot1) + fGoalRot);
    float fDist = CComManBasket::GetRandValueF(fMin, fMax);
    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, fAngle);
    rPos = m_TargetGoalPos;
    rPos.y = 0.0f;
    rPos.x += dir.x * fDist;
    rPos.z += dir.z * fDist;
}

// Whether the character is in a combination play.
int CComDrvBasketGame::IsCombination() const
{
    return (static_cast<const CChrCoreBasket *>(m_pChr)->m_nBasketFlag >> 11) & 1;
}


// (guessed helper) Whether the two positions are on the same side of the
// court's centre line.
static inline int IsSameSideZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1)
{
    if ((rPos0.z >= 0.0f && rPos1.z > 0.0f) || (rPos0.z < 0.0f && rPos1.z < 0.0f)) {
        return 1;
    }
    return 0;
}

// The position the driver takes on offence when it does not hold the
// ball: one of three spots by where the ball holder stands, shared with
// the team mates so that the spots are farthest from the others.
void CComDrvBasketGame::GetOffenseAPos(bcs::FVEC3 &rPos)
{
    // static-order swap: declared in the order the original emits them.
    static const float fDist = 2.5f;
    static const long nTargetNum = 3;
    static const float fMFOffsetX = 5.0f;
    static const float fFWOffsetX = 5.0f;

    if (m_pChr->IsHoldBall()) {
        rPos = m_MyPos;
        CChrCore *pNear = CComManBasket::GetNearChrCore(rPos, m_nMyTeam, -1, m_pChr->GetPlayerNo(), 1);
        if (pNear != 0) {
            rPos = CComMan::GetChrPos(pNear);
        }
        return;
    }

    rPos = m_MyPos;
    const CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
    if (pHolder == 0) {
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (pBall != 0 && pBall->IsPassMode()) {
            long nTarget = static_cast<CBallBasket *>(pBall)->GetPassTargetNo();
            if (nTarget >= 0 && nTarget < CChrMan::GetChrCoreNum()) {
                pHolder = CChrMan::GetChrCore(nTarget);
            }
        }
    }
    if (pHolder == 0 || pHolder->GetTeamNo() != m_nMyTeam) {
        return;
    }

    float fZ0 = m_TargetGoalPos.z > 0.0f ? 10.0f : -10.0f;
    float fZ1 = m_TargetGoalPos.z > 0.0f ? 5.0f : -5.0f;
    bcs::FVEC3 aTbl[6][3] = {
        {bcs::FVEC3(-fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
        {bcs::FVEC3(fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(-fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
        {bcs::FVEC3(fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(-fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
        {bcs::FVEC3(-fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
        {bcs::FVEC3(-fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
        {bcs::FVEC3(fFWOffsetX, 0.0f, fZ0), bcs::FVEC3(-fFWOffsetX, 0.0f, fZ1), bcs::FVEC3(0.0f, 0.0f, 0.0f)},
    };

    const bcs::FVEC3 &rHolderPos = pHolder->GetPos();
    long nArea = GetCourtAreaZ(rHolderPos);
    long nIdx = 2.0f * nArea;
    bcs::FVEC3 holderPos = CComMan::GetChrPos(pHolder);
    if (holderPos.x > 0.0f) {
        nIdx++;
    }
    bcs::FVEC3 aPos[3];
    aPos[0] = aTbl[nIdx][0];
    aPos[1] = aTbl[nIdx][1];
    aPos[2] = aTbl[nIdx][2];

    {
        bcs::FVEC3 chrPos = CComMan::GetChrPos(pHolder);
        bcs::FVEC3 goal = CComMan::World2Local(pHolder->GetTargetGoalPos(), -1);
        if (!IsSameSideZ(chrPos, goal)) {
            float fZ = pHolder->GetPos().z;
            aPos[0].z += fZ;
            aPos[1].z += fZ;
            aPos[2].z += fZ;
        } else {
            float fOfs = 0.0f;
            float fRate = fabsf(rHolderPos.x / fMFOffsetX);
            fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
            switch (nArea) {
            case 0:
                if (fRate * (rHolderPos.x < 0.0f)) {
                    fOfs = -fDist;
                } else {
                    fOfs = fDist;
                }
                break;
            case 1:
                if (fRate * (rHolderPos.x > 0.0f)) {
                    fOfs = -fDist;
                } else {
                    fOfs = fDist;
                }
                break;
            }
            aPos[2].x += fOfs;
        }
    }

    const CChrCore *apMember[6];
    long nMember = CComManBasketGame::GetMemBer(m_nMyTeam, apMember, 6, -1, 0);
    long anScore[2];
    float afDist[2];
    long anArea[2];
    const CChrCore *apChr[2];
    bcs::FVEC3 aTarget[2][2];
    for (long i = 0; i < 2; i++) {
        anScore[i] = 0;
        afDist[i] = 0.0f;
        anArea[i] = -1;
        apChr[i] = 0;
        aTarget[i][0].Set(0.0f, 0.0f, 0.0f);
        aTarget[i][1].Set(0.0f, 0.0f, 0.0f);
    }
    long nChr = 0;
    for (long i = 0; i < nMember; i++) {
        if (apMember[i] == pHolder) {
            continue;
        }
        apChr[nChr] = apMember[i];
        bcs::FVEC3 pos = CComMan::GetChrPos(apMember[i]);
        anArea[nChr] = GetCourtAreaZ(pos);
        nChr++;
    }

    {
        long n0 = (nArea + 1) % nTargetNum;
        long n1 = nTargetNum - (n0 + nArea);
        aTarget[0][0] = aPos[n0];
        aTarget[0][1] = aPos[n1];
        aTarget[1][0] = aPos[n1];
        aTarget[1][1] = aPos[n0];
    }
    for (long i = 0; i < 2; i++) {
        for (long j = 0; j < 2; j++) {
            bcs::FVEC3 pos = CComMan::GetChrPos(apChr[j]);
            afDist[i] += DistXZ(pos, aTarget[i][j]);
        }
    }
    if (afDist[0] < afDist[1]) {
        anScore[0] = 50;
    } else {
        anScore[1] = 50;
    }

    {
        bcs::FVEC3 chrPos = CComMan::GetChrPos(pHolder);
        bcs::FVEC3 goal = CComMan::World2Local(pHolder->GetTargetGoalPos(), -1);
        if (IsSameSideZ(chrPos, goal)) {
            for (long i = 0; i < 2; i++) {
                for (long j = 0; j < 2; j++) {
                    long nChrArea = GetCourtAreaZ(CComMan::GetChrPos(apChr[j]));
                    long nPosArea = GetCourtAreaZ(aTarget[i][j]);
                    switch (nArea) {
                    case 2:
                        if (nChrArea == 2 && nPosArea == 1) {
                            anScore[i] += 100;
                        }
                        break;
                    case 1:
                        if (nChrArea == 0 && nPosArea == 0) {
                            anScore[i] += 100;
                        }
                        break;
                    }
                }
            }
        }
    }

    long nBest = anScore[1] >= anScore[0];
    for (long i = 0; i < 2; i++) {
        if (m_pChr == apChr[i]) {
            rPos = aTarget[nBest][i];
            break;
        }
    }
    if (nMember <= 2) {
        if (pHolder->GetPos().x > 0.0f) {
            rPos.x = -fabsf(rPos.x);
        } else {
            rPos.x = fabsf(rPos.x);
        }
    }
}

// The position the driver takes on defence: the character it marks is
// picked by trying every pairing of the two teams, then the driver stands
// between that character and the goal it guards.
void CComDrvBasketGame::GetDefenseAPos(bcs::FVEC3 &rPos)
{
    rPos = m_MyPos;
    const float &fLv = GetComLvF();
    const CChrCore *apMember[6];
    long nMember = CComManBasketGame::GetMemBer(m_nMyTeam, apMember, 6, -1, 0);
    const CChrCore *pMark = 0;
    if (nMember == 1) {
        pMark = CChrMan::GetChrCore(1 - m_pChr->GetPlayerNo());
    } else {
        // Every way of pairing players 0, 2, 4 with 1, 3, 5.
        unsigned char aPair[6][6] = {
            {0, 1, 2, 3, 4, 5}, {0, 3, 2, 1, 4, 5}, {0, 5, 2, 3, 4, 1},
            {0, 1, 2, 5, 4, 3}, {0, 3, 2, 5, 4, 1}, {0, 5, 2, 1, 4, 3},
        };
        CChrCore *pNear = CChrMan::GetChrCore(CComMan::GetNearChr(m_MyPos, 1 - m_nMyTeam, -1, -1, 1));
        SortBase::SORT aSort[32];
        long nNum = 0;
        float fBlockDist = CComMan::GetDatF(0x938, fLv);
        SortBase::SORT *pSort = aSort;
        for (long i = 0; i < nMember * (nMember - 1); i++) {
            float fScore = 0.0f;
            for (long j = 0; j < nMember; j++) {
                const CChrCore *pChr0 = CChrMan::GetChrCore(aPair[i][j * 2]);
                const CChrCore *pChr1 = CChrMan::GetChrCore(aPair[i][j * 2 + 1]);
                float fDist = CComMan::GetDisXZ(CComMan::GetChrPos(pChr0), CComMan::GetChrPos(pChr1));
                const CChrCore *pHolder = 0;
                const CChrCore *pOther = 0;
                bcs::FVEC3 otherPos;
                if (pChr0->IsHoldBall()) {
                    pHolder = pChr0;
                    otherPos = pChr1->GetPos();
                    pOther = pChr1;
                } else if (pChr1->IsHoldBall()) {
                    pHolder = pChr1;
                    otherPos = pChr0->GetPos();
                    pOther = pChr0;
                }
                if (pHolder != 0) {
                    float fHolderDist = CComMan::GetDisXZ(pHolder->GetPos(), pHolder->GetTargetGoalPos());
                    if (fHolderDist + fBlockDist < CComMan::GetDisXZ(otherPos, pHolder->GetTargetGoalPos()) &&
                        !pOther->IsNowBlockWork()) {
                        fDist += 999.9f;
                    }
                }
                if ((m_nMyTeam == 0 && pChr0 == m_pChr && pChr1 == pNear) ||
                    (m_nMyTeam == 1 && pChr1 == m_pChr && pChr0 == pNear)) {
                    for (long k = 0; k < nMember; k++) {
                        if (ChangeMarkToPlayer(pNear, apMember[k])) {
                            fDist += 9999.9f;
                            break;
                        }
                    }
                }
                fScore += fDist;
            }
            pSort->m_nId = nNum;
            nNum++;
            pSort->m_fParam = fScore;
            pSort++;
        }
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        const unsigned char *pBest = aPair[aSort[0].m_nId];
        for (long k = 0; k < 6; k++) {
            if (pBest[k] == m_pChr->GetPlayerNo()) {
                if (m_nMyTeam == 0) {
                    pMark = CChrMan::GetChrCore(pBest[k + 1]);
                } else {
                    pMark = CChrMan::GetChrCore(pBest[k - 1]);
                }
                break;
            }
        }
        if (!IsMarkChangeable() && m_pMarkChr != 0) {
            pMark = m_pMarkChr;

        }
        if (pMark != m_pMarkChr) {
            m_fUnk190 = CComMan::GetDatF(0x930, fLv);
        }
    }

    const bcs::FVEC3 &rGuard = m_Pos148;
    bcs::FVEC3 dir;
    dir = Diff(CComMan::GetChrPos(pMark), rGuard);
    m_pMarkChr = pMark;
    NormalizeXZ(dir, dir);
    float fRate;
    if (!pMark->IsHoldBall()) {
        fRate = 1.0f;
    } else {
        fRate = 0.9f;
    }
    float fLen = fRate * DistXZ(CComMan::GetChrPos(pMark), rGuard);
    rPos.x = rGuard.x + dir.x * fLen;
    rPos.y = 0.0f;
    rPos.z = rGuard.z + dir.z * fLen;

    long nCount;
    if (pMark->IsHoldBall()) {
        const CChrCoreBasket *pMy = static_cast<const CChrCoreBasket *>(m_pChr);
        if (pMy->m_nBasketFlag & 0x400) {
            float fMarkRot = pMy->GetRotY_ToPosXZ(pMark->GetPos());
            bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
            float fDiff = RoundRadPI(pMy->GetRotY_ToPosXZ(world) - fMarkRot);
            bcs::FVEC3 ofs;
            ofs = Diff(rPos, pMark->GetPos());
            NormalizeXZ(ofs, ofs);
            float fTurn = 0.34906584f;
            bcs::BCS_Vec_RotY(ofs, ofs, fDiff < 0.0f ? -fTurn : fTurn);
            ofs.x *= 3.0f;
            ofs.z *= 3.0f;
            bcs::FVEC3Sum(rPos, ofs, pMark->GetPos());
            return;
        }
        nCount = 0;
    } else {
        nCount = 0;
        float fMarkDist = CComManBasket::GetDistXZ(CComMan::GetChrPos(pMark), rGuard);
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() != m_nMyTeam && pChr != pMark &&
                !pChr->IsHoldBall() && CComManBasket::GetDistXZ(CComMan::GetChrPos(pChr), rGuard) < fMarkDist) {
                nCount++;
            }
        }
        if (CChrMan::Util_SearchHoldBallChr(0) != 0 && (unsigned long)nCount++ > 1) {
            return;
        }
    }
    if (nCount < 0 || nCount >= nMember) {
        return;
    }

    float fDist = CComManBasket::GetDistXZ(rPos, rGuard);
    long anRate[3];
    anRate[0] = CComMan::GetDatI(0x8E0, fLv);
    anRate[1] = CComMan::GetDatI(0x8E8, fLv);
    anRate[2] = CComMan::GetDatI(0x8F0, fLv);
    bcs::FVEC3 d = Diff(rPos, rGuard);
    dir.Set(d.x, 0.0f, d.z);
    float fFar = CComMan::GetDatF(0x8D8, fLv);
    float fNear = CComMan::GetDatF(0x8D0, fLv);
    float fT = (fDist - fNear) / (fFar - fNear);
    fT = fT > 1.0f ? 1.0f : (fT < 0.0f ? 0.0f : fT);
    float fScale = (10.0f - fT * (10.0f - anRate[nCount])) / 10.0f;
    fLen = fScale * LenXZ(dir);
    NormalizeXZ(dir, dir);
    rPos.x = rGuard.x + fLen * dir.x;
    rPos.y = 0.0f;
    rPos.z = rGuard.z + fLen * dir.z;
}

// The radius the driver searches "?" panels in.
float CComDrvBasketGame::GetSearchHatenaPanelArea() const
{
    float fArea = CComMan::GetDatF(0xA30, (long)m_pChr->GetChrTypeID());
    return fArea * CComMan::GetDatF(0xA45, GetComLvF());
}

// The rate of the "?" panels of the stage (lower on the haunted stage
// while the chandelier is off).
float CComDrvBasketGame::GetHatenaPanelStageRate_() const
{
    if (CGame::g_GameWork.GetStage() == 6 && !GetGameBK()->IsChandelierOn_()) {
        return CComMan::GetDatF(0x1033, GetComLvF());
    }
    return 1.0f;
}

// The shot score the stage gimmick adds.
long CComDrvBasketGame::GetBaseShootPointToGimmick() const
{
    const float &fLv = GetComLvF();
    switch (CGame::g_GameWork.GetStage()) {
    case 5:
        switch (GetDCruise3PointAreaState()) {
        case -1:
            return CComMan::GetDatF(0xF6B, fLv);
        case 1:
            return CComMan::GetDatF(0xF63, fLv);
        }
        break;
    case 7: {
        CNpcBossPackun *pBoss = CChrManBasket::GetInstance()->GetBossPackun_(m_nUnk8C);
        if (pBoss != 0) {
            if (pBoss->FsmqGetState() == 0) {
                return CComMan::GetDatI(0xE87, fLv);
            }
            if (pBoss->IsMouthOpen()) {
                return CComMan::GetDatI(0xE7F, fLv);
            }
            return CComMan::GetDatI(0xE77, fLv);
        }
        break;
    }
    case 6:
        if (!GetGameBK()->IsChandelierOn_()) {
            return CComMan::GetDatI(0xFC3, fLv);
        }
        break;
    }
    return 0;
}

// The ghost of the haunted stage caught a shot.
void CComDrvBasketGame::MsgShootCatchToGhost()
{
    if (CGame::g_GameWork.GetStage() == 6) {
        Reset();
    }
}

// Whether the driver's team has at least as many coins as the other.
int CComDrvBasketGame::IsScoureWin() const
{
    long nTeam = m_nMyTeam;
    long nCoin = CComManBasket::GetTeamCoin(nTeam);
    return nCoin >= CComManBasket::GetTeamCoin(nTeam == 0);
}

int CComDrvBasketGame::IsAllComTeam() const
{
    return CComManBasket::IsAllComTeam(m_nMyTeam);
}

long CComDrvBasketGame::GetDCruise3PointAreaState() const
{
    return CComManBasket::GetDistOnGoalTo3PointAreaCenter(m_nUnk8C) > 1.0f ? 1 : -1;
}

// The score for passing the ball to the driver: higher nearer the goal,
// with no one of the other team about (ppChr, nNum of them) or in the
// way, and for some stage gimmicks.
long CComDrvBasketGame::CalcMyHoldBallScore(long nNum, const CChrCore **ppChr) const
{
    const CChrCore *pMy = m_pChr;
    const float &fLv = GetComLvF();
    const bcs::FVEC3 &rGoal = pMy->GetTargetGoalPos();
    long nScore = 0;
    if (pMy->IsHoldBall()) {
        long nMin = CComMan::GetDatI(0x20, fLv);
        long nMax = CComMan::GetDatI(0x28, fLv);
        nScore = RandRangeI(nMin, nMax);
    }
    if (pMy->GetState() == 6) {
        nScore += CComMan::GetDatI(0x30, fLv);
    }

    float fMaxDist;
    long nBase;
    if (CGame::g_GameWork.GetStage() == 7) {
        fMaxDist = CComMan::GetDatF(0xE37, fLv);
        nBase = CComMan::GetDatF(0xE2F, fLv);
    } else {
        fMaxDist = CComMan::GetDatF(0x8, fLv);
        nBase = CComMan::GetDatF(0x0, fLv);
    }
    float fRate = CComManBasket::GetDistXZ(pMy->GetPos(), rGoal) / fMaxDist;
    fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
    if (CGame::g_GameWork.GetStage() == 10) {
        if (CComManBasketGame::IsJStreetBonus()) {
            nBase += CComMan::GetDatF(0xF7B, fLv);
        }
        if (pMy->IsHoldBall()) {
            CStage10Base::CPanelInfoBase *pPanel = CComManBasketGame::GetJStreetPanelRide();
            if (pPanel != 0) {
                switch (pPanel->GetPanelBonusType()) {
                case 1:
                    nBase += CComMan::GetDatF(pPanel->GetPanelBonusRate() > 0 ? 0xF83 : 0xF93, fLv);
                    break;
                case 2:
                    nBase += CComMan::GetDatF(0xF8B, fLv);
                    break;
                }
            }
        }
    }
    nScore += (1.0f - fRate) * nBase;

    for (long i = 0; i < nNum; i++) {
        const CChrCore *pChr = ppChr[i];
        if (CChrManBasket::IsBadConditon(pChr)) {
            continue;
        }
        float fDist = CComManBasket::GetDistXZ(pMy->GetPos(), pChr->GetPos());
        float fNear = fDist / CComMan::GetDatF(0x18, fLv);
        if (fNear > 1.0f) {
            fNear = 1.0f;
        } else if (fNear < 0.0f) {
            fNear = 0.0f;
        }
        nScore += (1.0f - fNear) * CComMan::GetDatF(0x10, fLv);
    }

    if (m_pChr->IsHaveStockItem()) {
        nScore += CComMan::GetDatI(0x70, fLv);
    }
    if (JudgeShootCourseFree(GetEmTeamEnableBit())) {
        nScore += CComMan::GetDatI(0x60, fLv);
    }

    // The pad-driven players of the other team stand in the way of a shot
    // of an all-computer team.
    int bBlocked;
    if (!CComManBasket::IsAllComTeam(m_nMyTeam)) {
        bBlocked = 0;
    } else if (!m_pChr->IsHoldBall()) {
        bBlocked = 0;
    } else {
        const CChrCore *apPad[6];
        long nPad = CComManBasketGame::GetMemBer(1 - m_pChr->GetTeamNo(), apPad, 6, 1, m_pChr);
        unsigned long nBits = 0;
        for (long i = 0; i < nPad; i++) {
            nBits |= 1 << apPad[i]->GetPlayerNo();
        }
        bBlocked = !JudgeShootCourseFree(nBits);
    }
    if (bBlocked) {
        nScore += CComMan::GetDatF(0x68, fLv);
    }

    if (!m_pChr->IsHoldBall()) {
        CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
        if (pHolder != 0 && pHolder->GetTeamNo() == m_nMyTeam) {
            float fWidth = CComMan::GetDatF(0x40, fLv) * 0.5f;
            float fLen = CComMan::GetDatF(0x48, fLv);
            if (IsExistChrToTarget(CComMan::GetChrPos(pHolder), fWidth, fLen, GetEmTeamEnableBit())) {
                nScore += CComMan::GetDatI(0x38, fLv);
            }
        }
    }
    if (m_pChr->GetStatusFlag() & 0x20000) {
        nScore += CComMan::GetDatI(0x50, fLv);
    }
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() != m_nMyTeam && (pChr->GetStatusFlag() & 0x20000)) {
            nScore += CComMan::GetDatI(0x58, fLv);
        }
    }
    return nScore;
}

// Whether the pad-driven pPlayer is near enough to pMark (and, when pMark
// holds the ball, in front of it towards its goal) to mark it instead of
// the driver.
int CComDrvBasketGame::ChangeMarkToPlayer(const CChrCore *pMark, const CChrCore *pPlayer) const
{
    if (GetComDrv(pPlayer)) {
        return 0;
    }
    if (pPlayer->IsMoveAssist()) {
        return 0;
    }
    const float &fLv = GetComLvF();
    long nId = pMark->IsHoldBall() ? 0x918 : 0x928;
    float fDist = CComMan::GetDatF(nId, fLv);
    if (CComManBasket::GetDistXZ(pMark, pPlayer) < fDist) {
        if (!pMark->IsHoldBall()) {
            return 1;
        }
        float fGoalRot = pMark->GetRotY_ToGoal();
        float fRot = pMark->GetRotY_ToPosXZ(pPlayer->GetPos());
        fRot = RoundRadPI(fRot - fGoalRot);
        long nDeg = CComMan::GetDatI(0x920, fLv);
        if (fabsf(fRot) < bcs::DEG_TO_RAD(nDeg)) {
            return 1;
        }
    }
    return 0;

}

// The direction to jump in for a jump shot (-1: straight up): away from
// the nearest character of the other team in the way to the goal, or a
// random one.
float CComDrvBasketGame::GetJumpShootJumpDir() const
{
    const float &fLv = GetComLvF();
    float fWidth = CComMan::GetDatI(0xB2F, fLv) * 0.5f;
    float fLen = CComMan::GetDatI(0xB37, fLv);
    bcs::FVEC3 from = m_MyPos;
    bcs::FVEC3 to = m_TargetGoalPos;
    const CChrCore *pNear = 0;
    float fNearDist = 9999.9f;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr && pChr->GetTeamNo() != m_nMyTeam) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            if (CComMan::IsPointInFaceXZ(pos, from, to, fWidth, fLen)) {
                float fDist = CComManBasket::GetDistXZ(pChr, m_pChr);
                if (!(fDist > fNearDist)) {
                    pNear = pChr;
                    fNearDist = fDist;
                }
            }
        }
    }

    CChrCore *pMyChr = m_pChr;
    float fGoalRot = pMyChr->GetRotY_ToPosXZ(pMyChr->GetTargetGoalPos());
    if (pNear != 0) {
        if (LotRate() < CComMan::GetDatI(0xB4F, fLv)) {
            float fRot = m_pChr->GetRotY_ToPosXZ(pNear->GetPos());
            float fDiff = RoundRadPI(fRot - fGoalRot);
            float fOfs = bcs::DEG_TO_RAD(RandRangeI(90, 180));
            if (fDiff > 0.0f) {
                fOfs = -fOfs;
            }
            return RoundRad(fRot + fOfs);
        }
        return -1.0f;
    }
    if (LotRate() < CComMan::GetDatI(0xB3F, fLv)) {
        long nDeg = CComMan::GetDatI(0xB47, fLv);
        return RoundRad(fGoalRot + bcs::DEG_TO_RAD(RandRangeI(-nDeg, nDeg)));
    }
    return -1.0f;
}

// The frame rate of a dunk by computer level (1 at every level).
float CComDrvBasketGame::GetDunkFrameRate() const
{
    if (CComManBasket::IsAllComTeam(m_nMyTeam)) {
        switch ((long)(0.5f + GetComLvF())) {
        case 0:
            return 1.0f;
        case 1:
            return 1.0f;
        case 2:
            return 1.0f;
        case 3:
            return 1.0f;
        case 4:
            return 1.0f;
        case 5:
            return 1.0f;
        case 6:
            return 1.0f;
        case 7:
            return 1.0f;
        }
        return 1.0f;
    }
    return 1.0f;
}

} // namespace msw

namespace unit_A3AF0_cpp {
// uncalled stand-in: as UnusedConst4, for the constant -4 the original
// creates here.
float UnusedConstMinus4()
{
    return -4.0f;
}
}

namespace msw {

// (guessed helper; the original has it inlined in JudgeItemThrow1, with
// its own null check)
// Whether pChr is a character of the other team an item thrown from the
// driver reaches unhindered.
int CComDrvBasketGame::IsItemThrowTarget(const CChrCore *pChr) const
{
    if (pChr == 0) {
        return 0;
    }
    if (pChr->IsDisable()) {
        return 0;
    }
    if (pChr->GetTeamNo() == m_nMyTeam) {
        return 0;
    }
    if (pChr->GetComLevelWork() != 0) {
        return 0;
    }
    float fLen = DistXZ(pChr->GetPos(), m_pChr->GetPos());
    return !IsExistChrToTarget(CComMan::GetChrPos(pChr), 1.1f, fLen, GetEmTeamEnableBit());
}

// (guessed helper) Whether pChr stands in the course from rFrom to rTo,
// fWidth wide.
static inline int IsChrInCourse(const CChrCore *pChr, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fWidth)
{
    float fLen = CComManBasket::GetDistXZ(rFrom, rTo);
    return CComMan::IsPointInFaceXZ(CComMan::GetChrPos(pChr), rFrom, rTo, fWidth, fLen);
}

// The side (2 or 4; -1 for neither) to curve an item shot to: the shot
// goes by a point fCurve beside the middle of the way to the goal, and a
// side is free when no one of the other team stands in the two legs
// (fRange0 and fRange1 wide). With both free, the side with fewer of them.
long CComDrvBasketGame::CheckItemShootDir(float fCurve, float fRange0, float fRange1) const
{
    static const long nArrayNum = 3;
    const CChrCore *apEm[nArrayNum];
    long nEm = CComManBasketGame::GetMemBer(1 - m_nMyTeam, apEm, nArrayNum, -1, 0);
    const bcs::FVEC3 &rMy = m_MyPos;
    const bcs::FVEC3 &rGoal = m_TargetGoalPos;
    bcs::FVEC3 mid;
    mid = Diff(rGoal, rMy);
    float fHalf = PSVECMag((const Vec *)&mid) * 0.5f;
    NormalizeXZ(mid, mid);
    bcs::FVEC3 side;
    bcs::BCS_Vec_RotY(side, mid, PI / 2);
    mid.x = mid.x * fHalf + rMy.x;
    side.x *= fCurve;
    mid.z = mid.z * fHalf + rMy.z;
    side.z *= fCurve;
    bcs::FVEC3 pos0 = Diff(mid, side);
    bcs::FVEC3 pos1 = Sum(mid, side);

    float fLeft, fRight, fFront, fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);
    int bFree0 = 1;
    if (pos0.x > fLeft && pos0.x < fRight && pos0.z > fFront && pos0.z < fBack) {
        for (long i = 0; i < nEm; i++) {
            if (IsChrInCourse(apEm[i], rMy, pos0, fRange0)) {
                bFree0 = 0;
                break;
            }
            if (IsChrInCourse(apEm[i], pos0, rGoal, fRange1)) {
                bFree0 = 0;
                break;
            }
        }
    } else {
        return 2;
    }
    int bFree1 = 1;
    if (pos1.x > fLeft && pos1.x < fRight && pos1.z > fFront && pos1.z < fBack) {
        for (long i = 0; i < nEm; i++) {
            if (IsChrInCourse(apEm[i], rMy, pos1, fRange0)) {
                bFree1 = 0;
                break;
            }
            if (IsChrInCourse(apEm[i], pos1, rGoal, fRange1)) {
                bFree1 = 0;
                break;
            }
        }
    } else {
        return 4;
    }

    if (bFree1 && bFree0) {
        long nLeft = 0;
        float fGoalRot = m_pChr->GetRotY_ToGoal();
        for (long i = 0; i < nEm; i++) {
            float fRot = RoundRadPI(m_pChr->GetRotY_ToPosXZ(apEm[i]->GetPos()) - fGoalRot);
            if (!(fabsf(fRot) > PI / 2) && fRot > 0.0f) {
                nLeft++;
            }
        }
        if (nLeft > nEm - nLeft) {
            return 4;
        }
        if (nLeft < nEm - nLeft) {
            return 2;
        }
        return RandMaxI(1) ? 2 : 4;
    }
    if (bFree1) {
        return 2;
    }
    return bFree0 ? 4 : -1;
}

// The same for a curved shot by the nNum characters of ppChr, fW0 and fW1
// wide; 0 when there is no one to check.
long CComDrvBasketGame::CheckFreeShootCourseToCurve(long nNum, const CChrCore **ppChr, float fCurve, float fW0,
                                                    float fW1) const
{
    static const long TBL_MAX = 2;
    int bFree1 = 1;
    int bFree0 = 1;
    if (nNum > 0) {
        bcs::FVEC3 my = m_MyPos;
        bcs::FVEC3 goal = m_TargetGoalPos;
        bcs::FVEC3 d = Diff(goal, my);
        bcs::FVEC3 half(d.x * 0.5f, 0.0f, d.z * 0.5f);
        bcs::FVEC3 side;
        NormalizeXZ(side, half);
        bcs::BCS_Vec_RotY(side, side, PI / 2);
        side.x *= fCurve;
        side.z *= fCurve;
        // (one width per character, as the original reads them)
        float afWidth[TBL_MAX] = {fW0, fW1};

        {
            bcs::FVEC3 pos = Diff(Sum(my, half), side);
            bcs::FVEC3 aLine[TBL_MAX][2] = {{my, pos}, {pos, goal}};
            for (long i = 0; i < TBL_MAX; i++) {
                bcs::FVEC3 to = aLine[i][1];
                float fLen = DistXZ(to, aLine[i][0]);
                for (long j = 0; j < nNum; j++) {
                    if (CComManBasket::IsExistTarget(aLine[i][0], to, afWidth[j], fLen, ppChr[j])) {
                        bFree0 = 0;
                        break;
                    }
                }
            }
        }
        {
            bcs::FVEC3 pos = Sum(Sum(my, half), side);
            bcs::FVEC3 aLine[TBL_MAX][2] = {{my, pos}, {pos, goal}};
            for (long i = 0; i < TBL_MAX; i++) {
                bcs::FVEC3 to = aLine[i][1];
                float fLen = DistXZ(to, aLine[i][0]);
                for (long j = 0; j < nNum; j++) {
                    if (CComManBasket::IsExistTarget(aLine[i][0], to, afWidth[j], fLen, ppChr[j])) {
                        bFree1 = 0;
                        break;
                    }
                }
            }
        }
    }

    if (!bFree1 && !bFree0) {
        return -1;
    }
    if (bFree1 && bFree0) {
        long nLeft = 0;
        float fGoalRot = m_pChr->GetRotY_ToGoal();
        for (long i = 0; i < nNum; i++) {
            float fRot = RoundRadPI(m_pChr->GetRotY_ToPosXZ(ppChr[i]->GetPos()) - fGoalRot);
            if (!(fabsf(fRot) > 2.1816616f) && fRot > 0.0f) {
                nLeft++;
            }
        }
        if (nLeft > nNum - nLeft) {
            return 4;
        }
        if (nLeft < nNum - nLeft) {
            return 2;
        }
        return RandMaxI(1) ? 2 : 4;
    }
    if (bFree1) {
        return 2;
    }
    if (bFree0) {
        return 4;
    }
    return 0;
}

// Whether the arts shot (Mario and Luigi curve theirs) has a free course
// to the goal: -1 when not, else the side to curve to (0 straight).
long CComDrvBasketGame::CheckArtsShootDir() const
{
    switch (GetChrTypeID()) {
    case 3:
        if (LotRate() >= CComMan::GetDatI(0x125B, GetComLvF())) {
            return -1;
        }
        break;
    case 4:
        if (LotRate() >= CComMan::GetDatI(0x12E3, GetComLvF())) {
            return -1;
        }
        break;
    }
    return MsgCheckArtsShootFree_();
}

// Whether rPos (local) is far enough to run to.
int CComDrvBasketGame::CheckMoveToTargetDist(bcs::FVEC3 pos) const
{
    float fMin = GetAttainmentMoveMinDist() + CComMan::GetDatF(0x50B, GetComLvF());
    return fMin <= DistXZ(pos, m_MyPos);
}

// The members of team nTeam with the score for passing to each, best
// first; returns how many there are.
long CComDrvBasketGame::CalcHoldBallScore(long nTeam, MEMBER_INFO &rInfo)
{
    long nNum = CComManBasketGame::GetMemBer(nTeam, rInfo, -1, 0);
    if (nNum >= 1) {
        static const long nEmCoreArrayNum = 3;
        const CChrCore *apEm[nEmCoreArrayNum];
        long nEm = CComManBasketGame::GetMemBer(1 - nTeam, apEm, nEmCoreArrayNum, -1, 0);
        for (long i = 0; i < nNum; i++) {
            CComDrvBasketGame *pDrv = CComManBasketGame::GetComDrvBK(rInfo.m_apChr[i]);
            if (pDrv != 0) {
                rInfo.m_aScore[i] += pDrv->CalcMyHoldBallScore(nEm, apEm);
            }
        }
        for (long i = 0; i < nNum - 1; i++) {
            for (long j = i + 1; j < nNum; j++) {
                if (rInfo.m_aScore[i] < rInfo.m_aScore[j]) {
                    const CChrCore *pChr = rInfo.m_apChr[i];
                    long nScore = rInfo.m_aScore[i];
                    rInfo.m_apChr[i] = rInfo.m_apChr[j];
                    rInfo.m_aScore[i] = rInfo.m_aScore[j];
                    rInfo.m_apChr[j] = pChr;
                    rInfo.m_aScore[j] = nScore;
                }
            }
        }
    }
    return nNum;
}

// The yaw the Black Mage's arts shot is steered to: of the three ways the
// shot can go from the character and its two clones, one no one of the
// other team stands in (-1 when there is none).
float CComDrvBasketGame::GetArtsInputToNinja() const
{
    // static-order swap: the statics of this function are declared here in
    // the order the original emits them.
    static const float fWidth = 1.0f;
    static const long EM_CORE_MAX = 3;
    static const float fDist1 = 3.5f;
    static const float fDist0 = 3.0f;
    static const long MAX = 3;
    const CChrCoreBasketNinja *pNinja = static_cast<const CChrCoreBasketNinja *>(m_pChr);
    const bcs::FVEC3 *apFrom[MAX];
    apFrom[0] = pNinja->GetClonePos(0);
    apFrom[1] = pNinja->GetClonePos(1);
    apFrom[2] = &pNinja->GetPos();

    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, pNinja->GetRotY_FromGoal());
    bcs::FVEC3 base(fDist0 * dir.x + pNinja->GetTargetGoalPos().x, 0.0f,
                    fDist0 * dir.z + pNinja->GetTargetGoalPos().z);
    bcs::BCS_VecZ_RotY(dir, 1.0f, PI / 2 + pNinja->GetRotY_FromGoal());
    dir.x *= fDist1;
    dir.y = 0.0f;
    dir.z *= fDist1;
    bcs::FVEC3 aTo[MAX] = {Sum(base, dir), Diff(base, dir), base};

    const CChrCore *apEm[EM_CORE_MAX];
    long nEm = CComManBasketGame::GetMemberNoBadCondition(1 - m_nMyTeam, apEm, EM_CORE_MAX, -1, 0);
    long anFree[MAX];
    long nFree = 0;
    for (long i = 0; i < MAX; i++) {
        if (apFrom[i] == 0) {
            continue;
        }
        bcs::FVEC3 from = CComMan::World2Local(*apFrom[i], -1);
        bcs::FVEC3 to = CComMan::World2Local(aTo[i], -1);
        float fLen = DistXZ(to, from);
        int bHit = 0;
        for (long j = 0; j < nEm; j++) {
            if (static_cast<CChrManBasket *>(CChrMan::GetInstance())
                    ->InRectAndRad_(from, to, CComMan::GetChrPos(apEm[j]), fLen, fWidth, PI / 2)) {
                bHit = 1;
                break;
            }
        }
        if (!bHit) {
            long anDir[MAX] = {4, 2, 1};
            anFree[nFree] = anDir[i];
            nFree++;
        }
    }
    if (nFree == 0) {
        return -1.0f;
    }

    float fOfs;
    switch (anFree[bcs::BCS_RandMax(nFree - 1)]) {
    case 2:
        fOfs = PI * 3 / 2;
        break;
    case 4:
        fOfs = PI / 2;
        break;
    default:
        fOfs = 0.0f;
        break;
    }
    if (fabsf(RoundRadPI(m_pChr->m_fCamRotY)) < PI / 2) {
        fOfs += PI;
    }
    return RoundRad(fOfs + m_pChr->m_fCamRotY);
}

// (names guessed) The work of the lay-up actions (ACT_LAY_UP,
// ACT_CANCEL_LAY_UP) and of the pass actions (ACT_JUMP_SHOOT,
// ACT_ITEM_SHOOT) as far as the draws below use it.
struct LAYUP_WORK : public CComDrv::ACT_WORK {
    long m_bChecked; // 0x1C, set once the cancel was drawn
    long m_nRate;    // 0x20, the rate of the cancel
};

struct SHOOT_WORK : public CComDrv::ACT_WORK {
    unsigned char m_Unk1C[0x20 - 0x1C];
    // 0x20: bit 0: the shot curves the other way; bit 1: the boss stage's
    // draw was made.
    unsigned long m_nFlag;
    long m_nUseItem;       // 0x24, what LotRateUseItem returns
    long m_bChecked;       // 0x28, set once the cancel can be drawn
    float m_fCurve;        // 0x2C, the curve of the shot
};

// (guessed helper) Changes the curve of the shot by fAdd; past zero the
// curve turns to the other side.
static inline void AddShootCurve(SHOOT_WORK *pWork, float fAdd)
{
    pWork->m_fCurve += fAdd;
    if (pWork->m_nFlag & 1) {
        if (pWork->m_fCurve < 0.0f) {
            pWork->m_fCurve = 1.0f - pWork->m_fCurve;
            pWork->m_nFlag &= ~1;
        }
    } else if (pWork->m_fCurve >= 0.0f) {
        pWork->m_fCurve = pWork->m_fCurve - 1.0f;
        pWork->m_nFlag |= 1;
    }
}

// The draw of a lay-up (-1 when no lay-up runs or it was not drawn yet).
long CComDrvBasketGame::LotRateLayUp()
{
    long nAct = GetCurrentActType();
    if (nAct >= ACT_LAY_UP && nAct <= ACT_CANCEL_LAY_UP) {
        LAYUP_WORK *pWork = static_cast<LAYUP_WORK *>(GetCurrentActWork());
        if (pWork->m_bChecked != 0) {
            return LotRate() < 50;
        }
    }
    return -1;
}

// Whether to break off a dunk for a lay-up: once a character of the other
// team that jumps to block comes near enough, the rate is drawn.
int CComDrvBasketGame::LotRateCancelLayUp()
{
    if (!m_pChr->IsHoldBall()) {
        return 0;
    }
    if (GetRootActType() != ACT_DUNK) {
        return 0;
    }
    if (m_pChr->GetState() != 0x28) {
        return 0;
    }
    LAYUP_WORK *pWork = static_cast<LAYUP_WORK *>(GetCurrentActWork());
    if (pWork->m_bChecked != 0) {
        return 0;
    }
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable() || pChr->GetTeamNo() == m_nMyTeam) {
            continue;
        }
        long nState = pChr->GetState();
        if (nState != 0x30 && nState != 0xC) {
            continue;
        }
        bcs::FVEC3 em;
        pChr->GetVNodePos(0, em);
        bcs::FVEC3 my;
        m_pChr->GetVNodePos(0, my);
        bcs::FVEC3 d = Diff(em, my);
        float fDist = PSVECMag((const Vec *)&d);
        if (fDist <= static_cast<CChrCoreBasket *>(pChr)->GetJumpCutColR() + CComMan::GetDatF(0x143, GetComLvF())) {
            pWork->m_bChecked = 1;
            if (CGame::g_GameWork.GetStage() == 7) {
                CNpcBossPackun *pBoss =
                    static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetBossPackun_(m_pChr->GetTargetGoalIdx());
                if (pBoss != 0) {
                    if (pBoss->FsmqGetState() == 0) {
                        pWork->m_nRate = CComMan::GetDatI(0xECF, GetComLvF());
                    } else if (pBoss->IsMouthOpen()) {
                        pWork->m_nRate = CComMan::GetDatI(0xEC7, GetComLvF());
                    } else {
                        pWork->m_nRate = CComMan::GetDatI(0xEBF, GetComLvF());
                    }
                    break;
                }
            }
            pWork->m_nRate = CComMan::GetDatI(0x14B, GetComLvF());
            break;
        }
    }
    if (pWork->m_bChecked) {
        return LotRate() < pWork->m_nRate;
    }
    return 0;
}

} // namespace msw

namespace unit_A3AF0_cpp {
// uncalled stand-in: the original has here a function its link stripped,
// msw::CComDrvBasketGame::CancelLayUpCmd_ (body not known), whose static
// fDist (2) stays in .rodata.
float UnusedCancelLayUpCmd()
{
    static const float fDist = 2.0f;
    return fDist;
}
}

namespace msw {

// Whether to break off a jump shot for a pass: once a character of the
// other team stands in the way, the rate is drawn; while the shot goes on
// its curve is changed at random.
int CComDrvBasketGame::LotRateCancelPass()
{
    if (!m_pChr->IsHoldBall()) {
        return 0;
    }
    if (m_pChr->GetState() != 0x22) {
        return 0;
    }
    const float &fLv = GetComLvF();
    SHOOT_WORK *pWork;
    switch (GetRootActType()) {
    case ACT_ITEM_SHOOT:
    case ACT_JUMP_SHOOT: {
        pWork = static_cast<SHOOT_WORK *>(GetCurrentActWork());
        if (pWork->m_bChecked != 0) {
            return 0;
        }
        float fWidth = CComMan::GetDatF(0x15B, fLv);
        float fLen = CComMan::GetDatF(0x163, fLv);
        long nDeg = CComMan::GetDatI(0x16B, fLv);
        if (!CComManBasket::IsExistChrToTarget(m_pChr, bcs::DEG_TO_RAD(nDeg), m_TargetGoalPos, fWidth, fLen,
                                                GetEmTeamEnableBit())) {
            return 0;
        }
        pWork->m_bChecked = 1;
        break;
    }
    default:
        return 0;
    }

    int bCancel = LotRate() < CComMan::GetDatI(0x153, fLv);
    if (!bCancel && pWork != 0) {
        if (LotRate() < CComMan::GetDatI(0x173, fLv)) {
            long nMin = CComMan::GetDatI(0x17B, fLv);
            long nMax = CComMan::GetDatI(0x183, fLv);
            AddShootCurve(pWork, CComManBasket::GetRandValueI(nMin, nMax) / 100.0f);

        }
    }
    return bCancel;
}

// Whether to turn a team mate's shot into an alley-oop dunk.
int CComDrvBasketGame::JudgeShootToAlleyoopDunk()
{
    long nState = m_pChr->GetState();
    if (nState == 0x28 || nState == 0x24 || nState == 0x26 || nState == 0x30) {
        return 0;
    }
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (!pBall->IsActive()) {
        return 0;
    }
    if (!pBall->IsShootMode()) {
        return 0;
    }
    if (pBall->IsItemBall()) {
        return 0;
    }
    if (pBall != 0 && (static_cast<CBallBasket *>(pBall)->GetBasketFlag() & 0x2000)) {
        return 0;
    }
    bcs::FVEC3 ballPos = CComMan::World2Local(pBall->GetPos(), -1);
    ballPos.y = 0.0f;
    if (CComManBasket::GetDistXZ(ballPos, m_TargetGoalPos) < 1.0f) {
        return 0;
    }
    if (LotRate() > CComMan::GetDatI(0x2A8, GetComLvF())) {
        return 0;
    }
    float fGoalRot = m_pChr->GetRotY_FromGoal();
    float fRot = RoundRadPI(CComManBasket::GetRotY_P1ToP2XZ(m_TargetGoalPos, pBall->GetPos()) - fGoalRot);
    fRot = fRot > PI / 2 ? PI / 2 : (fRot < -PI / 2 ? -PI / 2 : fRot);
    float fRate = 1.0f - fabsf(fRot) / (PI / 2);
    long nChr = m_pChr->GetChrTypeID();
    long nMax = 0;
    long nMin = 0;
    if (CGame::g_GameWork.GetStage() == 7) {
        CNpcBossPackun *pBoss = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetBossPackun_(m_nUnk8C);
        if (pBoss != 0) {
            // By whether the boss is asleep (state 0).
            long anMaxId[2] = {0xEFF, 0xF29};
            long anMinId[2] = {0xF14, 0xF3E};
            long nSleep = pBoss->FsmqGetState() == 0;
            nMax = CComMan::GetDatI(anMaxId[nSleep], nChr);
            nMin = CComMan::GetDatI(anMinId[nSleep], nChr);
        }
    } else {
        nMax = CComMan::GetDatI(0x2B0, nChr);
        nMin = CComMan::GetDatI(0x2C5, nChr);
    }
    long nRate = fRate * (nMax - nMin) + nMin;
    if (LotRate() > nRate) {
        return 0;
    }
    Reset();
    ActCallAlleyoop();
    return 1;
}

// Whether to go for a rebound (or a rebound dunk) of a shot about to miss:
// drawn every few frames, more likely for the team mate placed best.
int CComDrvBasketGame::JudgeReboundOrReboundDunk()
{
    const float &fLv = GetComLvF();
    m_fUnk1AC += m_pChr->m_fFrameStep;
    if (m_fUnk1AC < CComMan::GetDatI(0x2DA, fLv)) {
        return 0;
    }
    m_fUnk1AC = 0.0f;
    long nRoot = GetRootActType();
    if ((nRoot >= ACT_REBOUND_DUNK && nRoot <= ACT_ALLEYOOP) || nRoot == ACT_REBOUND) {
        return 0;
    }
    if (!In3PointArea()) {
        return 0;
    }
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall == 0) {
        return 0;
    }
    if (!pBall->IsActive()) {
        return 0;
    }
    if (!pBall->IsNowRebound()) {
        return 0;
    }

    MEMBER_INFO info;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable() || pChr->GetTeamNo() != m_nMyTeam || pChr->GetState() == 6) {
            continue;
        }
        info.m_apChr[nNum] = pChr;
        info.m_aScore[nNum] = 0;
        float fDist = CComManBasket::GetDistXZ(pBall->GetPos(), pChr->GetPos()) / CGameBasket::GetCourt3PointSize();
        if (fDist > 1.0f) {
            fDist = 1.0f;
        } else if (fDist < 0.0f) {
            fDist = 0.0f;
        }
        info.m_aScore[nNum] += 50.0f * (1.0f - fDist);
        float fBallRot = CComManBasket::GetRotY_P1ToP2XZ(bcs::FVEC3(0.0f, 0.0f, 0.0f), pBall->GetVelocity());
        float fRot = RoundRadPI(CComManBasket::GetRotY_P1ToP2XZ(pBall->GetPos(), pChr->GetPos()) - fBallRot);
        fRot = fabsf(fRot) / PI;
        if (fRot > 1.0f) {
            fRot = 1.0f;
        } else if (fRot < 0.0f) {
            fRot = 0.0f;
        }
        info.m_aScore[nNum] += 50.0f * (1.0f - fRot);
        nNum++;
    }
    for (long i = 0; i < nNum; i++) {
        for (long j = i + 1; j < nNum; j++) {
            if (info.m_aScore[i] <= info.m_aScore[j]) {
                // (the original swaps the characters twice and leaves the
                // score of i as it is)
                const CChrCore *pChr = info.m_apChr[i];
                long nScore = info.m_aScore[i];
                info.m_apChr[i] = info.m_apChr[j];
                info.m_apChr[i] = info.m_apChr[j];
                info.m_apChr[j] = pChr;
                info.m_aScore[j] = nScore;
            }
        }
    }
    long nRank = 0;
    for (long i = 0; i < nNum; i++) {
        if (info.m_apChr[i] == m_pChr) {
            nRank = i;
            break;
        }
    }
    long anRate[3];
    anRate[0] = CComMan::GetDatI(0x2E2, fLv);
    anRate[1] = CComMan::GetDatI(0x2EA, fLv);
    anRate[2] = CComMan::GetDatI(0x2F2, fLv);
    if (LotRate() > anRate[nRank]) {
        return 0;
    }
    Reset();
    long nMin = CComMan::GetDatI(0x2FA, fLv);
    long nMax = CComMan::GetDatI(0x302, fLv);
    long nFrame = CComManBasket::GetRandValueI(nMin, nMax);
    if (LotRate() < CComMan::GetDatI(0x30A, fLv)) {
        ActCallRebound(nFrame);
    } else {
        ActCallReboundDunk(nFrame);
    }
    return 1;
}

// Whether to go for a smash cut (Yoshi's charged cut), drawn every few
// frames once the charge is full.
int CComDrvBasketGame::JudgeSmashCut()
{
    float fLv = GetComLvF();
    m_fUnk19C += m_pChr->m_fFrameStep;
    if (m_fUnk19C < CComMan::GetDatF(0x392, fLv)) {
        return 0;
    }
    m_fUnk19C = 0.0f;
    if (static_cast<CChrCoreBasket *>(m_pChr)->GetSmashChargeRate() != 100) {
        return 0;
    }
    if (LotRate() > CComMan::GetDatI(0x39A, fLv)) {
        return 0;
    }
    Reset();
    ActCallGoSmash();
    return 1;
}

// Whether to try a cut on the ball holder now (once until the next
// ActInterrupt clears m_nUnk188).
int CComDrvBasketGame::JudgeCut()
{
    if (m_nUnk188 != 0 || (m_pChr->GetStatusFlag() & 0x20000)) {
        return 0;
    }
    if (!CanbeCut()) {
        return 0;
    }
    long nRoot = GetRootActType();
    if (nRoot == ACT_CUT || nRoot == ACT_CUT_CHANCE) {
        return 0;
    }
    if (!IsCutHit(CComMan::GetHoldBallChrCore(0))) {
        return 0;
    }
    Reset();
    ActCallCutChance();
    m_nUnk188 = 1;
    return 1;
}

// Whether to try a cut that misses on purpose, drawn every few frames when
// the ball holder of the other team is at the right distance.
int CComDrvBasketGame::JudgeMissCut()
{
    long nWait = CComMan::GetDatF(0x3A2, GetComLvF());
    m_fUnk198 += m_pChr->m_fFrameStep;
    if (nWait > m_fUnk198) {
        return 0;
    }
    m_fUnk198 = 0.0f;
    if (!CanbeCut()) {
        return 0;
    }
    long nRoot = GetRootActType();
    if (nRoot == ACT_CUT || nRoot == ACT_CUT_CHANCE) {
        return 0;
    }
    int bStar = (m_pChr->GetStatusFlag() & 0x20000) != 0;
    if (bStar) {
        return 0;
    }
    const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder == 0) {
        return 0;
    }
    if (pHolder->GetTeamNo() == m_pChr->GetTeamNo()) {
        return 0;
    }
    float fLv = GetComLvF();
    float fMin = CComMan::GetDatF(0x3AA, fLv);
    float fMax = CComMan::GetDatF(0x3B2, fLv);
    float fDist = DistXZ(pHolder->GetPos(), m_pChr->GetPos());
    if (fMin > fDist || fMax < fDist) {
        return 0;
    }
    long nRate = CComMan::GetDatF(IsAllComTeam() ? 0x3BA : 0x3C2, fLv);
    if (!(LotRate() < nRate)) {
        return 0;
    }
    Reset();
    ActCallMissCut();
    return 1;
}

// Whether to jump to block a shot coming near.
int CComDrvBasketGame::JudgeShootBlock()
{
    static const float fBaseDist = 3.0f;
    if (!CanbeJumpBlock()) {
        return 0;
    }
    switch (m_pChr->GetState()) {
    case 1:
    case 4:
    case 0x3C:
        break;
    default:
        m_nUnk180 &= ~2;
        return 0;
    }
    if (GetCurrentActType() == ACT_DEF_JUMP) {
        return 0;
    }
    if (m_nUnk180 & 2) {
        return 0;
    }
    const float &fLv = GetComLvF();
    float fRange = CComMan::GetDatF(0x3DA, fLv);
    long nBall = -1;
    for (long i = 0; i < CBallMan::GetActiveBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall != 0 && pBall->IsShootMode() &&
            CComManBasket::GetDistXZ(pBall->GetPos(), m_pChr->GetPos()) <= fBaseDist + fRange) {
            nBall = i;
        }
    }
    if (nBall < 0) {
        return 0;
    }
    m_nUnk180 |= 2;
    if (!(LotRate() < CComMan::GetDatI(0x3E2, fLv))) {
        return 0;
    }
    CItemBall *pItem = CBallMan::GetBallPtr(nBall)->GetItemEffect_();
    if (pItem != 0) {
        switch (pItem->GetType()) {
        case 2:
            if (!(LotRate() < CComMan::GetDatI(0x3EA, fLv))) {
                return 0;
            }
            break;
        case 3:
            if (LotRate() < CComMan::GetDatI(0x3F2, fLv)) {
                return 0;
            }
            break;
        }
    }
    Reset();
    CBall *pBall = CBallMan::GetBallPtr(nBall);
    m_nUnk180 |= 2;
    ActCallDefJumpRandWait(GetRotY_P1ToP2XZ(m_pChr->GetPos(), pBall->GetPos()));
    return 1;
}

// Whether to jump to block an alley-oop pass.
int CComDrvBasketGame::JudgeAlleyoopPassBlock()
{
    long nState = m_pChr->GetState();
    if ((nState >= 0x30 && nState <= 0x31) || nState == 0xC || nState == 6) {
        return 0;
    }
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall == 0) {
        return 0;
    }
    if (!pBall->IsAlleyoopPassMode()) {
        m_nUnk180 &= ~0x20;
        return 0;
    }
    if (!CanbeJumpBlock()) {
        return 0;
    }
    if (m_nUnk180 & 0x20) {
        return 0;
    }
    pBall = CBallMan::GetBallPtr(0);
    const float &fLv = GetComLvF();
    float fRange = 2.0f + CComMan::GetDatF(0x3FA, fLv);
    if (fRange < CComManBasket::GetDistXZ(pBall->GetPos(), m_pChr->GetPos())) {
        return 0;
    }
    long nRate = CComMan::GetDatI(0x402, fLv);
    m_nUnk180 |= 0x20;
    if (LotRate() > nRate) {
        return 0;
    }
    Reset();
    ActCallDefJumpRandWait(GetRotY_P1ToP2XZ(m_pChr->GetPos(), pBall->GetPos()));
    m_nUnk180 |= 0x20;
    return 1;
}

// Whether to jump to block a dunk.
int CComDrvBasketGame::JudgeDunkBlock()
{
    static const float fBaseDist = 3.0f;
    switch (m_pChr->GetState()) {
    case 1:
    case 4:
    case 0x3C:
        break;
    default:
        m_nUnk180 &= ~4;
        return 0;
    }
    if (m_nUnk180 & 4) {
        return 0;
    }
    if (!CanbeJumpBlock()) {
        return 0;
    }
    const CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
    if (pHolder == 0) {
        return 0;
    }
    switch (pHolder->GetState()) {
    case 0x26:
    case 0x24:
    case 0x28:
        break;
    default:
        m_nUnk180 &= ~4;
        return 0;
    }
    const float &fLv = GetComLvF();
    float fRange = CComMan::GetDatF(0x40A, fLv);
    bcs::FVEC3 pos = pHolder->GetPos();
    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, pHolder->GetRotY());
    float fAhead = CComMan::GetDatF(0x412, fLv);
    pos.x += dir.x * fAhead;
    pos.z += dir.z * fAhead;
    if (fBaseDist + fRange < CComManBasket::GetDistXZ(pos, m_pChr->GetPos())) {
        return 0;
    }
    m_nUnk180 |= 4;
    long nRate = CComMan::GetDatF(0x41A, fLv);
    if (LotRate() < nRate) {
        Reset();
        ActCallDefJumpRandWait(GetRotY_P1ToP2XZ(m_pChr, pHolder));
        m_nUnk180 |= 4;
        return 1;
    }
    return 0;
}

int CComDrvBasketGame::JudgeAlleyoopBlock()
{
    return 0;
}

// Whether to try an uppercut on a dunk.
int CComDrvBasketGame::JudgeJumpUpper()
{
    static const float fBaseDist = 3.0f;
    if (!static_cast<CChrCoreBasket *>(m_pChr)->CanbeNowJumpUpper() || (m_pChr->GetStatusFlag() & 0x20000)) {
        m_nUnk180 &= ~8;
        return 0;
    }
    if (m_nUnk180 & 8) {
        return 0;
    }
    const CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
    if (pHolder == 0) {
        return 0;
    }
    switch (pHolder->GetState()) {
    case 0x28:
    case 0x26:
    case 0x24:
        break;
    default:
        return 0;
    }
    float fRange = CComMan::GetDatF(0x422, GetComLvF());
    if (CComManBasket::GetDist(pHolder->GetPos(), m_pChr->GetPos()) > fBaseDist + fRange) {
        return 0;
    }
    Reset();
    ActCallJumpUpperChance();
    m_nUnk180 |= 8;
    return 1;
}

// Whether to jump to swat a shot or the holder's ball.
int CComDrvBasketGame::JudgeJumpCut()
{
    static const float fBaseDist = 3.0f;
    if (!static_cast<CChrCoreBasket *>(m_pChr)->CanbeSwatIt() || (m_pChr->GetStatusFlag() & 0x20000)) {
        m_nUnk180 &= ~0x10;
        return 0;
    }
    if (m_nUnk180 & 0x10) {
        return 0;
    }
    bcs::FVEC3 pos;
    const CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
    if (pHolder != 0) {
        if (pHolder->GetState() != 0x25) {
            return 0;
        }
        pos = pHolder->GetPos();
    } else {
        CBall *pNear = 0;
        float fNear = 9999.9f;
        for (long i = 0; i < CBallMan::GetActiveBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall != 0 && pBall->IsShootMode() && pBall->GetShootType() == 6) {
                const bcs::FVEC3 &rBallPos = pBall->GetPos();
                float fDist = CComManBasket::GetDistXZ(rBallPos, m_pChr->GetPos());
                if (fDist < fNear) {
                    fNear = fDist;
                    pos = rBallPos;
                    pNear = pBall;
                }
            }
        }
        if (pNear == 0) {
            return 0;
        }
    }
    float fRange = CComMan::GetDatF(0x42A, GetComLvF());
    if (CComManBasket::GetDist(pos, m_pChr->GetPos()) > fBaseDist + fRange) {
        return 0;
    }
    Reset();
    ActCallJumpCutChance();
    m_nUnk180 |= 0x10;
    return 1;
}

// Whether to throw the stock item now; rPos gets the target.
int CComDrvBasketGame::JudgeItemThrow(bcs::FVEC3 &rPos)
{
    if (!m_pChr->IsHaveStockItem()) {
        return 0;
    }
    if (m_pChr->IsHoldBall()) {
        return 0;
    }
    if (!CanbeItemThrow()) {
        return 0;
    }
    float fLv = GetComLvF();
    m_fUnk1B0 += m_pChr->m_fFrameStep;
    if (m_fUnk1B0 < CComMan::GetDatF(0x5A3, fLv)) {
        return 0;
    }
    m_fUnk1B0 = 0.0f;
    if (JudgeItemThrow1(rPos)) {
        return 1;
    }
    return JudgeItemThrow2(rPos);
}


// The throw of an item at a character of the other team: one in range,
// the one nearest to a loose ball, or one drawn by whether it holds the
// ball or blocks. rPos gets the target (local), spread at random.
int CComDrvBasketGame::JudgeItemThrow1(bcs::FVEC3 &rPos)
{
    const float &fLv = GetComLvF();
    long nKind;
    switch (m_pChr->GetStockItemType()) {
    case 0:
        nKind = 0;
        break;
    case 1:
        nKind = 1;
        break;
    case 3:
        nKind = 2;
        break;
    case 5:
        nKind = 3;
        break;
    case 2:
        nKind = 4;
        break;
    default:
        return 0;
    }

    long nChr = m_pChr->GetChrTypeID();
    // The data numbers of the throw rate (all-computer team, other) and of
    // the range, by item.
    long anRateId[5][2] = {{0x5E8, 0x5F0}, {0x610, 0x618}, {0x638, 0x640}, {0x660, 0x668}, {0x688, 0x690}};
    long bAllCom = CComManBasket::IsAllComTeam(m_nMyTeam);
    float fChrRate = CComMan::GetDatF(0x5D3, nChr);
    long nRate = CComMan::GetDatF(anRateId[nKind][bAllCom == 0], fLv) * fChrRate;
    if (!(LotRate() < nRate)) {
        return 0;
    }
    long anRangeId[5][2] = {{0x5F8, 0x600}, {0x620, 0x628}, {0x648, 0x650}, {0x670, 0x678}, {0x698, 0x6A0}};
    const float &fMin = CComMan::GetDatF(anRangeId[nKind][0], fLv);
    const float &fMax = CComMan::GetDatF(anRangeId[nKind][1], fLv);
    const CChrCore *apTarget[8];
    long nTarget = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable() || pChr->GetTeamNo() == m_nMyTeam || pChr->GetComLevelWork() != 0) {
            continue;
        }
        float fDist = DistXZ(m_pChr->GetPos(), pChr->GetPos());
        if (fDist >= fMin && fDist <= fMax) {
            apTarget[nTarget] = pChr;
            nTarget++;
        }
    }
    if (nTarget == 0) {
        return 0;
    }

    const CChrCore *pTarget = 0;
    if ((CGame::g_GameWork.m_nFlag18 & 1) != 0) {
        // A loose ball: the one nearest to it.
        long nLooseRate = CComMan::GetDatF(0x5CB, fLv);
        if (!(LotRate() < nLooseRate)) {
            return 0;
        }
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (pBall == 0) {
            return 0;
        }
        float fNear = DistXZ(pBall->GetPos(), m_pChr->GetPos());
        for (long i = 0; i < nTarget; i++) {
            const CChrCore *pChr = apTarget[i];
            float fDist = DistXZ(pBall->GetPos(), pChr->GetPos());
            if (!(fDist > fNear)) {
                fNear = fDist;
                pTarget = pChr;
            }
        }
    } else if (IsMyTeamHoldBall(0) || CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
        // On offence: rather the ones that block.
        float afRate[2];
        afRate[0] = CComMan::GetDatF(0x5B3, fLv);
        afRate[1] = CComMan::GetDatF(0x5AB, fLv);
        for (long i = 0; i < nTarget; i++) {
            const CChrCore *pChr = apTarget[i];
            float fRate = afRate[pChr->IsNowBlockWork()];
            if (LotRate() < fRate && IsItemThrowTarget(pChr)) {
                pTarget = pChr;
                break;
            }
        }
    } else {
        // On defence: rather the ball holder.
        float afRate[2];
        afRate[0] = CComMan::GetDatF(0x5C3, fLv);
        afRate[1] = CComMan::GetDatF(0x5BB, fLv);
        for (long i = 0; i < nTarget; i++) {
            const CChrCore *pChr = apTarget[i];
            float fRate = afRate[pChr->IsHoldBall()];
            if (LotRate() < fRate && IsItemThrowTarget(pChr)) {
                pTarget = pChr;
                break;
            }
        }
    }
    if (pTarget == 0) {
        return 0;
    }

    long anSpreadId[5] = {0x608, 0x630, 0x658, 0x680, 0x6A8};
    float fSpread = bcs::BCS_RandMax(CComMan::GetDatI(anSpreadId[nKind], fLv));
    if (bcs::BCS_RandMax(1L)) {
        fSpread = -fSpread;
    }
    fSpread = bcs::DEG_TO_RAD(fSpread);
    float fRot = RoundRad(fSpread + m_pChr->GetRotY_ToPosXZ(pTarget->GetPos()));
    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, fRot);
    float fDist = DistXZ(m_pChr->GetPos(), pTarget->GetPos());
    dir.x = dir.x * fDist + m_pChr->GetPos().x;
    dir.z = dir.z * fDist + m_pChr->GetPos().z;
    rPos = CComMan::World2Local(dir, -1);
    return 1;
}

// The throw of a banana or a shell (item types 5 and 2) in front of the
// goal the driver guards. rPos gets the target (local).
int CComDrvBasketGame::JudgeItemThrow2(bcs::FVEC3 &rPos)
{
    long nKind;
    switch (m_pChr->GetStockItemType()) {
    case 5:
        nKind = 0;
        break;
    case 2:
        nKind = 1;
        break;
    default:
        return 0;
    }
    const float &fLv = GetComLvF();
    long anRateId[2][2] = {{0x6C8, 0x6D0}, {0x6F0, 0x6F8}};
    if (!(LotRate() < CComMan::GetDatF(anRateId[nKind][IsAllComTeam() == 0], fLv))) {
        return 0;
    }
    float fDist = DistXZ(m_pChr->GetPos(), m_pChr->GetGuardGoalPos());
    long anRangeId[2][2] = {{0x6B0, 0x6B8}, {0x6D8, 0x6E0}};
    float fMin = CComMan::GetDatF(anRangeId[nKind][0], fLv);
    float fMax = CComMan::GetDatF(anRangeId[nKind][1], fLv);
    if (!(fDist >= fMin && fDist <= fMax)) {
        return 0;
    }
    long anSpreadId[2] = {0x6C0, 0x6E8};
    float fSpread = bcs::BCS_RandMax(CComMan::GetDatI(anSpreadId[nKind], fLv));
    if (bcs::BCS_RandMax(1L)) {
        fSpread = -fSpread;
    }
    fSpread = bcs::DEG_TO_RAD(fSpread);
    float fRot = RoundRad(fSpread + m_pChr->GetRotY_ToGuardGoal());
    bcs::FVEC3 dir;
    bcs::BCS_VecZ_RotY(dir, 1.0f, fRot);
    dir.x = dir.x * fDist + m_pChr->GetPos().x;
    dir.z = dir.z * fDist + m_pChr->GetPos().z;
    rPos = CComMan::World2Local(dir, -1);
    return 1;
}

// Whether no character of nChrBits stands in the way of a shot.
int CComDrvBasketGame::JudgeShootCourseFree(unsigned long nChrBits) const
{
    float fLv = GetComLvF();
    float fWidth = CComMan::GetDatF(0x78, fLv);
    float fLen = CComMan::GetDatF(0x80, fLv);
    long nDeg = CComMan::GetDatI(0x88, fLv);
    return !CComManBasket::IsExistChrToTarget(m_pChr, bcs::DEG_TO_RAD(nDeg), m_TargetGoalPos, fWidth, fLen,
                                              nChrBits);
}

// Whether no character of the other team stands in the way of a shot.
int CComDrvBasketGame::JudgeShootCourseFree() const
{
    return JudgeShootCourseFree(GetEmTeamEnableBit());
}

} // namespace msw

namespace unit_A3AF0_cpp {
// uncalled stand-in: the original has here a function its link stripped,
// msw::CComDrvBasketGame::JudgeShootCourseBlockPlayer (body not known),
// whose static NUM (6) and constant 1.5 stay in .rodata.
float UnusedJudgeShootCourseBlockPlayer(long n)
{
    static const long NUM = 6;
    return n < NUM ? 1.5f : 0.0f;
}
}

namespace msw {

// Whether to escape from a bomb ball shot at the goal that will blow up
// where the driver stands (once per shot: flag 0x8000).
int CComDrvBasketGame::JudgeBombShootEsp()
{
    if (m_nFlag17C & 0x8000) {
        return 0;
    }
    CBall *pFound = 0;
    CItemBallBasket *pBomb = 0;
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall == 0 || !pBall->IsActive() || !pBall->IsShootMode()) {
            continue;
        }
        CItemBallBasket *pItem = static_cast<CItemBallBasket *>(pBall->GetItemEffect_());
        if (pItem == 0 || pItem->GetType() != 3) {
            continue;
        }
        bcs::FVEC3 center = pItem->GetExplodeCenterPos();
        float fDist = DistXZ(m_pChr->GetPos(), center);
        if (!(fDist > 1.0f + pItem->GetExplodeR())) {
            pFound = pBall;
            pBomb = pItem;
            break;
        }
    }
    if (pFound == 0) {
        return 0;
    }
    m_nFlag17C |= 0x8000;
    if (!(LotRate() < CComMan::GetDatI(0x3F2, GetComLvF()))) {
        return 0;
    }
    bcs::FVEC3 center = pBomb->GetExplodeCenterPos();
    bcs::FVEC3 dir = Diff(m_pChr->GetPos(), center);
    NormalizeXZ(dir, dir);
    CChrCore *pMy = m_pChr;
    float fLen = RandRangeF(1.5f, 2.5f);
    fLen += pBomb->GetExplodeR();
    fLen += pMy->GetCollSize_BodyDamage();
    bcs::FVEC3 pos;
    pos.x = dir.x * fLen + center.x;
    pos.y = 0.0f;
    pos.z = dir.z * fLen + center.z;
    pos = CComMan::World2Local(pos, -1);
    Reset();
    ActCallGoEscape2(pos);
    return 1;
}

// Whether to start a combination play: drawn, at the right distance from
// the goal.
int CComDrvBasketGame::JudgeCombinationPlay() const
{
    if (!CanbeCombination()) {
        return 0;
    }
    const float &fLv = GetComLvF();
    if (!(LotRate() < CComMan::GetDatF(0x123, fLv))) {
        return 0;
    }
    bcs::FVEC3 d = Diff(m_TargetGoalPos, m_MyPos);
    float fDist = PSVECMag((const Vec *)&d);
    float fMin = CComMan::GetDatF(0x113, fLv);
    float fMax = CComMan::GetDatF(0x11B, fLv);
    if (fDist < fMin || fDist > fMax) {
        return 0;
    }
    return 1;
}

// A character of the other team starts a shot (pMsg: the character, the
// kind of shot): block it, run back to the goal, or defend the alley-oop.
void CComDrvBasketGame::MsgShootStart(_CHR_MSG *pMsg)
{
    // static-order swap: declared in the order the original emits them.
    static const long ARRAY_NUM = 3;
    static const float fBlockDist = 3.0f;
    const CChrCore *pShooter = CChrMan::GetChrCore(pMsg->m_nParam0);
    if (m_nMyTeam == pShooter->GetTeamNo()) {
        return;
    }
    const float &fLv = GetComLvF();
    const bcs::FVEC3 &rShootPos = pShooter->GetPos();
    switch (pMsg->m_nParam1) {
    case 0:
    case 1:
    case 6: {
        float fRange = CComMan::GetDatF(0x3CA, fLv);
        if (fBlockDist + fRange >= CComManBasket::GetDistXZ(rShootPos, m_pChr->GetPos()) &&
            LotRate() < CComMan::GetDatI(0x3D2, fLv)) {
            Reset();
            ActCallShootBlock();
            return;
        }
        if (m_pChr->GetState() == 6) {
            return;
        }
        const bcs::FVEC3 &rGuard = m_pChr->GetGuardGoalPos();
        float fMyDist = CComManBasket::GetDistXZ(rGuard, m_pChr->GetPos());
        long nNearer = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr != m_pChr &&
                pChr->GetState() != 6 && CComManBasket::GetDistXZ(rGuard, pChr->GetPos()) < fMyDist) {
                nNearer++;
            }
        }
        long anRate[3];
        anRate[0] = CComMan::GetDatI(0x44A, fLv);
        anRate[1] = CComMan::GetDatI(0x452, fLv);
        anRate[2] = CComMan::GetDatI(0x45A, fLv);
        if (LotRate() < anRate[nNearer]) {
            Reset();
            ActCallGoDefenseGoal();
        }
        break;
    }
    case 3:
        if (m_pChr == CComManBasketGame::GetUrgentGuardCore(m_nMyTeam, -1, 0)) {
            Reset();
            ActCallGoDefenseGoal();
        }
        break;
    case 8: {
        if (GetRootActType() == ACT_DEF_ALLEYOOP) {
            return;
        }
        const CChrCore *apMember[ARRAY_NUM];
        long nMember = CComManBasketGame::GetMemBer(m_nMyTeam, apMember, ARRAY_NUM, -1, 0);
        for (long i = 0; i < nMember; i++) {
            CComDrv *pDrv = CComMan::GetComDrv(const_cast<CChrCore *>(apMember[i]));
            if (pDrv != 0 && pDrv->GetRootActType() == ACT_DEF_ALLEYOOP) {
                return;
            }
        }
        bcs::FVEC3 from = CComMan::GetChrPos(pShooter);
        bcs::FVEC3 to = m_Pos148;
        bcs::FVEC3 dir;
        dir = Diff(from, to);
        NormalizeXZ(dir, dir);
        float fFront = CComMan::GetDatF(0x482, fLv);
        from.x = dir.x * fFront + from.x;
        from.y = 0.0f;
        from.z = dir.z * fFront + from.z;
        float fBack = CComMan::GetDatF(0x48A, fLv);
        to.x = to.x - dir.x * fBack;
        to.y = 0.0f;
        to.z = to.z - dir.z * fBack;
        float fLen = DistXZ(to, from);
        float fWidth = CComMan::GetDatF(0x47A, fLv);
        SortBase::SORT aSort[32];
        SortBase::SORT *pSort = aSort;
        long nNum = 0;
        for (long i = 0; i < nMember; i++) {
            if (CComMan::IsPointInFaceXZ(CComMan::GetChrPos(apMember[i]), from, to, fWidth, fLen)) {
                float fDist = CComManBasket::GetDistXZ(apMember[i], pShooter) / 5.0f;
                if (fDist > 1.0f) {
                    fDist = 1.0f;
                } else if (fDist < 0.0f) {
                    fDist = 0.0f;
                }
                long nScore = 50.0f * (1.0f - fDist);
                float fGoalRot = pShooter->GetRotY_ToPosXZ(pShooter->GetTargetGoalPos());
                float fRot = RoundRadPI(pShooter->GetRotY_ToPosXZ(apMember[i]->GetPos()) - fGoalRot);
                fRot = fabsf(fRot) / PI;
                if (fRot > 1.0f) {
                    fRot = 1.0f;
                } else if (fRot < 0.0f) {
                    fRot = 0.0f;
                }
                nNum++;
                pSort->m_nId = apMember[i]->GetPlayerNo();
                nScore += 50.0f * (1.0f - fRot);
                pSort->m_fParam = nScore;
                pSort++;
            } else {
                if (m_pChr == apMember[i]) {
                    return;
                }
                apMember[i] = 0;
            }
        }
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        if (m_pChr->GetPlayerNo() != aSort[nNum - 1].m_nId) {
            return;
        }
        if (LotRate() < CComMan::GetDatI(0x492, fLv)) {
            Reset();
            ActCallDefAlleyoop(pShooter);
        }
        break;
    }
    }
}

// A team mate or a character of the other team shoots (pMsg: the
// shooter, the kind of shot).
void CComDrvBasketGame::MsgSomeBodyShoot(_CHR_MSG *pMsg)
{
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return;
    }
    CChrCore *pShooter = CChrMan::GetChrCore(pMsg->m_nParam0);
    if (m_pChr == pShooter) {
        return;
    }
    if (m_pChr->GetState() == 6) {
        return;
    }
    if (m_nMyTeam == pShooter->GetTeamNo()) {
        MsgSomeBodyShootOffense(pMsg);
        return;
    }
    if (GetRootActType() != ACT_GO_DEFENSE_GOAL) {
        return;
    }
    switch (pMsg->m_nParam1) {
    case 0:
    case 1:
        Reset();
        ActCallGoDefenseGoal();
        break;
    }
}

// A team mate shoots (pMsg: the shooter, the kind of shot, the ball):
// turn it into an alley-oop, or go for the rebound dunk or back on
// offence.
void CComDrvBasketGame::MsgSomeBodyShootOffense(_CHR_MSG *pMsg)
{
    if (JudgeShootToAlleyoopDunk()) {
        return;
    }
    CChrCore *pShooter = CChrMan::GetChrCore(pMsg->m_nParam0);
    switch (pMsg->m_nParam1) {
    case 0:
    case 1:
    case 6: {
        CItemBall *pItem = CBallMan::GetBallPtr(pMsg->m_nParam2)->GetItemEffect_();
        if (pItem != 0 && pItem->GetType() == 3) {
            return;
        }
        const bcs::FVEC3 &rGuard = m_pChr->GetGuardGoalPos();
        float fMyDist = CComManBasket::GetDistXZ(rGuard, m_pChr->GetPos());
        long nNearer = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr != m_pChr &&
                pChr != pShooter && pChr->GetState() != 6 &&
                CComManBasket::GetDistXZ(rGuard, pChr->GetPos()) < fMyDist) {
                nNearer++;
            }
        }
        float fLv = GetComLvF();
        long anRate[2];
        anRate[0] = CComMan::GetDatI(0x322, fLv);
        anRate[1] = CComMan::GetDatI(0x32A, fLv);
        if (LotRate() < anRate[nNearer]) {
            Reset();
            ActCallGoReboundDunk();
        } else {
            Reset();
            ActCallGoOffense();
        }
        break;
    }
    }
}

// The ball bounced off the goal: go for the rebound (on defence) or the
// rebound dunk, more likely the nearer the driver is to the goal.
void CComDrvBasketGame::MsgReboundStart(_CHR_MSG *pMsg)
{
    if (m_pChr->GetState() == 6) {
        return;
    }
    long nRoot = GetRootActType();
    if ((nRoot >= ACT_GO_REBOUND_DUNK && nRoot <= ACT_ALLEYOOP) || nRoot == ACT_REBOUND) {
        return;
    }
    long nUserTeam = CGame::g_GameWork.m_nUserTeam;
    const bcs::FVEC3 &rGoal = CGame::GetGoalPos(CGame::GetTargetGoalIdx(nUserTeam));
    float fMyDist = CComManBasket::GetDistXZ(rGoal, m_pChr->GetPos());
    long nNearer = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr != m_pChr &&
            CComManBasket::GetDistXZ(rGoal, pChr->GetPos()) < fMyDist) {
            nNearer++;
        }
    }
    const float &fLv = GetComLvF();
    if (nUserTeam != m_nMyTeam) {
        long anRate[3];
        anRate[0] = CComMan::GetDatI(0x462, fLv);
        anRate[1] = CComMan::GetDatI(0x46A, fLv);
        anRate[2] = CComMan::GetDatI(0x472, fLv);
        if (LotRate() < anRate[nNearer]) {
            Reset();
            ActCallRebound();
        }
    } else {
        long anRate[3];
        anRate[0] = CComMan::GetDatI(0x332, fLv);
        anRate[1] = CComMan::GetDatI(0x33A, fLv);
        anRate[2] = CComMan::GetDatI(0x342, fLv);
        if (LotRate() < anRate[nNearer]) {
            Reset();
            ActCallGoReboundDunk();
        }
    }
}

inline long CComDrvBasketGame::MsgCheckArtsShootFree_() const
{
    static const long MAX = 3;
    const CChrCore *apEm[MAX];
    long nEm = CComManBasketGame::GetMemberNoBadCondition(1 - m_nMyTeam, apEm, MAX, -1, 0);
    float fWidth = m_fUnk1F8;
    long nType = GetChrTypeID();
    if (nType >= 3 && nType <= 4) {
        return CheckFreeShootCourseToCurve(
            nEm, apEm,
            static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetArtsCurveValue_(m_pChr->GetChrTypeID()), fWidth,
            fWidth);
    }
    if (nEm == 0) {
        return 0;
    }
    float fLen = DistXZ(m_MyPos, m_TargetGoalPos);
    for (long i = 0; i < nEm; i++) {
        if (CComManBasket::IsExistTarget(m_MyPos, m_TargetGoalPos, fWidth, fLen, apEm[i])) {
            return -1;
        }
    }
    return 0;
}

// Asked every frame while the arts shot is held (pMsg: the frames held and
// the frames it may be held): whether to shoot now. The driver draws how
// long to hold it, waits for a free course and a short delay by character.
int CComDrvBasketGame::MsgCheckArtsShootFree(_CHR_MSG *pMsg)
{
    float fLv = GetComLvF();
    int bShoot = 0;
    switch (m_nUnk1EC) {
    case 0: {
        long nMinId;
        long nMaxId;
        switch (m_pChr->GetChrTypeID()) {
        case 1:
            nMinId = 0x1123;
            nMaxId = 0x112B;
            break;
        case 2:
            nMinId = 0x11BB;
            nMaxId = 0x11C3;
            break;
        case 3:
            nMinId = 0x123B;
            nMaxId = 0x1243;
            break;
        case 4:
            nMinId = 0x12C3;
            nMaxId = 0x12CB;
            break;
        case 5:
            nMinId = 0x1353;
            nMaxId = 0x135B;
            break;
        case 6:
            nMinId = 0x13CB;
            nMaxId = 0x13D3;
            break;
        case 7:
            nMinId = 0x145B;
            nMaxId = 0x1463;
            break;
        case 8:
            nMinId = 0x14DB;
            nMaxId = 0x14E3;
            break;
        case 9:
            nMinId = 0x1563;
            nMaxId = 0x156B;
            break;
        case 10:
            nMinId = 0x15F3;
            nMaxId = 0x15FB;
            break;
        case 11:
            nMinId = 0x167B;
            nMaxId = 0x1683;
            break;
        case 12:
            nMinId = 0x16F3;
            nMaxId = 0x16FB;
            break;
        case 13:
            nMinId = 0x177B;
            nMaxId = 0x1783;
            break;
        case 14:
            nMinId = 0x17EB;
            nMaxId = 0x17F3;
            break;
        case 15:
            nMinId = 0x187B;
            nMaxId = 0x1883;
            break;
        case 16:
            nMinId = 0x1913;
            nMaxId = 0x191B;
            break;
        case 17:
            nMinId = 0x1993;
            nMaxId = 0x199B;
            break;
        case 0:
            nMinId = 0x10AB;
            nMaxId = 0x10B3;
            break;
        default:
            nMinId = 0x10AB;
            nMaxId = 0x10B3;
            break;
        }
        long nMin = CComMan::GetDatI(nMinId, fLv);
        long nMax = CComMan::GetDatI(nMaxId, fLv);
        m_fUnk1F0 = CComManBasket::GetRandValueI(nMin, nMax) / 100.0f;
        m_fUnk1F8 = GetArtsCourseCheckWidth();
        m_nUnk1EC++;
    }
    case 1: {
        if (MsgCheckArtsShootFree_() == -1) {
            break;
        }
        long nId;
        switch (m_pChr->GetChrTypeID()) {
        case 1:
            nId = 0x113B;
            break;
        case 2:
            nId = 0x11D3;
            break;
        case 3:
            nId = 0x1253;
            break;
        case 4:
            nId = 0x12DB;
            break;
        case 5:
            nId = 0x136B;
            break;
        case 6:
            nId = 0x13E3;
            break;
        case 7:
            nId = 0x1473;
            break;
        case 8:
            nId = 0x14F3;
            break;
        case 9:
            nId = 0x157B;
            break;
        case 10:
            nId = 0x160B;
            break;
        case 11:
            nId = 0x1693;
            break;
        case 12:
            nId = 0x170B;
            break;
        case 13:
            nId = 0x1793;
            break;
        case 14:
            nId = 0x1803;
            break;
        case 15:
            nId = 0x1893;
            break;
        case 16:
            nId = 0x192B;
            break;
        case 17:
            nId = 0x19AB;
            break;
        case 0:
            nId = 0x10C3;
            break;
        default:
            nId = 0x10C3;
            break;
        }
        m_fUnk1F4 = CComMan::GetDatF(nId, fLv);
        m_nUnk1EC++;
    }
    case 2:
        m_fUnk1F4 -= m_pChr->m_fFrameStep;
        if (m_fUnk1F4 <= 0.0f) {
            bShoot = 1;
            m_fUnk1F4 = 0.0f;
            m_nUnk1EC++;
        }
        break;
    }
    if (bShoot == 0) {
        float fRate = (float)pMsg->m_nParam0 / (float)pMsg->m_nParam1;
        fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
        bShoot = 1.0f - fRate > m_fUnk1F0;
    }
    return bShoot;
}

} // namespace msw

namespace unit_A3AF0_cpp {
// uncalled stand-in: as UnusedConst4, for the constant 15 the original
// creates here.
float UnusedConst15()
{
    return 15.0f;
}
}

namespace msw {

// Whether the swat runs.
int CComDrvBasketGame::LotRateSwatIt()
{
    return GetCurrentActType() == ACT_JUMP_CUT;
}

// The item use of a jump shot or an item shot (0 when not now).
long CComDrvBasketGame::LotRateUseItem(long nFrame, long nStart, long nEnd)
{
    long nAct = GetCurrentActType();
    if (nAct == ACT_JUMP_SHOOT || nAct == ACT_ITEM_SHOOT) {
        if (!LotRateJumpShoot(nFrame, nStart, nEnd)) {
            return 0;
        }
        return static_cast<SHOOT_WORK *>(GetCurrentActWork())->m_nUseItem;
    }
    return 0;
}

// Whether to let the jump shot go now (nFrame of the jump, from nStart to
// nEnd): at the frame the curve gives; on the boss stage the shot may be
// changed or broken off once while the boss's mouth is shut.
long CComDrvBasketGame::LotRateJumpShoot(long nFrame, long nStart, long nEnd)
{
    long nAct = GetCurrentActType();
    if (nAct == ACT_JUMP_SHOOT || nAct == ACT_ITEM_SHOOT) {
        SHOOT_WORK *pWork = static_cast<SHOOT_WORK *>(GetCurrentActWork());
        float fRate;
        if (pWork->m_nFlag & 1) {
            fRate = (float)(nFrame - nStart) / (float)(nEnd - nStart);
        } else {
            fRate = (float)nFrame / (float)nStart;
        }
        long bShoot = fRate >= pWork->m_fCurve;
        if (bShoot && !(pWork->m_nFlag & 2)) {
            pWork->m_nFlag |= 2;
            const float &fLv = GetComLvF();
            if (CGame::g_GameWork.GetStage() == 7) {
                CNpcBossPackun *pBoss = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetBossPackun_(m_nUnk8C);
                if (pBoss != 0 && !pBoss->IsMouthOpen()) {
                    long anRate[3];
                    anRate[0] = CComMan::GetDatI(0xE57, fLv);
                    anRate[1] = CComMan::GetDatI(0xE5F, fLv);
                    anRate[2] = CComMan::GetDatI(0xE6F, fLv);
                    switch (LotProbability(anRate, 3)) {
                    case 1:
                        AddShootCurve(pWork, CComMan::GetDatI(0xE67, fLv) / 100.0f);
                        return 0;
                    case 2:
                        Reset();
                        ActCallCancelPass(0);
                        return 0;
                    }
                }
            }
        }
        return bShoot;
    }
    float fRate = (float)nFrame / (float)nEnd;
    fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
    if (fRate < 0.8f) {
        return LotRate() < 5;
    }
    return 1;
}

// The panels of the court: PANEL_W across (x) and PANEL_H deep (z).
static const long PANEL_W = 6;
static const long PANEL_H = 8;

void CComDrvBasketGame::ClearPanelData()
{
    bcs::BCS_CpuClear32(m_aPanelDate, sizeof(m_aPanelDate));
}

// The column of the panel at fX (world).
long CComDrvBasketGame::GetPanelWIdx(float fX) const
{
    float fLeft = CStageMan::GetInstance()->GetCourtLeft();
    float fWidth = CStageMan::GetInstance()->GetCourtRight() - fLeft;
    float fPanel = fWidth / PANEL_W;
    float fOfs = fX - fLeft;
    fOfs = fOfs > fWidth ? fWidth : (fOfs < 0.0f ? 0.0f : fOfs);
    long nIdx = fOfs / fPanel;
    return nIdx > PANEL_W - 1 ? PANEL_W - 1 : (nIdx < 0 ? 0 : nIdx);
}

// The row of the panel at fZ (world).
long CComDrvBasketGame::GetPanelHIdx(float fZ) const
{
    float fFront = CStageMan::GetInstance()->GetCourtFront();
    float fDepth = CStageMan::GetInstance()->GetCourtBack() - fFront;
    float fPanel = fDepth / PANEL_H;
    float fOfs = fZ - fFront;
    fOfs = fOfs > fDepth ? fDepth : (fOfs < 0.0f ? 0.0f : fOfs);
    long nIdx = fOfs / fPanel;
    return nIdx > PANEL_H - 1 ? PANEL_H - 1 : (nIdx < 0 ? 0 : nIdx);
}

signed char CComDrvBasketGame::GetPanelDate(long nW, long nH) const
{
    if ((unsigned long)nW > PANEL_W - 1) {
        return -1;
    }
    if ((unsigned long)nH <= PANEL_H - 1) {
        return m_aPanelDate[nH * PANEL_W + nW];
    }
    return -1;
}

void CComDrvBasketGame::SetPanelDate(long nW, long nH, char nDate)
{
    if ((unsigned long)nW > PANEL_W - 1) {
        return;
    }
    if ((unsigned long)nH > PANEL_H - 1) {
        return;
    }
    m_aPanelDate[nH * PANEL_W + nW] = nDate;
}

// The rectangle (world) of panel (nW, nH).
void CComDrvBasketGame::GetPanelRect(long nW, long nH, float &rLeft, float &rRight, float &rTop, float &rBottom)
{
    CStageMan::GetInstance()->GetCourtRect(&rLeft, &rRight, &rTop, &rBottom);
    float fWidth = (rRight - rLeft) / PANEL_W;
    rLeft = fWidth * nW + rLeft;
    rRight = rLeft + fWidth;
    float fDepth = (rBottom - rTop) / PANEL_H;
    rTop = fDepth * nH + rTop;
    rBottom = rTop + fDepth;
}

// The free area around the driver as a rectangle (world).
void CComDrvBasketGame::GetFreeAreaRect(float &rLeft, float &rRight, float &rTop, float &rBottom)
{
    long nLeft, nRight, nTop, nBottom;
    GetFreeAreaRect(nLeft, nRight, nTop, nBottom);
    float fRight, fBottom;
    GetPanelRect(nLeft, nTop, rLeft, fRight, rTop, fBottom);
    float fLeft, fTop;
    GetPanelRect(nRight, nBottom, fLeft, rRight, fTop, rBottom);
}

// The free area around the driver in panels: nCheckMath panels each way,
// shifted towards the court's middle.
void CComDrvBasketGame::GetFreeAreaRect(long &rLeft, long &rRight, long &rTop, long &rBottom)
{
    static const long nCheckMath = 2;
    bcs::FVEC3 pos = m_pChr->GetPos();
    long nOfsX = nCheckMath * pos.x / fabsf(CStageMan::GetInstance()->GetCourtRight());
    long nOfsZ = nCheckMath * pos.z / fabsf(CStageMan::GetInstance()->GetCourtFront());
    long nW = GetPanelWIdx(pos.x) - nOfsX;
    long nH = GetPanelHIdx(pos.z) - nOfsZ;
    rLeft = nW - nCheckMath;
    rTop = nH - nCheckMath;
    if (rLeft < 0) {
        rLeft = 0;
    }
    if (rTop < 0) {
        rTop = 0;
    }
    rRight = nW + nCheckMath;
    rBottom = nH + nCheckMath;
    if (rRight >= PANEL_W) {
        rRight = PANEL_W - 1;
    }
    if (rBottom >= PANEL_H) {
        rBottom = PANEL_H - 1;
    }
}

void CComDrvBasketGame::ClearPanelDisableFlg()
{
    for (long i = 0; i < 18; i++) {
        for (long j = 0; j < 4; j++) {
            for (long k = 0; k < 8; k++) {
                m_aPanelDisable[i][j][k] = 0;
            }
        }
    }
}

void CComDrvBasketGame::GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos)
{
    GetPosTurnAround(rPos);
}

// What GetPosTurnAround goes round (m_nTurnKind): 7 none any more; 10 the
// obstacle changed.
enum TURN_KIND {
    TURN_STAGE03 = 1,
    TURN_STAGE09,
    TURN_STAGE07,
    TURN_STAGE11,
    TURN_STAGE15,
    TURN_STAGE15_KARON,
    TURN_NONE,
    TURN_CHR,
    TURN_ITEM,
    TURN_CHANGED
};

// rPos (where the driver runs to) moved to go round what is in the way: a
// stage gimmick, the train of stage 17 (it stops short of it), a
// character or an item. Once a turn has started it keeps going round the
// same one while that is still in the way. Not zero while turning.
int CComDrvBasketGame::GetPosTurnAround(bcs::FVEC3 &rPos)
{
    m_nUnk1E8 = 0;
    long nKind = -1;
    long nTarget = -1;
    long nDir = -1;
    if (m_nTurnKind < 0) {
        if ((nTarget = Stage03_GetGimmickToTarget(rPos)) != 0) {
            nKind = TURN_STAGE03;
            nDir = Stage03_GetPosTurnAroundGimmick(rPos, nTarget, -1);
        } else if ((nTarget = Stage07_GetGimmickToTarget(rPos)) != 0) {
            nKind = TURN_STAGE07;
            nDir = Stage07_GetPosTurnAroundGimmick(rPos, nTarget, -1);
        } else if ((nTarget = Stage09_GetGimmickToTarget(rPos)) != 0) {
            nKind = TURN_STAGE09;
            nDir = Stage09_GetPosTurnAroundGimmick(rPos, nTarget, -1);
        } else if ((nTarget = Stage11_GetGimmickToTarget(rPos)) != 0) {
            nKind = TURN_STAGE11;
            nDir = GetTurnAroundRotDirLotOfOneCore(Stage11_GetPosTurnAroundGimmick(rPos, nTarget, -1), rPos);
        } else if ((nTarget = Stage15_GetGimmickToTarget(rPos)) != 0) {
            nKind = TURN_STAGE15;
            nDir = Stage15_GetPosTurnAroundGimmick(rPos, nTarget, -1);
        } else {
            bcs::FVEC3 stop;
            int bHit = CComManBasketGame::HitCheckTrain(stop, m_MyPos, rPos) != 0;
            if (bHit) {
                if (CComManBasketGame::HitCheckTrain(rPos, m_MyPos, rPos)) {
                    m_nUnk1E8 = 1;
                }
                return 1;
            }
            if ((nTarget = StageXX_GetChrToTarget(rPos)) >= 0) {
                nKind = TURN_CHR;
                nDir = GetTurnAroundRotDirLotOfOneCore(StageXX_GetPosTurnAroundChr(rPos, nTarget, -1), rPos);
            } else if ((nTarget = StageXX_GetItemToTarget(rPos)) != 0) {
                nKind = TURN_ITEM;
                nDir = StageXX_GetPosTurnAroundItem(rPos, nTarget, -1);
            } else if ((nTarget = Stage15_GetGimmickToTargetKaron(rPos)) != 0) {
                nKind = TURN_STAGE15_KARON;
                nDir = Stage15_GetPosTurnAroundGimmickKaron(rPos, nTarget, -1);
            }
        }
        if (nKind >= 0) {
            m_nTurnKind = nKind;
            m_nTurnTarget = nTarget;
            m_nTurnDir = nDir;
            m_fTurnFrame = 40.0f;
        }
    } else {
        nDir = m_nTurnDir;
        switch (m_nTurnKind) {
        case TURN_STAGE03:
            if ((nTarget = Stage03_GetGimmickToTarget(rPos)) == m_nTurnTarget) {
                Stage03_GetPosTurnAroundGimmick(rPos, nTarget, nDir);
            }
            break;
        case TURN_STAGE07:
            if ((nTarget = Stage07_GetGimmickToTarget(rPos)) == m_nTurnTarget) {
                Stage07_GetPosTurnAroundGimmick(rPos, nTarget, nDir);
            }
            break;
        case TURN_STAGE09:
            if ((nTarget = Stage09_GetGimmickToTarget(rPos)) == m_nTurnTarget) {
                Stage09_GetPosTurnAroundGimmick(rPos, nTarget, nDir);
            }
            break;
        case TURN_STAGE11:
            if ((nTarget = Stage11_GetGimmickToTarget(rPos)) == m_nTurnTarget) {
                Stage11_GetPosTurnAroundGimmick(rPos, nTarget, nDir);
            }
            break;
        case TURN_STAGE15:
            if ((nTarget = Stage15_GetGimmickToTarget(rPos)) == m_nTurnTarget) {
                Stage15_GetPosTurnAroundGimmick(rPos, nTarget, nDir);
            }
            break;
        case TURN_STAGE15_KARON:
            if ((nTarget = Stage15_GetGimmickToTargetKaron(rPos)) == m_nTurnTarget) {
                Stage15_GetPosTurnAroundGimmickKaron(rPos, nTarget, nDir);
            }
            break;
        case TURN_NONE:
            m_nTurnTarget = -1;
            break;
        case TURN_CHR:
            if ((nTarget = StageXX_GetChrToTarget(rPos)) == m_nTurnTarget) {
                StageXX_GetPosTurnAroundChr(rPos, nTarget, nDir);
            }
            break;
        case TURN_ITEM:
            if ((nTarget = StageXX_GetItemToTarget(rPos)) == m_nTurnTarget) {
                StageXX_GetPosTurnAroundItem(rPos, nTarget, nDir);
            }
            break;
        }
        if (nTarget != m_nTurnTarget) {
            m_nTurnKind = TURN_CHANGED;
        }
    }
    return m_nTurnKind >= 0;
}

// (guessed helper; inline in the original) Whether the driver goes round
// pChr: not while it runs for the ball, nor round a holder of the other
// team or a character that dunks, lays up or is down.
inline int CComDrvBasketGame::IsTurnAroundChr(const CChrCore *pChr) const
{
    if (GetRootActType() == ACT_GET_BALL) {
        return 0;
    }
    if (pChr->IsHoldBall() && pChr->GetTeamNo() != m_pChr->GetTeamNo()) {
        return 0;
    }
    long nState = pChr->GetState();
    if ((nState >= 0x26 && nState <= 0x28) || (nState >= 0x30 && nState <= 0x31) || nState == 0x24 ||
        nState == 0x36 || nState == 0xC) {
        return 0;
    }
    return 1;
}

// The nearest character in the way to rPos, or -1.
long CComDrvBasketGame::StageXX_GetChrToTarget(const bcs::FVEC3 &rPos)
{
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable() || i == m_nMyChrIdx) {
            continue;
        }
        if (!IsTurnAroundChr(pChr)) {
            continue;
        }
        bcs::FVEC3 pos = CComMan::GetChrWorldPos(pChr);
        unsigned long nBit = 0;
        CComMan::SetEnableBit(nBit, i);
        if (CComMan::IsExistChrToTargetWorld(m_pChr, target, 1.2f, 2.5f, nBit, 0)) {
            float fDis = GetDisXZ(m_MyWorldPos, pos);
            pSort->m_nId = i;
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return -1;
}

// The position to go round character nChr on the way to rPos (nDir: the
// side, as returned before; -1 for a new one). When the driver already
// stands too near, rPos is put just outside of it instead (-1).
long CComDrvBasketGame::StageXX_GetPosTurnAroundChr(bcs::FVEC3 &rPos, long nChr, long nDir) const
{
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    float fRange = 0.5f + (pChr->GetCollSize_BodyPush() + m_pChr->GetCollSize_BodyPush());
    const bcs::FVEC3 &rChrPos = pChr->GetPos();
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(rChrPos, world) <= fRange) {
        bcs::FVEC3 dir = Diff(GetPosC(m_pChr), rChrPos);
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + rChrPos.x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + rChrPos.z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    if (!IsTurnAroundChr(pChr)) {
        return -1;
    }
    bcs::FVEC3 pos = CComMan::GetChrWorldPos(pChr);
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 3: the nearest water pillar (its task id) in the way to rPos, or 0.
unsigned long CComDrvBasketGame::Stage03_GetGimmickToTarget(const bcs::FVEC3 &rPos)
{
    if (CGame::g_GameWork.GetStage() != 3) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CStageBasket03::GetWaterPillarMaxNum(); i++) {
        CStgGmk_WaterPillar *pPillar = CStageBasket03::GetWaterPillar(i);
        if (pPillar == 0) {
            continue;
        }
        if (CComMan::IsPointInFaceXZ(pPillar->GetPos(), m_pChr->GetPos(), target, 0.7f, 2.5f)) {
            CChrCore *pChr = m_pChr;
            float fDis = GetDisXZ(pChr->GetPos(), pPillar->GetPos());
            pSort->m_nId = i;
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return CStageBasket03::GetWaterPillar(aSort[0].m_nId)->GetId();
    }
    return 0;
}

// Stage 3: the position to go round water pillar nId.
long CComDrvBasketGame::Stage03_GetPosTurnAroundGimmick(bcs::FVEC3 &rPos, unsigned long nId, long nDir)
{
    if (CGame::g_GameWork.GetStage() != 3) {
        return -1;
    }
    CStgGmk_WaterPillar *pPillar = static_cast<CStgGmk_WaterPillar *>(bcs::CTaskMan::GetTask(nId));
    if (pPillar == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pPillar->GetPos();
    float fRange = 1.0f + m_pChr->GetCollSize_BodyDamage();
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pPillar->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(m_pChr->GetPos(), pPillar->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pPillar->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pPillar->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 7: the nearest exploding mud bomb (its task id) in the way to rPos,
// or 0.
unsigned long CComDrvBasketGame::Stage07_GetGimmickToTarget(const bcs::FVEC3 &rPos)
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    long nNum = 0;
    const CNpcBossPackun::CMudBomb *pBomb = CComManBasketGame::GetMudFirst();
    SortBase::SORT *pSort = aSort;
    for (; pBomb != 0; pBomb = CComManBasketGame::GetMudNext(pBomb)) {
        if (!pBomb->IsNowExplode()) {
            continue;
        }
        if (CComMan::IsPointInFaceXZ(pBomb->GetPos(), m_pChr->GetPos(), target, 1.7f, 4.0f)) {
            CChrCore *pChr = m_pChr;
            float fDis = GetDisXZ(pChr->GetPos(), pBomb->GetPos());
            pSort->m_nId = pBomb->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// Stage 7: the position to go round mud bomb nId.
long CComDrvBasketGame::Stage07_GetPosTurnAroundGimmick(bcs::FVEC3 &rPos, unsigned long nId, long nDir)
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return -1;
    }
    CNpcBossPackun::CMudBomb *pBomb = CComManBasketGame::GetMudPtr(nId);
    if (pBomb == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pBomb->GetPos();
    float fRange = 0.8f + (m_pChr->GetCollSize_BodyDamage() + pBomb->GetAtkCollRadius());
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pBomb->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(m_pChr->GetPos(), pBomb->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pBomb->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pBomb->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 9: the nearest arrow panel not pointing to the goal the team
// attacks (its task id) in the way to rPos, or 0.
unsigned long CComDrvBasketGame::Stage09_GetGimmickToTarget(const bcs::FVEC3 &rPos)
{
    if (CGame::g_GameWork.GetStage() != 9) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    long nNum = 0;
    long nTeam = m_pChr->GetTeamNo();
    CQPanel *pPanel = CComManBasketGame::ArrowPanel_GetFirst();
    SortBase::SORT *pSort = aSort;
    for (; pPanel != 0; pPanel = CComManBasketGame::ArrowPanel_GetNext(pPanel)) {
        if (CComManBasketGame::IsTeamTargetDirArrowPanel(nTeam, pPanel)) {
            continue;
        }
        if (CComMan::IsPointInFaceXZ(pPanel->GetPos(), m_pChr->GetPos(), target, 2.2f, 3.0f)) {
            CChrCore *pChr = m_pChr;
            float fDis = GetDisXZ(pChr->GetPos(), pPanel->GetPos());
            pSort->m_nId = pPanel->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// Stage 9: the position to go round arrow panel nId.
long CComDrvBasketGame::Stage09_GetPosTurnAroundGimmick(bcs::FVEC3 &rPos, unsigned long nId, long nDir)
{
    if (CGame::g_GameWork.GetStage() != 9) {
        return -1;
    }
    CQPanel *pPanel = static_cast<CQPanel *>(bcs::CTaskMan::GetTask(nId));
    if (pPanel == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pPanel->GetPos();
    float fRange = 2.0f + m_pChr->GetCollSize_BodyDamage();
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pPanel->GetPos(), world) <= fRange) {
        if (GetRootActType() == ACT_GET_BALL) {
            return nDir;
        }
        bcs::FVEC3 dir = Diff(m_pChr->GetPos(), pPanel->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pPanel->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pPanel->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 11: the nearest obstacle of kind 15 (its task id) in the way to
// rPos, or 0.
unsigned long CComDrvBasketGame::Stage11_GetGimmickToTarget(const bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 11) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    long nNum = 0;
    CChrObstacle *pObstacle = CComMan::ObstacleSearch(0);
    SortBase::SORT *pSort = aSort;
    for (; pObstacle != 0; pObstacle = CComMan::ObstacleSearch(pObstacle)) {
        if (pObstacle->GetObstacleType() != 15) {
            continue;
        }
        const bcs::FVEC3 &rObsPos = pObstacle->GetPos();
        if (CComMan::IsPointInFaceXZ(rObsPos, GetPosC(m_pChr), target, 1.2f, 2.5f)) {
            float fDis = GetDisXZ(GetPosC(m_pChr), rObsPos);
            pSort->m_nId = pObstacle->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// Stage 11: the position to go round obstacle nId.
long CComDrvBasketGame::Stage11_GetPosTurnAroundGimmick(bcs::FVEC3 &rPos, unsigned long nId, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 11) {
        return -1;
    }
    CChrObstacle *pObstacle = static_cast<CChrObstacle *>(bcs::CTaskMan::GetTask(nId));
    if (pObstacle == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pObstacle->GetPos();
    float fRange = 1.2f + pObstacle->GetAttackRadius();
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pObstacle->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(GetPosC(m_pChr), pObstacle->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pObstacle->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pObstacle->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 15: the nearest case (its task id) in the way to rPos, or 0.
unsigned long CComDrvBasketGame::Stage15_GetGimmickToTarget(const bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CComManBasketGame::GetGmkCaseNum(); i++) {
        const CStgGmk_Case *pCase = CComManBasketGame::GetGmkCase(i);
        if (pCase == 0) {
            continue;
        }
        bcs::FVEC3 pos = pCase->GetPos();
        if (CComMan::IsPointInFaceXZ(pos, GetPosC(m_pChr), target, 2.0f, 4.0f)) {
            float fDis = GetDisXZ(GetPosC(m_pChr), pos);
            pSort->m_nId = pCase->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// Stage 15: the position to go round case nId; near the side lines of the
// court the case is gone round on the court's side.
long CComDrvBasketGame::Stage15_GetPosTurnAroundGimmick(bcs::FVEC3 &rPos, unsigned long nId, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }
    const CStgGmk_Case *pCase = CComManBasketGame::GetGmkCasePtr(nId);
    if (pCase == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pCase->GetPos();
    float fRange = pCase->GetWidth();
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pCase->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(GetPosC(m_pChr), pCase->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pCase->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pCase->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    float fSide = fabsf(CStageMan::GetInstance()->GetCourtLeft());
    if (fSide - 2.0f < fabsf(GetPosC(m_pChr).x)) {
        float fRot = RoundRadPI(m_pChr->GetRotY_ToPosXZ(target));
        if (rPos.x > 0.0f) {
            nDir = fabsf(fRot) < PI / 2 ? 0 : 1;
        } else {
            nDir = !(fabsf(fRot) > PI / 2);
        }
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// Stage 15: the nearest obstacle of kind 16 (the dry bones; its task id)
// in the way to rPos, or 0.
unsigned long CComDrvBasketGame::Stage15_GetGimmickToTargetKaron(const bcs::FVEC3 &rPos) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return 0;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    SortBase::SORT aSort[32];
    long nNum = 0;
    CChrObstacle *pObstacle = CComMan::ObstacleSearch(0);
    SortBase::SORT *pSort = aSort;
    for (; pObstacle != 0; pObstacle = CComMan::ObstacleSearch(pObstacle)) {
        if (pObstacle->GetObstacleType() != 16) {
            continue;
        }
        const bcs::FVEC3 &rObsPos = pObstacle->GetPos();
        if (CComMan::IsPointInFaceXZ(rObsPos, GetPosC(m_pChr), target, 1.2f, 2.5f)) {
            float fDis = GetDisXZ(GetPosC(m_pChr), rObsPos);
            pSort->m_nId = pObstacle->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// Stage 15: the position to go round the dry bones nId.
long CComDrvBasketGame::Stage15_GetPosTurnAroundGimmickKaron(bcs::FVEC3 &rPos, unsigned long nId, long nDir) const
{
    if (CGame::g_GameWork.GetStage() != 15) {
        return -1;
    }
    CChrObstacle *pObstacle = static_cast<CChrObstacle *>(bcs::CTaskMan::GetTask(nId));
    if (pObstacle == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pObstacle->GetPos();
    float fRange = 2.2f;
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pObstacle->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(GetPosC(m_pChr), pObstacle->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pObstacle->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pObstacle->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// The nearest thrown shell or banana (its task id) on the way to rPos, or
// 0; one still flying counts where it will land, if it lands soon.
unsigned long CComDrvBasketGame::StageXX_GetItemToTarget(const bcs::FVEC3 &rPos) const
{
    SortBase::SORT aSort[32];
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return 0;
    }
    CItem *pItem = CComMan::ThrowItem_GetFirst();
    SortBase::SORT *pSort = aSort;
    for (; pItem != 0; pItem = CComMan::ThrowItem_GetNext(pItem)) {
        long nType = pItem->GetItemType();
        if (nType != 2 && nType != 5) {
            continue;
        }
        bcs::FVEC3 pos;
        if (!pItem->IsTouchDown()) {
            if (pItem->GetTouchDownFrame() > 60.0f) {
                continue;
            }
            if (!pItem->GetThrowTargetPos(pos)) {
                continue;
            }
        } else {
            pos = pItem->GetPos();
        }
        float fWidth = 1.0f + m_pChr->GetCollSize_BodyDamage();
        if (CComMan::IsPointInFaceXZ(pos, m_MyWorldPos, target, fWidth, 0.5f + fWidth)) {
            float fDis = GetDisXZ(m_MyWorldPos, pos);
            pSort->m_nId = pItem->GetId();
            nNum++;
            pSort->m_fParam = fDis;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum > 0) {
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        return aSort[0].m_nId;
    }
    return 0;
}

// The position to go round item nItem.
long CComDrvBasketGame::StageXX_GetPosTurnAroundItem(bcs::FVEC3 &rPos, unsigned long nItem, long nDir) const
{
    CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(nItem));
    if (pItem == 0) {
        return -1;
    }
    bcs::FVEC3 target;
    if (GetWorldTargetDis(target, rPos, 3.0f) < 0.3f) {
        return -1;
    }
    bcs::FVEC3 pos = pItem->GetPos();
    float fRange = 1.0f + m_pChr->GetCollSize_BodyDamage();
    if (!pItem->IsTouchDown() && !pItem->GetThrowTargetPos(pos)) {
        return -1;
    }
    bcs::FVEC3 world = CComMan::Local2World(rPos, -1);
    if (DistXZ(pItem->GetPos(), world) <= fRange) {
        bcs::FVEC3 dir = Diff(GetPosC(m_pChr), pItem->GetPos());
        NormalizeXZ(dir, dir);
        rPos.x = dir.x * fRange + pItem->GetPos().x;
        rPos.y = 0.0f;
        rPos.z = dir.z * fRange + pItem->GetPos().z;
        rPos = CComMan::World2Local(rPos, -1);
        m_nUnk1E8 = 1;
        return -1;
    }
    return CComMan::GetPosTurnAroundGimmick(rPos, m_MyWorldPos, target, pos, fRange, nDir);
}

// The side to go round a character on: nDir unless the driver is near the
// edge of the court, else the side with fewer characters (0 or 1).
long CComDrvBasketGame::GetTurnAroundRotDirLotOfOneCore(long nDir, const bcs::FVEC3 &rPos) const
{
    static const float fDist = 2.5f;
    float fLeft, fRight, fFront, fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);
    fLeft += fDist;
    fRight -= fDist;
    fFront += fDist;
    fBack -= fDist;
    bcs::FVEC3 my = GetPosC(m_pChr);
    if (!(my.x <= fLeft || my.x >= fRight || my.z <= fFront || my.z >= fBack)) {
        return nDir;
    }
    const CChrCore *pMy = m_pChr;
    float fRot = pMy->GetRotY_ToPosXZ(CComMan::Local2World(rPos, -1));
    long nNum = 0;
    long nLeft = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr == 0 || pChr->IsDisable()) {
            continue;
        }
        const CChrCore *pMe = m_pChr;
        if (pChr == pMe) {
            continue;
        }
        nNum++;
        if (RoundRadPI(pMe->GetRotY_ToPosXZ(pChr->GetPos()) - fRot) > 0.0f) {
            nLeft++;
        }
    }
    return nLeft < nNum / 2;
}

} // namespace msw


