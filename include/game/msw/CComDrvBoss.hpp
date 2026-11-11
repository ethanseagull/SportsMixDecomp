#ifndef MSW_CCOMDRVBOSS_HPP_
#define MSW_CCOMDRVBOSS_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    // The computer driver of the dodgeball game (DodgeF). Adds three
    // virtuals for the side-step roll; no members of its own were found.
    class CComDrvBoss : public CComDrv {
        public:
            CComDrvBoss(long nArg);
            virtual ~CComDrvBoss();

            virtual int CanbeBossRoll(float fRot);
            virtual int MethdBossRoll(float fRot);
            virtual int MethdBossRollTarget(const bcs::FVEC3 &pos);
    };
}

#endif /* !MSW_CCOMDRVBOSS_HPP_ */
