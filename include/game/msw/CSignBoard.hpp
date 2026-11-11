#ifndef CSIGNBOARD_HPP_
#define CSIGNBOARD_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. A board shown on the screen. Lives in the DOL; only the
    // constructor is declared so far (argument names guessed; nType picks
    // the board). The members past CCockpitLayoutBase are not mapped.
    class CSignBoard : public CCockpitLayoutBase {
        public:
            CSignBoard(long nId, long nType, float fSortZ);
            virtual ~CSignBoard();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x70 - 0x60];
    };
}

#endif /* !CSIGNBOARD_HPP_ */
