#ifndef CCOCKPITVOLLEYRESULT_HPP_
#define CCOCKPITVOLLEYRESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    // Size 0x6C (CCockpitVolley::CreateObjImpl allocates it, objects 9 and
    // 10): the volley game's result page, the score, points and perfect
    // marks of each set (VolleyF game/volley/CCockpitVolleyResult.cpp).
    // Return types and argument names are guesses.
    class CCockpitVolleyResult : public CCockpitLayoutBase, public CCockpitResultTime {
        public:
            CCockpitVolleyResult(long nId, float fSortZ);
            virtual ~CCockpitVolleyResult();
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
            // The perfect marks of each period of the two teams; nNum the
            // number of periods.
            long SetPerfectIcon(long *pPerfect0, long *pPerfect1, long nNum);
            long SetTextNext(long nNum, long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);

            // Steps the page's in and out animation (m_nState).
            void StateControl();
            // Queues animation nAnim (in or out) on the score panes.
            void SetAnimResultWin(long nAnim);
    };
}

#endif /* !CCOCKPITVOLLEYRESULT_HPP_ */
