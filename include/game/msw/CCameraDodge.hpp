#ifndef CCAMERADODGE_HPP_
#define CCAMERADODGE_HPP_

#include "game/msw/CGameCamera.hpp"

namespace msw {

    // The dodgeball camera: the base of CCameraDodgeGame (the match) and
    // CCameraDodgeBD (the party game). Adds no members of its own.
    class CCameraDodge : public CGameCamera {
        public:
            CCameraDodge(long view);
            virtual ~CCameraDodge() {}

            // Makes one camera per view, of the kind the game type needs.
            static void Initialize();
    };
}

#endif /* !CCAMERADODGE_HPP_ */
