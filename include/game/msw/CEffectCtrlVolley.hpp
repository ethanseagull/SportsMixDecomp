#ifndef CEFFECTCTRLVOLLEY_HPP_
#define CEFFECTCTRLVOLLEY_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the volley map. Adds no data.
    class CEffectCtrlVolley : public CEffectCtrl {
        public:
            CEffectCtrlVolley(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlVolley();
            virtual void Main();
            virtual void Disp();

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEFFECTCTRLVOLLEY_HPP_ */
