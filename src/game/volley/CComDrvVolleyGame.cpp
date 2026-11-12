// msw::CComDrvVolleyGame: the computer driver of the normal volleyball game.
// Constructor, positions (home, defence, symmetry, serve, toss, spike,
// block), the rate draws (Lot*), the checks the think functions ask
// (Shoud*, Is*), the action table and the vtable. The actions themselves
// are in game/volley/unit_9FDD0.cpp, the frame and message handlers in
// game/volley/unit_AA504.cpp (each file has its own float constants).
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManVolley.hpp"
#include "game/msw/CComManVolleyGame.hpp"
#include "game/msw/CComOSG.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItemBallVolleyKinds.hpp"
#include "game/msw/CStageVolley05.hpp"
#include "game/msw/CStageVolley06.hpp"
#include "game/msw/CTournament.hpp"
#include "nw4r/math/triangular.h"
#include <math.h>

// (guessed) v kept between lo and hi; the bounds are evaluated again for
// the result, as the original calls the court edge getters twice.
#define CLAMP(v, lo, hi) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))

namespace {
    typedef bcs::CSimpleSortBase<32> SortBase;

    // (guessed helper) fVal kept between fMin and fMax.
    inline float Clamp(float fVal, float fMin, float fMax)
    {
        return fVal > fMax ? fMax : (fVal < fMin ? fMin : fVal);
    }

    // (guessed helper) rIn's direction in the XZ plane, length 1.
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
    }

    // (guessed helper) Whether a character of nPadBits stands closer than
    // fDist to rPos (world), counting its handicap distance.
    inline int IsExistPadChrNear(const msw::CComDrv *pDrv, unsigned long nPadBits, const bcs::FVEC3 &rPos,
                                 float fDist)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pChr = msw::CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && msw::CComMan::IsEnableBit(nPadBits, i)) {
                bcs::FVEC3 pos = msw::CComMan::GetChrWorldPos(pChr);
                if (pDrv->GetDisXZ(rPos, pos) + msw::CComManVolleyGame::GetConditionHandicapDis(pChr) < fDist) {
                    return 1;
                }
            }
        }
        return 0;
    }

    // (guessed helper) Whether the two courts are moved apart (6 along x or
    // 3 along z).
    inline int IsCourtMoved()
    {
        msw::CStageMan *pStage = msw::CStageMan::GetInstance();
        float fX0 = pStage->GetTeamCourtOffsetX(0);
        float fZ0 = pStage->GetTeamCourtOffsetZ(0);
        float fX1 = pStage->GetTeamCourtOffsetX(1);
        float fZ1 = pStage->GetTeamCourtOffsetZ(1);
        return fabsf(fX0 - fX1) >= 6.0f || fabsf(fZ0 - fZ1) >= 3.0f;
    }

    // (guessed helper) Whether rPos (world) is in the other team's court.
    inline bool IsInnerEmCourt(const msw::CComDrv *pDrv, const bcs::FVEC3 &rPos)
    {
        float fLeft = pDrv->GetEmCourtLeft();
        float fRight = pDrv->GetEmCourtRight();
        float fFront = pDrv->GetEmCourtFront();
        float fBack = pDrv->GetEmCourtBack();
        return msw::CComMan::IsInnerRect(fLeft, fRight, fFront, fBack, rPos);
    }

    // (guessed helper; meaning not known) Whether nTeam's defence keeps to
    // the front: under mission rule 0xD, or on stage 6 while the lights of
    // goal 0 are off.
    inline int IsDefFrontOnly(long nTeam)
    {
        if (msw::CTournament::IsMissionRuleEnable(0xD, nTeam)) {
            return 1;
        }
        if (msw::CGame::g_GameWork.GetStage() == 6) {
            msw::CStageVolley06 *pStage = static_cast<msw::CStageVolley06 *>(msw::CStageMan::GetInstance());
            if (pStage != 0 && !pStage->Illumi_IsOn(0)) {
                return 1;
            }
        }
        return 0;
    }
}

namespace msw {

CComDrvVolleyGame::ActFunc CComDrvVolleyGame::g_ActFuncMap[ACT_NUM] = {
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
    &CComDrvVolleyGame::ActLoopReceiveGo,
    &CComDrvVolleyGame::ActLoopReceiveSudden,
    &CComDrvVolleyGame::ActLoopDiveReceive,
    &CComDrvVolleyGame::ActLoopTossReady,
    &CComDrvVolleyGame::ActLoopTossGo,
    &CComDrvVolleyGame::ActLoopSpikeReady,
    &CComDrvVolleyGame::ActLoopSpikeGo,
    &CComDrvVolleyGame::ActLoopSpikeJump,
    &CComDrvVolleyGame::ActLoopBlockReady,
    &CComDrvVolleyGame::ActLoopBlockGo,
    &CComDrvVolleyGame::ActLoopServe,
    &CComDrvVolleyGame::ActLoopServeWait,
    &CComDrvVolleyGame::ActLoopGoHome,
    &CComDrvVolleyGame::ActLoopDefHome,
    &CComDrvVolleyGame::ActLoopSymmetry,
    &CComDrvVolleyGame::ActLoopGoAway,
    &CComDrvVolleyGame::ActLoopTargetMarkerGo,
    &CComDrvVolleyGame::ActLoopGetQPanel,
    &CComDrvVolleyGame::ActLoopGetDropCoin,
    &CComDrvVolleyGame::ActLoopItemShoot,
    &CComDrvVolleyGame::ActLoopInterThrowItem,
    &CComDrvVolleyGame::ActLoopEscGimmick,
    &CComDrvVolleyGame::ActLoopSpOffMoveMarker,
    &CComDrvVolleyGame::ActLoopSpDaisyDef,
    &CComDrvVolleyGame::ActLoopSpKoopaOff,
    &CComDrvVolleyGame::ActLoopSpKoopaDef,
    &CComDrvVolleyGame::ActLoopSpNinjaDef,
};

// The action work is at least 0x54 bytes, this driver's own need.
static inline unsigned long CalcWorkMaxSize(const CComDrv *pDrv)
{
    unsigned long nBase = pDrv->CalcWorkBaseMaxSize();
    unsigned long nSize = 0x54;
    if (nBase > nSize) {
        nSize = nBase;
    }
    return nSize;
}

CComDrvVolleyGame::CComDrvVolleyGame() : CComDrvVolley(CalcWorkMaxSize(this))
{
}

CComDrvVolleyGame::~CComDrvVolleyGame()
{
}

void CComDrvVolleyGame::ClearAll()
{
    CComDrvVolley::ClearAll();
    for (int i = 0; i < 4; i++) {
        m_aHomePos[i].x = 0.0f;
        m_aHomePos[i].y = 0.0f;
        m_aHomePos[i].z = 0.0f;
    }
    m_nAPos = 0;
    m_nFrontChrIdx = 0;
    m_nBackChrIdx = 0;
    m_nUnk1AC = 0;
    m_nUnk1AD = 0;
    m_nSpikeCurve = 1;
    m_nUnk1AF = 0;
    m_nUnk1B0 = 0;
    m_bEarlyDef = 0;
    m_fEarlyDefTime = 0.0f;
    m_nDefPosKind = 0;
    m_nUnk1B9 = 0;
    m_fHomeOfsX = 0.0f;
    m_fHomeOfsZ = 0.0f;
    m_fUnk1C4 = 0.0f;
    m_fUnk1C8 = 0.0f;
    m_bTouchGo = 0;
    m_nUnk1CD = 0;
    m_nUnk1CE = 0;
    m_nUnk1CF = 0;
    m_bTwoAttack = 0;
    m_nTouchCount = 0;
    m_nUnk1D8 = -1;
    m_bEscBomb = 0;
    m_bEscItem = 0;
    m_bEscObstacle = 0;
    m_bSpObstacleSide = 0;
    m_nUnk1DD = -1;
    m_Unk1E0.x = 0.0f;
    m_Unk1E0.y = 0.0f;
    m_Unk1E0.z = 0.0f;
    m_bSpBefore = 0;
    m_nUnk1F0 = 0;
    m_fUnk1F4 = 0.0f;
    m_bGimmick = 1;
}

void CComDrvVolleyGame::UpdateInfo()
{
    CComDrvVolley::UpdateInfo();
    SetComLv();
}

// Keeps rPos on the stage, off the other court, and short of walls.
void CComDrvVolleyGame::NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const
{
    long nOther;
    rPos.y = 0.0f;
    long nCourt = CComMan::GetCourtIdxByWorldPos(rPos);
    nOther = 1 - nCourt;
    float fLeft = CComMan::GetStageOuterWorldLeft(nOther);
    float fRight = CComMan::GetStageOuterWorldRight(nOther);
    float fFront = CComMan::GetStageOuterWorldFront(nOther);
    float fBack = CComMan::GetStageOuterWorldBack(nOther);
    float fMargin = 0.01f + m_pChr->GetUnk26C();
    if (nCourt == 0) {
        fFront = CComMan::GetStageCourtWorldFront(nCourt);
    }
    if (nCourt != 0) {
        fBack = CComMan::GetStageCourtWorldBack(nCourt);
    }
    fLeft += fMargin;
    fRight -= fMargin;
    fFront += fMargin;
    fBack -= fMargin;
    rPos.x = Clamp(rPos.x, fLeft, fRight);
    rPos.z = Clamp(rPos.z, fFront, fBack);

    bcs::FVEC3 hit;
    if (m_pChr->CheckMapHit(hit, m_MyWorldPos, rPos)) {
        rPos = hit;
    }
}

// Whether the ball is the driver's team's to play: its own side, or the
// ball of Daisy's (character 6) special shot of the driver's team while it
// is still in the air.
int CComDrvVolleyGame::IsMyTurn() const
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp != 0 && pSp->GetState() == CChrCore::STATE_AIR_TRICK && pSp->GetChrTypeID() == 6 &&
        pSp->IsVolleyTargetMarkerActive()) {
        if (pSp->GetTeamNo() == m_nMyTeam) {
            return 1;
        }
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall != 0 && pBall->IsActive() && pBall->IsArtsShootMode()) {
                return 1;
            }
        }
        return 0;
    }
    if (m_nMyTeam == CGame::g_GameWork.GetUserTeam()) {
        return 1;
    }
    return 0;
}

