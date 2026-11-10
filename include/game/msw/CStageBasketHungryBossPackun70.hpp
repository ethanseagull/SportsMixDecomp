#ifndef MSW_CSTAGEBASKETHUNGRYBOSSPACKUN70_HPP_
#define MSW_CSTAGEBASKETHUNGRYBOSSPACKUN70_HPP_

#include "game/msw/CStageBasketHungryBossPackun.hpp"

namespace msw {

    // Hungry Boss Packun stage 70. Size 0x23C (from CStageFactoryBasket::
    // Create): nothing beyond its base. It gives the boss its parameter
    // table and plays one stage sound.
    class CStageBasketHungryBossPackun70 : public CStageBasketHungryBossPackun {
        public:
            CStageBasketHungryBossPackun70(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun70();

            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);
    };
}

#endif /* !MSW_CSTAGEBASKETHUNGRYBOSSPACKUN70_HPP_ */
