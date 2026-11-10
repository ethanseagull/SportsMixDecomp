#ifndef CEVENTMOVEACTION_HPP_
#define CEVENTMOVEACTION_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CEventModel;
    class CRouteBase;

    // What a moving event model plays while it moves (0x14 bytes, vptr at
    // 0x00). The class has functions in the DOL (SetLink, AnimPlay) and in
    // TourF (destructor, SetFrame, UpdateFrame, vtable); msw::CRouteMove's
    // constructor (TourF) creates the tournament kinds inline:
    // CEvMoveActTmNormal, CEvMoveActTmJump and CEvMoveActTmBigGun. Layout
    // read from that constructor; member names guessed.
    class CEventMoveAction {
        public:
            CEventMoveAction(CEventModel *pModel) : m_pModel(pModel), m_fFrame(0.0f), m_pRoute(0), m_pPrevRoute(0)
            {
            }

            // Empty: the derived destructors call nothing.
            virtual ~CEventMoveAction() {}
            virtual void SetFrame(float fFrame);
            virtual void UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame);

        protected:
            CEventModel *m_pModel;   // 0x04
            float m_fFrame;          // 0x08
            CRouteBase *m_pRoute;    // 0x0C: the route being run
            CRouteBase *m_pPrevRoute; // 0x10: m_pRoute at the last UpdateFrame
    }; // size 0x14
}

#endif /* !CEVENTMOVEACTION_HPP_ */
