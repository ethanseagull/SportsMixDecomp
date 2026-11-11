// msw::CComDrvBasket, the computer driver of the basketball game:
// constructor, destructor and the small helpers. The shot, dunk, cut and
// block methods are in another file (game/basket/unit_7E8FC.cpp).
#include "game/msw/CComDrvBasket.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComManBasket.hpp"

// The file's static initialiser sets this float, which nothing here uses.
// unit_7E8FC.cpp has the same one, so it most likely comes from a header
// the computer-driver files share (DodgeF's have 60 degrees); name and
// header are not known.
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle70 = DegToRad(70.0f);
}

namespace msw {

CComDrvBasket::CComDrvBasket(long nArg) : CComDrv(nArg) {
}

CComDrvBasket::~CComDrvBasket() {
}

long CComDrvBasket::GetChrTypeID() const {
    return m_pChr->GetChrTypeID();
}

CChrCore *CComDrvBasket::GetChrCore(long nChr) {
    return CChrMan::GetChrCore(nChr);
}

float CComDrvBasket::GetRotY_P1ToP2XZ(const CChrCore *p1, const CChrCore *p2) {
    return CComManBasket::GetRotY_P1ToP2XZ(p1, p2);
}

float CComDrvBasket::GetRotY_P1ToP2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2) {
    return CComManBasket::GetRotY_P1ToP2XZ(rPos1, rPos2);
}

CComDrvBasket *CComDrvBasket::GetComDrv(const CChrCore *pChr) const {
    if (pChr->DriverGetType() != 3) {
        return 0;
    }
    return static_cast<CComDrvBasket *>(pChr->DriverGetCurrent());
}

int CComDrvBasket::IsCutHit(const CChrCore *pChr) const {
    if (pChr == 0) {
        return 0;
    }
    if (pChr->IsHoldBall()) {
        return static_cast<CChrCoreBasket *>(m_pChr)->CutHitCheck(pChr);
    }
    return 0;
}

} // namespace msw
