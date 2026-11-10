#ifndef CCOCKPITDODGEPAUSERESULT_HPP_
#define CCOCKPITDODGEPAUSERESULT_HPP_

#include "game/msw/CCockpitPauseLayoutBase.hpp"

namespace msw {

    // The dodge games' page of the pause score board: the score of each
    // period. DodgeF (unit game/dodge/CCockpitDodgePauseResult). Size
    // 0x60 (CCockpitDodgePause::ScoreBoard_In allocates it), no members
    // past CCockpitLayoutBase's. Return types and argument meanings are
    // guesses from the one caller.
    // Every method from the unit's mangled names is declared.
    class CCockpitDodgePauseResult : public CCockpitPauseLayoutBase {
        public:
            CCockpitDodgePauseResult(long nId, float fSortZ);
            virtual ~CCockpitDodgePauseResult();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

            void StateControl();
            void SetAnimResultWin(long nAnim);

            // The period scores of the two teams (nNum periods each).
            long SetCountScore(long *pScore0, long *pScore1, long nNum);
            // (guess) Another per-team, per-period table, filled up to
            // nPeriodNum.
            long SetCountPoint(long *pPoint0, long *pPoint1, long nPeriod, long nPeriodNum);
            // Empty in DodgeF (8 bytes).
            long SetTextNext(long nNum, long nPeriod);
            long SetCountNum(long nPeriod);
            long SetTotalPoint(long nTotal0, long nTotal1);
    };
}

#endif /* !CCOCKPITDODGEPAUSERESULT_HPP_ */
