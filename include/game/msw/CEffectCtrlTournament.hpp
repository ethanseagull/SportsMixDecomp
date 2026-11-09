#ifndef CEFFECTCTRLTOURNAMENT_HPP_
#define CEFFECTCTRLTOURNAMENT_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the tournament map. Adds no data.
    class CEffectCtrlTournament : public CEffectCtrl {
        public:
            CEffectCtrlTournament(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                InitializeDataTable();
            }

            virtual ~CEffectCtrlTournament();
            virtual void Main();
            virtual void Disp();
            virtual int InitPatch(long, bcs::EF::CEffect *);

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEFFECTCTRLTOURNAMENT_HPP_ */
