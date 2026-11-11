// msw::CComDrvVolleyRR: the computer driver of the volleyball game's
// rainbow road stage. Constructor, home positions, the choice of ball and
// coin, and the vtable. The actions are in game/volley/unit_B0780.cpp, the
// frame and message handlers in game/volley/unit_B16D4.cpp, going round
// obstacles in game/volley/unit_CA990.cpp.
#include "game/msw/CComDrvVolleyRR.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComManVolleyRR.hpp"
#include "game/msw/CDropCoin.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/CSimpleSortBase.hpp"

namespace {
    typedef bcs::CSimpleSortBase<32> SortBase;

    // (guessed helper) fVal kept between fMin and fMax.
    inline float Clamp(float fVal, float fMin, float fMax)
    {
        return fVal > fMax ? fMax : (fVal < fMin ? fMin : fVal);
    }
}

namespace msw {

CComDrvVolleyRR::ActFunc CComDrvVolleyRR::g_ActFuncMap[ACT_NUM] = {
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
    &CComDrvVolleyRR::ActLoopGoHome,
    &CComDrvVolleyRR::ActLoopReceiveReady,
    &CComDrvVolleyRR::ActLoopReceiveGo,
    &CComDrvVolleyRR::ActLoopGetDropCoin,
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

CComDrvVolleyRR::CComDrvVolleyRR() : CComDrvVolley(CalcWorkMaxSize(this))
{
}

CComDrvVolleyRR::~CComDrvVolleyRR()
{
}

void CComDrvVolleyRR::ClearAll()
{
    CComDrvVolley::ClearAll();
    for (int i = 0; i < 4; i++) {
        m_aHomePos[i].x = 0.0f;
        m_aHomePos[i].y = 0.0f;
        m_aHomePos[i].z = 0.0f;
    }
    m_nUnk1A0 = 1;
}

void CComDrvVolleyRR::UpdateInfo()
{
    CComDrvVolley::UpdateInfo();
}

// Keeps rPos on the stage, off the other court, and short of walls.
void CComDrvVolleyRR::NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const
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

// The home of the driver's place among the characters from left to right,
// moved back or forward when a ball or an item comes down there.
void CComDrvVolleyRR::GetHomePos(bcs::FVEC3 &rPos)
{
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nIdx = m_nMyChrIdx / 2;
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable()) {
            bcs::FVEC3 pos = CComMan::GetChrPos(i);
            pSort->m_nId = i;
            nNum++;
            pSort->m_fParam = pos.x;
            pSort++;
        }
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
    }
    for (long i = 0; i < nNum; i++) {
        if (m_nMyChrIdx == aSort[i].m_nId) {
            nIdx = i;
            break;
        }
    }

    rPos = m_aHomePos[nIdx];
    float fRange = 0.2f + (1.5f + m_pChr->GetCollSize_BodyDamage());
    float fShift = 3.5f + m_pChr->GetCollSize_BodyDamage();
    if (CComManVolleyRR::IsExistOthersBall(m_pChr, rPos, fRange) ||
        CComManVolleyRR::IsExistItemBall(m_pChr, rPos)) {
        rPos.z = m_aHomePos[nIdx].z - fShift;
        if (CComManVolleyRR::IsExistOthersBall(m_pChr, rPos, fRange) ||
            CComManVolleyRR::IsExistItemBall(m_pChr, rPos)) {
            rPos.z = fShift + m_aHomePos[nIdx].z;
        }
    }
}

CBall *CComDrvVolleyRR::GetMyBallPos(bcs::FVEC3 &rPos) const
{
    rPos.x = 0.0f;
    rPos.y = 0.0f;
    rPos.z = 0.0f;
    CBall *pBall = CComManVolleyRR::SearchPersonalBall(m_pChr, 0);
    if (pBall == 0) {
        pBall = GetAssignRainbowBall();
    }
    if (pBall != 0) {
        CComManVolley::GetBallTargetPos(pBall, rPos);
    }
    return pBall;
}

