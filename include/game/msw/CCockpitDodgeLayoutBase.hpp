#ifndef CCOCKPITDODGELAYOUTBASE_HPP_
#define CCOCKPITDODGELAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x60: adds no data to CCockpitLayoutBase. Base of the dodge game's
    // cockpit layouts (CCockpitDodgeCoin, CCockpitDodgeCoder). Abstract: Set
    // and Remove of CMenuLinkBase are still pure here.
    //
    // The dodge cockpit is three parts (gauge, set count, coins), each slid
    // out or in, with or without waiting. A layout overrides the parts it
    // owns; the All* functions run the three of one kind. The virtuals are
    // in the order of the vtable (39 slots, from DodgeF). Every part returns
    // 0 here; the return type is a guess, no caller reads it.
    class CCockpitDodgeLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitDodgeLayoutBase(long nId);
            virtual ~CCockpitDodgeLayoutBase();

            virtual int AllOut();
            virtual int AllIn();
            virtual int AllOutWait();
            virtual int AllInWait();

            // Inline: game/dodge/CCockpitDodge.cpp emits them.
            virtual int GaugeOut() { return 0; }
            virtual int GaugeIn() { return 0; }
            virtual int GaugeOutWait() { return 0; }
            virtual int GaugeInWait() { return 0; }
            virtual int SetCountOut() { return 0; }
            virtual int SetCountIn() { return 0; }
            virtual int SetCountOutWait() { return 0; }
            virtual int SetCountInWait() { return 0; }
            virtual int CoinOut() { return 0; }
            virtual int CoinIn() { return 0; }
            virtual int CoinOutWait() { return 0; }
            virtual int CoinInWait() { return 0; }
    };
}

#endif /* !CCOCKPITDODGELAYOUTBASE_HPP_ */
