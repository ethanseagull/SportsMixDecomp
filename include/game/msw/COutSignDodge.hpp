#ifndef COUTSIGNDODGE_HPP_
#define COUTSIGNDODGE_HPP_

#include "bcs/G2D/CLayoutObj.hpp"
#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"

namespace msw {

    // Size 0x30 (guessed from the members its functions touch). A sign of
    // the dodge game ("ck_out" layout, with a "matchpoint" pane and one
    // picture per player) in DodgeF game/dodge/CCockpitDodgeTelopInOutChange.cpp.
    // CMenuLinkBase at 0, CMenuLytObj at 0x1C (its destructor thunk is
    // @28@). Nothing in DodgeF creates one.
    // Member names are guesses.
    class COutSignDodge : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CLayoutObj, 1> {
        public:
            // Out of line but called by nothing, so the module link dropped
            // it; its trace is the weak ~CMenuLinkBase and CMenuLinkBase's
            // vtable that the file emits before ~COutSignDodge. Body guessed.
            COutSignDodge(long nId);
            virtual ~COutSignDodge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);

        private:
            long m_nStep; // 0x2C, 1 while the sign waits for its animation (set by event 4)
    };
}

#endif /* !COUTSIGNDODGE_HPP_ */
