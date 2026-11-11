#ifndef MSW_CGOALOBJBASKET_BOSSPACKUN_HPP_
#define MSW_CGOALOBJBASKET_BOSSPACKUN_HPP_

#include "game/msw/CGoalObjBasket.hpp"

namespace msw {

    class CNpcBossPackun;

    // The goal of the Hungry Boss Packun stages (BasketF,
    // game/basket/CGoalObjBasket.cpp): the boss carries it, so its model and
    // ring come from the boss. Size 0x64AC (allocated so by
    // CStageBasketHungryBossPackun::CreateGoal). Its vtable
    // (__vt__Q23msw25CGoalObjBasket_BossPackun) ends in SetNpc, after
    // CGoalObjBasket's own virtual functions.
    class CGoalObjBasket_BossPackun : public CGoalObjBasket {
        public:
            CGoalObjBasket_BossPackun(bcs::CTask *pParent, long idx, CNpcBossPackun *pNpc);
            virtual ~CGoalObjBasket_BossPackun();

            virtual unsigned long GetMdlHandle() const;
            virtual long GetRingNodeIdx() const;
            virtual bcs::CScnObj *GetMdlObj(int idx);
            virtual const bcs::CScnObj *GetMdlObj(int idx) const;

            // The boss that carries this goal.
            virtual void SetNpc(CNpcBossPackun *pNpc) { m_pNpc = pNpc; }

        private:
            CNpcBossPackun *m_pNpc; // 0x64A8
    };
}

#endif /* !MSW_CGOALOBJBASKET_BOSSPACKUN_HPP_ */
