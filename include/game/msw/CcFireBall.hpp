#ifndef MSW_CCFIREBALL_HPP_
#define MSW_CCFIREBALL_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace KOOPA {

            // (DOL class; meaning guessed from the name) Bowser's fireball.
            // 0xD8 bytes. INCOMPLETE: the constructor and Create are in
            // main.dol and only declared; members are not mapped.
            class CcFireBall : public CChrObstacle {
                public:
                    // (field names and meanings guessed) The per-game tuning
                    // the modules pass to Create: three effect ids, then
                    // eight values (HockeyF CChrCoreHockeyKoopa).
                    struct FIRE_PARAM {
                        long m_anEffectID[3];
                        float m_afParam[8];
                    };

                    CcFireBall(bcs::CTask *pParent);
                    virtual ~CcFireBall();
                    virtual void Main();

                    // Starts the fireball of pOwner from rPos towards rTarget.
                    void Create(CChrCore *pOwner, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget,
                                FIRE_PARAM &rParam);
                    // (meaning guessed) Not zero once the fireball is over.
                    int IsEnd() const;

                protected:
                    unsigned char m_Unk94[0xD8 - 0x94];
            };
        }
    }
}

#endif /* !MSW_CCFIREBALL_HPP_ */
