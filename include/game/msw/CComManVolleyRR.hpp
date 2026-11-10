#ifndef MSW_CCOMMANVOLLEYRR_HPP_
#define MSW_CCOMMANVOLLEYRR_HPP_

#include "game/msw/CComManVolley.hpp"

namespace msw {

    // Computer helpers of a volleyball party game with several balls, each
    // belonging to a team ("RR": the balls answer GetVolleyBallTypeRR and
    // GetVolleyBallColorTypeRR). All static. (that it derives from
    // CComManVolley is a guess; it calls its helpers unqualified)
    class CComManVolleyRR : public CComManVolley {
        public:
            // Of the balls of pChr's team (by colour when bColor is set),
            // the one that touches down first; null when none.
            static CBall *SearchPersonalBall(const CChrCore *pChr, int bColor);
            // Not zero when a ball of another team (other than type 4) will
            // come down within fRange of rPos (court frame).
            static int IsExistOthersBall(const CChrCore *pChr, const bcs::FVEC3 &rPos, float fRange);
            // Not zero when a thrown item of kind 2 or 5 lands, or will
            // soon, within reach of pChr standing at rPos (court frame).
            static int IsExistItemBall(const CChrCore *pChr, const bcs::FVEC3 &rPos);
            // Ball nBall when it is active and has a target position;
            // null otherwise.
            static CBall *GetValidPtr(long nBall);
    };
}

#endif /* !MSW_CCOMMANVOLLEYRR_HPP_ */
