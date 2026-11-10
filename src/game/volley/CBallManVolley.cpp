// msw::CBallManVolley and the managers of the volley game's other two game
// types (BALL_MAN_VOLLEY::CBallManVolleyVsBoss, CBallManVolleyRhythmReceive).

// Must come first: it stands in for bcs/CTask.hpp (see the file).
#include "bcs/CTask.hpp"

#include "game/msw/CBallManVolley.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBallMarkerVolley.hpp"
#include "game/msw/CBallTargetMarker.hpp"
#include "game/msw/CBallVolley.hpp"
#include "game/msw/CGame.hpp"

msw::CBallMan *msw::CBallManVolley::CreateInstance(bcs::CTask *pParent)
{
    if (CBallMan::m_pInstance == 0) {
        switch (CGame::g_GameWork.GetGameType()) {
            case 1:
                CBallMan::m_pInstance = new (0) BALL_MAN_VOLLEY::CBallManVolleyVsBoss(pParent, 8);
                break;
            case 2:
                CBallMan::m_pInstance = new (0) BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive(pParent, 16);
                break;
            default:
                CBallMan::m_pInstance = new (0) CBallManVolley(pParent, 8);
                break;
        }
    }
    return CBallMan::m_pInstance;
}

msw::CBallManVolley::~CBallManVolley()
{
}

msw::CBall *msw::CBallManVolley::GetNearPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::CBallManVolley::GetFarPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::CBallManVolley::GetNearTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::CBallManVolley::GetFarTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

// One ball, the marker of the place where it will come down, and the
// marker that shows the ball itself.
void msw::CBallManVolley::Create_()
{
    DeleteAllBall_();

    CBall *pBall = new (0) CBallVolley(this, 0);
    pBall->Create(-1, 0x40);
    m_pBallHandle[0] = pBall->GetId();

    new (0x100) CBallTargetMarker(this, pBall->GetId(), 1);
    new (0) CBallMarkerVolley(this, pBall->GetId());
}

msw::CBall *msw::CBallManVolley::AddBall_(CBall *pBall)
{
    CBall *pAdded = CBallMan::AddBall_(pBall);

    if (pAdded != 0) {
        new (0x100) CBallTargetMarker(pAdded, pAdded->GetId(), 1);
    }
    return pAdded;
}

// Deletes the balls made to be destroyed (CBall::SetDestroyOnReset) once
// they ask for it, from the end of the table down, and stops at the first
// ball in play that does not. Place 0, the game's own ball, is never
// looked at.
void msw::CBallManVolley::MainFunc_()
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
        // The first ball that is still wanted ends the search.
        if (!bDelete) {
            break;
        }
        pBall->Delete();
        m_pBallHandle[i] = 0;
    }
}

// ---------------------------------------------------------------------------
// BALL_MAN_VOLLEY::CBallManVolleyVsBoss

msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::~CBallManVolleyVsBoss()
{
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::GetNearPosBall_(const bcs::FVEC3 &rPos)
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

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::GetFarPosBall_(const bcs::FVEC3 &rPos)
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

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::GetNearTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::GetFarTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

// One ball and the marker of the place where it will come down.
void msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::Create_()
{
    DeleteAllBall_();

    CBall *pBall = new (0) CBallVolleyVsBoss(this, 0);
    pBall->Create(-1, 0x40);
    m_pBallHandle[0] = pBall->GetId();

    new (0x100) CBallTargetMarkerVsBoss(this, pBall->GetId());
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::AddBall_(CBall *pBall)
{
    return CBallMan::AddBall_(pBall);
}

// Deletes the balls made to be destroyed (CBall::SetDestroyOnReset) once
// they ask for it. Place 0 is never looked at.
void msw::BALL_MAN_VOLLEY::CBallManVolleyVsBoss::MainFunc_()
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

// ---------------------------------------------------------------------------
// BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive

msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::~CBallManVolleyRhythmReceive()
{
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::GetNearPosBall_(const bcs::FVEC3 &rPos)
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

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::GetFarPosBall_(const bcs::FVEC3 &rPos)
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

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::GetNearTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::GetFarTargetPosBall_(const bcs::FVEC3 &)
{
    return GetBallPtr_(0);
}

// The balls of this game are added later (AddBall_).
void msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::Create_()
{
    DeleteAllBall_();
}

msw::CBall *msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::AddBall_(CBall *pBall)
{
    return CBallMan::AddBall_(pBall);
}

// Deletes the balls made to be destroyed (CBall::SetDestroyOnReset) once
// they ask for it. Place 0 is never looked at.
void msw::BALL_MAN_VOLLEY::CBallManVolleyRhythmReceive::MainFunc_()
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
