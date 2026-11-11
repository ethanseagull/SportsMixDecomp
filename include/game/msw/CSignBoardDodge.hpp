#ifndef CSIGNBOARDDODGE_HPP_
#define CSIGNBOARDDODGE_HPP_

#include "game/msw/CSignBoard.hpp"

namespace msw {

    // Size 0x70: adds no data to CSignBoard. The dodge game's boards
    // (DodgeF game/dodge/CCockpitDodgeTelopInOutChange.cpp); it overrides
    // only the destructor and SendEvent.
    class CSignBoardDodge : public CSignBoard {
        public:
            CSignBoardDodge(long nId, long nType, float fSortZ);
            virtual ~CSignBoardDodge();
            virtual long SendEvent(long nEvent, void *pParam);
    };
}

#endif /* !CSIGNBOARDDODGE_HPP_ */