// The computer level: the other team's when a human plays in the driver's
// team, else its own team's, raised a little while the team is five points
// or more behind in the period.
void CComDrvVolleyGame::SetComLv()
{
    if (IsExistPadDrv()) {
        float fLv = CGame::GetTeamComLevel(m_nEmTeam);
        if (CGame::g_GameWork.m_Unk151 != 0) {
            m_fComBaseLv = CComMan::GetDatF(8, 0L);
        } else {
            m_fComBaseLv = CComMan::GetDatF(0, fLv);
        }
        m_fComLv = m_fComBaseLv;
    } else {
        m_fComBaseLv = CGame::GetTeamComLevel(m_nMyTeam);
        m_fComLv = m_fComBaseLv;
        if (m_fComLv >= 3.0f) {
            long nPeriod = CGame::g_GameWork.m_nPeriod;
            long nEmScore = CGame::GetTeamPeriodScore(m_nEmTeam, nPeriod);
            if (nEmScore - CGame::GetTeamPeriodScore(m_nMyTeam, nPeriod) >= 5) {
                if (m_fComBaseLv < 3.5f) {
                    m_fComLv = 0.3f + m_fComBaseLv;
                } else {
                    m_fComLv = 0.5f + m_fComBaseLv;
                }
            }
        }
    }
    m_fComBaseLv = Clamp(m_fComBaseLv, 0.0f, 7.0f);
    m_fComLv = Clamp(m_fComLv, 0.0f, 7.0f);
}

// Whether a special shot is in the air with its target marker still to be
// moved (or before its swing ends); for the other team's shot only while
// the ball is not in its arts shot yet.
int CComDrvVolleyGame::IsSpMarkerMode() const
{
    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return 0;
    }
    if (pSp->GetState() != CChrCore::STATE_AIR_TRICK) {
        return 0;
    }
    if (pSp->GetTeamNo() != m_nMyTeam && m_pBall != 0 && m_pBall->IsArtsShootMode()) {
        return 0;
    }
    if (pSp->CanbeMoveVolleyTargetMarkerPos()) {
        return 1;
    }
    return !pSp->IsVolleyAirTrickSwingEnd();
}

// The driver's team's characters that may touch the ball now, as bits.
unsigned long CComDrvVolleyGame::GetMyTeamCanTouchEnableBit() const
{
    CBall *pBall = GetActiveBall();
    if (pBall == 0) {
        return 0;
    }
    long nBall = pBall->GetBallNo();
    unsigned long nBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() &&
            CComManVolley::CanbeTouch(pChr, nBall)) {
            CComMan::SetEnableBit(nBits, i);
        }
    }
    return nBits;
}

// The same, only the characters a human plays.
unsigned long CComDrvVolleyGame::GetMyTeamPadDrvCanTouchEnableBit() const
{
    CBall *pBall = GetActiveBall();
    if (pBall == 0) {
        return 0;
    }
    long nBall = pBall->GetBallNo();
    unsigned long nBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && pChr->IsPadCtrl() &&
            CComManVolley::CanbeTouch(pChr, nBall)) {
            CComMan::SetEnableBit(nBits, i);
        }
    }
    return nBits;
}

// Decides who of the team plays in front and who behind: the one nearest
// to the net in front, the one farthest from it behind. With a human in the
// team its characters keep the place they stand in.
void CComDrvVolleyGame::UpdateAPosition()
{
    if (m_nMateNum == 0) {
        m_nFrontChrIdx = m_nMyChrIdx;
        m_nBackChrIdx = m_nMyChrIdx;
        m_nAPos = 0;
        return;
    }

    m_nFrontChrIdx = -1;
    m_nBackChrIdx = -1;
    float fHalf = 0.5f * GetMyCourtSizeZ();
    float fBorder = CComMan::GetCourtBorderZ(m_MyPos);
    if (IsExistPadDrv()) {
        if (m_nMateNum == 1) {
            if (fabsf(m_aMatePos[0].z - fBorder) < fHalf) {
                m_nFrontChrIdx = m_nMateChrIdx;
                m_nBackChrIdx = m_nMyChrIdx;
            } else {
                m_nFrontChrIdx = m_nMyChrIdx;
                m_nBackChrIdx = m_nMateChrIdx;
            }
        } else {
            SortBase::SORT aSort[32];
            SortBase::SORT *pSort = aSort;
            long nNum = 0;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && pChr->IsPadCtrl()) {
                    bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
                    nNum++;
                    pSort->m_nId = pChr->GetPlayerNo();
                    pSort->m_fParam = fabsf(pos.z - fBorder);
                    pSort++;
                }
            }
            if (nNum > 0) {
                if (nNum != 1) {
                    bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
                }
                if (aSort[0].m_fParam < fHalf) {
                    m_nFrontChrIdx = aSort[0].m_nId;
                }
                if (aSort[nNum - 1].m_fParam >= fHalf) {
                    m_nBackChrIdx = aSort[nNum - 1].m_nId;
                }
            }

            pSort = aSort;
            nNum = 0;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && i != m_nFrontChrIdx &&
                    i != m_nBackChrIdx) {
                    bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
                    nNum++;
                    pSort->m_nId = pChr->GetPlayerNo();
                    pSort->m_fParam = fabsf(pos.z - fBorder);
                    pSort++;
                }
            }
            if (nNum <= 0) {
                return;
            }
            if (nNum != 1) {
                bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
            }
            if (m_nFrontChrIdx < 0) {
                m_nFrontChrIdx = aSort[0].m_nId;
            }
            if (m_nBackChrIdx < 0) {
                m_nBackChrIdx = aSort[nNum - 1].m_nId;
            }
        }
    } else {
        SortBase::SORT aSort[32];
        SortBase::SORT *pSort = aSort;
        long nNum = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo()) {
                bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
                nNum++;
                pSort->m_nId = pChr->GetPlayerNo();
                pSort->m_fParam = fabsf(pos.z - fBorder);
                pSort++;
            }
        }
        if (nNum <= 0) {
            return;
        }
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
        }
        m_nFrontChrIdx = aSort[0].m_nId;
        m_nBackChrIdx = aSort[nNum - 1].m_nId;
    }

    if (m_nFrontChrIdx == m_nMyChrIdx) {
        m_nAPos = 0;
    } else if (m_nBackChrIdx == m_nMyChrIdx) {
        m_nAPos = 2;
    } else {
        m_nAPos = 1;
    }
}

// The four home positions of the court: two in front, two behind; with
// three characters (m_nMateNum 1 means two) the front ones further apart.
void CComDrvVolleyGame::UpdateHomePos()
{
    long nSide = m_nUnk8C;
    float fSizeX = GetMyCourtSizeX();
    float fSizeZ = GetMyCourtSizeZ();
    float fFront = fSizeZ / 3.0f;
    float fX = fSizeX / 2.3f;
    float fBack = fSizeZ / 2.7f;
    float fBorder = CComMan::GetCourtBorderZ(m_MyPos);
    if (nSide != 0) {
        fFront = -fFront;
        fBack = -fBack;
    }
    if (m_nMateNum == 1) {
        float fZ0 = fBorder + fBack;
        float fZ1 = fFront + fZ0;
        m_aHomePos[0].Set(-fX, 0.0f, fZ0);
        m_aHomePos[1].Set(fX, 0.0f, fZ0);
        m_aHomePos[2].Set(-fX, 0.0f, fZ1);
        m_aHomePos[3].Set(fX, 0.0f, fZ1);
    } else {
        float fZ0 = fBorder + fBack;
        float fZ1 = fFront + fZ0;
        m_aHomePos[0].Set(1.1f * -fX, 0.0f, fZ0);
        m_aHomePos[1].Set(1.1f * fX, 0.0f, fZ0);
        m_aHomePos[2].Set(1.2f * -fX, 0.0f, fZ1);
        m_aHomePos[3].Set(1.2f * fX, 0.0f, fZ1);
    }
}

