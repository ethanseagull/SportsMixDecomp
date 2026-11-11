// msw::CComDrvVolley, the computer driver of the volleyball game:
// constructor, destructor and the small helpers. The serve and receive
// methods are in another file (they follow this file's weak functions in
// the original).
#include "game/msw/CComDrvVolley.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComManVolley.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

CComDrvVolley::CComDrvVolley(long nArg) : CComDrv(nArg) {
}

CComDrvVolley::~CComDrvVolley() {
}

void CComDrvVolley::ClearAll() {
    CComDrv::ClearAll();
}

void CComDrvVolley::UpdateInfo() {
    CComDrv::UpdateInfo();
}

int CComDrvVolley::GetBallTargetPos(bcs::FVEC3 &rPos) const {
    return CComManVolley::GetBallTargetPos(m_pBall, rPos);
}

int CComDrvVolley::GetBallSpikeJumpTargetPos(bcs::FVEC3 &rPos) const {
    return CComManVolley::GetBallSpikeJumpTargetPos(m_pBall, rPos);
}

int CComDrvVolley::GetTargetMarkerPos(bcs::FVEC3 &rPos) const {
    return CComManVolley::GetTargetMarkerPos(m_pChr, rPos, 0);
}

int CComDrvVolley::CanbeMyTouch() const {
    long nBall = GetActiveBallIdx();
    if (nBall < 0) {
        return 0;
    }
    return CComManVolley::CanbeTouch(m_pChr, nBall);
}

int CComDrvVolley::IsMyTurn() const {
    if (m_nMyTeam == CGame::g_GameWork.GetUserTeam()) {
        return 1;
    }
    return 0;
}

int CComDrvVolley::IsExistServeMode() const {
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->IsVolleyServeMode()) {
            return 1;
        }
    }
    return 0;
}

} // namespace msw
