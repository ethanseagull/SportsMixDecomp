#ifndef CCOCKPITDODGERESULT_HPP_
#define CCOCKPITDODGERESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    // Size 0x6C (CCockpitDodge::CreateObjImpl allocates it): the dodge
    // game's result page, the score and points of each set (DodgeF
    // game/dodge/CCockpitDodgeResult.cpp). Return types and argument names
    // are guesses.
    class CCockpitDodgeResult : public CCockpitLayoutBase, public CCockpitResultTime {
        public:
            CCockpitDodgeResult(long nId, float fSortZ);
            virtual ~CCockpitDodgeResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu(); // returns 0 (the original sets r3)

            // The period scores of the two teams; nPeriod is the period
            // reached, nNum the number of periods.
            long SetCountScore(long *pScore0, long *pScore1, long nPeriod, long nNum);
            // The points of each period of the two teams, up to nPeriod.
            long SetCountPoint(long *pPoint0, long *pPoint1, long nPeriod);
            long SetTextNext(long nNum, long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);
    };
}

#endif /* !CCOCKPITDODGERESULT_HPP_ */
