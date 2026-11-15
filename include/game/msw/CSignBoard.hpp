#ifndef CSIGNBOARD_HPP_
#define CSIGNBOARD_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. A board shown on the screen: one layout of a fixed table
    // (in, loop and out animations, some with an effect). Lives in the DOL
    // (game/msw/CSignBoard.cpp); argument names guessed, nType picks the
    // board.
    // CCockpitFrameOutBase is at 0x60: DodgeF's CSignBoardDodge has a
    // destructor thunk @96@, and CCockpitDodge::CreateObjImpl sets the word
    // at 0x60 to 1 on some boards (added for DodgeF
    // game/dodge/CCockpitDodge.cpp).
    class CSignBoard : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CSignBoard(long nId, long nType, float fSortZ);
            virtual ~CSignBoard();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            // Inline: DodgeF and VolleyF emit it with their boards' vtables.
            virtual void Disp() {}
            virtual int Set();
            virtual void Remove();

        protected:
            long m_nType; // 0x6C, index into the board table
    };
}

#endif /* !CSIGNBOARD_HPP_ */
