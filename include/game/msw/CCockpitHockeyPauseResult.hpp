#ifndef CCOCKPITHOCKEYPAUSERESULT_HPP_
#define CCOCKPITHOCKEYPAUSERESULT_HPP_

#include "game/msw/CCockpitPauseLayoutBase.hpp"

namespace msw {

    // The hockey games' page of the pause score board: the score of each
    // period. HockeyF (unit game/hockey/CCockpitHockeyPauseResult). Size
    // 0x60 (CCockpitHockeyPause::ScoreBoard_In allocates it), no members
    // past CCockpitLayoutBase's. Return types and argument meanings are
    // guesses from the one caller.
    // Every method from the unit's mangled names is declared.
    class CCockpitHockeyPauseResult : public CCockpitPauseLayoutBase {
        public:
            CCockpitHockeyPauseResult(long nId, float fSortZ);
            virtual ~CCockpitHockeyPauseResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

            void StateControl();
            void SetAnimResultWin(long nAnim);

            // The period scores of the two teams (nNum periods each).
            long SetCountScore(long *pScore0, long *pScore1, long nNum);
            long SetTextNext(long nNum, long nPeriod);
            long SetCountNum(long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);
    };
}

#endif /* !CCOCKPITHOCKEYPAUSERESULT_HPP_ */
