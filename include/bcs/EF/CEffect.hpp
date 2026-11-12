#ifndef BCS_EF_CEFFECT_HPP_
#define BCS_EF_CEFFECT_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    class CScnObj;

    namespace EF {

        // One effect of the effect system (DOL class). Only the members used
        // so far (TourF's Stage33 hangs the effects of a cluster on a
        // model). Parameter names and return types guessed.
        class CEffect {
            public:
                void SetParentMdl(const CScnObj *pParent, unsigned long nNode, int b);
                void SetAttachStatus(int b1, int b2, int b3);
                void SetLocalTranslate(const FVEC3 &vPos);

                // (name guessed; the DOL's fn_8001FA28) Shows the effect for
                // 1 and hides it otherwise: sets or clears bits 0x40 and
                // 0x80 of the word at 0xFC of the object at 0x44, and bit 2
                // of the word at 0x50 of each emitter. msw::cockpit::
                // CEffectSet::SetDispFlag calls it through a member pointer.
                void SetDispFlag(int bDisp);
                // (DOL; parameter name guessed) MenuF msw::SelectMenu::
                // CCourtChr calls it through a member pointer with 4 frames.
                void SetDelayTime(float fTime);
                // (name guessed; the DOL's fn_8001F120) Stores fRate at
                // 0x15C and passes it to SetRate of the animation objects at
                // 0x48-0x58. msw::CCockpitModelTelop::SendEvent calls it
                // through a member pointer.
                void SetAnmRate(float fRate);

                // (name guessed; the DOL's fn_80123490, a weak inline: its
                // address is taken by msw::CEffectCluster's fn_801233FC and
                // by msw::CCockpitModelTelop) Sets bit 0x100000 of the flags
                // at 0xA4 for 1, clears it otherwise.
                void Set2DFlag(int b2D)
                {
                    if (b2D == 1) {
                        m_nFlags |= 0x100000;
                    } else {
                        m_nFlags &= ~0x100000;
                    }
                }

                // (name from the weak copy in BasketF's
                // CChrCoreBasketNinja.cpp) The frames the effect has played.
                float GetCurrentFrame() { return m_fFrame - m_fStartFrame; }

            private:
                unsigned char m_Unk00[0xA4];
                unsigned long m_nFlags; // 0xA4 (bits used by SetAttachStatus too)
                unsigned char m_UnkA8[0x128 - 0xA8];
                // (names guessed) GetCurrentFrame returns the difference.
                float m_fFrame;         // 0x128
                float m_fStartFrame;    // 0x12C
        };
    }
}

#endif /* !BCS_EF_CEFFECT_HPP_ */
