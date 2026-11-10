// msw::CBallManBasket: the ball manager of the basketball game.
#include "game/msw/CBallManBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallManBasketHungryBossPackun.hpp"
#include "game/msw/CBallMarkerBasket.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcBossPackunBasketBase.hpp"
#include "game/msw/CStageMan.hpp"

// Set by the file's static initialiser and used by nothing in the file:
// about 60 files of the game modules start their .bss with such an angle
// (DodgeF's CChrMarkerDodge.cpp has 60 degrees), so it most likely comes
// from a shared header. Which one, and the name, are not known.
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

msw::CBallMan *msw::CBallManBasket::CreateInstance(bcs::CTask *pParent)
{
    CBallMan *pMan;

    if (CGame::g_GameWork.GetGameType() == 2) {
        pMan = new CBallManBasketHungryBossPackun(pParent);
    } else {
        pMan = new CBallManBasket(pParent, 5);
    }
    m_pInstance = pMan;
    return pMan;
}

msw::CBallManBasket::CBallManBasket(bcs::CTask *pParent, long nBallMax) : CBallMan(pParent, nBallMax)
{
    m_fRaiseGoalInRate = 0.0f;
    m_nModeFlag = 0;
}

msw::CBallManBasket::~CBallManBasket()
{
}

int msw::CBallManBasket::IsInfinityReboundMode_() const
{
    return (m_nModeFlag >> 2) & 1;
}

void msw::CBallManBasket::SetInfinityReboundMode_(int bOn)
{
    if (bOn) {
        m_nModeFlag |= MODE_INFINITY_REBOUND;
    } else {
        m_nModeFlag &= ~MODE_INFINITY_REBOUND;
    }
}

int msw::CBallManBasket::IsDisableReboundMode_() const
{
    return (m_nModeFlag >> 1) & 1;
}

void msw::CBallManBasket::SetDisableReboundMode_(int bOn)
{
    if (bOn) {
        m_nModeFlag |= MODE_DISABLE_REBOUND;
    } else {
        m_nModeFlag &= ~MODE_DISABLE_REBOUND;
    }
}

int msw::CBallManBasket::IsDisableAirBallMode_() const
{
    return m_nModeFlag & MODE_DISABLE_AIRBALL;
}

void msw::CBallManBasket::SetDisableAirBallMode_(int bOn)
{
    if (bOn) {
        m_nModeFlag |= MODE_DISABLE_AIRBALL;
    } else {
        m_nModeFlag &= ~MODE_DISABLE_AIRBALL;
    }
}

int msw::CBallManBasket::IsBallNowRebound_(long nBall)
{
    CBall *pBall = GetBallPtr_(nBall);

    if (pBall != 0) {
        return pBall->IsNowRebound();
    }
    return 0;
}

void msw::CBallManBasket::Create_()
{
    DeleteAllBall_();

    CBall *pBall = new (0) CBallBasket(this, 0);
    pBall->Create(-1, 0x40);
    m_pBallHandle[0] = pBall->GetId();

    new (0) CBallMarkerBasket(this, pBall->GetId());
}

void msw::CBallManBasket::AllReset_()
{
    CBallMan::AllReset_();
}

msw::CBall *msw::CBallManBasket::GetNearPosBall_(const bcs::FVEC3 &rPos)
{
    // Squares of distances.
    float fNear = 99999.9f * 99999.9f;
    long nNear = 0;

    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = GetBallPtr_(i);

        if (pBall != 0 && !pBall->IsSleep()) {
            float fDist = bcs::FVEC3DistSq(&pBall->GetPos(), &rPos);

            if (fDist < fNear) {
                fNear = fDist;
                nNear = i;
            }
        }
    }
    return GetBallPtr_(nNear);
}

int msw::CBallManBasket::IsDummyBall_(const CBall *pBall) const
{
    return static_cast<const CBallBasket *>(pBall)->IsDummy();
}