// The driver's home position: one of the four, by its place (front or
// back) and where its team mates stand; bOfs adds the defence offset.
void CComDrvVolleyGame::GetHomePos(bcs::FVEC3 &rPos, int bOfs) const
{
    long nIdx = 0;
    bcs::FVEC3 frontPos = CComMan::GetChrPos(m_nFrontChrIdx);
    bcs::FVEC3 backPos = CComMan::GetChrPos(m_nBackChrIdx);
    float fCenterX = GetMyCourtCenterX();
    float fHalf = 0.5f * GetMyCourtSizeZ();
    float fBorder = CComMan::GetCourtBorderZ(m_MyPos);
    if (IsExistPadDrv()) {
        long bPad0 = IsExistPadChrHomePosIdx(0);
        long bPad1 = IsExistPadChrHomePosIdx(1);
        long bPad2 = IsExistPadChrHomePosIdx(2);
        long bPad3 = IsExistPadChrHomePosIdx(3);
        long nFront = -1;
        long nBack = -1;

        // The front character's home (0 left, 1 right).
        if (CComMan::IsPadChr(m_nFrontChrIdx)) {
            if (frontPos.x >= fCenterX) {
                nFront = 1;
            } else {
                nFront = 0;
            }
        } else if (bPad0 == 0 && bPad1 == 0) {
            if (bPad2 != 0 && bPad3 == 0) {
                nFront = 1;
            } else if (bPad3 != 0 && bPad2 == 0) {
                nFront = 0;
            }
        } else if (bPad0 != 0) {
            nFront = 1;
        } else if (bPad1 != 0) {
            nFront = 0;
        }
        if (nFront < 0) {
            nFront = backPos.x < frontPos.x;
        }

        // The back character's home (2 left, 3 right).
        if (CComMan::IsPadChr(m_nBackChrIdx)) {
            if (backPos.x >= fCenterX) {
                nBack = 3;
            } else {
                nBack = 2;
            }
        } else if (bPad2 == 0 && bPad3 == 0) {
            if (bPad0 != 0 && bPad1 == 0) {
                nBack = 3;
            } else if (bPad1 != 0 && bPad0 == 0) {
                nBack = 2;
            }
        } else if (bPad2 != 0) {
            nBack = 3;
        } else if (bPad3 != 0) {
            nBack = 2;
        }
        if (nBack < 0) {
            if (frontPos.x < backPos.x) {
                nBack = 3;
            } else {
                nBack = 2;
            }
        }

        if (m_nAPos == 0) {
            nIdx = nFront;
        } else if (m_nAPos == 2) {
            nIdx = nBack;
        } else if (nFront == 0 && nBack == 2) {
            if (fabsf(m_MyPos.z - fBorder) <= fHalf) {
                nIdx = 1;
            } else {
                nIdx = 3;
            }
        } else if (nFront == 0 && nBack == 3) {
            if (m_MyPos.x >= fCenterX) {
                nIdx = 1;
            } else {
                nIdx = 2;
            }
        } else if (nFront == 1 && nBack == 2) {
            if (m_MyPos.x >= fCenterX) {
                nIdx = 3;
            } else {
                nIdx = 0;
            }
        } else if (nFront == 1 && nBack == 3) {
            if (fabsf(m_MyPos.z - fBorder) <= fHalf) {
                nIdx = 0;
            } else {
                nIdx = 2;
            }
        }
    } else {
        switch (m_nAPos) {
            case 0:
                if (backPos.x < frontPos.x) {
                    nIdx = 1;
                } else {
                    nIdx = 0;
                }
                break;
            case 2:
                if (frontPos.x < backPos.x) {
                    nIdx = 3;
                } else {
                    nIdx = 2;
                }
                break;
            case 1:
                if (frontPos.x < backPos.x) {
                    if (m_MyPos.x >= fCenterX) {
                        nIdx = 1;
                    } else {
                        nIdx = 2;
                    }
                } else if (m_MyPos.x >= fCenterX) {
                    nIdx = 3;
                } else {
                    nIdx = 0;
                }
                break;
        }
    }

    rPos = m_aHomePos[nIdx];
    if (bOfs) {
        float fOfsX = m_fHomeOfsX;
        float fOfsZ = m_fHomeOfsZ;
        if (IsExistServeMode()) {
            fOfsX = Clamp(fOfsX, -2.5f, 2.5f);
            fOfsZ = Clamp(fOfsZ, -2.5f, 2.5f);
        }
        rPos.x += fOfsX;
        rPos.z += fOfsZ;
    }

    float fMargin = 0.65f;
    rPos.x = CLAMP(rPos.x, GetMyCourtParamLeft(fMargin), GetMyCourtParamRight(fMargin));
    rPos.z = CLAMP(rPos.z, GetMyCourtParamFront(fMargin), GetMyCourtParamBack(fMargin));
}

// Whether a character a human plays stands in the area of home position
// nIdx (0/1 front left/right, 2/3 back left/right).
int CComDrvVolleyGame::IsExistPadChrHomePosIdx(long nIdx) const
{
    float fCenterX = GetMyCourtCenterX();
    float fHalf = 0.5f * GetMyCourtSizeZ();
    float fBorder = CComMan::GetCourtBorderZ(m_MyPos);
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && pChr->IsPadCtrl()) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            switch (nIdx) {
                case 0:
                    if (pos.x <= fCenterX && fabsf(pos.z - fBorder) <= fHalf) {
                        return 1;
                    }
                    break;
                case 1:
                    if (pos.x >= fCenterX && fabsf(pos.z - fBorder) <= fHalf) {
                        return 1;
                    }
                    break;
                case 2:
                    if (pos.x <= fCenterX && fabsf(pos.z - fBorder) >= fHalf) {
                        return 1;
                    }
                    break;
                default:
                    if (pos.x >= fCenterX && fabsf(pos.z - fBorder) >= fHalf) {
                        return 1;
                    }
                    break;
            }
        }
    }
    return 0;
}

// The defence's position: the home position (with its offset), kept
// inside the court (closer to its edges against a curve spike); with
// m_nUnk1B9 set, only one axis moves, the one that is nearer.
void CComDrvVolleyGame::GetDefHomePos(bcs::FVEC3 &rPos) const
{
    GetHomePos(rPos, 1);
    float fMarginX = 0.65f;
    float fMarginZ = 0.65f;
    if (IsExistCurveSpikeEmChrCore()) {
        long nRate = CComMan::GetDatI(0x146, GetComLvF());
        if (LotRate() < nRate) {
            fMarginX = 0.4f;
        }
    }
    float fX;
    if (rPos.x > GetMyCourtParamRight(fMarginX)) {
        fX = GetMyCourtParamRight(fMarginX);
    } else if (rPos.x < GetMyCourtParamLeft(fMarginX)) {
        fX = GetMyCourtParamLeft(fMarginX);
    } else {
        fX = rPos.x;
    }
    rPos.x = fX;
    float fZ;
    if (rPos.z > GetMyCourtParamBack(fMarginZ)) {
        fZ = GetMyCourtParamBack(fMarginZ);
    } else if (rPos.z < GetMyCourtParamFront(fMarginZ)) {
        fZ = GetMyCourtParamFront(fMarginZ);
    } else {
        fZ = rPos.z;
    }
    rPos.z = fZ;
    if (m_nUnk1B9 != 0) {
        if (fabsf(rPos.x - m_MyPos.x) < fabsf(rPos.z - m_MyPos.z)) {
            rPos.x = m_MyPos.x;
        } else {
            rPos.z = m_MyPos.z;
        }
    }
}

