#ifndef MSW_CCOMMANBASKET_HPP_
#define MSW_CCOMMANBASKET_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The basketball computer manager's shared helpers (BasketF).
    //
    // INCOMPLETE: only what game/basket/CComDrvBasket.cpp calls is
    // declared. Static, like CComManVolley's helpers.
    class CComManBasket : public CComMan {
        public:
            // The yaw (radians) from p1 towards p2 in the XZ plane.
            static float GetRotY_P1ToP2XZ(const CChrCore *p1, const CChrCore *p2);
            static float GetRotY_P1ToP2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);
    };
}

#endif /* !MSW_CCOMMANBASKET_HPP_ */
