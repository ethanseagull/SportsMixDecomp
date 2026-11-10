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
            void SetAnimResultWin(long nTeam);

            // The period scores of the two teams (nNum periods each).
            void SetCountScore(long *pScore0, long *pScore1, long nNum);
            // (guess) Another per-team, per-period table, filled up to
            // nPeriodNum.
            void SetCountPoint(long *pPoint0, long *pPoint1, long nPeriod, long nPeriodNum);
            // Empty in DodgeF (8 bytes).
            void SetTextNext(long nNum, long nPeriod);
            void SetCountNum(long nPeriod);
            void SetTotalPoint(long nTotal0, long nTotal1);
    };
}

#endif /* !CCOCKPITDODGEPAUSERESULT_HPP_ */
