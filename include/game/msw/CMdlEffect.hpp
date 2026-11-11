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
        // INCOMPLETE: only what VolleyF calls. The base class is a guess
        // (CGrdMdlEffect, its ground-bound relative, is a CUnivTask);
        // parameter meanings guessed.
        class CMdlEffect : public bcs::CUnivTask {
            public:
                // Effect nEff of the effect table at pos, turned by rot.
                CMdlEffect(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                           unsigned long, long);
                virtual ~CMdlEffect();
                virtual void Main();

                // Sets a konstant colour of the effect's models.
                void SetMatTevKColorRGB(_GXTevKColorID id, bcs::BCS_Color color);

            private:
                unsigned char m_Unk34[0x54 - 0x34];
        };
    }
}

#endif /* !MSW_CMDLEFFECT_HPP_ */
