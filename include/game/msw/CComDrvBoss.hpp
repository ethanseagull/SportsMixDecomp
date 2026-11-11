#ifndef MSW_CCOMDRVBOSS_HPP_
#define MSW_CCOMDRVBOSS_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    // The computer driver of the dodgeball game (DodgeF). Adds three
    // virtuals for the side-step roll; no members of its own were found.
    class CComDrvBoss : public CComDrv {
        public:
            CComDrvBoss(long nArg);
            // The constructor BossF has (__ct__Q23msw11CComDrvBossFv, called
            // by CChrManBoss::CreateDriver_). It works out CComDrv's argument
            // itself (from CalcWorkBaseMaxSize, at least 0x2C); the (long)
            // one above was ported from DodgeF and is not in BossF.
            CComDrvBoss();
            virtual ~CComDrvBoss();

            virtual int CanbeBossRoll(float fRot);
            virtual int MethdBossRoll(float fRot);
            virtual int MethdBossRollTarget(const bcs::FVEC3 &pos);

        protected:
            // Not mapped. CChrManBoss::CreateDriver_ allocates 0x1F8 bytes.
            unsigned char m_Unk170[0x1F8 - 0x170];
    };
}

#endif /* !MSW_CCOMDRVBOSS_HPP_ */
