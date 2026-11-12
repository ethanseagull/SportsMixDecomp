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
            CEventModel *m_pLine; // 0x34: the model of the line, owned (CreateWinRoute)
            float m_fLength;      // 0x38
            float m_fRotY;        // 0x3C
            float m_fLineFrame;   // 0x40: frame the line's animation starts at
            long m_nLineType;     // 0x44: argument of MakeLine (0x40: drawn backwards)
    }; // size 0x48
}

#endif /* !CROUTEBASICLINE_HPP_ */
