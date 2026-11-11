// msw::CComDrvBasket, second file: running to a point and the shot, dunk,
// cut, jump and block methods. The constructor and the small helpers are in
// game/basket/CComDrvBasket.cpp.
#include "game/msw/CComDrvBasket.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CTournament.hpp"

// The same unused float as in CComDrvBasket.cpp (see there): set by the
// file's static initialiser, read by nothing.
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle70 = DegToRad(70.0f);
}

namespace msw {

// Runs towards pos (relative to the character). Returns whether the
// character is already within fDist of it; when it is and bKeepRunning is
// zero nothing else is done. (parameter names guessed)
int CComDrvBasket::MethdBKRun(const bcs::FVEC3 &pos, float fDist, float fDpm, int bKeepRunning) {
    bcs::FVEC3 local = pos;
    NormalizeCanMovePos(local);
    bcs::FVEC3 world = Local2World(local, -1);

    int bArrived = m_pChr->GetDist_ToPosXZ(world) <= fDist;
    if (!bArrived || bKeepRunning) {
        float fRot = m_pChr->GetRotY_ToPosXZ(world);
        if (m_pChr->IsHoldBall()) {
            if (!m_pChr->MethdAutoDPM(fRot, fDpm)) {
                CChrCore::INPUT_WORK work;
                work.m_nUnk00 = 0;
                m_pChr->MethdRDB(&work, fRot);
            }
        } else {
            m_pChr->MethdRun(fRot);
        }
    }
    return bArrived;
}

int CComDrvBasket::CanbeJumpShoot() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    if (CTournament::IsMissionComThrowingShootDisable(m_pChr->GetTeamNo())) {
        return 0;
    }
    return m_pChr->CanbeJumpShoot();
}

int CComDrvBasket::MethdJumpShoot(float fRot) {
    if (CanbeJumpShoot()) {
        return m_pChr->MethdJumpShoot(fRot, 0);
    }
    return 0;
}

int CComDrvBasket::CanbeDunk() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    if (CTournament::IsMissionComDunkShootDisable(m_pChr->GetTeamNo())) {
        return 0;
    }
    return m_pChr->CanbeDunk();
}

int CComDrvBasket::MethdDunk() {
    if (CanbeDunk()) {
        return m_pChr->MethdDunk();
    }
    return 0;
}

int CComDrvBasket::CanbeLayUp() const {
    if (CTournament::IsMissionComLayupShootDisable(m_pChr->GetTeamNo())) {
        return 0;
    }
    return m_pChr->CanbeLayUp();
}

int CComDrvBasket::CanbeCut() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->CanbeBlockCut(0);
}

int CComDrvBasket::MethdCut(float fRot) {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdBlockCut(fRot);
}

// Jumps where the character faces.
int CComDrvBasket::MethdJump() {
    float fRot = m_pChr->GetRotY();
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdJumpBlock(fRot);
}

int CComDrvBasket::CanbeJumpBlock() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->CanbeJumpBlock();
}

int CComDrvBasket::MethdJumpBlock(float fRot) {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdJumpBlock(fRot);
}

int CComDrvBasket::CanbeUpperCut() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->CanbeJumpUpper();
}

int CComDrvBasket::MethdUpperCut() {
    if (m_pChr->IsMoveAssist()) {
        return 0;
    }
    return m_pChr->MethdJumpUpper();
}

int CComDrvBasket::CanbeJumpCut() const {
    return m_pChr->CanbeSwatIt();
}

} // namespace msw
