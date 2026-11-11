#ifndef COCKPIT_RESULTUTIL_HPP_
#define COCKPIT_RESULTUTIL_HPP_

namespace msw {
    namespace cockpit {

        // Free helpers of the result and score board displays. Live in the
        // DOL (0x80265D30 onwards); the file name is a guess.

        // Which form of the "points" label goes with nScore (0 or 1, an
        // index into a two-entry label table). What the other two
        // arguments select is not known; the score boards pass 0 and 1.
        long GetTotalScoreUnitIndex(long nScore, long nArg0, long nArg1);
        // Added for BasketF game/basket/CCockpitBasket.cpp: the title of the
        // result screen and of the score board, written to pBuf (nLen
        // characters); both return pBuf (guess).
        const wchar_t *GetResultTitleString(wchar_t *pBuf, long nLen);
        const wchar_t *GetScoreBoardTitleString(wchar_t *pBuf, long nLen);
    }
}

#endif /* !COCKPIT_RESULTUTIL_HPP_ */
