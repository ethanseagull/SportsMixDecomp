#ifndef MSW_CSTGEFFECTSLOT_HPP_
#define MSW_CSTGEFFECTSLOT_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CEffectCluster;
    class CStgMdl;

    // A fixed set of effect slots owned by a stage actor, the effect
    // counterpart of msw::CStgSeSlot. A DOL class, size 0xC with the vptr at
    // 0x08 (the two data members are declared before the first virtual
    // function), as msw::CStgAct's layout shows: this part at 0x00 (vptr at
    // 0x08), CStgSeSlot at 0x0C (vptr at 0x14).
    //
    // The order of the virtual functions is the order of the first part of
    // __vt__Q23msw22CStgGmk_Wave_Dodge_Act (DodgeF). All are DOL functions
    // except GetMdl, which msw::CStgAct supplies, and Eff_GetSlotNum, which
    // is inline (most users override it). Parameter names and return types
    // are guesses.
    class CStgEffectSlot {
        private:
            void *m_pSlot;   // 0x00, Eff_GetSlotNum() slots, made by Eff_Init (type not mapped)
            long m_nSlotNum; // 0x04

        public:
            virtual ~CStgEffectSlot();
            virtual void Eff_Init();
            // The model the effects attach to.
            virtual CStgMdl *GetMdl(long idx) = 0;
            // Inline: no slots by default (VolleyF game/volley/CStageNetVolley.cpp
            // has a weak copy, as do DodgeF and HockeyF).
            virtual long Eff_GetSlotNum() const { return 0; }
            virtual unsigned long Eff_CreateEffect(long slot, long eff, const bcs::FVEC3 *pPos, unsigned long flag);
            virtual unsigned long Eff_CreateAttach(long slot, long eff, long mdl, unsigned long node,
                                                   const bcs::FVEC3 *pOfs, int, int, int, unsigned long flag);
            virtual unsigned long Eff_CreateAttach(long slot, long eff, long mdl, const char *pNode,
                                                   const bcs::FVEC3 *pOfs, int, int, int, unsigned long flag);
            virtual unsigned long Eff_CreateAttachFadeIn(long slot, float fade, long eff, long mdl,
                                                         unsigned long node, const bcs::FVEC3 *pOfs, int, int, int,
                                                         unsigned long flag);
            virtual unsigned long Eff_GetHandle(long slot);
            // The effect of a slot (msw::CStgGmk_WaterPillar_Act::OnUpdate
            // in the DOL calls CEffectCluster::SetAlpha on it).
            virtual CEffectCluster *Eff_GetObject(long slot);
            virtual void Eff_Stop(long slot);
            virtual void Eff_Erase(long slot);
            virtual void Eff_StopAll();
            virtual void Eff_EraseAll();
            virtual void Eff_FadeIn(long slot, float fade);
            virtual void Eff_FadeOut(long slot, float fade);
            virtual bool Eff_IsAlive(long slot);
            virtual void Eff_DelayCreateAttach(long slot, float delay, long eff, long mdl, unsigned long node,
                                               const bcs::FVEC3 *pOfs, int, int, int, unsigned long flag);
            virtual void Eff_DelayCreateAttachFadeIn(long slot, float delay, float fade, long eff, long mdl,
                                                     unsigned long node, const bcs::FVEC3 *pOfs, int, int, int,
                                                     unsigned long flag);
            virtual void Eff_DelayStop(long slot, float delay);
            virtual void Eff_DelayErase(long slot, float delay);
            virtual void Eff_DelayFadeOut(long slot, float delay, float fade);
            virtual void Eff_Update(float);
    };
}

#endif /* !MSW_CSTGEFFECTSLOT_HPP_ */
