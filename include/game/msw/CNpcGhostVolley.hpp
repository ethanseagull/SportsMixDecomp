#ifndef MSW_CNPCGHOSTVOLLEY_HPP_
#define MSW_CNPCGHOSTVOLLEY_HPP_

#include "game/msw/CNpcBase.hpp"

namespace msw {

    class CChrMan;

    // INCOMPLETE: the ghost NPC of volleyball stage 6 (VolleyF), size 0x22C
    // (what CChrManVolley::Create_ allocates). In the original it derives
    // from CNpcGhost (a CNpcBase) and CDefCollision, which are not declared
    // yet; only the constructor is here.
    class CNpcGhostVolley : public CNpcBase {
        public:
            // (parameter meaning guessed) CChrManVolley::Create_ passes
            // itself and 5.
            CNpcGhostVolley(CChrMan *pMan, long nArg);

        protected:
            unsigned char m_Unk188[0x22C - 0x188];
    };
}

#endif /* !MSW_CNPCGHOSTVOLLEY_HPP_ */
