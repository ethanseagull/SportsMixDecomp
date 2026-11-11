#ifndef MSW_CCHRDRVPADHOCKEY_HPP_
#define MSW_CCHRDRVPADHOCKEY_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    // INCOMPLETE: the pad driver of the hockey game (HockeyF), size 0x1A8
    // (what CChrManHockey::CreateDriver_ allocates). Its base in the original
    // is the DOL's CChrDrvPad, which is not declared yet; only the
    // constructor is here, so the vtable must not be emitted from this
    // declaration.
    class CChrDrvPadHockey : public CChrDrv {
        public:
            CChrDrvPadHockey();

        protected:
            unsigned char m_Unk04[0x1A8 - 0x04];
    };
}

#endif /* !MSW_CCHRDRVPADHOCKEY_HPP_ */
