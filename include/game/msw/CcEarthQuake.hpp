#ifndef MSW_CCEARTHQUAKE_HPP_
#define MSW_CCEARTHQUAKE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace DONKEY {

            // (DOL class; meaning guessed from the name) Donkey Kong's
            // shock wave. 0x9C bytes (HockeyF allocates it so). INCOMPLETE:
            // the constructor and Main are in main.dol and only declared;
            // members are not mapped. The two virtuals at the end are the
            // per-game tuning hooks the modules' CcDonkeyQuake<Game> classes
            // override.
            class CcEarthQuake : public CChrObstacle {
                public:
                    CcEarthQuake(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float);
                    virtual ~CcEarthQuake();
                    virtual void Main();

                    virtual float GetCollHitEffectFrm_() const = 0;
                    virtual float GetCollHitRadius_() const = 0;

                protected:
                    unsigned char m_Unk94[0x9C - 0x94];
            };
        }
    }
}

#endif /* !MSW_CCEARTHQUAKE_HPP_ */
