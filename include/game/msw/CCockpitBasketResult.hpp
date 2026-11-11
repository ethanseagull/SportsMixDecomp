#ifndef CCOCKPITBASKETRESULT_HPP_
#define CCOCKPITBASKETRESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"
#include "game/msw/CCockpitResultTime.hpp"

namespace msw {

    // Size 0x6C (CCockpitBasket::CreateObjImpl allocates it): the basket
    // game's result page, the score of each period (BasketF
    // game/basket/CCockpitBasketResult; the same calls as
    // CCockpitBasketPauseResult). Return types and argument names are
    // guesses.
    class CCockpitBasketResult : public CCockpitLayoutBase, public CCockpitResultTime {
        public:
            CCockpitBasketResult(long nId, float fSortZ);
            virtual ~CCockpitBasketResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu();

            // The period scores of the two teams; nPeriod is the period
            // reached, nNum the number of periods.
            long SetCountScore(long *pScore0, long *pScore1, long nPeriod, long nNum);
            long SetTextNext(long nNum, long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);

        private:
            void StateControl();
            // nAnim is 4 (in) or 5 (out), for every score pane.
            void SetAnimResultWin(long nAnim);
    };
}

#endif /* !CCOCKPITBASKETRESULT_HPP_ */
