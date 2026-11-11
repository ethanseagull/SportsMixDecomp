#ifndef MSW_CCFIREBALL_HPP_
#define MSW_CCFIREBALL_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace KOOPA {

            // (DOL class; meaning guessed from the name) A fireball Bowser
            // (Koopa) breathes. 0xD8 bytes. INCOMPLETE: the constructor and
            // Create are in main.dol and only declared; members are not
            // mapped (Create copies the FIRE_PARAM to 0xAC).
            class CcFireBall : public CChrObstacle {
                public:
                    // The tuning of one fireball (names guessed): three
                    // effect or resource ids, then eight values. BasketF's
                    // CChrCoreBasketKoopa passes ids 0x579, 0x57A and 0x57D.
                    struct FIRE_PARAM {
                        long m_anId[3];        // 0x00
                        float m_afValue[8];    // 0x0C
                    };

                    CcFireBall(bcs::CTask *pParent);
                    virtual ~CcFireBall();
                    virtual void Main();

                    // Starts the fireball for pOwner at rPos, flying to
                    // rTarget.
                    void Create(CChrCore *pOwner, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget,
                                FIRE_PARAM &rParam);

                protected:
                    unsigned char m_Unk94[0xD8 - 0x94];
            };
        }
    }
}

#endif /* !MSW_CCFIREBALL_HPP_ */