void msw::CBallManBasket::CreateDummyBall_()
{
    long nBallNo = GetInstance()->GetUseEnableIndex_();
    CBall *pBall = new (0) CBallBasketDummy(CBallMan::GetInstance(), nBallNo);

    pBall->Create(-1, 0x40);
    pBall->Reset();
    CBallMan::AddBall(pBall);
}

long msw::CBallManBasket::CheckCurveDir_(float fArg, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                          float fRadius) const
{
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &rTo, &rFrom);
    bcs::FVEC3 dir = diff;

    // Half the distance, along the horizontal direction from rFrom to rTo.
    float fHalf = PSVECMag((const Vec *)&dir) * 0.5f;
    bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));

    bcs::FVEC3 side;
    bcs::BCS_Vec_RotY(side, dir, 1.5707964f);

    float fWidth = fArg + fRadius;
    dir.x = dir.x * fHalf + rFrom.x;
    side.x *= fWidth;
    side.z *= fWidth;
    dir.z = dir.z * fHalf + rFrom.z;

    // The two sides of the curve at its middle.
    bcs::FVEC3 right;
    bcs::FVEC3 left;
    bcs::FVEC3Sub(&right, &dir, &side);
    bcs::FVEC3Add(&left, &dir, &side);
    bcs::FVEC3 posRight = right;
    bcs::FVEC3 posLeft = left;

    float fLeft;
    float fRight;
    float fFront;
    float fBack;
    CStageMan::GetInstance()->GetCourtRect(&fLeft, &fRight, &fFront, &fBack);

    int bRightIn = 0;
    if (posRight.x - fRadius > fLeft && posRight.x + fRadius < fRight && posRight.z - fRadius > fFront &&
        posRight.z + fRadius < fBack) {
        bRightIn = 1;
    }
    int bLeftIn = 0;
    if (posLeft.x - fRadius > fLeft && posLeft.x + fRadius < fRight && posLeft.z - fRadius > fFront &&
        posLeft.z + fRadius < fBack) {
        bLeftIn = 1;
    }

    if (bRightIn && bLeftIn) {
        return 0;
    }
    if (bRightIn) {
        return 4;
    }
    return 2;
}

long msw::CBallManBasket::GetUseEnableIndex_() const
{
    unsigned long used = 0;

    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);

        if (pBall != 0) {
            long nNo = pBall->GetBallNo();
            used |= 1 << nNo;
        }
    }

    long nBallNo = 0;
    for (;;) {
        unsigned long nBit = 1 << nBallNo;
        if ((used & nBit) == 0) {
            break;
        }
        nBallNo++;
    }
    long nResult = nBallNo;
    return nResult;
}

namespace {
    // (guessed helper) The horizontal distance from rFrom to rTo.
    inline float GetDistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rTo, &rFrom);
        bcs::FVEC3 d = diff;
        bcs::FVEC3 xz(d.x, 0.0f, d.z);
        return PSVECMag((const Vec *)&xz);
    }

    // (guessed helper) The frame for a shot from rFrom to rTo, blended
    // between the frames given for the shot distances around it.
    inline long CalcJustShotFrame(const long *pFrame, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        long n = msw::BASKET_PRM_S70_JS_MID_DIST <= GetDistXZ(rFrom, rTo);
        float fFrame0 = pFrame[n];
        float fFrame1 = pFrame[n + 1];
        float fDist[3] = { msw::BASKET_PRM_S70_JS_MIN_DIST, msw::BASKET_PRM_S70_JS_MID_DIST,
                           msw::BASKET_PRM_S70_JS_MAX_DIST };
        float fDist0 = fDist[n];
        float fDist1 = fDist[n + 1];

        float fLen = GetDistXZ(rFrom, rTo);
        if (fLen > fDist1) {
            fLen = fDist1;
        } else if (fLen < fDist0) {
            fLen = fDist0;
        }

        float fRate = (fLen - fDist0) / (fDist1 - fDist0);
        if (fRate > 1.0f) {
            fRate = 1.0f;
        } else if (fRate < 0.0f) {
            fRate = 0.0f;
        }
        return (fFrame1 - fFrame0) * fRate + fFrame0;
    }
}

