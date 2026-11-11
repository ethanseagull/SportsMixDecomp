#ifndef CCOCKPITHOCKEYRESULT_HPP_
#define CCOCKPITHOCKEYRESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    // Size 0x6C (CCockpitHockey::CreateObjImpl allocates it, objects 11 and
    // 12): the hockey game's result page, the score of each period (HockeyF
    // game/hockey/CCockpitHockeyResult.cpp). Return types and argument names
    // are guesses.
    class CCockpitHockeyResult : public CCockpitLayoutBase, public CCockpitResultTime {
        public:
            CCockpitHockeyResult(long nId, float fSortZ);
            virtual ~CCockpitHockeyResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu(); // returns 0 (the original sets r3)

            // The period scores of the two teams; nPeriod is the period
            // reached, nNum the number of periods.
            long SetCountScore(long *pScore0, long *pScore1, long nPeriod, long nNum);
            long SetTextNext(long nNum, long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);

            // Steps the page's in and out animation (m_nState).
            void StateControl();
            // Queues animation nAnim (in or out) on the score panes.
            void SetAnimResultWin(long nAnim);
    };
}

#endif /* !CCOCKPITHOCKEYRESULT_HPP_ */
