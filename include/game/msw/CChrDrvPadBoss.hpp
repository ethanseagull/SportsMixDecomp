#ifndef MSW_CCHRDRVPADBOSS_HPP_
#define MSW_CCHRDRVPADBOSS_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    // INCOMPLETE: the pad driver of the boss game (BossF), size 0xB0 (what
    // CChrManBoss::CreateDriver_ allocates). Its base in the original is the
    // DOL's CChrDrvPad, which is not declared yet; only the constructor is
    // here, so the vtable must not be emitted from this declaration.
    class CChrDrvPadBoss : public CChrDrv {
        public:
            CChrDrvPadBoss();

        protected:
            unsigned char m_Unk04[0xB0 - 0x04];
    };
}

#endif /* !MSW_CCHRDRVPADBOSS_HPP_ */
