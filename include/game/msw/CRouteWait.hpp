#ifndef CROUTEWAIT_HPP_
#define CROUTEWAIT_HPP_

#include "game/msw/CRouteBase.hpp"

namespace msw {

    // A route that stands still at the end point of its _PARAM for a number
    // of frames, facing the route's direction (module TourF). 0x38 bytes.
    // Defined in game/tour/CTournamentRouteMan.cpp.
    class CRouteWait : public CRouteBase {
        public:
            // Inline: msw::CTournamentRouteMan_Stage32's routes construct it
            // in place; TourF also has a weak out-of-line copy.
            CRouteWait(const _PARAM &param, float fNumFrame, long n) : CRouteBase(param.vEnd, param.vEnd, n)
            {
                m_fRotY = param.fRotY;
                m_fNumFrame = fNumFrame;
            }

            virtual ~CRouteWait();
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);

        protected:
            float m_fRotY; // 0x34
    }; // size 0x38
}

#endif /* !CROUTEWAIT_HPP_ */
