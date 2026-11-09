// msw::CBallManBoss: the ball manager of the boss game.

// Must come first: it stands in for bcs/CTask.hpp (see the file).
#include "bcs/pending/CTask.hpp"

#include "game/msw/CBallManBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBallBoss.hpp"
#include "game/msw/CBallTargetMarkerBoss.hpp"

msw::CBallMan *msw::CBallManBoss::CreateInstance(bcs::CTask *pParent)
{
    if (CBallMan::m_pInstance == 0) {
        CBallMan::m_pInstance = new (0) CBallManBoss(pParent, 8);
    }
    return CBallMan::m_pInstance;
}

msw::CBallManBoss::~CBallManBoss()
{
}

msw::CBall *msw::CBallManBoss::GetNearPosBall_(const bcs::FVEC3 &rPos)
{
    // Squares of distances.
    float fNear = 99999.9f * 99999.9f;
    long nNear = 0;
    long nBallNum = CBallMan::GetBallNum();

    for (long i = 0; i < nBallNum; i++) {
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

msw::CBall *msw::CBallManBoss::GetFarPosBall_(const bcs::FVEC3 &rPos)
{
    float fFar = 0.0f;
    long nFar = 0;
    long nBallNum = CBallMan::GetBallNum();

    for (long i = 0; i < nBallNum; i++) {
        CBall *pBall = GetBallPtr_(i);

        if (pBall != 0 && !pBall->IsSleep()) {
            float fDist = bcs::FVEC3DistSq(&pBall->GetPos(), &rPos);

            if (fDist > fFar) {
                fFar = fDist;
                nFar = i;
            }
        }
    }
    return GetBallPtr_(nFar);
}

msw::CBall *msw::CBallManBoss::GetNearTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::CBallManBoss::GetFarTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

// One ball, and the marker of the place where it will come down.
void msw::CBallManBoss::Create_()
{
    DeleteAllBall_();

    CBall *pBall = new (0) CBallBoss(this, 0);
    pBall->Create(-1, 0x40);
    m_pBallHandle[0] = pBall->GetId();

    new (0x100) CBallTargetMarkerBoss(this, pBall->GetId(), 1);
}

msw::CBall *msw::CBallManBoss::AddBall_(CBall *pBall)
{
    return CBallMan::AddBall_(pBall);
}

// Deletes the balls made to be destroyed (CBall::SetDestroyOnReset) once
// they ask for it. Place 0, the game's own ball, is never looked at.
void msw::CBallManBoss::MainFunc_()
{
    if (m_pBallHandle == 0) {
        return;
    }

    for (long i = m_nBallNum - 1; i > 0; i--) {
        if (m_pBallHandle[i] == 0) {
            continue;
        }

        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_pBallHandle[i]));
        if (pBall == 0 || pBall->IsDelete()) {
            continue;
        }

        int bDelete = 0;
        if (pBall->IsDestroyOnReset() && pBall->IsVolleyReqDelete() && pBall->IsVolleyVanish()) {
            bDelete = 1;
        }
        if (bDelete) {
            pBall->Delete();
            m_pBallHandle[i] = 0;
        }
    }
}
