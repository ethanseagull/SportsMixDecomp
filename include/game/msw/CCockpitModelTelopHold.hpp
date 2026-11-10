#ifndef CCOCKPITMODELTELOPHOLD_HPP_
#define CCOCKPITMODELTELOPHOLD_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x84. Lives in the DOL. A telop drawn as an effect that stays on
    // screen until it is told to go. CCockpitLayoutBase is at 0,
    // CCockpitFrameOutBase at 0x60 (vptr at 0x68), own members from 0x6C.
    // The layout is read from the constructor; member names are guesses.
    // Set has no name in the DOL (probably fn_8019D6B8).
    class CCockpitModelTelopHold : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            // nType is stored at 0x6C, nUnk at 0x7C.
            CCockpitModelTelopHold(long nId, long nType, float fSortZ, long nUnk);
            virtual ~CCockpitModelTelopHold();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // Return type and argument names are guesses.
            void SetEffect(long nUnk, long nEffectId, long nUnk2, int bUnk, float fUnk);

        protected:
            long m_nType;          // 0x6C, the constructor argument
            unsigned long m_Unk70; // 0x70
            unsigned long m_Unk74; // 0x74
            unsigned long m_Unk78; // 0x78
            long m_nUnk7C;         // 0x7C, the constructor's last argument
            long m_nEffectId;      // 0x80, -1 until Set
    };
}

#endif /* !CCOCKPITMODELTELOPHOLD_HPP_ */
