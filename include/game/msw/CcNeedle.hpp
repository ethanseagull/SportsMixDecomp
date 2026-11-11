#ifndef MSW_CCNEEDLE_HPP_
#define MSW_CCNEEDLE_HPP_

#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace SABOTEN {

            // (DOL class; meaning guessed from the name) A needle the cactus
            // (Saboten) shoots. 0xA0 bytes. INCOMPLETE: the constructor and
            // Main are in main.dol and only declared; members are not mapped.
            // The three virtuals at the end are the per-game hooks the
            // modules' CcSabotenNeedle<Game> classes override (tuning values).
            class CcNeedle : public CChrObstacle {
                public:
                    CcNeedle(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float);
                    virtual ~CcNeedle();
                    virtual void Main();

                    virtual float GetCollHitEffectFrm_() const = 0;
                    virtual float GetCollHitRadius_() const = 0;
                    virtual int GetCororinDmgEffectID_() const = 0;

                protected:
                    unsigned char m_Unk94[0xA0 - 0x94];
            };
        }
    }
}

#endif /* !MSW_CCNEEDLE_HPP_ */