// The position across from where the ball comes down: the ball's x, the
// home position's z, moved by the offset at 0x1C4.
void CComDrvVolleyGame::GetSymmetryPos(bcs::FVEC3 &rPos) const
{
    bcs::FVEC3 target;
    if (GetBallTargetPos(target)) {
        GetHomePos(rPos, 0);
        rPos.x = target.x;
        float fMarginX = 0.65f;
        float fMarginZ = 0.65f;
        rPos.x = target.x + m_fUnk1C4;
        rPos.z += m_fUnk1C8;
        if (IsExistCurveSpikeEmChrCore()) {
            long nRate = CComMan::GetDatI(0x146, GetComLvF());
            if (LotRate() < nRate) {
                fMarginX = 0.4f;
            }
        }
        float fX;
        if (rPos.x > GetMyCourtParamRight(fMarginX)) {
            fX = GetMyCourtParamRight(fMarginX);
        } else if (rPos.x < GetMyCourtParamLeft(fMarginX)) {
            fX = GetMyCourtParamLeft(fMarginX);
        } else {
            fX = rPos.x;
        }
        rPos.x = fX;
        float fZ;
        if (rPos.z > GetMyCourtParamBack(fMarginZ)) {
            fZ = GetMyCourtParamBack(fMarginZ);
        } else if (rPos.z < GetMyCourtParamFront(fMarginZ)) {
            fZ = GetMyCourtParamFront(fMarginZ);
        } else {
            fZ = rPos.z;
        }
        rPos.z = fZ;
        if (m_nUnk1B9 != 0) {
            if (fabsf(rPos.x - m_MyPos.x) < fabsf(rPos.z - m_MyPos.z)) {
                rPos.x = m_MyPos.x;
            } else {
                rPos.z = m_MyPos.z;
            }
        }
    } else {
        GetDefHomePos(rPos);
    }
}

// Whether a character of the other team that may touch the ball can play a
// curve spike.
int CComDrvVolleyGame::IsExistCurveSpikeEmChrCore() const
{
    if (IsMyTurn()) {
        return 0;
    }
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return 0;
    }
    CBall *pBall = CBallMan::GetBallPtr(nBall);
    if (pBall == 0) {
        return 0;
    }
    if (pBall->GetTouchNum() <= 0) {
        return 0;
    }
    if (!pBall->IsVolleyChargeBall()) {
        return 0;
    }
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam != pChr->GetTeamNo() &&
            CComManVolley::CanbeTouch(pChr, nBall) && pChr->IsVolleySuperSpikeCurveType()) {
            return 1;
        }
    }
    return 0;
}

// The character of the other team with a target marker nearest to where the
// ball comes down (or to the ball).
CChrCore *CComDrvVolleyGame::GetEmTargetMarkerChrCore() const
{
    SortBase::SORT aSort[32];
    bcs::FVEC3 ballPos = m_BallWorldPos;
    long nNum = 0;
    bcs::FVEC3 target;
    if (GetBallTargetPos(target)) {
        ballPos = Local2World(target, -1);
    }
    SortBase::SORT *pSort = aSort;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam != pChr->GetTeamNo()) {
            bcs::FVEC3 marker;
            if (pChr->GetTargetMarkerPos(marker)) {
                marker = CComMan::GetChrWorldPos(pChr);
                float fDis = CComMan::GetDisXZ(marker, ballPos);
                pSort->m_nId = i;
                nNum++;
                pSort->m_fParam = fDis;
                pSort++;
            }
        }
    }
    if (nNum <= 0) {
        return 0;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
    }
    return CChrMan::GetChrCore(aSort[0].m_nId);
}

// Where the target marker of that character is, in the court's frame.
int CComDrvVolleyGame::GetEmTargetMarkerPos(bcs::FVEC3 &rPos) const
{
    CChrCore *pChr = GetEmTargetMarkerChrCore();
    bcs::FVEC3 marker;
    if (pChr != 0 && pChr->GetTargetMarkerPos(marker)) {
        rPos = World2Local(marker, -1);
        return 1;
    }
    return 0;
}

// Which character of the driver's team should follow the ball to rPos: the
// nearest one that may touch it and is free (bAny 0: not blocking; else
// with no action at all).
long CComDrvVolleyGame::GetShoudFollowTargetChrIdx(const bcs::FVEC3 &rPos, int bAny) const
{
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return -1;
    }
    unsigned long nBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() &&
            CComManVolley::CanbeTouch(pChr, nBall) && !pChr->IsVolleyCombinationMode()) {
            if (m_nMateNum < 2 || pChr->GetState() == CChrCore::STATE_SPIKE) {
                long nAct = CComMan::GetRootActType(pChr);
                if (bAny == 0) {
                    if (nAct != ACT_BLOCK_READY && nAct != ACT_BLOCK_GO) {
                        CComMan::SetEnableBit(nBits, i);
                    }
                } else if (nAct == -1) {
                    CComMan::SetEnableBit(nBits, i);
                }
            }
        }
    }
    float fRate = 0.0f;
    if (m_nMateNum <= 1) {
        fRate = 1.0f;
    }
    return GetNearChrBaseWithCondition(rPos, nBits, fRate);
}

// An open space in the other team's court (bCorner: in one of its
// corners), away from its characters.
int CComDrvVolleyGame::GetOpenSpaceInEmCourt(bcs::FVEC3 &rPos, const bcs::FVEC3 *pFrom, int bCorner) const
{
    bcs::FVEC3 aPos[4];
    bcs::FVEC3 *pPos = aPos;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam != pChr->GetTeamNo()) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            nNum++;
            *pPos = pos;
            pPos++;
        }
    }
    if (bCorner == 0) {
        return Util_GetOpenSpace(rPos, aPos, nNum, m_nCourtIdx, pFrom, 1) != 0;
    }
    return Util_GetCornerOpenSpace(rPos, aPos, nNum, m_nCourtIdx, pFrom) != 0;
}

// The same in the driver's own court, away from its team mates.
int CComDrvVolleyGame::GetOpenSpaceInMyCourt(bcs::FVEC3 &rPos, const bcs::FVEC3 *pFrom, int bCorner) const
{
    bcs::FVEC3 aPos[4];
    bcs::FVEC3 *pPos = aPos;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo()) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            nNum++;
            *pPos = pos;
            pPos++;
        }
    }
    if (bCorner == 0) {
        return Util_GetOpenSpace(rPos, aPos, nNum, m_nUnk98, pFrom, 0) != 0;
    }
    return Util_GetCornerOpenSpace(rPos, aPos, nNum, m_nUnk98, pFrom) != 0;
}

// Whether rPos (world) is in the other team's court with none of its
// characters within fRange.
int CComDrvVolleyGame::IsOpenSpaceForSpike(const bcs::FVEC3 &rPos, float fRange) const
{
    if (!IsInnerEmCourt(this, rPos)) {
        return 0;
    }
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam != pChr->GetTeamNo()) {
            bcs::FVEC3 pos = CComMan::GetChrWorldPos(pChr);
            if (CComMan::GetDisXZ(pos, rPos) < fRange) {
                return 0;
            }
        }
    }
    return 1;
}

// Draws whether to go for the next touch quickly (0x1CE) and 0x1CD.
void CComDrvVolleyGame::LotRateTouch(long nTouch)
{
    long nRate;
    m_nUnk1CE = 0;
    if (IsMyTurn()) {
        nRate = CComMan::GetDatI(0x7E, GetComLvF());
    } else {
        nRate = CComMan::GetDatI(0x86, GetComLvF());
    }
    if (LotRate() < nRate) {
        m_nUnk1CE = 1;
    }
    m_nUnk1CD = 0;
    nRate = CComMan::GetDatI(0xF6, GetComLvF());
    if (LotRate() < nRate) {
        m_nUnk1CD = 1;
    }
}

void CComDrvVolleyGame::LotRateDefense(long nTouch)
{
    LotDefPosKind(nTouch);
}

// Draws the kind of defence position (m_nDefPosKind, 0-3; 2 and 3 not
// before the second touch) and its offset.
void CComDrvVolleyGame::LotDefPosKind(long nTouch)
{
    long nRate0 = CComMan::GetDatI(0x126, (long)(0.5f + GetComLvF()));
    long nRate1 = CComMan::GetDatI(0x12E, (long)(0.5f + GetComLvF()));
    long nRate2 = CComMan::GetDatI(0x136, (long)(0.5f + GetComLvF()));
    long nRate3 = CComMan::GetDatI(0x13E, (long)(0.5f + GetComLvF()));
    if (nTouch <= 1) {
        nRate2 = 0;
        nRate3 = 0;
    }
    if (m_nMateNum == 2 || IsDefFrontOnly(m_nMyTeam)) {
        nRate2 = 0;
        nRate3 = 0;
    }
    long anRate[4] = {nRate0, nRate1, nRate2, nRate3};
    m_nDefPosKind = LotProbability(anRate, 4);
    m_nUnk1B9 = 0;
    if (LotRate() < CComMan::GetDatI(0x14E, GetComLvF())) {
        m_nUnk1B9 = 1;
    }
    LotDefPosHosei();
}

