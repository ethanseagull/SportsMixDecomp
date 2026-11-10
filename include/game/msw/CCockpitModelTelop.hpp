#ifndef CCOCKPITMODELTELOP_HPP_
#define CCOCKPITMODELTELOP_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x74. Lives in the DOL. A telop drawn as an effect. Layout from
    // its constructor and SetEffect; member names are guesses. Its Set has
    // no name in the DOL (fn_8019CE14); each sport derives its own.
    class CCockpitModelTelop : public CCockpitLayoutBase {
        public:
            // nType is stored at 0x60.
            CCockpitModelTelop(long nId, long nType, float fSortZ);
            virtual ~CCockpitModelTelop();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Replaces effect nSlot (0 or 1; others are ignored) with a new
            // effect nEffectId. Return type guessed.
            void SetEffect(long nSlot, long nEffectId);

        protected:
            long m_nType;               // 0x60, the constructor argument
            unsigned long m_hEffect[2]; // 0x64, effect handles, 0 when none
            unsigned char m_Unk6C[0x74 - 0x6C];
    };
}

#endif /* !CCOCKPITMODELTELOP_HPP_ */
