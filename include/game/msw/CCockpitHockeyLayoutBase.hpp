#ifndef CCOCKPITHOCKEYLAYOUTBASE_HPP_
#define CCOCKPITHOCKEYLAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x60: adds no data to CCockpitLayoutBase. Base of the hockey game's
    // cockpit layouts (CCockpitHockeyCoin, CCockpitHockeyCoder). Abstract: Set
    // and Remove of CMenuLinkBase are still pure here.
    //
    // The cockpit is four parts (score, coins, time, period), each slid out
    // or in, with or without waiting. A layout overrides the parts it owns;
    // the All* functions run the four of one kind. The virtuals are in the
    // order of the vtable. Every one returns 0 here; the return type is a
    // guess, no caller in this unit reads it.
    class CCockpitHockeyLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitHockeyLayoutBase(long nId);
            virtual ~CCockpitHockeyLayoutBase();

            virtual int AllOut();
            virtual int AllIn();
            virtual int AllOutWait();
            virtual int AllInWait();

            // Inline: game/hockey/CCockpitHockey.cpp emits them.
            virtual int ScoreOut() { return 0; }
            virtual int ScoreIn() { return 0; }
            virtual int ScoreOutWait() { return 0; }
            virtual int ScoreInWait() { return 0; }
            virtual int CoinOut() { return 0; }
            virtual int CoinIn() { return 0; }
            virtual int CoinOutWait() { return 0; }
            virtual int CoinInWait() { return 0; }
            virtual int TimeOut() { return 0; }
            virtual int TimeIn() { return 0; }
            virtual int TimeOutWait() { return 0; }
            virtual int TimeInWait() { return 0; }
            virtual int PeriodOut() { return 0; }
            virtual int PeriodIn() { return 0; }
            virtual int PeriodOutWait() { return 0; }
            virtual int PeriodInWait() { return 0; }
    };
}

#endif /* !CCOCKPITHOCKEYLAYOUTBASE_HPP_ */