// Draws the offset of the defence position for its kind: from the home
// position for kinds 0 and 1, from the symmetry position for 2 and 3.
void CComDrvVolleyGame::LotDefPosHosei()
{
    float fX = 0.0f;
    float fZ = 0.0f;
    switch (m_nDefPosKind) {
        case 0:
            fX = CComMan::GetDatF(0x156, GetComLvF());
            fZ = CComMan::GetDatF(0x15E, GetComLvF());
            break;
        case 1:
            fX = CComMan::GetDatF(0x166, GetComLvF());
            fZ = CComMan::GetDatF(0x16E, GetComLvF());
            break;
        case 2:
            fX = CComMan::GetDatF(0x176, GetComLvF());
            fZ = CComMan::GetDatF(0x17E, GetComLvF());
            break;
        case 3:
            fX = CComMan::GetDatF(0x186, GetComLvF());
            fZ = CComMan::GetDatF(0x18E, GetComLvF());
            break;
    }
    if (!IsExistPadDrv() && m_nMateNum == 2) {
        fX += 1.0f;
        fZ += 1.0f;
    }
    if (m_nDefPosKind == 0 || m_nDefPosKind == 1) {
        m_fHomeOfsX = RandRangeF(-fX, fX);
        m_fHomeOfsZ = RandRangeF(-fZ, fZ);
    } else {
        m_fUnk1C4 = RandRangeF(-fX, fX);
        m_fUnk1C8 = RandRangeF(-fZ, fZ);
    }
}

// Whether to block; less often on the first touches.
int CComDrvVolleyGame::LotRateBlock(long nTouch) const
{
    long nRate = CComMan::GetDatI(0xBE, GetComLvF());
    if (nTouch < 2) {
        nRate /= 3;
    }
    if (LotRate() < nRate) {
        return 1;
    }
    return 0;
}

// Draws the attack: m_nUnk1AC, m_nUnk1AD (three kinds each), the side to
// curve to, m_nUnk1AF, and m_nUnk1B0.
void CComDrvVolleyGame::LotRateOffense()
{
    long anRate[3];
    anRate[0] = CComMan::GetDatI(0x1DE, GetComLvF());
    anRate[1] = CComMan::GetDatI(0x1E6, GetComLvF());
    anRate[2] = CComMan::GetDatI(0x1EE, GetComLvF());
    m_nUnk1AC = LotProbability(anRate, 3);
    anRate[0] = CComMan::GetDatI(0x1F6, GetComLvF());
    anRate[1] = CComMan::GetDatI(0x1FE, GetComLvF());
    anRate[2] = CComMan::GetDatI(0x206, GetComLvF());
    m_nUnk1AD = LotProbability(anRate, 3);
    if (LotRate() < 50) {
        m_nSpikeCurve = 1;
    } else {
        m_nSpikeCurve = -1;
    }
    anRate[0] = CComMan::GetDatI(0x1AE, GetComLvF());
    anRate[1] = CComMan::GetDatI(0x1B6, GetComLvF());
    anRate[2] = CComMan::GetDatI(0x1BE, GetComLvF());
    m_nUnk1AF = LotProbability(anRate, 3);
    m_nUnk1B0 = 0;
    anRate[0] = CComMan::GetDatI(0x296, GetComLvF());
    if (LotRate() < anRate[0]) {
        m_nUnk1B0 = 1;
    }
}

// After the first touch: an open space of the other court seen from the
// driver's mirrored position (kept at 0x1E0, mirrored back).
void CComDrvVolleyGame::LotRateReceiveFinished()
{
    bcs::FVEC3 from(m_MyPos.x, m_MyPos.y, -m_MyPos.z);
    bcs::FVEC3 pos;
    if (GetOpenSpaceInEmCourt(pos, &from, 0)) {
        m_Unk1E0 = bcs::FVEC3(pos.x, pos.y, -pos.z);
    }
}

// Whether to attack on the second touch (m_bTwoAttack): more often when the
// place across the net from the driver is open.
void CComDrvVolleyGame::LotRateTwoAttack()
{
    bcs::FVEC3 mirror(m_MyPos.x, m_MyPos.y, -m_MyPos.z);
    bcs::FVEC3 pos = Local2World(mirror, -1);
    float fRange = CComMan::GetDatF(0x216, GetComLvF());
    long nRate;
    m_bTwoAttack = 0;
    if (IsOpenSpaceForSpike(pos, fRange)) {
        nRate = CComMan::GetDatI(0x266, GetComLvF());
    } else {
        nRate = CComMan::GetDatI(0x26E, GetComLvF());
    }
    if (LotRate() < nRate) {
        m_bTwoAttack = 1;
    }
}

// Whether to play a quick spike (0x1CF), the same way.
void CComDrvVolleyGame::LotRateQuickSpike()
{
    bcs::FVEC3 mirror(m_MyPos.x, m_MyPos.y, -m_MyPos.z);
    bcs::FVEC3 pos = Local2World(mirror, -1);
    float fRange = CComMan::GetDatF(0x216, GetComLvF());
    long nRate;
    m_nUnk1CF = 0;
    if (IsOpenSpaceForSpike(pos, fRange)) {
        nRate = CComMan::GetDatI(0x276, GetComLvF());
    } else {
        nRate = CComMan::GetDatI(0x27E, GetComLvF());
    }
    if (LotRate() < nRate) {
        m_nUnk1CF = 1;
    }
}

