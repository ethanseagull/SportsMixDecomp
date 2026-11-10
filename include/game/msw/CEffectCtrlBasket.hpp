#ifndef CEFFECTCTRLBASKET_HPP_
#define CEFFECTCTRLBASKET_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the basket map. Adds no data.
    class CEffectCtrlBasket : public CEffectCtrl {
        public:
            CEffectCtrlBasket(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlBasket();
            virtual void Main();
            virtual void Disp();
            virtual int InitPatch(long, bcs::EF::CEffect *);

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEFFECTCTRLBASKET_HPP_ */
