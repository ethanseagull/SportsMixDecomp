#ifndef CCAMERAHOCKEY_HPP_
#define CCAMERAHOCKEY_HPP_

#include "game/msw/CGameCamera.hpp"

namespace msw {

    // The hockey camera: the base of CCameraHockeyGame (the match) and
    // CCameraHockeyCS (the party game). Adds no members of its own.
    class CCameraHockey : public CGameCamera {
        public:
            CCameraHockey(long view);
            virtual ~CCameraHockey() {}

            // Makes one camera per view, of the kind the game type needs.
            static void Initialize();
    };
}

#endif /* !CCAMERAHOCKEY_HPP_ */
