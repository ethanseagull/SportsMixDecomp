// msw::CBallManDodge: the ball manager of the dodge game. Ported from
// src/game/boss/CBallManBoss.cpp; the Dodge-specific functions are new.

// Included first, as in src/game/boss/CBallManBoss.cpp.
#include "bcs/CTask.hpp"

#include "game/msw/CBallManDodge.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBallDodge.hpp"
#include "game/msw/CBallMarkerDodge.hpp"

msw::CBallMan *msw::CBallManDodge::CreateInstance(bcs::CTask *pParent)
{
    if (CBallMan::m_pInstance == 0) {
        CBallMan::m_pInstance = new (0) CBallManDodge(pParent, BALL_MAX);
    }
    return CBallMan::m_pInstance;
}

msw::CBallManDodge::~CBallManDodge()
{
}

float msw::CBallManDodge::GetPassFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nArg) const
{
    CBall *pBall = GetBallPtr(0);

    if (pBall != 0) {
        return pBall->GetPassFlyFrame(rFrom, rTo, nArg);
    }
    return 0.0f;
}

float msw::CBallManDodge::GetShootFlyFrame_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nArg) const
{
    CBall *pBall = GetBallPtr(0);

    if (pBall != 0) {
        return pBall->GetShootFlyFrame(rFrom, rTo, nArg);
    }
    return 0.0f;
}

// Not zero when every ball allows a turnover.
int msw::CBallManDodge::CanbeTurnOver_() const
{
    long nBallNum = GetBallNum();

    for (long i = 0; i < nBallNum; i++) {
        CBall *pBall = GetBallPtr(i);

        if (pBall != 0 && !pBall->IsCanbeTurnOver()) {
            return 0;
        }
    }
    return 1;
}

int msw::CBallManDodge::IsBallPassMode_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        return pBall->IsPassMode();
    }
    return 0;
}

int msw::CBallManDodge::IsBallShootMode_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        return pBall->IsShootMode();
    }
    return 0;
}

int msw::CBallManDodge::IsBallInvincible_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        return pBall->IsInvincible();
    }
    return 0;
}

int msw::CBallManDodge::IsBallScored_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        return pBall->IsScored();
    }
    return 0;
}

void msw::CBallManDodge::SetBallPos_(long nBall, const bcs::FVEC3 *pPos)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        pBall->SetPos(pPos);
    }
}

void msw::CBallManDodge::SetJumpBallMode_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        pBall->SetJumpBallMode();
    }
}

void msw::CBallManDodge::SetSleepMode_(long nBall)
{
    CBall *pBall = GetBallPtr(nBall);

    if (pBall != 0) {
        pBall->SetSleepMode();
    }
}

void msw::CBallManDodge::Create_()
{
    DeleteAllBall_();

    long nBallNum = GetBallNum();
    for (long i = 0; i < nBallNum; i++) {
        CBall *pBall = new (0) CBallDodge(this, i);
        pBall->Create(-1, 0x40);
        m_pBallHandle[i] = pBall->GetId();

        new (0) CBallMarkerDodge(pBall, m_pBallHandle[i], 0);
    }
}

msw::CBall *msw::CBallManDodge::GetNearPosBall_(const bcs::FVEC3 &rPos)
{
    CBall *pNear = 0;
    float fNear;

    for (long i = 0; i < BALL_MAX; i++) {
        CBall *pBall = GetBallPtr(i);

        if (pBall != 0 && !pBall->IsSleepMode()) {
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &pBall->GetPos(), &rPos);
            float fDist = bcs::FVEC3Len(&diff);

            if (pNear == 0 || fDist < fNear) {
                pNear = pBall;
                fNear = fDist;
            }
        }
    }
    return pNear;
}

msw::CBall *msw::CBallManDodge::GetFarPosBall_(const bcs::FVEC3 &rPos)
{
    CBall *pFar = 0;
    float fFar;

    for (long i = 0; i < BALL_MAX; i++) {
        CBall *pBall = GetBallPtr(i);

        if (pBall != 0 && !pBall->IsSleepMode()) {
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &pBall->GetPos(), &rPos);
            float fDist = bcs::FVEC3Len(&diff);

            if (pFar == 0 || fDist > fFar) {
                pFar = pBall;
                fFar = fDist;
            }
        }
    }
    return pFar;
}

// The places of the table form three groups by their number modulo 3; the
// first three places belong to their own group only, the rest are shared.
// The new ball takes the first place, in pBall's group, that is empty, or
// whose ball is gone, being deleted or asleep (that ball is deleted first).
//
// As in the original, pNew keeps the ball found at an earlier place when the
// place taken is an empty one, and that ball is then restarted instead of a
// new one being made.
msw::CBall *msw::CBallManDodge::AddBall_(CBall *pBall)
{
    long nGroup;
    long nPlace = -1;
    CBall *pNew = 0;
    long i;

    nGroup = pBall->GetBallNo() % 3;
    for (i = 0; i < BALL_MAX; i++) {
        if (i % 3 == nGroup || i >= 3) {
            if (m_pBallHandle[i] == 0) {
                nPlace = i;
                break;
            }

            pNew = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
            if (pNew == 0 || pNew->IsDelete()) {
                m_pBallHandle[i] = 0;
                nPlace = i;
                pNew = 0;
                break;
            }
            if (pNew->IsSleep()) {
                DelBall(i);
                nPlace = i;
                pNew = 0;
                break;
            }
        }
    }

    CBall *pRet = 0;
    if (nPlace >= 0) {
        if (pNew == 0) {
            pNew = new (0) CBallDodge(this, pBall->GetBallNo());
            pNew->Create(-1, 0x40);
            m_pBallHandle[nPlace] = pNew->GetId();
            pNew->SetBallNo(nPlace);

            new (0) CBallMarkerDodge(pNew, m_pBallHandle[nPlace], 0);
        }
        pRet = pNew;
        pRet->SetJumpBallMode();
    }
    return pRet;
}

int msw::CBallManDodge::NotifyMessage(CBall *pFrom, long nMsg)
{
    return static_cast<CBallManDodge *>(GetInstance())->NotifyMessage_(pFrom, nMsg);
}

int msw::CBallManDodge::NotifyMessage_(CBall *pFrom, long nMsg)
{
    for (long i = 0; i < BALL_MAX; i++) {
        if (m_pBallHandle[i] == 0) {
            continue;
        }

        CBallDodge *pBall = static_cast<CBallDodge *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
        if (pBall == 0 || pBall->IsDelete() || pBall->IsSleep() || pBall == pFrom) {
            continue;
        }
        if (!pBall->OnMessage(pFrom, nMsg)) {
            return 1;
        }
    }
    return 0;
}
