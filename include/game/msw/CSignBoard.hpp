#ifndef CSIGNBOARD_HPP_
#define CSIGNBOARD_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. A board shown on the screen. Lives in the DOL; only the
    // constructor is declared so far (argument names guessed; nType picks
    // the board). The members past CCockpitLayoutBase are not mapped.
    // CCockpitFrameOutBase is at 0x60: DodgeF's CSignBoardDodge has a
    // destructor thunk @96@, and CCockpitDodge::CreateObjImpl sets the word
    // at 0x60 to 1 on some boards (added for DodgeF
    // game/dodge/CCockpitDodge.cpp).
    class CSignBoard : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CSignBoard(long nId, long nType, float fSortZ);
            virtual ~CSignBoard();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk6C[0x70 - 0x6C];
    };
}

#endif /* !CSIGNBOARD_HPP_ */
