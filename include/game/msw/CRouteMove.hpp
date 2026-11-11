#ifndef CROUTEMOVE_HPP_
#define CROUTEMOVE_HPP_

#include "game/msw/CEventMove.hpp"

namespace msw {

    class CRouteBase;

    // Moves an event model along a queue of routes (module TourF; derived
    // from the DOL's msw::CEventMove, which is not declared yet). 0x25C
    // bytes. Defined in game/tour/CTournamentRouteMan.cpp. Only the members
    // the stage route managers use; parameter names guessed.
    class CRouteMove : public CEventMove {
        public:
            CRouteMove(CEventModel *pModel, int b);
            virtual ~CRouteMove();
            virtual void SetFrame(float fFrame);
            virtual void UpdateFrame(float fFrame);

            // Starts the queued routes at frame fFrame. (names guessed)
            void SetupMove(float fFrame, int b);
            // Ends the routes, fFade frames from now. (name guessed)
            void CleanRoute(float fFade);

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
            unsigned char m_Unk060[0x1F0]; // 0x060 (not mapped)
            float m_fRouteFrame;           // 0x250
            unsigned char m_Unk254[0x8];   // 0x254
    }; // size 0x25C
}

#endif /* !CROUTEMOVE_HPP_ */
