#ifndef MSW_CCHEARTBOMB_HPP_
#define MSW_CCHEARTBOMB_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    class CChrCore;

    namespace CC_CMN {
        namespace PEACH {

            // (DOL class; meaning guessed from the name) One of Peach's
            // heart bombs. 0x13C bytes (allocated so by every module).
            // INCOMPLETE: the constructor is in main.dol and only declared;
            // members are not mapped.
            class CcHeartBomb : public CChrObstacle {
                public:
                    // (field names and meanings guessed) The per-game tuning
                    // the modules pass to the constructor: five effect or
                    // sound ids, then four values; VolleyF's Peach draws
                    // m_afParam[0] anew for each bomb.
                    struct FIRE_PARAM {
                        long m_anId[5];
                        float m_afParam[4];
                    };

                    // A bomb of pOwner leaving rPos in the direction rDir.
                    CcHeartBomb(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos,
                                const bcs::FVEC3 &rDir, FIRE_PARAM &rParam);
                    virtual ~CcHeartBomb();
                    virtual void Main();

                protected:
                    unsigned char m_Unk94[0x13C - 0x94];
            };
        }
    }
}

#endif /* !MSW_CCHEARTBOMB_HPP_ */