// Before the spike: which spike to play (m_nUnk1DD; 0 curve, 2 and 3 the
// feints, else a draw), when the place it would land is open.
void CComDrvVolleyGame::LotRateSpikeBefore()
{
    if (m_nUnk1DD != -1) {
        return;
    }
    long nRateCurve = CComMan::GetDatI(0x22E, GetComLvF());
    long nRateFeint0 = CComMan::GetDatI(0x236, GetComLvF());
    long nRateFeint1 = CComMan::GetDatI(0x23E, GetComLvF());
    if (IsDefFrontOnly(m_nMyTeam)) {
        nRateCurve = 0;
        nRateFeint0 = 0;
        nRateFeint1 = 0;
    }
    float fRangeCurve = CComMan::GetDatF(0x216, GetComLvF());
    float fRangeFeint0 = CComMan::GetDatF(0x21E, GetComLvF());
    float fRangeFeint1 = CComMan::GetDatF(0x226, GetComLvF());
    bcs::FVEC3 marker(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 feint0(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 feint1(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 feint2;
    if (!m_pChr->GetTargetMarkerPos(marker)) {
        nRateCurve = 0;
    }
    if (!m_pChr->GetVolleySpikeFeintTargetPos(0.0f, feint0, feint1, feint2)) {
        nRateFeint0 = 0;
        nRateFeint1 = 0;
    }

    if (LotRate() < nRateCurve) {
        if (m_pBall->IsVolleyChargeBall() && m_pChr->IsVolleySuperSpikeCurveType()) {
            // A curve spike: try both sides of the marker.
            float fDist = m_pChr->GetVolleySuperSpikeCurveMaxDist();
            bcs::FVEC3 aPos[2];
            aPos[0].Set(marker.x + fDist, marker.y, marker.z);
            aPos[1].Set(marker.x - fDist, marker.y, marker.z);
            long abOpen[2];
            abOpen[1] = 0;
            abOpen[0] = 0;
            for (long i = 0; i < 2; i++) {
                if (IsOpenSpaceForSpike(aPos[i], fRangeCurve)) {
                    abOpen[i] = 1;
                }
            }
            if (abOpen[0] && abOpen[1]) {
                m_nUnk1DD = 0;
                if (LotRate() < 50) {
                    m_nSpikeCurve = 1;
                } else {
                    m_nSpikeCurve = -1;
                }
            } else if (abOpen[0]) {
                m_nUnk1DD = 0;
                m_nSpikeCurve = 1;
            } else if (abOpen[1]) {
                m_nUnk1DD = 0;
                m_nSpikeCurve = -1;
            }
        } else if (IsOpenSpaceForSpike(marker, fRangeCurve)) {
            m_nUnk1DD = 0;
        }
    }

    if (m_nUnk1DD == -1) {
        if (LotRate() < nRateFeint0 && IsOpenSpaceForSpike(feint0, fRangeFeint0)) {
            m_nUnk1DD = 2;
            return;
        }
        if (LotRate() < nRateFeint1 && IsOpenSpaceForSpike(feint1, fRangeFeint1)) {
            m_nUnk1DD = 3;
            return;
        }
        long anRate[4];
        anRate[0] = CComMan::GetDatI(0x246, GetComLvF());
        anRate[1] = CComMan::GetDatI(0x24E, GetComLvF());
        anRate[2] = CComMan::GetDatI(0x256, GetComLvF());
        anRate[3] = CComMan::GetDatI(0x25E, GetComLvF());
        m_nUnk1DD = LotProbability(anRate, 4);
    }
}

// Whether to go for a question panel (good and bad luck).
int CComDrvVolleyGame::LotRateGetQPanelGood() const
{
    long nRate = CComMan::GetDatI(0x9E, GetComLvF());
    return LotRate() < nRate;
}

int CComDrvVolleyGame::LotRateGetQPanelBad() const
{
    long nRate = CComMan::GetDatI(0xA6, GetComLvF());
    return LotRate() < nRate;
}

// Whether to go for a dropped coin.
int CComDrvVolleyGame::LotRateGetDropCoinGood() const
{
    long nRate = CComMan::GetDatI(0xAE, GetComLvF());
    return LotRate() < nRate;
}

int CComDrvVolleyGame::LotRateGetDropCoinBad() const
{
    long nRate = CComMan::GetDatI(0xB6, GetComLvF());
    return LotRate() < nRate;
}

// Whether now is a good time to throw an item: never while the two courts
// are moved apart, nor with a human in the team.
int CComDrvVolleyGame::LotRateItemThrowTimingGood() const
{
    if (IsCourtMoved()) {
        return 0;
    }
    if (IsExistPadDrv()) {
        return 0;
    }
    long nRate = CComMan::GetDatI(0x56, GetComLvF());
    return LotRate() < nRate;
}

int CComDrvVolleyGame::LotRateItemThrowTimingBad() const
{
    if (IsCourtMoved()) {
        return 0;
    }
    if (IsExistPadDrv()) {
        return 0;
    }
    long nRate = CComMan::GetDatI(0x5E, GetComLvF());
    return LotRate() < nRate;
}

// When a special shot starts: how to answer it (m_nUnk1F0; characters 4
// and 12 always 1) and when (m_fUnk1F4, from the shooter's table).
void CComDrvVolleyGame::LotRateSpBefore()
{
    // The first data number of each character's special shot defence.
    static long g_SpDefChrTop[25] = {
        0xBBD, 0xBCD, 0xBDD, 0xBED, 0xBFD, 0xC0D, 0xC1D, 0xC2D, 0xC3D, 0xC4D, 0xC5D, 0xC6D, 0xC7D,
        0xC8D, 0xC9D, 0xCAD, 0xCBD, 0xCCD, 0xCDD, 0xCED, 0xCFD, 0xCED, 0xCFD, 0xCED, 0xCFD,
    };

    CChrCore *pSp = GetLastSpChrCore();
    if (pSp == 0) {
        return;
    }
    long nMyType = m_pChr->GetChrTypeID();
    long nSpType = pSp->GetChrTypeID();
    if (nMyType == 4 || nMyType == 12) {
        m_nUnk1F0 = 1;
    } else {
        long anRate[2];
        anRate[0] = CComMan::GetDatI(0x2B6, GetComLvF());
        anRate[1] = CComMan::GetDatI(0x2BE, GetComLvF());
        m_nUnk1F0 = LotProbability(anRate, 2);
        if (m_nUnk1F0 < 0) {
            m_nUnk1F0 = 1;
        }
    }
    long nTop = g_SpDefChrTop[nSpType];
    float fMin = CComMan::GetDatF(nTop, GetComLvF());
    m_fUnk1F4 = RandRangeF(fMin, CComMan::GetDatF(nTop + 8, GetComLvF()));
}

// Where to serve from: somewhere along the back line, a little behind it.
void CComDrvVolleyGame::GetServePos(bcs::FVEC3 &rPos) const
{
    float fRight = GetMyCourtParamRight(0.9f);
    rPos.x = bcs::BCS_RandRange(GetMyCourtParamLeft(0.9f), fRight);
    rPos.y = 0.0f;
    if (m_nUnk8C == 0) {
        float fOfs = bcs::BCS_RandRange(0.5f, 1.0f);
        rPos.z = GetMyCourtBack() + fOfs;
    } else {
        float fOfs = bcs::BCS_RandRange(0.5f, 1.0f);
        rPos.z = GetMyCourtFront() - fOfs;
    }
}

int CComDrvVolleyGame::GetServeTargetMarkerPos(bcs::FVEC3 &rPos) const
{
    return GetMoveTargetMarkerPos(rPos, m_nUnk1AC);
}

int CComDrvVolleyGame::GetSpikeTargetMarkerPos(bcs::FVEC3 &rPos) const
{
    return GetMoveTargetMarkerPos(rPos, m_nUnk1AD);
}

// Where to move a target marker in the other court, by kind: 0 where it
// is, 1 an open space, 2 near a human player, 3 the back line, 4 the front
// line, 5 around the middle, 6 a corner of the grid. On a charged ball with
// a curve spike, kept the curve's width from the side lines; on stage 5,
// moved to where the courts will be when it lands.
int CComDrvVolleyGame::GetMoveTargetMarkerPos(bcs::FVEC3 &rPos, long nKind) const
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 marker;
    if (!GetTargetMarkerPos(marker)) {
        return 0;
    }
    float fRight = GetEmCourtRight();
    rPos.x = RandRangeF(GetEmCourtLeft(), fRight);
    float fBack = GetEmCourtBack();
    rPos.z = RandRangeF(GetEmCourtFront(), fBack);
    float fRot = RandRangeF(0.0f, 6.2831855f);
    switch (nKind) {
        case 0:
            rPos = marker;
            break;
        case 1: {
            bcs::FVEC3 pos;
            if (GetOpenSpaceInEmCourt(pos, &marker, 0)) {
                rPos = pos;
            }
            break;
        }
        case 2: {
            CChrCore *pChr = CComMan::GetPadDrvChrCoreRand(m_nEmTeam);
            if (pChr != 0) {
                bcs::FVEC3 chrPos = CComMan::GetChrPosXZ(pChr);
                if ((long)(0.5f + GetComBaseLvF()) <= 3) {
                    if (chrPos.x < GetEmCourtCenterX()) {
                        float fCenter = GetEmCourtCenterX();
                        rPos.x = RandRangeF(GetEmCourtLeft(), fCenter);
                    } else {
                        float fRightX = GetEmCourtRight();
                        rPos.x = RandRangeF(GetEmCourtCenterX(), fRightX);
                    }
                    if (chrPos.z < GetEmCourtCenterZ()) {
                        float fCenter = GetEmCourtCenterZ();
                        rPos.z = RandRangeF(GetEmCourtFront(), fCenter);
                    } else {
                        float fBackZ = GetEmCourtBack();
                        rPos.z = RandRangeF(GetEmCourtCenterZ(), fBackZ);
                    }
                } else if (RandMaxI(1) == 0) {
                    if (chrPos.x < GetEmCourtCenterX()) {
                        float fCenter = GetEmCourtCenterX();
                        rPos.x = RandRangeF(GetEmCourtLeft(), fCenter);
                    } else {
                        float fRightX = GetEmCourtRight();
                        rPos.x = RandRangeF(GetEmCourtCenterX(), fRightX);
                    }
                } else if (chrPos.z < GetEmCourtCenterZ()) {
                    float fCenter = GetEmCourtCenterZ();
                    rPos.z = RandRangeF(GetEmCourtFront(), fCenter);
                } else {
                    float fBackZ = GetEmCourtBack();
                    rPos.z = RandRangeF(GetEmCourtCenterZ(), fBackZ);
                }
            }
            break;
        }
        case 3:
            if (m_nCourtIdx != 0) {
                float fOfs = RandRangeF(0.0f, 4.0f);
                rPos.z = GetEmCourtBack() - fOfs;
            } else {
                float fOfs = RandRangeF(0.0f, 4.0f);
                rPos.z = GetEmCourtFront() + fOfs;
            }
            break;
        case 4:
            if (m_nCourtIdx != 0) {
                float fOfs = RandRangeF(0.0f, 3.0f);
                rPos.z = GetEmCourtFront() + fOfs;
            } else {
                float fOfs = RandRangeF(0.0f, 3.0f);
                rPos.z = GetEmCourtBack() - fOfs;
            }
            break;
        case 5: {
            float fRadius = 3.5f;
            float fSin = nw4r::math::SinRad(fRot);
            float fCenterX = GetEmCourtCenterX();
            rPos.x = RandRangeF(0.0f, fRadius) * fSin + fCenterX;
            float fCos = nw4r::math::CosRad(fRot);
            float fCenterZ = GetEmCourtCenterZ();
            rPos.z = RandRangeF(0.0f, fRadius) * fCos + fCenterZ;
            break;
        }
        case 6: {
            CComOSG grid;
            long nX = 0;
            long nY = 0;
            grid.Init(5, 5, m_nCourtIdx);
            grid.FillFrontBack(1, -1);
            grid.FillLeftRight(1, -1);
            if (grid.SearchValue(nX, nY, 1, 1)) {
                grid.GetPos(rPos, nX, nY, 1);
            }
            break;
        }
    }
    rPos.x = CLAMP(rPos.x, GetEmCourtLeft(), GetEmCourtRight());
    rPos.z = CLAMP(rPos.z, GetEmCourtFront(), GetEmCourtBack());

    if (m_pBall->IsVolleyChargeBall() && m_pChr->IsVolleySuperSpikeCurveType() &&
        LotRate() < CComMan::GetDatI(0x20E, GetComLvF())) {
        float fMargin = m_pChr->GetVolleySuperSpikeCurveMaxDist() + RandRangeF(0.5f, 1.0f);
        rPos.x = CLAMP(rPos.x, fMargin + GetEmCourtLeft(), GetEmCourtRight() - fMargin);
    }

    if (CGame::g_GameWork.GetStage() == 5) {
        // The courts of stage 5 move: aim where they will be.
        long nCourt = m_nCourtIdx;
        bcs::FVEC3 ofsNow = CStageMan::GetInstance()->GetTeamCourtOffset(nCourt);
        float fFrame = RandRangeF(60.0f, 120.0f);
        if (m_pBall->IsVolleyChargeBall() && m_pChr->IsVolleySuperSpikeCurveType()) {
            fFrame = RandRangeF(120.0f, 180.0f);
        }
        bcs::FVEC3 ofsThen;
        CStageVolley05::GetAssumedTeamCourtOffset(fFrame, nCourt, ofsThen);
        rPos.x += ofsThen.x - ofsNow.x;
        rPos.z += ofsThen.z - ofsNow.z;
    }
    return 1;
}

// Whether to run to stop the other team's target marker: when it shows,
// no blocking is going on, no human of the team is near it, and the driver
// is the one to follow it.
int CComDrvVolleyGame::ShoudTargetMarkerGo() const
{
    if (IsMyTurn()) {
        return 0;
    }
    bcs::FVEC3 marker;
    if (!GetEmTargetMarkerPos(marker)) {
        return 0;
    }
    if (m_pChr->IsVolleyCombinationReceiver()) {
        return 1;
    }
    if (GetRootActType() == ACT_BLOCK_READY || GetRootActType() == ACT_BLOCK_GO ||
        GetRootActType() == ACT_TARGET_MARKER_GO) {
        return 0;
    }

    if (IsExistPadChrNear(this, GetMyTeamPadDrvCanTouchEnableBit(), Local2World(marker, -1), 3.0f)) {
        return 0;
    }

    // Peach's (character 17) special shot leaves its marker to her team.
    int bCheckOthers = 1;
    if (IsSpMarkerMode()) {
        CChrCore *pSp = GetLastSpChrCore();
        if (pSp != 0 && pSp->GetChrTypeID() == 17) {
            bCheckOthers = 0;
        }
    }
    if (bCheckOthers && CComMan::IsExistComRootActChr(m_nMyTeam, ACT_TARGET_MARKER_GO)) {
        if (!IsExistPadDrv() && m_bTouchGo) {
            return 1;
        }
        return 0;
    }
    long nChr = GetShoudFollowTargetChrIdx(marker, 0);
    if (nChr < 0) {
        return 0;
    }
    return nChr == m_nMyChrIdx;
}

// Whether to throw the stocked item now (between touches): never with a
// human in the team, while the courts are moved apart, or from far back.
int CComDrvVolleyGame::ShoudInterThrowItem() const
{
    if (GetMyStockItemType() < 0) {
        return 0;
    }
    if (IsExistPadDrv()) {
        return 0;
    }
    if (m_nUnk1CE == 0) {
        return 0;
    }
    if (!CanbeItemShoot()) {
        return 0;
    }
    if (IsCourtMoved()) {
        return 0;
    }
    return !(fabsf(m_MyPos.z - CComMan::GetCourtBorderZ(m_MyPos)) > 6.0f);
}

// Whether to play the special shot as a defence: on the team's touch, when
// the ball comes down soon and away from every team mate.
int CComDrvVolleyGame::ShoudSpDefensive() const
{
    if (!IsMyTurn()) {
        return 0;
    }
    if (!CanbeSp()) {
        return 0;
    }
    if (IsOutBallJudge()) {
        return 0;
    }
    if (m_nUnk1B0 == 0) {
        return 0;
    }
    if (m_pBall->GetTouchDownFrame(0.3f) >= 16.0f) {
        return 0;
    }
    bcs::FVEC3 down;
    CComManVolley::GetTouchDownPos(m_pBall, down);
    bcs::FVEC3 chrPos =
        CComMan::GetChrPosXZ(GetNearChrBaseWithCondition(down, GetMyTeamCanTouchEnableBit(), 0.0f));
    return !(GetDisXZ(down, chrPos) < 4.0f);
}

// Whether to break off for the ball after all: the driver is the nearest
// of the team to where it comes down and clearly nearer than the next.
int CComDrvVolleyGame::ShoudReceiveSuddenGo() const
{
    if (!IsMyTurn()) {
        return 0;
    }
    if (!CanbeMyTouch()) {
        return 0;
    }
    switch (GetRootActType()) {
        case ACT_TOSS_READY:
        case ACT_SPIKE_READY:
        case ACT_SPIKE_GO:
        case ACT_BLOCK_READY:
        case ACT_GO_HOME:
        case ACT_GET_QPANEL:
        case ACT_ITEM_SHOOT:
        case ACT_ESC_GIMMICK: {
            float fFrame = m_pBall->GetTouchDownFrame(0.3f);
            if (m_pBall->IsVolleyCurveSpike()) {
                if (fFrame >= 40.0f) {
                    return 0;
                }
            } else if (fFrame >= 15.0f) {
                return 0;
            }
            bcs::FVEC3 down;
            CComManVolley::GetTouchDownPos(m_pBall, down);
            unsigned long nBits = GetMyTeamCanTouchEnableBit();
            if (GetNearChrBaseWithCondition(down, nBits, 0.0f) != m_nMyChrIdx) {
                return 0;
            }
            long nNext = GetNearChrBaseWithConditionNo2(down, nBits, 0.0f);
            if (nNext < 0) {
                return 1;
            }
            bcs::FVEC3 nextPos = CComMan::GetChrPosXZ(nNext);
            float fMyDis = GetDisXZ(down, m_MyPos);
            float fNextDis = GetDisXZ(down, nextPos);
            if (fNextDis > 3.0f && fNextDis - fMyDis > 1.0f) {
                return 1;
            }
            return 0;
        }
        default:
            return 0;
    }
}

// Whether to dive for the ball: the driver is the nearest of the team to
// where it comes down very soon, and dives (always when its character
// can, else by a draw kept for the rally).
int CComDrvVolleyGame::ShoudDiveReceiveGo()
{
    if (m_pBall == 0) {
        return 0;
    }
    if (!IsMyTurn()) {
        return 0;
    }
    if (!IsExistPadDrv() && m_nUnk1CD == 0) {
        return 0;
    }
    if (m_pBall->GetState() == 2) {
        return 0;
    }
    if (m_pBall->IsArtsShootMode()) {
        CChrCore *pSp = GetLastSpChrCore();
        if (pSp != 0) {
            if (m_nMyTeam == pSp->GetTeamNo()) {
                return 0;
            }
            long nType = pSp->GetChrTypeID();
            if (nType == 0 || nType == 10) {
                return 0;
            }
        }
    }
    if (!CanbeMyTouch()) {
        return 0;
    }
    switch (GetRootActType()) {
        case ACT_RECEIVE_GO:
        case ACT_RECEIVE_SUDDEN:
        case ACT_TOSS_READY:
        case ACT_TOSS_GO:
        case ACT_SPIKE_READY:
        case ACT_SPIKE_GO:
        case ACT_BLOCK_READY:
        case ACT_BLOCK_GO:
        case ACT_GO_HOME:
        case ACT_GET_QPANEL:
        case ACT_ITEM_SHOOT:
        case ACT_ESC_GIMMICK: {
            if (IsOutBallJudge()) {
                return 0;
            }
            float fFrame = m_pBall->GetTouchDownFrame(0.3f);
            float fLimit = 15.0f;
            if (m_pChr->GetChrTypeID() == 17) {
                fLimit = 30.0f;
            }
            if (fFrame >= fLimit) {
                return 0;
            }
            bcs::FVEC3 down;
            CComManVolley::GetTouchDownPos(m_pBall, down);
            if (GetNearChrBaseWithCondition(down, GetMyTeamCanTouchEnableBit(), 0.0f) != m_nMyChrIdx) {
                return 0;
            }
            GetDisXZ(down, m_MyPos);
            int bDive = 0;
            if (CanbeDiveReceive()) {
                if (m_pChr->IsVolleyEnableDiveReceive()) {
                    bDive = 1;
                } else {
                    if (m_nUnk1D8 < 0) {
                        if (LotRate() < CComMan::GetDatI(0xFE, GetComLvF())) {
                            m_nUnk1D8 = 1;
                        } else {
                            m_nUnk1D8 = 0;
                        }
                    }
                    if (m_nUnk1D8 == 1) {
                        bDive = 1;
                    }
                }
            }
            return bDive;
        }
        default:
            return 0;
    }
}

// The character of the team that can spike the ball on the second touch,
// nearest to where it comes down; -1 for none.
long CComDrvVolleyGame::GetShoudTwoAttackChrIdx() const
{
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return -1;
    }
    if (m_pBall->GetTouchNum() != 1) {
        return -1;
    }
    bcs::FVEC3 target;
    if (!GetBallTargetPos(target)) {
        return -1;
    }
    unsigned long nBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() &&
            CComManVolley::CanbeTouch(pChr, nBall) && pChr->CanbeSpike(0)) {
            CComMan::SetEnableBit(nBits, i);
        }
    }
    return CComMan::GetNearChrBase(target, nBits);
}

