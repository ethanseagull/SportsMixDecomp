// msw::CComDrvDodge, the computer driver of the dodgeball game: constructor
// and destructor. Its three own virtuals are in another file
// (game/dodge/unit_77468.cpp).
#include "game/msw/CComDrvDodge.hpp"

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

CComDrvDodge::CComDrvDodge(long nArg) : CComDrv(nArg) {
}

CComDrvDodge::~CComDrvDodge() {
}

} // namespace msw