// Shares the rainbow balls coming down soonest (up to four) among the
// characters that have no ball of their own: a ball a character already
// stands under is that character's; the rest go to the others by the best
// overall pairing of characters and balls.
CBall *CComDrvVolleyRR::GetAssignRainbowBall() const
{
    unsigned long nFreeBits = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && CComManVolleyRR::SearchPersonalBall(pChr, 0) == 0) {
            CComMan::SetEnableBit(nFreeBits, i);
        }
    }

    // The rainbow balls by the time they come down.
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall == 0 || !pBall->IsActive()) {
            continue;
        }
        if (CComManVolleyRR::GetValidPtr(i) == 0 || pBall->GetVolleyBallTypeRR() != 4) {
            continue;
        }
        bcs::FVEC3 target;
        if (!CComManVolley::GetBallTargetPos(pBall, target)) {
            continue;
        }
        float fFrame = pBall->GetTouchDownFrame(0.3f);
        if (fFrame < 0.0f || fFrame > 80.0f) {
            continue;
        }
        pSort->m_nId = i;
        nNum++;
        pSort->m_fParam = fFrame;
        pSort++;
        if (nNum >= 32) {
            break;
        }
    }
    if (nNum <= 0) {
        return 0;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
    }
    if (nNum > 4) {
        nNum = 4;
    }

    // A ball whose nearest free character is already under it is taken.
    unsigned long nBits = nFreeBits;
    pSort = aSort;
    for (long i = 0; i < nNum; i++, pSort++) {
        CBall *pBall = CBallMan::GetBallPtr(pSort->m_nId);
        if (pBall == 0) {
            continue;
        }
        bcs::FVEC3 target;
        if (!CComManVolley::GetBallTargetPos(pBall, target)) {
            continue;
        }
        long nChr = CComMan::GetNearChrBase(target, nFreeBits);
        if (nChr < 0) {
            continue;
        }
        bcs::FVEC3 chrPos = CComMan::GetChrPos(nChr);
        if (GetDisXZ(chrPos, target) < 1.5f) {
            if (nChr == m_nMyChrIdx) {
                return pBall;
            }
            CComMan::ResetEnableBit(nBits, nChr);
            pSort->m_nId = -1;
            pSort->m_fParam = 99999.0f;
        }
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
    }

    // The characters left, and the balls left.
    bcs::FVEC3 aChrPos[4];
    long nChrNum = 0;
    long nMyIdx = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && CComMan::IsEnableBit(nBits, i)) {
            aChrPos[nChrNum] = CComMan::GetChrPos(pChr);
            if (i == m_nMyChrIdx) {
                nMyIdx = nChrNum;
            }
            nChrNum++;
        }
    }
    if (nChrNum <= 0) {
        return 0;
    }

    bcs::FVEC3 aBallPos[4];
    long aBallNo[4];
    long nBallNum = 0;
    pSort = aSort;
    for (long i = 0; i < nNum && nBallNum < nChrNum; i++, pSort++) {
        if (pSort->m_nId < 0) {
            break;
        }
        if (CComManVolley::GetBallTargetPos(CBallMan::GetBallPtr(pSort->m_nId), aBallPos[nBallNum])) {
            aBallNo[nBallNum] = pSort->m_nId;
            nBallNum++;
        }
    }
    if (nBallNum <= 0) {
        return 0;
    }

    long nBest = CComMan::GetNearBestPosBaseEx(aChrPos, aBallPos, nChrNum, nBallNum, nMyIdx);
    if (nBest < 0) {
        return 0;
    }
    return CBallMan::GetBallPtr(aBallNo[nBest]);
}

// The nearest coin on the ground that no other ball comes down on and that
// this character is the nearest to.
unsigned long CComDrvVolleyRR::GetShoudGetDropCoin() const
{
    SortBase::SORT aSort[32];
    SortBase::SORT *pSort = aSort;
    long nNum = 0;
    for (CDropCoin *pCoin = CComMan::DropCoin_GetFirst(); pCoin != 0; pCoin = CComMan::DropCoin_GetNext(pCoin)) {
        if (!pCoin->CanbePickup()) {
            continue;
        }
        bcs::FVEC3 pos = pCoin->GetPos();
        if (pos.y > 1.0f) {
            continue;
        }
        if (CComManVolleyRR::IsExistOthersBall(m_pChr, pos, 3.0f)) {
            continue;
        }
        if (CComMan::DropCoin_GetNearChr(pCoin, 0xFF) != m_nMyChrIdx) {
            continue;
        }
        float fDist = GetDisXZ(pos, m_MyWorldPos);
        if (fDist < 5.0f) {
            pSort->m_nId = pCoin->GetId();
            nNum++;
            pSort->m_fParam = fDist;
            pSort++;
            if (nNum >= 32) {
                break;
            }
        }
    }
    if (nNum <= 0) {
        return 0;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, SortBase::SortCmpParamF);
    }
    return aSort[0].m_nId;
}
}
