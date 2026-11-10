// msw::CComDrvDodge's own virtuals: the side-step roll. A file of its own
// in the original (it has its own static initialiser, after these three
// functions); its name is not known.
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CChrCore.hpp"

// The file's static initialiser sets this float, which nothing here uses.
// It most likely comes from a header shared with many other files (see
// src/game/dodge/CChrMarkerDodge.cpp); name and header are not known.
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle60 = DegToRad(60.0f);
}

namespace msw {

int CComDrvDodge::CanbeDodgeRoll(float fRot) {
    return m_pChr->CanbeSideStep(fRot, fRot);
}

int CComDrvDodge::MethdDodgeRoll(float fRot) {
    return m_pChr->MethdSideStep(fRot, fRot);
}

int CComDrvDodge::MethdDodgeRollTarget(const bcs::FVEC3 &pos) {
    bcs::FVEC3 world = Local2World(pos, -1);
    float fRot = m_pChr->GetRotY_ToPosXZ(world);
    return m_pChr->MethdSideStep(fRot, fRot);
}

} // namespace msw
