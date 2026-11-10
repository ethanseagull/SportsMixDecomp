#ifndef CEFFECTCTRLDODGE_HPP_
#define CEFFECTCTRLDODGE_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the dodge map. Adds no data.
    class CEffectCtrlDodge : public CEffectCtrl {
        public:
            CEffectCtrlDodge(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlDodge();
            virtual void Main();
            virtual void Disp();
            // Forwards to the base; why Dodge overrides it is not known.
            virtual void CreateAttachClusterAuto(long id, const bcs::CScnObj *pObj, unsigned long flags);

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEFFECTCTRLDODGE_HPP_ */
