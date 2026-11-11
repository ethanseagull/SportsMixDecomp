#ifndef CCOCKPITCOMBIICON2D_HPP_
#define CCOCKPITCOMBIICON2D_HPP_

#include "game/msw/CCockpitChrIcon2D.hpp"

namespace msw {

    // Size 0x68, no members past CCockpitChrIcon2D's. Lives in the DOL; only
    // the constructor is declared so far (the arguments are those of
    // CCockpitChrIcon2D).
    class CCockpitCombiIcon2D : public CCockpitChrIcon2D {
        public:
            CCockpitCombiIcon2D(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitCombiIcon2D();
            virtual int Set();
            virtual void Remove();
    };
}

#endif /* !CCOCKPITCOMBIICON2D_HPP_ */
