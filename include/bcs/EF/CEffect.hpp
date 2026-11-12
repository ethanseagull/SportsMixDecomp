#ifndef BCS_EF_CEFFECT_HPP_
#define BCS_EF_CEFFECT_HPP_

#include "bcs/FVEC3.hpp"
#include "revolution/gx/GXStruct.h"

namespace bcs {
    class CScnObj;
    class CMdlObj;

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
                // (DOL; added for BossF game/boss/GameBoss.cpp, which calls it
                // through a member pointer with 1; meaning guessed from the
                // name)
                void SetFrameRateCalcDisable(int bDisable);

                // (names guessed from the msw::CEffectCluster functions that
                // call them through member pointers; DOL fn_8001E6D0,
                // fn_8001E6E8, fn_8001E71C, fn_8001E744, fn_8001F3C0,
                // fn_8001FB24, fn_8001FBB8, fn_8001FC68, fn_8001F1F8,
                // fn_8001F534, fn_8001F6CC, fn_8001F714, fn_8001F8AC)
                // Fade times in frames (stored at 0xB4 / 0xBC).
                void SetFadeinTime(float fFrame);
                void SetFadeoutTime(float fFrame);
                // Scale (0xF0) and rotation in radians (0xFC); both set flag
                // 0x20 of the word at 0xA4.
                void SetScale(const FVEC3 &vScale);
                void SetRotateRad(const FVEC3 &vRot);
                // Stores fAlpha * 255 at 0x1B4 and sets flag 0x200000.
                void SetAlpha(float fAlpha);
                // Passes fOffset to every emitter.
                void SetZSortOffset(float fOffset);
                // Stores the scale at 0x148 and passes it to every emitter.
                void SetParticleScale(const FVEC3 &vScale);
                // Sets (bOn == 1) or clears flag 0x40000 and stores fSpeed
                // at 0x168.
                void SetSpeedOverride(int bOn, float fSpeed);
                // Nonzero if one of the emitters' resources is named pName.
                int HasEmitter(const char *pName);
                // Colours passed to every emitter (meanings guessed).
                void SetPrimaryColor(_GXColor color);
                void SetSecondaryColor(_GXColor color);
                void SetPointLightColor(_GXColor color0, _GXColor color1);
                // SetPointLightColor with a pair from a table of presets.
                void SetPointLightColorPreset(long nPreset);

                // (name guessed) The model of the effect, null for a
                // particle effect.
                CMdlObj *GetMdlObj() const { return m_pMdlObj; }
                // (name guessed) 1 for an effect drawn as a model.
                int GetType() const { return m_nType; }

                // (name guessed; the DOL's fn_80123490, a weak inline: its
                // address is taken by msw::CEffectCluster's fn_801233FC and
                // by msw::CCockpitModelTelop) Sets bit 0x100000 of the flags
                // at 0xA4 for 1, clears it otherwise.
                void Set2DFlag(int b2D)
                {
                    m_nFlags = (b2D == 1) ? (m_nFlags | 0x100000) : (m_nFlags & ~0x100000);
                }

                // (name from the weak copy in BasketF's
                // CChrCoreBasketNinja.cpp) The frames the effect has played.
                float GetCurrentFrame() { return m_fFrame - m_fStartFrame; }

            private:
                unsigned char m_Unk00[0x44];
                CMdlObj *m_pMdlObj;     // 0x44
                unsigned char m_Unk48[0x9C - 0x48];
                unsigned char m_nType;  // 0x9C (name guessed)
                unsigned char m_Unk9D[0xA4 - 0x9D];
                unsigned long m_nFlags; // 0xA4 (bits used by SetAttachStatus too)
                unsigned char m_UnkA8[0x128 - 0xA8];
                // (names guessed) GetCurrentFrame returns the difference.
                float m_fFrame;         // 0x128
                float m_fStartFrame;    // 0x12C
        };
    }
}

#endif /* !BCS_EF_CEFFECT_HPP_ */
