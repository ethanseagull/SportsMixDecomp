#ifndef MSW_CMDLEFFECT_HPP_
#define MSW_CMDLEFFECT_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "revolution/gx/GXEnum.h"

namespace msw {
    namespace CC_CMN {

        // (DOL class; meaning guessed from the name) A task holding an
        // effect with a model, at a point. 0x54 bytes (VolleyF's
        // CChrCoreVolleyKinopio allocates it with that size).
        //
        // The base is bcs::CUnivTaskEx (the destructor tests `this` twice;
        // the vtable holds CUnivTask's GetFrameRateF; priority 0x16). Members read from DOL game/msw/CC_CMN/CMdlEffect.cpp;
        // parameter meanings guessed. The task deletes itself once every
        // animation of the effect's models has ended (looping ones never
        // end).
        class CMdlEffect : public bcs::CUnivTaskEx {
            public:
                // Effect nEff of the effect table at pos, turned by rot.
                // nFlag: FLAG_FOLLOW_FLOOR or 0. nArg goes to
                // CEffectCtrl::CreateAttachEffect as its eighth argument.
                CMdlEffect(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                           unsigned long nFlag, long nArg);
                virtual ~CMdlEffect();
                virtual void Main();

                // Sets a konstant colour of the effect's models.
                void SetMatTevKColorRGB(_GXTevKColorID id, bcs::BCS_Color color);

                enum {
                    // (name guessed) Bit 0 of m_nEffFlag: the effect moves
                    // with a flowing floor under it.
                    FLAG_FOLLOW_FLOOR = 1
                };

            private:
                bcs::FVEC3 m_Pos;        // 0x34
                bcs::FVEC3 m_Rot;        // 0x40
                unsigned long m_hEffect; // 0x4C, CEffectCtrl handle
                unsigned long m_nEffFlag; // 0x50
        };
    }
}

#endif /* !MSW_CMDLEFFECT_HPP_ */
