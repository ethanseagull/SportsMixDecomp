#ifndef CCOCKPITTOURNAMENTLAYOUTBASE_HPP_
#define CCOCKPITTOURNAMENTLAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x60 (presumably; it adds no data the destructor touches). Base
    // of tournament cockpit layouts. Its destructor and vtable are in
    // game/tour/CCockpitTournament.cpp; its constructor is not there.
    // Abstract: Set and Remove of CMenuLinkBase are still pure, and it adds
    // one pure virtual of its own (the last, null slot of its vtable).
    class CCockpitTournamentLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitTournamentLayoutBase(long nId);
            virtual ~CCockpitTournamentLayoutBase();

            // (placeholder) Nothing in TourF overrides it.
            virtual void Unk5C() = 0;
    };
}

#endif /* !CCOCKPITTOURNAMENTLAYOUTBASE_HPP_ */
