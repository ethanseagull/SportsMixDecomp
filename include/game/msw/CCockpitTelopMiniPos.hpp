#ifndef CCOCKPITTELOPMINIPOS_HPP_
#define CCOCKPITTELOPMINIPOS_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. A small telop at a position in the world
    // (CCockpit::CreateTelopMiniPos places it). Lives in the DOL; only the
    // constructor is declared so far (argument names guessed). The members
    // past CCockpitLayoutBase are not mapped.
    class CCockpitTelopMiniPos : public CCockpitLayoutBase {
        public:
            CCockpitTelopMiniPos(long nId, float fSortZ, unsigned char nType);
            virtual ~CCockpitTelopMiniPos();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x70 - 0x60];
    };
}

#endif /* !CCOCKPITTELOPMINIPOS_HPP_ */
