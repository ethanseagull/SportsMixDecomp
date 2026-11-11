#ifndef MSW_CCOMDRVVOLLEYRR_HPP_
#define MSW_CCOMDRVVOLLEYRR_HPP_

#include "game/msw/CComDrvVolley.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the volleyball game's rainbow
    // road stage (VolleyF; __vt__Q23msw15CComDrvVolleyRR). Only what
    // game/volley/unit_CA990.cpp needs; the rest of the class is the
    // CComDrvVolleyRR unit (0xB0780 on), not written yet. Its own members
    // start at 0x170 (ClearAll).
    class CComDrvVolleyRR : public CComDrvVolley {
        public:
            CComDrvVolleyRR();
            virtual ~CComDrvVolleyRR();

            virtual void GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos);
            virtual void GetViewPointPosForRunEx(bcs::FVEC3 &rPos);

            // Steers rPos round a character or item in the way; non-zero
            // while it is turning round one.
            int GetPosTurnAroundGimmick(bcs::FVEC3 &rPos);
    };
}

#endif /* !MSW_CCOMDRVVOLLEYRR_HPP_ */
