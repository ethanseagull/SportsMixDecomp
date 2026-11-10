#ifndef SELECTMENU_CBGMDL_HPP_
#define SELECTMENU_CBGMDL_HPP_

#include "game/msw/CMenuLinkBase.hpp"

namespace msw {
    namespace SelectMenu {

        // The background model of the SelectMenu screens; menu object 0x1D
        // of CSelectMenu. Only the members used so far: the direct base is
        // not known (CMenuLinkBase is at 0, since CSelectMenu's object list
        // holds it), its virtuals are not declared, so never create one
        // through this header.
        class CBgMdl : public CMenuLinkBase {
            public:
                // Shows the ball of game nBall (0 to 3).
                void ChangeBall(long nBall);
        };
    }
}

#endif /* !SELECTMENU_CBGMDL_HPP_ */
