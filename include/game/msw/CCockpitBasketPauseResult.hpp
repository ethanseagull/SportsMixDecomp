#ifndef CCOCKPITBASKETPAUSERESULT_HPP_
#define CCOCKPITBASKETPAUSERESULT_HPP_

#include "game/msw/CCockpitPauseLayoutBase.hpp"

namespace msw {

    // The basket games' page of the pause score board: the score of each
    // period. BasketF (unit game/basket/CCockpitBasketPauseResult). Size
    // 0x60 (CCockpitBasketPause::ScoreBoard_In allocates it), no members
    // past CCockpitLayoutBase's. Return types and argument meanings are
    // guesses from the one caller.
    class CCockpitBasketPauseResult : public CCockpitPauseLayoutBase {
        public:
            CCockpitBasketPauseResult(long nId, float fSortZ);
            virtual ~CCockpitBasketPauseResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

            // The period scores of the two teams (nNum periods each). The
            // four return 0 (SetTotalPoint -1 without the message data).
            long SetCountScore(long *pScore0, long *pScore1, long nNum);
            long SetTextNext(long nNum, long nPeriod);
            long SetCountNum(long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);

            // Steps the page's in and out animation (m_nState).
            void StateControl();
            // Queues animation nAnim (in or out) on the score panes.
            void SetAnimResultWin(long nAnim);
    };
}

#endif /* !CCOCKPITBASKETPAUSERESULT_HPP_ */
