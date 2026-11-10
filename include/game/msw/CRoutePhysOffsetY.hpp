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
            unsigned char m_Unk34[0x20]; // 0x34
    }; // size 0x54
}

#endif /* !CROUTEPHYSOFFSETY_HPP_ */
