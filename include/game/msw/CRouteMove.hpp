#ifndef CROUTEMOVE_HPP_
#define CROUTEMOVE_HPP_

namespace msw {

    class CRouteBase;

    // Moves an event model along a queue of routes (module TourF; derived
    // from the DOL's msw::CEventMove, which is not declared yet). 0x25C
    // bytes. Defined in game/tour/CTournamentRouteMan.cpp. Only the members
    // the stage route managers use; parameter names guessed.
    class CRouteMove {
        public:
            // Queues pRoute after the routes already queued.
            void PushRoute(CRouteBase *pRoute, unsigned long ulFlag, float f1, float f2);

            // Event ids of the queued routes, for msw::CTournamentRouteMan's
            // timers.
            long GetMoveEventId(long nRouteId, long n) const;
            long GetMoveFirstEventId() const;
            long GetMoveEndEventId() const;

            // The length in frames of the routes queued so far. (TourF has
            // a weak out-of-line copy)
            float GetRouteFrame() const { return m_fRouteFrame; }

        private:
            unsigned char m_Unk000[0x250]; // 0x000 (base class and members not mapped)
            float m_fRouteFrame;           // 0x250
            unsigned char m_Unk254[0x8];   // 0x254
    }; // size 0x25C
}

#endif /* !CROUTEMOVE_HPP_ */