long msw::CBallManBasket::GetJustShotTimingMin_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                                const CBall *pBall) const
{
    if (pBall == 0) {
        return 0;
    }
    // No timing while the boss Piranha Plant is in state 0.
    CNpcBossPackunBasketBase *pBoss = CChrManBasket::GetInstance()->GetBossPackun_(0);
    if (pBoss != 0 && pBoss->GetState() == 0) {
        return 0;
    }

    const CBallBasket *pBasketBall = static_cast<const CBallBasket *>(pBall);
    long frame[3];
    frame[0] = pBasketBall->GetJustShotMinStartFrm();
    frame[1] = pBasketBall->GetJustShotMidStartFrm();
    frame[2] = pBasketBall->GetJustShotMaxStartFrm();
    return CalcJustShotFrame(frame, rFrom, rTo);
}

long msw::CBallManBasket::GetJustShotTimingMax_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                                const CBall *pBall) const
{
    if (pBall == 0) {
        return 0;
    }
    // No timing while the boss Piranha Plant is in state 0.
    CNpcBossPackunBasketBase *pBoss = CChrManBasket::GetInstance()->GetBossPackun_(0);
    if (pBoss != 0 && pBoss->GetState() == 0) {
        return 0;
    }

    const CBallBasket *pBasketBall = static_cast<const CBallBasket *>(pBall);
    long frame[3];
    frame[0] = pBasketBall->GetJustShotMinEndFrm();
    frame[1] = pBasketBall->GetJustShotMidEndFrm();
    frame[2] = pBasketBall->GetJustShotMaxEndFrm();
    return CalcJustShotFrame(frame, rFrom, rTo);
}

int msw::CBallManBasket::GetShootRateBase_() const
{
    return 65;
}

int msw::CBallManBasket::GetShootRate0_() const
{
    return 100;
}

int msw::CBallManBasket::GetShootRate1_() const
{
    return 56;
}

int msw::CBallManBasket::GetShootRate2_() const
{
    return 10;
}

int msw::CBallManBasket::GetShootRate3_() const
{
    return 3;
}

float msw::CBallManBasket::GetShootArea1_() const
{
    return 7.1f;
}

float msw::CBallManBasket::GetShootArea2_() const
{
    return 11.6f;
}

float msw::CBallManBasket::GetShootArea3_() const
{
    return 24.1f;
}

int msw::CBallManBasket::GetAirBallRate_() const
{
    return 5;
}

// Stages 7 and 0x46 to 0x48 have a wider air ball range. (what the stages
// are is not known)
float msw::CBallManBasket::GetAirBallRangeDistMin_() const
{
    switch (CGame::g_GameWork.GetStage()) {
        case 7:
        case 0x46:
        case 0x47:
        case 0x48:
            return 2.0f;
    }
    return 0.5f;
}

float msw::CBallManBasket::GetAirBallRangeDistMax_() const
{
    switch (CGame::g_GameWork.GetStage()) {
        case 7:
        case 0x46:
        case 0x47:
        case 0x48:
            return 2.5f;
    }
    return 1.0f;
}

// A ball can be caught when it is in play and in state 0xB, rebounding, or
// moving freely at 1.5 or higher. (what state 0xB is is not known)
int msw::CBallManBasket::CatchEnable_(const CBall *pBall) const
{
    if (pBall == 0) {
        return 0;
    }
    if (!pBall->IsActive()) {
        return 0;
    }
    if (pBall->FsmqGetState() == 0xB) {
        return 1;
    }
    if (!pBall->IsNowRebound()) {
        if (!pBall->IsFreeMoveMode() || pBall->GetPos().y < 1.5f) {
            return 0;
        }
    }
    return 1;
}