// The character of the team that can spike the first touch quickly: the
// one nearest the net among those other than the one nearest to where the
// ball comes down; -1 for none.
long CComDrvVolleyGame::GetShoudQuickSpikeChrIdx() const
{
    long nRet = -1;
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return -1;
    }
    if (m_pBall->GetTouchNum() != 1) {
        return -1;
    }
    bcs::FVEC3 target;
    if (!GetBallTargetPos(target)) {
        return -1;
    }
    unsigned long nBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() &&
            CComManVolley::CanbeTouch(pChr, nBall) && pChr->CanbeSpike(0)) {
            CComMan::SetEnableBit(nBits, i);
        }
    }
    long nReceiver = CComMan::GetNearChrBase(target, nBits);
    float fBorder = CComMan::GetCourtBorderZ(m_MyPos);
    float fMin = 1000.0f;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() && i != nReceiver) {
            bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
            float fDis = fabsf(pos.z - fBorder);
            if (nRet < 0 || fDis < fMin) {
                nRet = i;
                fMin = fDis;
            }
        }
    }
    return nRet;
}

// Whether to get ready to block: on the other team's touches, for the
// one in front.
int CComDrvVolleyGame::ShoudBlockReady() const
{
    if (GetRootActType() == ACT_BLOCK_GO) {
        return 0;
    }
    if (IsMyTurn()) {
        return 0;
    }
    if (m_pBall->GetTouchNum() == 0) {
        return 0;
    }
    return m_nAPos == 0;
}

