#ifndef CCOCKPITBASKETBONUSTIME_HPP_
#define CCOCKPITBASKETBONUSTIME_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x70. The basket game's "bonus time" telop (layout
    // TE_bonus_time): it comes in, holds until it is told to go, plays a
    // second animation pair, holds again, then goes out and ends itself.
    // CCockpitLayoutBase is at 0, CCockpitFrameOutBase at 0x60 (vptr at
    // 0x68), own member at 0x6C.
    class CCockpitBasketBonusTime : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            // The second argument is not used.
            CCockpitBasketBonusTime(long nId, long nUnk, float fSortZ);
            virtual ~CCockpitBasketBonusTime();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

            // Steps the animations by m_nAnimState.
            void StateControl();

        private:
            long m_nAnimState; // 0x6C, ANIM_STATE_* in the .cpp
    };
}

#endif /* !CCOCKPITBASKETBONUSTIME_HPP_ */
