#ifndef CCOCKPITMODELTELOPHOLD_HPP_
#define CCOCKPITMODELTELOPHOLD_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // Size 0x84. Lives in the DOL. A telop drawn as an effect that stays on
    // screen until it is told to go. CCockpitLayoutBase is at 0,
    // CCockpitFrameOutBase at 0x60 (vptr at 0x68), own members from 0x6C.
    // The layout is read from the constructor; member names are guesses.
    // Set has no name in the DOL (fn_8019D6B8, its vtable slot).
    // m_nState (0x30) is the step: 1 start, 3 shown and waiting for the
    // frame-out, 4 and 5 leaving, 0 off.
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

            // Replaces effect nSlot (0 or 1; others are ignored) with a new
            // effect nEffectId, passing nUnk on to CreateEffect, and sets its
            // display flag and animation rate. Return type and argument names
            // are guesses.
            void SetEffect(long nSlot, long nEffectId, long nUnk, int bDisp, float fRate);

            // (no symbol; inline in SendEvent) Sets the animation rate of
            // the effects of slot 0.
            void SetAnmRate(float fRate);

            // (no symbol; inline in Main) Nonzero once the character
            // animation of the first effect that has one has ended or looped,
            // and when neither effect has one.
            int IsAnmEnd()
            {
                for (long i = 0; i < 2; i++) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect[i]);
                    if (pCluster != 0 && pCluster->GetMdlObjNum() > 0) {
                        bcs::CMdlObj *pMdlObj = pCluster->GetMdlObj(0);
                        if (pMdlObj != 0) {
                            bcs::CAnmObj *pAnmObj = pMdlObj->GetAnmObjChr();
                            if (pAnmObj != 0) {
                                return pAnmObj->IsLooped() || pAnmObj->IsEnd();
                            }
                        }
                    }
                }
                return 1;
            }

        protected:
            long m_nType;          // 0x6C, the constructor argument
            unsigned long m_hEffect[2]; // 0x70, effect handles, 0 when none
            unsigned long m_Unk78; // 0x78
            long m_nUnk7C;         // 0x7C, the constructor's last argument
            long m_nEffectId;      // 0x80, -1 until Set
    };
}

#endif /* !CCOCKPITMODELTELOPHOLD_HPP_ */
