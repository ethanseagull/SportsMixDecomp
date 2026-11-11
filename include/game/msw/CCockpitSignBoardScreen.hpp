#ifndef CCOCKPITSIGNBOARDSCREEN_HPP_
#define CCOCKPITSIGNBOARDSCREEN_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. Lives in the DOL; only the constructor is declared so far
    // (argument names guessed). The members past CCockpitLayoutBase are not
    // mapped.
    class CCockpitSignBoardScreen : public CCockpitLayoutBase {
        public:
            CCockpitSignBoardScreen(long nId, long nType, float fSortZ);
            virtual ~CCockpitSignBoardScreen();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x70 - 0x60];
    };
}

#endif /* !CCOCKPITSIGNBOARDSCREEN_HPP_ */
