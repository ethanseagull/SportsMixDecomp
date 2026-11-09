#ifndef CCOCKPITBOSSLAYOUTBASE_HPP_
#define CCOCKPITBOSSLAYOUTBASE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x60: adds no data to CCockpitLayoutBase. Base of the boss game's
    // cockpit layouts (CCockpitBossCoin, CCockpitBossCoder). Abstract: Set
    // and Remove of CMenuLinkBase are still pure here.
    //
    // The cockpit is three parts (life gauge, boss gauge, coins), each slid
    // out or in, with or without waiting. A layout overrides the parts it
    // owns; the All* functions run the three of one kind. The virtuals are
    // in the order of the vtable. Every one returns 0 here; the return type
    // is a guess, no caller in this unit reads it.
    class CCockpitBossLayoutBase : public CCockpitLayoutBase {
        public:
            CCockpitBossLayoutBase(long nId);
            virtual ~CCockpitBossLayoutBase();

            virtual int AllOut();
            virtual int AllIn();
            virtual int AllOutWait();
            virtual int AllInWait();

            // Inline: game/boss/CCockpitBoss.cpp emits them after
            // CCockpitBoss::PartMoveCheck, their first caller.
            virtual int LifeGaugeOut() { return 0; }
            virtual int LifeGaugeIn() { return 0; }
            virtual int LifeGaugeOutWait() { return 0; }
            virtual int LifeGaugeInWait() { return 0; }
            virtual int BossGaugeOut() { return 0; }
            virtual int BossGaugeIn() { return 0; }
            virtual int BossGaugeOutWait() { return 0; }
            virtual int BossGaugeInWait() { return 0; }
            virtual int CoinOut() { return 0; }
            virtual int CoinIn() { return 0; }
            virtual int CoinOutWait() { return 0; }
            virtual int CoinInWait() { return 0; }
    };
}

#endif /* !CCOCKPITBOSSLAYOUTBASE_HPP_ */
