#ifndef CEFFECTCTRLBOSS_HPP_
#define CEFFECTCTRLBOSS_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the boss game. Adds no data.
    class CEffectCtrlBoss : public CEffectCtrl {
        public:
            CEffectCtrlBoss(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlBoss();
            virtual void Main();
            virtual void Disp();

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEFFECTCTRLBOSS_HPP_ */
