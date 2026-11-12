#ifndef CROUTEPHYSOFFSETY_HPP_
#define CROUTEPHYSOFFSETY_HPP_

#include "game/msw/CRouteBase.hpp"

namespace msw {

    // A route from the start to the end point of its _PARAM with a
    // physical arc in height (module TourF; the cannon shot of tournament
    // stage 32). 0x54 bytes. Defined in game/tour/CTournamentRouteMan.cpp.
    // Members not mapped yet; parameter names guessed.
    class CRoutePhysOffsetY : public CRouteBase {
        public:
            CRoutePhysOffsetY(const _PARAM &param, float rate, long n);
            virtual ~CRoutePhysOffsetY();
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);

            // Sets the length of the route from the speed and the height of
            // the arc.
            void CalcSpeed(float fSpeed, float fHeight);

        private:
            float m_fRotY;         // 0x34
            float m_fLength;       // 0x38
            bcs::FVEC3 m_vSpeed;   // 0x3C: per frame
            bcs::FVEC3 m_vGravity; // 0x48: per frame squared
    }; // size 0x54
}

#endif /* !CROUTEPHYSOFFSETY_HPP_ */
