#ifndef CCOCKPITTELOPSHAKE_HPP_
#define CCOCKPITTELOPSHAKE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x84, in the DOL (seventeen module sites allocate it); a telop
    // that shakes. Only the constructor is known so far; the vptr at 0x18
    // puts it in the CCockpitLayoutBase family. Argument names are guesses.
    class CCockpitTelopShake : public CCockpitLayoutBase {
        public:
            CCockpitTelopShake(long nId, long nType, long nArg, float fSortZ);
            virtual ~CCockpitTelopShake();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x84 - 0x60];
    };
}

#endif /* !CCOCKPITTELOPSHAKE_HPP_ */
