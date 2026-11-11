#ifndef CSIGNBOARDVOLLEY_HPP_
#define CSIGNBOARDVOLLEY_HPP_

#include "game/msw/CSignBoard.hpp"

namespace msw {

    // Size 0x70: adds no data to CSignBoard. The volley game's boards
    // (game/volley/CCockpitVolleyTelopServe.cpp); it overrides only the
    // destructor and SendEvent.
    class CSignBoardVolley : public CSignBoard {
        public:
            CSignBoardVolley(long nId, long nType, float fSortZ);
            virtual ~CSignBoardVolley();
            virtual long SendEvent(long nEvent, void *pParam);
    };
}

#endif /* !CSIGNBOARDVOLLEY_HPP_ */
