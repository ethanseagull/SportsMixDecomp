#ifndef CROUTEMOVE_HPP_
#define CROUTEMOVE_HPP_

#include "game/msw/CEventMove.hpp"
#include "game/msw/CRouteBase.hpp"

namespace msw {

    class CEventMoveAction;

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

            // Queues pRoute after the routes already queued; returns the
            // frame of the routes it starts at.
            float PushRoute(CRouteBase *pRoute, unsigned long ulFlag, float f1, float f2);
            // Start and end frame of pRoute when queued with ulFlag; whether
            // it runs backwards.
            int GetFrame(float &fStart, float &fEnd, CRouteBase *pRoute, unsigned long ulFlag, float f1, float f2);

            // Event ids of the queued routes, for msw::CTournamentRouteMan's
            // timers.
            long GetMoveEventId(long nRouteId, long n) const;
            long GetMoveFirstEventId() const;
            long GetMoveEndEventId() const;

            // The length in frames of the routes queued so far. (TourF has
            // a weak out-of-line copy)
            float GetRouteFrame() const { return m_fRouteFrame; }

            // One queued route, 0x14 bytes. (names guessed)
            struct ROUTE {
                CRouteBase *pRoute;     // 0x00
                float fStart;           // 0x04: frame of the route it starts at
                float fEnd;             // 0x08: and ends at
                float fFrame;           // 0x0C: fEnd - fStart
                signed char bReverse;   // 0x10
                signed char bOwn;       // 0x11: deleted with the move
                signed char bOpen;      // 0x12: the next route may still change its end
                signed char bOpenEnd;   // 0x13
            };

        private:
            // The route the frame fFrame falls in.
            float _SetStartFrame(long &nIndex, float fFrame);
            float _GetResult(CRouteBase::_RESULT &result, float fFrame, long nIndex, float fStart);

            CEventMoveAction *m_apAction[4]; // 0x060: by the routes' m_nAction
            ROUTE m_aRoute[24];              // 0x070
            float m_fRouteFrame;             // 0x250
            float m_fSpeed;                  // 0x254: frames of the routes per frame
            long m_nRouteNum;                // 0x258
    }; // size 0x25C
}

#endif /* !CROUTEMOVE_HPP_ */
