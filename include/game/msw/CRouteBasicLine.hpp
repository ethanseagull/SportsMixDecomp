#ifndef CROUTEBASICLINE_HPP_
#define CROUTEBASICLINE_HPP_

#include "game/msw/CRouteBase.hpp"

namespace msw {

    // A route that walks in a straight line from the start to the end point
    // of its _PARAM (module TourF). 0x48 bytes. Defined in
    // game/tour/CTournamentRouteMan.cpp. Members not mapped yet; parameter
    // names guessed.
    class CRouteBasicLine : public CRouteBase {
        public:
            CRouteBasicLine(const _PARAM &param, float rate, long n);
            virtual ~CRouteBasicLine();
            virtual void CalcFrame();
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual void MakeLine(long nFrom, long nTo);
            virtual void SetDisp(int bDisp);
            virtual int IsDisp() const;
            virtual void AnimPlay(long nAnim);

        private:
            unsigned char m_Unk34[0x14]; // 0x34
    }; // size 0x48
}

#endif /* !CROUTEBASICLINE_HPP_ */