// Where to wait for the block: across the net from where the other team
// will spike, near the net.
int CComDrvVolleyGame::GetBlockReadyPos(bcs::FVEC3 &rPos) const
{
    bcs::FVEC3 spike;
    if (!GetBallSpikeJumpTargetPos(spike)) {
        return 0;
    }
    long nCourt = m_nUnk98;
    if (CComMan::GetCourtIdxByPos(spike) != nCourt) {
        spike = bcs::FVEC3(spike.x, spike.y, -spike.z);
    }
    rPos.x = 0.8f * spike.x;
    rPos.x = rPos.x + RandRange(-2.0f, 2.0f);
    rPos.y = 0.0f;
    if (m_nUnk8C == 0) {
        float fOfs = RandRangeF(0.8f, 1.2f);
        rPos.z = CComMan::GetCourtBorderZ(m_MyPos) + fOfs;
    } else {
        float fOfs = RandRangeF(0.8f, 1.2f);
        rPos.z = CComMan::GetCourtBorderZ(m_MyPos) - fOfs;
    }
    GetEmTargetMarkerChrCore();
    float fMargin = 0.7f;
    rPos.x = CLAMP(rPos.x, GetMyCourtParamLeft(fMargin), GetMyCourtParamRight(fMargin));
    rPos.z = CLAMP(rPos.z, GetMyCourtParamFront(fMargin), GetMyCourtParamBack(fMargin));
    return 1;
}

// Whether to go for the first touch: the driver is the one to follow the
// ball and no human of the team is already close to where it comes down.
int CComDrvVolleyGame::ShoudReceiveGo() const
{
    if (!IsMyTurn()) {
        return 0;
    }
    if (!CanbeMyTouch()) {
        return 0;
    }
    if (m_pBall->GetTouchNum() != 0) {
        return 0;
    }
    if (CComMan::IsExistComRootActChr(m_nMyTeam, ACT_RECEIVE_GO)) {
        return 0;
    }
    bcs::FVEC3 target;
    if (!GetBallTargetPos(target)) {
        return 0;
    }
    if (m_pChr->IsVolleyCombinationReceiver()) {
        return 1;
    }
    int bAny = 0;
    if (m_nMateNum >= 2) {
        bAny = 1;
    }
    long nChr = GetShoudFollowTargetChrIdx(target, bAny);
    if (nChr < 0) {
        return 0;
    }
    if (nChr != m_nMyChrIdx) {
        return 0;
    }
    if (IsExistPadChrNear(this, GetMyTeamPadDrvCanTouchEnableBit(), Local2World(target, -1), 2.0f)) {
        return 0;
    }
    return 1;
}

// Where to wait for the second touch: the home position, or round a bomb
// coming down there.
void CComDrvVolleyGame::GetTossReadyPos(bcs::FVEC3 &rPos) const
{
    GetHomePos(rPos, 1);
    if (m_bEscBomb && m_pBall->GetItemBallType() == 3) {
        bcs::FVEC3 target;
        if (GetBallTargetPos(target)) {
            float fRange = CItemBallVolleyBomb::GetExplosionCollSize();
            fRange = 0.6f + (fRange + m_pChr->GetCollSize_BodyDamage());
            if (GetDisXZ(rPos, target) < fRange) {
                bcs::FVEC3 dir;
                NormalizeXZ(dir, bcs::FVEC3(rPos.x - target.x, 0.0f, rPos.z - target.z));
                dir.x *= fRange;
                dir.z *= fRange;
                rPos.x = target.x + dir.x;
                rPos.y = 0.0f;
                rPos.z = target.z + dir.z;
            }
        }
    }
}

// Whether to play the second touch: the driver is the one to follow the
// ball.
int CComDrvVolleyGame::ShoudTossGo() const
{
    if (!IsMyTurn()) {
        return 0;
    }
    if (!CanbeMyTouch()) {
        return 0;
    }
    if (m_pBall->GetTouchNum() != 1) {
        return 0;
    }
    if (CComMan::IsExistComRootActChr(m_nMyTeam, ACT_TOSS_GO)) {
        return 0;
    }
    bcs::FVEC3 target;
    if (!GetBallTargetPos(target)) {
        return 0;
    }
    int bAny = 0;
    if (m_nMateNum >= 2) {
        bAny = 1;
    }
    if (GetShoudFollowTargetChrIdx(target, bAny) == m_nMyChrIdx) {
        return 1;
    }
    return 0;
}

// Where to wait for the spike: the home position, the open space found
// after the first touch, or somewhere near the net.
void CComDrvVolleyGame::GetSpikeReadyPos(bcs::FVEC3 &rPos) const
{
    long anRate[3] = {20, 20, 50};
    long nKind = LotProbability(anRate, 3);
    if (nKind == 0) {
        GetHomePos(rPos, 1);
        return;
    }
    if (nKind == 1) {
        rPos = m_Unk1E0;
        return;
    }
    float fRight = GetMyCourtParamRight(1.0f);
    rPos.x = RandRangeF(GetMyCourtParamLeft(1.0f), fRight);
    rPos.y = 0.0f;
    if (m_nUnk8C == 0) {
        float fOfs = RandRangeF(1.0f, 2.5f);
        rPos.z = CComMan::GetCourtBorderZ(m_MyPos) + fOfs;
    } else {
        float fOfs = RandRangeF(1.0f, 2.5f);
        rPos.z = CComMan::GetCourtBorderZ(m_MyPos) - fOfs;
    }
}

// Whether to spike: the ball was passed to the driver, or it is the one to
// follow it.
int CComDrvVolleyGame::ShoudSpikeGo() const
{
    if (m_pBall == 0) {
        return 0;
    }
    if (!IsMyTurn()) {
        return 0;
    }
    if (!CanbeMyTouch()) {
        return 0;
    }
    if (m_pBall->GetTouchNum() != 2) {
        return 0;
    }
    bcs::FVEC3 target;
    if (!GetBallTargetPos(target)) {
        return 0;
    }
    if (m_nMateNum >= 2 && m_pBall->GetPassChrIdx() == m_nMyChrIdx) {
        return 1;
    }
    if (GetShoudFollowTargetChrIdx(target, 0) == m_nMyChrIdx) {
        return 1;
    }
    return 0;
}

}
